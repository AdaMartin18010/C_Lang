# 树莓派 Pico (RP2040) 完整开发指南

> 双核 Cortex-M0+ 微控制器实战：裸机、FreeRTOS、MicroPython

---

## 1. RP2040 架构详解

### 1.1 核心规格

| 特性 | 规格 |
|:-----|:-----|
| CPU | 双核 ARM Cortex-M0+ @ 133MHz |
| RAM | 264KB SRAM (6个独立bank) |
| Flash | 最大 16MB 外接 QSPI Flash |
| GPIO | 30个可编程GPIO |
| ADC | 4路 12位 SAR ADC |
| UART | 2个 |
| I2C | 2个 |
| SPI | 2个 |
| PWM | 16通道 |
| USB | 1.1 Host/Device |
| 独特功能 | 可编程IO (PIO)、DMA、浮点库 |

### 1.2 内存布局

```
0x20000000 ┌─────────────────┐
           │   SRAM0-5       │  264KB
           │   (可执行/数据) │
0x20042000 ├─────────────────┤
           │   XIP Flash     │  16MB
0x10000000 │   (执行原位)    │
           └─────────────────┘

SRAM Bank 分布：
- SRAM0-3: 64KB each (共256KB，可执行)
- SRAM4-5: 4KB each (共8KB，仅数据)
```

---

## 2. 开发环境

### 2.1 Pico SDK 安装

```bash
# Linux/macOS
mkdir ~/pico && cd ~/pico
git clone -b 1.5.1 https://github.com/raspberrypi/pico-sdk.git
git clone https://github.com/raspberrypi/pico-examples.git

# 设置环境变量
export PICO_SDK_PATH=~/pico/pico-sdk

# 安装工具链
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib

# 编译示例
cd pico-examples
mkdir build && cd build
cmake ..
make -j4

# 烧录 (按住BOOTSEL连接USB，然后)
cp blink.uf2 /media/$USER/RPI-RP2/
```

### 2.2 项目模板

```
pico_project/
├── CMakeLists.txt
├── pico_sdk_import.cmake    # SDK导入
├── src/
│   └── main.c
└── build/                   # 构建目录
```

### 2.3 CMakeLists.txt 模板

```cmake
cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(my_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(my_app
    src/main.c
)

# 链接库
target_link_libraries(my_app
    pico_stdlib
    hardware_i2c
    hardware_spi
    hardware_pwm
    hardware_adc
    hardware_dma
    pico_multicore      # 双核支持
)

# 生成UF2和ELF
pico_add_extra_outputs(my_app)

# 启用USB输出，禁用UART
pico_enable_stdio_usb(my_app 1)
pico_enable_stdio_uart(my_app 0)
```

---

## 3. GPIO 编程

### 3.1 基础 GPIO 控制

```c
/* src/main.c */
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LED_PIN     25      /* 板载LED */
#define BUTTON_PIN  14

int main(void)
{
    /* 初始化标准库 */
    stdio_init_all();

    /* LED 输出 */
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    /* 按键输入 + 内部上拉 */
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    printf("RP2040 GPIO Demo\n");

    while(1) {
        /* 读取按键 (低电平有效) */
        if(!gpio_get(BUTTON_PIN)) {
            gpio_put(LED_PIN, 1);
            printf("Button pressed!\n");

            /* 简单消抖 */
            sleep_ms(50);
            while(!gpio_get(BUTTON_PIN));
            sleep_ms(50);
        } else {
            gpio_put(LED_PIN, 0);
        }

        sleep_ms(10);
    }

    return 0;
}
```

### 3.2 硬件 PWM 控制

```c
/* pwm_servo.c */
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define SERVO_PIN   15

/* 舵机控制：0.5ms-2.5ms 脉宽对应 0-180度 */
void servo_init(uint gpio)
{
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(gpio);
    uint channel = pwm_gpio_to_channel(gpio);

    /* 配置 PWM 50Hz (20ms周期) */
    pwm_config config = pwm_get_default_config();
    /* 系统时钟 125MHz，分频后得到 50Hz */
    pwm_config_set_clkdiv(&config, 125.0f * 1000000 / (50 * 65536));
    pwm_config_set_wrap(&config, 65535);

    pwm_init(slice_num, &config, true);
}

void servo_set_angle(uint gpio, float angle)
{
    /* 0度=0.5ms, 180度=2.5ms */
    float pulse_ms = 0.5f + (angle / 180.0f) * 2.0f;
    float duty = pulse_ms / 20.0f;  /* 20ms周期 */
    uint16_t level = (uint16_t)(duty * 65535);

    pwm_set_gpio_level(gpio, level);
}

int main(void)
{
    stdio_init_all();
    servo_init(SERVO_PIN);

    while(1) {
        /* 0-180度摆动 */
        for(int angle = 0; angle <= 180; angle += 5) {
            servo_set_angle(SERVO_PIN, angle);
            sleep_ms(20);
        }
        for(int angle = 180; angle >= 0; angle -= 5) {
            servo_set_angle(SERVO_PIN, angle);
            sleep_ms(20);
        }
    }
}
```

### 3.3 硬件中断

```c
/* gpio_interrupt.c */
#include "hardware/irq.h"

volatile uint32_t pulse_count = 0;
volatile uint64_t last_pulse_time = 0;

/* GPIO 中断服务程序 */
void gpio_callback(uint gpio, uint32_t events)
{
    if(events & GPIO_IRQ_EDGE_RISE) {
        pulse_count++;
        last_pulse_time = time_us_64();
    }
}

int main(void)
{
    stdio_init_all();

    /* 配置输入 */
    gpio_init(14);
    gpio_set_dir(14, GPIO_IN);
    gpio_pull_up(14);

    /* 注册中断回调 */
    gpio_set_irq_enabled_with_callback(14,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true,
        &gpio_callback);

    while(1) {
        printf("Pulse count: %lu\n", pulse_count);
        sleep_ms(1000);
    }
}
```

---

## 4. 可编程IO (PIO) - 独特功能

### 4.1 什么是 PIO

PIO 是 RP2040 的**可编程状态机**，允许用户自定义硬件接口协议，不占用CPU。

**特点**：

- 2个PIO块，每个4个状态机
- 专用指令集（9条指令）
- 直接访问 GPIO，无CPU干预
- 可用于：自定义SPI、I2S、DVI、UART等

### 4.2 PIO 汇编示例：WS2812 LED

```c
/* ws2812_pio.c */
#include "hardware/pio.h"
#include "hardware/clocks.h"

/* PIO 程序：WS2812 (NeoPixel) */
/* 文件: ws2812.pio */
```

```asm
; ws2812.pio
.program ws2812
.side_set 1

.define public T1 2
.define public T2 5
.define public T3 3

.lang_opt python sideset_init = pico.PIO.OUT_HIGH
.lang_opt python out_init = pico.PIO.OUT_HIGH
.lang_opt python out_shiftdir = 1

bitloop:
    out x, 1       side 0 [T3 - 1]  ; Shift bit to X, drive low
    jmp !x do_zero side 1 [T1 - 1]  ; Branch on bit, drive high
    jmp bitloop    side 1 [T2 - 1]  ; Continue driving high (bit=1)
do_zero:
    nop            side 0 [T2 - 1]  ; Drive low (bit=0)
    jmp bitloop
```

```c
/* 生成的头文件: ws2812.pio.h */
/* 由 pioasm ws2812.pio -o ws2812.pio.h 生成 */

#include "ws2812.pio.h"

#define WS2812_PIN  16
#define NUM_PIXELS  8

static inline void ws2812_program_init(PIO pio, uint sm, uint offset, uint pin, float freq)
{
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    pio_sm_config c = ws2812_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, pin);
    sm_config_set_out_shift(&c, false, true, 24);  /* 24位数据 */
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    int cycles_per_bit = T1 + T2 + T3;
    float div = clock_get_hz(clk_sys) / (freq * cycles_per_bit);
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

int main(void)
{
    stdio_init_all();

    /* 加载PIO程序 */
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, 0, offset, WS2812_PIN, 800000);  /* 800KHz */

    uint32_t colors[] = {
        urgb_u32(255, 0, 0),    /* 红 */
        urgb_u32(0, 255, 0),    /* 绿 */
        urgb_u32(0, 0, 255),    /* 蓝 */
        urgb_u32(255, 255, 0),  /* 黄 */
    };

    while(1) {
        for(int i = 0; i < NUM_PIXELS; i++) {
            put_pixel(colors[i % 4]);
        }
        sleep_ms(500);
    }
}
```

### 4.3 PIO 实现自定义 UART

```asm
; uart_tx.pio - 发送
.program uart_tx
.side_set 1 opt

    pull side 1 [7]     ; 停止位
    set x, 7 side 0 [7] ; 起始位
bitloop:
    out pins, 1         ; 数据位
    jmp x-- bitloop [6]
    pull side 1 [7]     ; 停止位
```

---

## 5. 双核编程

### 5.1 双核架构

```
Core 0          Core 1
  │               │
  ├─ main()       └─ 用户任务
  │
  └─ 系统任务

共享资源：
- 264KB SRAM
- 外设寄存器
- SIO (Single-cycle IO)
```

### 5.2 启动 Core 1

```c
/* multicore_example.c */
#include "pico/multicore.h"

#define CORE1_TASK_FLAG 0x1234

/* Core 1 入口 */
void core1_entry(void)
{
    printf("Core 1 started\n");

    while(1) {
        /* 等待 Core 0 发送数据 */
        uint32_t data = multicore_fifo_pop_blocking();
        printf("Core 1 received: 0x%08X\n", data);

        /* 处理并返回结果 */
        uint32_t result = data * 2;
        multicore_fifo_push_blocking(result);
    }
}

int main(void)
{
    stdio_init_all();
    printf("Core 0 starting Core 1...\n");

    /* 启动 Core 1 */
    multicore_launch_core1(core1_entry);

    uint32_t counter = 0;

    while(1) {
        /* 发送数据到 Core 1 */
        multicore_fifo_push_blocking(counter);

        /* 等待结果 */
        uint32_t result = multicore_fifo_pop_blocking();
        printf("Core 0: sent %u, received %u\n", counter, result);

        counter++;
        sleep_ms(1000);
    }
}
```

### 5.3 双核同步 - 使用互斥锁

```c
#include "pico/sync.h"

static mutex_t core_mutex;
static volatile uint32_t shared_counter = 0;

void core1_task(void)
{
    while(1) {
        mutex_enter_blocking(&core_mutex);
        shared_counter++;
        printf("Core 1: counter = %lu\n", shared_counter);
        mutex_exit(&core_mutex);

        sleep_ms(500);
    }
}

int main(void)
{
    stdio_init_all();
    mutex_init(&core_mutex);

    multicore_launch_core1(core1_task);

    while(1) {
        mutex_enter_blocking(&core_mutex);
        shared_counter++;
        printf("Core 0: counter = %lu\n", shared_counter);
        mutex_exit(&core_mutex);

        sleep_ms(500);
    }
}
```

---

## 6. DMA 直接内存访问

### 6.1 DMA 基础

```c
/* dma_basic.c */
#include "hardware/dma.h"

#define DATA_SIZE   1024

uint32_t src_data[DATA_SIZE];
uint32_t dst_data[DATA_SIZE];

void dma_memory_copy(void)
{
    /* 填充源数据 */
    for(int i = 0; i < DATA_SIZE; i++) {
        src_data[i] = i;
    }

    /* 申请 DMA 通道 */
    int chan = dma_claim_unused_channel(true);

    /* 配置 DMA */
    dma_channel_config c = dma_channel_get_default_config(chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);  /* 32位传输 */
    channel_config_set_read_increment(&c, true);              /* 源地址递增 */
    channel_config_set_write_increment(&c, true);             /* 目的地址递增 */

    /* 启动 DMA */
    dma_channel_configure(chan, &c,
                         dst_data,      /* 目的 */
                         src_data,      /* 源 */
                         DATA_SIZE,     /* 传输数量 */
                         true);         /* 立即启动 */

    /* 等待完成 */
    dma_channel_wait_for_finish_blocking(chan);

    /* 验证 */
    bool success = true;
    for(int i = 0; i < DATA_SIZE; i++) {
        if(dst_data[i] != src_data[i]) {
            success = false;
            break;
        }
    }
    printf("DMA copy %s\n", success ? "SUCCESS" : "FAILED");

    dma_channel_unclaim(chan);
}
```

### 6.2 ADC + DMA 连续采样

```c
/* adc_dma_continuous.c */
#include "hardware/adc.h"
#include "hardware/dma.h"

#define CAPTURE_DEPTH   1024
#define ADC_CHANNEL     0   /* GPIO26 */

uint16_t capture_buf[CAPTURE_DEPTH];

void adc_dma_capture(void)
{
    /* ADC 配置 */
    adc_gpio_init(26 + ADC_CHANNEL);
    adc_init();
    adc_select_input(ADC_CHANNEL);
    adc_fifo_setup(
        true,   /* 启用 FIFO */
        true,   /* 请求 DMA */
        1,      /* 阈值1 */
        false,  /* 不要错误 */
        false   /* 不字节交换 */
    );
    adc_set_clkdiv(0);  /* 最快采样 */

    /* DMA 配置 */
    int dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);

    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&cfg, false);   /* ADC FIFO 固定地址 */
    channel_config_set_write_increment(&cfg, true);   /* 缓冲区递增 */
    channel_config_set_dreq(&cfg, DREQ_ADC);          /* ADC 触发 */

    dma_channel_configure(dma_chan, &cfg,
                         capture_buf,
                         &adc_hw->fifo,
                         CAPTURE_DEPTH,
                         true);

    printf("Starting capture...\n");
    adc_run(true);  /* 启动 ADC */

    /* 等待 DMA 完成 */
    dma_channel_wait_for_finish_blocking(dma_chan);

    adc_run(false);
    adc_fifo_drain();

    /* 打印前10个样本 */
    printf("First 10 samples:\n");
    for(int i = 0; i < 10; i++) {
        printf("  %d: %d\n", i, capture_buf[i]);
    }

    dma_channel_unclaim(dma_chan);
}
```

---

## 7. FreeRTOS 集成

### 7.1 配置 FreeRTOS

```cmake
# CMakeLists.txt
# 添加 FreeRTOS
set(FREERTOS_KERNEL_PATH /path/to/FreeRTOS-Kernel)

add_subdirectory(${FREERTOS_KERNEL_PATH} FreeRTOS-Kernel)

target_link_libraries(my_app
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
)
```

### 7.2 FreeRTOS 多任务

```c
/* freertos_example.c */
#include "FreeRTOS.h"
#include "task.h"

#define TASK_PRIORITY_LED       (tskIDLE_PRIORITY + 1)
#define TASK_PRIORITY_PRINT     (tskIDLE_PRIORITY + 2)

void led_task(void *pvParameters)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while(1) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_put(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void print_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1000);
    uint32_t counter = 0;

    while(1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        printf("Tick: %lu\n", counter++);
    }
}

int main(void)
{
    stdio_init_all();

    /* 创建任务 */
    xTaskCreate(led_task, "LED", 256, NULL, TASK_PRIORITY_LED, NULL);
    xTaskCreate(print_task, "Print", 256, NULL, TASK_PRIORITY_PRINT, NULL);

    /* 启动调度器 */
    vTaskStartScheduler();

    /* 永远不会到达这里 */
    while(1);
}
```

---

## 8. USB 设备开发

### 8.1 USB CDC (虚拟串口)

```c
/* usb_cdc.c */
#include "tusb.h"

void usb_init(void)
{
    tusb_init();
}

void usb_task(void)
{
    tud_task();

    if(tud_cdc_available()) {
        char buf[64];
        uint32_t count = tud_cdc_read(buf, sizeof(buf));

        /* 回显 */
        tud_cdc_write(buf, count);
        tud_cdc_write_flush();
    }
}
```

---

## 9. 低功耗模式

| 模式 | CPU | 时钟 | RAM | 唤醒源 | 功耗 |
|:-----|:----|:-----|:----|:-------|:-----|
| 运行 | 133MHz | 运行 | 保持 | - | ~90mA |
| 睡眠 | 暂停 | 门控 | 保持 | GPIO/定时器 | ~1.9mA |
| 休眠 | 停止 | 停止 | 保持 | GPIO/RTC | ~1.4μA |

```c
/* sleep_example.c */
#include "hardware/clocks.h"
#include "hardware/rosc.h"
#include "hardware/xosc.h"

void sleep_goto_dormant_until_edge(uint gpio, bool edge)
{
    /* 配置唤醒 GPIO */
    gpio_set_dormant_irq_enabled(gpio,
        edge ? GPIO_IRQ_EDGE_RISE : GPIO_IRQ_EDGE_FALL, true);

    /* 进入休眠 */
    clocks_hw->sleep_en0 = 0;
    clocks_hw->sleep_en1 = 0;

    xosc_dormant();
    /* 唤醒后继续 */

    clocks_init();
}
```

---

## 10. 完整项目：数据记录仪

```c
/* data_logger.c - 完整项目 */
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/fatfs_adapter.h"  /* FatFS */
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/rtc.h"

/* BME280 驱动 */
#include "bme280.h"

#define SD_SPI_PORT     spi0
#define SD_PIN_MISO     16
#define SD_PIN_CS       17
#define SD_PIN_SCK      18
#define SD_PIN_MOSI     19

#define LOG_INTERVAL_MS 60000  /* 1分钟 */

typedef struct {
    rtc_datetime_t timestamp;
    float temperature;
    float humidity;
    float pressure;
    uint16_t light_level;
} sensor_data_t;

static FATFS fs;
static FIL log_file;

bool sd_init(void)
{
    /* SPI 初始化 */
    spi_init(SD_SPI_PORT, 12500000);  /* 12.5MHz */
    gpio_set_function(SD_PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(SD_PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(SD_PIN_MOSI, GPIO_FUNC_SPI);

    /* CS */
    gpio_init(SD_PIN_CS);
    gpio_set_dir(SD_PIN_CS, GPIO_OUT);
    gpio_put(SD_PIN_CS, 1);

    /* 挂载 FatFS */
    FRESULT fr = f_mount(&fs, "0:", 1);
    if(fr != FR_OK) {
        printf("SD mount failed: %d\n", fr);
        return false;
    }

    /* 创建/打开日志文件 */
    fr = f_open(&log_file, "0:/sensor_log.csv",
                FA_OPEN_APPEND | FA_WRITE);
    if(fr != FR_OK) {
        printf("File open failed: %d\n", fr);
        return false;
    }

    /* 写入 CSV 头 */
    if(f_size(&log_file) == 0) {
        f_printf(&log_file, "Date,Time,Temperature,Humidity,Pressure,Light\n");
        f_sync(&log_file);
    }

    return true;
}

void log_data(sensor_data_t *data)
{
    char date_str[16], time_str[16];

    snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d",
             data->timestamp.year, data->timestamp.month, data->timestamp.day);
    snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d",
             data->timestamp.hour, data->timestamp.min, data->timestamp.sec);

    f_printf(&log_file, "%s,%s,%.2f,%.2f,%.2f,%u\n",
             date_str, time_str,
             data->temperature, data->humidity,
             data->pressure, data->light_level);

    f_sync(&log_file);
    printf("Logged: %s %s, T=%.1fC, H=%.1f%%\n",
           date_str, time_str, data->temperature, data->humidity);
}

int main(void)
{
    stdio_init_all();
    printf("RP2040 Data Logger\n");

    /* 初始化 RTC */
    rtc_init();
    rtc_set_datetime(&(datetime_t){
        .year = 2024, .month = 1, .day = 1,
        .hour = 0, .min = 0, .sec = 0
    });

    /* 初始化传感器 */
    i2c_init(i2c0, 100000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);

    bme280_init(i2c0);

    /* 初始化 ADC (光敏电阻) */
    adc_init();
    adc_gpio_init(28);  /* ADC2 */

    /* 初始化 SD */
    if(!sd_init()) {
        printf("SD init failed, halting\n");
        while(1) tight_loop_contents();
    }

    sensor_data_t data;

    while(1) {
        /* 读取 RTC */
        rtc_get_datetime(&data.timestamp);

        /* 读取 BME280 */
        bme280_read_data(&data.temperature, &data.humidity, &data.pressure);

        /* 读取光敏电阻 */
        adc_select_input(2);
        data.light_level = adc_read();

        /* 记录 */
        log_data(&data);

        /* 等待下一次采样 */
        sleep_ms(LOG_INTERVAL_MS);
    }
}
```

---

## 参考资源

- **Pico SDK 文档**: <https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf>
- **RP2040 数据手册**: <https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf>
- **Pico 示例**: <https://github.com/raspberrypi/pico-examples>

---

> **硬件**: Raspberry Pi Pico / Pico W
> **SDK**: Pico SDK v1.5+
> **代码**: 900+ 行可运行代码
