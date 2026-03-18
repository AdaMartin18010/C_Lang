# 中断控制器详解

## 目录

- [中断控制器详解](#中断控制器详解)
  - [目录](#目录)
  - [1. 概述](#1-概述)
    - [1.1 中断的基本概念](#11-中断的基本概念)
    - [1.2 中断的分类](#12-中断的分类)
  - [2. 8259A可编程中断控制器](#2-8259a可编程中断控制器)
    - [2.1 8259A架构](#21-8259a架构)
    - [2.2 8259A编程接口](#22-8259a编程接口)
    - [2.3 8259A初始化流程 (Linux实现)](#23-8259a初始化流程-linux实现)
    - [2.4 8259A级联配置](#24-8259a级联配置)
  - [3. APIC高级可编程中断控制器](#3-apic高级可编程中断控制器)
    - [3.1 APIC架构概述](#31-apic架构概述)
    - [3.2 Local APIC寄存器](#32-local-apic寄存器)
    - [3.3 APIC初始化代码 (Linux)](#33-apic初始化代码-linux)
  - [4. 中断优先级处理](#4-中断优先级处理)
    - [4.1 优先级编码机制](#41-优先级编码机制)
    - [4.2 优先级比较机制](#42-优先级比较机制)
    - [4.3 Linux优先级管理](#43-linux优先级管理)
  - [5. 中断向量表](#5-中断向量表)
    - [5.1 IVT (实模式) vs IDT (保护模式)](#51-ivt-实模式-vs-idt-保护模式)
    - [5.2 IDT条目结构](#52-idt条目结构)
    - [5.3 Linux IDT布局](#53-linux-idt布局)
  - [6. 中断响应流程](#6-中断响应流程)
    - [6.1 硬件响应时序](#61-硬件响应时序)
    - [6.2 软件处理流程](#62-软件处理流程)
    - [6.3 Linux中断入口代码](#63-linux中断入口代码)
    - [6.4 C语言中断处理](#64-c语言中断处理)
  - [7. 嵌套中断处理](#7-嵌套中断处理)
    - [7.1 嵌套中断模型](#71-嵌套中断模型)
    - [7.2 中断栈管理](#72-中断栈管理)
    - [7.3 中断屏蔽与使能](#73-中断屏蔽与使能)
  - [8. Linux内核中断处理](#8-linux内核中断处理)
    - [8.1 中断子系统架构](#81-中断子系统架构)
    - [8.2 IRQ描述符管理](#82-irq描述符管理)
    - [8.3 驱动注册中断示例](#83-驱动注册中断示例)
  - [9. Verilog实现框架](#9-verilog实现框架)
    - [9.1 简化中断控制器设计](#91-简化中断控制器设计)
    - [9.2 测试平台](#92-测试平台)
  - [总结](#总结)

---

## 1. 概述

### 1.1 中断的基本概念

中断是计算机系统中处理器响应外部或内部事件的机制，允许CPU在执行正常指令流时暂停当前任务，转去处理紧急或重要的事件。

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         中断系统整体架构                                 │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐                   │
│   │ 定时器  │  │  键盘   │  │  磁盘   │  │  网卡   │    中断源         │
│   │ (PIT)   │  │ (IRQ1)  │  │ (IRQ14) │  │ (IRQ11) │                   │
│   └────┬────┘  └────┬────┘  └────┬────┘  └────┬────┘                   │
│        │            │            │            │                         │
│        └────────────┴────────────┴────────────┘                         │
│                          │                                              │
│                    ┌─────┴─────┐                                       │
│                    │  中断控制器  │  ← 8259A / APIC                      │
│                    │   (PIC)   │                                       │
│                    └─────┬─────┘                                       │
│                          │ INT                                          │
│                          ▼                                              │
│   ┌─────────────────────────────────────┐                              │
│   │              CPU                     │                              │
│   │  ┌─────────┐    ┌─────────┐        │                              │
│   │  │  IF标志 │    │  EFLAGS │        │                              │
│   │  │ (bit 9) │    └─────────┘        │                              │
│   │  └─────────┘                       │                              │
│   │  ┌─────────────────────────────┐   │                              │
│   │  │  中断描述符表寄存器 (IDTR)  │   │                              │
│   │  │  Base │        Limit       │   │                              │
│   │  └───────┴────────────────────┘   │                              │
│   └─────────────────────────────────────┘                              │
│                          │                                              │
│                          ▼                                              │
│   ┌─────────────────────────────────────┐                              │
│   │        中断描述符表 (IDT)            │                              │
│   │  ┌─────┬─────────────────────────┐  │                              │
│   │  │ 0x0 │   除法错误 #DE          │  │                              │
│   │  │ 0x8 │   双重错误 #DF          │  │                              │
│   │  │0x20 │   定时器 IRQ0 → ISR     │  │                              │
│   │  │0x21 │   键盘   IRQ1 → ISR     │  │                              │
│   │  │0x80 │   系统调用 (Linux)      │  │                              │
│   │  │0xFF │   ...                   │  │                              │
│   │  └─────┴─────────────────────────┘  │                              │
│   └─────────────────────────────────────┘                              │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 1.2 中断的分类

| 中断类型 | 触发方式 | 典型来源 | 异步性 |
|---------|---------|---------|--------|
| 外部中断 (硬件) | 电平/边沿触发 | 外设、定时器 | 异步 |
| 内部中断 (异常) | 指令执行 | 除零、缺页、非法指令 | 同步 |
| 软件中断 | INT指令 | 系统调用、调试 | 同步 |

---

## 2. 8259A可编程中断控制器

### 2.1 8259A架构

Intel 8259A是经典的可编程中断控制器，在x86早期系统中广泛使用。

```
┌─────────────────────────────────────────────────────────────────┐
│                    8259A PIC 内部架构                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   IR0 ──┐                                                       │
│   IR1 ──┤                                                       │
│   IR2 ──┤    ┌──────────┐    ┌──────────┐    ┌──────────┐      │
│   IR3 ──┼───→│  IRR     │───→│  优先级  │───→│  ISR     │      │
│   IR4 ──┤    │(中断请求 │    │  判优器  │    │(服务中  │      │
│   IR5 ──┤    │ 寄存器)  │    │          │    │ 断寄存器)│      │
│   IR6 ──┤    └──────────┘    └────┬─────┘    └────┬─────┘      │
│   IR7 ──┘                         │               │             │
│                                   │               │             │
│                                   ▼               │             │
│                            ┌──────────┐          │             │
│                            │  IMR     │          │             │
│   D0-D7 ←→ 数据总线 ─────→│(中断屏蔽│          │             │
│                            │ 寄存器)  │          │             │
│                            └────┬─────┘          │             │
│                                 │                │             │
│   CAS0-CAS2 ←→ 级联线           │         ┌──────┘             │
│                                 │         │                    │
│   INT ────────→ CPU             │         ▼                    │
│   INTA ←─────── CPU             │    ┌──────────┐              │
│                                 └───→│  控制    │              │
│   RD/WR ←→ 读写控制 ───────────────→│  逻辑    │              │
│                                      │          │              │
│   A0 ───→ 地址线 ──────────────────→│  读写    │              │
│                                      │  寄存器  │              │
│                                      └──────────┘              │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 2.2 8259A编程接口

```c
// Linux内核 8259A 相关定义 (arch/x86/include/asm/i8259.h)
#define PIC_MASTER_CMD      0x20    // 主片命令口
#define PIC_MASTER_DATA     0x21    // 主片数据口
#define PIC_SLAVE_CMD       0xA0    // 从片命令口
#define PIC_SLAVE_DATA      0xA1    // 从片数据口

// ICW1 - 初始化命令字1
#define ICW1_ICW4       0x01    // 需要ICW4
#define ICW1_SINGLE     0x02    // 单片模式
#define ICW1_INTERVAL4  0x04    // 中断向量间隔4
#define ICW1_LEVEL      0x08    // 电平触发模式
#define ICW1_INIT       0x10    // 初始化命令

// ICW4 - 初始化命令字4
#define ICW4_8086       0x01    // 8086/88模式
#define ICW4_AUTO       0x02    // 自动EOI
#define ICW4_BUF_SLAVE  0x08    // 缓冲模式-从片
#define ICW4_BUF_MASTER 0x0C    // 缓冲模式-主片
#define ICW4_SFNM       0x10    // 特殊全嵌套模式

// OCW2 - 操作命令字2 (EOI)
#define PIC_EOI         0x20    // 非特定EOI
```

### 2.3 8259A初始化流程 (Linux实现)

```c
/*
 * Linux内核 8259A 初始化代码
 * arch/x86/kernel/i8259.c
 */

static void __init init_8259A(int auto_eoi)
{
    unsigned long flags;

    raw_spin_lock_irqsave(&i8259A_lock, flags);

    // ICW1: 边沿触发，级联，需要ICW4
    outb(0x11, PIC_MASTER_CMD);
    io_delay();
    outb(0x11, PIC_SLAVE_CMD);
    io_delay();

    // ICW2: 中断向量偏移
    // 主片: 0x20 (32), 从片: 0x28 (40)
    outb(IRQ0_VECTOR, PIC_MASTER_DATA);
    io_delay();
    outb(IRQ8_VECTOR, PIC_SLAVE_DATA);
    io_delay();

    // ICW3: 级联设置
    // 主片: IR2连接从片 (0x04)
    // 从片: 连接到主片IR2 (0x02)
    outb(0x04, PIC_MASTER_DATA);  // 主片IR2有从片
    io_delay();
    outb(0x02, PIC_SLAVE_DATA);   // 从片连接到主片IR2
    io_delay();

    // ICW4: 8086模式，正常EOI，非缓冲
    outb(auto_eoi ? 0x03 : 0x01, PIC_MASTER_DATA);
    io_delay();
    outb(auto_eoi ? 0x03 : 0x01, PIC_SLAVE_DATA);
    io_delay();

    // OCW1: 屏蔽所有中断 (IMR = 0xFF)
    outb(0xff, PIC_MASTER_DATA);
    io_delay();
    outb(0xff, PIC_SLAVE_DATA);
    io_delay();

    raw_spin_unlock_irqrestore(&i8259A_lock, flags);
}

// EOI (End of Interrupt) 发送
static inline void pic_eoi(int irq)
{
    if (irq >= 8) {
        // 从片中断，先发送从片EOI
        outb(PIC_EOI, PIC_SLAVE_CMD);
    }
    // 发送主片EOI
    outb(PIC_EOI, PIC_MASTER_CMD);
}
```

### 2.4 8259A级联配置

```
┌─────────────────────────────────────────────────────────────────────┐
│                     8259A 级联配置                                   │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│                        ┌───────────┐                               │
│                        │    CPU    │                               │
│                        │    INT    │◄────────────────┐            │
│                        │   INTA    │─────────────────┘            │
│                        └─────┬─────┘                               │
│                              │                                      │
│                        ┌─────┴─────┐                               │
│                        │  主 8259A │  ← Master PIC                 │
│                        │  (0x20)   │                               │
│                        │           │                               │
│     外部设备 ──IR0────→│ IRQ0-7    │◄── 向量 0x20-0x27             │
│     外部设备 ──IR1────→│           │                               │
│     外部设备 ──IR2────→│  IR2 ─────┼──┐  ← 级联到从片              │
│     外部设备 ──IR3────→│           │  │                            │
│        ...             │           │  │                            │
│     外部设备 ──IR7────→│           │  │                            │
│                        └───────────┘  │                            │
│                                       │ CAS0-CAS2                  │
│                                       │                            │
│                        ┌───────────┐  │                            │
│                        │  从 8259A │◄─┘                            │
│                        │  (0xA0)   │  ← Slave PIC                  │
│                        │           │                               │
│     外部设备 ──IR0────→│ IRQ8-15   │◄── 向量 0x28-0x2F             │
│     外部设备 ──IR1────→│           │                               │
│        ...             │           │                               │
│     外部设备 ──IR7────→│           │                               │
│                        └───────────┘                               │
│                                                                     │
│   注意: 级联时主片的IR2被占用，实际可用IRQ为 0,1,3,4,5,6,7,8-15   │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 3. APIC高级可编程中断控制器

### 3.1 APIC架构概述

APIC (Advanced Programmable Interrupt Controller) 取代了传统的8259A，提供更强大的中断处理能力。

```
┌─────────────────────────────────────────────────────────────────────────┐
│                       APIC 系统架构                                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   ┌─────────────────────────────────────────────────────────────────┐  │
│   │                         系统总线                                 │  │
│   └─────────────────────────────────────────────────────────────────┘  │
│         │              │              │              │                  │
│         ▼              ▼              ▼              ▼                  │
│   ┌──────────┐   ┌──────────┐   ┌──────────┐   ┌──────────┐           │
│   │ Local    │   │ Local    │   │ Local    │   │ Local    │           │
│   │ APIC     │   │ APIC     │   │ APIC     │   │ APIC     │           │
│   │ (CPU0)   │   │ (CPU1)   │   │ (CPU2)   │   │ (CPU3)   │           │
│   │          │   │          │   │          │   │          │           │
│   │ ┌──────┐ │   │ ┌──────┐ │   │ ┌──────┐ │   │ ┌──────┐ │           │
│   │ │Timer │ │   │ │Timer │ │   │ │Timer │ │   │ │Timer │ │           │
│   │ │LVT   │ │   │ │LVT   │ │   │ │LVT   │ │   │ │LVT   │ │           │
│   │ │Perf  │ │   │ │Perf  │ │   │ │Perf  │ │   │ │Perf  │ │           │
│   │ │Thermal│ │   │ │Thermal│ │   │ │Thermal│ │   │ │Thermal│ │           │
│   │ │Error │ │   │ │Error │ │   │ │Error │ │   │ │Error │ │           │
│   │ └──────┘ │   │ └──────┘ │   │ └──────┘ │   │ └──────┘ │           │
│   └────┬─────┘   └────┬─────┘   └────┬─────┘   └────┬─────┘           │
│        │              │              │              │                  │
│        └──────────────┴──────┬───────┴──────────────┘                  │
│                              │                                         │
│                        ┌─────┴─────┐                                   │
│                        │   APIC    │                                   │
│                        │   BUS     │                                   │
│                        │(串行总线) │                                   │
│                        └─────┬─────┘                                   │
│                              │                                         │
│                        ┌─────┴─────┐                                   │
│                        │  I/O APIC │                                   │
│                        │           │                                   │
│   设备IRQ0 ───────────→│  RTE0     │                                   │
│   设备IRQ1 ───────────→│  RTE1     │  Redirection Table (24 entries)   │
│   设备IRQ2 ───────────→│  RTE2     │                                   │
│      ...               │   ...     │                                   │
│   设备IRQ23 ──────────→│  RTE23    │                                   │
│                        │           │                                   │
│   寄存器访问:          │  ID       │◄── 0xFEC00000 (MMIO)              │
│   Memory Mapped I/O    │  VER      │                                   │
│                        └───────────┘                                   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 3.2 Local APIC寄存器

```c
/*
 * Local APIC 寄存器定义
 * arch/x86/include/asm/apicdef.h
 */

#define APIC_DEFAULT_PHYS_BASE  0xFEE00000

// Local APIC 寄存器偏移
#define APIC_ID         0x020   // Local APIC ID
#define APIC_VER        0x030   // Local APIC Version
#define APIC_TPR        0x080   // Task Priority Register
#define APIC_APR        0x090   // Arbitration Priority Register
#define APIC_PPR        0x0A0   // Processor Priority Register
#define APIC_EOI        0x0B0   // End of Interrupt
#define APIC_RRR        0x0C0   // Remote Read Register
#define APIC_LDR        0x0D0   // Logical Destination Register
#define APIC_DFR        0x0E0   // Destination Format Register
#define APIC_SVR        0x0F0   // Spurious Interrupt Vector
#define APIC_ISR        0x100   // In-Service Register (256-bit)
#define APIC_TMR        0x180   // Trigger Mode Register (256-bit)
#define APIC_IRR        0x200   // Interrupt Request Register (256-bit)
#define APIC_ESR        0x280   // Error Status Register
#define APIC_ICR        0x300   // Interrupt Command Register (64-bit)
#define APIC_ICR2       0x310   // ICR[63:32]
#define APIC_LVTT       0x320   // LVT Timer Register
#define APIC_LVTTHMR    0x330   // LVT Thermal Monitor
#define APIC_LVTPC      0x340   // LVT Performance Counter
#define APIC_LVT0       0x350   // LVT LINT0
#define APIC_LVT1       0x360   // LVT LINT1
#define APIC_LVTERR     0x370   // LVT Error
#define APIC_TMICT      0x380   // Timer Initial Count
#define APIC_TMCCT      0x390   // Timer Current Count
#define APIC_TDCR       0x3E0   // Timer Divide Configuration

// ICR (Interrupt Command Register) 定义
#define APIC_DEST_SELF          0x40000
#define APIC_DEST_ALLINC        0x80000
#define APIC_DEST_ALLBUT        0xC0000
#define APIC_DEST_LOGICAL       0x800
#define APIC_DM_FIXED           0x000
#define APIC_DM_LOWEST          0x100
#define APIC_DM_SMI             0x200
#define APIC_DM_NMI             0x400
#define APIC_DM_INIT            0x500
#define APIC_DM_STARTUP         0x600
#define APIC_DM_EXTINT          0x700
#define APIC_VECTOR_MASK        0x000FF
```

### 3.3 APIC初始化代码 (Linux)

```c
/*
 * Linux内核 Local APIC 初始化
 * arch/x86/kernel/apic/apic.c
 */

static void __init setup_local_APIC(void)
{
    u32 value;

    // 1. 设置Task Priority Register (TPR)
    // 设置当前任务的优先级，屏蔽优先级低于此值的中断
    apic_write(APIC_TPR, 0);

    // 2. 设置Spurious Interrupt Vector Register (SVR)
    // 使能APIC，设置伪中断向量
    value = apic_read(APIC_SVR);
    value &= ~APIC_VECTOR_MASK;
    value |= APIC_SPIV_APIC_ENABLED;  // 使能APIC
    value |= 0xFF & SPURIOUS_APIC_VECTOR;  // 设置伪中断向量
    apic_write(APIC_SVR, value);

    // 3. 设置Logical Destination Register (LDR)
    // 用于逻辑中断投递
    value = apic_read(APIC_LDR) & ~APIC_LDR_MASK;
    value |= (1 << (smp_processor_id() + 24));
    apic_write(APIC_LDR, value);

    // 4. 设置Destination Format Register (DFR)
    // 平坦模式: 所有位为1
    apic_write(APIC_DFR, APIC_DFR_FLAT);

    // 5. 配置LVT条目
    // LVT Timer: 一次性模式，向量0xEF
    value = APIC_LVT_TIMER_ONESHOT | LOCAL_TIMER_VECTOR;
    apic_write(APIC_LVTT, value);

    // LVT Error: 使能错误中断
    value = ERROR_APIC_VECTOR;
    apic_write(APIC_LVTERR, value);

    // LVT LINT0/LINT1: 配置为 ExtINT 和 NMI (如果使用)
    value = apic_read(APIC_LVT0);
    value &= ~(APIC_MODE_MASK | APIC_SEND_PENDING | APIC_INPUT_POLARITY);
    value |= APIC_LVT_MASKED;  // 默认屏蔽
    apic_write(APIC_LVT0, value);

    value = apic_read(APIC_LVT1);
    value &= ~(APIC_MODE_MASK | APIC_SEND_PENDING | APIC_INPUT_POLARITY);
    value |= APIC_LVT_MASKED;
    apic_write(APIC_LVT1, value);

    // 6. 清除错误状态寄存器
    apic_write(APIC_ESR, 0);
    apic_write(APIC_ESR, 0);

    // 7. 初始化Timer Divide Configuration
    // 设置分频比为1
    apic_write(APIC_TDCR, APIC_TDR_DIV_1);
}

// 发送IPI (Inter-Processor Interrupt) - 处理器间中断
void apic_send_IPI(int cpu, int vector)
{
    u32 icr_low, icr_high;
    unsigned long flags;

    local_irq_save(flags);

    // 设置目标CPU
    icr_high = SET_APIC_DEST_FIELD(cpu_physical_id(cpu));
    apic_write(APIC_ICR2, icr_high);

    // 设置中断向量
    icr_low = APIC_DM_FIXED | vector;
    apic_write(APIC_ICR, icr_low);

    local_irq_restore(flags);
}
```

---

## 4. 中断优先级处理

### 4.1 优先级编码机制

```
┌─────────────────────────────────────────────────────────────────────┐
│                    中断优先级编码                                    │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   8位中断向量号的高4位决定优先级:                                    │
│                                                                     │
│   向量号范围        优先级    用途                                  │
│   ───────────────────────────────────                               │
│   0x00 - 0x1F       最高      CPU保留异常                           │
│   0x20 - 0x2F       高        硬件中断 (ISA)                        │
│   0x30 - 0x7F       中高      硬件中断 (PCI等)                      │
│   0x80 - 0x8F       中        Linux系统调用                         │
│   0x90 - 0xEF       中低      动态分配                              │
│   0xF0 - 0xFE       低        特殊用途 (IPI等)                      │
│   0xFF              最低      APIC Spurious                         │
│                                                                     │
│   优先级判定规则:                                                    │
│   ┌────────────────────────────────────────┐                       │
│   │  数值越小，优先级越高                  │                       │
│   │  0x00 (优先级15) > 0x10 (优先级1)      │                       │
│   │  0x20 (优先级2) > 0x21 (优先级2)       │                       │
│   │  向量号低4位用于同优先级内排序         │                       │
│   └────────────────────────────────────────┘                       │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 4.2 优先级比较机制

```
┌─────────────────────────────────────────────────────────────────────┐
│                中断优先级比较逻辑                                    │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   当前处理器状态:                                                    │
│   ┌─────────┐  ┌─────────┐  ┌─────────┐                            │
│   │  TPR    │  │  PPR    │  │  ISR    │                            │
│   │Task Prio│  │Proc Prio│  │In-Service│                            │
│   │  0x30   │  │  0x30   │  │  0x20   │                            │
│   └────┬────┘  └────┬────┘  └────┬────┘                            │
│        └─────────────┴─────────────┘                                │
│                      │                                              │
│                      ▼                                              │
│              ┌───────────────┐                                      │
│              │  优先级比较器  │                                      │
│              └───────┬───────┘                                      │
│                      │                                              │
│                      ▼                                              │
│   新中断请求 ──IR0──→│                                              │
│   向量 0x21 ────────→│  比较: 0x21(优先级2) < 0x30(TPR)?           │
│                      │  结果: 是，允许中断                           │
│                      │                                              │
│   新中断请求 ──IR1──→│                                              │
│   向量 0x35 ────────→│  比较: 0x35(优先级3) < 0x30(TPR)?           │
│                      │  结果: 否，中断被屏蔽                         │
│                      │                                              │
└─────────────────────────────────────────────────────────────────────┘
```

### 4.3 Linux优先级管理

```c
/*
 * Linux内核中断优先级管理
 * include/linux/interrupt.h
 */

// IRQ描述符中的优先级相关字段
struct irq_desc {
    struct irq_data     irq_data;
    unsigned int        __percpu *kstat_irqs;
    irq_flow_handler_t  handle_irq;
    struct irqaction    *action;
    unsigned int        status_use_accessors;

    // 亲和性和优先级设置
    cpumask_var_t       affinity;
    cpumask_var_t       effective_affinity;
    unsigned int        affinity_notify;

    // 中断线程化相关
    struct task_struct  *threads;
    struct irqaction    *threads_oneshot;
};

// 设置中断亲和性 (绑定到特定CPU)
int irq_set_affinity(unsigned int irq, const struct cpumask *cpumask)
{
    struct irq_desc *desc = irq_to_desc(irq);

    if (!desc)
        return -EINVAL;

    // 更新亲和性掩码
    cpumask_copy(desc->irq_data.affinity, cpumask);

    // 通知中断控制器更新
    if (desc->irq_data.chip->irq_set_affinity)
        return desc->irq_data.chip->irq_set_affinity(&desc->irq_data,
                                                      cpumask, false);
    return -EINVAL;
}

// IRQF_* 标志定义中断优先级/类型
#define IRQF_TRIGGER_RISING     0x00000001
#define IRQF_TRIGGER_FALLING    0x00000002
#define IRQF_TRIGGER_HIGH       0x00000004
#define IRQF_TRIGGER_LOW        0x00000008
#define IRQF_TRIGGER_MASK       (IRQF_TRIGGER_HIGH | IRQF_TRIGGER_LOW | \
                                 IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING)
#define IRQF_SHARED             0x00000080   // 可共享中断
#define IRQF_IRQPOLL            0x00001000   // 用于IRQ轮询
#define IRQF_ONESHOT            0x00002000   // 硬中断处理完后屏蔽，直到线程处理完
#define IRQF_NO_SUSPEND         0x00004000   // 系统挂起期间不禁用
#define IRQF_FORCE_RESUME       0x00008000   // 系统恢复时强制使能
#define IRQF_NO_THREAD          0x00010000   // 禁止线程化
#define IRQF_EARLY_RESUME       0x00020000   // 系统恢复时提前使能
#define IRQF_COND_SUSPEND       0x00040000   // 条件挂起
```

---

## 5. 中断向量表

### 5.1 IVT (实模式) vs IDT (保护模式)

```
┌─────────────────────────────────────────────────────────────────────┐
│              实模式 IVT vs 保护模式 IDT                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   实模式 IVT (Interrupt Vector Table):                              │
│   ┌─────────────────────────────────────────────────────────────┐  │
│   │  地址范围: 0x00000 - 0x003FF (1KB)                          │  │
│   │                                                             │  │
│   │  0x0000: ┌─────────────────┐  中断0 (除零)                  │  │
│   │          │  IP (2 bytes)   │                                │  │
│   │          │  CS (2 bytes)   │                                │  │
│   │          └─────────────────┘                                │  │
│   │  0x0004: ┌─────────────────┐  中断1 (调试)                  │  │
│   │          │  IP (2 bytes)   │                                │  │
│   │          │  CS (2 bytes)   │                                │  │
│   │          └─────────────────┘                                │  │
│   │            ...                                              │  │
│   │  0x03FC: ┌─────────────────┐  中断255                       │  │
│   │          │  IP (2 bytes)   │                                │  │
│   │          │  CS (2 bytes)   │                                │  │
│   │          └─────────────────┘                                │  │
│   └─────────────────────────────────────────────────────────────┘  │
│                                                                     │
│   保护模式 IDT (Interrupt Descriptor Table):                        │
│   ┌─────────────────────────────────────────────────────────────┐  │
│   │  每个条目8字节 (32位) 或 16字节 (64位/x86-64)               │  │
│   │                                                             │  │
│   │  ┌────────┬────────┬────────┬────────┬────────┬────────┐   │  │
│   │  │Offset  │ Segment│  Zero  │  Flags │ Offset │        │   │  │
│   │  │[31:16] │Selector│        │  Type  │[15:0]  │        │   │  │
│   │  │ 16bits │ 16bits │ 8bits  │ 8bits  │ 16bits │        │   │  │
│   │  └────────┴────────┴────────┴────────┴────────┴────────┘   │  │
│   │                                                             │  │
│   │  Flags格式:                                                 │  │
│   │  ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐         │  │
│   │  │  P  │  DPL│  DPL│  0  │  D  │ 1   │ 1   │ 1   │         │  │
│   │  │Present│ Priv │Lvl │     │Size │ Type=Interrupt Gate   │  │
│   │  └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘         │  │
│   │                                                             │  │
│   └─────────────────────────────────────────────────────────────┘  │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 5.2 IDT条目结构

```c
/*
 * x86_64 IDT门描述符结构
 * arch/x86/include/asm/desc_defs.h
 */

// 32位中断门描述符 (8字节)
struct gate_struct32 {
    u16 offset_low;
    u16 segment;
    unsigned ist : 3, zero0 : 5, type : 5, dpl : 2, p : 1;
    u16 offset_high;
} __attribute__((packed));

// 64位中断门描述符 (16字节)
struct gate_struct64 {
    u16 offset_low;
    u16 segment;
    unsigned ist : 3, zero0 : 5, type : 5, dpl : 2, p : 1;
    u16 offset_middle;
    u32 offset_high;
    u32 zero1;
} __attribute__((packed));

// IDT描述符类型
#define GATE_INTERRUPT  0xE     // 中断门 (禁止IF)
#define GATE_TRAP       0xF     // 陷阱门 (不禁止IF)
#define GATE_CALL       0xC     // 调用门
#define GATE_TASK       0x5     // 任务门

// Linux内核IDT初始化
typedef struct gate_struct gate_desc;

// 设置IDT门描述符的宏
#define _set_gate(gate_addr, type, dpl, addr, ist)          \
{                                                           \
    gate_desc *gate = (gate_desc *)(gate_addr);             \
    u64 a = (u64)(addr);                                    \
    gate->offset_low = (u16)(a);                            \
    gate->segment = __KERNEL_CS;                            \
    gate->ist = ist;                                        \
    gate->p = 1;                                            \
    gate->dpl = (dpl);                                      \
    gate->zero0 = 0;                                        \
    gate->type = (type);                                    \
    gate->offset_middle = (u16)((a) >> 16);                 \
    gate->offset_high = (u32)((a) >> 32);                   \
    gate->zero1 = 0;                                        \
}

// 设置中断门
#define set_intr_gate_notrace(n, addr)                      \
    _set_gate(&idt_table[n], GATE_INTERRUPT, 0, addr, 0)

// 设置系统门 (允许用户态调用，如int 0x80)
#define set_system_intr_gate(n, addr)                       \
    _set_gate(&idt_table[n], GATE_INTERRUPT, 3, addr, 0)

// 设置陷阱门
#define set_trap_gate(n, addr)                              \
    _set_gate(&idt_table[n], GATE_TRAP, 0, addr, 0)
```

### 5.3 Linux IDT布局

```c
/*
 * Linux内核 IDT 向量分配
 * arch/x86/include/asm/irq_vectors.h
 */

// CPU异常向量 (0-31)
#define X86_TRAP_DE         0   // 除零错误
#define X86_TRAP_DB         1   // 调试
#define X86_TRAP_NMI        2   // 非屏蔽中断
#define X86_TRAP_BP         3   // 断点
#define X86_TRAP_OF         4   // 溢出
#define X86_TRAP_BR         5   // 边界检查
#define X86_TRAP_UD         6   // 非法指令
#define X86_TRAP_NM         7   // 设备不可用
#define X86_TRAP_DF         8   // 双重错误
#define X86_TRAP_OLD_MF     9   // 协处理器段溢出
#define X86_TRAP_TS         10  // 无效TSS
#define X86_TRAP_NP         11  // 段不存在
#define X86_TRAP_SS         12  // 栈段错误
#define X86_TRAP_GP         13  // 通用保护错误
#define X86_TRAP_PF         14  // 页错误
#define X86_TRAP_SPURIOUS   15  // 预留/伪中断
#define X86_TRAP_MF         16  // x87浮点错误
#define X86_TRAP_AC         17  // 对齐检查
#define X86_TRAP_MC         18  // 机器检查
#define X86_TRAP_XF         19  // SIMD浮点错误
#define X86_TRAP_VE         20  // 虚拟化异常

// ISA中断向量 (传统PIC)
#define IRQ0_VECTOR         0x20
#define IRQ1_VECTOR         0x21
#define IRQ2_VECTOR         0x22
#define IRQ3_VECTOR         0x23
#define IRQ4_VECTOR         0x24
#define IRQ5_VECTOR         0x25
#define IRQ6_VECTOR         0x26
#define IRQ7_VECTOR         0x27
#define IRQ8_VECTOR         0x28
#define IRQ9_VECTOR         0x29
#define IRQ10_VECTOR        0x2A
#define IRQ11_VECTOR        0x2B
#define IRQ12_VECTOR        0x2C
#define IRQ13_VECTOR        0x2D
#define IRQ14_VECTOR        0x2E
#define IRQ15_VECTOR        0x2F

// APIC特殊向量
#define LOCAL_TIMER_VECTOR      0xEF
#define ERROR_APIC_VECTOR       0xFE
#define SPURIOUS_APIC_VECTOR    0xFF

// IPI向量
#define RESCHEDULE_VECTOR       0xF1
#define CALL_FUNCTION_VECTOR    0xF2
#define CALL_FUNCTION_SINGLE_VECTOR 0xF3
#define REBOOT_VECTOR           0xF4
```

---

## 6. 中断响应流程

### 6.1 硬件响应时序

```
┌────────────────────────────────────────────────────────────────────────────┐
│                     中断响应时序图                                          │
├────────────────────────────────────────────────────────────────────────────┤
│                                                                            │
│   CPU时钟    ─┐                                                            │
│               │    ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐              │
│               └───→│  │──│  │──│  │──│  │──│  │──│  │──│  │──              │
│                    └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──┘              │
│                                                                            │
│   INTR       ────────────────────┐                                        │
│   (请求)                         └────────────────────────────             │
│                                                                            │
│   INTA#      ────────────┐     ┌─────────┐     ┌────────────               │
│   (响应)                 └─────┘         └─────┘                           │
│                              ↑  ↑                                          │
│                    ┌─────────┘  └─────────┐                                │
│                    │    第1个INTA          │    第2个INTA                  │
│                    │    (PIC响应)          │    (获取向量)                 │
│                    └───────────────────────┘                                │
│                                                                            │
│   数据总线   ──────  ─────── 0x20 ────  ───────  ────── 向量 ─────────     │
│   (D0-D7)                    ↑                   ↑  0x21                  │
│                              │                   │                         │
│                         优先级编码            中断向量                      │
│                                                                            │
│   CPU状态    ──────  ┌─────────────┐  ┌─────────────┐  ┌────────────┐     │
│              执行   →│  当前指令   │→│  压栈保存   │→│  跳转ISR   │     │
│              指令    │   完成      │  │  EFLAGS/CS/IP│  │            │     │
│                      └─────────────┘  └─────────────┘  └────────────┘     │
│                                                                            │
│   时序说明:                                                                │
│   ├─ 指令边界检测: 在指令完成后采样INTR                                     │
│   ├─ 第1个INTA: PIC响应，准备最高优先级中断                                 │
│   ├─ 第2个INTA: PIC将中断向量号送上数据总线                                 │
│   └─ CPU跳转: 根据向量号从IDT获取ISR地址并跳转                               │
│                                                                            │
└────────────────────────────────────────────────────────────────────────────┘
```

### 6.2 软件处理流程

```
┌────────────────────────────────────────────────────────────────────────────┐
│                    中断处理软件流程                                         │
├────────────────────────────────────────────────────────────────────────────┤
│                                                                            │
│   硬件完成部分                          软件处理部分                        │
│   ┌─────────────────┐                  ┌─────────────────┐                │
│   │  1. 采样INTR    │                  │  4. 通用入口代码 │                │
│   │     IF=1?       │─────────────────→│   (asm)         │                │
│   └─────────────────┘                  │   - 保存寄存器  │                │
│            │                           │   - 设置数据段  │                │
│            ▼                           │   - 清除IF     │                │
│   ┌─────────────────┐                  └────────┬────────┘                │
│   │  2. 发送INTA    │                           │                         │
│   │     获取向量    │                           ▼                         │
│   └─────────────────┘                  ┌─────────────────┐                │
│            │                           │  5. 调用handle_  │                │
│            ▼                           │     irq()       │                │
│   ┌─────────────────┐                  │  (C语言处理)    │                │
│   │  3. 压栈保存:   │                  │   - ack中断     │                │
│   │     EFLAGS      │←─────────────────│   - 调用handler │                │
│   │     CS:IP       │                  │   - 发送EOI     │                │
│   │     ErrorCode?  │                  └────────┬────────┘                │
│   └─────────────────┘                           │                         │
│                                                 ▼                         │
│                                        ┌─────────────────┐                │
│                                        │  6. 恢复寄存器  │                │
│                                        │  7. IRET返回    │                │
│                                        └─────────────────┘                │
│                                                                            │
└────────────────────────────────────────────────────────────────────────────┘
```

### 6.3 Linux中断入口代码

```asm
/*
 * Linux内核 x86_64 中断入口汇编代码
 * arch/x86/entry/entry_64.S
 */

/* 通用中断入口宏 */
.macro interrupt_entry vector
    /* 1. 保存寄存器到栈 */
    pushq   %rax
    pushq   %rbp
    pushq   %rdi
    pushq   %rsi
    pushq   %rdx
    pushq   %rcx
    pushq   %rbx

    /* 2. 加载内核数据段 */
    movl    $__KERNEL_DS, %eax
    movl    %eax, %ds
    movl    %eax, %es

    /* 3. 检查中断栈 */
    movq    %rsp, %rdi          /* 将pt_regs指针作为参数 */

    /* 4. 调用C语言处理函数 */
    call    do_IRQ

    /* 5. 恢复寄存器 */
    popq    %rbx
    popq    %rcx
    popq    %rdx
    popq    %rsi
    popq    %rdi
    popq    %rbp
    popq    %rax

    /* 6. 中断返回 */
    iretq
.endm

/* 特定中断向量入口 */
ENTRY(irq_entries_start)
    vector = FIRST_EXTERNAL_VECTOR
    .rept NR_IRQS
    .align 8
1:
    pushq   $(vector)
    jmp     common_interrupt
    .previous
    .pushsection .rodata.irq_vectors
    .quad   1b
    .previous
    vector = vector+1
    .endr
END(irq_entries_start)

/* 通用中断处理 */
common_interrupt:
    /* 切换到中断栈 */
    addq    $-0x80, (%rsp)      /* 调整栈指针 */
    interrupt_entry vector=0

/* 异常处理入口 (带错误码) */
ENTRY(error_entry)
    /* 异常可能已经压入了错误码 */
    /* 需要处理栈对齐 */
    cld
    pushq   %rdi
    pushq   %rsi
    pushq   %rdx
    pushq   %rcx
    pushq   %rax
    pushq   %r8
    pushq   %r9
    pushq   %r10
    pushq   %r11
    pushq   %rbx
    pushq   %rbp
    pushq   %r12
    pushq   %r13
    pushq   %r14
    pushq   %r15

    /* 构建pt_regs结构 */
    movq    %rsp, %rdi
    call    do_exception

    /* 恢复并返回 */
    jmp     error_exit
END(error_entry)
```

### 6.4 C语言中断处理

```c
/*
 * Linux内核通用中断处理
 * arch/x86/kernel/irq.c
 */

// 主中断处理入口
__visible unsigned int __irq_entry do_IRQ(struct pt_regs *regs)
{
    struct pt_regs *old_regs = set_irq_regs(regs);
    struct irq_desc *desc;
    unsigned vector = ~regs->orig_ax;

    // 根据向量号获取中断描述符
    desc = __this_cpu_read(vector_irq[vector]);

    if (!desc) {
        // 未处理的中断
        ack_APIC_irq();
        goto exit;
    }

    // 通用中断处理
    generic_handle_irq_desc(desc);

exit:
    set_irq_regs(old_regs);
    return 1;
}

// 通用中断处理函数
int generic_handle_irq_desc(struct irq_desc *desc)
{
    // 调用中断控制器特定的高层级处理
    return desc->handle_irq(desc);
}

// 边缘触发中断处理 (handle_edge_irq)
void handle_edge_irq(struct irq_desc *desc)
{
    // 1. 屏蔽该中断 (直到处理完成)
    mask_ack_irq(desc);

    // 2. 检查中断是否正在处理中
    if (unlikely(irqd_irq_inprogress(&desc->irq_data))) {
        // 中断重入，延迟处理
        desc->istate |= IRQS_PENDING;
        return;
    }

    // 3. 设置处理中标志
    desc->istate |= IRQS_INPROGRESS;

    // 4. 处理中断
    do {
        // 重新使能中断 (允许嵌套)
        unmask_irq(desc);

        // 调用注册的处理函数
        handle_irq_event(desc);

        // 再次屏蔽
        mask_irq(desc);

        // 检查是否有挂起的中断
    } while ((desc->istate & IRQS_PENDING) &&
             !irqd_irq_disabled(&desc->irq_data));

    // 5. 清除处理中标志
    desc->istate &= ~IRQS_INPROGRESS;

    // 6. 最终确认和使能
    ack_irq(desc);
    unmask_irq(desc);
}

// 电平触发中断处理 (handle_level_irq)
void handle_level_irq(struct irq_desc *desc)
{
    // 1. 屏蔽并确认中断
    mask_ack_irq(desc);

    // 2. 如果中断被禁用，直接返回
    if (unlikely(irqd_irq_disabled(&desc->irq_data)))
        return;

    // 3. 处理中断事件
    handle_irq_event(desc);

    // 4. 取消屏蔽 (电平触发需要等待设备释放IRQ线)
    unmask_irq(desc);
}
```

---

## 7. 嵌套中断处理

### 7.1 嵌套中断模型

```
┌────────────────────────────────────────────────────────────────────────────┐
│                     嵌套中断处理模型                                        │
├────────────────────────────────────────────────────────────────────────────┤
│                                                                            │
│   时间轴 ───────────────────────────────────────────────────────────→     │
│                                                                            │
│   CPU0活动                                                                 │
│   ───────────────────────────────────────────────────────────────────     │
│                                                                            │
│   用户态代码                                                               │
│   ┌───────────────────────────────────────────────────────────────┐       │
│   │                                                               │       │
│   └───────────────────────────────────────────────────────────────┘       │
│                ↑                                                           │
│                │ 定时器IRQ0 (优先级高)                                      │
│                ▼                                                           │
│   ┌─────────────────────────┐                                              │
│   │  定时器ISR              │                                              │
│   │  向量 0xEF              │                                              │
│   │  优先级: 14             │                                              │
│   │                         │◄── 保存IF=1，允许更高优先级                   │
│   │    ┌───────────────┐    │                                              │
│   │    │  网络IRQ11    │    │  网络包到达 (优先级更高)                     │
│   │    │  向量 0x2B    │    │  优先级: 2                                  │
│   │    │  优先级: 2    │    │                                             │
│   │    │               │    │                                             │
│   │    │  ┌─────────┐  │    │                                             │
│   │    │  │网卡ISR  │  │    │                                             │
│   │    │  │ 发送EOI │  │    │                                             │
│   │    │  │ 恢复定时器│  │    │                                             │
│   │    │  └─────────┘  │    │                                             │
│   │    │               │    │                                             │
│   │    └───────────────┘    │                                              │
│   │         ↑               │                                              │
│   │         │ IRET返回定时器│                                              │
│   │         └───────────────┘                                              │
│   │                         │                                              │
│   │  发送EOI                │                                              │
│   │  恢复用户态             │                                              │
│   └─────────────────────────┘                                              │
│                ↑                                                           │
│                │ IRET返回用户态                                             │
│                ▼                                                           │
│   用户态代码继续...                                                        │
│                                                                            │
│   栈状态变化:                                                              │
│   ┌───────────────────────────────────────────────────────────────────┐   │
│   │ 用户态 → 定时器栈 → 网卡栈 → 网卡ISR完成 → 定时器完成 → 用户态   │   │
│   │                                                                   │   │
│   │ SS:SP                    SS:SP        恢复        恢复            │   │
│   │ EFLAGS                   EFLAGS      EFLAGS      EFLAGS           │   │
│   │ CS:IP ←用户态            CS:IP       CS:IP       CS:IP            │   │
│   │ error_code(可能)         定时器ISR   网卡ISR                      │   │
│   │                          ──────────  ──────────                   │   │
│   │                          保存寄存器  保存寄存器                   │   │
│   └───────────────────────────────────────────────────────────────────┘   │
│                                                                            │
└────────────────────────────────────────────────────────────────────────────┘
```

### 7.2 中断栈管理

```c
/*
 * Linux内核中断栈管理
 * arch/x86/include/asm/processor.h
 */

// 每个CPU的中断栈定义
#define IRQ_STACK_ORDER     2
#define IRQ_STACK_SIZE      (PAGE_SIZE << IRQ_STACK_ORDER)  // 16KB

struct irq_stack {
    u32 stack[IRQ_STACK_SIZE / sizeof(u32)];
} __aligned(IRQ_STACK_SIZE);

// 当前CPU的中断栈指针
DECLARE_PER_CPU(struct irq_stack *, irq_stack_ptr);

/*
 * 中断栈切换代码
 * arch/x86/entry/entry_64.S
 */

/* 切换到中断栈 */
.macro SWITCH_TO_INTERRUPT_STACK
    /* 检查是否已经在中断栈上 */
    movq    %rsp, %rax
    andq    $(~(IRQ_STACK_SIZE-1)), %rax
    cmpq    %rax, PER_CPU_VAR(irq_stack_ptr)
    je      1f                      /* 已经在IRQ栈上，跳过 */

    /* 切换到IRQ栈 */
    movq    %rsp, PER_CPU_VAR(irq_stack_backup)
    movq    PER_CPU_VAR(irq_stack_ptr), %rsp

1:
.endm

/* 从中断栈恢复 */
.macro RESTORE_FROM_INTERRUPT_STACK
    /* 检查是否需要恢复 */
    cmpq    PER_CPU_VAR(irq_stack_ptr), %rsp
    jbe     1f                      /* 如果低于IRQ栈基址，不需要恢复 */

    /* 恢复原栈 */
    movq    PER_CPU_VAR(irq_stack_backup), %rsp
1:
.endm

// 双重故障栈 (Double Fault Handler栈)
// 在x86_64上，使用IST (Interrupt Stack Table) 机制
#define DOUBLEFAULT_STACK   1
#define NMI_STACK           2
#define DEBUG_STACK         3
#define MCE_STACK           4

// IST条目在TSS中的偏移
#define IST_INDEX_DF        0   // 双重故障
#define IST_INDEX_NMI       1   // NMI
#define IST_INDEX_DB        2   // 调试
#define IST_INDEX_MCE       3   // 机器检查
#define IST_INDEX_VC        4   // VMM通信
```

### 7.3 中断屏蔽与使能

```c
/*
 * 中断控制原语
 * arch/x86/include/asm/irqflags.h
 */

// 本地中断使能/屏蔽 (操作EFLAGS.IF位)
static inline void native_irq_disable(void)
{
    asm volatile("cli" ::: "memory");
}

static inline void native_irq_enable(void)
{
    asm volatile("sti" ::: "memory");
}

// 保存并恢复中断状态
static inline unsigned long native_save_fl(void)
{
    unsigned long flags;
    asm volatile("pushf ; pop %0" : "=g" (flags) : : "memory");
    return flags;
}

static inline void native_restore_fl(unsigned long flags)
{
    asm volatile("push %0 ; popf" : : "g" (flags) : "memory", "cc");
}

// 同时执行的操作 (原子性)
static inline unsigned long native_irq_save(void)
{
    unsigned long flags = native_save_fl();
    native_irq_disable();
    return flags;
}

// 使用场景示例
void critical_section_example(void)
{
    unsigned long flags;

    // 进入临界区，保存并屏蔽中断
    local_irq_save(flags);

    // 临界区代码 - 不会被中断打扰
    shared_counter++;
    list_add(&entry, &head);

    // 退出临界区，恢复之前的中断状态
    local_irq_restore(flags);
}

// 嵌套中断支持
void nested_irq_handler(void)
{
    // 硬件自动清除IF
    // 但某些高优先级中断可以设置IST或使用特定门描述符

    // 如果希望允许嵌套，手动使能中断
    local_irq_enable();

    // 现在可以被更高优先级中断打断
    do_work();

    // 恢复屏蔽状态
    local_irq_disable();
}
```

---

## 8. Linux内核中断处理

### 8.1 中断子系统架构

```
┌────────────────────────────────────────────────────────────────────────────┐
│                  Linux内核中断子系统架构                                    │
├────────────────────────────────────────────────────────────────────────────┤
│                                                                            │
│   ┌───────────────────────────────────────────────────────────────────┐   │
│   │                        设备驱动层                                  │   │
│   │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐               │   │
│   │  │ 网卡驱动    │  │ 磁盘驱动    │  │ USB驱动     │               │   │
│   │  │ request_irq │  │ request_irq │  │ request_irq │               │   │
│   │  │ (handler)   │  │ (handler)   │  │ (handler)   │               │   │
│   │  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘               │   │
│   └─────────┼────────────────┼────────────────┼──────────────────────┘   │
│             │                │                │                           │
│             ▼                ▼                ▼                           │
│   ┌───────────────────────────────────────────────────────────────────┐   │
│   │                     通用中断处理层                                 │   │
│   │  ┌─────────────────────────────────────────────────────────────┐  │   │
│   │  │                    irq_desc[] 数组                           │  │   │
│   │  │  ┌─────────┐ ┌─────────┐ ┌─────────┐        ┌─────────┐    │  │   │
│   │  │  │ irq 16  │ │ irq 17  │ │ irq 18  │  ...   │ irq N   │    │  │   │
│   │  │  │  eth0   │ │  sata   │ │  usb    │        │  ...    │    │  │   │
│   │  │  │ action  │ │ action  │ │ action  │        │ action  │    │  │   │
│   │  │  │ chip    │ │ chip    │ │ chip    │        │ chip    │    │  │   │
│   │  │  └─────────┘ └─────────┘ └─────────┘        └─────────┘    │  │   │
│   │  └─────────────────────────────────────────────────────────────┘  │   │
│   │                    generic_handle_irq()                           │   │
│   └───────────────────────────────────────────────────────────────────┘   │
│                                    │                                      │
│             ┌──────────────────────┼──────────────────────┐               │
│             ▼                      ▼                      ▼               │
│   ┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐      │
│   │   8259A PIC     │    │    IO-APIC      │    │    MSI/MSI-X    │      │
│   │   (传统)        │    │   (x86 SMP)     │    │   (PCIe现代)    │      │
│   │                 │    │                 │    │                 │      │
│   │ mask/unmask     │    │ mask/unmask     │    │ mask/unmask     │      │
│   │ ack/eoi         │    │ ack/eoi         │    │ 写配置空间      │      │
│   │ set_type        │    │ set_affinity    │    │ set_affinity    │      │
│   └─────────────────┘    └─────────────────┘    └─────────────────┘      │
│                                                                            │
│   关键数据结构:                                                             │
│   - struct irq_desc: 每个IRQ一个，包含所有状态和处理信息                     │
│   - struct irq_chip: 中断控制器操作接口                                      │
│   - struct irqaction: 驱动注册的处理函数链表                                 │
│   - struct irq_data: 硬件相关的数据                                          │
│                                                                            │
└────────────────────────────────────────────────────────────────────────────┘
```

### 8.2 IRQ描述符管理

```c
/*
 * Linux内核 IRQ描述符核心数据结构
 * include/linux/irqdesc.h
 */

struct irq_desc {
    struct irq_common_data  irq_common_data;
    struct irq_data         irq_data;
    unsigned int __percpu   *kstat_irqs;

    // 中断处理入口函数
    irq_flow_handler_t      handle_irq;

    // 驱动注册的处理函数链表
    struct irqaction        *action;

    // 中断统计信息
    unsigned int            status_use_accessors;
    atomic_t                threads_active;

    // 中断线程化支持
    struct task_struct      *threads;
    struct irqaction        *threads_oneshot;

    // 亲和性和唤醒设置
    cpumask_var_t           affinity;
    cpumask_var_t           effective_affinity;

    // 电源管理
#ifdef CONFIG_PM_SLEEP
    unsigned int            nr_actions;
    unsigned int            no_suspend_depth;
    unsigned int            cond_suspend_depth;
    unsigned int            force_resume_depth;
#endif

#ifdef CONFIG_PROC_FS
    struct proc_dir_entry   *dir;
#endif

    // 核心锁
    struct mutex            request_mutex;
};

// irqaction - 驱动注册的处理函数
struct irqaction {
    irq_handler_t           handler;        // 硬中断处理函数
    void                    *dev_id;        // 设备标识
    void __percpu           *percpu_dev_id;
    struct irqaction        *next;          // 链表下一个 (共享中断)
    irq_handler_t           thread_fn;      // 线程处理函数
    struct task_struct      *thread;
    struct irqaction        *secondary;

    // 标志和名称
    unsigned int            irq;
    unsigned int            flags;
    unsigned long           thread_flags;
    unsigned long           thread_mask;
    const char              *name;
    struct proc_dir_entry   *dir;
};

// irq_chip - 中断控制器操作接口
struct irq_chip {
    const char              *name;
    unsigned int            (*irq_startup)(struct irq_data *data);
    void                    (*irq_shutdown)(struct irq_data *data);
    void                    (*irq_enable)(struct irq_data *data);
    void                    (*irq_disable)(struct irq_data *data);

    // 核心操作
    void                    (*irq_ack)(struct irq_data *data);
    void                    (*irq_mask)(struct irq_data *data);
    void                    (*irq_mask_ack)(struct irq_data *data);
    void                    (*irq_unmask)(struct irq_data *data);
    void                    (*irq_eoi)(struct irq_data *data);

    // 高级特性
    int                     (*irq_set_affinity)(struct irq_data *data,
                                                 const struct cpumask *dest,
                                                 bool force);
    int                     (*irq_retrigger)(struct irq_data *data);
    int                     (*irq_set_type)(struct irq_data *data, unsigned int flow_type);
    int                     (*irq_set_wake)(struct irq_data *data, unsigned int on);

    // ... 更多回调
};
```

### 8.3 驱动注册中断示例

```c
/*
 * Linux设备驱动中断注册完整示例
 * 以网卡驱动为例
 */

#include <linux/interrupt.h>
#include <linux/netdevice.h>

struct my_netdev {
    struct net_device *netdev;
    void __iomem *mmio_base;
    int irq;
    struct napi_struct napi;

    // 统计信息
    u64 rx_packets;
    u64 rx_bytes;
    u64 tx_packets;
    u64 tx_bytes;
};

/*
 * 硬中断处理函数 (Top Half)
 * 特点: 快速执行，屏蔽当前IRQ
 */
static irqreturn_t my_netdev_irq_handler(int irq, void *dev_id)
{
    struct my_netdev *priv = dev_id;
    u32 status;

    // 读取设备状态寄存器
    status = readl(priv->mmio_base + ISR_REG);

    // 检查是否是我们设备的中断
    if (!(status & (RX_INT | TX_INT | ERR_INT)))
        return IRQ_NONE;    // 不是我们的中断

    // 禁用中断 (NAPI轮询模式)
    writel(0, priv->mmio_base + IER_REG);

    // 调度NAPI进行软中断处理
    napi_schedule(&priv->napi);

    return IRQ_HANDLED;
}

/*
 * NAPI轮询函数 (Bottom Half)
 * 特点: 可以睡眠，处理大量数据包
 */
static int my_netdev_poll(struct napi_struct *napi, int budget)
{
    struct my_netdev *priv = container_of(napi, struct my_netdev, napi);
    int work_done = 0;

    // 处理接收数据包，最多budget个
    while (work_done < budget) {
        struct sk_buff *skb;

        // 检查是否有数据
        if (!(readl(priv->mmio_base + RX_STATUS) & RX_AVAIL))
            break;

        // 分配skb并接收数据
        skb = netdev_alloc_skb(priv->netdev, RX_BUF_SIZE);
        if (!skb)
            break;

        // 从设备读取数据到skb
        // ... DMA同步，复制数据等

        // 提交给网络栈
        napi_gro_receive(napi, skb);

        priv->rx_packets++;
        work_done++;
    }

    // 如果处理完所有数据，退出NAPI模式
    if (work_done < budget) {
        napi_complete_done(napi, work_done);

        // 重新使能中断
        writel(RX_INT | TX_INT, priv->mmio_base + IER_REG);
    }

    return work_done;
}

/*
 * 设备初始化 - 注册中断
 */
static int my_netdev_open(struct net_device *netdev)
{
    struct my_netdev *priv = netdev_priv(netdev);
    int ret;

    // 分配NAPI
    netif_napi_add(netdev, &priv->napi, my_netdev_poll, 64);

    // 1. 请求中断 (传统方式)
    ret = request_irq(priv->irq,                     // IRQ号
                      my_netdev_irq_handler,         // 处理函数
                      IRQF_SHARED,                   // 共享中断标志
                      netdev->name,                  // 设备名称
                      priv);                         // dev_id
    if (ret) {
        pr_err("Failed to request IRQ %d\n", priv->irq);
        netif_napi_del(&priv->napi);
        return ret;
    }

    // 2. 或者使用线程化中断 (更简单)
    // ret = request_threaded_irq(priv->irq,
    //                            my_netdev_irq_handler,  // 硬中断
    //                            my_netdev_thread_fn,    // 线程函数
    //                            IRQF_ONESHOT,
    //                            netdev->name, priv);

    // 3. 或者使用 managed 版本 (自动释放)
    // ret = devm_request_irq(&pdev->dev, priv->irq, ...);

    // 初始化硬件
    // ...

    // 使能NAPI
    napi_enable(&priv->napi);

    // 使能设备中断
    writel(RX_INT | TX_INT, priv->mmio_base + IER_REG);

    netif_start_queue(netdev);

    return 0;
}

/*
 * 设备关闭 - 释放中断
 */
static int my_netdev_stop(struct net_device *netdev)
{
    struct my_netdev *priv = netdev_priv(netdev);

    // 停止网络队列
    netif_stop_queue(netdev);

    // 禁用NAPI
    napi_disable(&priv->napi);

    // 禁用设备中断
    writel(0, priv->mmio_base + IER_REG);

    // 同步中断
    synchronize_irq(priv->irq);

    // 释放中断
    free_irq(priv->irq, priv);

    // 删除NAPI
    netif_napi_del(&priv->napi);

    return 0;
}
```

---

## 9. Verilog实现框架

### 9.1 简化中断控制器设计

```verilog
//============================================================================
// 简化版可编程中断控制器 (PIC)
// 参考: 8259A 功能子集
//============================================================================

module pic_simple (
    // 系统信号
    input  wire        clk,
    input  wire        rst_n,

    // CPU接口
    input  wire [7:0]  cpu_data_in,     // CPU写入数据
    output reg  [7:0]  cpu_data_out,    // CPU读取数据
    input  wire [1:0]  cpu_addr,        // 寄存器地址 (0=命令, 1=数据)
    input  wire        cpu_rd_n,        // 读使能 (低有效)
    input  wire        cpu_wr_n,        // 写使能 (低有效)
    output reg         cpu_int,         // 中断请求输出
    input  wire        cpu_inta_n,      // 中断响应 (低有效)

    // 中断请求输入 (8个IR线)
    input  wire [7:0]  ir_lines,        // 中断请求输入

    // 级联接口
    output reg  [2:0]  cascade_addr,    // 级联地址
    input  wire        cascade_in,      // 从片模式输入
    output wire        cascade_out      // 主片输出
);

    //------------------------------------------------------------------------
    // 寄存器定义
    //------------------------------------------------------------------------

    // IRR - 中断请求寄存器
    reg [7:0] irr_reg;

    // ISR - 服务中寄存器
    reg [7:0] isr_reg;

    // IMR - 中断屏蔽寄存器
    reg [7:0] imr_reg;

    // ICW1-4 - 初始化命令字
    reg icw1_received;
    reg icw2_received;
    reg icw3_received;
    reg icw4_received;
    reg [7:0] icw1_reg;
    reg [7:0] icw2_reg;
    reg [7:0] icw3_reg;
    reg [7:0] icw4_reg;

    // OCW1-3 - 操作命令字
    reg [7:0] ocw2_reg;
    reg [7:0] ocw3_reg;

    // 状态机
    localparam STATE_IDLE      = 3'b000;
    localparam STATE_ICW2      = 3'b001;
    localparam STATE_ICW3      = 3'b010;
    localparam STATE_ICW4      = 3'b011;
    localparam STATE_READY     = 3'b100;
    localparam STATE_INTA_1    = 3'b101;
    localparam STATE_INTA_2    = 3'b110;

    reg [2:0] state;
    reg [2:0] next_state;

    // 中断向量基址
    wire [7:0] vector_base = icw2_reg & 8'hF8;

    // 最高优先级中断
    reg [2:0] highest_irq;
    reg [7:0] highest_mask;

    // 自动EOI使能
    wire auto_eoi = icw4_reg[1];

    //------------------------------------------------------------------------
    // 优先级编码器 - 查找最高优先级中断
    //------------------------------------------------------------------------

    wire [7:0] valid_irqs = irr_reg & ~imr_reg;

    always @(*) begin
        highest_irq = 3'd7;  // 默认最低优先级
        highest_mask = 8'b00000000;

        // 优先级: IR0 > IR1 > ... > IR7 (固定优先级)
        if (valid_irqs[0]) begin highest_irq = 3'd0; highest_mask = 8'b00000001; end
        else if (valid_irqs[1]) begin highest_irq = 3'd1; highest_mask = 8'b00000010; end
        else if (valid_irqs[2]) begin highest_irq = 3'd2; highest_mask = 8'b00000100; end
        else if (valid_irqs[3]) begin highest_irq = 3'd3; highest_mask = 8'b00001000; end
        else if (valid_irqs[4]) begin highest_irq = 3'd4; highest_mask = 8'b00010000; end
        else if (valid_irqs[5]) begin highest_irq = 3'd5; highest_mask = 8'b00100000; end
        else if (valid_irqs[6]) begin highest_irq = 3'd6; highest_mask = 8'b01000000; end
        else if (valid_irqs[7]) begin highest_irq = 3'd7; highest_mask = 8'b10000000; end
    end

    //------------------------------------------------------------------------
    // 状态机 - 控制中断响应流程
    //------------------------------------------------------------------------

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state <= STATE_IDLE;
        end else begin
            state <= next_state;
        end
    end

    always @(*) begin
        next_state = state;

        case (state)
            STATE_IDLE: begin
                if (!cpu_wr_n && cpu_addr == 2'b00 && cpu_data_in[4])
                    next_state = STATE_ICW2;  // 收到ICW1
                else if (|valid_irqs)
                    next_state = STATE_READY;  // 有待处理中断
            end

            STATE_ICW2: begin
                if (!cpu_wr_n && cpu_addr == 2'b01)
                    next_state = icw1_reg[0] ? STATE_ICW4 : STATE_READY;
            end

            STATE_ICW4: begin
                if (!cpu_wr_n && cpu_addr == 2'b01)
                    next_state = STATE_READY;
            end

            STATE_READY: begin
                if (!cpu_inta_n)
                    next_state = STATE_INTA_1;
            end

            STATE_INTA_1: begin
                if (cpu_inta_n)
                    next_state = STATE_INTA_2;
            end

            STATE_INTA_2: begin
                if (auto_eoi)
                    next_state = STATE_READY;
                else if (!cpu_wr_n && cpu_addr == 2'b00 && cpu_data_in[5])
                    next_state = STATE_READY;  // 收到EOI
            end

            default: next_state = STATE_IDLE;
        endcase
    end

    //------------------------------------------------------------------------
    // 寄存器操作
    //------------------------------------------------------------------------

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            irr_reg <= 8'b00000000;
            isr_reg <= 8'b00000000;
            imr_reg <= 8'b11111111;  // 默认屏蔽所有中断

            icw1_reg <= 8'b00000000;
            icw2_reg <= 8'b00000000;
            icw3_reg <= 8'b00000000;
            icw4_reg <= 8'b00000000;

            ocw2_reg <= 8'b00000000;
            ocw3_reg <= 8'b00000000;

            cpu_data_out <= 8'b00000000;
            cpu_int <= 1'b0;
            cascade_addr <= 3'b000;
        end else begin
            // 采样中断请求 (边沿触发)
            irr_reg <= irr_reg | (ir_lines & ~irr_reg);

            // CPU写操作
            if (!cpu_wr_n) begin
                case (cpu_addr)
                    2'b00: begin  // 命令寄存器
                        if (cpu_data_in[4]) begin
                            // ICW1
                            icw1_reg <= cpu_data_in;
                        end else if (cpu_data_in[3:0] == 4'b0010) begin
                            // OCW2 - EOI命令
                            if (cpu_data_in[5]) begin  // EOI
                                if (cpu_data_in[6]) begin  // 特定EOI
                                    isr_reg[cpu_data_in[2:0]] <= 1'b0;
                                end else begin
                                    // 非特定EOI - 清除最高优先级ISR位
                                    isr_reg <= isr_reg & ~highest_mask;
                                end
                            end
                            ocw2_reg <= cpu_data_in;
                        end else if (cpu_data_in[3:0] == 4'b1000) begin
                            // OCW3
                            ocw3_reg <= cpu_data_in;
                        end
                    end

                    2'b01: begin  // 数据寄存器
                        case (state)
                            STATE_ICW2: begin
                                icw2_reg <= cpu_data_in;
                            end
                            STATE_ICW4: begin
                                icw4_reg <= cpu_data_in;
                            end
                            default: begin
                                // OCW1 - 设置IMR
                                imr_reg <= cpu_data_in;
                            end
                        endcase
                    end
                endcase
            end

            // CPU读操作
            if (!cpu_rd_n) begin
                case (cpu_addr)
                    2'b00: cpu_data_out <= irr_reg;  // 读IRR
                    2'b01: cpu_data_out <= imr_reg;  // 读IMR
                endcase
            end

            // 中断响应序列
            case (state)
                STATE_READY: begin
                    cpu_int <= |valid_irqs;
                end

                STATE_INTA_1: begin
                    cpu_int <= 1'b0;
                    // 第一个INTA周期: 准备响应
                    cascade_addr <= highest_irq;
                end

                STATE_INTA_2: begin
                    // 第二个INTA周期: 将向量号送上数据总线
                    cpu_data_out <= vector_base | {5'b00000, highest_irq};

                    // 将IRR对应位清0，ISR对应位置1
                    irr_reg <= irr_reg & ~highest_mask;
                    isr_reg <= isr_reg | highest_mask;

                    // 自动EOI处理
                    if (auto_eoi) begin
                        isr_reg <= isr_reg & ~highest_mask;
                    end
                end
            endcase
        end
    end

    assign cascade_out = cpu_int;

endmodule


//============================================================================
// Local APIC 简化模型
//============================================================================

module local_apic_simple (
    input  wire        clk,
    input  wire        rst_n,

    // CPU接口
    input  wire [31:0] cpu_addr,
    input  wire [63:0] cpu_wdata,
    output reg  [63:0] cpu_rdata,
    input  wire        cpu_wr,
    input  wire        cpu_rd,
    output wire        cpu_int,         // 中断输出到CPU

    // APIC总线接口
    output wire [31:0] apic_bus_addr,
    output wire [63:0] apic_bus_data,
    output wire        apic_bus_valid,
    input  wire [63:0] apic_bus_rdata,
    input  wire        apic_bus_rvalid,

    // 外部中断输入 (LVT)
    input  wire [3:0]  ext_int,
    input  wire        nmi_in,
    input  wire        init_in
);

    // Local APIC ID
    reg [31:0] apic_id;

    // Local APIC Version
    localparam APIC_VERSION = 32'h00050014;  // Version 0x14, Max LVT 5

    // Task Priority Register
    reg [31:0] tpr;
    wire [7:0] task_priority = tpr[7:4];

    // Processor Priority Register
    reg [31:0] ppr;

    // End of Interrupt
    reg [31:0] eoi;

    // Spurious Interrupt Vector
    reg [31:0] svr;
    wire apic_enabled = svr[8];
    wire [7:0] spurious_vector = svr[7:0];

    // In-Service Register (256 bits)
    reg [7:0] isr[0:31];

    // Trigger Mode Register
    reg [7:0] tmr[0:31];

    // Interrupt Request Register
    reg [7:0] irr[0:31];

    // Error Status Register
    reg [31:0] esr;

    // LVT寄存器
    reg [31:0] lvt_timer;
    reg [31:0] lvt_thermal;
    reg [31:0] lvt_perf;
    reg [31:0] lvt_lint0;
    reg [31:0] lvt_lint1;
    reg [31:0] lvt_error;

    // Timer寄存器
    reg [31:0] timer_initial;
    reg [31:0] timer_current;
    reg [31:0] timer_divide;

    // ICR (Interrupt Command Register)
    reg [63:0] icr;

    // 定时器逻辑
    reg [3:0] timer_div;
    reg [31:0] timer_counter;
    reg timer_tick;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            timer_div <= 4'b1011;  // 分频1
            timer_counter <= 32'b0;
            timer_tick <= 1'b0;
        end else begin
            case (timer_divide[2:0])
                3'b000: timer_tick <= 1'b1;                          // 2
                3'b001: timer_tick <= (timer_counter[0] == 1'b1);    // 4
                3'b010: timer_tick <= (timer_counter[1:0] == 2'b11); // 8
                3'b011: timer_tick <= (timer_counter[2:0] == 3'b111);// 16
                3'b100: timer_tick <= 1'b1;                          // 32
                3'b101: timer_tick <= (timer_counter[0] == 1'b1);    // 64
                3'b110: timer_tick <= (timer_counter[1:0] == 2'b11); // 128
                3'b111: timer_tick <= (timer_counter[2:0] == 3'b111);// 1
            endcase
            timer_counter <= timer_counter + 1;
        end
    end

    // 定时器递减
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            timer_current <= 32'b0;
        end else if (timer_tick && apic_enabled && !(lvt_timer[16])) begin
            if (timer_current > 0) begin
                timer_current <= timer_current - 1;
                if (timer_current == 1) begin
                    // 定时器到期，设置IRR
                    irr[lvt_timer[7:0] / 8][lvt_timer[7:0] % 8] <= 1'b1;
                    // 如果是周期模式，重新加载
                    if (lvt_timer[17]) begin
                        timer_current <= timer_initial;
                    end
                end
            end
        end
    end

    // MMIO寄存器访问
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            apic_id <= 32'b0;
            tpr <= 32'b0;
            ppr <= 32'b0;
            svr <= 32'b0;
            lvt_timer <= 32'h00010000;  // 屏蔽
            lvt_thermal <= 32'h00010000;
            lvt_perf <= 32'h00010000;
            lvt_lint0 <= 32'h00010000;
            lvt_lint1 <= 32'h00010000;
            lvt_error <= 32'h00010000;
            timer_initial <= 32'b0;
            timer_divide <= 32'b0;
            icr <= 64'b0;
        end else begin
            if (cpu_wr) begin
                case (cpu_addr[11:0])
                    12'h020: apic_id <= cpu_wdata[31:0];
                    12'h080: tpr <= cpu_wdata[31:0];
                    12'h0B0: begin  // EOI - 写清ISR
                        // 找到最高优先级ISR位并清除
                        // ...
                    end
                    12'h0F0: svr <= cpu_wdata[31:0];
                    12'h320: lvt_timer <= cpu_wdata[31:0];
                    12'h330: lvt_thermal <= cpu_wdata[31:0];
                    12'h340: lvt_perf <= cpu_wdata[31:0];
                    12'h350: lvt_lint0 <= cpu_wdata[31:0];
                    12'h360: lvt_lint1 <= cpu_wdata[31:0];
                    12'h370: lvt_error <= cpu_wdata[31:0];
                    12'h380: timer_initial <= cpu_wdata[31:0];
                    12'h3E0: timer_divide <= cpu_wdata[31:0];
                    12'h300: icr[31:0] <= cpu_wdata[31:0];
                    12'h310: icr[63:32] <= cpu_wdata[31:0];
                endcase
            end

            if (cpu_rd) begin
                case (cpu_addr[11:0])
                    12'h020: cpu_rdata <= {32'b0, apic_id};
                    12'h030: cpu_rdata <= {32'b0, APIC_VERSION};
                    12'h080: cpu_rdata <= {32'b0, tpr};
                    12'h0A0: cpu_rdata <= {32'b0, ppr};
                    12'h0F0: cpu_rdata <= {32'b0, svr};
                    12'h320: cpu_rdata <= {32'b0, lvt_timer};
                    12'h380: cpu_rdata <= {32'b0, timer_initial};
                    12'h390: cpu_rdata <= {32'b0, timer_current};
                    default: cpu_rdata <= 64'b0;
                endcase
            end
        end
    end

    // 中断生成逻辑
    // 扫描IRR，找到最高优先级且高于TPR的中断
    wire [7:0] highest_vector;
    wire interrupt_pending;

    // ... 优先级扫描逻辑 ...

    assign cpu_int = apic_enabled && interrupt_pending &&
                     (highest_vector[7:4] < task_priority);

endmodule
```

### 9.2 测试平台

```verilog
//============================================================================
// 中断控制器测试平台
//============================================================================

module tb_pic_simple;

    reg         clk;
    reg         rst_n;
    reg  [7:0]  cpu_data_in;
    wire [7:0]  cpu_data_out;
    reg  [1:0]  cpu_addr;
    reg         cpu_rd_n;
    reg         cpu_wr_n;
    wire        cpu_int;
    reg         cpu_inta_n;
    reg  [7:0]  ir_lines;

    // 实例化DUT
    pic_simple dut (
        .clk(clk),
        .rst_n(rst_n),
        .cpu_data_in(cpu_data_in),
        .cpu_data_out(cpu_data_out),
        .cpu_addr(cpu_addr),
        .cpu_rd_n(cpu_rd_n),
        .cpu_wr_n(cpu_wr_n),
        .cpu_int(cpu_int),
        .cpu_inta_n(cpu_inta_n),
        .ir_lines(ir_lines),
        .cascade_addr(),
        .cascade_in(1'b0),
        .cascade_out()
    );

    // 时钟生成
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    // 测试序列
    initial begin
        $display("============================================");
        $display("    中断控制器功能测试开始                 ");
        $display("============================================");

        // 初始化
        rst_n = 0;
        cpu_data_in = 8'h00;
        cpu_addr = 2'b00;
        cpu_rd_n = 1;
        cpu_wr_n = 1;
        cpu_inta_n = 1;
        ir_lines = 8'h00;

        #20 rst_n = 1;
        #10;

        // 1. 初始化8259A
        $display("\n[测试1] 初始化8259A");

        // ICW1: 边沿触发，级联，需要ICW4
        write_cmd(8'h11);
        #10;

        // ICW2: 向量基址0x20
        write_data(8'h20);
        #10;

        // ICW4: 8086模式，正常EOI
        write_data(8'h01);
        #10;

        // OCW1: 取消屏蔽所有中断
        write_data(8'h00);
        #10;

        $display("  初始化完成，向量基址=0x20");

        // 2. 测试单个中断
        $display("\n[测试2] 测试IR3中断");
        ir_lines = 8'h08;  // IR3置位
        #20;

        if (cpu_int)
            $display("  CPU_INT有效，中断请求已生成");
        else
            $display("  错误: CPU_INT未生成!");

        // 模拟中断响应
        #10 cpu_inta_n = 0;
        #10 cpu_inta_n = 1;
        #10 cpu_inta_n = 0;
        #10 cpu_inta_n = 1;

        $display("  读取向量号: 0x%02X (期望0x23)", cpu_data_out);

        // 发送EOI
        #20;
        write_cmd(8'h20);  // 非特定EOI

        ir_lines = 8'h00;
        #20;

        // 3. 测试中断屏蔽
        $display("\n[测试3] 测试中断屏蔽");
        write_data(8'hFF);  // 屏蔽所有中断
        ir_lines = 8'hFF;   // 所有IR置位
        #30;

        if (!cpu_int)
            $display("  中断被正确屏蔽");
        else
            $display("  错误: 中断未被屏蔽!");

        // 取消屏蔽
        write_data(8'h00);
        ir_lines = 8'h00;
        #20;

        // 4. 测试优先级
        $display("\n[测试4] 测试中断优先级");
        ir_lines = 8'h0A;  // IR1和IR3同时置位
        #30;

        // 响应中断
        cpu_inta_n = 0;
        #10 cpu_inta_n = 1;
        #10 cpu_inta_n = 0;
        #10 cpu_inta_n = 1;

        $display("  读取向量号: 0x%02X (期望0x21，IR1优先级更高)", cpu_data_out);

        // EOI
        write_cmd(8'h20);

        // 第二个中断
        #10 cpu_inta_n = 0;
        #10 cpu_inta_n = 1;
        #10 cpu_inta_n = 0;
        #10 cpu_inta_n = 1;

        $display("  读取向量号: 0x%02X (期望0x23，IR3)", cpu_data_out);
        write_cmd(8'h20);

        ir_lines = 8'h00;
        #20;

        $display("\n============================================");
        $display("    测试完成                              ");
        $display("============================================");

        $finish;
    end

    // 任务: 写命令寄存器
    task write_cmd(input [7:0] data);
        begin
            @(posedge clk);
            cpu_addr = 2'b00;
            cpu_data_in = data;
            cpu_wr_n = 0;
            @(posedge clk);
            cpu_wr_n = 1;
        end
    endtask

    // 任务: 写数据寄存器
    task write_data(input [7:0] data);
        begin
            @(posedge clk);
            cpu_addr = 2'b01;
            cpu_data_in = data;
            cpu_wr_n = 0;
            @(posedge clk);
            cpu_wr_n = 1;
        end
    endtask

    // 波形dump
    initial begin
        $dumpfile("pic_simple.vcd");
        $dumpvars(0, tb_pic_simple);
    end

endmodule
```

---

## 总结

中断控制器是计算机系统中至关重要的组件，负责管理外设与CPU之间的异步通信。本文档详细覆盖了:

1. **8259A PIC**: 传统中断控制器的工作原理、编程接口和初始化流程
2. **APIC**: 现代多处理器系统中的高级中断控制器，包括Local APIC和I/O APIC
3. **中断优先级**: 硬件优先级编码机制和软件优先级管理
4. **IDT/IVT**: 中断向量表的结构和配置方法
5. **中断响应流程**: 从硬件采样到软件处理的完整时序
6. **嵌套中断**: 嵌套处理模型和中断栈管理
7. **Linux实现**: 内核中断子系统的架构和编程接口
8. **Verilog实现**: 可综合的中断控制器硬件设计

理解中断控制器的工作原理对于系统编程、驱动开发和性能优化都至关重要。通过硬件描述语言实现中断控制器可以加深对其内部机制的理解。
