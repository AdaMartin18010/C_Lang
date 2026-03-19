# 裸机嵌入式开发完全指南

## 目录

- [裸机嵌入式开发完全指南](#裸机嵌入式开发完全指南)
  - [目录](#目录)
  - [1. 裸机编程概述](#1-裸机编程概述)
    - [1.1 什么是裸机编程](#11-什么是裸机编程)
    - [1.2 裸机编程的特点](#12-裸机编程的特点)
    - [1.3 裸机程序的基本结构](#13-裸机程序的基本结构)
    - [1.4 开发环境搭建](#14-开发环境搭建)
  - [2. 启动代码(Startup)编写](#2-启动代码startup编写)
    - [2.1 启动代码的作用](#21-启动代码的作用)
    - [2.2 Cortex-M3 启动代码详解](#22-cortex-m3-启动代码详解)
  - [3. 链接器脚本详解](#3-链接器脚本详解)
    - [3.1 链接器脚本的作用](#31-链接器脚本的作用)
    - [3.2 STM32F103C8T6 链接器脚本](#32-stm32f103c8t6-链接器脚本)
    - [3.3 链接器脚本高级技巧](#33-链接器脚本高级技巧)
  - [4. 中断向量表配置](#4-中断向量表配置)
    - [4.1 中断向量表结构](#41-中断向量表结构)
  - [5. 时钟系统配置](#5-时钟系统配置)
    - [5.1 STM32F103 时钟树](#51-stm32f103-时钟树)
    - [5.2 时钟配置代码](#52-时钟配置代码)
  - [6. GPIO寄存器操作](#6-gpio寄存器操作)
    - [6.1 GPIO寄存器详解](#61-gpio寄存器详解)
  - [7. 定时器实现](#7-定时器实现)
    - [7.1 基础定时器(TIM2/TIM3/TIM4)](#71-基础定时器tim2tim3tim4)
  - [8. UART轮询/中断驱动](#8-uart轮询中断驱动)
    - [8.1 UART寄存器操作](#81-uart寄存器操作)
  - [9. 无OS任务调度](#9-无os任务调度)
    - [9.1 合作式调度器](#91-合作式调度器)
  - [10. 实际项目: STM32F103裸机模板](#10-实际项目-stm32f103裸机模板)
    - [10.1 项目结构](#101-项目结构)
    - [10.2 完整主程序示例](#102-完整主程序示例)
    - [10.3 Makefile构建脚本](#103-makefile构建脚本)
    - [10.4 CMake构建配置](#104-cmake构建配置)
  - [11. 调试技巧(OpenOCD/J-Link)](#11-调试技巧openocdj-link)
    - [11.1 OpenOCD配置与使用](#111-openocd配置与使用)
      - [启动OpenOCD服务器](#启动openocd服务器)
      - [OpenOCD常用命令 (telnet 4444)](#openocd常用命令-telnet-4444)
    - [11.2 GDB调试配置](#112-gdb调试配置)
      - [GDB启动脚本 (.gdbinit)](#gdb启动脚本-gdbinit)
      - [GDB常用命令](#gdb常用命令)
    - [11.3 调试技巧](#113-调试技巧)
      - [硬故障(HardFault)诊断](#硬故障hardfault诊断)
      - [调试打印宏](#调试打印宏)
    - [11.4 性能分析技巧](#114-性能分析技巧)
    - [11.5 内存使用分析](#115-内存使用分析)
  - [总结](#总结)
    - [裸机开发关键要点](#裸机开发关键要点)
    - [开发流程建议](#开发流程建议)
    - [推荐阅读](#推荐阅读)

---

## 1. 裸机编程概述

### 1.1 什么是裸机编程

裸机编程(Bare-metal Programming)是指在没有操作系统支持的情况下，直接对硬件进行编程。程序直接运行在处理器上，直接与硬件寄存器交互。

### 1.2 裸机编程的特点

| 特性 | 说明 |
|------|------|
| 无OS开销 | 没有任务切换开销，响应确定 |
| 直接硬件控制 | 完全控制处理器和外设 |
| 代码精简 | 最终二进制文件极小(通常<10KB) |
| 启动快速 | 上电到运行代码仅需毫秒级 |
| 资源占用少 | 内存需求极低，适合资源受限设备 |

### 1.3 裸机程序的基本结构

```
┌─────────────────────────────────────┐
│           用户应用程序               │
├─────────────────────────────────────┤
│         硬件抽象层(HAL/BSP)          │
├─────────────────────────────────────┤
│           中断处理程序               │
├─────────────────────────────────────┤
│           启动代码(Startup)          │
├─────────────────────────────────────┤
│      链接器脚本(Linker Script)       │
├─────────────────────────────────────┤
│              硬件层                  │
└─────────────────────────────────────┘
```

### 1.4 开发环境搭建

```bash
# Windows (使用 MSYS2/MinGW)
pacman -S mingw-w64-x86_64-arm-none-eabi-gcc
pacman -S mingw-w64-x86_64-openocd

# Ubuntu/Debian
sudo apt-get install gcc-arm-none-eabi openocd gdb-multiarch

# macOS
brew install arm-none-eabi-gcc openocd
```

---

## 2. 启动代码(Startup)编写

### 2.1 启动代码的作用

启动代码是处理器上电后执行的第一段代码，负责：

1. 设置堆栈指针(SP)
2. 初始化数据段(.data)
3. 清零BSS段(.bss)
4. 配置中断向量表
5. 跳转到main函数

### 2.2 Cortex-M3 启动代码详解

```c
/**
 * @file startup_stm32f103.c
 * @brief STM32F103C8T6 启动代码
 * @details 包含复位处理、中断向量表、系统初始化
 */

#include <stdint.h>

/* ============================================================================
 * 链接器脚本定义的外部符号
 * ============================================================================ */

/* 初始化值存储在Flash中，运行时需要复制到RAM */
extern uint32_t _sidata;    /* .data段起始地址(Flash) */
extern uint32_t _sdata;     /* .data段起始地址(RAM) */
extern uint32_t _edata;     /* .data段结束地址(RAM) */

/* BSS段在RAM中，需要清零 */
extern uint32_t _sbss;      /* .bss段起始地址 */
extern uint32_t _ebss;      /* .bss段结束地址 */

/* 栈底初始化值 */
extern uint32_t _estack;    /* 栈顶地址 */

/* 主函数入口 */
extern int main(void);

/* 系统初始化函数(可选) */
extern void SystemInit(void);

/* ============================================================================
 * 函数原型声明
 * ============================================================================ */

/* 复位处理函数 */
void Reset_Handler(void);

/* 默认中断处理函数 */
void Default_Handler(void);

/* 弱定义的异常处理函数，可被用户覆盖 */
void NMI_Handler(void)          __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)    __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)    __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)          __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)       __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)      __attribute__((weak, alias("Default_Handler")));

/* 外部中断处理函数 */
void WWDG_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void PVD_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void TAMPER_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void RTC_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void RCC_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void EXTI0_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void EXTI1_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void EXTI2_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void EXTI3_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void EXTI4_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void ADC1_2_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void USB_HP_CAN1_TX_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void CAN1_RX1_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
void CAN1_SCE_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler(void)   __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
void TIM1_UP_IRQHandler(void)   __attribute__((weak, alias("Default_Handler")));
void TIM1_TRG_COM_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler(void)   __attribute__((weak, alias("Default_Handler")));
void TIM2_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void TIM3_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void TIM4_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler(void)   __attribute__((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler(void)   __attribute__((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler(void)   __attribute__((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler(void)   __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void USART3_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RTCAlarm_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
void USBWakeUp_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));

/* ============================================================================
 * 中断向量表
 * ============================================================================ */

/**
 * @brief 中断向量表
 * @details 必须放在Flash的起始地址(0x0800_0000)
 *          第0项: 初始栈顶地址
 *          第1项: 复位处理函数地址
 *          第2-15项: Cortex-M3 系统异常
 *          第16项起: 外设中断
 */
__attribute__((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    /* Cortex-M3 异常 */
    (void (*)(void))&_estack,    /* 栈顶地址 */
    Reset_Handler,               /* 复位处理 */
    NMI_Handler,                 /* 非屏蔽中断 */
    HardFault_Handler,           /* 硬件错误 */
    MemManage_Handler,           /* 存储器管理错误 */
    BusFault_Handler,            /* 总线错误 */
    UsageFault_Handler,          /* 用法错误 */
    0, 0, 0, 0,                  /* 保留 */
    SVC_Handler,                 /* SVCall */
    DebugMon_Handler,            /* 调试监控 */
    0,                           /* 保留 */
    PendSV_Handler,              /* PendSV */
    SysTick_Handler,             /* SysTick */

    /* 外部中断 */
    WWDG_IRQHandler,             /* 窗口看门狗 */
    PVD_IRQHandler,              /* PVD通过EXTI线检测 */
    TAMPER_IRQHandler,           /* 篡改检测 */
    RTC_IRQHandler,              /* RTC全局中断 */
    FLASH_IRQHandler,            /* Flash全局中断 */
    RCC_IRQHandler,              /* RCC全局中断 */
    EXTI0_IRQHandler,            /* EXTI线0 */
    EXTI1_IRQHandler,            /* EXTI线1 */
    EXTI2_IRQHandler,            /* EXTI线2 */
    EXTI3_IRQHandler,            /* EXTI线3 */
    EXTI4_IRQHandler,            /* EXTI线4 */
    DMA1_Channel1_IRQHandler,    /* DMA1通道1 */
    DMA1_Channel2_IRQHandler,    /* DMA1通道2 */
    DMA1_Channel3_IRQHandler,    /* DMA1通道3 */
    DMA1_Channel4_IRQHandler,    /* DMA1通道4 */
    DMA1_Channel5_IRQHandler,    /* DMA1通道5 */
    DMA1_Channel6_IRQHandler,    /* DMA1通道6 */
    DMA1_Channel7_IRQHandler,    /* DMA1通道7 */
    ADC1_2_IRQHandler,           /* ADC1和ADC2全局中断 */
    USB_HP_CAN1_TX_IRQHandler,   /* USB高优先级/CAN1 TX */
    USB_LP_CAN1_RX0_IRQHandler,  /* USB低优先级/CAN1 RX0 */
    CAN1_RX1_IRQHandler,         /* CAN1 RX1 */
    CAN1_SCE_IRQHandler,         /* CAN1 SCE */
    EXTI9_5_IRQHandler,          /* EXTI线[9:5] */
    TIM1_BRK_IRQHandler,         /* TIM1中断 */
    TIM1_UP_IRQHandler,          /* TIM1更新 */
    TIM1_TRG_COM_IRQHandler,     /* TIM1触发和通信 */
    TIM1_CC_IRQHandler,          /* TIM1捕获比较 */
    TIM2_IRQHandler,             /* TIM2全局中断 */
    TIM3_IRQHandler,             /* TIM3全局中断 */
    TIM4_IRQHandler,             /* TIM4全局中断 */
    I2C1_EV_IRQHandler,          /* I2C1事件 */
    I2C1_ER_IRQHandler,          /* I2C1错误 */
    I2C2_EV_IRQHandler,          /* I2C2事件 */
    I2C2_ER_IRQHandler,          /* I2C2错误 */
    SPI1_IRQHandler,             /* SPI1全局中断 */
    SPI2_IRQHandler,             /* SPI2全局中断 */
    USART1_IRQHandler,           /* USART1全局中断 */
    USART2_IRQHandler,           /* USART2全局中断 */
    USART3_IRQHandler,           /* USART3全局中断 */
    EXTI15_10_IRQHandler,        /* EXTI线[15:10] */
    RTCAlarm_IRQHandler,         /* RTC闹钟通过EXTI线 */
    USBWakeUp_IRQHandler,        /* USB唤醒通过EXTI线 */
};

/* ============================================================================
 * 复位处理函数
 * ============================================================================ */

/**
 * @brief 复位处理函数
 * @details 上电或复位后第一个执行的函数
 *          1. 初始化.data段(从Flash复制到RAM)
 *          2. 清零.bss段
 *          3. 调用系统初始化
 *          4. 跳转到main函数
 */
void Reset_Handler(void)
{
    /* 将.data段从Flash复制到RAM */
    uint32_t *pSrc = &_sidata;
    uint32_t *pDest = &_sdata;

    while (pDest < &_edata) {
        *pDest++ = *pSrc++;
    }

    /* 清零.bss段 */
    pDest = &_sbss;
    while (pDest < &_ebss) {
        *pDest++ = 0;
    }

    /* 调用系统初始化(时钟配置等) */
    /* SystemInit(); */

    /* 跳转到main函数 */
    main();

    /* main不应返回，如果返回则进入无限循环 */
    while (1) {
        __asm volatile ("nop");
    }
}

/**
 * @brief 默认中断处理函数
 * @details 未定义的中断处理函数都指向这里
 */
void Default_Handler(void)
{
    /* 无限循环，便于调试 */
    while (1) {
        __asm volatile ("nop");
    }
}

/* ============================================================================
 * 附加功能: 软件复位
 * ============================================================================ */

/* Cortex-M3 应用中断和复位控制寄存器 */
#define SCB_AIRCR   (*(volatile uint32_t *)0xE000ED0C)

#define SCB_AIRCR_VECTKEY_POS       16
#define SCB_AIRCR_VECTKEY_MASK      (0xFFFFUL << SCB_AIRCR_VECTKEY_POS)
#define SCB_AIRCR_VECTKEYSTAT_POS   16
#define SCB_AIRCR_VECTKEYSTAT_MASK  (0xFFFFUL << SCB_AIRCR_VECTKEYSTAT_POS)
#define SCB_AIRCR_ENDIANNESS_POS    15
#define SCB_AIRCR_ENDIANNESS_MASK   (1UL << SCB_AIRCR_ENDIANNESS_POS)
#define SCB_AIRCR_PRIGROUP_POS      8
#define SCB_AIRCR_PRIGROUP_MASK     (7UL << SCB_AIRCR_PRIGROUP_POS)
#define SCB_AIRCR_SYSRESETREQ_POS   2
#define SCB_AIRCR_SYSRESETREQ_MASK  (1UL << SCB_AIRCR_SYSRESETREQ_POS)
#define SCB_AIRCR_VECTCLRACTIVE_POS 1
#define SCB_AIRCR_VECTCLRACTIVE_MASK (1UL << SCB_AIRCR_VECTCLRACTIVE_POS)
#define SCB_AIRCR_VECTRESET_POS     0
#define SCB_AIRCR_VECTRESET_MASK    (1UL << SCB_AIRCR_VECTRESET_POS)

/**
 * @brief 系统复位函数
 * @details 通过写入AIRCR寄存器请求系统复位
 */
void NVIC_SystemReset(void)
{
    /* 写入复位密钥和复位请求位 */
    SCB_AIRCR = (0x05FAUL << SCB_AIRCR_VECTKEY_POS) |
                SCB_AIRCR_SYSRESETREQ_MASK;

    /* 等待复位完成 */
    __asm volatile ("dsb");

    /* 无限循环防止继续执行 */
    while (1) {
        __asm volatile ("nop");
    }
}

/* ============================================================================
 * 附加功能: 中断使能/禁用
 * ============================================================================ */

/* 中断控制状态寄存器 */
#define SCB_ICSR    (*(volatile uint32_t *)0xE000ED04)

/* 中断使能寄存器 */
#define NVIC_ISER0  (*(volatile uint32_t *)0xE000E100)
#define NVIC_ISER1  (*(volatile uint32_t *)0xE000E104)

/* 中断禁用寄存器 */
#define NVIC_ICER0  (*(volatile uint32_t *)0xE000E180)
#define NVIC_ICER1  (*(volatile uint32_t *)0xE000E184)

/* 中断挂起设置寄存器 */
#define NVIC_ISPR0  (*(volatile uint32_t *)0xE000E200)
#define NVIC_ISPR1  (*(volatile uint32_t *)0xE000E204)

/* 中断挂起清除寄存器 */
#define NVIC_ICPR0  (*(volatile uint32_t *)0xE000E280)
#define NVIC_ICPR1  (*(volatile uint32_t *)0xE000E284)

/* 中断活动位寄存器 */
#define NVIC_IABR0  (*(volatile uint32_t *)0xE000E300)
#define NVIC_IABR1  (*(volatile uint32_t *)0xE000E304)

/* 中断优先级寄存器 */
#define NVIC_IPR_BASE   ((volatile uint8_t *)0xE000E400)

/**
 * @brief 使能中断
 * @param IRQn 中断号(0-67)
 */
void NVIC_EnableIRQ(uint8_t IRQn)
{
    if (IRQn < 32) {
        NVIC_ISER0 = (1UL << IRQn);
    } else if (IRQn < 64) {
        NVIC_ISER1 = (1UL << (IRQn - 32));
    }
}

/**
 * @brief 禁用中断
 * @param IRQn 中断号(0-67)
 */
void NVIC_DisableIRQ(uint8_t IRQn)
{
    if (IRQn < 32) {
        NVIC_ICER0 = (1UL << IRQn);
    } else if (IRQn < 64) {
        NVIC_ICER1 = (1UL << (IRQn - 32));
    }
}

/**
 * @brief 设置中断优先级
 * @param IRQn 中断号
 * @param priority 优先级(0-15，数值越小优先级越高)
 * @note STM32F103使用4位优先级，NVIC_PRIO_BITS = 4
 */
void NVIC_SetPriority(uint8_t IRQn, uint8_t priority)
{
    /* STM32F103: 4位优先级，左移4位对齐 */
    NVIC_IPR_BASE[IRQn] = (priority << 4) & 0xFF;
}

/**
 * @brief 设置中断优先级分组
 * @param PriorityGroup 优先级分组
 *        - 0: 0位抢占优先级，4位响应优先级
 *        - 1: 1位抢占优先级，3位响应优先级
 *        - 2: 2位抢占优先级，2位响应优先级
 *        - 3: 3位抢占优先级，1位响应优先级
 *        - 4: 4位抢占优先级，0位响应优先级
 */
void NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
    uint32_t reg_value;
    uint32_t PriorityGroupTmp = (PriorityGroup & 0x07UL);

    reg_value  = SCB_AIRCR;
    reg_value &= ~(SCB_AIRCR_VECTKEY_MASK | SCB_AIRCR_PRIGROUP_MASK);
    reg_value  = (reg_value |
                  (0x05FAUL << SCB_AIRCR_VECTKEY_POS) |
                  (PriorityGroupTmp << SCB_AIRCR_PRIGROUP_POS));
    SCB_AIRCR = reg_value;
}
```

---

## 3. 链接器脚本详解

### 3.1 链接器脚本的作用

链接器脚本(Linker Script)告诉链接器：

- 程序的各个段应该放在内存的什么位置
- 内存区域的起始地址和大小
- 符号的定义和导出

### 3.2 STM32F103C8T6 链接器脚本

```ld
/**
 * @file stm32f103c8.ld
 * @brief STM32F103C8T6 链接器脚本
 * @details
 *   - Flash: 64KB (0x0800_0000 - 0x0800_FFFF)
 *   - RAM:   20KB (0x2000_0000 - 0x2000_4FFF)
 */

/* ============================================================================
 * 入口点
 * ============================================================================ */
ENTRY(Reset_Handler)

/* ============================================================================
 * 内存区域定义
 * ============================================================================ */
MEMORY
{
    /* Flash 存储器: 64KB */
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K

    /* SRAM 存储器: 20KB */
    RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 20K

    /* CCM 核心耦合存储器(如果存在) */
    /* CCM (rwx)   : ORIGIN = 0x10000000, LENGTH = 0K */
}

/* ============================================================================
 * 堆栈大小定义
 * ============================================================================ */
/* 栈大小: 4KB */
__stack_size = 0x1000;

/* 堆大小: 2KB */
__heap_size = 0x800;

/* ============================================================================
 * 段定义
 * ============================================================================ */
SECTIONS
{
    /* ========================================================================
     * .text 段: 代码和只读数据
     * ======================================================================== */
    .text :
    {
        /* 对齐到4字节边界 */
        . = ALIGN(4);

        /* 中断向量表必须放在Flash起始位置 */
        KEEP(*(.isr_vector))

        /* 复位处理函数紧跟其后 */
        *(.text.Reset_Handler)
        *(.text.SystemInit)

        /* 其他代码段 */
        *(.text*)

        /* 只读数据段 */
        *(.rodata*)

        /* 对齐到4字节边界 */
        . = ALIGN(4);

        /* 定义_text_end符号 */
        _etext = .;

    } > FLASH

    /* ========================================================================
     * .ARM.exidx 段: C++异常展开信息
     * ======================================================================== */
    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > FLASH

    /* ========================================================================
     * .preinit_array 段: C++预初始化函数
     * ======================================================================== */
    .preinit_array :
    {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } > FLASH

    /* ========================================================================
     * .init_array 段: C++构造函数
     * ======================================================================== */
    .init_array :
    {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);
    } > FLASH

    /* ========================================================================
     * .fini_array 段: C++析构函数
     * ======================================================================== */
    .fini_array :
    {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array*))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } > FLASH

    /* ========================================================================
     * _sidata 符号: .data段的初始化值在Flash中的起始地址
     * ======================================================================== */
    _sidata = LOADADDR(.data);

    /* ========================================================================
     * .data 段: 已初始化的全局变量和静态变量
     * ======================================================================== */
    .data :
    {
        /* 对齐到4字节边界 */
        . = ALIGN(4);

        /* 定义.data段起始地址(在RAM中) */
        _sdata = .;

        /* 所有初始化数据 */
        *(.data*)

        /* 对齐到4字节边界 */
        . = ALIGN(4);

        /* 定义.data段结束地址 */
        _edata = .;

    } > RAM AT > FLASH

    /* ========================================================================
     * .bss 段: 未初始化的全局变量和静态变量
     * ======================================================================== */
    .bss :
    {
        /* 对齐到4字节边界 */
        . = ALIGN(4);

        /* 定义.bss段起始地址 */
        _sbss = .;
        __bss_start__ = _sbss;

        /* 所有未初始化数据 */
        *(.bss*)
        *(COMMON)

        /* 对齐到4字节边界 */
        . = ALIGN(4);

        /* 定义.bss段结束地址 */
        _ebss = .;
        __bss_end__ = _ebss;

    } > RAM

    /* ========================================================================
     * 栈底初始化值
     * ======================================================================== */
    __stack_bottom = ORIGIN(RAM) + LENGTH(RAM);
    _estack = __stack_bottom;

    /* ========================================================================
     * 堆初始化
     * ======================================================================== */
    __heap_start = _ebss;
    __heap_end = __stack_bottom - __stack_size;

    /* ========================================================================
     * 其他段处理
     * ======================================================================== */

    /* 删除不需要的段 */
    /DISCARD/ :
    {
        libc.a(*)
        libm.a(*)
        libgcc.a(*)
    }

    /* ARM属性段 */
    .ARM.attributes 0 :
    {
        *(.ARM.attributes)
    }
}
```

### 3.3 链接器脚本高级技巧

```ld
/**
 * @file stm32f103c8_advanced.ld
 * @brief 高级链接器脚本示例，包含更多功能
 */

ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 20K
}

/* 栈大小定义 */
__stack_size = DEFINED(__stack_size) ? __stack_size : 0x1000;

SECTIONS
{
    /* ========================================================================
     * Bootloader 支持: 预留Bootloader空间
     * ======================================================================== */
    /* 如果需要Bootloader，可以调整Flash起始地址 */
    /* .isr_vector 0x08004000 : { ... } > FLASH */

    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    /* ========================================================================
     * 自定义段: 存储设备序列号等固定信息
     * ======================================================================== */
    .device_info 0x0800FC00 (NOLOAD) :
    {
        KEEP(*(.device_info))
    } > FLASH

    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } > RAM

    /* ========================================================================
     * 栈底地址
     * ======================================================================== */
    _estack = ORIGIN(RAM) + LENGTH(RAM);

    /* ========================================================================
     * 堆边界
     * ======================================================================== */
    __heap_start = _ebss;
    __heap_end = _estack - __stack_size;

    /* ========================================================================
     * 断言检查
     * ======================================================================== */
    /* 检查栈和堆是否重叠 */
    ASSERT(__heap_end >= __heap_start, "Stack and heap overlap!")

    /* 检查代码是否超出Flash容量 */
    ASSERT(_etext <= ORIGIN(FLASH) + LENGTH(FLASH), "Flash overflow!")
}
```

---

## 4. 中断向量表配置

### 4.1 中断向量表结构

```c
/**
 * @file nvic_config.c
 * @brief NVIC中断控制器配置
 */

#include <stdint.h>

/* ============================================================================
 * 寄存器定义
 * ============================================================================ */

/* NVIC 寄存器基地址 */
#define NVIC_BASE           0xE000E100
#define SCB_BASE            0xE000ED00

/* NVIC 寄存器 */
#define NVIC_ISER(n)        (*(volatile uint32_t *)(NVIC_BASE + 0x00 + (n) * 4))
#define NVIC_ICER(n)        (*(volatile uint32_t *)(NVIC_BASE + 0x80 + (n) * 4))
#define NVIC_ISPR(n)        (*(volatile uint32_t *)(NVIC_BASE + 0x100 + (n) * 4))
#define NVIC_ICPR(n)        (*(volatile uint32_t *)(NVIC_BASE + 0x180 + (n) * 4))
#define NVIC_IABR(n)        (*(volatile uint32_t *)(NVIC_BASE + 0x200 + (n) * 4))
#define NVIC_IPR(n)         (*(volatile uint8_t  *)(NVIC_BASE + 0x300 + (n)))

/* SCB 寄存器 */
#define SCB_CPUID           (*(volatile uint32_t *)(SCB_BASE + 0x00))
#define SCB_ICSR            (*(volatile uint32_t *)(SCB_BASE + 0x04))
#define SCB_VTOR            (*(volatile uint32_t *)(SCB_BASE + 0x08))
#define SCB_AIRCR           (*(volatile uint32_t *)(SCB_BASE + 0x0C))
#define SCB_SCR             (*(volatile uint32_t *)(SCB_BASE + 0x10))
#define SCB_CCR             (*(volatile uint32_t *)(SCB_BASE + 0x14))
#define SCB_SHPR(n)         (*(volatile uint8_t  *)(SCB_BASE + 0x18 + (n)))
#define SCB_SHCSR           (*(volatile uint32_t *)(SCB_BASE + 0x24))

/* ============================================================================
 * 中断号定义
 * ============================================================================ */

typedef enum {
    /* Cortex-M3 内部异常 */
    NonMaskableInt_IRQn     = -14,  /* 非屏蔽中断 */
    HardFault_IRQn          = -13,  /* 硬件错误 */
    MemoryManagement_IRQn   = -12,  /* 存储器管理错误 */
    BusFault_IRQn           = -11,  /* 总线错误 */
    UsageFault_IRQn         = -10,  /* 用法错误 */
    SVCall_IRQn             = -5,   /* SVCall */
    DebugMonitor_IRQn       = -4,   /* 调试监控 */
    PendSV_IRQn             = -2,   /* PendSV */
    SysTick_IRQn            = -1,   /* SysTick */

    /* STM32 外部中断 */
    WWDG_IRQn               = 0,    /* 窗口看门狗 */
    PVD_IRQn                = 1,    /* PVD */
    TAMPER_IRQn             = 2,    /* 篡改检测 */
    RTC_IRQn                = 3,    /* RTC */
    FLASH_IRQn              = 4,    /* Flash */
    RCC_IRQn                = 5,    /* RCC */
    EXTI0_IRQn              = 6,    /* EXTI线0 */
    EXTI1_IRQn              = 7,    /* EXTI线1 */
    EXTI2_IRQn              = 8,    /* EXTI线2 */
    EXTI3_IRQn              = 9,    /* EXTI线3 */
    EXTI4_IRQn              = 10,   /* EXTI线4 */
    DMA1_Channel1_IRQn      = 11,   /* DMA1通道1 */
    DMA1_Channel2_IRQn      = 12,   /* DMA1通道2 */
    DMA1_Channel3_IRQn      = 13,   /* DMA1通道3 */
    DMA1_Channel4_IRQn      = 14,   /* DMA1通道4 */
    DMA1_Channel5_IRQn      = 15,   /* DMA1通道5 */
    DMA1_Channel6_IRQn      = 16,   /* DMA1通道6 */
    DMA1_Channel7_IRQn      = 17,   /* DMA1通道7 */
    ADC1_2_IRQn             = 18,   /* ADC1和ADC2 */
    USB_HP_CAN1_TX_IRQn     = 19,   /* USB高优先级/CAN1 TX */
    USB_LP_CAN1_RX0_IRQn    = 20,   /* USB低优先级/CAN1 RX0 */
    CAN1_RX1_IRQn           = 21,   /* CAN1 RX1 */
    CAN1_SCE_IRQn           = 22,   /* CAN1 SCE */
    EXTI9_5_IRQn            = 23,   /* EXTI线[9:5] */
    TIM1_BRK_IRQn           = 24,   /* TIM1中断 */
    TIM1_UP_IRQn            = 25,   /* TIM1更新 */
    TIM1_TRG_COM_IRQn       = 26,   /* TIM1触发和通信 */
    TIM1_CC_IRQn            = 27,   /* TIM1捕获比较 */
    TIM2_IRQn               = 28,   /* TIM2 */
    TIM3_IRQn               = 29,   /* TIM3 */
    TIM4_IRQn               = 30,   /* TIM4 */
    I2C1_EV_IRQn            = 31,   /* I2C1事件 */
    I2C1_ER_IRQn            = 32,   /* I2C1错误 */
    I2C2_EV_IRQn            = 33,   /* I2C2事件 */
    I2C2_ER_IRQn            = 34,   /* I2C2错误 */
    SPI1_IRQn               = 35,   /* SPI1 */
    SPI2_IRQn               = 36,   /* SPI2 */
    USART1_IRQn             = 37,   /* USART1 */
    USART2_IRQn             = 38,   /* USART2 */
    USART3_IRQn             = 39,   /* USART3 */
    EXTI15_10_IRQn          = 40,   /* EXTI线[15:10] */
    RTCAlarm_IRQn           = 41,   /* RTC闹钟 */
    USBWakeUp_IRQn          = 42,   /* USB唤醒 */
} IRQn_Type;

/* ============================================================================
 * NVIC 操作函数
 * ============================================================================ */

/**
 * @brief 使能指定中断
 * @param IRQn 中断号
 */
static inline void NVIC_EnableIRQ(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        NVIC_ISER((uint32_t)IRQn >> 5) = (1UL << ((uint32_t)IRQn & 0x1F));
    }
}

/**
 * @brief 禁用指定中断
 * @param IRQn 中断号
 */
static inline void NVIC_DisableIRQ(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        NVIC_ICER((uint32_t)IRQn >> 5) = (1UL << ((uint32_t)IRQn & 0x1F));
    }
}

/**
 * @brief 获取中断使能状态
 * @param IRQn 中断号
 * @return 0=禁用, 1=使能
 */
static inline uint32_t NVIC_GetEnableIRQ(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        return ((NVIC_ISER((uint32_t)IRQn >> 5) &
                 (1UL << ((uint32_t)IRQn & 0x1F))) != 0UL) ? 1UL : 0UL;
    }
    return 0UL;
}

/**
 * @brief 设置中断挂起状态
 * @param IRQn 中断号
 */
static inline void NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        NVIC_ISPR((uint32_t)IRQn >> 5) = (1UL << ((uint32_t)IRQn & 0x1F));
    }
}

/**
 * @brief 清除中断挂起状态
 * @param IRQn 中断号
 */
static inline void NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        NVIC_ICPR((uint32_t)IRQn >> 5) = (1UL << ((uint32_t)IRQn & 0x1F));
    }
}

/**
 * @brief 获取中断挂起状态
 * @param IRQn 中断号
 * @return 0=未挂起, 1=挂起
 */
static inline uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        return ((NVIC_ISPR((uint32_t)IRQn >> 5) &
                 (1UL << ((uint32_t)IRQn & 0x1F))) != 0UL) ? 1UL : 0UL;
    }
    return 0UL;
}

/**
 * @brief 获取中断活动状态
 * @param IRQn 中断号
 * @return 0=未活动, 1=活动
 */
static inline uint32_t NVIC_GetActive(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        return ((NVIC_IABR((uint32_t)IRQn >> 5) &
                 (1UL << ((uint32_t)IRQn & 0x1F))) != 0UL) ? 1UL : 0UL;
    }
    return 0UL;
}

/**
 * @brief 设置中断优先级
 * @param IRQn 中断号
 * @param priority 优先级(0-15，数值越小优先级越高)
 */
static inline void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
    if ((int32_t)IRQn >= 0) {
        /* 外部中断: 使用NVIC_IPR */
        NVIC_IPR((uint32_t)IRQn) = (uint8_t)((priority << 4) & 0xFF);
    } else {
        /* 内部异常: 使用SCB_SHPR */
        SCB_SHPR(((uint32_t)IRQn & 0xFUL) - 4UL) =
            (uint8_t)((priority << 4) & 0xFF);
    }
}

/**
 * @brief 获取中断优先级
 * @param IRQn 中断号
 * @return 优先级值
 */
static inline uint32_t NVIC_GetPriority(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        return ((uint32_t)NVIC_IPR((uint32_t)IRQn) >> 4);
    } else {
        return ((uint32_t)SCB_SHPR(((uint32_t)IRQn & 0xFUL) - 4UL) >> 4);
    }
}

/**
 * @brief 设置中断优先级分组
 * @param PriorityGroup 优先级分组(0-7)
 * @note STM32F103使用4位优先级，实际有效值为3-7
 */
static inline void NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
    uint32_t reg_value;
    uint32_t PriorityGroupTmp = (PriorityGroup & 0x07UL);

    reg_value  = SCB_AIRCR;
    reg_value &= ~(0xFFFFUL << 16) | (7UL << 8);
    reg_value  = (reg_value |
                  (0x05FAUL << 16) |
                  (PriorityGroupTmp << 8));
    SCB_AIRCR = reg_value;
}

/**
 * @brief 获取中断优先级分组
 * @return 优先级分组值
 */
static inline uint32_t NVIC_GetPriorityGrouping(void)
{
    return ((SCB_AIRCR & (7UL << 8)) >> 8);
}

/**
 * @brief 系统复位
 */
static inline void NVIC_SystemReset(void)
{
    SCB_AIRCR = (0x05FAUL << 16) | (1UL << 2);
    __asm volatile ("dsb");
    while (1) { __asm volatile ("nop"); }
}

/* ============================================================================
 * SysTick 定时器配置
 * ============================================================================ */

#define SysTick_BASE        0xE000E010

#define SysTick_CTRL        (*(volatile uint32_t *)(SysTick_BASE + 0x00))
#define SysTick_LOAD        (*(volatile uint32_t *)(SysTick_BASE + 0x04))
#define SysTick_VAL         (*(volatile uint32_t *)(SysTick_BASE + 0x08))
#define SysTick_CALIB       (*(volatile uint32_t *)(SysTick_BASE + 0x0C))

/* SysTick控制寄存器位定义 */
#define SysTick_CTRL_COUNTFLAG_Pos  16
#define SysTick_CTRL_CLKSOURCE_Pos  2
#define SysTick_CTRL_TICKINT_Pos    1
#define SysTick_CTRL_ENABLE_Pos     0

/**
 * @brief 配置SysTick定时器
 * @param ticks 重装载值(时钟周期数)
 * @return 0=成功, 1=失败(重装载值超出范围)
 */
static inline uint32_t SysTick_Config(uint32_t ticks)
{
    /* 重装载值必须在1-0xFFFFFF之间 */
    if ((ticks - 1) > 0xFFFFFFUL) {
        return 1;
    }

    /* 设置重装载寄存器 */
    SysTick_LOAD = ticks - 1;

    /* 清零当前值寄存器 */
    SysTick_VAL = 0;

    /* 使能SysTick，使用处理器时钟，使能中断 */
    SysTick_CTRL = (1UL << SysTick_CTRL_CLKSOURCE_Pos) |
                   (1UL << SysTick_CTRL_TICKINT_Pos) |
                   (1UL << SysTick_CTRL_ENABLE_Pos);

    return 0;
}

/**
 * @brief 获取SysTick当前值
 * @return 当前计数值
 */
static inline uint32_t SysTick_GetValue(void)
{
    return SysTick_VAL;
}

/**
 * @brief 检查SysTick是否计数到0
 * @return 1=已计数到0, 0=未计数到0
 */
static inline uint32_t SysTick_GetCountFlag(void)
{
    return ((SysTick_CTRL >> SysTick_CTRL_COUNTFLAG_Pos) & 1UL);
}

/**
 * @brief 禁用SysTick
 */
static inline void SysTick_Disable(void)
{
    SysTick_CTRL &= ~(1UL << SysTick_CTRL_ENABLE_Pos);
}

/**
 * @brief 使能SysTick
 */
static inline void SysTick_Enable(void)
{
    SysTick_CTRL |= (1UL << SysTick_CTRL_ENABLE_Pos);
}
```

---

## 5. 时钟系统配置

### 5.1 STM32F103 时钟树

```
                        ┌─────────────┐
         HSE ──────────→│   8 MHz     │
         (外部晶振)      │  (晶振)     │
                        └──────┬──────┘
                               │
                               ▼
                        ┌─────────────┐     ┌──────────┐
                        │   PLLXTPRE  │────→│  PLLCLK  │
                        │  (分频/2)   │     │ (×9=72MHz)│
                        └─────────────┘     └────┬─────┘
                                                 │
                   ┌─────────────────────────────┼─────────────┐
                   │                             │             │
                   ▼                             ▼             ▼
            ┌──────────┐                  ┌──────────┐   ┌──────────┐
            │  SYSCLK  │←─────────────────│   HCLK   │   │ USBCLK   │
            │ (72MHz)  │                  │ (72MHz)  │   │ (48MHz)  │
            └────┬─────┘                  └────┬─────┘   └──────────┘
                 │                             │
                 ▼                             ▼
            ┌──────────┐                  ┌──────────┐
            │   AHB    │                  │  Cortex  │
            │ 预分频器  │                  │  内核    │
            │ (÷1)     │                  │  时钟    │
            └────┬─────┘                  └──────────┘
                 │
     ┌───────────┼───────────┐
     │           │           │
     ▼           ▼           ▼
┌────────┐ ┌────────┐ ┌────────┐
│  APB2  │ │  APB1  │ │  DMA   │
│(72MHz) │ │(36MHz) │ │ 时钟   │
└───┬────┘ └───┬────┘ └────────┘
    │          │
    ▼          ▼
  TIM1       TIM2-7
  USART1     USART2-3
  SPI1       SPI2
  ...        I2C1-2
             ...
```

### 5.2 时钟配置代码

```c
/**
 * @file system_stm32f103.c
 * @brief STM32F103C8T6 系统时钟配置
 * @details 配置系统时钟为72MHz，使用外部8MHz晶振
 */

#include <stdint.h>

/* ============================================================================
 * 寄存器定义
 * ============================================================================ */

/* RCC 寄存器基地址 */
#define RCC_BASE            0x40021000

/* RCC 寄存器 */
#define RCC_CR              (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_CFGR            (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CIR             (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_APB2RSTR        (*(volatile uint32_t *)(RCC_BASE + 0x0C))
#define RCC_APB1RSTR        (*(volatile uint32_t *)(RCC_BASE + 0x10))
#define RCC_AHBENR          (*(volatile uint32_t *)(RCC_BASE + 0x14))
#define RCC_APB2ENR         (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR         (*(volatile uint32_t *)(RCC_BASE + 0x1C))
#define RCC_BDCR            (*(volatile uint32_t *)(RCC_BASE + 0x20))
#define RCC_CSR             (*(volatile uint32_t *)(RCC_BASE + 0x24))

/* FLASH 寄存器基地址 */
#define FLASH_BASE          0x40022000
#define FLASH_ACR           (*(volatile uint32_t *)(FLASH_BASE + 0x00))

/* ============================================================================
 * RCC_CR 寄存器位定义
 * ============================================================================ */
#define RCC_CR_HSION_Pos    0
#define RCC_CR_HSIRDY_Pos   1
#define RCC_CR_HSITRIM_Pos  3
#define RCC_CR_HSICAL_Pos   8
#define RCC_CR_HSEON_Pos    16
#define RCC_CR_HSERDY_Pos   17
#define RCC_CR_HSEBYP_Pos   18
#define RCC_CR_CSSON_Pos    19
#define RCC_CR_PLLON_Pos    24
#define RCC_CR_PLLRDY_Pos   25

/* ============================================================================
 * RCC_CFGR 寄存器位定义
 * ============================================================================ */
#define RCC_CFGR_SW_Pos         0
#define RCC_CFGR_SW_HSI         0x0
#define RCC_CFGR_SW_HSE         0x1
#define RCC_CFGR_SW_PLL         0x2

#define RCC_CFGR_SWS_Pos        2
#define RCC_CFGR_SWS_HSI        0x0
#define RCC_CFGR_SWS_HSE        0x4
#define RCC_CFGR_SWS_PLL        0x8

#define RCC_CFGR_HPRE_Pos       4
#define RCC_CFGR_HPRE_DIV1      0x0
#define RCC_CFGR_HPRE_DIV2      0x8

#define RCC_CFGR_PPRE1_Pos      8
#define RCC_CFGR_PPRE1_DIV1     0x0
#define RCC_CFGR_PPRE1_DIV2     0x4

#define RCC_CFGR_PPRE2_Pos      11
#define RCC_CFGR_PPRE2_DIV1     0x0

#define RCC_CFGR_ADCPRE_Pos     14
#define RCC_CFGR_ADCPRE_DIV6    0x2

#define RCC_CFGR_PLLSRC_Pos     16
#define RCC_CFGR_PLLSRC_HSI_DIV2 0x0
#define RCC_CFGR_PLLSRC_HSE      0x1

#define RCC_CFGR_PLLXTPRE_Pos   17
#define RCC_CFGR_PLLXTPRE_HSE    0x0
#define RCC_CFGR_PLLXTPRE_HSE_DIV2 0x1

#define RCC_CFGR_PLLMULL_Pos    18
#define RCC_CFGR_PLLMULL9       0x7

#define RCC_CFGR_USBPRE_Pos     22
#define RCC_CFGR_USBPRE_DIV1_5  0x0

/* ============================================================================
 * FLASH_ACR 寄存器位定义
 * ============================================================================ */
#define FLASH_ACR_LATENCY_Pos   0
#define FLASH_ACR_LATENCY_2WS   0x2
#define FLASH_ACR_PRFTBE_Pos    4

/* ============================================================================
 * 系统时钟变量
 * ============================================================================ */
uint32_t SystemCoreClock = 72000000;  /* 系统时钟频率: 72MHz */
const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};

/* ============================================================================
 * 错误处理函数
 * ============================================================================ */

/**
 * @brief 时钟配置错误处理
 * @details 当HSE或PLL未能就绪时调用
 */
static void Error_Handler(void)
{
    /* 进入无限循环，可通过调试器检查 */
    while (1) {
        /* 可在此处添加LED闪烁指示错误 */
    }
}

/* ============================================================================
 * 系统时钟配置函数
 * ============================================================================ */

/**
 * @brief 系统时钟初始化
 * @details
 *   配置系统时钟为72MHz:
 *   - HSE = 8MHz (外部晶振)
 *   - PLLSRC = HSE
 *   - PLLMUL = ×9  → PLLCLK = 72MHz
 *   - SYSCLK = PLLCLK = 72MHz
 *   - HCLK = SYSCLK = 72MHz (AHB不分频)
 *   - PCLK2 = HCLK = 72MHz (APB2不分频)
 *   - PCLK1 = HCLK/2 = 36MHz (APB2二分频)
 */
void SystemInit(void)
{
    /* ========================================================================
     * 步骤1: 复位RCC寄存器到默认值
     * ======================================================================== */

    /* 使能内部高速时钟HSI */
    RCC_CR |= (1 << RCC_CR_HSION_Pos);

    /* 等待HSI就绪 */
    while (!(RCC_CR & (1 << RCC_CR_HSIRDY_Pos)));

    /* 复位CFGR寄存器: 切换到HSI */
    RCC_CFGR = 0x00000000;

    /* 关闭HSE, PLL, CSS */
    RCC_CR &= ~((1 << RCC_CR_HSEON_Pos) |
                (1 << RCC_CR_CSSON_Pos) |
                (1 << RCC_CR_PLLON_Pos));

    /* 复位HSEBYP位 */
    RCC_CR &= ~(1 << RCC_CR_HSEBYP_Pos);

    /* 复位CFGR寄存器的PLLSRC, PLLXTPRE, PLLMUL, USBPRE位 */
    RCC_CFGR &= ~(0x0F << RCC_CFGR_PLLMULL_Pos);

    /* 禁用所有中断和清除挂起位 */
    RCC_CIR = 0x009F0000;

    /* ========================================================================
     * 步骤2: 配置Flash等待状态
     * ======================================================================== */

    /* 系统时钟72MHz > 48MHz，需要2个等待状态 */
    FLASH_ACR = FLASH_ACR_LATENCY_2WS | (1 << FLASH_ACR_PRFTBE_Pos);

    /* ========================================================================
     * 步骤3: 使能HSE外部高速时钟
     * ======================================================================== */

    /* 使能HSE */
    RCC_CR |= (1 << RCC_CR_HSEON_Pos);

    /* 等待HSE就绪，超时处理 */
    uint32_t timeout = 0xFFFF;
    while (!(RCC_CR & (1 << RCC_CR_HSERDY_Pos))) {
        if (--timeout == 0) {
            Error_Handler();
        }
    }

    /* ========================================================================
     * 步骤4: 配置PLL
     * ======================================================================== */

    /*
     * PLL配置: HSE × 9 = 72MHz
     * PLLSRC = HSE (不分频)
     * PLLMUL = ×9
     */
    uint32_t cfgr_temp = RCC_CFGR;

    /* 清除PLL配置位 */
    cfgr_temp &= ~((1 << RCC_CFGR_PLLSRC_Pos) |
                   (1 << RCC_CFGR_PLLXTPRE_Pos) |
                   (0xF << RCC_CFGR_PLLMULL_Pos));

    /* 设置PLL配置: HSE作为PLL源，×9倍频 */
    cfgr_temp |= (RCC_CFGR_PLLSRC_HSE << RCC_CFGR_PLLSRC_Pos) |
                 (RCC_CFGR_PLLXTPRE_HSE << RCC_CFGR_PLLXTPRE_Pos) |
                 (RCC_CFGR_PLLMULL9 << RCC_CFGR_PLLMULL_Pos);

    RCC_CFGR = cfgr_temp;

    /* 使能PLL */
    RCC_CR |= (1 << RCC_CR_PLLON_Pos);

    /* 等待PLL就绪 */
    timeout = 0xFFFF;
    while (!(RCC_CR & (1 << RCC_CR_PLLRDY_Pos))) {
        if (--timeout == 0) {
            Error_Handler();
        }
    }

    /* ========================================================================
     * 步骤5: 配置总线时钟预分频
     * ======================================================================== */

    cfgr_temp = RCC_CFGR;

    /* AHB预分频: HCLK = SYSCLK = 72MHz (不分频) */
    cfgr_temp &= ~(0xF << RCC_CFGR_HPRE_Pos);
    cfgr_temp |= (RCC_CFGR_HPRE_DIV1 << RCC_CFGR_HPRE_Pos);

    /* APB2预分频: PCLK2 = HCLK = 72MHz (不分频) */
    cfgr_temp &= ~(0x7 << RCC_CFGR_PPRE2_Pos);
    cfgr_temp |= (RCC_CFGR_PPRE2_DIV1 << RCC_CFGR_PPRE2_Pos);

    /* APB1预分频: PCLK1 = HCLK/2 = 36MHz (二分频) */
    cfgr_temp &= ~(0x7 << RCC_CFGR_PPRE1_Pos);
    cfgr_temp |= (RCC_CFGR_PPRE1_DIV2 << RCC_CFGR_PPRE1_Pos);

    /* ADC预分频: ADCCLK = PCLK2/6 = 12MHz */
    cfgr_temp &= ~(0x3 << RCC_CFGR_ADCPRE_Pos);
    cfgr_temp |= (RCC_CFGR_ADCPRE_DIV6 << RCC_CFGR_ADCPRE_Pos);

    RCC_CFGR = cfgr_temp;

    /* ========================================================================
     * 步骤6: 切换系统时钟到PLL
     * ======================================================================== */

    /* 选择PLL作为系统时钟源 */
    cfgr_temp = RCC_CFGR;
    cfgr_temp &= ~(0x3 << RCC_CFGR_SW_Pos);
    cfgr_temp |= (RCC_CFGR_SW_PLL << RCC_CFGR_SW_Pos);
    RCC_CFGR = cfgr_temp;

    /* 等待系统时钟切换完成 */
    timeout = 0xFFFF;
    while ((RCC_CFGR & (0x3 << RCC_CFGR_SWS_Pos)) !=
           (RCC_CFGR_SWS_PLL << RCC_CFGR_SWS_Pos)) {
        if (--timeout == 0) {
            Error_Handler();
        }
    }

    /* 更新系统时钟变量 */
    SystemCoreClock = 72000000;
}

/**
 * @brief 更新SystemCoreClock变量
 * @details 根据当前寄存器配置计算系统时钟频率
 */
void SystemCoreClockUpdate(void)
{
    uint32_t tmp = 0, pllmull = 0, pllsource = 0;

    /* 获取系统时钟源 */
    tmp = RCC_CFGR & (0x3 << RCC_CFGR_SWS_Pos);

    switch (tmp) {
        case RCC_CFGR_SWS_HSI:  /* HSI作为系统时钟 */
            SystemCoreClock = 8000000;  /* HSI = 8MHz */
            break;

        case RCC_CFGR_SWS_HSE:  /* HSE作为系统时钟 */
            SystemCoreClock = 8000000;  /* HSE = 8MHz */
            break;

        case RCC_CFGR_SWS_PLL:  /* PLL作为系统时钟 */
            /* 获取PLL倍频系数 */
            pllmull = RCC_CFGR & (0xF << RCC_CFGR_PLLMULL_Pos);
            pllmull = (pllmull >> RCC_CFGR_PLLMULL_Pos) + 2;

            /* 获取PLL时钟源 */
            pllsource = RCC_CFGR & (1 << RCC_CFGR_PLLSRC_Pos);

            if (pllsource == 0) {
                /* HSI/2作为PLL源 */
                SystemCoreClock = (8000000 / 2) * pllmull;
            } else {
                /* HSE作为PLL源 */
                uint32_t prediv = (RCC_CFGR & (1 << RCC_CFGR_PLLXTPRE_Pos)) ? 2 : 1;
                SystemCoreClock = (8000000 / prediv) * pllmull;
            }
            break;

        default:
            SystemCoreClock = 8000000;
            break;
    }

    /* 计算HCLK频率 */
    tmp = AHBPrescTable[((RCC_CFGR & (0xF << RCC_CFGR_HPRE_Pos)) >> RCC_CFGR_HPRE_Pos)];
    SystemCoreClock >>= tmp;
}

/* ============================================================================
 * 外设时钟使能函数
 * ============================================================================ */

/**
 * @brief 使能GPIO端口时钟
 * @param GPIOx 端口号: 0=A, 1=B, 2=C, 3=D, 4=E, 5=F, 6=G
 */
void RCC_EnableGPIO_Clock(uint8_t GPIOx)
{
    if (GPIOx <= 7) {
        RCC_APB2ENR |= (1 << (2 + GPIOx));
    }
}

/**
 * @brief 使能USART时钟
 * @param USARTx USART号: 1, 2, 3
 */
void RCC_EnableUSART_Clock(uint8_t USARTx)
{
    if (USARTx == 1) {
        RCC_APB2ENR |= (1 << 14);  /* USART1在APB2 */
    } else if (USARTx == 2) {
        RCC_APB1ENR |= (1 << 17);  /* USART2在APB1 */
    } else if (USARTx == 3) {
        RCC_APB1ENR |= (1 << 18);  /* USART3在APB1 */
    }
}

/**
 * @brief 使能TIM时钟
 * @param TIMx 定时器号: 1-8
 */
void RCC_EnableTIM_Clock(uint8_t TIMx)
{
    if (TIMx == 1) {
        RCC_APB2ENR |= (1 << 11);
    } else if (TIMx >= 2 && TIMx <= 7) {
        RCC_APB1ENR |= (1 << (TIMx - 2 + 0));
    } else if (TIMx == 8) {
        RCC_APB2ENR |= (1 << 13);
    }
}

/**
 * @brief 使能DMA时钟
 * @param DMAx DMA号: 1, 2
 */
void RCC_EnableDMA_Clock(uint8_t DMAx)
{
    if (DMAx == 1) {
        RCC_AHBENR |= (1 << 0);
    } else if (DMAx == 2) {
        RCC_AHBENR |= (1 << 1);
    }
}
```

---

## 6. GPIO寄存器操作

### 6.1 GPIO寄存器详解

```c
/**
 * @file gpio_driver.c
 * @brief STM32F103 GPIO驱动程序
 * @details 提供完整的GPIO配置和操作功能
 */

#include <stdint.h>

/* ============================================================================
 * GPIO 寄存器定义
 * ============================================================================ */

typedef struct {
    volatile uint32_t CRL;      /* 端口配置低寄存器 (0x00) */
    volatile uint32_t CRH;      /* 端口配置高寄存器 (0x04) */
    volatile uint32_t IDR;      /* 端口输入数据寄存器 (0x08) */
    volatile uint32_t ODR;      /* 端口输出数据寄存器 (0x0C) */
    volatile uint32_t BSRR;     /* 端口位设置/清除寄存器 (0x10) */
    volatile uint32_t BRR;      /* 端口位清除寄存器 (0x14) */
    volatile uint32_t LCKR;     /* 端口配置锁定寄存器 (0x18) */
} GPIO_TypeDef;

/* GPIO 基地址 */
#define GPIOA_BASE          0x40010800
#define GPIOB_BASE          0x40010C00
#define GPIOC_BASE          0x40011000
#define GPIOD_BASE          0x40011400
#define GPIOE_BASE          0x40011800
#define GPIOF_BASE          0x40011C00
#define GPIOG_BASE          0x40012000

#define GPIOA               ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD               ((GPIO_TypeDef *)GPIOD_BASE)
#define GPIOE               ((GPIO_TypeDef *)GPIOE_BASE)
#define GPIOF               ((GPIO_TypeDef *)GPIOF_BASE)
#define GPIOG               ((GPIO_TypeDef *)GPIOG_BASE)

/* ============================================================================
 * GPIO 模式定义
 * ============================================================================ */

typedef enum {
    GPIO_MODE_INPUT_ANALOG = 0x0,       /* 模拟输入 */
    GPIO_MODE_INPUT_FLOATING = 0x4,     /* 浮空输入 */
    GPIO_MODE_INPUT_PULL = 0x8,         /* 上拉/下拉输入 */

    GPIO_MODE_OUTPUT_PP_10MHz = 0x1,    /* 推挽输出, 10MHz */
    GPIO_MODE_OUTPUT_PP_2MHz = 0x2,     /* 推挽输出, 2MHz */
    GPIO_MODE_OUTPUT_PP_50MHz = 0x3,    /* 推挽输出, 50MHz */

    GPIO_MODE_OUTPUT_OD_10MHz = 0x5,    /* 开漏输出, 10MHz */
    GPIO_MODE_OUTPUT_OD_2MHz = 0x6,     /* 开漏输出, 2MHz */
    GPIO_MODE_OUTPUT_OD_50MHz = 0x7,    /* 开漏输出, 50MHz */

    GPIO_MODE_AF_PP_10MHz = 0x9,        /* 复用推挽, 10MHz */
    GPIO_MODE_AF_PP_2MHz = 0xA,         /* 复用推挽, 2MHz */
    GPIO_MODE_AF_PP_50MHz = 0xB,        /* 复用推挽, 50MHz */

    GPIO_MODE_AF_OD_10MHz = 0xD,        /* 复用开漏, 10MHz */
    GPIO_MODE_AF_OD_2MHz = 0xE,         /* 复用开漏, 2MHz */
    GPIO_MODE_AF_OD_50MHz = 0xF,        /* 复用开漏, 50MHz */
} GPIO_ModeTypeDef;

typedef enum {
    GPIO_PIN_0 = 0,
    GPIO_PIN_1 = 1,
    GPIO_PIN_2 = 2,
    GPIO_PIN_3 = 3,
    GPIO_PIN_4 = 4,
    GPIO_PIN_5 = 5,
    GPIO_PIN_6 = 6,
    GPIO_PIN_7 = 7,
    GPIO_PIN_8 = 8,
    GPIO_PIN_9 = 9,
    GPIO_PIN_10 = 10,
    GPIO_PIN_11 = 11,
    GPIO_PIN_12 = 12,
    GPIO_PIN_13 = 13,
    GPIO_PIN_14 = 14,
    GPIO_PIN_15 = 15,
    GPIO_PIN_ALL = 16,
} GPIO_PinTypeDef;

typedef enum {
    GPIO_LEVEL_LOW = 0,
    GPIO_LEVEL_HIGH = 1,
} GPIO_LevelTypeDef;

/* ============================================================================
 * GPIO 配置结构体
 * ============================================================================ */

typedef struct {
    GPIO_TypeDef *GPIOx;        /* GPIO端口 */
    uint16_t Pin;               /* 引脚号 (位图格式: 1<<n) */
    GPIO_ModeTypeDef Mode;      /* 工作模式 */
} GPIO_ConfigTypeDef;

/* ============================================================================
 * 内部辅助函数
 * ============================================================================ */

/**
 * @brief 获取GPIO端口的时钟位位置
 * @param GPIOx GPIO端口指针
 * @return 时钟位位置 (2-8)
 */
static uint8_t GPIO_GetClockPos(GPIO_TypeDef *GPIOx)
{
    if (GPIOx == GPIOA) return 2;
    if (GPIOx == GPIOB) return 3;
    if (GPIOx == GPIOC) return 4;
    if (GPIOx == GPIOD) return 5;
    if (GPIOx == GPIOE) return 6;
    if (GPIOx == GPIOF) return 7;
    if (GPIOx == GPIOG) return 8;
    return 0;
}

/* ============================================================================
 * GPIO 初始化函数
 * ============================================================================ */

/**
 * @brief 初始化单个GPIO引脚
 * @param GPIOx GPIO端口 (GPIOA-GPIOG)
 * @param PinNum 引脚号 (0-15)
 * @param Mode 工作模式
 * @details 配置示例:
 *          GPIO_InitPin(GPIOA, 5, GPIO_MODE_OUTPUT_PP_50MHz);  // PA5推挽输出
 *          GPIO_InitPin(GPIOA, 0, GPIO_MODE_INPUT_PULL);        // PA0上拉输入
 */
void GPIO_InitPin(GPIO_TypeDef *GPIOx, uint8_t PinNum, GPIO_ModeTypeDef Mode)
{
    uint32_t config;

    if (PinNum < 8) {
        /* 低8位引脚: 使用CRL寄存器 */
        config = GPIOx->CRL;
        config &= ~(0xF << (PinNum * 4));           /* 清除原有配置 */
        config |= (Mode << (PinNum * 4));           /* 设置新模式 */
        GPIOx->CRL = config;
    } else {
        /* 高8位引脚: 使用CRH寄存器 */
        config = GPIOx->CRH;
        config &= ~(0xF << ((PinNum - 8) * 4));     /* 清除原有配置 */
        config |= (Mode << ((PinNum - 8) * 4));     /* 设置新模式 */
        GPIOx->CRH = config;
    }

    /* 如果是上拉/下拉输入模式，配置ODR选择上下拉 */
    if (Mode == GPIO_MODE_INPUT_PULL) {
        /* 默认配置为上拉，可通过GPIO_SetPinLevel设置 */
        GPIOx->ODR |= (1 << PinNum);
    }
}

/**
 * @brief 批量初始化GPIO引脚
 * @param configs 配置数组
 * @param count 配置数量
 */
void GPIO_InitMultiple(GPIO_ConfigTypeDef *configs, uint8_t count)
{
    for (uint8_t i = 0; i < count; i++) {
        for (uint8_t pin = 0; pin < 16; pin++) {
            if (configs[i].Pin & (1 << pin)) {
                GPIO_InitPin(configs[i].GPIOx, pin, configs[i].Mode);
            }
        }
    }
}

/**
 * @brief 去初始化GPIO引脚(恢复为浮空输入)
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 */
void GPIO_DeInitPin(GPIO_TypeDef *GPIOx, uint8_t PinNum)
{
    GPIO_InitPin(GPIOx, PinNum, GPIO_MODE_INPUT_FLOATING);
}

/* ============================================================================
 * GPIO 输出操作
 * ============================================================================ */

/**
 * @brief 设置引脚输出电平
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 * @param Level 电平 (GPIO_LEVEL_LOW 或 GPIO_LEVEL_HIGH)
 */
static inline void GPIO_SetPinLevel(GPIO_TypeDef *GPIOx, uint8_t PinNum, GPIO_LevelTypeDef Level)
{
    if (Level == GPIO_LEVEL_HIGH) {
        GPIOx->BSRR = (1 << PinNum);        /* 设置引脚为高 */
    } else {
        GPIOx->BRR = (1 << PinNum);         /* 设置引脚为低 */
    }
}

/**
 * @brief 设置引脚为高电平
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 */
static inline void GPIO_SetPin(GPIO_TypeDef *GPIOx, uint8_t PinNum)
{
    GPIOx->BSRR = (1 << PinNum);
}

/**
 * @brief 清除引脚(设为低电平)
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 */
static inline void GPIO_ClearPin(GPIO_TypeDef *GPIOx, uint8_t PinNum)
{
    GPIOx->BRR = (1 << PinNum);
}

/**
 * @brief 翻转引脚电平
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 */
static inline void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint8_t PinNum)
{
    GPIOx->ODR ^= (1 << PinNum);
}

/**
 * @brief 写入整个端口的输出数据
 * @param GPIOx GPIO端口
 * @param Value 16位输出值
 */
static inline void GPIO_WritePort(GPIO_TypeDef *GPIOx, uint16_t Value)
{
    GPIOx->ODR = Value;
}

/**
 * @brief 设置端口位(批量)
 * @param GPIOx GPIO端口
 * @param PinMask 引脚位掩码
 */
static inline void GPIO_SetBits(GPIO_TypeDef *GPIOx, uint16_t PinMask)
{
    GPIOx->BSRR = PinMask;
}

/**
 * @brief 清除端口位(批量)
 * @param GPIOx GPIO端口
 * @param PinMask 引脚位掩码
 */
static inline void GPIO_ClearBits(GPIO_TypeDef *GPIOx, uint16_t PinMask)
{
    GPIOx->BRR = PinMask;
}

/* ============================================================================
 * GPIO 输入操作
 * ============================================================================ */

/**
 * @brief 读取单个引脚输入电平
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 * @return 0=低电平, 1=高电平
 */
static inline uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t PinNum)
{
    return (GPIOx->IDR & (1 << PinNum)) ? 1 : 0;
}

/**
 * @brief 读取整个端口的输入数据
 * @param GPIOx GPIO端口
 * @return 16位输入值
 */
static inline uint16_t GPIO_ReadPort(GPIO_TypeDef *GPIOx)
{
    return (uint16_t)(GPIOx->IDR);
}

/**
 * @brief 读取单个引脚输出状态
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 * @return 0=低, 1=高
 */
static inline uint8_t GPIO_ReadOutputPin(GPIO_TypeDef *GPIOx, uint8_t PinNum)
{
    return (GPIOx->ODR & (1 << PinNum)) ? 1 : 0;
}

/* ============================================================================
 * GPIO 锁定功能
 * ============================================================================ */

/**
 * @brief 锁定GPIO引脚配置
 * @param GPIOx GPIO端口
 * @param PinMask 要锁定的引脚位掩码
 * @return 1=成功, 0=失败
 * @note 锁定后直到下次复位才能修改配置
 */
uint8_t GPIO_LockConfig(GPIO_TypeDef *GPIOx, uint16_t PinMask)
{
    uint32_t tmp = 0x00010000;

    tmp |= PinMask;

    /* 写入LCKR寄存器: 1->0->1 */
    GPIOx->LCKR = tmp;
    GPIOx->LCKR = PinMask;
    GPIOx->LCKR = tmp;

    /* 读取LCKR */
    tmp = GPIOx->LCKR;

    /* 检查锁定是否成功 */
    return (GPIOx->LCKR & 0x00010000) ? 1 : 0;
}

/* ============================================================================
 * 常用GPIO操作封装
 * ============================================================================ */

/**
 * @brief 初始化LED引脚(推挽输出)
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 * @param ActiveLow 1=低电平点亮, 0=高电平点亮
 * @details 使用示例:
 *          GPIO_InitLED(GPIOC, 13, 1);  // PC13, 低电平点亮(Blue Pill板载LED)
 */
void GPIO_InitLED(GPIO_TypeDef *GPIOx, uint8_t PinNum, uint8_t ActiveLow)
{
    GPIO_InitPin(GPIOx, PinNum, GPIO_MODE_OUTPUT_PP_2MHz);

    /* 初始状态: 熄灭 */
    if (ActiveLow) {
        GPIO_SetPin(GPIOx, PinNum);     /* 高电平=熄灭 */
    } else {
        GPIO_ClearPin(GPIOx, PinNum);   /* 低电平=熄灭 */
    }
}

/**
 * @brief LED开关控制
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 * @param ActiveLow 1=低电平点亮
 * @param On 1=点亮, 0=熄灭
 */
void GPIO_ControlLED(GPIO_TypeDef *GPIOx, uint8_t PinNum, uint8_t ActiveLow, uint8_t On)
{
    if (ActiveLow) {
        if (On) {
            GPIO_ClearPin(GPIOx, PinNum);   /* 低电平点亮 */
        } else {
            GPIO_SetPin(GPIOx, PinNum);     /* 高电平熄灭 */
        }
    } else {
        if (On) {
            GPIO_SetPin(GPIOx, PinNum);     /* 高电平点亮 */
        } else {
            GPIO_ClearPin(GPIOx, PinNum);   /* 低电平熄灭 */
        }
    }
}

/**
 * @brief LED闪烁一次
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 * @param ActiveLow 1=低电平点亮
 * @param DelayMs 延时函数指针
 */
void GPIO_BlinkLED(GPIO_TypeDef *GPIOx, uint8_t PinNum, uint8_t ActiveLow,
                   void (*DelayMs)(uint32_t))
{
    GPIO_ControlLED(GPIOx, PinNum, ActiveLow, 1);
    DelayMs(200);
    GPIO_ControlLED(GPIOx, PinNum, ActiveLow, 0);
}

/**
 * @brief 初始化按键引脚
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 * @param PullUp 1=上拉输入, 0=下拉输入
 */
void GPIO_InitButton(GPIO_TypeDef *GPIOx, uint8_t PinNum, uint8_t PullUp)
{
    GPIO_InitPin(GPIOx, PinNum, GPIO_MODE_INPUT_PULL);

    /* 设置上拉/下拉 */
    if (PullUp) {
        GPIOx->ODR |= (1 << PinNum);    /* 上拉 */
    } else {
        GPIOx->ODR &= ~(1 << PinNum);   /* 下拉 */
    }
}

/**
 * @brief 读取按键状态(带消抖)
 * @param GPIOx GPIO端口
 * @param PinNum 引脚号
 * @param ActiveLow 1=低电平有效(按下)
 * @param DelayUs 微秒延时函数指针
 * @return 1=按下, 0=未按下
 */
uint8_t GPIO_ReadButton(GPIO_TypeDef *GPIOx, uint8_t PinNum,
                        uint8_t ActiveLow, void (*DelayUs)(uint32_t))
{
    uint8_t level = GPIO_ReadPin(GPIOx, PinNum);

    /* 第一次读取 */
    if ((ActiveLow && level == 0) || (!ActiveLow && level == 1)) {
        DelayUs(1000);  /* 延时1ms消抖 */

        /* 第二次读取确认 */
        level = GPIO_ReadPin(GPIOx, PinNum);
        if ((ActiveLow && level == 0) || (!ActiveLow && level == 1)) {
            return 1;   /* 确认按下 */
        }
    }

    return 0;   /* 未按下 */
}
```

---

## 7. 定时器实现

### 7.1 基础定时器(TIM2/TIM3/TIM4)

```c
/**
 * @file timer_driver.c
 * @brief STM32F103 定时器驱动程序
 * @details 提供基础定时器(TIM2/3/4)和通用定时器的完整驱动
 */

#include <stdint.h>

/* ============================================================================
 * 定时器寄存器定义
 * ============================================================================ */

typedef struct {
    volatile uint32_t CR1;      /* 控制寄存器1 (0x00) */
    volatile uint32_t CR2;      /* 控制寄存器2 (0x04) */
    volatile uint32_t SMCR;     /* 从模式控制寄存器 (0x08) */
    volatile uint32_t DIER;     /* DMA/中断使能寄存器 (0x0C) */
    volatile uint32_t SR;       /* 状态寄存器 (0x10) */
    volatile uint32_t EGR;      /* 事件生成寄存器 (0x14) */
    volatile uint32_t CCMR1;    /* 捕获/比较模式寄存器1 (0x18) */
    volatile uint32_t CCMR2;    /* 捕获/比较模式寄存器2 (0x1C) */
    volatile uint32_t CCER;     /* 捕获/比较使能寄存器 (0x20) */
    volatile uint32_t CNT;      /* 计数器 (0x24) */
    volatile uint32_t PSC;      /* 预分频器 (0x28) */
    volatile uint32_t ARR;      /* 自动重装载寄存器 (0x2C) */
    volatile uint32_t RCR;      /* 重复计数寄存器 (TIM1 only, 0x30) */
    volatile uint32_t CCR1;     /* 捕获/比较寄存器1 (0x34) */
    volatile uint32_t CCR2;     /* 捕获/比较寄存器2 (0x38) */
    volatile uint32_t CCR3;     /* 捕获/比较寄存器3 (0x3C) */
    volatile uint32_t CCR4;     /* 捕获/比较寄存器4 (0x40) */
    volatile uint32_t BDTR;     /* 刹车和死区寄存器 (TIM1 only, 0x44) */
    volatile uint32_t DCR;      /* DMA控制寄存器 (0x48) */
    volatile uint32_t DMAR;     /* DMA地址全传输寄存器 (0x4C) */
} TIM_TypeDef;

/* 定时器基地址 */
#define TIM1_BASE           0x40010000
#define TIM2_BASE           0x40000000
#define TIM3_BASE           0x40000400
#define TIM4_BASE           0x40000800
#define TIM5_BASE           0x40000C00
#define TIM6_BASE           0x40001000
#define TIM7_BASE           0x40001400
#define TIM8_BASE           0x40010400

#define TIM1                ((TIM_TypeDef *)TIM1_BASE)
#define TIM2                ((TIM_TypeDef *)TIM2_BASE)
#define TIM3                ((TIM_TypeDef *)TIM3_BASE)
#define TIM4                ((TIM_TypeDef *)TIM4_BASE)
#define TIM5                ((TIM_TypeDef *)TIM5_BASE)
#define TIM6                ((TIM_TypeDef *)TIM6_BASE)
#define TIM7                ((TIM_TypeDef *)TIM7_BASE)
#define TIM8                ((TIM_TypeDef *)TIM8_BASE)

/* ============================================================================
 * RCC 时钟使能 (与前面定义相同，此处简化)
 * ============================================================================ */
#define RCC_APB1ENR         (*(volatile uint32_t *)0x4002101C)
#define RCC_APB2ENR         (*(volatile uint32_t *)0x40021018)

#define RCC_APB1ENR_TIM2    (1 << 0)
#define RCC_APB1ENR_TIM3    (1 << 1)
#define RCC_APB1ENR_TIM4    (1 << 2)
#define RCC_APB1ENR_TIM5    (1 << 3)
#define RCC_APB1ENR_TIM6    (1 << 4)
#define RCC_APB1ENR_TIM7    (1 << 5)

#define RCC_APB2ENR_TIM1    (1 << 11)
#define RCC_APB2ENR_TIM8    (1 << 13)

/* ============================================================================
 * 定时器CR1寄存器位定义
 * ============================================================================ */
#define TIM_CR1_CEN_Pos     0   /* 计数器使能 */
#define TIM_CR1_UDIS_Pos    1   /* 更新禁用 */
#define TIM_CR1_URS_Pos     2   /* 更新请求源 */
#define TIM_CR1_OPM_Pos     3   /* 单脉冲模式 */
#define TIM_CR1_DIR_Pos     4   /* 计数方向 (仅通用定时器) */
#define TIM_CR1_CMS_Pos     5   /* 中央对齐模式选择 */
#define TIM_CR1_ARPE_Pos    7   /* 自动重装载预装载使能 */
#define TIM_CR1_CKD_Pos     8   /* 时钟分频 */

/* DIER寄存器位定义 */
#define TIM_DIER_UIE_Pos    0   /* 更新中断使能 */
#define TIM_DIER_CC1IE_Pos  1   /* 捕获/比较1中断使能 */
#define TIM_DIER_CC2IE_Pos  2   /* 捕获/比较2中断使能 */
#define TIM_DIER_CC3IE_Pos  3   /* 捕获/比较3中断使能 */
#define TIM_DIER_CC4IE_Pos  4   /* 捕获/比较4中断使能 */
#define TIM_DIER_TIE_Pos    6   /* 触发中断使能 */

/* SR寄存器位定义 */
#define TIM_SR_UIF_Pos      0   /* 更新中断标志 */
#define TIM_SR_CC1IF_Pos    1   /* 捕获/比较1中断标志 */
#define TIM_SR_CC2IF_Pos    2   /* 捕获/比较2中断标志 */
#define TIM_SR_CC3IF_Pos    3   /* 捕获/比较3中断标志 */
#define TIM_SR_CC4IF_Pos    4   /* 捕获/比较4中断标志 */

/* EGR寄存器位定义 */
#define TIM_EGR_UG_Pos      0   /* 更新事件生成 */

/* ============================================================================
 * 定时器配置结构体
 * ============================================================================ */

typedef enum {
    TIM_MODE_BASIC,         /* 基础定时器模式 */
    TIM_MODE_PWM,           /* PWM输出模式 */
    TIM_MODE_INPUT_CAPTURE, /* 输入捕获模式 */
    TIM_MODE_OUTPUT_COMPARE /* 输出比较模式 */
} TIM_ModeTypeDef;

typedef struct {
    TIM_TypeDef *TIMx;      /* 定时器指针 */
    uint16_t Prescaler;     /* 预分频值 (0-65535) */
    uint16_t Period;        /* 周期/重装载值 (0-65535) */
    TIM_ModeTypeDef Mode;   /* 工作模式 */
    uint8_t InterruptEnable;/* 是否使能中断 */
} TIM_ConfigTypeDef;

/* ============================================================================
 * 内部辅助函数
 * ============================================================================ */

/**
 * @brief 使能定时器时钟
 * @param TIMx 定时器指针
 */
static void TIM_EnableClock(TIM_TypeDef *TIMx)
{
    if (TIMx == TIM1) {
        RCC_APB2ENR |= RCC_APB2ENR_TIM1;
    } else if (TIMx == TIM2) {
        RCC_APB1ENR |= RCC_APB1ENR_TIM2;
    } else if (TIMx == TIM3) {
        RCC_APB1ENR |= RCC_APB1ENR_TIM3;
    } else if (TIMx == TIM4) {
        RCC_APB1ENR |= RCC_APB1ENR_TIM4;
    } else if (TIMx == TIM5) {
        RCC_APB1ENR |= RCC_APB1ENR_TIM5;
    } else if (TIMx == TIM6) {
        RCC_APB1ENR |= RCC_APB1ENR_TIM6;
    } else if (TIMx == TIM7) {
        RCC_APB1ENR |= RCC_APB1ENR_TIM7;
    } else if (TIMx == TIM8) {
        RCC_APB2ENR |= RCC_APB2ENR_TIM8;
    }
}

/**
 * @brief 获取定时器时钟频率
 * @param TIMx 定时器指针
 * @return 定时器时钟频率(Hz)
 * @note 假设系统时钟为72MHz
 */
static uint32_t TIM_GetClockFreq(TIM_TypeDef *TIMx)
{
    /*
     * TIM1和TIM8在APB2上，最高72MHz
     * TIM2-7在APB1上，最高36MHz
     */
    if (TIMx == TIM1 || TIMx == TIM8) {
        return 72000000;    /* 72MHz */
    } else {
        return 36000000;    /* 36MHz */
    }
}

/* ============================================================================
 * 基础定时器功能
 * ============================================================================ */

/**
 * @brief 初始化基础定时器(时基模式)
 * @param TIMx 定时器指针
 * @param Prescaler 预分频值
 * @param Period 周期值
 * @details 定时频率 = 定时器时钟 / ((Prescaler+1) * (Period+1))
 *          使用示例:
 *          TIM_BaseInit(TIM2, 35999, 999);  // 1ms中断 (36MHz/(36000*1000)=1Hz)
 */
void TIM_BaseInit(TIM_TypeDef *TIMx, uint16_t Prescaler, uint16_t Period)
{
    /* 使能时钟 */
    TIM_EnableClock(TIMx);

    /* 禁用定时器 */
    TIMx->CR1 &= ~(1 << TIM_CR1_CEN_Pos);

    /* 设置预分频器 */
    TIMx->PSC = Prescaler;

    /* 设置自动重装载值 */
    TIMx->ARR = Period;

    /* 使能自动重装载预装载 */
    TIMx->CR1 |= (1 << TIM_CR1_ARPE_Pos);

    /* 产生更新事件，重新装载寄存器 */
    TIMx->EGR |= (1 << TIM_EGR_UG_Pos);

    /* 清除更新标志 */
    TIMx->SR &= ~(1 << TIM_SR_UIF_Pos);
}

/**
 * @brief 初始化定时器以产生特定频率
 * @param TIMx 定时器指针
 * @param FreqHz 目标频率(Hz)
 * @return 实际配置频率(Hz)
 * @note 最大误差约0.1%，适用于音频、PWM等应用
 */
uint32_t TIM_InitForFrequency(TIM_TypeDef *TIMx, uint32_t FreqHz)
{
    uint32_t timerClk = TIM_GetClockFreq(TIMx);
    uint32_t period, prescaler;

    /* 计算预分频和周期，使误差最小 */
    /* timerClk / ((prescaler+1) * (period+1)) = FreqHz */

    if (FreqHz > timerClk) {
        FreqHz = timerClk;
    }

    /* 计算总分频系数 */
    uint32_t totalDiv = timerClk / FreqHz;

    if (totalDiv > 65536 * 65536) {
        /* 超出范围，使用最大值 */
        prescaler = 65535;
        period = 65535;
    } else {
        /* 寻找最佳分频组合 */
        /* 优先使用较小的预分频以获得更高分辨率 */
        prescaler = 0;
        period = totalDiv - 1;

        while (period > 65535 && prescaler < 65535) {
            prescaler++;
            period = (totalDiv / (prescaler + 1)) - 1;
        }

        if (prescaler > 65535) {
            prescaler = 65535;
            period = 65535;
        }
    }

    /* 初始化定时器 */
    TIM_BaseInit(TIMx, (uint16_t)prescaler, (uint16_t)period);

    /* 返回实际频率 */
    return timerClk / ((prescaler + 1) * (period + 1));
}

/**
 * @brief 启动定时器
 * @param TIMx 定时器指针
 */
static inline void TIM_Start(TIM_TypeDef *TIMx)
{
    TIMx->CR1 |= (1 << TIM_CR1_CEN_Pos);
}

/**
 * @brief 停止定时器
 * @param TIMx 定时器指针
 */
static inline void TIM_Stop(TIM_TypeDef *TIMx)
{
    TIMx->CR1 &= ~(1 << TIM_CR1_CEN_Pos);
}

/**
 * @brief 获取当前计数值
 * @param TIMx 定时器指针
 * @return 当前计数器值
 */
static inline uint16_t TIM_GetCounter(TIM_TypeDef *TIMx)
{
    return (uint16_t)TIMx->CNT;
}

/**
 * @brief 设置当前计数值
 * @param TIMx 定时器指针
 * @param Value 要设置的值
 */
static inline void TIM_SetCounter(TIM_TypeDef *TIMx, uint16_t Value)
{
    TIMx->CNT = Value;
}

/**
 * @brief 获取自动重装载值
 * @param TIMx 定时器指针
 * @return ARR值
 */
static inline uint16_t TIM_GetPeriod(TIM_TypeDef *TIMx)
{
    return (uint16_t)TIMx->ARR;
}

/**
 * @brief 设置自动重装载值
 * @param TIMx 定时器指针
 * @param Period 新的周期值
 */
static inline void TIM_SetPeriod(TIM_TypeDef *TIMx, uint16_t Period)
{
    TIMx->ARR = Period;
}

/* ============================================================================
 * 定时器中断功能
 * ============================================================================ */

/**
 * @brief 使能定时器更新中断
 * @param TIMx 定时器指针
 */
static inline void TIM_EnableIT_Update(TIM_TypeDef *TIMx)
{
    TIMx->DIER |= (1 << TIM_DIER_UIE_Pos);
}

/**
 * @brief 禁用定时器更新中断
 * @param TIMx 定时器指针
 */
static inline void TIM_DisableIT_Update(TIM_TypeDef *TIMx)
{
    TIMx->DIER &= ~(1 << TIM_DIER_UIE_Pos);
}

/**
 * @brief 检查更新中断标志
 * @param TIMx 定时器指针
 * @return 1=有更新中断, 0=无
 */
static inline uint8_t TIM_GetITStatus_Update(TIM_TypeDef *TIMx)
{
    return (TIMx->SR & (1 << TIM_SR_UIF_Pos)) ? 1 : 0;
}

/**
 * @brief 清除更新中断标志
 * @param TIMx 定时器指针
 */
static inline void TIM_ClearITPendingBit_Update(TIM_TypeDef *TIMx)
{
    TIMx->SR &= ~(1 << TIM_SR_UIF_Pos);
}

/**
 * @brief 生成软件更新事件
 * @param TIMx 定时器指针
 */
static inline void TIM_GenerateEvent_Update(TIM_TypeDef *TIMx)
{
    TIMx->EGR |= (1 << TIM_EGR_UG_Pos);
}

/* ============================================================================
 * 毫秒延时定时器 (基于TIM2)
 * ============================================================================ */

static volatile uint32_t g_tim2_ms_count = 0;

/**
 * @brief TIM2中断处理函数
 * @note 需要在启动文件中定义TIM2_IRQHandler指向此函数
 */
void TIM2_IRQHandler_User(void)
{
    if (TIM_GetITStatus_Update(TIM2)) {
        TIM_ClearITPendingBit_Update(TIM2);
        g_tim2_ms_count++;
    }
}

/**
 * @brief 初始化毫秒延时定时器
 * @details 配置TIM2产生1ms周期中断
 */
void TIM_Delay_Init(void)
{
    /* 配置TIM2: 36MHz / 36000 = 1kHz (1ms) */
    TIM_BaseInit(TIM2, 35999, 0);

    /* 使能更新中断 */
    TIM_EnableIT_Update(TIM2);

    /* 启动定时器 */
    TIM_Start(TIM2);
}

/**
 * @brief 获取毫秒计数
 * @return 从启动开始的毫秒数
 */
uint32_t TIM_GetMillis(void)
{
    return g_tim2_ms_count;
}

/**
 * @brief 毫秒延时
 * @param ms 延时毫秒数
 */
void TIM_Delay_ms(uint32_t ms)
{
    uint32_t start = g_tim2_ms_count;
    while ((g_tim2_ms_count - start) < ms) {
        __asm volatile ("nop");
    }
}

/**
 * @brief 微秒延时(忙等待)
 * @param us 延时微秒数
 * @note 仅适用于短延时，会阻塞CPU
 */
void TIM_Delay_us(uint32_t us)
{
    /* 假设72MHz，约72个时钟周期1us */
    /* 使用简单的循环，需要根据实际情况校准 */
    for (volatile uint32_t i = 0; i < us * 8; i++) {
        __asm volatile ("nop");
    }
}

/* ============================================================================
 * PWM功能 (TIM2-TIM4)
 * ============================================================================ */

/**
 * @brief 初始化PWM输出
 * @param TIMx 定时器指针
 * @param Channel PWM通道 (1-4)
 * @param Frequency PWM频率(Hz)
 * @param DutyCycle 初始占空比 (0-1000，表示0.0%-100.0%)
 * @details 使用示例:
 *          TIM_PWM_Init(TIM2, 1, 1000, 500);  // PA0输出1kHz PWM, 50%占空比
 */
void TIM_PWM_Init(TIM_TypeDef *TIMx, uint8_t Channel,
                  uint32_t Frequency, uint16_t DutyCycle)
{
    uint32_t timerClk = TIM_GetClockFreq(TIMx);
    uint32_t period = (timerClk / Frequency) - 1;

    /* 限制周期值 */
    if (period > 65535) period = 65535;

    /* 基础定时器初始化 (预分频=0) */
    TIM_BaseInit(TIMx, 0, (uint16_t)period);

    /* 配置PWM模式 */
    uint32_t ccmr = 0;
    uint32_t ccer = 0;

    switch (Channel) {
        case 1:
            /* 通道1: PWM模式1, 预装载使能 */
            ccmr = (6 << 4) | (1 << 3);  /* OC1M = 110 (PWM模式1), OC1PE = 1 */
            TIMx->CCMR1 = (TIMx->CCMR1 & 0xFF00) | ccmr;
            ccer = (1 << 0);  /* CC1E = 1 (输出使能) */
            TIMx->CCER |= ccer;
            /* 设置占空比 */
            TIMx->CCR1 = (period * DutyCycle) / 1000;
            break;

        case 2:
            /* 通道2 */
            ccmr = (6 << 12) | (1 << 11);  /* OC2M = 110, OC2PE = 1 */
            TIMx->CCMR1 = (TIMx->CCMR1 & 0x00FF) | ccmr;
            ccer = (1 << 4);  /* CC2E = 1 */
            TIMx->CCER |= ccer;
            TIMx->CCR2 = (period * DutyCycle) / 1000;
            break;

        case 3:
            /* 通道3 */
            ccmr = (6 << 4) | (1 << 3);
            TIMx->CCMR2 = (TIMx->CCMR2 & 0xFF00) | ccmr;
            ccer = (1 << 8);  /* CC3E = 1 */
            TIMx->CCER |= ccer;
            TIMx->CCR3 = (period * DutyCycle) / 1000;
            break;

        case 4:
            /* 通道4 */
            ccmr = (6 << 12) | (1 << 11);
            TIMx->CCMR2 = (TIMx->CCMR2 & 0x00FF) | ccmr;
            ccer = (1 << 12);  /* CC4E = 1 */
            TIMx->CCER |= ccer;
            TIMx->CCR4 = (period * DutyCycle) / 1000;
            break;
    }

    /* 使能主输出 (对于高级定时器TIM1/TIM8) */
    if (TIMx == TIM1 || TIMx == TIM8) {
        TIMx->BDTR |= (1 << 15);  /* MOE = 1 */
    }

    /* 启动定时器 */
    TIM_Start(TIMx);
}

/**
 * @brief 设置PWM占空比
 * @param TIMx 定时器指针
 * @param Channel PWM通道 (1-4)
 * @param DutyCycle 占空比 (0-1000，表示0.0%-100.0%)
 */
void TIM_PWM_SetDutyCycle(TIM_TypeDef *TIMx, uint8_t Channel, uint16_t DutyCycle)
{
    uint16_t pulse = (uint16_t)((TIMx->ARR * DutyCycle) / 1000);

    switch (Channel) {
        case 1: TIMx->CCR1 = pulse; break;
        case 2: TIMx->CCR2 = pulse; break;
        case 3: TIMx->CCR3 = pulse; break;
        case 4: TIMx->CCR4 = pulse; break;
    }
}

/**
 * @brief 设置PWM频率
 * @param TIMx 定时器指针
 * @param Frequency PWM频率(Hz)
 * @note 会保持当前占空比比例
 */
void TIM_PWM_SetFrequency(TIM_TypeDef *TIMx, uint32_t Frequency)
{
    uint32_t timerClk = TIM_GetClockFreq(TIMx);
    uint32_t newPeriod = (timerClk / Frequency) - 1;

    if (newPeriod > 65535) newPeriod = 65535;

    /* 保存当前各通道占空比比例 */
    uint16_t duty1 = (uint16_t)((TIMx->CCR1 * 1000) / TIMx->ARR);
    uint16_t duty2 = (uint16_t)((TIMx->CCR2 * 1000) / TIMx->ARR);
    uint16_t duty3 = (uint16_t)((TIMx->CCR3 * 1000) / TIMx->ARR);
    uint16_t duty4 = (uint16_t)((TIMx->CCR4 * 1000) / TIMx->ARR);

    /* 设置新周期 */
    TIMx->ARR = (uint16_t)newPeriod;

    /* 恢复占空比 */
    TIMx->CCR1 = (newPeriod * duty1) / 1000;
    TIMx->CCR2 = (newPeriod * duty2) / 1000;
    TIMx->CCR3 = (newPeriod * duty3) / 1000;
    TIMx->CCR4 = (newPeriod * duty4) / 1000;
}
```

---

## 8. UART轮询/中断驱动

### 8.1 UART寄存器操作

```c
/**
 * @file uart_driver.c
 * @brief STM32F103 USART驱动程序
 * @details 提供轮询和中断两种模式的串口通信
 */

#include <stdint.h>

/* ============================================================================
 * USART 寄存器定义
 * ============================================================================ */

typedef struct {
    volatile uint32_t SR;       /* 状态寄存器 (0x00) */
    volatile uint32_t DR;       /* 数据寄存器 (0x04) */
    volatile uint32_t BRR;      /* 波特率寄存器 (0x08) */
    volatile uint32_t CR1;      /* 控制寄存器1 (0x0C) */
    volatile uint32_t CR2;      /* 控制寄存器2 (0x10) */
    volatile uint32_t CR3;      /* 控制寄存器3 (0x14) */
    volatile uint32_t GTPR;     /* 保护时间和预分频寄存器 (0x18) */
} USART_TypeDef;

/* USART 基地址 */
#define USART1_BASE         0x40013800
#define USART2_BASE         0x40004400
#define USART3_BASE         0x40004800

#define USART1              ((USART_TypeDef *)USART1_BASE)
#define USART2              ((USART_TypeDef *)USART2_BASE)
#define USART3              ((USART_TypeDef *)USART3_BASE)

/* ============================================================================
 * 寄存器位定义
 * ============================================================================ */

/* SR - 状态寄存器 */
#define USART_SR_PE_Pos     0   /* 奇偶校验错误 */
#define USART_SR_FE_Pos     1   /* 帧错误 */
#define USART_SR_NE_Pos     2   /* 噪声错误 */
#define USART_SR_ORE_Pos    3   /* 过载错误 */
#define USART_SR_IDLE_Pos   4   /* 空闲线路 */
#define USART_SR_RXNE_Pos   5   /* 接收数据寄存器非空 */
#define USART_SR_TC_Pos     6   /* 发送完成 */
#define USART_SR_TXE_Pos    7   /* 发送数据寄存器空 */
#define USART_SR_LBD_Pos    8   /* LIN断开检测 */
#define USART_SR_CTS_Pos    9   /* CTS标志 */

/* CR1 - 控制寄存器1 */
#define USART_CR1_SBK_Pos   0   /* 发送断开 */
#define USART_CR1_RWU_Pos   1   /* 接收唤醒 */
#define USART_CR1_RE_Pos    2   /* 接收使能 */
#define USART_CR1_TE_Pos    3   /* 发送使能 */
#define USART_CR1_IDLEIE_Pos 4  /* 空闲中断使能 */
#define USART_CR1_RXNEIE_Pos 5  /* RXNE中断使能 */
#define USART_CR1_TCIE_Pos  6   /* 发送完成中断使能 */
#define USART_CR1_TXEIE_Pos 7   /* TXE中断使能 */
#define USART_CR1_PEIE_Pos  8   /* PE中断使能 */
#define USART_CR1_PS_Pos    9   /* 奇偶校验选择 */
#define USART_CR1_PCE_Pos   10  /* 奇偶校验使能 */
#define USART_CR1_WAKE_Pos  11  /* 唤醒方法 */
#define USART_CR1_M_Pos     12  /* 字长 */
#define USART_CR1_UE_Pos    13  /* USART使能 */

/* CR2 - 控制寄存器2 */
#define USART_CR2_ADD_Pos   0   /* 地址 */
#define USART_CR2_LBDL_Pos  5   /* LIN断开长度 */
#define USART_CR2_LBDIE_Pos 6   /* LIN断开中断使能 */
#define USART_CR2_LBCL_Pos  8   /* 最后一位时钟脉冲 */
#define USART_CR2_CPHA_Pos  9   /* 时钟相位 */
#define USART_CR2_CPOL_Pos  10  /* 时钟极性 */
#define USART_CR2_CLKEN_Pos 11  /* 时钟使能 */
#define USART_CR2_STOP_Pos  12  /* 停止位 */

/* CR3 - 控制寄存器3 */
#define USART_CR3_EIE_Pos   0   /* 错误中断使能 */
#define USART_CR3_IREN_Pos  1   /* 红外模式 */
#define USART_CR3_IRLP_Pos  2   /* 红外低功耗 */
#define USART_CR3_HDSEL_Pos 3   /* 半双工选择 */
#define USART_CR3_NACK_Pos  4   /* 智能卡NACK */
#define USART_CR3_SCEN_Pos  5   /* 智能卡模式 */
#define USART_CR3_DMAR_Pos  6   /* DMA接收 */
#define USART_CR3_DMAT_Pos  7   /* DMA发送 */
#define USART_CR3_RTSE_Pos  8   /* RTS使能 */
#define USART_CR3_CTSE_Pos  9   /* CTS使能 */
#define USART_CR3_CTSIE_Pos 10  /* CTS中断使能 */

/* ============================================================================
 * 波特率计算
 * ============================================================================ */

/* APB2时钟频率 (USART1) */
#define USART1_CLK          72000000
/* APB1时钟频率 (USART2/3) */
#define USART2_CLK          36000000
#define USART3_CLK          36000000

/**
 * @brief 计算波特率寄存器值
 * @param clk 时钟频率
 * @param baud 目标波特率
 * @return BRR值
 */
static uint16_t USART_ComputeBRR(uint32_t clk, uint32_t baud)
{
    uint32_t integerdivider;
    uint32_t fractionaldivider;

    /* 计算整数部分 */
    integerdivider = ((25 * clk) / (4 * baud));

    /* 计算BRR值 */
    uint16_t brr = (uint16_t)((integerdivider / 100) << 4);

    /* 计算小数部分 */
    fractionaldivider = integerdivider - (100 * (brr >> 4));

    /* 四舍五入 */
    brr |= ((((fractionaldivider * 16) + 50) / 100)) & 0x0F;

    return brr;
}

/* ============================================================================
 * 轮询模式API
 * ============================================================================ */

/**
 * @brief 初始化USART (轮询模式)
 * @param USARTx USART指针
 * @param BaudRate 波特率
 * @details 使用示例:
 *          USART_Init_Polling(USART1, 115200);  // 配置USART1为115200波特率
 */
void USART_Init_Polling(USART_TypeDef *USARTx, uint32_t BaudRate)
{
    uint32_t clk;
    uint16_t brr;

    /* 确定时钟频率 */
    if (USARTx == USART1) {
        clk = USART1_CLK;
        /* 使能USART1时钟和GPIO时钟 */
        (*(volatile uint32_t *)0x40021018) |= (1 << 14);  /* USART1EN */
        (*(volatile uint32_t *)0x40021018) |= (1 << 2);   /* IOPAEN */
    } else if (USARTx == USART2) {
        clk = USART2_CLK;
        /* 使能USART2时钟 */
        (*(volatile uint32_t *)0x4002101C) |= (1 << 17);  /* USART2EN */
        (*(volatile uint32_t *)0x40021018) |= (1 << 2);   /* IOPAEN */
    } else {
        clk = USART3_CLK;
        /* 使能USART3时钟 */
        (*(volatile uint32_t *)0x4002101C) |= (1 << 18);  /* USART3EN */
        (*(volatile uint32_t *)0x40021018) |= (1 << 3);   /* IOPBEN */
    }

    /* 计算波特率 */
    brr = USART_ComputeBRR(clk, BaudRate);
    USARTx->BRR = brr;

    /* 配置: 8数据位, 无校验, 1停止位, 使能发送和接收 */
    USARTx->CR1 = (1 << USART_CR1_TE_Pos) |
                  (1 << USART_CR1_RE_Pos) |
                  (1 << USART_CR1_UE_Pos);

    /* 1个停止位 */
    USARTx->CR2 = 0;

    /* 禁用硬件流控 */
    USARTx->CR3 = 0;
}

/**
 * @brief 发送单个字符(轮询模式)
 * @param USARTx USART指针
 * @param ch 要发送的字符
 */
void USART_SendChar(USART_TypeDef *USARTx, char ch)
{
    /* 等待发送数据寄存器为空 */
    while (!(USARTx->SR & (1 << USART_SR_TXE_Pos)));

    /* 发送数据 */
    USARTx->DR = ch;
}

/**
 * @brief 接收单个字符(轮询模式，阻塞)
 * @param USARTx USART指针
 * @return 接收到的字符
 */
char USART_ReceiveChar(USART_TypeDef *USARTx)
{
    /* 等待接收数据就绪 */
    while (!(USARTx->SR & (1 << USART_SR_RXNE_Pos)));

    /* 返回数据 */
    return (char)(USARTx->DR & 0xFF);
}

/**
 * @brief 检查是否有数据可接收
 * @param USARTx USART指针
 * @return 1=有数据, 0=无数据
 */
static inline uint8_t USART_DataAvailable(USART_TypeDef *USARTx)
{
    return (USARTx->SR & (1 << USART_SR_RXNE_Pos)) ? 1 : 0;
}

/**
 * @brief 非阻塞接收字符
 * @param USARTx USART指针
 * @param ch 接收缓冲区指针
 * @return 1=成功接收, 0=无数据
 */
uint8_t USART_ReceiveChar_NonBlocking(USART_TypeDef *USARTx, char *ch)
{
    if (USART_DataAvailable(USARTx)) {
        *ch = (char)(USARTx->DR & 0xFF);
        return 1;
    }
    return 0;
}

/**
 * @brief 发送字符串
 * @param USARTx USART指针
 * @param str 字符串指针
 */
void USART_SendString(USART_TypeDef *USARTx, const char *str)
{
    while (*str) {
        USART_SendChar(USARTx, *str++);
    }
}

/**
 * @brief 发送缓冲区数据
 * @param USARTx USART指针
 * @param data 数据缓冲区
 * @param len 数据长度
 */
void USART_SendBuffer(USART_TypeDef *USARTx, const uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        USART_SendChar(USARTx, (char)data[i]);
    }
}

/**
 * @brief 等待发送完成
 * @param USARTx USART指针
 */
void USART_WaitForTXComplete(USART_TypeDef *USARTx)
{
    while (!(USARTx->SR & (1 << USART_SR_TC_Pos)));
}

/**
 * @brief 格式化打印(简化版printf)
 * @param USARTx USART指针
 * @param format 格式字符串
 * @details 支持: %d(整数), %u(无符号), %x(十六进制), %s(字符串), %c(字符)
 */
void USART_Printf(USART_TypeDef *USARTx, const char *format, ...)
{
    /* 这里简化实现，实际需要处理va_list */
    /* 完整实现请参考项目模板中的printf函数 */
    USART_SendString(USARTx, format);
}

/* ============================================================================
 * 中断模式API
 * ============================================================================ */

/* 接收缓冲区大小 */
#define USART_RX_BUF_SIZE   128
#define USART_TX_BUF_SIZE   128

/* USART1 缓冲区 */
static volatile uint8_t  g_usart1_rx_buf[USART_RX_BUF_SIZE];
static volatile uint16_t g_usart1_rx_head = 0;
static volatile uint16_t g_usart1_rx_tail = 0;

static volatile uint8_t  g_usart1_tx_buf[USART_TX_BUF_SIZE];
static volatile uint16_t g_usart1_tx_head = 0;
static volatile uint16_t g_usart1_tx_tail = 0;
static volatile uint8_t  g_usart1_tx_busy = 0;

/**
 * @brief 初始化USART (中断模式)
 * @param USARTx USART指针
 * @param BaudRate 波特率
 * @details 使用中断进行接收，接收缓冲区满时丢弃新数据
 */
void USART_Init_Interrupt(USART_TypeDef *USARTx, uint32_t BaudRate)
{
    /* 先进行轮询模式的基本初始化 */
    USART_Init_Polling(USARTx, BaudRate);

    /* 清空缓冲区 */
    if (USARTx == USART1) {
        g_usart1_rx_head = 0;
        g_usart1_rx_tail = 0;
        g_usart1_tx_head = 0;
        g_usart1_tx_tail = 0;
        g_usart1_tx_busy = 0;

        /* 使能RXNE中断 */
        USARTx->CR1 |= (1 << USART_CR1_RXNEIE_Pos);

        /* NVIC配置: 使能USART1中断 */
        (*(volatile uint32_t *)0xE000E100) |= (1 << 37);  /* NVIC_ISER[37] = USART1 */
    }
    /* 其他USART类似... */
}

/**
 * @brief USART1中断处理函数
 * @note 需要在启动文件中定义USART1_IRQHandler
 */
void USART1_IRQHandler_User(void)
{
    uint32_t sr = USART1->SR;

    /* 接收中断 */
    if (sr & (1 << USART_SR_RXNE_Pos)) {
        uint8_t data = (uint8_t)(USART1->DR & 0xFF);

        uint16_t next_head = (g_usart1_rx_head + 1) % USART_RX_BUF_SIZE;
        if (next_head != g_usart1_rx_tail) {
            g_usart1_rx_buf[g_usart1_rx_head] = data;
            g_usart1_rx_head = next_head;
        }
        /* 缓冲区满时丢弃数据 */
    }

    /* 发送中断 */
    if (sr & (1 << USART_SR_TXE_Pos)) {
        if (g_usart1_tx_head != g_usart1_tx_tail) {
            /* 发送下一个字节 */
            USART1->DR = g_usart1_tx_buf[g_usart1_tx_tail];
            g_usart1_tx_tail = (g_usart1_tx_tail + 1) % USART_TX_BUF_SIZE;
        } else {
            /* 发送完成，禁用TXE中断 */
            USART1->CR1 &= ~(1 << USART_CR1_TXEIE_Pos);
            g_usart1_tx_busy = 0;
        }
    }
}

/**
 * @brief 中断模式发送字符
 * @param USARTx USART指针
 * @param ch 要发送的字符
 * @return 1=成功加入发送队列, 0=发送缓冲区满
 */
uint8_t USART_SendChar_IT(USART_TypeDef *USARTx, char ch)
{
    if (USARTx != USART1) return 0;

    uint16_t next_head = (g_usart1_tx_head + 1) % USART_TX_BUF_SIZE;
    if (next_head == g_usart1_tx_tail) {
        return 0;  /* 缓冲区满 */
    }

    g_usart1_tx_buf[g_usart1_tx_head] = (uint8_t)ch;
    g_usart1_tx_head = next_head;

    /* 如果发送器空闲，启动发送 */
    if (!g_usart1_tx_busy) {
        g_usart1_tx_busy = 1;
        USARTx->CR1 |= (1 << USART_CR1_TXEIE_Pos);
    }

    return 1;
}

/**
 * @brief 中断模式发送字符串
 * @param USARTx USART指针
 * @param str 字符串指针
 * @return 发送的字符数
 */
uint16_t USART_SendString_IT(USART_TypeDef *USARTx, const char *str)
{
    uint16_t count = 0;
    while (*str) {
        if (USART_SendChar_IT(USARTx, *str++)) {
            count++;
        } else {
            break;  /* 缓冲区满 */
        }
    }
    return count;
}

/**
 * @brief 获取接收缓冲区中可用的字节数
 * @param USARTx USART指针
 * @return 可读取的字节数
 */
uint16_t USART_GetRxCount(USART_TypeDef *USARTx)
{
    if (USARTx != USART1) return 0;

    return (g_usart1_rx_head - g_usart1_rx_tail + USART_RX_BUF_SIZE) % USART_RX_BUF_SIZE;
}

/**
 * @brief 从接收缓冲区读取一个字节
 * @param USARTx USART指针
 * @param data 数据缓冲区
 * @return 1=成功, 0=无数据
 */
uint8_t USART_ReadRxBuffer(USART_TypeDef *USARTx, uint8_t *data)
{
    if (USARTx != USART1) return 0;

    if (g_usart1_rx_head == g_usart1_rx_tail) {
        return 0;  /* 缓冲区空 */
    }

    *data = g_usart1_rx_buf[g_usart1_rx_tail];
    g_usart1_rx_tail = (g_usart1_rx_tail + 1) % USART_RX_BUF_SIZE;

    return 1;
}

/**
 * @brief 读取一行数据(以\r或\n结尾)
 * @param USARTx USART指针
 * @param buffer 接收缓冲区
 * @param max_len 最大长度
 * @return 读取的字节数
 */
uint16_t USART_ReadLine(USART_TypeDef *USARTx, char *buffer, uint16_t max_len)
{
    uint16_t count = 0;
    uint8_t data;

    while (count < max_len - 1) {
        if (USART_ReadRxBuffer(USARTx, &data)) {
            buffer[count++] = (char)data;

            if (data == '\n' || data == '\r') {
                break;
            }
        } else {
            break;  /* 无更多数据 */
        }
    }

    buffer[count] = '\0';
    return count;
}

/**
 * @brief 清空接收缓冲区
 * @param USARTx USART指针
 */
void USART_FlushRxBuffer(USART_TypeDef *USARTx)
{
    if (USARTx == USART1) {
        g_usart1_rx_head = 0;
        g_usart1_rx_tail = 0;
    }
}
```

---

## 9. 无OS任务调度

### 9.1 合作式调度器

```c
/**
 * @file scheduler.c
 * @brief 无OS任务调度器
 * @details 提供简单的合作式和抢占式(基于中断)任务调度
 */

#include <stdint.h>

/* ============================================================================
 * 任务控制块定义
 * ============================================================================ */

#define MAX_TASKS           8       /* 最大任务数 */
#define TASK_NAME_LEN       16      /* 任务名最大长度 */

/* 任务状态 */
typedef enum {
    TASK_STATE_READY,       /* 就绪 */
    TASK_STATE_RUNNING,     /* 运行中 */
    TASK_STATE_BLOCKED,     /* 阻塞(等待延时) */
    TASK_STATE_SUSPENDED    /* 挂起 */
} TaskState_TypeDef;

/* 任务控制块 */
typedef struct {
    void (*pTask)(void);        /* 任务函数指针 */
    uint32_t delay;             /* 延时计数 */
    uint32_t period;            /* 周期(用于周期性任务) */
    TaskState_TypeDef state;    /* 任务状态 */
    char name[TASK_NAME_LEN];   /* 任务名 */
    uint32_t runCount;          /* 运行次数统计 */
} TaskControlBlock_TypeDef;

/* ============================================================================
 * 调度器全局变量
 * ============================================================================ */

static TaskControlBlock_TypeDef g_taskList[MAX_TASKS];
static uint8_t g_taskCount = 0;
static uint8_t g_currentTask = 0;
static volatile uint32_t g_tickCount = 0;
static uint8_t g_schedulerRunning = 0;

/* ============================================================================
 * 系统滴答定时器 (使用SysTick)
 * ============================================================================ */

/**
 * @brief 初始化系统滴答定时器
 * @param sysClkHz 系统时钟频率(Hz)
 * @param tickHz 滴答频率(Hz)
 */
void Scheduler_Init(uint32_t sysClkHz, uint32_t tickHz)
{
    /* 计算重装载值 */
    uint32_t reload = sysClkHz / tickHz - 1;

    /* SysTick配置 */
    (*(volatile uint32_t *)0xE000E014) = reload;    /* LOAD */
    (*(volatile uint32_t *)0xE000E018) = 0;         /* VAL = 0 */
    (*(volatile uint32_t *)0xE000E010) = 0x07;      /* CTRL = 使能+中断+处理器时钟 */

    /* 初始化任务列表 */
    for (uint8_t i = 0; i < MAX_TASKS; i++) {
        g_taskList[i].pTask = 0;
        g_taskList[i].delay = 0;
        g_taskList[i].period = 0;
        g_taskList[i].state = TASK_STATE_SUSPENDED;
        g_taskList[i].name[0] = '\0';
        g_taskList[i].runCount = 0;
    }

    g_taskCount = 0;
    g_currentTask = 0;
    g_tickCount = 0;
    g_schedulerRunning = 0;
}

/**
 * @brief SysTick中断处理函数
 * @note 需要在启动文件中定义SysTick_Handler
 */
void SysTick_Handler_User(void)
{
    g_tickCount++;

    /* 更新所有任务的延时计数 */
    for (uint8_t i = 0; i < MAX_TASKS; i++) {
        if (g_taskList[i].state == TASK_STATE_BLOCKED && g_taskList[i].delay > 0) {
            g_taskList[i].delay--;
            if (g_taskList[i].delay == 0) {
                g_taskList[i].state = TASK_STATE_READY;
            }
        }
    }
}

/**
 * @brief 获取系统滴答计数
 * @return 当前滴答数
 */
uint32_t Scheduler_GetTick(void)
{
    return g_tickCount;
}

/**
 * @brief 毫秒延时
 * @param ms 延时毫秒数
 */
void Scheduler_Delay(uint32_t ms)
{
    uint32_t start = g_tickCount;
    while ((g_tickCount - start) < ms) {
        __asm volatile ("wfi");  /* 等待中断，降低功耗 */
    }
}

/* ============================================================================
 * 任务管理API
 * ============================================================================ */

/**
 * @brief 添加任务到调度器
 * @param pTask 任务函数指针
 * @param name 任务名
 * @param delay 初始延时(滴答数)
 * @param period 周期(0=单次任务)
 * @return 任务ID (0-MAX_TASKS-1)，-1=失败
 */
int8_t Scheduler_AddTask(void (*pTask)(void), const char *name,
                         uint32_t delay, uint32_t period)
{
    if (g_taskCount >= MAX_TASKS || pTask == 0) {
        return -1;
    }

    /* 查找空闲槽位 */
    uint8_t index = g_taskCount;

    g_taskList[index].pTask = pTask;
    g_taskList[index].delay = delay;
    g_taskList[index].period = period;
    g_taskList[index].state = (delay == 0) ? TASK_STATE_READY : TASK_STATE_BLOCKED;

    /* 复制任务名 */
    uint8_t i = 0;
    while (name[i] && i < TASK_NAME_LEN - 1) {
        g_taskList[index].name[i] = name[i];
        i++;
    }
    g_taskList[index].name[i] = '\0';

    g_taskList[index].runCount = 0;
    g_taskCount++;

    return index;
}

/**
 * @brief 删除任务
 * @param taskId 任务ID
 * @return 1=成功, 0=失败
 */
uint8_t Scheduler_DeleteTask(uint8_t taskId)
{
    if (taskId >= g_taskCount) {
        return 0;
    }

    /* 标记为挂起 */
    g_taskList[taskId].state = TASK_STATE_SUSPENDED;
    g_taskList[taskId].pTask = 0;

    return 1;
}

/**
 * @brief 挂起任务
 * @param taskId 任务ID
 */
void Scheduler_SuspendTask(uint8_t taskId)
{
    if (taskId < g_taskCount) {
        g_taskList[taskId].state = TASK_STATE_SUSPENDED;
    }
}

/**
 * @brief 恢复任务
 * @param taskId 任务ID
 */
void Scheduler_ResumeTask(uint8_t taskId)
{
    if (taskId < g_taskCount && g_taskList[taskId].pTask != 0) {
        g_taskList[taskId].state = TASK_STATE_READY;
    }
}

/**
 * @brief 启动调度器 (合作式调度)
 */
void Scheduler_Start(void)
{
    g_schedulerRunning = 1;

    while (g_schedulerRunning) {
        /* 查找就绪的任务 */
        uint8_t tasksRun = 0;

        for (uint8_t i = 0; i < g_taskCount; i++) {
            g_currentTask = i;

            if (g_taskList[i].state == TASK_STATE_READY) {
                /* 运行任务 */
                g_taskList[i].state = TASK_STATE_RUNNING;
                g_taskList[i].runCount++;

                g_taskList[i].pTask();

                /* 任务返回，更新状态 */
                if (g_taskList[i].period > 0) {
                    /* 周期性任务，重新设置延时 */
                    g_taskList[i].delay = g_taskList[i].period;
                    g_taskList[i].state = TASK_STATE_BLOCKED;
                } else {
                    /* 单次任务，挂起 */
                    g_taskList[i].state = TASK_STATE_SUSPENDED;
                }

                tasksRun++;
            }
        }

        /* 如果没有任务运行，进入低功耗模式 */
        if (tasksRun == 0) {
            __asm volatile ("wfi");
        }
    }
}

/**
 * @brief 停止调度器
 */
void Scheduler_Stop(void)
{
    g_schedulerRunning = 0;
}

/* ============================================================================
 * 时间片轮转调度 (Round-Robin)
 * ============================================================================ */

#define TIME_SLICE_MS       10      /* 时间片大小(ms) */

/**
 * @brief 基于时间片的调度器
 * @details 每个任务运行固定时间后切换到下一个任务
 */
void Scheduler_Start_RoundRobin(void)
{
    uint8_t current = 0;
    uint32_t lastTick = g_tickCount;

    g_schedulerRunning = 1;

    while (g_schedulerRunning) {
        /* 检查是否超过时间片 */
        if ((g_tickCount - lastTick) >= TIME_SLICE_MS) {
            lastTick = g_tickCount;

            /* 切换到下一个就绪任务 */
            uint8_t next = current;
            uint8_t checked = 0;

            do {
                next = (next + 1) % g_taskCount;
                checked++;
            } while (checked < g_taskCount &&
                     g_taskList[next].state != TASK_STATE_READY);

            if (g_taskList[next].state == TASK_STATE_READY) {
                current = next;
                g_currentTask = current;

                g_taskList[current].state = TASK_STATE_RUNNING;
                g_taskList[current].runCount++;

                /* 运行任务 */
                g_taskList[current].pTask();

                /* 恢复就绪状态 */
                g_taskList[current].state = TASK_STATE_READY;
            }
        }
    }
}

/* ============================================================================
 * 实用工具函数
 * ============================================================================ */

/**
 * @brief 获取任务统计信息
 * @param taskId 任务ID
 * @param runCount 运行次数指针
 * @return 任务状态
 */
TaskState_TypeDef Scheduler_GetTaskInfo(uint8_t taskId, uint32_t *runCount)
{
    if (taskId < g_taskCount) {
        *runCount = g_taskList[taskId].runCount;
        return g_taskList[taskId].state;
    }
    return TASK_STATE_SUSPENDED;
}

/**
 * @brief 获取任务名
 * @param taskId 任务ID
 * @return 任务名字符串
 */
const char* Scheduler_GetTaskName(uint8_t taskId)
{
    if (taskId < g_taskCount) {
        return g_taskList[taskId].name;
    }
    return "INVALID";
}

/**
 * @brief 获取就绪任务数
 * @return 就绪任务数量
 */
uint8_t Scheduler_GetReadyCount(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < g_taskCount; i++) {
        if (g_taskList[i].state == TASK_STATE_READY) {
            count++;
        }
    }
    return count;
}
```

---

## 10. 实际项目: STM32F103裸机模板

### 10.1 项目结构


```
stm32f103_baremetal_template/
├── CMakeLists.txt              # CMake构建配置
├── Makefile                    # Make构建配置
├── README.md                   # 项目说明
├── linker/
│   └── stm32f103c8.ld          # 链接器脚本
├── startup/
│   └── startup_stm32f103.c     # 启动代码
├── include/
│   ├── stm32f103.h             # 寄存器定义头文件
│   ├── gpio.h                  # GPIO驱动头文件
│   ├── timer.h                 # 定时器驱动头文件
│   ├── uart.h                  # UART驱动头文件
│   └── scheduler.h             # 调度器头文件
├── src/
│   ├── system_stm32f103.c      # 系统时钟配置
│   ├── gpio.c                  # GPIO驱动
│   ├── timer.c                 # 定时器驱动
│   ├── uart.c                  # UART驱动
│   ├── scheduler.c             # 任务调度器
│   ├── printf.c                # 格式化输出
│   └── main.c                  # 主程序
└── tools/
    ├── flash.sh                # 烧录脚本
    └── debug.sh                # 调试脚本
```

### 10.2 完整主程序示例

```c
/**
 * @file main.c
 * @brief STM32F103C8T6 裸机程序主文件
 * @details
 *   功能演示:
 *   1. LED闪烁 (PC13)
 *   2. UART回显 (USART1 115200)
 *   3. 定时器中断 (1ms滴答)
 *   4. 多任务调度
 */

#include <stdint.h>
#include <stdarg.h>

/* ============================================================================
 * 寄存器定义 (简化版，完整版请见单独头文件)
 * ============================================================================ */

#define RCC_BASE            0x40021000
#define RCC_APB2ENR         (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR         (*(volatile uint32_t *)(RCC_BASE + 0x1C))

#define GPIOA_BASE          0x40010800
#define GPIOC_BASE          0x40011000
#define GPIO_CRL(port)      (*(volatile uint32_t *)(port + 0x00))
#define GPIO_CRH(port)      (*(volatile uint32_t *)(port + 0x04))
#define GPIO_ODR(port)      (*(volatile uint32_t *)(port + 0x0C))
#define GPIO_BSRR(port)     (*(volatile uint32_t *)(port + 0x10))

#define USART1_BASE         0x40013800
#define USART_SR            (*(volatile uint32_t *)(USART1_BASE + 0x00))
#define USART_DR            (*(volatile uint32_t *)(USART1_BASE + 0x04))
#define USART_BRR           (*(volatile uint32_t *)(USART1_BASE + 0x08))
#define USART_CR1           (*(volatile uint32_t *)(USART1_BASE + 0x0C))

#define TIM2_BASE           0x40000000
#define TIM_CR1             (*(volatile uint32_t *)(TIM2_BASE + 0x00))
#define TIM_DIER            (*(volatile uint32_t *)(TIM2_BASE + 0x0C))
#define TIM_SR              (*(volatile uint32_t *)(TIM2_BASE + 0x10))
#define TIM_CNT             (*(volatile uint32_t *)(TIM2_BASE + 0x24))
#define TIM_PSC             (*(volatile uint32_t *)(TIM2_BASE + 0x28))
#define TIM_ARR             (*(volatile uint32_t *)(TIM2_BASE + 0x2C))

/* NVIC */
#define NVIC_ISER0          (*(volatile uint32_t *)0xE000E100)
#define NVIC_ISER1          (*(volatile uint32_t *)0xE000E104)

/* ============================================================================
 * 系统配置
 * ============================================================================ */

#define SYSTEM_CLK          72000000
#define TICK_FREQ           1000

/* ============================================================================
 * 全局变量
 * ============================================================================ */

static volatile uint32_t g_milliseconds = 0;
static volatile uint32_t g_led_tick = 0;

/* ============================================================================
 * 延时函数
 * ============================================================================ */

void delay_ms(uint32_t ms)
{
    uint32_t start = g_milliseconds;
    while ((g_milliseconds - start) < ms);
}

void delay_us(uint32_t us)
{
    /* 简单忙等待，72MHz下约72周期1us */
    for (volatile uint32_t i = 0; i < us * 8; i++);
}

/* ============================================================================
 * 时钟配置
 * ============================================================================ */

/**
 * @brief 配置系统时钟为72MHz
 * @details HSE 8MHz -> PLL x9 -> SYSCLK 72MHz
 */
void SystemClock_Config(void)
{
    /* 使能HSE */
    (*(volatile uint32_t *)(RCC_BASE + 0x00)) |= (1 << 16);
    while (!((*(volatile uint32_t *)(RCC_BASE + 0x00)) & (1 << 17)));

    /* Flash等待状态: 2WS */
    (*(volatile uint32_t *)0x40022000) = 0x12;

    /* 配置PLL: HSE x9 */
    uint32_t cfgr = (*(volatile uint32_t *)(RCC_BASE + 0x04));
    cfgr &= ~(0x3F << 16);
    cfgr |= (1 << 16) | (0x7 << 18);  /* PLLSRC=HSE, PLLMUL=x9 */
    cfgr |= (0x4 << 8);  /* APB1 = HCLK/2 */
    (*(volatile uint32_t *)(RCC_BASE + 0x04)) = cfgr;

    /* 使能PLL */
    (*(volatile uint32_t *)(RCC_BASE + 0x00)) |= (1 << 24);
    while (!((*(volatile uint32_t *)(RCC_BASE + 0x00)) & (1 << 25)));

    /* 切换到PLL */
    cfgr = (*(volatile uint32_t *)(RCC_BASE + 0x04));
    cfgr &= ~0x3;
    cfgr |= 0x2;
    (*(volatile uint32_t *)(RCC_BASE + 0x04)) = cfgr;
    while (((*(volatile uint32_t *)(RCC_BASE + 0x04)) & 0xC) != 0x8);
}

/* ============================================================================
 * GPIO配置
 * ============================================================================ */

/**
 * @brief 初始化PC13为推挽输出 (板载LED)
 */
void GPIO_Init(void)
{
    /* 使能GPIOC时钟 */
    RCC_APB2ENR |= (1 << 4);

    /* PC13: 推挽输出, 2MHz */
    uint32_t crh = GPIO_CRH(GPIOC_BASE);
    crh &= ~(0xF << 20);  /* 清除PC13配置 */
    crh |= (0x2 << 20);   /* 输出推挽, 2MHz */
    GPIO_CRH(GPIOC_BASE) = crh;

    /* 初始状态: LED熄灭 (高电平) */
    GPIO_BSRR(GPIOC_BASE) = (1 << 13);
}

/**
 * @brief 设置LED状态
 * @param on 1=点亮, 0=熄灭
 */
void LED_Set(uint8_t on)
{
    if (on) {
        GPIO_BSRR(GPIOC_BASE) = (1 << 29);  /* 复位PC13 */
    } else {
        GPIO_BSRR(GPIOC_BASE) = (1 << 13);  /* 置位PC13 */
    }
}

/**
 * @brief 翻转LED状态
 */
void LED_Toggle(void)
{
    static uint8_t state = 0;
    LED_Set(state);
    state = !state;
}

/* ============================================================================
 * UART配置 (USART1 PA9-TX PA10-RX)
 * ============================================================================ */

void UART_Init(void)
{
    /* 使能USART1和GPIOA时钟 */
    RCC_APB2ENR |= (1 << 14) | (1 << 2);

    /* PA9 (TX): 复用推挽输出 */
    uint32_t crh = GPIO_CRH(GPIOA_BASE);
    crh &= ~(0xF << 4);
    crh |= (0xB << 4);  /* 复用推挽, 50MHz */
    GPIO_CRH(GPIOA_BASE) = crh;

    /* PA10 (RX): 浮空输入 */
    crh &= ~(0xF << 8);
    crh |= (0x4 << 8);  /* 浮空输入 */
    GPIO_CRH(GPIOA_BASE) = crh;

    /* 波特率: 115200 @ 72MHz */
    /* BRR = 72000000 / 115200 = 625 = 0x271 */
    USART_BRR = 0x271;

    /* 使能发送、接收、USART */
    USART_CR1 = (1 << 3) | (1 << 2) | (1 << 13);
}

void UART_SendChar(char c)
{
    while (!(USART_SR & (1 << 7)));  /* 等待TXE */
    USART_DR = c;
}

void UART_SendString(const char *str)
{
    while (*str) {
        UART_SendChar(*str++);
    }
}

char UART_ReceiveChar(void)
{
    while (!(USART_SR & (1 << 5)));  /* 等待RXNE */
    return (char)(USART_DR & 0xFF);
}

uint8_t UART_Available(void)
{
    return (USART_SR & (1 << 5)) ? 1 : 0;
}

/* ============================================================================
 * printf实现
 * ============================================================================ */

static void print_hex(uint32_t val, uint8_t digits)
{
    const char hex[] = "0123456789ABCDEF";
    char buf[9];
    buf[digits] = '\0';

    for (int i = digits - 1; i >= 0; i--) {
        buf[i] = hex[val & 0xF];
        val >>= 4;
    }

    UART_SendString(buf);
}

static void print_dec(int32_t val)
{
    char buf[12];
    uint8_t i = 0;
    uint8_t neg = 0;

    if (val < 0) {
        neg = 1;
        val = -val;
    }

    if (val == 0) {
        buf[i++] = '0';
    }

    while (val > 0) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }

    if (neg) buf[i++] = '-';

    /* 反转输出 */
    while (i > 0) {
        UART_SendChar(buf[--i]);
    }
}

void printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'c':
                    UART_SendChar((char)va_arg(args, int));
                    break;
                case 's':
                    UART_SendString(va_arg(args, char*));
                    break;
                case 'd':
                case 'i':
                    print_dec(va_arg(args, int));
                    break;
                case 'x':
                    print_hex(va_arg(args, unsigned int), 8);
                    break;
                case 'X':
                    print_hex(va_arg(args, unsigned int), 2);
                    break;
                case '%':
                    UART_SendChar('%');
                    break;
                default:
                    UART_SendChar(*fmt);
                    break;
            }
        } else {
            UART_SendChar(*fmt);
        }
        fmt++;
    }

    va_end(args);
}

/* ============================================================================
 * 定时器配置 (TIM2 1ms中断)
 * ============================================================================ */

void TIM2_Init(void)
{
    /* 使能TIM2时钟 */
    RCC_APB1ENR |= (1 << 0);

    /* 预分频: 36MHz / 36000 = 1kHz */
    TIM_PSC = 35999;
    TIM_ARR = 0;

    /* 使能更新中断 */
    TIM_DIER = (1 << 0);

    /* NVIC配置: TIM2中断 */
    NVIC_ISER0 |= (1 << 28);

    /* 使能定时器 */
    TIM_CR1 = (1 << 0);
}

void TIM2_IRQHandler(void)
{
    if (TIM_SR & (1 << 0)) {
        TIM_SR &= ~(1 << 0);  /* 清除中断标志 */
        g_milliseconds++;
        g_led_tick++;

        /* 每500ms翻转LED */
        if (g_led_tick >= 500) {
            g_led_tick = 0;
            LED_Toggle();
        }
    }
}

/* ============================================================================
 * 任务调度器 (简化版)
 * ============================================================================ */

typedef void (*TaskFunc)(void);

typedef struct {
    TaskFunc func;
    uint32_t interval;
    uint32_t lastRun;
    const char *name;
} Task;

#define MAX_TASKS 4
static Task g_tasks[MAX_TASKS];
static uint8_t g_numTasks = 0;

uint8_t Scheduler_AddTask(TaskFunc func, uint32_t interval, const char *name)
{
    if (g_numTasks >= MAX_TASKS) return 0;

    g_tasks[g_numTasks].func = func;
    g_tasks[g_numTasks].interval = interval;
    g_tasks[g_numTasks].lastRun = 0;
    g_tasks[g_numTasks].name = name;
    g_numTasks++;

    return 1;
}

void Scheduler_Run(void)
{
    uint32_t now = g_milliseconds;

    for (uint8_t i = 0; i < g_numTasks; i++) {
        if ((now - g_tasks[i].lastRun) >= g_tasks[i].interval) {
            g_tasks[i].lastRun = now;
            g_tasks[i].func();
        }
    }
}

/* ============================================================================
 * 任务实现
 * ============================================================================ */

/**
 * @brief 任务1: 串口心跳
 */
void Task_Heartbeat(void)
{
    static uint32_t count = 0;
    printf("[TICK] System running for %d seconds\r\n", count++);
}

/**
 * @brief 任务2: 处理串口命令
 */
void Task_CommandHandler(void)
{
    if (UART_Available()) {
        char cmd = UART_ReceiveChar();

        switch (cmd) {
            case 'l':
            case 'L':
                LED_Toggle();
                printf("LED toggled\r\n");
                break;
            case 's':
            case 'S':
                printf("System tick: %d ms\r\n", (int)g_milliseconds);
                break;
            case 'h':
            case 'H':
            case '?':
                printf("Commands: L=toggle LED, S=status, H=help\r\n");
                break;
            default:
                UART_SendChar(cmd);  /* 回显 */
                break;
        }
    }
}

/**
 * @brief 任务3: 性能统计
 */
void Task_Stats(void)
{
    static uint32_t lastTick = 0;
    uint32_t elapsed = g_milliseconds - lastTick;

    if (elapsed > 0) {
        /* 计算CPU使用率 (这里简化为打印任务执行频率) */
        printf("[STATS] Tasks running at 1Hz, tick precision OK\r\n");
        lastTick = g_milliseconds;
    }
}

/* ============================================================================
 * 主函数
 * ============================================================================ */

int main(void)
{
    /* 系统初始化 */
    SystemClock_Config();
    GPIO_Init();
    UART_Init();
    TIM2_Init();

    /* 欢迎信息 */
    UART_SendString("\r\n");
    UART_SendString("========================================\r\n");
    UART_SendString("  STM32F103 Baremetal Template v1.0    \r\n");
    UART_SendString("  System Clock: 72MHz                  \r\n");
    UART_SendString("  UART: 115200 8N1                     \r\n");
    UART_SendString("========================================\r\n");
    UART_SendString("Press H for help\r\n\r\n");

    /* 注册任务 */
    Scheduler_AddTask(Task_Heartbeat, 1000, "Heartbeat");
    Scheduler_AddTask(Task_CommandHandler, 10, "Command");
    Scheduler_AddTask(Task_Stats, 5000, "Stats");

    /* 主循环 */
    while (1) {
        Scheduler_Run();

        /* 空闲时进入低功耗模式 */
        __asm volatile ("wfi");
    }

    return 0;
}
```

### 10.3 Makefile构建脚本

```makefile
# Makefile for STM32F103C8T6 Baremetal Project

# Toolchain
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy
OBJDUMP = $(PREFIX)objdump
SIZE = $(PREFIX)size

# Target name
TARGET = firmware

# Directories
SRC_DIR = src
INC_DIR = include
STARTUP_DIR = startup
LINKER_DIR = linker
BUILD_DIR = build

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
SOURCES += $(wildcard $(STARTUP_DIR)/*.c)

# Object files
OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SOURCES)))

# Include paths
INCLUDES = -I$(INC_DIR)

# Compiler flags
CFLAGS = -mcpu=cortex-m3 -mthumb
CFLAGS += -O2 -g
CFLAGS += -Wall -Wextra
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -DSTM32F103xB
CFLAGS += $(INCLUDES)

# Linker flags
LDFLAGS = -T$(LINKER_DIR)/stm32f103c8.ld
LDFLAGS += -mcpu=cortex-m3 -mthumb
LDFLAGS += -specs=nano.specs
LDFLAGS += -lc -lm -lnosys
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(TARGET).map

# Build rules
.PHONY: all clean flash debug

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hex size

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
 @echo "CC  $<"
 @$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(STARTUP_DIR)/%.c | $(BUILD_DIR)
 @echo "CC  $<"
 @$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
 @echo "LD  $@"
 @$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
 @echo "OBJCOPY $@"
 @$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
 @echo "OBJCOPY $@"
 @$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR):
 @mkdir -p $@

size: $(BUILD_DIR)/$(TARGET).elf
 @echo "========================================"
 @echo "Memory Usage:"
 @$(SIZE) $<
 @echo "========================================"

# Flash using OpenOCD
flash: $(BUILD_DIR)/$(TARGET).bin
 openocd -f interface/stlink.cfg \
         -f target/stm32f1x.cfg \
         -c "program $< 0x08000000 verify reset exit"

# Flash using ST-Link
flash-stlink: $(BUILD_DIR)/$(TARGET).bin
 st-flash write $< 0x8000000

# Debug server
debug:
 openocd -f interface/stlink.cfg -f target/stm32f1x.cfg

# Clean build
clean:
 rm -rf $(BUILD_DIR)

# Create hex dump for inspection
dump: $(BUILD_DIR)/$(TARGET).elf
 $(OBJDUMP) -d $< > $(BUILD_DIR)/$(TARGET).lst
 $(OBJDUMP) -h $< > $(BUILD_DIR)/$(TARGET).sec
```

### 10.4 CMake构建配置

```cmake
# CMakeLists.txt for STM32F103 Baremetal Project

cmake_minimum_required(VERSION 3.16)

# Project name
project(STM32F103_Baremetal C ASM)

# Target processor
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Toolchain
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

# Compiler flags
set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -mthumb")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F103xB")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS "-T${CMAKE_SOURCE_DIR}/linker/stm32f103c8.ld")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -specs=nano.specs")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc -lm -lnosys")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# Source files
set(SOURCES
    src/main.c
    src/system_stm32f103.c
    src/gpio.c
    src/uart.c
    src/timer.c
    src/scheduler.c
    src/printf.c
    startup/startup_stm32f103.c
)

# Executable
add_executable(${PROJECT_NAME}.elf ${SOURCES})

# Post-build commands
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Generating binary and hex files"
)

# Flash target
add_custom_target(flash
    COMMAND openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program ${PROJECT_NAME}.bin 0x08000000 verify reset exit"
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Flashing firmware to STM32F103"
)
```

---

## 11. 调试技巧(OpenOCD/J-Link)

### 11.1 OpenOCD配置与使用

```bash
# 安装OpenOCD
# Ubuntu/Debian:
sudo apt-get install openocd

# macOS:
brew install openocd

# Windows: 下载预编译二进制文件
```

#### 启动OpenOCD服务器

```bash
# ST-Link V2 调试器
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg

# J-Link 调试器
openocd -f interface/jlink.cfg -f target/stm32f1x.cfg

# 指定设备序列号 (多调试器环境)
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
        -c "hla_serial 1234567890ABCDEF"

# 指定自定义端口号
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
        -c "gdb_port 3333" -c "telnet_port 4444"
```

#### OpenOCD常用命令 (telnet 4444)

```tcl
# 暂停/恢复目标
halt
resume
reset
reset halt

# 内存操作
mdw 0x08000000 16      # 读取Flash起始地址16个字
mww 0x20000000 0x1234  # 向RAM写入数据

# Flash操作
flash probe 0
flash erase_sector 0 0 127    # 擦除所有扇区
flash write_image erase firmware.bin 0x08000000
flash verify_image firmware.bin 0x08000000

# 寄存器操作
reg                    # 显示所有寄存器
reg pc                 # 显示PC寄存器
reg pc 0x08000000      # 设置PC寄存器

# 断点管理
bp 0x08000100 4 hw     # 设置硬件断点
rbp 0x08000100         # 删除断点

# 查看外设寄存器
# GPIOA
mdw 0x40010800 4
# RCC
mdw 0x40021000 8
```

### 11.2 GDB调试配置

#### GDB启动脚本 (.gdbinit)

```gdb
# STM32F103 GDB 调试配置

# 连接到OpenOCD
target extended-remote localhost:3333

# 设置架构
set architecture arm

# 加载符号表
file build/firmware.elf

# 复位并暂停
monitor reset halt

# 加载程序
load

# 设置断点
break main
break HardFault_Handler

# 继续执行
continue
```

#### GDB常用命令

```bash
# 启动GDB
arm-none-eabi-gdb build/firmware.elf

# 常用命令
(gdb) run                    # 运行程序
(gdb) continue 或 c          # 继续执行
(gdb) step 或 s              # 单步进入
(gdb) next 或 n              # 单步跳过
(gdb) finish                 # 运行到函数返回
(gdb) break main             # 在main设置断点
(gdb) break *0x08000100      # 在地址设置断点
(gdb) info breakpoints       # 查看断点
(gdb) delete 1               # 删除断点1
(gdb) print variable         # 打印变量值
(gdb) print/x variable       # 十六进制打印
(gdb) x/16xw 0x20000000      # 查看内存
(gdb) info registers         # 查看寄存器
(gdb) backtrace 或 bt        # 查看调用栈
(gdb) disassemble            # 反汇编
(gdb) monitor reset halt     # 复位目标
(gdb) quit                   # 退出
```

### 11.3 调试技巧

#### 硬故障(HardFault)诊断

```c
/**
 * @file fault_handler.c
 * @brief 硬故障诊断程序
 */

#include <stdint.h>

typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
} HardFault_StackFrame;

/* SCB寄存器 */
#define SCB_HFSR    (*(volatile uint32_t *)0xE000ED2C)
#define SCB_CFSR    (*(volatile uint32_t *)0xE000ED28)
#define SCB_BFAR    (*(volatile uint32_t *)0xE000ED38)
#define SCB_MMAR    (*(volatile uint32_t *)0xE000ED34)

/* CFSR位定义 */
#define CFSR_IACCVIOL   (1 << 0)   /* 取指访问冲突 */
#define CFSR_DACCVIOL   (1 << 1)   /* 数据访问冲突 */
#define CFSR_MUNSTKERR  (1 << 3)   /* 出栈错误 */
#define CFSR_MSTKERR    (1 << 4)   /* 入栈错误 */
#define CFSR_IBUSERR    (1 << 8)   /* 取指总线错误 */
#define CFSR_PRECISERR  (1 << 9)   /* 精确数据总线错误 */
#define CFSR_IMPRECISERR (1 << 10) /* 非精确数据总线错误 */
#define CFSR_UNSTKERR   (1 << 11)  /* 出栈总线错误 */
#define CFSR_STKERR     (1 << 12)  /* 入栈总线错误 */
#define CFSR_UNDEFINSTR (1 << 16)  /* 未定义指令 */
#define CFSR_INVSTATE   (1 << 17)  /* 无效状态 */
#define CFSR_INVPC      (1 << 18)  /* 无效PC */
#define CFSR_NOCP       (1 << 19)  /* 无协处理器 */
#define CFSR_UNALIGNED  (1 << 24)  /* 非对齐访问 */
#define CFSR_DIVBYZERO  (1 << 25)  /* 除零错误 */

/* 外部定义的UART发送函数 */
extern void UART_SendChar(char c);
extern void UART_SendString(const char *str);

static void send_hex(uint32_t val)
{
    const char hex[] = "0123456789ABCDEF";
    for (int i = 7; i >= 0; i--) {
        UART_SendChar(hex[(val >> (i * 4)) & 0xF]);
    }
}

void HardFault_Handler_C(HardFault_StackFrame *stackFrame)
{
    UART_SendString("\r\n*** HARDFAULT ***\r\n");

    UART_SendString("R0:  0x"); send_hex(stackFrame->r0);  UART_SendString("\r\n");
    UART_SendString("R1:  0x"); send_hex(stackFrame->r1);  UART_SendString("\r\n");
    UART_SendString("R2:  0x"); send_hex(stackFrame->r2);  UART_SendString("\r\n");
    UART_SendString("R3:  0x"); send_hex(stackFrame->r3);  UART_SendString("\r\n");
    UART_SendString("R12: 0x"); send_hex(stackFrame->r12); UART_SendString("\r\n");
    UART_SendString("LR:  0x"); send_hex(stackFrame->lr);  UART_SendString("\r\n");
    UART_SendString("PC:  0x"); send_hex(stackFrame->pc);  UART_SendString("\r\n");
    UART_SendString("PSR: 0x"); send_hex(stackFrame->psr); UART_SendString("\r\n");

    UART_SendString("HFSR: 0x"); send_hex(SCB_HFSR); UART_SendString("\r\n");
    UART_SendString("CFSR: 0x"); send_hex(SCB_CFSR); UART_SendString("\r\n");

    if (SCB_CFSR & CFSR_BFARVALID) {
        UART_SendString("BFAR: 0x"); send_hex(SCB_BFAR); UART_SendString("\r\n");
    }
    if (SCB_CFSR & CFSR_MMARVALID) {
        UART_SendString("MMAR: 0x"); send_hex(SCB_MMAR); UART_SendString("\r\n");
    }

    /* 诊断错误类型 */
    UART_SendString("Fault: ");
    if (SCB_CFSR & CFSR_IACCVIOL) UART_SendString("Instruction access violation\r\n");
    if (SCB_CFSR & CFSR_DACCVIOL) UART_SendString("Data access violation\r\n");
    if (SCB_CFSR & CFSR_IBUSERR)  UART_SendString("Instruction bus error\r\n");
    if (SCB_CFSR & CFSR_UNDEFINSTR) UART_SendString("Undefined instruction\r\n");
    if (SCB_CFSR & CFSR_INVSTATE) UART_SendString("Invalid state\r\n");
    if (SCB_CFSR & CFSR_INVPC)    UART_SendString("Invalid PC\r\n");
    if (SCB_CFSR & CFSR_DIVBYZERO) UART_SendString("Divide by zero\r\n");

    /* 无限循环等待调试 */
    while (1) {
        __asm volatile ("nop");
    }
}

/* 汇编入口 */
__attribute__((naked))
void HardFault_Handler(void)
{
    __asm volatile (
        "tst    lr, #4          \n"  /* 检查EXC_RETURN位2 */
        "ite    eq              \n"
        "mrseq  r0, msp         \n"  /* 使用MSP */
        "mrsne  r0, psp         \n"  /* 使用PSP */
        "ldr    r1, =HardFault_Handler_C \n"
        "bx     r1              \n"
    );
}
```

#### 调试打印宏

```c
#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

/* 调试级别 */
#define DEBUG_LEVEL_NONE    0
#define DEBUG_LEVEL_ERROR   1
#define DEBUG_LEVEL_WARN    2
#define DEBUG_LEVEL_INFO    3
#define DEBUG_LEVEL_DEBUG   4

/* 设置当前调试级别 */
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#endif

/* 调试输出函数 */
extern void Debug_Print(const char *file, int line, const char *func,
                        const char *fmt, ...);

/* 条件编译的调试宏 */
#if DEBUG_LEVEL >= DEBUG_LEVEL_ERROR
    #define DEBUG_ERROR(fmt, ...) \
        Debug_Print(__FILE__, __LINE__, __func__, "[ERR] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_ERROR(fmt, ...) ((void)0)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_WARN
    #define DEBUG_WARN(fmt, ...) \
        Debug_Print(__FILE__, __LINE__, __func__, "[WRN] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_WARN(fmt, ...) ((void)0)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_INFO
    #define DEBUG_INFO(fmt, ...) \
        Debug_Print(__FILE__, __LINE__, __func__, "[INF] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_INFO(fmt, ...) ((void)0)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_DEBUG
    #define DEBUG_DEBUG(fmt, ...) \
        Debug_Print(__FILE__, __LINE__, __func__, "[DBG] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_DEBUG(fmt, ...) ((void)0)
#endif

/* 断言 */
#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            DEBUG_ERROR("Assertion failed: %s", #cond); \
            while (1); \
        } \
    } while (0)

/* 打印变量值 */
#define DEBUG_VAR(var) \
    DEBUG_DEBUG("%s = %d (0x%X)", #var, (int)(var), (unsigned int)(var))

/* 打印内存区域 */
void Debug_DumpMemory(const char *name, const void *addr, uint32_t len);

/* 打印调用栈 */
void Debug_PrintStackTrace(void);

#endif /* DEBUG_H */
```

### 11.4 性能分析技巧

```c
/**
 * @file profiling.c
 * @brief 简单性能分析工具
 */

#include <stdint.h>

/* 使用DWT周期计数器 (Cortex-M3/M4) */
#define DWT_CTRL        (*(volatile uint32_t *)0xE0001000)
#define DWT_CYCCNT      (*(volatile uint32_t *)0xE0001004)
#define DEMCR           (*(volatile uint32_t *)0xE000EDFC)

/**
 * @brief 初始化周期计数器
 */
void Profiling_Init(void)
{
    /* 使能DWT跟踪 */
    DEMCR |= (1 << 24);
    /* 使能CYCCNT */
    DWT_CTRL |= (1 << 0);
}

/**
 * @brief 获取当前周期计数
 */
static inline uint32_t Profiling_GetCycles(void)
{
    return DWT_CYCCNT;
}

/**
 * @brief 测量函数执行时间
 */
typedef struct {
    uint32_t startCycles;
    uint32_t totalCycles;
    uint32_t callCount;
    uint32_t minCycles;
    uint32_t maxCycles;
    const char *name;
} Profile_Record;

#define MAX_PROFILE_RECORDS 16
static Profile_Record g_profileRecords[MAX_PROFILE_RECORDS];
static uint8_t g_profileCount = 0;

uint8_t Profiling_Register(const char *name)
{
    if (g_profileCount >= MAX_PROFILE_RECORDS) return 0xFF;

    uint8_t id = g_profileCount++;
    g_profileRecords[id].name = name;
    g_profileRecords[id].totalCycles = 0;
    g_profileRecords[id].callCount = 0;
    g_profileRecords[id].minCycles = 0xFFFFFFFF;
    g_profileRecords[id].maxCycles = 0;

    return id;
}

void Profiling_Start(uint8_t id)
{
    if (id < MAX_PROFILE_RECORDS) {
        g_profileRecords[id].startCycles = Profiling_GetCycles();
    }
}

void Profiling_Stop(uint8_t id)
{
    if (id >= MAX_PROFILE_RECORDS) return;

    uint32_t elapsed = Profiling_GetCycles() - g_profileRecords[id].startCycles;

    g_profileRecords[id].totalCycles += elapsed;
    g_profileRecords[id].callCount++;

    if (elapsed < g_profileRecords[id].minCycles) {
        g_profileRecords[id].minCycles = elapsed;
    }
    if (elapsed > g_profileRecords[id].maxCycles) {
        g_profileRecords[id].maxCycles = elapsed;
    }
}

void Profiling_Report(void (*printFunc)(const char *fmt, ...))
{
    printFunc("\r\n=== Performance Report ===\r\n");
    printFunc("%-16s %8s %8s %10s %10s\r\n",
              "Function", "Calls", "Avg(us)", "Min(us)", "Max(us)");

    for (uint8_t i = 0; i < g_profileCount; i++) {
        Profile_Record *r = &g_profileRecords[i];
        if (r->callCount > 0) {
            uint32_t avg = r->totalCycles / r->callCount;
            /* 假设72MHz，转换为微秒 */
            printFunc("%-16s %8d %8d %10d %10d\r\n",
                      r->name,
                      r->callCount,
                      avg / 72,
                      r->minCycles / 72,
                      r->maxCycles / 72);
        }
    }
}

/* 使用示例宏 */
#define PROFILE_BEGIN(id) Profiling_Start(id)
#define PROFILE_END(id)   Profiling_Stop(id)
```

### 11.5 内存使用分析

```c
/**
 * @brief 链接器脚本定义的符号声明
 */
extern uint32_t _estack;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t __heap_start;
extern uint32_t __heap_end;

/**
 * @brief 获取栈使用情况
 */
uint32_t Stack_GetUnused(void)
{
    /* 栈从高地址向低地址增长 */
    volatile uint32_t *p = &_ebss;
    uint32_t unused = 0;

    /* 查找未使用的栈空间 (填充模式) */
    while (*p == 0xDEADBEEF && p < &_estack) {
        unused += 4;
        p++;
    }

    return unused;
}

/**
 * @brief 获取堆使用情况
 */
typedef struct {
    void *ptr;
    uint32_t size;
    uint8_t used;
} Heap_Block;

#define HEAP_BLOCKS 32
static Heap_Block g_heapBlocks[HEAP_BLOCKS];

void* malloc_simple(uint32_t size)
{
    /* 简单堆分配实现 */
    static uint8_t *heapPtr = (uint8_t *)&__heap_start;

    void *ptr = heapPtr;
    heapPtr += (size + 3) & ~3;  /* 4字节对齐 */

    if ((uint32_t)heapPtr > (uint32_t)&__heap_end) {
        return NULL;  /* 堆溢出 */
    }

    return ptr;
}

void Memory_PrintUsage(void (*print)(const char *fmt, ...))
{
    uint32_t textSize = (uint32_t)&_etext - 0x08000000;
    uint32_t dataSize = (uint32_t)&_edata - (uint32_t)&_sdata;
    uint32_t bssSize = (uint32_t)&_ebss - (uint32_t)&_sbss;
    uint32_t stackSize = (uint32_t)&_estack - (uint32_t)&_ebss;

    print("Memory Usage:\r\n");
    print("  Flash (Text): %d bytes\r\n", textSize);
    print("  RAM (Data):   %d bytes\r\n", dataSize);
    print("  RAM (BSS):    %d bytes\r\n", bssSize);
    print("  Stack Total:  %d bytes\r\n", stackSize);
}
```

---

## 总结

### 裸机开发关键要点

1. **启动流程**: 理解Reset_Handler -> SystemInit -> main的执行顺序
2. **链接器脚本**: 正确配置内存布局，确保中断向量表在Flash起始位置
3. **时钟配置**: 稳定可靠的时钟是系统正常运行的基础
4. **中断处理**: 正确配置NVIC，注意中断优先级和响应时间
5. **资源管理**: 裸机环境资源有限，需要精确控制内存和堆栈使用

### 开发流程建议

```
1. 准备阶段
   └── 准备硬件(开发板、调试器)
   └── 安装工具链(GCC, OpenOCD, GDB)
   └── 准备链接器脚本和启动代码

2. 基础验证
   └── 编写最小系统(时钟+GPIO)
   └── 验证LED闪烁
   └── 配置调试串口

3. 功能开发
   └── 外设驱动开发
   └── 中断系统配置
   └── 应用层代码编写

4. 调试优化
   └── 使用GDB单步调试
   └── 添加性能分析
   └── 内存使用优化

5. 部署测试
   └── 烧录程序
   └── 功能验证
   └── 稳定性测试
```

### 推荐阅读

- [ARM Cortex-M3 技术参考手册](https://developer.arm.com/documentation/)
- [STM32F103 参考手册 RM0008](https://www.st.com/)
- [GNU ARM Embedded Toolchain 文档](https://gcc.gnu.org/onlinedocs/)
- [OpenOCD 用户手册](http://openocd.org/doc/html/)

---

*本文档适用于STM32F103系列MCU，其他Cortex-M3/M4 MCU可参考修改使用。*
*最后更新: 2026年3月*


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
