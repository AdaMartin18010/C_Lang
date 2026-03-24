
---

## 🔗 全面知识关联体系

### 【全局层】知识库导航

| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../00_GLOBAL_INDEX.md](../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../readme.md](../readme.md) | 模块总览与目录导航 |
| **学习路径** | [../06_Thinking_Representation/06_Learning_Paths/readme.md](../06_Thinking_Representation/06_Learning_Paths/readme.md) | 阶段化学习路线规划 |
| **概念映射** | [../06_Thinking_Representation/05_Concept_Mappings/readme.md](../06_Thinking_Representation/05_Concept_Mappings/readme.md) | 核心概念等价关系图 |

### 【阶段层】学习定位

**当前模块**: 知识库
**难度等级**: L1-L6
**前置依赖**: 核心知识体系
**后续延伸**: 持续学习

```
学习阶段金字塔:
    L6 专家层 [形式验证、编译器]
    L5 高级层 [并发、系统编程] ⬅️ 可能在此
    L4 进阶层 [指针、内存管理]
    L3 基础层 [函数、结构体]
    L2 入门层 [语法、数据类型]
    L1 零基础 [环境搭建]
```

### 【层次层】纵向知识链

| 层级 | 关联文档 | 层次关系 |
|:-----|:---------|:---------|
| **理论基础** | [../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md](../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md) | 语义学理论基础 |
| **核心机制** | [../01_Core_Knowledge_System/02_Core_Layer/readme.md](../01_Core_Knowledge_System/02_Core_Layer/readme.md) | C语言核心机制 |
| **标准接口** | [../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md](../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md) | 标准库API |
| **系统实现** | [../03_System_Technology_Domains/readme.md](../03_System_Technology_Domains/readme.md) | 系统级实现 |

### 【局部层】横向关联网

| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md](../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 并发编程技术 |
| **安全规范** | [../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md](../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md) | 安全编码标准 |
| **工具支持** | [../07_Modern_Toolchain/readme.md](../07_Modern_Toolchain/readme.md) | 现代开发工具链 |
| **实践案例** | [../04_Industrial_Scenarios/readme.md](../04_Industrial_Scenarios/readme.md) | 工业实践场景 |

### 【总体层】知识体系架构

```
┌─────────────────────────────────────────────────────────────┐
│                     总体知识体系架构                          │
├─────────────────────────────────────────────────────────────┤
│  01 Core Knowledge          → 核心概念与机制                  │
│  02 Formal Semantics        → 理论与物理基础                  │
│  03 System Technology       → 系统级技术领域                  │
│  04 Industrial Scenarios    → 工业应用场景                    │
│  05 Deep Structure          → 深层结构与元物理                │
│  06 Thinking Representation → 思维表征与学习                  │
│  07 Modern Toolchain        → 现代工具链                      │
└─────────────────────────────────────────────────────────────┘
```

### 【决策层】学习路径选择

| 目标 | 推荐路径 | 关键文档 |
|:-----|:---------|:---------|
| **系统学习** | 01 → 02 → 03 → 04 | 按顺序阅读各模块 |
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../06_Thinking_Representation/01_Decision_Trees/readme.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../04_Industrial_Scenarios/readme.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../02_Formal_Semantics_and_Physics/readme.md) |

---

# 嵌入式编程挑战 (Embedded Challenges)

> **难度**: L3-L6 | **预计时间**: 30-50小时
> **硬件**: ARM Cortex-M / Arduino / ESP32

---

## 挑战1: GPIO控制基础

### 目标

实现LED闪烁和按键检测。

### 裸机实现 (无库)

```c
// 假设STM32F4系列，地址从参考手册获取
#define GPIOA_BASE      0x40020000
#define RCC_BASE        0x40023800

#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define GPIOA_MODER     (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x14))
#define GPIOA_IDR       (*(volatile uint32_t *)(GPIOA_BASE + 0x10))

// 使能GPIOA时钟
void gpioa_clock_enable(void) {
    RCC_AHB1ENR |= (1 << 0);  // 设置位0
}

// 配置PA5为输出 (LED)
void led_init(void) {
    gpioa_clock_enable();
    GPIOA_MODER &= ~(3 << 10);   // 清除模式位
    GPIOA_MODER |= (1 << 10);    // 设置为输出模式
}

// 设置LED状态
void led_set(bool on) {
    if (on) {
        GPIOA_ODR |= (1 << 5);   // 置位
    } else {
        GPIOA_ODR &= ~(1 << 5);  // 清零
    }
}

// 简单延时
void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 16000; i++);
}

int main(void) {
    led_init();

    while (1) {
        led_set(true);
        delay_ms(500);
        led_set(false);
        delay_ms(500);
    }
}
```

### 按键消抖

```c
// 配置PA0为输入 (按键)
void button_init(void) {
    gpioa_clock_enable();
    GPIOA_MODER &= ~(3 << 0);  // 输入模式
}

// 带消抖的按键检测
bool button_pressed(void) {
    static uint32_t last_time = 0;
    static bool last_state = false;

    bool current_state = (GPIOA_IDR & (1 << 0)) != 0;
    uint32_t current_time = get_tick();  // 需要实现systick

    if (current_state != last_state &&
        current_time - last_time > 20) {  // 20ms消抖
        last_time = current_time;
        last_state = current_state;
        return current_state;
    }
    return false;
}
```

---


---

## 📑 目录

- [嵌入式编程挑战 (Embedded Challenges)](#嵌入式编程挑战-embedded-challenges)
  - [挑战1: GPIO控制基础](#挑战1-gpio控制基础)
    - [目标](#目标)
    - [裸机实现 (无库)](#裸机实现-无库)
    - [按键消抖](#按键消抖)
  - [📑 目录](#-目录)
  - [挑战2: 串口通信](#挑战2-串口通信)
    - [目标](#目标-1)
    - [寄存器级实现](#寄存器级实现)
    - [中断驱动接收](#中断驱动接收)
  - [挑战3: ADC数据采集](#挑战3-adc数据采集)
    - [目标](#目标-2)
    - [轮询方式](#轮询方式)
    - [DMA方式 (连续采集)](#dma方式-连续采集)
  - [挑战4: 实时时钟与低功耗](#挑战4-实时时钟与低功耗)
    - [目标](#目标-3)
    - [RTC配置](#rtc配置)
    - [低功耗模式](#低功耗模式)
  - [挑战5: 简单RTOS调度器](#挑战5-简单rtos调度器)
    - [目标](#目标-4)
    - [任务定义](#任务定义)
    - [上下文切换](#上下文切换)
    - [调度器API](#调度器api)
  - [评估标准](#评估标准)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 挑战2: 串口通信

### 目标

实现UART驱动，支持printf重定向。

### 寄存器级实现

```c
// USART2 (PA2=TX, PA3=RX)
#define USART2_BASE     0x40004400
#define USART_SR        (*(volatile uint32_t *)(USART2_BASE + 0x00))
#define USART_DR        (*(volatile uint32_t *)(USART2_BASE + 0x04))
#define USART_BRR       (*(volatile uint32_t *)(USART2_BASE + 0x08))
#define USART_CR1       (*(volatile uint32_t *)(USART2_BASE + 0x0C))

// 状态寄存器位
#define USART_SR_TXE    (1 << 7)   // 发送寄存器空
#define USART_SR_RXNE   (1 << 5)   // 接收寄存器非空

void uart2_init(uint32_t baudrate) {
    // 1. 使能GPIOA和USART2时钟
    RCC_AHB1ENR |= (1 << 0);
    RCC_APB1ENR |= (1 << 17);

    // 2. 配置PA2(TX)为复用功能
    GPIOA_MODER &= ~(3 << 4);
    GPIOA_MODER |= (2 << 4);     // 复用模式
    GPIOA_AFRL &= ~(0xF << 8);
    GPIOA_AFRL |= (7 << 8);      // AF7 = USART2

    // 3. 配置波特率 (84MHz APB1)
    uint32_t div = 84000000 / baudrate;
    USART_BRR = div;

    // 4. 使能发送和接收
    USART_CR1 |= (1 << 3) | (1 << 2) | (1 << 13);  // TX, RX, USART使能
}

void uart2_putchar(char c) {
    while (!(USART_SR & USART_SR_TXE));  // 等待发送就绪
    USART_DR = c;
}

char uart2_getchar(void) {
    while (!(USART_SR & USART_SR_RXNE));  // 等待接收数据
    return USART_DR;
}

void uart2_puts(const char *str) {
    while (*str) {
        uart2_putchar(*str++);
    }
}

// 重定向printf
int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        uart2_putchar(ptr[i]);
    }
    return len;
}
```

### 中断驱动接收

```c
// 环形缓冲区
typedef struct {
    uint8_t buffer[256];
    volatile uint8_t head;
    volatile uint8_t tail;
} RingBuffer;

RingBuffer rx_buffer = {0};

void uart2_init_interrupt(void) {
    uart2_init(115200);

    // 使能接收中断
    USART_CR1 |= (1 << 5);  // RXNEIE

    // NVIC配置
    NVIC_ISER1 |= (1 << 6);  // USART2_IRQn = 38
}

// 中断服务程序
void USART2_IRQHandler(void) {
    if (USART_SR & USART_SR_RXNE) {
        uint8_t data = USART_DR;
        uint8_t next_head = (rx_buffer.head + 1) % sizeof(rx_buffer.buffer);
        if (next_head != rx_buffer.tail) {  // 缓冲区未满
            rx_buffer.buffer[rx_buffer.head] = data;
            rx_buffer.head = next_head;
        }
    }
}

// 非阻塞读取
int uart2_read_nonblock(uint8_t *data) {
    if (rx_buffer.head == rx_buffer.tail) {
        return 0;  // 无数据
    }
    *data = rx_buffer.buffer[rx_buffer.tail];
    rx_buffer.tail = (rx_buffer.tail + 1) % sizeof(rx_buffer.buffer);
    return 1;
}
```

---

## 挑战3: ADC数据采集

### 目标

实现多通道ADC采样和DMA传输。

### 轮询方式

```c
void adc_init(void) {
    // 使能GPIOA和ADC1时钟
    RCC_AHB1ENR |= (1 << 0);
    RCC_APB2ENR |= (1 << 8);

    // 配置PA0为模拟输入
    GPIOA_MODER |= (3 << 0);  // 模拟模式

    // ADC配置
    ADC1_CR1 = 0;
    ADC1_CR2 = (1 << 0);  // ADON

    // 采样时间
    ADC1_SMPR2 = (7 << 0);  // 480 cycles for channel 0

    // 软件启动转换
    ADC1_CR2 |= (1 << 30);  // SWSTART
}

uint16_t adc_read_channel(uint8_t channel) {
    // 设置通道
    ADC1_SQR3 = channel;

    // 开始转换
    ADC1_CR2 |= (1 << 30);

    // 等待完成
    while (!(ADC1_SR & (1 << 1)));  // EOC

    return ADC1_DR;
}
```

### DMA方式 (连续采集)

```c
#define ADC_BUFFER_SIZE 1024
uint16_t adc_buffer[ADC_BUFFER_SIZE];

void adc_dma_init(void) {
    // 使能DMA2时钟
    RCC_AHB1ENR |= (1 << 22);

    // DMA2 Stream0, Channel 0 (ADC1)
    DMA2_Stream0_CR = 0;  // 禁用
    while (DMA2_Stream0_CR & (1 << 0));  // 等待禁用

    // 配置
    DMA2_Stream0_PAR = (uint32_t)&ADC1_DR;
    DMA2_Stream0_M0AR = (uint32_t)adc_buffer;
    DMA2_Stream0_NDTR = ADC_BUFFER_SIZE;

    // 配置寄存器
    DMA2_Stream0_CR = (0 << 25) |   // Channel 0
                      (1 << 10) |   // 内存递增
                      (1 << 8) |    // 循环模式
                      (1 << 6) |    // 外设到内存
                      (1 << 5) |    // 中断使能
                      (0 << 13);    // 半字传输

    // 使能DMA流
    DMA2_Stream0_CR |= (1 << 0);

    // ADC配置为DMA模式
    ADC1_CR2 |= (1 << 8);  // DMA
    ADC1_CR2 |= (1 << 1);  // CONT (连续模式)
}
```

---

## 挑战4: 实时时钟与低功耗

### 目标

实现RTC和低功耗模式管理。

### RTC配置

```c
void rtc_init(void) {
    // 使能PWR和BKP时钟
    RCC_APB1ENR |= (1 << 28) | (1 << 27);

    // 允许访问备份域
    PWR_CR |= (1 << 8);  // DBP

    // 选择LSE (32.768kHz)
    RCC_BDCR |= (1 << 0);  // LSEON
    while (!(RCC_BDCR & (1 << 1)));  // 等待就绪

    RCC_BDCR |= (1 << 8);  // RTCSEL = LSE
    RCC_BDCR |= (1 << 15); // RTCEN

    // 进入初始化模式
    RTC_ISR |= (1 << 7);  // INIT
    while (!(RTC_ISR & (1 << 6)));  // 等待INITF

    // 配置预分频器 (32768 / 128 / 256 = 1Hz)
    RTC_PRER = (127 << 16) | 255;

    // 退出初始化模式
    RTC_ISR &= ~(1 << 7);
}

void rtc_set_time(uint8_t hour, uint8_t minute, uint8_t second) {
    uint32_t tr = ((hour / 10) << 20) | ((hour % 10) << 16) |
                  ((minute / 10) << 12) | ((minute % 10) << 8) |
                  ((second / 10) << 4) | (second % 10);
    RTC_TR = tr;
}

void rtc_get_time(uint8_t *hour, uint8_t *minute, uint8_t *second) {
    uint32_t tr = RTC_TR;
    *hour = ((tr >> 20) & 0x3) * 10 + ((tr >> 16) & 0xF);
    *minute = ((tr >> 12) & 0x7) * 10 + ((tr >> 8) & 0xF);
    *second = ((tr >> 4) & 0x7) * 10 + (tr & 0xF);
}
```

### 低功耗模式

```c
// 进入Sleep模式 (CPU停止，外设运行)
void enter_sleep_mode(void) {
    SCB_SCR &= ~(1 << 2);  // 清除SLEEPDEEP
    __WFI();  // Wait For Interrupt
}

// 进入Stop模式 (CPU和大部分时钟停止)
void enter_stop_mode(void) {
    SCB_SCR |= (1 << 2);   // SLEEPDEEP
    PWR_CR &= ~(3 << 0);   // 清除PDDS和LPMS
    PWR_CR |= (1 << 1);    // PDDS=0, 电压调节器开
    __WFI();
}

// 进入Standby模式 (最低功耗，RAM丢失)
void enter_standby_mode(void) {
    SCB_SCR |= (1 << 2);   // SLEEPDEEP
    PWR_CR |= (1 << 1);    // PDDS=1
    SCB_AIRCR = (0x5FA << 16) | (1 << 2);  // 系统复位
}
```

---

## 挑战5: 简单RTOS调度器

### 目标

实现一个合作式多任务调度器。

### 任务定义

```c
#define MAX_TASKS 8
#define STACK_SIZE 1024

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_SUSPENDED
} TaskState;

typedef struct {
    uint32_t *stack_ptr;
    void (*task_func)(void);
    TaskState state;
    uint32_t stack[STACK_SIZE];
    uint32_t delay_until;
    uint8_t priority;
} Task;

typedef struct {
    Task tasks[MAX_TASKS];
    uint8_t task_count;
    uint8_t current_task;
    volatile uint32_t tick_count;
} Scheduler;

Scheduler scheduler = {0};
```

### 上下文切换

```c
// 保存当前上下文
// 恢复下一个任务的上下文
__attribute__((naked)) void PendSV_Handler(void) {
    __asm volatile (
        "MRS     R0, PSP\n"           // 获取进程栈指针
        "STMDB   R0!, {R4-R11}\n"    // 保存R4-R11
        "LDR     R3, =scheduler\n"
        "LDR     R2, [R3, #4]\n"     // current_task
        "STR     R0, [R3, R2, LSL #2]\n"  // 保存SP

        // 选择下一个任务...

        "LDMIA   R0!, {R4-R11}\n"    // 恢复R4-R11
        "MSR     PSP, R0\n"           // 恢复PSP
        "BX      LR\n"
    );
}
```

### 调度器API

```c
int task_create(void (*func)(void), uint8_t priority);
void task_delay(uint32_t ms);
void task_yield(void);
void task_suspend(int task_id);
void task_resume(int task_id);

// 启动调度器
void scheduler_start(void) {
    // 配置SysTick (1ms)
    SysTick_Config(SystemCoreClock / 1000);

    // 启动第一个任务
    // ...
}
```

---

## 评估标准

| 项目 | 要求 |
|:-----|:-----|
| 功能正确 | 代码在目标硬件上正常运行 |
| 寄存器理解 | 正确配置外设寄存器 |
| 中断处理 | 正确处理中断和异常 |
| 资源管理 | 合理使用内存和外设 |
| 低功耗考虑 | 实现功耗优化 |

---

> **返回导航**: [练习模块总览](./readme.md) | [知识库总览](../readme.md)


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
