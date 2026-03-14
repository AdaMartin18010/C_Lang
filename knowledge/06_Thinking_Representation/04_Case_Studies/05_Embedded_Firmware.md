# 嵌入式固件案例研究

> **分类**: 嵌入式系统开发
> **难度**: 中级
> **适用场景**: MCU固件、IoT设备、低功耗系统

---

## 1. 概述

本案例研究一个典型嵌入式固件的设计与实现，涵盖启动流程、外设驱动、中断管理和低功耗设计等核心主题。

---

## 2. 架构图

```
┌─────────────────────────────────────────────────────────────┐
│                      应用层                                  │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐    │
│  │ 传感器   │  │ 通信协议  │  │ 控制算法  │  │ 状态机   │    │
│  │ 数据采集 │  │ 处理      │  │ (PID等)  │  │ 管理     │    │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘    │
└───────┼─────────────┼─────────────┼─────────────┼──────────┘
        │             │             │             │
        └─────────────┴─────────────┴─────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                      中间件层                                │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐    │
│  │ RTOS     │  │ 文件系统 │  │ 网络栈   │  │ 电源管理 │    │
│  │ (FreeRTOS│  │ (LittleFS│  │ (lwIP)   │  │ (PM框架) │    │
│  │ /Zephyr) │  │ /FATFS)  │  │          │  │          │    │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘    │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    硬件抽象层(HAL)                           │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐    │
│  │ GPIO     │  │ UART     │  │ SPI/I2C  │  │ Timer    │    │
│  │ 驱动     │  │ 驱动     │  │ 驱动     │  │ 驱动     │    │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘    │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐    │
│  │ ADC      │  │ PWM      │  │ DMA      │  │ Interrupt│    │
│  │ 驱动     │  │ 驱动     │  │ 驱动     │  │ 管理     │    │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘    │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                        硬件层                                │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐    │
│  │ MCU核心  │  │ 内存      │  │ 外设      │  │ 外接     │    │
│  │ (Cortex- │  │ (Flash/  │  │ (定时器   │  │ 传感器   │    │
│  │ M4/etc)  │  │  SRAM)   │  │ /通信)    │  │ 执行器   │    │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘    │
└─────────────────────────────────────────────────────────────┘
```

---

## 3. 核心实现

### 3.1 启动流程 (Bootloader)

```c
// startup_stm32f4xx.s (简化版)
// 启动文件 - 汇编部分

.section .text.Reset_Handler
.global Reset_Handler

Reset_Handler:
    // 1. 初始化栈指针
    ldr sp, =_estack

    // 2. 复制.data段从Flash到RAM
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
    b CopyDataInit

CopyDataLoop:
    ldr r3, [r2], #4
    str r3, [r0], #4

CopyDataInit:
    cmp r0, r1
    blo CopyDataLoop

    // 3. 清零.bss段
    ldr r0, =_sbss
    ldr r1, =_ebss
    mov r2, #0
    b ZeroBssInit

ZeroBssLoop:
    str r2, [r0], #4

ZeroBssInit:
    cmp r0, r1
    blo ZeroBssLoop

    // 4. 调用SystemInit (时钟配置)
    bl SystemInit

    // 5. 调用main函数
    bl main

    // 6. main返回后的处理
    b InfiniteLoop

InfiniteLoop:
    b InfiniteLoop
```

```c
// system_stm32f4xx.c
// 系统初始化

#define HSE_VALUE 8000000U
#define PLL_M 8
#define PLL_N 336
#define PLL_P 2
#define PLL_Q 7

void SystemInit(void) {
    // 1. 使能FPU
    SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));

    // 2. 配置Flash预取取和等待状态
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |
                 FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;

    // 3. 使能HSE
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    // 4. 配置PLL
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) |
                   (((PLL_P >> 1) - 1) << 16) |
                   RCC_PLLCFGR_PLLSRC_HSE | (PLL_Q << 24);

    // 5. 使能PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // 6. 切换系统时钟到PLL
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    // 7. 更新SystemCoreClock变量
    SystemCoreClockUpdate();
}
```

### 3.2 外设驱动框架

```c
// gpio_driver.h
#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_AF,       // 复用功能
    GPIO_MODE_ANALOG
} gpio_mode_t;

typedef enum {
    GPIO_SPEED_LOW,
    GPIO_SPEED_MEDIUM,
    GPIO_SPEED_HIGH,
    GPIO_SPEED_VERY_HIGH
} gpio_speed_t;

typedef enum {
    GPIO_PULL_NONE,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN
} gpio_pull_t;

typedef struct {
    uint8_t        pin;
    gpio_mode_t    mode;
    gpio_speed_t   speed;
    gpio_pull_t    pull;
    uint8_t        alternate;  // 复用功能号
} gpio_config_t;

// API声明
void gpio_init(uint8_t port, const gpio_config_t* config);
void gpio_write(uint8_t port, uint8_t pin, bool value);
bool gpio_read(uint8_t port, uint8_t pin);
void gpio_toggle(uint8_t port, uint8_t pin);

#endif
```

```c
// gpio_driver.c
#include "gpio_driver.h"

// GPIO寄存器基地址
static GPIO_TypeDef* const GPIO_PORTS[] = {
    GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI
};

void gpio_init(uint8_t port, const gpio_config_t* cfg) {
    GPIO_TypeDef* GPIOx = GPIO_PORTS[port];
    uint8_t pin = cfg->pin;

    // 1. 使能GPIO时钟
    RCC->AHB1ENR |= (1 << port);

    // 2. 配置模式
    GPIOx->MODER &= ~(3 << (pin * 2));
    GPIOx->MODER |= (cfg->mode << (pin * 2));

    // 3. 配置输出速度
    GPIOx->OSPEEDR &= ~(3 << (pin * 2));
    GPIOx->OSPEEDR |= (cfg->speed << (pin * 2));

    // 4. 配置上拉/下拉
    GPIOx->PUPDR &= ~(3 << (pin * 2));
    GPIOx->PUPDR |= (cfg->pull << (pin * 2));

    // 5. 配置复用功能(如果需要)
    if (cfg->mode == GPIO_MODE_AF) {
        if (pin < 8) {
            GPIOx->AFR[0] &= ~(0xF << (pin * 4));
            GPIOx->AFR[0] |= (cfg->alternate << (pin * 4));
        } else {
            GPIOx->AFR[1] &= ~(0xF << ((pin - 8) * 4));
            GPIOx->AFR[1] |= (cfg->alternate << ((pin - 8) * 4));
        }
    }
}

void gpio_write(uint8_t port, uint8_t pin, bool value) {
    GPIO_TypeDef* GPIOx = GPIO_PORTS[port];
    if (value) {
        GPIOx->BSRR = (1 << pin);
    } else {
        GPIOx->BSRR = (1 << (pin + 16));
    }
}

bool gpio_read(uint8_t port, uint8_t pin) {
    GPIO_TypeDef* GPIOx = GPIO_PORTS[port];
    return (GPIOx->IDR & (1 << pin)) != 0;
}

void gpio_toggle(uint8_t port, uint8_t pin) {
    GPIO_TypeDef* GPIOx = GPIO_PORTS[port];
    GPIOx->ODR ^= (1 << pin);
}
```

### 3.3 中断管理

```c
// interrupt_manager.h
#ifndef INTERRUPT_MANAGER_H
#define INTERRUPT_MANAGER_H

typedef void (*irq_handler_t)(void);

// 中断优先级配置
typedef enum {
    IRQ_PRIO_HIGHEST = 0,
    IRQ_PRIO_HIGH    = 2,
    IRQ_PRIO_MEDIUM  = 8,
    IRQ_PRIO_LOW     = 14,
    IRQ_PRIO_LOWEST  = 15
} irq_priority_t;

// API
void irq_register(uint8_t irq_num, irq_handler_t handler,
                  irq_priority_t priority);
void irq_enable(uint8_t irq_num);
void irq_disable(uint8_t irq_num);
void irq_set_pending(uint8_t irq_num);
void irq_clear_pending(uint8_t irq_num);

#endif
```

```c
// interrupt_manager.c
#include "interrupt_manager.h"

#define MAX_IRQ_HANDLERS 96

static irq_handler_t irq_handlers[MAX_IRQ_HANDLERS];

void irq_register(uint8_t irq_num, irq_handler_t handler,
                  irq_priority_t priority) {
    // 保存处理函数
    irq_handlers[irq_num] = handler;

    // 配置优先级 (NVIC)
    uint8_t reg = irq_num / 4;
    uint8_t off = (irq_num % 4) * 8;

    // 设置优先级 (使用4位优先级)
    NVIC->IP[reg] = (NVIC->IP[reg] & ~(0xFF << off)) |
                    (priority << (off + 4));
}

void irq_enable(uint8_t irq_num) {
    NVIC->ISER[irq_num / 32] = (1 << (irq_num % 32));
}

void irq_disable(uint8_t irq_num) {
    NVIC->ICER[irq_num / 32] = (1 << (irq_num % 32));
}

// 弱定义默认处理函数
void __attribute__((weak)) TIM2_IRQHandler(void) {
    if (irq_handlers[TIM2_IRQn]) {
        irq_handlers[TIM2_IRQn]();
    }
}

void __attribute__((weak)) USART1_IRQHandler(void) {
    if (irq_handlers[USART1_IRQn]) {
        irq_handlers[USART1_IRQn]();
    }
}

// 其他中断处理函数...
```

---

## 4. 低功耗设计

### 4.1 功耗管理状态机

```
┌─────────────────────────────────────────────────────────────┐
│                     功耗状态转换图                           │
│                                                              │
│    ┌─────────┐    空闲超时    ┌─────────┐                   │
│    │  RUN    │──────────────▶│  SLEEP  │                   │
│    │ (全速)  │◀──────────────│ (睡眠)  │                   │
│    └────┬────┘   中断唤醒     └────┬────┘                   │
│         │                          │                        │
│         │ 长时间空闲               │ 更长空闲                │
│         ▼                          ▼                        │
│    ┌─────────┐               ┌─────────┐                   │
│    │  STOP   │◀─────────────│  IDLE   │                   │
│    │ (停止)  │   中断唤醒    │ (等待)  │                   │
│    └────┬────┘               └─────────┘                   │
│         │                                                   │
│         │ 外部事件                                          │
│         ▼                                                   │
│    ┌─────────┐                                              │
│    │ STANDBY │                                              │
│    │ (待机)  │                                              │
│    └─────────┘                                              │
│                                                              │
│  功耗: RUN > SLEEP > IDLE > STOP > STANDBY                   │
│  唤醒延迟: RUN < SLEEP < IDLE < STOP < STANDBY               │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 低功耗代码示例

```c
// power_manager.c

#include "power_manager.h"

static volatile power_mode_t current_mode = POWER_MODE_RUN;

void enter_sleep_mode(void) {
    // 配置WFI(等待中断)进入睡眠模式
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

    // 可选: 关闭某些外设时钟
    RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    __WFI();  // 等待中断

    // 唤醒后恢复时钟
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
}

void enter_stop_mode(void) {
    current_mode = POWER_MODE_STOP;

    // 配置进入STOP模式
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    // 配置电压调节器
    PWR->CR &= ~PWR_CR_PDDS;  // 进入STOP模式，非STANDBY
    PWR->CR |= PWR_CR_LPMS_STOP1;

    // 保存外设状态
    backup_peripheral_state();

    // 进入低功耗模式
    __WFI();

    // 唤醒后的恢复
    restore_peripheral_state();
    SystemClock_Restore();

    current_mode = POWER_MODE_RUN;
}

void enter_standby_mode(void) {
    // 清除唤醒标志
    PWR->CR |= PWR_CR_CWUF;

    // 配置进入STANDBY模式
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    PWR->CR |= PWR_CR_PDDS;

    // 使能WKUP引脚唤醒
    PWR->CSR |= PWR_CSR_EWUP1;

    __WFI();
    // 注意: 从STANDBY唤醒相当于复位
}

// 功耗优化: 动态电压频率调节(DVFS)
void set_cpu_frequency(uint32_t freq_mhz) {
    if (freq_mhz <= 16) {
        // 使用HSI，关闭PLL
        RCC->CFGR &= ~RCC_CFGR_SW;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
        RCC->CR &= ~RCC_CR_PLLON;
    } else {
        // 重新配置PLL
        // ... 根据目标频率调整PLL参数
    }
}
```

---

## 5. 设计要点

### 5.1 内存布局规划

```
┌────────────────────────────────────────────────────────┐
│                    Flash内存布局                        │
│  ┌────────────────────────────────────────────────┐   │
│  │ 0x0800_0000: Bootloader (16KB)                 │   │
│  ├────────────────────────────────────────────────┤   │
│  │ 0x0800_4000: Application (480KB)               │   │
│  │   - .text (代码段)                              │   │
│  │   - .rodata (只读数据)                          │   │
│  ├────────────────────────────────────────────────┤   │
│  │ 0x0807_C000: Config Storage (16KB)             │   │
│  └────────────────────────────────────────────────┘   │
├────────────────────────────────────────────────────────┤
│                    SRAM内存布局                         │
│  ┌────────────────────────────────────────────────┐   │
│  │ 0x2000_0000: .data (已初始化数据)               │   │
│  ├────────────────────────────────────────────────┤   │
│  │                .bss (未初始化数据)              │   │
│  ├────────────────────────────────────────────────┤   │
│  │                Heap (动态分配)                  │   │
│  ├────────────────────────────────────────────────┤   │
│  │                Stack (向下增长)                 │   │
│  │ 0x2002_0000: Stack Top                         │   │
│  └────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────┘
```

### 5.2 实时性保障

| 技术 | 实现方式 | 适用场景 |
|------|----------|----------|
| **中断优先级** | NVIC分组 | 紧急事件响应 |
| **DMA传输** | 外设-DMA-内存 | 大数据量吞吐 |
| **双缓冲** | 乒乓缓冲 | 连续数据采集 |
| **零拷贝** | 指针传递 | 协议栈优化 |

---

## 6. 常见问题

### 6.1 调试技巧

```
┌────────────────────────────────────────────────────────┐
│                   嵌入式调试方法                        │
├────────────────┬───────────────────────────────────────┤
│ SWD/JTAG调试   │ 断点、单步、查看寄存器/内存           │
├────────────────┼───────────────────────────────────────┤
│ 串口打印       │ printf重定向到UART                    │
├────────────────┼───────────────────────────────────────┤
│ LED指示        │ GPIO输出调试状态                      │
├────────────────┼───────────────────────────────────────┤
│ 逻辑分析仪     │ 捕获时序、协议分析                    │
├────────────────┼───────────────────────────────────────┤
│ 示波器         │ 模拟信号、电源分析                    │
└────────────────┴───────────────────────────────────────┘
```

### 6.2 常见陷阱

| 问题 | 症状 | 解决方案 |
|------|------|----------|
| 时钟未使能 | 外设无响应 | 检查RCC时钟配置 |
| 中断未清除 | 重复进入中断 | 及时清除中断标志 |
| 栈溢出 | 随机崩溃 | 增大栈空间/优化局部变量 |
| 看门狗复位 | 定期重启 | 正确喂狗/优化超时 |
| 浮点异常 | 硬故障 | 使能FPU/检查除零 |

---

## 7. 扩展阅读

- [Cortex-M4 Programming Manual](https://example.com/cm4)
- [Mastering STM32](https://example.com/stm32)
- [Making Embedded Systems](https://example.com/embedded)

---

> **状态**: ✅ 已完成
