# 裸机嵌入式开发 (Baremetal Development)

> 无操作系统直接硬件编程：启动代码、链接器脚本、中断向量、寄存器级外设控制

---

## 📋 技术概述

裸机开发（Bare-metal Programming）是指在没有操作系统支持的情况下，直接对硬件进行编程。程序直接运行在处理器上，通过直接操作硬件寄存器来控制外设，是理解计算机系统底层原理的最佳途径，也是资源受限设备和实时控制系统的首选方案。

### 裸机编程特点

| 特性 | 说明 | 优势 |
|:-----|:-----|:-----|
| **无 OS 开销** | 没有任务切换开销 | 响应确定性，实时性高 |
| **直接硬件控制** | 完全控制处理器和外设 | 极致优化可能 |
| **代码精简** | 最终二进制文件极小 | 通常 <10KB 启动代码 |
| **启动快速** | 上电到运行代码仅需毫秒级 | 适合快速响应场景 |
| **资源占用少** | 内存需求极低 | 适合资源受限设备 |

### 与 RTOS 的对比

| 特性 | 裸机 | RTOS | Linux |
|:-----|:-----|:-----|:------|
| 代码体积 | < 10KB | 10-100KB | > 5MB |
| RAM 需求 | < 1KB | 4KB+ | 8MB+ |
| 启动时间 | < 10ms | 100ms+ | 数秒 |
| 响应确定性 | 极高 | 高 | 中 |
| 开发复杂度 | 高 | 中 | 低 |
| 功能丰富度 | 低 | 中 | 高 |
| 适用场景 | 简单控制 | 复杂实时系统 | 复杂应用 |

---


---

## 📑 目录

- [裸机嵌入式开发 (Baremetal Development)](#裸机嵌入式开发-baremetal-development)
  - [📋 技术概述](#-技术概述)
    - [裸机编程特点](#裸机编程特点)
    - [与 RTOS 的对比](#与-rtos-的对比)
  - [📑 目录](#-目录)
  - [⭐ 核心知识体系](#-核心知识体系)
    - [1. 启动流程 (Boot Sequence)](#1-启动流程-boot-sequence)
    - [2. 内存布局与链接器脚本](#2-内存布局与链接器脚本)
    - [3. 中断系统](#3-中断系统)
    - [4. 外设编程模型](#4-外设编程模型)
  - [🎯 应用场景](#-应用场景)
    - [1. 启动引导程序 (Bootloader)](#1-启动引导程序-bootloader)
    - [2. 实时控制系统](#2-实时控制系统)
    - [3. 安全关键系统](#3-安全关键系统)
    - [4. 极简嵌入式设备](#4-极简嵌入式设备)
    - [5. 学习与研究](#5-学习与研究)
  - [🛠️ 开发环境要求](#️-开发环境要求)
    - [硬件工具](#硬件工具)
    - [软件工具链](#软件工具链)
      - [ARM Cortex-M 工具链](#arm-cortex-m-工具链)
      - [构建工具](#构建工具)
    - [推荐的开发环境](#推荐的开发环境)
  - [📚 与主文档的关系](#-与主文档的关系)
    - [文档架构](#文档架构)
    - [技术层次关系](#技术层次关系)
  - [🚀 快速开始指引](#-快速开始指引)
    - [步骤 1: 最小启动代码](#步骤-1-最小启动代码)
    - [步骤 2: 链接器脚本](#步骤-2-链接器脚本)
    - [步骤 3: GPIO 控制](#步骤-3-gpio-控制)
    - [步骤 4: 编译与烧录](#步骤-4-编译与烧录)
  - [❓ 常见问题](#-常见问题)
    - [启动问题](#启动问题)
    - [编译链接问题](#编译链接问题)
    - [调试问题](#调试问题)
    - [HardFault 诊断](#hardfault-诊断)
  - [📖 推荐学习路径](#-推荐学习路径)
    - [初级路径](#初级路径)
    - [中级路径](#中级路径)
    - [高级路径](#高级路径)
  - [🔗 返回上级](#-返回上级)
  - [📞 支持与资源](#-支持与资源)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## ⭐ 核心知识体系

### 1. 启动流程 (Boot Sequence)

```
上电/复位
    │
    ▼
┌─────────────────┐
│ 1. 从复位向量    │ ← 从 Flash 地址 0x0800_0000 读取初始 SP 和 PC
│    读取栈顶和    │
│    复位处理函数  │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ 2. 初始化栈指针  │ ← 设置 SP = _estack (栈顶地址)
│    (SP)         │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ 3. 复制.data段   │ ← 将初始化数据从 Flash 复制到 RAM
│    到 RAM       │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ 4. 清零.bss段    │ ← 将未初始化数据段清零
│                 │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ 5. 系统初始化    │ ← 配置时钟、外设等
│    (可选)       │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ 6. 调用 main()   │ ← 进入用户程序
│                 │
└────────┬────────┘
         │
         ▼
    用户程序运行
```

### 2. 内存布局与链接器脚本

```
STM32F103C8T6 内存布局 (64KB Flash, 20KB RAM):

Flash (0x0800_0000 - 0x0800_FFFF)
├─ 0x0800_0000: 中断向量表 (.isr_vector)
├─ 0x0800_0200: 代码段 (.text)
├─ 0x0800_4000: 只读数据 (.rodata)
└─ 0x0800_F000: 常量数据存储

RAM (0x2000_0000 - 0x2000_4FFF)
├─ 0x2000_0000: 已初始化数据 (.data)
├─ 0x2000_1000: 未初始化数据 (.bss)
├─ 0x2000_2000: 堆区 (Heap, 向上增长)
├─ 0x2000_3000: ─────────────────── (空闲区域)
└─ 0x2000_4FFF: 栈底 (_estack, 向下增长)
```

### 3. 中断系统

| 中断类型 | 优先级 | 用途 |
|:---------|:-------|:-----|
| Reset | -3 (最高) | 系统复位 |
| NMI | -2 | 不可屏蔽中断 |
| HardFault | -1 | 硬件错误 |
| SVCall | 可配置 | 系统调用 |
| PendSV | 可配置 | 上下文切换 |
| SysTick | 可配置 | 系统时钟 |
| 外设中断 | 可配置 | 各种外设事件 |

### 4. 外设编程模型

```
┌─────────────────────────────────────────────────────────────┐
│                    外设寄存器访问模型                         │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  外设基地址 + 偏移量                                          │
│  ├── GPIOA: 0x4001_0800                                      │
│  │   ├── MODER   (偏移 0x00) - 模式寄存器                     │
│  │   ├── ODR     (偏移 0x14) - 输出数据寄存器                 │
│  │   ├── IDR     (偏移 0x10) - 输入数据寄存器                 │
│  │   └── BSRR    (偏移 0x18) - 位设置/清除寄存器              │
│  │                                                           │
│  ├── RCC: 0x4002_1000                                        │
│  │   ├── CR      (偏移 0x00) - 时钟控制寄存器                 │
│  │   ├── APB2ENR (偏移 0x18) - APB2 外设时钟使能              │
│  │   └── CFGR    (偏移 0x04) - 时钟配置寄存器                 │
│  │                                                           │
│  └── USART1: 0x4001_3800                                     │
│      ├── SR      (偏移 0x00) - 状态寄存器                     │
│      ├── DR      (偏移 0x04) - 数据寄存器                     │
│      ├── BRR     (偏移 0x08) - 波特率寄存器                   │
│      └── CR1     (偏移 0x0C) - 控制寄存器 1                   │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 🎯 应用场景

### 1. 启动引导程序 (Bootloader)

- 固件更新功能
- 多重启动选择
- 安全启动验证
- 恢复模式支持

### 2. 实时控制系统

- 电机控制器
- 飞行控制系统
- 机器人运动控制
- CNC 控制器

### 3. 安全关键系统

- 汽车 ECU
- 医疗设备
- 航空航天设备
- 工业安全系统

### 4. 极简嵌入式设备

- 传感器节点
- RFID 读卡器
- 简单显示设备
- 遥控器

### 5. 学习与研究

- 操作系统原理
- 计算机体系结构
- 编译器原理
- 嵌入式系统设计

---

## 🛠️ 开发环境要求

### 硬件工具

| 工具 | 用途 | 价格范围 |
|:-----|:-----|:---------|
| 开发板 | 目标平台 | $3-30 |
| ST-Link V2 | 调试/编程 | $3-10 |
| J-Link | 专业调试 | $20-400 |
| USB-TTL | 串口输出 | $2-5 |
| 逻辑分析仪 | 信号分析 | $10-100 |
| 示波器 | 信号测量 | $50-500 |

### 软件工具链

#### ARM Cortex-M 工具链

```bash
# Ubuntu/Debian
sudo apt install gcc-arm-none-eabi binutils-arm-none-eabi \
    gdb-multiarch openocd

# macOS
brew install arm-none-eabi-gcc openocd

# Windows (MSYS2)
pacman -S mingw-w64-x86_64-arm-none-eabi-gcc \
    mingw-w64-x86_64-openocd
```

#### 构建工具

```bash
# Makefile 构建
make clean && make

# CMake 构建
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../arm-none-eabi.cmake ..
make

# 烧录
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
    -c "program build/firmware.bin verify reset exit 0x08000000"
```

### 推荐的开发环境

| IDE/编辑器 | 特点 | 推荐度 |
|:-----------|:-----|:------:|
| VS Code + Cortex-Debug | 免费、功能完善 | ⭐⭐⭐⭐⭐ |
| STM32CubeIDE | ST 官方、集成度高 | ⭐⭐⭐⭐ |
| Keil MDK-ARM | 商业软件、稳定 | ⭐⭐⭐⭐ |
| IAR EWARM | 商业软件、优化好 | ⭐⭐⭐⭐ |
| CLion + OpenOCD | JetBrains 生态 | ⭐⭐⭐⭐ |

---

## 📚 与主文档的关系

本目录是 [14_Embedded_IoT](../readme.md) 的底层基础，与上层技术栈形成完整知识体系。

### 文档架构

```
Baremetal/
├── README.md                          # 本文件 - 技术概览
├── 01_Baremetal_Programming.md        # 完整编程指南（1000+ 行代码）
└── examples/                          # 示例项目
    ├── startup/                       # 启动代码示例
    ├── linker_scripts/                # 链接器脚本
    ├── gpio_basic/                    # GPIO 基础操作
    ├── uart_driver/                   # UART 驱动
    ├── timer_interrupt/               # 定时器中断
    └── cooperative_scheduler/         # 合作式调度器
```

### 技术层次关系

```
应用层
    │
    ├── [../IoT_Protocols/](../IoT_Protocols/readme.md) - MQTT/CoAP/HTTP
    │
中间层
    │
    ├── [../Memory_Constrained/](../Memory_Constrained/readme.md) - 内存管理
    │
硬件抽象层
    │
    ├── [../RPi_Pico/](../RPi_Pico/readme.md) - SDK API
    │
裸机层 (本目录)
    │
    ├── 启动代码、链接器脚本
    ├── 中断向量表
    ├── 寄存器级外设控制
    │
硬件层
    │
    └── ARM Cortex-M / RISC-V / AVR
```

---

## 🚀 快速开始指引

### 步骤 1: 最小启动代码

```c
// startup_stm32f103.c
#include <stdint.h>

// 链接器符号
extern uint32_t _sidata, _sdata, _edata;
extern uint32_t _sbss, _ebss;
extern uint32_t _estack;
extern int main(void);

// 默认中断处理
void Default_Handler(void) {
    while (1);
}

// 弱定义异常处理
void Reset_Handler(void);
void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));

// 中断向量表
__attribute__((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    (void (*)(void))&_estack,    // 栈顶
    Reset_Handler,               // 复位
    NMI_Handler,                 // NMI
    HardFault_Handler,           // Hard Fault
    0, 0, 0, 0,                  // 保留
    SVC_Handler,                 // SVCall
    0, 0,                        // 保留
    PendSV_Handler,              // PendSV
    SysTick_Handler,             // SysTick
};

// 复位处理
void Reset_Handler(void) {
    // 复制 .data
    uint32_t *pSrc = &_sidata;
    uint32_t *pDest = &_sdata;
    while (pDest < &_edata) {
        *pDest++ = *pSrc++;
    }

    // 清零 .bss
    pDest = &_sbss;
    while (pDest < &_ebss) {
        *pDest++ = 0;
    }

    // 跳转到 main
    main();

    // 不应该到达这里
    while (1);
}
```

### 步骤 2: 链接器脚本

```ld
/* stm32f103c8.ld */
ENTRY(Reset_Handler)

MEMORY {
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 20K
}

SECTIONS {
    .text : {
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        _etext = .;
    } > FLASH

    _sidata = LOADADDR(.data);
    .data : {
        _sdata = .;
        *(.data*)
        _edata = .;
    } > RAM AT > FLASH

    .bss : {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > RAM

    _estack = ORIGIN(RAM) + LENGTH(RAM);
}
```

### 步骤 3: GPIO 控制

```c
// main.c
#include <stdint.h>

// 寄存器定义
#define RCC_BASE        0x40021000
#define GPIOA_BASE      0x40010800

#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))

#define RCC_APB2ENR_IOPAEN  (1 << 2)

void delay(volatile uint32_t count) {
    while (count--);
}

int main(void) {
    // 使能 GPIOA 时钟
    RCC_APB2ENR |= RCC_APB2ENR_IOPAEN;

    // 配置 PA5 为推挽输出
    GPIOA_CRL &= ~(0xF << 20);  // 清除原有配置
    GPIOA_CRL |= (0x3 << 20);   // 输出模式，最大速度 50MHz

    while (1) {
        GPIOA_ODR ^= (1 << 5);  // 翻转 PA5
        delay(500000);
    }

    return 0;
}
```

### 步骤 4: 编译与烧录

```makefile
# Makefile
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CFLAGS = -mcpu=cortex-m3 -mthumb -O0 -g -Wall
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS = -T stm32f103c8.ld -nostartfiles

SRCS = startup_stm32f103.c main.c
OBJS = $(SRCS:.c=.o)

TARGET = firmware

all: $(TARGET).bin

$(TARGET).elf: $(OBJS)
 $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
 $(SIZE) $@

$(TARGET).bin: $(TARGET).elf
 $(OBJCOPY) -O binary $< $@

%.o: %.c
 $(CC) $(CFLAGS) -c -o $@ $<

clean:
 rm -f $(OBJS) $(TARGET).elf $(TARGET).bin

flash: $(TARGET).bin
 openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
  -c "program $< verify reset exit 0x08000000"

.PHONY: all clean flash
```

---

## ❓ 常见问题

### 启动问题

| 问题 | 原因 | 解决方案 |
|:-----|:-----|:---------|
| 程序不运行 | 向量表位置错误 | 确保 .isr_vector 放在 Flash 起始 |
|  HardFault | 栈指针初始化错误 | 检查 _estack 定义 |
| 数据错误 | .data 未正确复制 | 检查 _sidata/_sdata/_edata |
| 全局变量异常 | .bss 未清零 | 检查 _sbss/_ebss 清零循环 |

### 编译链接问题

| 问题 | 原因 | 解决方案 |
|:-----|:-----|:---------|
| undefined reference | 链接库缺失 | 添加 -lc -lm -lgcc |
| section overlap | 内存区域定义错误 | 检查 MEMORY 定义 |
| 代码太大 | Flash 空间不足 | 启用 -Os 优化，移除未使用代码 |
| 未定义 Reset_Handler | 入口点错误 | 检查 ENTRY() 和符号定义 |

### 调试问题

| 问题 | 原因 | 解决方案 |
|:-----|:-----|:---------|
| OpenOCD 连接失败 | 调试器未识别 | 检查接线，安装驱动 |
| GDB 无法连接 | OpenOCD 未启动 | 先启动 OpenOCD 服务器 |
| 断点无效 | 代码优化 | 使用 -O0 编译 |
| 无法单步 | 编译器优化 | 添加 -g 选项 |

### HardFault 诊断

```c
// HardFault 处理程序
void HardFault_Handler(void) {
    __asm volatile (
        "tst lr, #4\n"
        "ite eq\n"
        "mrseq r0, msp\n"
        "mrsne r0, psp\n"
        "ldr r1, [r0, #24]\n"
        "b HardFault_Handler_C\n"
    );
}

void HardFault_Handler_C(uint32_t *stackFrame) {
    uint32_t stacked_pc = stackFrame[6];
    printf("HardFault at PC: 0x%08X\n", (unsigned int)stacked_pc);
    while (1);
}
```

---

## 📖 推荐学习路径

### 初级路径

1. 阅读 [01_Baremetal_Programming.md](./01_Baremetal_Programming.md) 第 1-4 章
2. 理解启动流程和向量表
3. 编写第一个 GPIO 闪烁程序
4. 学习链接器脚本基础

### 中级路径

1. 掌握中断和异常处理
2. 编写 UART 驱动（轮询和中断方式）
3. 实现 SysTick 定时器
4. 学习时钟系统配置

### 高级路径

1. 实现 DMA 传输
2. 编写复杂的设备驱动
3. 设计简单的任务调度器
4. 优化启动时间和功耗

---

## 🔗 返回上级

[← 返回 14_Embedded_IoT 目录](../readme.md)

---

## 📞 支持与资源

- **ARM 文档**: <https://developer.arm.com/documentation/dui0552/>
- **Cortex-M3 手册**: <https://developer.arm.com/documentation/ddi0337/>
- **STM32 参考手册**: <https://www.st.com/en/microcontrollers-microprocessors/stm32f103.html>
- **OpenOCD 文档**: <http://openocd.org/documentation/>
- **Bare Metal STM32**: <https://github.com/ARMmbed/mbed-os>

---

> **最后更新**: 2026-03-17
> **维护者**: C_Lang Knowledge Base Team
> **适用平台**: ARM Cortex-M0/M3/M4, RISC-V, AVR


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
