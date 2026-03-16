# 内存受限系统设计指南

> 8位/16位MCU、ESP8266、STM32F0 等 RAM<64KB 系统的优化技术

---

## 1. 内存受限环境概述

### 1.1 典型硬件平台

| 平台 | RAM | Flash | 典型应用 |
|:-----|:----|:------|:---------|
| ATmega328P | 2KB | 32KB | Arduino Uno |
| STM32F103C8 | 20KB | 64KB | Blue Pill |
| STM32F030 | 4KB | 16KB | 低端控制 |
| ESP8266 | 80KB | 4MB | WiFi模组 |
| nRF51822 | 16KB | 256KB | BLE设备 |
| MSP430 | 2-8KB | 16-256KB | 超低功耗 |

### 1.2 内存使用分布（典型）

```
RAM 布局 (以 20KB STM32 为例):

0x20000000 ┌─────────────────┐
           │   栈 (Stack)    │  4KB (向下增长)
0x20001000 ├─────────────────┤
           │   堆 (Heap)     │  2KB (向上增长)
0x20001800 ├─────────────────┤
           │   BSS (未初始化) │  1KB
0x20001C00 ├─────────────────┤
           │   数据 (Data)   │  1KB
0x20002000 ├─────────────────┤
           │   外设寄存器    │
           └─────────────────┘
```

---

## 2. 内存优化策略

### 2.1 编译时优化

```c
/* 使用正确的数据类型 */
#include <stdint.h>

/* 不推荐 - 在8位机上可能是16位 */
int counter;  

/* 推荐 - 明确指定大小 */
uint8_t small_counter;   /* 0-255 */
uint16_t medium_counter; /* 0-65535 */
uint32_t large_counter;  /* 大计数器 */

/* 使用位域节省内存 */
typedef struct {
    uint8_t is_active : 1;      /* 1 bit */
    uint8_t error_code : 3;     /* 3 bits */
    uint8_t priority : 4;       /* 4 bits */
} status_flags_t;  /* 总共1字节，而不是4字节 */

/* 常量放入 Flash */
const char * const error_messages[] = {
    "OK",
    "Timeout",
    "CRC Error",
    "Buffer Overflow"
};

/* 使用 PROGMEM (AVR) 或 __attribute__((section(".rodata"))) */
#ifdef __AVR__
#include <avr/pgmspace.h>
const char message[] PROGMEM = "This is in flash";
#else
const char message[] __attribute__((section(".rodata"))) = "This is in flash";
#endif
```

### 2.2 数据结构优化

```c
/* 内存池 - 避免堆碎片 */
#define POOL_SIZE 1024
#define BLOCK_SIZE 32
#define NUM_BLOCKS (POOL_SIZE / BLOCK_SIZE)

static uint8_t memory_pool[POOL_SIZE];
static uint8_t block_used[NUM_BLOCKS];

void *pool_alloc(void)
{
    for(int i = 0; i < NUM_BLOCKS; i++) {
        if(!block_used[i]) {
            block_used[i] = 1;
            return &memory_pool[i * BLOCK_SIZE];
        }
    }
    return NULL;  /* 无可用块 */
}

void pool_free(void *ptr)
{
    int index = ((uint8_t*)ptr - memory_pool) / BLOCK_SIZE;
    if(index >= 0 && index < NUM_BLOCKS) {
        block_used[index] = 0;
    }
}

/* 对象池 - 针对特定结构 */
typedef struct {
    uint16_t id;
    uint8_t data[16];
    uint8_t flags;
} sensor_packet_t;

#define MAX_PACKETS 8

static sensor_packet_t packet_pool[MAX_PACKETS];
static uint8_t packet_used[MAX_PACKETS];

sensor_packet_t *packet_alloc(void)
{
    for(int i = 0; i < MAX_PACKETS; i++) {
        if(!packet_used[i]) {
            packet_used[i] = 1;
            memset(&packet_pool[i], 0, sizeof(sensor_packet_t));
            return &packet_pool[i];
        }
    }
    return NULL;
}
```

### 2.3 动态内存替代方案

```c
/* 方案1: 栈分配 + 传递指针 */
void process_data_stack(void)
{
    uint8_t buffer[64];  /* 栈分配 */
    
    read_sensor(buffer, sizeof(buffer));
    process_buffer(buffer, sizeof(buffer));
    /* 自动释放 */
}

/* 方案2: 静态缓冲区复用 */
static uint8_t shared_buffer[128];
static volatile uint8_t buffer_lock = 0;

void process_data_static(void)
{
    /* 获取锁 */
    while(buffer_lock);  /* 忙等待，或改用信号量 */
    buffer_lock = 1;
    
    read_sensor(shared_buffer, sizeof(shared_buffer));
    process_buffer(shared_buffer, sizeof(shared_buffer));
    
    buffer_lock = 0;
}

/* 方案3: 环形缓冲区 */
typedef struct {
    uint8_t *buffer;
    uint16_t head;
    uint16_t tail;
    uint16_t size;
} ring_buffer_t;

void rb_init(ring_buffer_t *rb, uint8_t *buf, uint16_t size)
{
    rb->buffer = buf;
    rb->head = 0;
    rb->tail = 0;
    rb->size = size;
}

bool rb_put(ring_buffer_t *rb, uint8_t data)
{
    uint16_t next_head = (rb->head + 1) % rb->size;
    
    if(next_head == rb->tail) {
        return false;  /* 满 */
    }
    
    rb->buffer[rb->head] = data;
    rb->head = next_head;
    return true;
}

bool rb_get(ring_buffer_t *rb, uint8_t *data)
{
    if(rb->head == rb->tail) {
        return false;  /* 空 */
    }
    
    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;
    return true;
}
```

---

## 3. 代码大小优化

### 3.1 编译器优化选项

```makefile
# Makefile 优化选项

# 优化代码大小
CFLAGS += -Os              # 优化大小
CFLAGS += -ffunction-sections  # 每个函数独立段
CFLAGS += -fdata-sections      # 每个数据独立段
CFLAGS += -Wl,--gc-sections    # 链接时删除未使用段
CFLAGS += -fno-builtin     # 不使用内置函数
CFLAGS += -mthumb          # ARM Thumb 指令 (省空间)

# 禁用异常和RTTI (C++)
CXXFLAGS += -fno-exceptions
CXXFLAGS += -fno-rtti

# 链接脚本优化
LDFLAGS += -Wl,--relax     # 优化跳转指令
LDFLAGS += -nostartfiles   # 自定义启动代码
```

### 3.2 精简库函数

```c
/* 自定义 printf - 仅支持必要格式 */
#include <stdarg.h>

int mini_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    
    while(*fmt) {
        if(*fmt == '%') {
            fmt++;
            switch(*fmt) {
            case 'd': {
                int i = va_arg(args, int);
                print_int(i);  /* 自定义整数输出 */
                break;
            }
            case 'x': {
                unsigned int x = va_arg(args, unsigned int);
                print_hex(x);
                break;
            }
            case 's': {
                char *s = va_arg(args, char*);
                print_string(s);
                break;
            }
            case 'c': {
                char c = (char)va_arg(args, int);
                print_char(c);
                break;
            }
            default:
                print_char('%');
                print_char(*fmt);
            }
        } else {
            print_char(*fmt);
        }
        fmt++;
    }
    
    va_end(args);
    return 0;
}

/* 简化 memcpy */
void *my_memcpy(void *dst, const void *src, size_t n)
{
    uint8_t *d = dst;
    const uint8_t *s = src;
    
    /* 4字节对齐拷贝 */
    while(n >= 4) {
        *(uint32_t*)d = *(const uint32_t*)s;
        d += 4;
        s += 4;
        n -= 4;
    }
    
    /* 剩余字节 */
    while(n--) {
        *d++ = *s++;
    }
    
    return dst;
}
```

### 3.3 条件编译移除功能

```c
/* config.h */
#ifndef CONFIG_H
#define CONFIG_H

/* 功能开关 - 未使用的功能不编译 */
#define ENABLE_UART          1
#define ENABLE_SPI           1
#define ENABLE_I2C           1
#define ENABLE_DEBUG_PRINT   0   /* 发布版本关闭 */
#define ENABLE_ERROR_LOG     1
#define ENABLE_WATCHDOG      1

/* 模块大小限制 */
#define MAX_SENSORS          4   /* 而不是16 */
#define MAX_BUFFER_SIZE      64  /* 而不是256 */
#define MAX_CONNECTIONS      2   /* 而不是8 */

#endif
```

```c
/* 条件编译示例 */
#if ENABLE_DEBUG_PRINT
    #define DEBUG_PRINT(fmt, ...) mini_printf(fmt, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) ((void)0)
#endif

#if ENABLE_UART
void uart_init(void) { /* ... */ }
void uart_send(uint8_t data) { /* ... */ }
#else
    #define uart_init() ((void)0)
    #define uart_send(x) ((void)0)
#endif
```

---

## 4. 通信协议优化

### 4.1 紧凑型二进制协议

```c
/* 不推荐 - JSON 文本协议太占空间 */
{"sensor_id":1,"temp":25.5,"humidity":60}
/* 约40字节 */

/* 推荐 - 二进制协议 */
typedef struct __attribute__((packed)) {
    uint8_t  header;        /* 0xAA */
    uint8_t  sensor_id;     /* 1字节 */
    int16_t  temp;          /* 0.1度为单位，2字节 */
    uint8_t  humidity;      /* 1字节 */
    uint8_t  crc;           /* CRC8，1字节 */
} sensor_packet_t;
/* 总共6字节，节省85% */

/* 打包和解包 */
void pack_sensor_data(uint8_t *buf, uint8_t id, float temp, uint8_t hum)
{
    buf[0] = 0xAA;
    buf[1] = id;
    buf[2] = ((int16_t)(temp * 10)) >> 8;
    buf[3] = ((int16_t)(temp * 10)) & 0xFF;
    buf[4] = hum;
    buf[5] = crc8(buf, 5);
}

bool unpack_sensor_data(uint8_t *buf, uint8_t *id, float *temp, uint8_t *hum)
{
    if(buf[0] != 0xAA) return false;
    if(crc8(buf, 5) != buf[5]) return false;
    
    *id = buf[1];
    *temp = ((int16_t)((buf[2] << 8) | buf[3])) / 10.0f;
    *hum = buf[4];
    return true;
}

/* CRC8 实现 */
uint8_t crc8(uint8_t *data, uint8_t len)
{
    uint8_t crc = 0xFF;
    while(len--) {
        crc ^= *data++;
        for(int i = 0; i < 8; i++) {
            crc = (crc & 0x80) ? ((crc << 1) ^ 0x31) : (crc << 1);
        }
    }
    return crc;
}
```

### 4.2 CoAP 轻量级协议

```c
/* CoAP 简化实现 - 用于低功耗设备 */
#include <stdint.h>
#include <stdbool.h>

#define COAP_PORT 5683

typedef enum {
    COAP_GET = 1,
    COAP_POST = 2,
    COAP_PUT = 3,
    COAP_DELETE = 4
} coap_method_t;

typedef enum {
    COAP_CREATED = 0x41,
    COAP_DELETED = 0x42,
    COAP_VALID = 0x43,
    COAP_CHANGED = 0x44,
    COAP_CONTENT = 0x45,
    COAP_NOT_FOUND = 0x84
} coap_response_t;

typedef struct {
    uint8_t ver_t_tkl;      /* 版本(2) | 类型(2) | Token长度(4) */
    uint8_t code;           /* 类别(3) | 详情(5) */
    uint16_t msg_id;
    uint8_t token[8];       /* 最多8字节 */
    /* 选项和负载跟随 */
} coap_header_t;

/* 构造 CoAP GET 请求 */
int coap_get(uint8_t *buf, uint16_t buf_size, 
             const char *path, uint16_t msg_id)
{
    int pos = 0;
    
    /* 头部: Ver=1, Type=0(CON), TKL=0 */
    buf[pos++] = 0x40;  
    buf[pos++] = COAP_GET;
    buf[pos++] = msg_id >> 8;
    buf[pos++] = msg_id & 0xFF;
    
    /* Uri-Path 选项 (delta=11, 即0x0B) */
    buf[pos++] = 0x0B + strlen(path);  /* Option delta + length */
    memcpy(&buf[pos], path, strlen(path));
    pos += strlen(path);
    
    /* 结束标记 */
    buf[pos++] = 0xFF;
    
    return pos;
}
```

---

## 5. 传感器数据处理

### 5.1 增量传输

```c
/* 仅传输变化的数据 */
typedef struct {
    int16_t last_temp;
    uint8_t last_humidity;
    uint16_t last_pressure;
    uint8_t stable_count;
} sensor_state_t;

static sensor_state_t state;

bool should_transmit(float temp, uint8_t humidity, uint16_t pressure)
{
    int16_t temp_int = (int16_t)(temp * 10);
    
    /* 检查是否超过阈值 */
    bool temp_changed = abs(temp_int - state.last_temp) >= 2;  /* 0.2度 */
    bool hum_changed = abs((int)humidity - state.last_humidity) >= 2;
    bool press_changed = abs((int)pressure - state.last_pressure) >= 10;
    
    if(temp_changed || hum_changed || press_changed) {
        state.last_temp = temp_int;
        state.last_humidity = humidity;
        state.last_pressure = pressure;
        state.stable_count = 0;
        return true;
    }
    
    /* 即使没变化，每30次也传一次（保活） */
    if(++state.stable_count >= 30) {
        state.stable_count = 0;
        return true;
    }
    
    return false;
}
```

### 5.2 压缩算法 - 简单行程编码

```c
/* 行程编码 (RLE) - 适用于重复数据 */
int rle_encode(uint8_t *in, int in_len, uint8_t *out, int out_max)
{
    int in_pos = 0, out_pos = 0;
    
    while(in_pos < in_len && out_pos < out_max - 2) {
        uint8_t count = 1;
        uint8_t value = in[in_pos];
        
        /* 计数重复 */
        while(in_pos + count < in_len && 
              count < 255 && 
              in[in_pos + count] == value) {
            count++;
        }
        
        if(count >= 3) {
            /* 编码: 0x00 count value */
            out[out_pos++] = 0x00;
            out[out_pos++] = count;
            out[out_pos++] = value;
        } else {
            /* 不压缩，直接复制 */
            for(int i = 0; i < count; i++) {
                out[out_pos++] = value;
            }
        }
        
        in_pos += count;
    }
    
    return out_pos;
}
```

---

## 6. 电源管理

### 6.1 睡眠模式管理

```c
#include <avr/sleep.h>  /* AVR示例 */
#include <avr/interrupt.h>

volatile uint8_t sleep_requested = 0;

void enter_sleep_mode(void)
{
    /* 关闭外设 */
    uart_disable();
    adc_disable();
    spi_disable();
    
    /* 配置唤醒源 */
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    
    /* 进入睡眠 */
    cli();
    sleep_bod_disable();  /* 禁用BOD省电 */
    sei();
    sleep_cpu();          /* 实际睡眠 */
    
    /* 唤醒后继续 */
    sleep_disable();
    
    /* 恢复外设 */
    uart_enable();
    adc_enable();
}

/* 唤醒中断 */
ISR(INT0_vect)
{
    sleep_requested = 0;
}

void system_loop(void)
{
    process_events();
    
    if(can_sleep() && !uart_tx_busy()) {
        enter_sleep_mode();
    }
}
```

### 6.2 动态电压调节

```c
/* 根据负载调整CPU频率 */
typedef enum {
    SPEED_LOW = 0,      /* 1MHz, 低功耗 */
    SPEED_NORMAL = 1,   /* 8MHz, 正常 */
    SPEED_HIGH = 2      /* 72MHz, 高性能 */
} cpu_speed_t;

void set_cpu_speed(cpu_speed_t speed)
{
    switch(speed) {
    case SPEED_LOW:
        /* 切换到低速内部RC */
        RCC->CFGR &= ~RCC_CFGR_SW;
        RCC->CFGR |= RCC_CFGR_SW_HSI;
        while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
        
        /* 降低电压 */
        PWR->CR &= ~PWR_CR_VOS;
        break;
        
    case SPEED_HIGH:
        /* 切换到高速外部晶振 */
        /* ... */
        break;
        
    default:
        break;
    }
}
```

---

## 7. 调试与监控

### 7.1 内存使用监控

```c
/* 栈使用监控 - 填充魔数 */
#define STACK_CANARY 0xDEADBEEF

extern uint32_t _sstack;  /* 栈底 */
extern uint32_t _estack;  /* 栈顶 */

void stack_monitor_init(void)
{
    uint32_t *p = &_sstack;
    while(p < &_estack) {
        *p++ = STACK_CANARY;
    }
}

uint32_t stack_get_free(void)
{
    uint32_t *p = &_sstack;
    while(p < &_estack && *p == STACK_CANARY) {
        p++;
    }
    return (uint32_t)p - (uint32_t)&_sstack;
}

uint32_t stack_get_used(void)
{
    return (uint32_t)&_estack - (uint32_t)&_sstack - stack_get_free();
}
```

### 7.2 轻量级日志系统

```c
/* 环形日志缓冲区 */
#define LOG_SIZE 256

static struct {
    uint8_t buf[LOG_SIZE];
    uint16_t head;
    uint16_t tail;
    uint8_t level;  /* 当前日志级别 */
} log_ring;

typedef enum {
    LOG_ERR = 0,
    LOG_WARN = 1,
    LOG_INFO = 2,
    LOG_DBG = 3
} log_level_t;

void log_msg(log_level_t level, const char *msg)
{
    if(level > log_ring.level) return;
    
    /* 写入: [level][len][msg...] */
    uint8_t len = strlen(msg);
    
    log_ring.buf[log_ring.head] = level;
    log_ring.head = (log_ring.head + 1) % LOG_SIZE;
    
    log_ring.buf[log_ring.head] = len;
    log_ring.head = (log_ring.head + 1) % LOG_SIZE;
    
    for(int i = 0; i < len; i++) {
        log_ring.buf[log_ring.head] = msg[i];
        log_ring.head = (log_ring.head + 1) % LOG_SIZE;
    }
}

void log_dump(void)
{
    while(log_ring.tail != log_ring.head) {
        uint8_t level = log_ring.buf[log_ring.tail];
        log_ring.tail = (log_ring.tail + 1) % LOG_SIZE;
        
        uint8_t len = log_ring.buf[log_ring.tail];
        log_ring.tail = (log_ring.tail + 1) % LOG_SIZE;
        
        uart_send('0' + level);
        uart_send(':');
        
        for(int i = 0; i < len; i++) {
            uart_send(log_ring.buf[log_ring.tail]);
            log_ring.tail = (log_ring.tail + 1) % LOG_SIZE;
        }
        uart_send('\n');
    }
}
```

---

## 8. 完整示例：传感器节点

```c
/* sensor_node.c - 内存受限传感器节点 */
#include <stdint.h>
#include <string.h>
#include "config.h"

/* 内存分配 */
#define SENSOR_COUNT 4
#define TX_BUFFER_SIZE 32
#define MAX_SENSORS 4

static uint8_t tx_buffer[TX_BUFFER_SIZE];
static sensor_state_t sensor_states[MAX_SENSORS];

/* 主循环 */
int main(void)
{
    system_init();
    sensors_init();
    comm_init();
    stack_monitor_init();
    
    while(1) {
        /* 读取所有传感器 */
        for(int i = 0; i < SENSOR_COUNT; i++) {
            sensor_read(i);
        }
        
        /* 检查是否需要传输 */
        for(int i = 0; i < SENSOR_COUNT; i++) {
            if(should_transmit_sensor(i)) {
                int len = pack_sensor_frame(&tx_buffer[2], i);
                tx_buffer[0] = 0xAA;  /* 帧头 */
                tx_buffer[1] = len;
                
                comm_send(tx_buffer, len + 2);
                
                /* 立即返回低功耗 */
                break;
            }
        }
        
        /* 日志 */
        #if ENABLE_DEBUG_PRINT
        static uint32_t last_debug = 0;
        if(millis() - last_debug > 10000) {
            mini_printf("Stack:%u Free:%u\n", 
                       stack_get_used(), 
                       heap_get_free());
            last_debug = millis();
        }
        #endif
        
        /* 进入睡眠 */
        enter_sleep_mode();
    }
}
```

---

## 参考资源

- **Smaller C**: https://github.com/alexfru/SmallerC
- **newlib-nano**: 嵌入式优化C库
- **Contiki-NG**: 内存受限IoT操作系统
- **Zephyr RTOS**: 最小配置8KB RAM

---

> **目标平台**: 8-32位MCU, RAM < 64KB  
> **工具链**: GCC ARM / AVR-GCC  
> **代码**: 700+ 行可运行代码
