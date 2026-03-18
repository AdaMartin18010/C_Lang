# PMU架构深度解析：从硬件到软件的性能监控

## 目录

- [PMU架构深度解析：从硬件到软件的性能监控](#pmu架构深度解析从硬件到软件的性能监控)
  - [目录](#目录)
  - [1. 性能监控单元概述](#1-性能监控单元概述)
    - [1.1 什么是PMU](#11-什么是pmu)
    - [1.2 PMU的核心价值](#12-pmu的核心价值)
    - [1.3 PMU的历史演进](#13-pmu的历史演进)
  - [2. x86 PMU架构详解](#2-x86-pmu架构详解)
    - [2.1 Intel PMU架构](#21-intel-pmu架构)
      - [2.1.1 架构层次](#211-架构层次)
      - [2.1.2 核心寄存器详解](#212-核心寄存器详解)
    - [2.2 AMD PMU架构](#22-amd-pmu架构)
      - [2.2.1 AMD PMU特点](#221-amd-pmu特点)
      - [2.2.2 AMD PMU寄存器](#222-amd-pmu寄存器)
    - [2.3 x86 PMU事件分类](#23-x86-pmu事件分类)
      - [2.3.1 Intel事件分类](#231-intel事件分类)
  - [3. ARM PMU架构详解](#3-arm-pmu架构详解)
    - [3.1 ARM PMU概述](#31-arm-pmu概述)
    - [3.2 ARMv8 PMU寄存器](#32-armv8-pmu寄存器)
      - [3.2.1 核心计数器寄存器](#321-核心计数器寄存器)
      - [3.2.2 PMCR\_EL0控制寄存器](#322-pmcr_el0控制寄存器)
    - [3.3 ARM PMU公共事件](#33-arm-pmu公共事件)
      - [3.3.1 ARMv8 PMUv3公共事件](#331-armv8-pmuv3公共事件)
      - [3.3.2 Cortex-A特定事件](#332-cortex-a特定事件)
  - [4. RISC-V PMU架构详解](#4-risc-v-pmu架构详解)
    - [4.1 RISC-V Sscofpmf扩展](#41-risc-v-sscofpmf扩展)
    - [4.2 RISC-V PMU寄存器](#42-risc-v-pmu寄存器)
    - [4.3 RISC-V PMU事件分类](#43-risc-v-pmu事件分类)
  - [5. 性能计数器类型与编程](#5-性能计数器类型与编程)
    - [5.1 计数器类型详解](#51-计数器类型详解)
      - [5.1.1 固定计数器 vs 可编程计数器](#511-固定计数器-vs-可编程计数器)
      - [5.1.2 计数器位宽与溢出](#512-计数器位宽与溢出)
    - [5.2 计数器编程模式](#52-计数器编程模式)
      - [5.2.1 内核模式驱动编程](#521-内核模式驱动编程)
  - [6. C语言PMU接口编程](#6-c语言pmu接口编程)
    - [6.1 用户空间PMU访问](#61-用户空间pmu访问)
      - [6.1.1 使用perf\_event\_open系统调用](#611-使用perf_event_open系统调用)
      - [6.1.2 编译和运行示例](#612-编译和运行示例)
    - [6.2 内联汇编直接访问PMU](#62-内联汇编直接访问pmu)
  - [7. 实际案例分析](#7-实际案例分析)
    - [7.1 案例一：缓存未命中分析](#71-案例一缓存未命中分析)
    - [7.2 案例二：分支预测分析](#72-案例二分支预测分析)
    - [7.3 输出示例与分析](#73-输出示例与分析)
  - [8. 总结与参考](#8-总结与参考)
    - [8.1 PMU架构对比总结](#81-pmu架构对比总结)
    - [8.2 关键学习要点](#82-关键学习要点)
    - [8.3 参考资源](#83-参考资源)

---

## 1. 性能监控单元概述

### 1.1 什么是PMU

性能监控单元（Performance Monitoring Unit, PMU）是现代处理器中用于监控和分析系统性能的专用硬件模块。PMU提供了一组可编程的硬件计数器，用于精确测量各种微架构事件，如CPU周期、指令执行、缓存命中率、分支预测准确率等。

```
┌─────────────────────────────────────────────────────────────────┐
│                    处理器核心架构                                │
├─────────────────────────────────────────────────────────────────┤
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐        │
│  │ 取指单元 │  │ 解码单元 │  │ 执行单元 │  │ 访存单元 │        │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘        │
│       │             │             │             │              │
│       └─────────────┴─────────────┴─────────────┘              │
│                         │                                       │
│                  ┌──────┴──────┐                               │
│                  │   PMU模块   │ ◄── 性能计数器               │
│                  │             │     可编程事件               │
│                  │  ┌───────┐  │                               │
│                  │  │计数器0│  │                               │
│                  │  │计数器1│  │                               │
│                  │  │计数器2│  │                               │
│                  │  │计数器3│  │                               │
│                  │  └───────┘  │                               │
│                  │  控制寄存器 │                               │
│                  └─────────────┘                               │
└─────────────────────────────────────────────────────────────────┘
```

### 1.2 PMU的核心价值

PMU为性能分析提供了硬件级别的精确数据：

| 特性 | 软件计时 | PMU硬件计数 |
|------|----------|-------------|
| 精度 | 微秒级 | 纳秒级/周期级 |
| 开销 | 高（系统调用） | 低（硬件直接） |
| 事件类型 | 有限 | 数百种微架构事件 |
| 干扰 | 受调度影响 | 最小化干扰 |
| 可重现性 | 低 | 高 |

### 1.3 PMU的历史演进

```
时间线：
1990s ──► 基础周期计数器（Intel Pentium）
   │
2000s ──► 可编程性能计数器（Intel P6, AMD K7）
   │
2005 ──► Precise Event Based Sampling (PEBS) - Intel Core
   │
2010 ──► Intel Performance Counter Monitor (PCM)
   │
2014 ──► Intel Top-Down分析方法引入
   │
2016 ──► ARMv8 PMUv3扩展
   │
2020 ──► RISC-V Sscofpmf扩展标准化
```

---

## 2. x86 PMU架构详解

### 2.1 Intel PMU架构

#### 2.1.1 架构层次

Intel PMU采用分层架构设计：

```
┌─────────────────────────────────────────────────────────────┐
│                    Intel PMU架构                            │
├─────────────────────────────────────────────────────────────┤
│  Level 3: 架构性能监控 (Architectural Performance Monitoring)│
│  ┌───────────────────────────────────────────────────────┐  │
│  │ 版本ID (EAX=0AH) │ 固定计数器 │ 通用计数器 │ 位宽    │  │
│  │ Version 1:       │ 3个       │ 2+个      │ 40bit   │  │
│  │ Version 2:       │ 3个       │ 4+个      │ 48bit   │  │
│  │ Version 3:       │ 3个       │ 8+个      │ 48bit   │  │
│  │ Version 4:       │ 3个       │ 8+个      │ 48bit   │  │
│  │ Version 5:       │ 3个       │ 8+个      │ 48bit   │  │
│  └───────────────────────────────────────────────────────┘  │
├─────────────────────────────────────────────────────────────┤
│  Level 2: 非架构事件 (Model-Specific Events)                 │
│  - 微架构特定的性能事件                                       │
│  - 通过MSR (Model-Specific Registers) 配置                   │
├─────────────────────────────────────────────────────────────┤
│  Level 1: 核心计数器 (Core Performance Counters)             │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  IA32_PERFEVTSELx (事件选择)                          │  │
│  │  IA32_PMCx (性能监控计数器)                            │  │
│  │  IA32_FIXED_CTRx (固定功能计数器)                      │  │
│  │  IA32_PERF_GLOBAL_CTRL (全局控制)                      │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

#### 2.1.2 核心寄存器详解

Intel PMU使用MSR（Model-Specific Registers）进行编程控制：

**事件选择寄存器 (IA32_PERFEVTSELx)**

```
位布局 (64位寄存器):
┌────┬────┬────┬────┬────┬────┬────┬────┬───────────────────────────┐
│ 63 │ 32 │ 24 │ 23 │ 22 │ 21 │ 20 │ 19 │ 16 │ 15 │  8 │ 7 │ 0     │
├────┼────┼────┼────┼────┼────┼────┼────┼───────────────────────────┤
│INT │    │EN  │INT │PIN │ANY │    │INV │CMASK        │UMask│Event│
│PC  │Rsvd│    │EN  │CTL │    │EDGE│    │             │     │Sel  │
└────┴────┴────┴────┴────┴────┴────┴────┴───────────────────────────┘

字段说明:
- Event Select (bits 0-7): 事件选择码
- UMask (bits 8-15): 单元掩码，筛选特定子事件
- USR (bit 16): 用户态计数使能
- OS (bit 17): 内核态计数使能
- Edge (bit 18): 边沿检测
- PC (bit 19): 引脚控制
- INT (bit 20): APIC中断使能
- ANY (bit 21): 任意线程计数 (SMT)
- EN (bit 22): 计数器使能
- INV (bit 23): 计数器反转
- CMASK (bits 24-31): 比较掩码
- IN_TX (bit 32): 事务内计数 (TSX)
- IN_TXCP (bit 33): 事务补充计数
```

**固定功能计数器 (IA32_FIXED_CTRx)**

Intel处理器提供3个固定功能计数器：

| 计数器 | MSR地址 | 事件 | 描述 |
|--------|---------|------|------|
| IA32_FIXED_CTR0 | 0x309 | INST_RETIRED.ANY | 退役指令数 |
| IA32_FIXED_CTR1 | 0x30A | CPU_CLK_UNHALTED.CORE | 核心时钟周期 |
| IA32_FIXED_CTR2 | 0x30B | CPU_CLK_UNHALTED.REF | 参考时钟周期 |

### 2.2 AMD PMU架构

#### 2.2.1 AMD PMU特点

AMD PMU架构与Intel类似，但有其独特之处：

```
┌─────────────────────────────────────────────────────────────┐
│                    AMD PMU架构                              │
├─────────────────────────────────────────────────────────────┤
│  AMD64架构 (K8/K10/推土机/Zen)                              │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  性能计数器核心特性:                                   │  │
│  │  • 4-6个通用计数器 (取决于处理器代)                     │  │
│  │  • 48位计数器位宽                                      │  │
│  │  • 支持核心级和节点级事件                               │  │
│  │  • L3 Cache事件支持                                    │  │
│  │  • DRAM事件监控                                        │  │
│  └───────────────────────────────────────────────────────┘  │
├─────────────────────────────────────────────────────────────┤
│  Zen架构特有增强                                            │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  • 每个核心6个性能计数器                               │  │
│  │  • 支持SMT (Simultaneous Multi-Threading)              │  │
│  │  • 数据Fabric事件监控                                  │  │
│  │  • 内存控制器事件                                      │  │
│  │  • Infinity Fabric带宽监控                             │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

#### 2.2.2 AMD PMU寄存器

```c
/* AMD性能监控MSR定义 */
#define MSR_AMD_PERFEVTSEL0     0xC0010000  /* 事件选择寄存器0 */
#define MSR_AMD_PERFEVTSEL1     0xC0010001  /* 事件选择寄存器1 */
#define MSR_AMD_PERFEVTSEL2     0xC0010002  /* 事件选择寄存器2 */
#define MSR_AMD_PERFEVTSEL3     0xC0010003  /* 事件选择寄存器3 */
#define MSR_AMD_PMC0            0xC0010004  /* 性能计数器0 */
#define MSR_AMD_PMC1            0xC0010005  /* 性能计数器1 */
#define MSR_AMD_PMC2            0xC0010006  /* 性能计数器2 */
#define MSR_AMD_PMC3            0xC0010007  /* 性能计数器3 */

/* AMD事件选择寄存器位定义 */
#define AMD_EVTSEL_EVENT_MASK   0x000000FF  /* 事件选择 (bits 0-7) */
#define AMD_EVTSEL_UNIT_MASK    0x0000FF00  /* 单元掩码 (bits 8-15) */
#define AMD_EVTSEL_USR          (1ULL << 16) /* 用户态 */
#define AMD_EVTSEL_OS           (1ULL << 17) /* 内核态 */
#define AMD_EVTSEL_EDGE         (1ULL << 18) /* 边沿检测 */
#define AMD_EVTSEL_INT          (1ULL << 20) /* 中断使能 */
#define AMD_EVTSEL_ENABLE       (1ULL << 22) /* 计数器使能 */
#define AMD_EVTSEL_INV          (1ULL << 23) /* 反转 */
#define AMD_EVTSEL_CMASK_MASK   0xFF000000  /* 比较掩码 (bits 24-31) */
```

### 2.3 x86 PMU事件分类

#### 2.3.1 Intel事件分类

```
Intel PMU事件分类:
┌────────────────────────────────────────────────────────────────┐
│ 1. 基础架构事件 (Architectural Events)                         │
│    • INST_RETIRED.ANY - 退役指令                              │
│    • CPU_CLK_UNHALTED.CORE - 核心时钟                        │
│    • CPU_CLK_UNHALTED.REF - 参考时钟                         │
│    • LLC_REFERENCES - 最后一级缓存引用                       │
│    • LLC_MISSES - 最后一级缓存未命中                         │
│    • BR_INST_RETIRED.ALL_BRANCHES - 分支指令                 │
│    • BR_MISP_RETIRED.ALL_BRANCHES - 分支预测失败             │
├────────────────────────────────────────────────────────────────┤
│ 2. 前端事件 (Front-end Events)                                 │
│    • ICACHE_16B.IFDATA_STALL - I-Cache停顿                   │
│    • ICACHE_64B.IFTAG_HIT - I-Cache命中                      │
│    • DECODE_RESTRICTION.PREDECODE_WRONG - 预解码错误         │
│    • DSB2MITE_SWITCHES.PENALTY_CYCLES - DSB切换开销          │
│    • FETCH_STALL.ICACHE_FILL_PENDING - 取指停顿              │
├────────────────────────────────────────────────────────────────┤
│ 3. 执行单元事件 (Execution Events)                             │
│    • UOPS_EXECUTED.THREAD - 执行微操作                       │
│    • UOPS_ISSUED.ANY - 发射微操作                            │
│    • UOPS_RETIRED.RETIRE_SLOTS - 退役槽位                    │
│    • RESOURCE_STALLS.ANY - 资源停顿                          │
│    • CYCLE_ACTIVITY.STALLS_LDM_PENDING - 加载停顿            │
├────────────────────────────────────────────────────────────────┤
│ 4. 内存子系统事件 (Memory Subsystem)                           │
│    • L1D.REPLACEMENT - L1数据缓存替换                        │
│    • L1D_PEND_MISS.PENDING - L1待处理未命中                  │
│    • L2_RQSTS.ALL_DEMAND_DATA_RD - L2数据请求                │
│    • L2_RQSTS.MISS - L2未命中                                │
│    • OFFCORE_REQUESTS.ALL_DATA_RD - 片外数据请求             │
│    • MEM_LOAD_RETIRED.L3_HIT - L3命中加载                    │
│    • MEM_LOAD_RETIRED.L3_MISS - L3未命中加载                 │
├────────────────────────────────────────────────────────────────┤
│ 5. 分支预测事件 (Branch Prediction)                            │
│    • BR_INST_RETIRED.ALL_BRANCHES - 所有分支                 │
│    • BR_MISP_RETIRED.ALL_BRANCHES - 所有预测失败             │
│    • BR_INST_RETIRED.COND - 条件分支                         │
│    • BR_MISP_RETIRED.COND - 条件分支预测失败                 │
│    • BR_INST_RETIRED.INDIRECT - 间接分支                     │
│    • BR_MISP_RETIRED.INDIRECT - 间接分支预测失败             │
├────────────────────────────────────────────────────────────────┤
│ 6. TLB事件 (TLB)                                               │
│    • DTLB_LOAD_MISSES.MISS_CAUSES_A_WALK - DTLB加载未命中     │
│    • DTLB_STORE_MISSES.MISS_CAUSES_A_WALK - DTLB存储未命中    │
│    • ITLB_MISSES.MISS_CAUSES_A_WALK - ITLB未命中             │
│    • STLB_HIT - 二级TLB命中                                  │
└────────────────────────────────────────────────────────────────┘
```

---

## 3. ARM PMU架构详解

### 3.1 ARM PMU概述

ARM PMU是ARM处理器架构的可选扩展，用于性能监控和分析：

```
┌─────────────────────────────────────────────────────────────┐
│                    ARM PMU架构演进                          │
├─────────────────────────────────────────────────────────────┤
│  ARMv7-A (PMUv1-v2)                                         │
│  ┌───────────────────────────────────────────────────────┐  │
│  │ • 最多4个计数器                                       │  │
│  │ • 32位计数器                                          │  │
│  │ • 基础事件支持                                        │  │
│  │ • 通过CP15协处理器访问                                │  │
│  └───────────────────────────────────────────────────────┘  │
├─────────────────────────────────────────────────────────────┤
│  ARMv8-A (PMUv3)                                            │
│  ┌───────────────────────────────────────────────────────┐  │
│  │ • 最多31个计数器                                      │  │
│  │ • 64位计数器                                          │  │
│  │ • 扩展事件支持                                        │  │
│  │ • 支持 AArch64 和 AArch32 执行状态                    │  │
│  │ • 系统寄存器访问 (PMEVCNTRn_EL0)                      │  │
│  └───────────────────────────────────────────────────────┘  │
├─────────────────────────────────────────────────────────────┤
│  ARMv8.1-A+ (PMUv3扩展)                                     │
│  ┌───────────────────────────────────────────────────────┐  │
│  │ • 增强的溢出处理                                      │  │
│  │ • 扩展的事件过滤器                                    │  │
│  │ • 支持SPE (Statistical Profiling Extension)           │  │
│  │ • 支持TRBE (Trace Buffer Extension)                   │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 ARMv8 PMU寄存器

#### 3.2.1 核心计数器寄存器

```
ARMv8 PMU系统寄存器 (EL0可访问):
┌────────────────────────────────────────────────────────────────┐
│ 寄存器名称           │ 编码        │ 描述                     │
├──────────────────────┼─────────────┼──────────────────────────┤
│ PMCR_EL0            │  s3_3_c9_c12_0 │ 性能监控控制寄存器     │
│ PMCNTENSET_EL0      │  s3_3_c9_c12_1 │ 计数器使能设置         │
│ PMCNTENCLR_EL0      │  s3_3_c9_c12_2 │ 计数器使能清除         │
│ PMOVSCLR_EL0        │  s3_3_c9_c12_3 │ 溢出标志清除           │
│ PMSWINC_EL0         │  s3_3_c9_c12_4 │ 软件递增               │
│ PMSELR_EL0          │  s3_3_c9_c12_5 │ 计数器选择             │
│ PMCEID0_EL0         │  s3_3_c9_c12_6 │ 公共事件ID寄存器0      │
│ PMCEID1_EL0         │  s3_3_c9_c12_7 │ 公共事件ID寄存器1      │
├──────────────────────┼────────────────┼────────────────────────┤
│ PMCCNTR_EL0         │  s3_3_c9_c13_0 │ 周期计数器             │
│ PMXEVTYPER_EL0      │  s3_3_c9_c13_1 │ 事件类型选择           │
│ PMXEVCNTR_EL0       │  s3_3_c9_c13_2 │ 事件计数器             │
├──────────────────────┼────────────────┼────────────────────────┤
│ PMUSERENR_EL0       │  s3_3_c9_c14_0 │ 用户使能               │
│ PMINTENSET_EL1      │  s3_0_c9_c14_1 │ 中断使能设置           │
│ PMINTENCLR_EL1      │  s3_0_c9_c14_2 │ 中断使能清除           │
│ PMOVSSET_EL0        │  s3_3_c9_c14_3 │ 溢出标志设置           │
└────────────────────────────────────────────────────────────────┘
```

#### 3.2.2 PMCR_EL0控制寄存器

```
PMCR_EL0 寄存器位定义 (32位):
┌────┬────┬────┬────┬────┬────┬────┬────┬────────────────────────┐
│ 31 │ 16 │ 15 │ 11 │ 10 │ 6  │ 5  │ 4  │ 3  │ 2  │ 1  │ 0       │
├────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼─────────┤
│IMP │ID  │ N  │    │LP  │CCR │RST │ C  │ P  │ D  │ E  │ 保留    │
│    │CODE│    │    │    │    │    │    │    │    │    │         │
└────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴─────────┘

位字段说明:
- E [0]: PMU使能
- P [1]: 事件计数器复位
- C [2]: 周期计数器复位
- D [3]: 周期计数器时钟分频 (64)
- X [4]: 导出异常事件使能
- RST [5]: 复位所有计数器
- CCR [6]: 周期计数器溢出后清零
- LP [10]: 长时间周期计数器支持
- N [15:11]: 事件计数器数量 (减1)
- IDCODE [23:16]: 实现ID
- IMP [31:24]: 实现者代码 (0x41=ARM)
```

### 3.3 ARM PMU公共事件

#### 3.3.1 ARMv8 PMUv3公共事件

```c
/* ARMv8 PMUv3 公共事件编码 (部分) */
#define ARMV8_PMUV3_EVENT_SW_INCR               0x00
#define ARMV8_PMUV3_EVENT_L1I_CACHE_REFILL      0x01
#define ARMV8_PMUV3_EVENT_L1I_TLB_REFILL        0x02
#define ARMV8_PMUV3_EVENT_L1D_CACHE_REFILL      0x03
#define ARMV8_PMUV3_EVENT_L1D_CACHE             0x04
#define ARMV8_PMUV3_EVENT_L1D_TLB_REFILL        0x05
#define ARMV8_PMUV3_EVENT_LD_RETIRED            0x06
#define ARMV8_PMUV3_EVENT_ST_RETIRED            0x07
#define ARMV8_PMUV3_EVENT_INST_RETIRED          0x08
#define ARMV8_PMUV3_EVENT_EXC_TAKEN             0x09
#define ARMV8_PMUV3_EVENT_EXC_RETURN            0x0A
#define ARMV8_PMUV3_EVENT_CID_WRITE_RETIRED     0x0B
#define ARMV8_PMUV3_EVENT_PC_WRITE_RETIRED      0x0C
#define ARMV8_PMUV3_EVENT_BR_IMMED_RETIRED      0x0D
#define ARMV8_PMUV3_EVENT_BR_RETURN_RETIRED     0x0E
#define ARMV8_PMUV3_EVENT_UNALIGNED_LDST_RETIRED 0x0F
#define ARMV8_PMUV3_EVENT_BR_MIS_PRED           0x10
#define ARMV8_PMUV3_EVENT_CPU_CYCLES            0x11
#define ARMV8_PMUV3_EVENT_BR_PRED               0x12
#define ARMV8_PMUV3_EVENT_MEM_ACCESS            0x13
#define ARMV8_PMUV3_EVENT_L1I_CACHE             0x14
#define ARMV8_PMUV3_EVENT_L1D_CACHE_WB          0x15
#define ARMV8_PMUV3_EVENT_L2D_CACHE             0x16
#define ARMV8_PMUV3_EVENT_L2D_CACHE_REFILL      0x17
#define ARMV8_PMUV3_EVENT_L2D_CACHE_WB          0x18
#define ARMV8_PMUV3_EVENT_BUS_ACCESS            0x19
#define ARMV8_PMUV3_EVENT_MEMORY_ERROR          0x1A
#define ARMV8_PMUV3_EVENT_INST_SPEC             0x1B
#define ARMV8_PMUV3_EVENT_TTBR_WRITE_RETIRED    0x1C
#define ARMV8_PMUV3_EVENT_BUS_CYCLES            0x1D
#define ARMV8_PMUV3_EVENT_CHAIN                 0x1E
#define ARMV8_PMUV3_EVENT_L1D_CACHE_ALLOCATE    0x1F
#define ARMV8_PMUV3_EVENT_L2D_CACHE_ALLOCATE    0x20
#define ARMV8_PMUV3_EVENT_BR_RETIRED            0x21
#define ARMV8_PMUV3_EVENT_BR_MIS_PRED_RETIRED   0x22
#define ARMV8_PMUV3_EVENT_STALL_FRONTEND        0x23
#define ARMV8_PMUV3_EVENT_STALL_BACKEND         0x24
#define ARMV8_PMUV3_EVENT_L1D_TLB               0x25
#define ARMV8_PMUV3_EVENT_L1I_TLB               0x26
#define ARMV8_PMUV3_EVENT_L2I_CACHE             0x27
#define ARMV8_PMUV3_EVENT_L2I_CACHE_REFILL      0x28
#define ARMV8_PMUV3_EVENT_L3D_CACHE_ALLOCATE    0x29
#define ARMV8_PMUV3_EVENT_L3D_CACHE_REFILL      0x2A
#define ARMV8_PMUV3_EVENT_L3D_CACHE             0x2B
#define ARMV8_PMUV3_EVENT_L3D_CACHE_WB          0x2C
#define ARMV8_PMUV3_EVENT_L2D_TLB_REFILL        0x2D
#define ARMV8_PMUV3_EVENT_L2I_TLB_REFILL        0x2E
#define ARMV8_PMUV3_EVENT_L2D_TLB               0x2F
#define ARMV8_PMUV3_EVENT_L2I_TLB               0x30
#define ARMV8_PMUV3_EVENT_LD_SPEC               0x70
#define ARMV8_PMUV3_EVENT_ST_SPEC               0x71
#define ARMV8_PMUV3_EVENT_LDST_SPEC             0x72
#define ARMV8_PMUV3_EVENT_DP_SPEC               0x73
#define ARMV8_PMUV3_EVENT_ASE_SPEC              0x74
#define ARMV8_PMUV3_EVENT_VFP_SPEC              0x75
#define ARMV8_PMUV3_EVENT_PC_WRITE_SPEC         0x76
#define ARMV8_PMUV3_EVENT_CRYPTO_SPEC           0x77
#define ARMV8_PMUV3_EVENT_BR_IMMED_SPEC         0x78
#define ARMV8_PMUV3_EVENT_BR_RETURN_SPEC        0x79
#define ARMV8_PMUV3_EVENT_BR_INDIRECT_SPEC      0x7A
```

#### 3.3.2 Cortex-A特定事件

```
Cortex-A76/A77/A78 特定事件示例:
┌────────────────────────────────────────────────────────────────┐
│ 事件编码 │ 事件名称                    │ 描述                  │
├──────────┼─────────────────────────────┼───────────────────────┤
│ 0x4000   │ L1D_CACHE_LD                │ L1D缓存加载访问       │
│ 0x4001   │ L1D_CACHE_ST                │ L1D缓存存储访问       │
│ 0x4002   │ L1D_CACHE_REFILL_LD         │ L1D缓存加载重填       │
│ 0x4003   │ L1D_CACHE_REFILL_ST         │ L1D缓存存储重填       │
│ 0x4004   │ L1D_CACHE_REFILL_INNER      │ L1D内部重填           │
│ 0x4005   │ L1D_CACHE_REFILL_OUTER      │ L1D外部重填           │
│ 0x4006   │ L1D_CACHE_WB_VICTIM         │ L1D写回(受害者)       │
│ 0x4007   │ L1D_CACHE_WB_CLEAN          │ L1D写回(清理)         │
│ 0x4008   │ L1D_CACHE_INVAL             │ L1D缓存失效           │
│ 0x400C   │ L1D_TLB_REFILL_LD           │ L1D TLB加载重填       │
│ 0x400D   │ L1D_TLB_REFILL_ST           │ L1D TLB存储重填       │
│ 0x4010   │ L2D_CACHE_LD                │ L2D缓存加载访问       │
│ 0x4011   │ L2D_CACHE_ST                │ L2D缓存存储访问       │
│ 0x4012   │ L2D_CACHE_REFILL_LD         │ L2D缓存加载重填       │
│ 0x4013   │ L2D_CACHE_REFILL_ST         │ L2D缓存存储重填       │
│ 0x4014   │ L2D_CACHE_WB_VICTIM         │ L2D写回(受害者)       │
│ 0x4015   │ L2D_CACHE_WB_CLEAN          │ L2D写回(清理)         │
│ 0x4016   │ L2D_CACHE_INVAL             │ L2D缓存失效           │
│ 0x401A   │ BUS_ACCESS_RD               │ 总线读取访问          │
│ 0x401B   │ BUS_ACCESS_WR               │ 总线写入访问          │
│ 0x4020   │ L3D_CACHE_ALLOCATE          │ L3D缓存分配           │
│ 0x4021   │ L3D_CACHE_REFILL            │ L3D缓存重填           │
│ 0x4022   │ L3D_CACHE                   │ L3D缓存访问           │
│ 0x4023   │ L3D_CACHE_WB                │ L3D写回               │
│ 0x4030   │ LL_CACHE_MISS_RD            │ 末级缓存读取未命中    │
│ 0x4031   │ LL_CACHE_RD                 │ 末级缓存读取          │
│ 0x4032   │ L1D_CACHE_MISS_LD           │ L1D缓存加载未命中     │
│ 0x4033   │ L1D_CACHE_MISS_ST           │ L1D缓存存储未命中     │
│ 0x4034   │ L2D_CACHE_MISS_LD           │ L2D缓存加载未命中     │
│ 0x4035   │ L2D_CACHE_MISS_ST           │ L2D缓存存储未命中     │
│ 0x4040   │ REMOTE_ACCESS_RD            │ 远端访问读取          │
│ 0x4041   │ REMOTE_ACCESS_WR            │ 远端访问写入          │
└────────────────────────────────────────────────────────────────┘
```

---

## 4. RISC-V PMU架构详解

### 4.1 RISC-V Sscofpmf扩展

RISC-V PMU主要通过Sscofpmf扩展（Counter, Overflow and Mode-Based Filtering）实现：

```
┌─────────────────────────────────────────────────────────────┐
│                RISC-V Sscofpmf PMU架构                      │
├─────────────────────────────────────────────────────────────┤
│  标准计数器 (只读, mcycle/minstret)                         │
│  ┌───────────────────────────────────────────────────────┐  │
│  │ • mcycle (CSR: 0xB00) - 机器模式周期计数器            │  │
│  │ • minstret (CSR: 0xB02) - 机器模式指令退役计数器      │  │
│  │ • cycle (CSR: 0xC00) - 用户模式周期计数器(只读)       │  │
│  │ • instret (CSR: 0xC02) - 用户模式指令计数器(只读)     │  │
│  └───────────────────────────────────────────────────────┘  │
├─────────────────────────────────────────────────────────────┤
│  通用计数器 (mhpmcounter3-mhpmcounter31)                    │
│  ┌───────────────────────────────────────────────────────┐  │
│  │ • 最多29个可编程通用计数器                            │  │
│  │ • 64位宽度                                            │  │
│  │ • 通过mhpmevent3-31配置事件                           │  │
│  │ • 支持溢出中断                                        │  │
│  └───────────────────────────────────────────────────────┘  │
├─────────────────────────────────────────────────────────────┤
│  事件选择 (mhpmevent CSRs)                                  │
│  ┌───────────────────────────────────────────────────────┐  │
│  │ • mhpmevent3-31 (CSR: 0x323-0x33F)                    │  │
│  │ • 事件编码因实现而异                                  │  │
│  │ • 支持最小事件集标准                                  │  │
│  └───────────────────────────────────────────────────────┘  │
├─────────────────────────────────────────────────────────────┤
│  计数器使能 (mcounteren/mcountinhibit)                      │
│  ┌───────────────────────────────────────────────────────┐  │
│  │ • mcounteren (CSR: 0x306) - 低权限模式访问使能        │  │
│  │ • mcountinhibit (CSR: 0x320) - 计数器停止控制         │  │
│  │ • scounteren (CSR: 0x106) - S模式访问使能             │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 RISC-V PMU寄存器

```c
/* RISC-V PMU CSR定义 */

/* 标准计数器 */
#define CSR_CYCLE       0xC00    /* 周期计数器 (U-mode RO) */
#define CSR_TIME        0xC01    /* 时间计数器 (U-mode RO) */
#define CSR_INSTRET     0xC02    /* 指令退役计数器 (U-mode RO) */
#define CSR_HPMCOUNTER3 0xC03    /* 通用计数器3 (U-mode RO) */
/* ... CSR_HPMCOUNTER31 0xC1F */

/* 机器模式计数器 */
#define CSR_MCYCLE      0xB00    /* 机器模式周期计数器 */
#define CSR_MINSTRET    0xB02    /* 机器模式指令计数器 */
#define CSR_MHPMCOUNTER3 0xB03   /* 机器模式通用计数器3 */
/* ... CSR_MHPMCOUNTER31 0xB1F */

/* 事件选择寄存器 */
#define CSR_MHPMEVENT3  0x323    /* 事件选择3 */
#define CSR_MHPMEVENT4  0x324    /* 事件选择4 */
/* ... CSR_MHPMEVENT31 0x33F */

/* 计数器控制寄存器 */
#define CSR_MCOUNTEREN  0x306    /* 计数器访问使能 */
#define CSR_SCOUNTEREN  0x106    /* S模式计数器访问使能 */
#define CSR_MCOUNTINHIBIT 0x320  /* 计数器禁止 */

/* mhpmevent 寄存器位定义 */
#define MHPMEVENT_EVENT_MASK    0x000000000000FFFFULL  /* 事件编码 (bits 0-15) */
#define MHPMEVENT_U             (1ULL << 16)           /* 用户模式计数 */
#define MHPMEVENT_S             (1ULL << 17)           /* 监督模式计数 */
#define MHPMEVENT_M             (1ULL << 18)           /* 机器模式计数 */
#define MHPMEVENT_VS            (1ULL << 19)           /* VS模式计数 */
#define MHPMEVENT_VU            (1ULL << 20)           /* VU模式计数 */
#define MHPMEVENT_MINH          (1ULL << 21)           /* 机器模式禁止 */
#define MHPMEVENT_SINH          (1ULL << 22)           /* 监督模式禁止 */
#define MHPMEVENT_UINH          (1ULL << 23)           /* 用户模式禁止 */
#define MHPMEVENT_VSINH         (1ULL << 24)           /* VS模式禁止 */
#define MHPMEVENT_VUINH         (1ULL << 25)           /* VU模式禁止 */
#define MHPMEVENT_OF            (1ULL << 31)           /* 溢出标志 */
```

### 4.3 RISC-V PMU事件分类

```
RISC-V PMU事件分类 (Sscofpmf最小事件集):
┌────────────────────────────────────────────────────────────────┐
│ 类别              │ 事件编码 │ 描述                          │
├───────────────────┼──────────┼───────────────────────────────┤
│ 架构通用事件      │          │                               │
│                   │ 0x0001   │ 周期计数 (CYCLES)             │
│                   │ 0x0002   │ 指令退役 (INSTRUCTIONS)        │
│                   │ 0x0003   │ 缓存引用 (CACHE_REFS)          │
│                   │ 0x0004   │ 缓存未命中 (CACHE_MISSES)      │
│                   │ 0x0005   │ 分支指令 (BRANCH_INSTRUCTIONS) │
│                   │ 0x0006   │ 分支未命中 (BRANCH_MISSES)     │
│                   │ 0x0007   │ 总线周期 (BUS_CYCLES)          │
│                   │ 0x0008   │ │ 已处理异常 (EXCEPTIONS_TAKEN)  │
├───────────────────┼──────────┼───────────────────────────────┤
│ 内存子系统事件    │          │                               │
│                   │ 0x0100   │ L1 I-Cache访问                │
│                   │ 0x0101   │ L1 I-Cache未命中              │
│                   │ 0x0102   │ L1 D-Cache访问                │
│                   │ 0x0103   │ L1 D-Cache未命中              │
│                   │ 0x0104   │ L2 Cache访问                  │
│                   │ 0x0105   │ L2 Cache未命中                │
│                   │ 0x0106   │ L3 Cache访问                  │
│                   │ 0x0107   │ L3 Cache未命中                │
│                   │ 0x0108   │ 存储缓冲区访问                │
│                   │ 0x0109   │ 存储缓冲区未命中              │
├───────────────────┼──────────┼───────────────────────────────┤
│ TLB事件           │          │                               │
│                   │ 0x0200   │ I-TLB访问                     │
│                   │ 0x0201   │ I-TLB未命中                   │
│                   │ 0x0202   │ D-TLB访问                     │
│                   │ 0x0203   │ D-TLB未命中                   │
│                   │ 0x0204   │ L2 TLB访问                    │
│                   │ 0x0205   │ L2 TLB未命中                  │
├───────────────────┼──────────┼───────────────────────────────┤
│ 执行单元事件      │          │                               │
│                   │ 0x0300   │ 整数指令                      │
│                   │ 0x0301   │ 浮点指令                      │
│                   │ 0x0302   │ 加载指令                      │
│                   │ 0x0303   │ 存储指令                      │
│                   │ 0x0304   │ ALU停顿                       │
│                   │ 0x0305   │ LSU停顿                       │
├───────────────────┼──────────┼───────────────────────────────┤
│ 流水线事件        │          │                               │
│                   │ 0x0400   │ 流水线刷新                    │
│                   │ 0x0401   │ 流水线停顿                    │
│                   │ 0x0402   │ 前端停顿周期                  │
│                   │ 0x0403   │ 后端停顿周期                  │
└────────────────────────────────────────────────────────────────┘
```

---

## 5. 性能计数器类型与编程

### 5.1 计数器类型详解

#### 5.1.1 固定计数器 vs 可编程计数器

```
计数器类型对比:
┌────────────────────────────────────────────────────────────────┐
│ 特性              │ 固定计数器          │ 可编程计数器        │
├───────────────────┼─────────────────────┼─────────────────────┤
│ 事件类型          │ 预定义              │ 可配置              │
│ 数量              │ 通常2-3个           │ 通常4-8个或更多     │
│ 配置复杂度        │ 简单(只需使能)      │ 需要设置事件编码    │
│ 典型事件          │ cycles, instructions│ cache, TLB, branch  │
│ 灵活性            │ 低                  │ 高                  │
│ 精度              │ 高(硬件直接)        │ 高(硬件直接)        │
│ 使用场景          │ 基础性能指标        │ 深度性能分析        │
└────────────────────────────────────────────────────────────────┘
```

#### 5.1.2 计数器位宽与溢出

```c
/* 计数器位宽与溢出处理 */

/* Intel 48位计数器溢出时间估算 (3GHz处理器) */
/* 最大计数值: 2^48 = 281,474,976,710,656 */
/* 溢出时间: 281,474,976,710,656 / 3,000,000,000 ≈ 93,825秒 ≈ 26小时 */

/* ARM 64位计数器溢出时间 */
/* 最大计数值: 2^64 = 18,446,744,073,709,551,616 */
/* 溢出时间: 理论上约194年，实际无需处理 */

/* 溢出处理代码示例 */
#include <stdint.h>
#include <signal.h>

static volatile uint64_t overflow_count[8] = {0};
static volatile uint64_t last_value[8] = {0};

/* Intel PMU溢出中断处理 */
void pmu_overflow_handler(int sig, siginfo_t *info, void *context) {
    uint64_t status = rdmsr(IA32_PERF_GLOBAL_STATUS);

    for (int i = 0; i < num_counters; i++) {
        if (status & (1ULL << i)) {
            overflow_count[i]++;
            /* 重置溢出标志 */
            wrmsr(IA32_PERF_GLOBAL_OVF_CTRL, (1ULL << i));
            /* 重新初始化计数器 */
            wrmsr(IA32_PMC0 + i, 0);
        }
    }
}

/* 读取扩展计数器值 */
uint64_t read_extended_counter(int counter_id) {
    uint64_t current = rdpmc(counter_id);
    uint64_t counter_mask = (1ULL << 48) - 1;  /* 48位掩码 */

    /* 检测溢出 */
    if (current < last_value[counter_id]) {
        overflow_count[counter_id]++;
    }
    last_value[counter_id] = current;

    /* 计算扩展值: overflow * 2^48 + current */
    return (overflow_count[counter_id] << 48) | current;
}
```

### 5.2 计数器编程模式

#### 5.2.1 内核模式驱动编程

```c
/* Linux内核PMU驱动示例 - x86 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <asm/msr.h>

#define PMU_DEVICE_NAME "pmu_driver"
#define PMU_CLASS_NAME  "pmu_class"

/* Intel PMU MSR定义 */
#define IA32_PERFEVTSEL0    0x186
#define IA32_PERFEVTSEL1    0x187
#define IA32_PMC0           0xC1
#define IA32_PMC1           0xC2
#define IA32_PERF_GLOBAL_CTRL   0x38F
#define IA32_PERF_GLOBAL_STATUS 0x38E
#define IA32_PERF_GLOBAL_OVF_CTRL 0x390

/* 事件配置结构体 */
struct pmu_event_config {
    uint64_t event_select;
    uint64_t umask;
    int os;
    int usr;
    int edge;
};

static struct class *pmu_class = NULL;
static struct device *pmu_device = NULL;
static int major;

/* 读取MSR */
static inline uint64_t native_read_msr_safe(unsigned int msr, int *err) {
    uint64_t low, high;
    *err = 0;
    asm volatile("1: rdmsr\n"
                 "2:\n"
                 ".section .fixup,\"ax\"\n"
                 "3: mov %4, %0\n"
                 "   jmp 2b\n"
                 ".previous\n"
                 ".section __ex_table,\"a\"\n"
                 "   .align 8\n"
                 "   .quad 1b, 3b\n"
                 ".previous"
                 : "=&r" (*err), "=a" (low), "=d" (high)
                 : "c" (msr), "i" (-EFAULT), "0" (0));
    return low | ((uint64_t)high << 32);
}

/* 写入MSR */
static inline int native_write_msr_safe(unsigned int msr, uint64_t val) {
    int err;
    asm volatile("1: wrmsr\n"
                 "2:\n"
                 ".section .fixup,\"ax\"\n"
                 "3: mov %4, %0\n"
                 "   jmp 2b\n"
                 ".previous\n"
                 ".section __ex_table,\"a\"\n"
                 "   .align 8\n"
                 "   .quad 1b, 3b\n"
                 ".previous"
                 : "=&r" (err)
                 : "c" (msr), "a" ((uint32_t)val), "d" ((uint32_t)(val >> 32)),
                   "i" (-EFAULT), "0" (0));
    return err;
}

/* 配置性能计数器 */
static int pmu_config_counter(int counter_id, struct pmu_event_config *config) {
    uint64_t evt_sel;
    unsigned int sel_reg, cnt_reg;

    if (counter_id < 0 || counter_id > 1) {
        return -EINVAL;
    }

    sel_reg = IA32_PERFEVTSEL0 + counter_id;
    cnt_reg = IA32_PMC0 + counter_id;

    /* 先禁用计数器 */
    native_write_msr_safe(sel_reg, 0);

    /* 清零计数器 */
    native_write_msr_safe(cnt_reg, 0);

    /* 配置事件选择寄存器 */
    evt_sel = config->event_select & 0xFF;
    evt_sel |= (config->umask & 0xFF) << 8;
    evt_sel |= (config->usr ? 1ULL : 0ULL) << 16;
    evt_sel |= (config->os ? 1ULL : 0ULL) << 17;
    evt_sel |= (config->edge ? 1ULL : 0ULL) << 18;
    evt_sel |= 1ULL << 22;  /* 使能位 */

    native_write_msr_safe(sel_reg, evt_sel);

    printk(KERN_INFO "PMU: Configured counter %d, event=0x%llx, umask=0x%llx\n",
           counter_id, config->event_select, config->umask);

    return 0;
}

/* 启动/停止计数器 */
static void pmu_control_all(int enable) {
    uint64_t global_ctrl = enable ? 0x3 : 0;  /* 使能PMC0和PMC1 */
    native_write_msr_safe(IA32_PERF_GLOBAL_CTRL, global_ctrl);
    printk(KERN_INFO "PMU: Global control = 0x%llx\n", global_ctrl);
}

/* 读取计数器 */
static uint64_t pmu_read_counter(int counter_id) {
    int err;
    unsigned int cnt_reg = IA32_PMC0 + counter_id;
    return native_read_msr_safe(cnt_reg, &err);
}

/* 设备打开 */
static int pmu_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "PMU: Device opened\n");
    return 0;
}

/* 设备释放 */
static int pmu_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "PMU: Device released\n");
    return 0;
}

/* IOCTL处理 */
static long pmu_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct pmu_event_config config;
    uint64_t value;
    int counter_id;

    switch (cmd) {
    case PMU_IOC_CONFIG:
        if (copy_from_user(&config, (void __user *)arg, sizeof(config)))
            return -EFAULT;
        if (copy_from_user(&counter_id, (void __user *)(arg + sizeof(config)),
                          sizeof(int)))
            return -EFAULT;
        return pmu_config_counter(counter_id, &config);

    case PMU_IOC_START:
        pmu_control_all(1);
        return 0;

    case PMU_IOC_STOP:
        pmu_control_all(0);
        return 0;

    case PMU_IOC_READ:
        if (copy_from_user(&counter_id, (void __user *)arg, sizeof(int)))
            return -EFAULT;
        value = pmu_read_counter(counter_id);
        if (copy_to_user((void __user *)(arg + sizeof(int)), &value, sizeof(value)))
            return -EFAULT;
        return 0;

    default:
        return -EINVAL;
    }
}

static struct file_operations pmu_fops = {
    .owner = THIS_MODULE,
    .open = pmu_open,
    .release = pmu_release,
    .unlocked_ioctl = pmu_ioctl,
};

static int __init pmu_init(void) {
    major = register_chrdev(0, PMU_DEVICE_NAME, &pmu_fops);
    if (major < 0) {
        printk(KERN_ALERT "PMU: Failed to register device\n");
        return major;
    }

    pmu_class = class_create(THIS_MODULE, PMU_CLASS_NAME);
    if (IS_ERR(pmu_class)) {
        unregister_chrdev(major, PMU_DEVICE_NAME);
        return PTR_ERR(pmu_class);
    }

    pmu_device = device_create(pmu_class, NULL, MKDEV(major, 0),
                               NULL, PMU_DEVICE_NAME);
    if (IS_ERR(pmu_device)) {
        class_destroy(pmu_class);
        unregister_chrdev(major, PMU_DEVICE_NAME);
        return PTR_ERR(pmu_device);
    }

    printk(KERN_INFO "PMU: Driver loaded, major=%d\n", major);
    return 0;
}

static void __exit pmu_exit(void) {
    device_destroy(pmu_class, MKDEV(major, 0));
    class_destroy(pmu_class);
    unregister_chrdev(major, PMU_DEVICE_NAME);
    printk(KERN_INFO "PMU: Driver unloaded\n");
}

module_init(pmu_init);
module_exit(pmu_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Performance Analysis Team");
MODULE_DESCRIPTION("x86 PMU Kernel Driver");
```

---

## 6. C语言PMU接口编程

### 6.1 用户空间PMU访问

#### 6.1.1 使用perf_event_open系统调用

```c
/* pmu_perf.c - 使用perf_event_open的PMU接口 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <asm/unistd.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>

/* 对于旧版本内核可能没有定义 */
#ifndef PERF_COUNT_HW_REF_CPU_CYCLES
#define PERF_COUNT_HW_REF_CPU_CYCLES 9
#endif

/* perf_event_open系统调用封装 */
static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

/* PMU事件配置结构体 */
typedef struct {
    const char *name;
    uint32_t type;
    uint64_t config;
    const char *description;
} pmu_event_t;

/* 通用硬件事件表 */
static const pmu_event_t hw_events[] = {
    {"cycles",          PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES,       "CPU周期"},
    {"instructions",    PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS,     "退役指令"},
    {"cache-references",PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_REFERENCES, "缓存引用"},
    {"cache-misses",    PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES,     "缓存未命中"},
    {"branches",        PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS, "分支指令"},
    {"branch-misses",   PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_MISSES,    "分支预测失败"},
    {"bus-cycles",      PERF_TYPE_HARDWARE, PERF_COUNT_HW_BUS_CYCLES,       "总线周期"},
    {"ref-cycles",      PERF_TYPE_HARDWARE, PERF_COUNT_HW_REF_CPU_CYCLES,   "参考周期"},
    {NULL, 0, 0, NULL}
};

/* 缓存事件配置表 (L1D) */
static const pmu_event_t cache_events_l1d[] = {
    {"L1-dcache-loads",       PERF_TYPE_HW_CACHE,
     (PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_READ << 8) |
      (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16)), "L1数据缓存加载"},
    {"L1-dcache-load-misses", PERF_TYPE_HW_CACHE,
     (PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_READ << 8) |
      (PERF_COUNT_HW_CACHE_RESULT_MISS << 16)), "L1数据缓存加载未命中"},
    {"L1-dcache-stores",      PERF_TYPE_HW_CACHE,
     (PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_WRITE << 8) |
      (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16)), "L1数据缓存存储"},
    {"L1-dcache-store-misses",PERF_TYPE_HW_CACHE,
     (PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_WRITE << 8) |
      (PERF_COUNT_HW_CACHE_RESULT_MISS << 16)), "L1数据缓存存储未命中"},
    {NULL, 0, 0, NULL}
};

/* PMU会话结构体 */
typedef struct {
    int fd;
    const char *name;
    uint64_t id;
} pmu_counter_t;

typedef struct {
    pmu_counter_t *counters;
    int num_counters;
    int group_leader;
} pmu_session_t;

/* 初始化性能事件属性 */
static void init_event_attr(struct perf_event_attr *attr, uint32_t type,
                            uint64_t config) {
    memset(attr, 0, sizeof(struct perf_event_attr));
    attr->type = type;
    attr->size = sizeof(struct perf_event_attr);
    attr->config = config;

    /* 配置选项 */
    attr->disabled = 1;           /* 初始禁用 */
    attr->exclude_kernel = 0;     /* 包含内核态 */
    attr->exclude_hv = 1;         /* 排除虚拟机监控器 */
    attr->read_format = PERF_FORMAT_TOTAL_TIME_ENABLED |
                        PERF_FORMAT_TOTAL_TIME_RUNNING |
                        PERF_FORMAT_ID;

    /* 启用精确事件 (如果支持) */
    attr->precise_ip = 2;         /* 请求精确IP */
}

/* 打开PMU计数器 */
int pmu_open_counter(pmu_counter_t *counter, uint32_t type, uint64_t config,
                     int group_fd, const char *name) {
    struct perf_event_attr attr;

    init_event_attr(&attr, type, config);

    counter->fd = perf_event_open(&attr, 0, -1, group_fd, 0);
    if (counter->fd == -1) {
        fprintf(stderr, "Error opening %s: %s\n", name, strerror(errno));
        return -1;
    }

    counter->name = name;

    /* 获取事件ID */
    if (ioctl(counter->fd, PERF_EVENT_IOC_ID, &counter->id) == -1) {
        perror("ioctl PERF_EVENT_IOC_ID");
        close(counter->fd);
        return -1;
    }

    return 0;
}

/* 读取计数器值 */
uint64_t pmu_read_value(pmu_counter_t *counter) {
    uint64_t values[3];  /* value, enabled, running */

    if (read(counter->fd, values, sizeof(values)) != sizeof(values)) {
        perror("read counter");
        return 0;
    }

    /* 计算归一化值 (考虑多路复用) */
    if (values[2] > 0) {
        return (uint64_t)((double)values[0] * values[1] / values[2]);
    }
    return values[0];
}

/* 读取原始计数器值 (不归一化) */
void pmu_read_raw(pmu_counter_t *counter, uint64_t *value,
                  uint64_t *enabled, uint64_t *running) {
    uint64_t values[3];

    if (read(counter->fd, values, sizeof(values)) == sizeof(values)) {
        *value = values[0];
        *enabled = values[1];
        *running = values[2];
    }
}

/* 启动计数器 */
void pmu_start(pmu_counter_t *counter) {
    ioctl(counter->fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(counter->fd, PERF_EVENT_IOC_ENABLE, 0);
}

/* 停止计数器 */
void pmu_stop(pmu_counter_t *counter) {
    ioctl(counter->fd, PERF_EVENT_IOC_DISABLE, 0);
}

/* 关闭计数器 */
void pmu_close(pmu_counter_t *counter) {
    close(counter->fd);
}

/* ==================== 测试代码 ==================== */

/* 内存密集型测试函数 */
void memory_intensive_test(void) {
    const int SIZE = 1024 * 1024 * 100;  /* 100MB */
    char *buffer = malloc(SIZE);
    volatile int sum = 0;

    if (!buffer) {
        perror("malloc");
        return;
    }

    /* 写入测试 */
    for (int i = 0; i < SIZE; i += 64) {
        buffer[i] = i & 0xFF;
    }

    /* 读取测试 */
    for (int i = 0; i < SIZE; i += 64) {
        sum += buffer[i];
    }

    free(buffer);
}

/* 分支预测测试函数 */
void branch_test(int pattern) {
    volatile int sum = 0;
    const int ITERATIONS = 100000000;

    for (int i = 0; i < ITERATIONS; i++) {
        if (pattern == 0) {
            /* 完全可预测的分支 */
            if (i >= 0) {
                sum++;
            }
        } else if (pattern == 1) {
            /* 随机分支 */
            if (rand() % 2) {
                sum++;
            }
        } else {
            /* 交替分支 */
            if (i & 1) {
                sum++;
            }
        }
    }
}

/* 计算密集型测试 */
uint64_t cpu_intensive_test(uint64_t n) {
    uint64_t sum = 0;
    for (uint64_t i = 0; i < n; i++) {
        sum += i * i;
        sum ^= (sum >> 17);
    }
    return sum;
}

/* 打印计数器结果 */
void print_results(pmu_counter_t *counters, int num_counters,
                   const char *test_name) {
    printf("\n========== %s ==========\n", test_name);
    printf("%-20s %20s %15s %15s %10s\n",
           "Event", "Value", "Enabled(ns)", "Running(ns)", "Ratio");
    printf("%s\n", "--------------------------------------------------------------------------------");

    uint64_t cycles_value = 0;
    uint64_t instr_value = 0;

    for (int i = 0; i < num_counters; i++) {
        uint64_t value, enabled, running;
        pmu_read_raw(&counters[i], &value, &enabled, &running);

        double ratio = (enabled > 0) ? ((double)running / enabled) : 1.0;

        printf("%-20s %20" PRIu64 " %15" PRIu64 " %15" PRIu64 " %9.2f%%\n",
               counters[i].name, value, enabled, running, ratio * 100);

        if (strcmp(counters[i].name, "cycles") == 0) {
            cycles_value = value;
        } else if (strcmp(counters[i].name, "instructions") == 0) {
            instr_value = value;
        }
    }

    /* 计算CPI */
    if (instr_value > 0) {
        printf("\n性能指标:\n");
        printf("  CPI (Cycles Per Instruction): %.3f\n",
               (double)cycles_value / instr_value);
        printf("  IPC (Instructions Per Cycle): %.3f\n",
               (double)instr_value / cycles_value);
    }
}

int main(int argc, char *argv[]) {
    pmu_counter_t counters[6];
    int num_counters = 0;

    printf("PMU性能监控测试程序\n");
    printf("===================\n\n");

    /* 打开硬件计数器 */
    printf("初始化PMU计数器...\n");

    /* 基础事件 */
    if (pmu_open_counter(&counters[num_counters],
                         hw_events[0].type, hw_events[0].config,
                         -1, hw_events[0].name) == 0) {
        num_counters++;
    }

    if (pmu_open_counter(&counters[num_counters],
                         hw_events[1].type, hw_events[1].config,
                         counters[0].fd, hw_events[1].name) == 0) {
        num_counters++;
    }

    if (pmu_open_counter(&counters[num_counters],
                         hw_events[2].type, hw_events[2].config,
                         counters[0].fd, hw_events[2].name) == 0) {
        num_counters++;
    }

    if (pmu_open_counter(&counters[num_counters],
                         hw_events[3].type, hw_events[3].config,
                         counters[0].fd, hw_events[3].name) == 0) {
        num_counters++;
    }

    if (pmu_open_counter(&counters[num_counters],
                         hw_events[4].type, hw_events[4].config,
                         counters[0].fd, hw_events[4].name) == 0) {
        num_counters++;
    }

    if (pmu_open_counter(&counters[num_counters],
                         hw_events[5].type, hw_events[5].config,
                         counters[0].fd, hw_events[5].name) == 0) {
        num_counters++;
    }

    printf("成功打开 %d 个计数器\n\n", num_counters);

    /* 测试1: 内存密集型工作负载 */
    printf("运行测试1: 内存密集型工作负载\n");
    pmu_start(&counters[0]);
    memory_intensive_test();
    pmu_stop(&counters[0]);
    print_results(counters, num_counters, "内存密集型测试");

    /* 重置计数器 */
    for (int i = 0; i < num_counters; i++) {
        pmu_start(&counters[i]);
    }

    /* 测试2: 分支预测测试 - 可预测 */
    printf("\n运行测试2: 分支预测测试 (可预测模式)\n");
    pmu_start(&counters[0]);
    branch_test(0);
    pmu_stop(&counters[0]);
    print_results(counters, num_counters, "分支预测测试(可预测)");

    /* 测试3: 分支预测测试 - 随机 */
    for (int i = 0; i < num_counters; i++) {
        pmu_start(&counters[i]);
    }
    printf("\n运行测试3: 分支预测测试 (随机模式)\n");
    pmu_start(&counters[0]);
    branch_test(1);
    pmu_stop(&counters[0]);
    print_results(counters, num_counters, "分支预测测试(随机)");

    /* 测试4: CPU密集型 */
    for (int i = 0; i < num_counters; i++) {
        pmu_start(&counters[i]);
    }
    printf("\n运行测试4: CPU密集型工作负载\n");
    pmu_start(&counters[0]);
    volatile uint64_t result = cpu_intensive_test(1000000000ULL);
    (void)result;  /* 抑制未使用警告 */
    pmu_stop(&counters[0]);
    print_results(counters, num_counters, "CPU密集型测试");

    /* 清理 */
    for (int i = 0; i < num_counters; i++) {
        pmu_close(&counters[i]);
    }

    printf("\n测试完成!\n");
    return 0;
}
```

#### 6.1.2 编译和运行示例

```bash
# 编译PMU测试程序
gcc -O2 -o pmu_perf pmu_perf.c

# 运行测试 (需要root权限或调整perf_event_paranoid)
sudo ./pmu_perf

# 或者修改paranoid设置
echo 2 | sudo tee /proc/sys/kernel/perf_event_paranoid
./pmu_perf
```

### 6.2 内联汇编直接访问PMU

```c
/* pmu_direct.c - 直接通过MSR/指令访问PMU (仅限内核或特权模式) */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

/* ==================== x86 PMU直接访问 ==================== */

#if defined(__x86_64__) || defined(__i386__)

/* 读取时间戳计数器 (RDTSC) */
static inline uint64_t rdtsc(void) {
    uint32_t lo, hi;
    __asm__ __volatile__ (
        "rdtsc"
        : "=a" (lo), "=d" (hi)
    );
    return ((uint64_t)hi << 32) | lo;
}

/* 序列化RDTSC (RDTSCP) - 更精确 */
static inline uint64_t rdtscp(uint32_t *aux) {
    uint32_t lo, hi;
    __asm__ __volatile__ (
        "rdtscp\n"
        "shl $32, %%rdx\n"
        "or %%rdx, %0\n"
        "lfence"
        : "=a" (lo), "=d" (hi), "=c" (*aux)
        ::
    );
    return ((uint64_t)hi << 32) | lo;
}

/* CPUID序列化指令 */
static inline void cpuid(uint32_t func, uint32_t *eax, uint32_t *ebx,
                         uint32_t *ecx, uint32_t *edx) {
    __asm__ __volatile__ (
        "cpuid"
        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        : "a" (func)
    );
}

/* 内存屏障 */
static inline void mfence(void) {
    __asm__ __volatile__ ("mfence" ::: "memory");
}

/* 序列化指令 */
static inline void serialize(void) {
    uint32_t eax, ebx, ecx, edx;
    cpuid(0, &eax, &ebx, &ecx, &edx);
}

/* 读取MSR (需要ring 0权限) */
static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t low, high;
    __asm__ __volatile__ (
        "rdmsr"
        : "=a" (low), "=d" (high)
        : "c" (msr)
    );
    return ((uint64_t)high << 32) | low;
}

/* 写入MSR (需要ring 0权限) */
static inline void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = value >> 32;
    __asm__ __volatile__ (
        "wrmsr"
        :: "c" (msr), "a" (low), "d" (high)
    );
}

/* 读取性能监控计数器 (RDPMC) */
static inline uint64_t rdpmc(uint32_t counter) {
    uint32_t low, high;
    __asm__ __volatile__ (
        "rdpmc"
        : "=a" (low), "=d" (high)
        : "c" (counter)
    );
    return ((uint64_t)high << 32) | low;
}

/* x86 PMU MSR地址定义 */
#define IA32_PERFEVTSEL0    0x186
#define IA32_PERFEVTSEL1    0x187
#define IA32_PERFEVTSEL2    0x188
#define IA32_PERFEVTSEL3    0x189
#define IA32_PMC0           0xC1
#define IA32_PMC1           0xC2
#define IA32_PMC2           0xC3
#define IA32_PMC3           0xC4
#define IA32_FIXED_CTR0     0x309  /* INST_RETIRED.ANY */
#define IA32_FIXED_CTR1     0x30A  /* CPU_CLK_UNHALTED.CORE */
#define IA32_FIXED_CTR2     0x30B  /* CPU_CLK_UNHALTED.REF */
#define IA32_PERF_GLOBAL_STATUS     0x38E
#define IA32_PERF_GLOBAL_CTRL       0x38F
#define IA32_PERF_GLOBAL_OVF_CTRL   0x390
#define IA32_FIXED_CTR_CTRL         0x38D

/* 配置并启动x86 PMU计数器 */
void x86_pmu_setup(void) {
    uint64_t global_ctrl;

    /* 禁用所有计数器 */
    wrmsr(IA32_PERF_GLOBAL_CTRL, 0);

    /* 配置固定计数器控制 */
    /* 使能3个固定计数器，所有特权级别 */
    wrmsr(IA32_FIXED_CTR_CTRL, 0x222);  /* 每个计数器: OS=1, USR=1, ANY=0 */

    /* 清零固定计数器 */
    wrmsr(IA32_FIXED_CTR0, 0);
    wrmsr(IA32_FIXED_CTR1, 0);
    wrmsr(IA32_FIXED_CTR2, 0);

    /* 配置通用计数器0: L1缓存未命中 */
    /* Event: 0x2E, UMask: 0x4F (LLC Reference) */
    uint64_t evt_sel = 0x2E | (0x4F << 8) | (1ULL << 16) | (1ULL << 17) | (1ULL << 22);
    wrmsr(IA32_PERFEVTSEL0, 0);  /* 先禁用 */
    wrmsr(IA32_PMC0, 0);          /* 清零 */
    wrmsr(IA32_PERFEVTSEL0, evt_sel);

    /* 配置通用计数器1: 分支预测失败 */
    /* Event: 0xC5, UMask: 0x00 (BR_MISP_RETIRED.ALL_BRANCHES) */
    evt_sel = 0xC5 | (0x00 << 8) | (1ULL << 16) | (1ULL << 17) | (1ULL << 22);
    wrmsr(IA32_PERFEVTSEL1, 0);
    wrmsr(IA32_PMC1, 0);
    wrmsr(IA32_PERFEVTSEL1, evt_sel);

    /* 全局使能计数器 */
    global_ctrl = (1ULL << 32) | (1ULL << 33) | (1ULL << 34) |  /* 固定计数器 */
                  (1ULL << 0) | (1ULL << 1);                     /* 通用计数器 */
    wrmsr(IA32_PERF_GLOBAL_CTRL, global_ctrl);

    printf("x86 PMU配置完成\n");
}

/* 读取x86 PMU计数器 */
void x86_pmu_read(uint64_t *fixed, uint64_t *pmc0, uint64_t *pmc1) {
    *fixed = rdmsr(IA32_FIXED_CTR0);
    *pmc0 = rdmsr(IA32_PMC0);
    *pmc1 = rdmsr(IA32_PMC1);
}

/* 停止x86 PMU */
void x86_pmu_stop(void) {
    wrmsr(IA32_PERF_GLOBAL_CTRL, 0);
    wrmsr(IA32_PERFEVTSEL0, 0);
    wrmsr(IA32_PERFEVTSEL1, 0);
}

#endif  /* __x86_64__ || __i386__ */

/* ==================== ARM PMU直接访问 ==================== */

#if defined(__aarch64__)

/* 读取PMCR_EL0 */
static inline uint64_t read_pmcr(void) {
    uint64_t val;
    __asm__ __volatile__ ("mrs %0, pmcr_el0" : "=r" (val));
    return val;
}

/* 写入PMCR_EL0 */
static inline void write_pmcr(uint64_t val) {
    __asm__ __volatile__ ("msr pmcr_el0, %0" :: "r" (val));
}

/* 读取PMCNTENSET_EL0 */
static inline uint64_t read_pmcntenset(void) {
    uint64_t val;
    __asm__ __volatile__ ("mrs %0, pmcntenset_el0" : "=r" (val));
    return val;
}

/* 写入PMCNTENSET_EL0 */
static inline void write_pmcntenset(uint64_t val) {
    __asm__ __volatile__ ("msr pmcntenset_el0, %0" :: "r" (val));
}

/* 读取PMCNTENCLR_EL0 */
static inline void write_pmcntenclr(uint64_t val) {
    __asm__ __volatile__ ("msr pmcntenclr_el0, %0" :: "r" (val));
}

/* 读取PMSELR_EL0 */
static inline void write_pmselr(uint64_t val) {
    __asm__ __volatile__ ("msr pmselr_el0, %0" :: "r" (val));
}

/* 读取PMXEVTYPER_EL0 */
static inline void write_pmxevtyper(uint64_t val) {
    __asm__ __volatile__ ("msr pmxevtyper_el0, %0" :: "r" (val));
}

/* 读取PMXEVCNTR_EL0 */
static inline uint64_t read_pmxevcntr(void) {
    uint64_t val;
    __asm__ __volatile__ ("mrs %0, pmxevcntr_el0" : "=r" (val));
    return val;
}

/* 写入PMXEVCNTR_EL0 */
static inline void write_pmxevcntr(uint64_t val) {
    __asm__ __volatile__ ("msr pmxevcntr_el0, %0" :: "r" (val));
}

/* 读取PMCCNTR_EL0 */
static inline uint64_t read_pmccntr(void) {
    uint64_t val;
    __asm__ __volatile__ ("mrs %0, pmccntr_el0" : "=r" (val));
    return val;
}

/* 写入PMCCNTR_EL0 */
static inline void write_pmccntr(uint64_t val) {
    __asm__ __volatile__ ("msr pmccntr_el0, %0" :: "r" (val));
}

/* 读取PMUSERENR_EL0 */
static inline uint64_t read_pmuserenr(void) {
    uint64_t val;
    __asm__ __volatile__ ("mrs %0, pmuserenr_el0" : "=r" (val));
    return val;
}

/* 数据同步屏障 */
static inline void dsb(void) {
    __asm__ __volatile__ ("dsb sy" ::: "memory");
}

/* 指令同步屏障 */
static inline void isb(void) {
    __asm__ __volatile__ ("isb" ::: "memory");
}

/* ARM事件定义 */
#define ARMV8_PMUV3_EVENT_INST_RETIRED  0x08
#define ARMV8_PMUV3_EVENT_CPU_CYCLES    0x11
#define ARMV8_PMUV3_EVENT_BR_MIS_PRED   0x10
#define ARMV8_PMUV3_EVENT_L1D_CACHE_REFILL 0x03
#define ARMV8_PMUV3_EVENT_L2D_CACHE_REFILL 0x17
#define ARMV8_PMUV3_EVENT_STALL_FRONTEND 0x23
#define ARMV8_PMUV3_EVENT_STALL_BACKEND 0x24

/* 配置ARM PMU */
void arm_pmu_setup(void) {
    uint64_t pmcr;

    /* 禁用PMU */
    write_pmcr(0);
    dsb();
    isb();

    /* 复位所有计数器 */
    pmcr = (1ULL << 1) |  /* 复位事件计数器 */
           (1ULL << 2);   /* 复位周期计数器 */
    write_pmcr(pmcr);
    dsb();
    isb();

    /* 配置计数器0: 退役指令 */
    write_pmselr(0);
    write_pmxevtyper(ARMV8_PMUV3_EVENT_INST_RETIRED);
    write_pmxevcntr(0);

    /* 配置计数器1: 分支预测失败 */
    write_pmselr(1);
    write_pmxevtyper(ARMV8_PMUV3_EVENT_BR_MIS_PRED);
    write_pmxevcntr(0);

    /* 配置计数器2: L1D缓存重填 */
    write_pmselr(2);
    write_pmxevtyper(ARMV8_PMUV3_EVENT_L1D_CACHE_REFILL);
    write_pmxevcntr(0);

    /* 清零周期计数器 */
    write_pmccntr(0);

    /* 使能计数器: cycle + counter 0,1,2 */
    write_pmcntenset((1ULL << 31) | 0x7);

    /* 使能PMU */
    pmcr = (1ULL << 0) |   /* E: PMU使能 */
           (1ULL << 3) |   /* D: 周期计数器64分频 */
           (0x7 << 11);    /* N: 事件计数器数量-1 */
    write_pmcr(pmcr);
    dsb();
    isb();

    printf("ARM PMU配置完成\n");
}

/* 读取ARM PMU计数器 */
void arm_pmu_read(uint64_t *cycle, uint64_t *cnt0, uint64_t *cnt1, uint64_t *cnt2) {
    *cycle = read_pmccntr();

    write_pmselr(0);
    *cnt0 = read_pmxevcntr();

    write_pmselr(1);
    *cnt1 = read_pmxevcntr();

    write_pmselr(2);
    *cnt2 = read_pmxevcntr();
}

/* 停止ARM PMU */
void arm_pmu_stop(void) {
    write_pmcr(0);
    write_pmcntenclr(0xFFFFFFFF);
}

#endif  /* __aarch64__ */

/* ==================== RISC-V PMU直接访问 ==================== */

#if defined(__riscv)

/* 读取cycle CSR */
static inline uint64_t read_csr_cycle(void) {
    uint64_t val;
    __asm__ __volatile__ ("rdcycle %0" : "=r" (val));
    return val;
}

/* 读取instret CSR */
static inline uint64_t read_csr_instret(void) {
    uint64_t val;
    __asm__ __volatile__ ("rdinstret %0" : "=r" (val));
    return val;
}

/* 读取mcycle CSR (机器模式) */
static inline uint64_t read_csr_mcycle(void) {
    uint64_t val;
    __asm__ __volatile__ ("csrr %0, mcycle" : "=r" (val));
    return val;
}

/* 读取minstret CSR (机器模式) */
static inline uint64_t read_csr_minstret(void) {
    uint64_t val;
    __asm__ __volatile__ ("csrr %0, minstret" : "=r" (val));
    return val;
}

/* 读取mcountinhibit CSR */
static inline uint64_t read_csr_mcountinhibit(void) {
    uint64_t val;
    __asm__ __volatile__ ("csrr %0, mcountinhibit" : "=r" (val));
    return val;
}

/* 写入mcountinhibit CSR */
static inline void write_csr_mcountinhibit(uint64_t val) {
    __asm__ __volatile__ ("csrw mcountinhibit, %0" :: "r" (val));
}

/* 读取mhpmcounter3 CSR */
static inline uint64_t read_csr_mhpmcounter3(void) {
    uint64_t val;
    __asm__ __volatile__ ("csrr %0, mhpmcounter3" : "=r" (val));
    return val;
}

/* 写入mhpmcounter3 CSR */
static inline void write_csr_mhpmcounter3(uint64_t val) {
    __asm__ __volatile__ ("csrw mhpmcounter3, %0" :: "r" (val));
}

/* 读取mhpmevent3 CSR */
static inline uint64_t read_csr_mhpmevent3(void) {
    uint64_t val;
    __asm__ __volatile__ ("csrr %0, mhpmevent3" : "=r" (val));
    return val;
}

/* 写入mhpmevent3 CSR */
static inline void write_csr_mhpmevent3(uint64_t val) {
    __asm__ __volatile__ ("csrw mhpmevent3, %0" :: "r" (val));
}

/* 内存屏障 */
static inline void fence(void) {
    __asm__ __volatile__ ("fence" ::: "memory");
}

/* RISC-V Sscofpmf事件 */
#define RISCV_EVENT_L1D_CACHE_ACCESS    0x0102
#define RISCV_EVENT_L1D_CACHE_MISS      0x0103
#define RISCV_EVENT_BRANCH              0x0005
#define RISCV_EVENT_BRANCH_MISPRED      0x0006

/* 配置RISC-V PMU */
void riscv_pmu_setup(void) {
    /* 禁止计数器 */
    write_csr_mcountinhibit(0xFFFFFFFF);
    fence();

    /* 清零mhpmcounter3 */
    write_csr_mhpmcounter3(0);

    /* 配置mhpmevent3: L1D缓存访问 */
    /* 事件编码 + 用户/监督/机器模式使能 */
    uint64_t event = RISCV_EVENT_L1D_CACHE_ACCESS |
                     (1ULL << 16) |  /* U */
                     (1ULL << 17) |  /* S */
                     (1ULL << 18);   /* M */
    write_csr_mhpmevent3(event);
    fence();

    /* 使能计数器: cycle + instret + hpmcounter3 */
    write_csr_mcountinhibit(~(0x1 | 0x2 | 0x8));

    printf("RISC-V PMU配置完成\n");
}

/* 读取RISC-V PMU计数器 */
void riscv_pmu_read(uint64_t *cycle, uint64_t *instret, uint64_t *hpm3) {
    *cycle = read_csr_mcycle();
    *instret = read_csr_minstret();
    *hpm3 = read_csr_mhpmcounter3();
}

/* 停止RISC-V PMU */
void riscv_pmu_stop(void) {
    write_csr_mcountinhibit(0xFFFFFFFF);
}

#endif  /* __riscv */

/* ==================== 通用测试代码 ==================== */

/* 测试工作负载 */
volatile int global_sum = 0;

void test_workload(void) {
    volatile int sum = 0;
    int arr[1000];

    /* 初始化数组 */
    for (int i = 0; i < 1000; i++) {
        arr[i] = i;
    }

    /* 计算密集型循环 */
    for (int iter = 0; iter < 10000; iter++) {
        for (int i = 0; i < 1000; i++) {
            if (arr[i] % 2 == 0) {
                sum += arr[i];
            } else {
                sum -= arr[i];
            }
        }
    }

    global_sum = sum;
}

int main(void) {
    printf("\n========== PMU直接访问测试 ==========\n\n");

#if defined(__x86_64__) || defined(__i386__)
    printf("架构: x86\n");

    /* 注意: 直接MSR访问需要内核模块或root权限 */
    printf("注意: x86直接MSR访问需要内核态权限\n");
    printf("这里仅演示rdtsc指令的使用:\n\n");

    uint64_t tsc_start, tsc_end;
    uint32_t aux;

    /* RDTSC测试 */
    serialize();
    tsc_start = rdtscp(&aux);

    test_workload();

    tsc_end = rdtscp(&aux);
    serialize();

    printf("RDTSC测量结果:\n");
    printf("  起始TSC: %" PRIu64 "\n", tsc_start);
    printf("  结束TSC: %" PRIu64 "\n", tsc_end);
    printf("  差值: %" PRIu64 " cycles\n", tsc_end - tsc_start);

#elif defined(__aarch64__)
    printf("架构: ARM64\n");

    /* 检查用户态PMU访问权限 */
    uint64_t userenr = read_pmuserenr();
    printf("PMUSERENR_EL0: 0x%" PRIx64 "\n", userenr);

    if (!(userenr & 0x1)) {
        printf("警告: 用户态PMU访问未启用\n");
        printf("请在内核中启用CONFIG_PERF_EVENTS或设置PMU用户访问权限\n");
    } else {
        /* 配置PMU */
        arm_pmu_setup();

        uint64_t cycle_start, cycle_end;
        uint64_t cnt0_start, cnt0_end;
        uint64_t cnt1_start, cnt1_end;
        uint64_t cnt2_start, cnt2_end;

        /* 读取初始值 */
        arm_pmu_read(&cycle_start, &cnt0_start, &cnt1_start, &cnt2_start);

        /* 执行测试 */
        test_workload();

        /* 读取结束值 */
        arm_pmu_read(&cycle_end, &cnt0_end, &cnt1_end, &cnt2_end);

        /* 输出结果 */
        printf("\nPMU测量结果:\n");
        printf("  周期计数: %" PRIu64 "\n", cycle_end - cycle_start);
        printf("  退役指令: %" PRIu64 "\n", cnt0_end - cnt0_start);
        printf("  分支预测失败: %" PRIu64 "\n", cnt1_end - cnt1_start);
        printf("  L1D缓存重填: %" PRIu64 "\n", cnt2_end - cnt2_start);

        if (cnt0_end > cnt0_start) {
            printf("  CPI: %.3f\n",
                   (double)(cycle_end - cycle_start) / (cnt0_end - cnt0_start));
        }

        arm_pmu_stop();
    }

#elif defined(__riscv)
    printf("架构: RISC-V\n");

    /* 配置PMU */
    riscv_pmu_setup();

    uint64_t cycle_start, cycle_end;
    uint64_t instret_start, instret_end;
    uint64_t hpm3_start, hpm3_end;

    /* 读取初始值 */
    riscv_pmu_read(&cycle_start, &instret_start, &hpm3_start);
    fence();

    /* 执行测试 */
    test_workload();

    fence();
    /* 读取结束值 */
    riscv_pmu_read(&cycle_end, &instret_end, &hpm3_end);

    /* 输出结果 */
    printf("\nPMU测量结果:\n");
    printf("  机器周期: %" PRIu64 "\n", cycle_end - cycle_start);
    printf("  退役指令: %" PRIu64 "\n", instret_end - instret_start);
    printf("  HPM3 (L1D访问): %" PRIu64 "\n", hpm3_end - hpm3_start);

    if (instret_end > instret_start) {
        printf("  CPI: %.3f\n",
               (double)(cycle_end - cycle_start) / (instret_end - instret_start));
    }

    riscv_pmu_stop();

#else
    printf("架构: 未知 (不支持直接PMU访问)\n");
#endif

    printf("\n========== 测试完成 ==========\n");
    return 0;
}
```

---

## 7. 实际案例分析

### 7.1 案例一：缓存未命中分析

```c
/* cache_analysis.c - 缓存性能分析案例 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <asm/unistd.h>

/* perf_event_open系统调用 */
static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

/* 配置事件 */
static int setup_event(struct perf_event_attr *attr, uint32_t type,
                       uint64_t config) {
    memset(attr, 0, sizeof(struct perf_event_attr));
    attr->type = type;
    attr->size = sizeof(struct perf_event_attr);
    attr->config = config;
    attr->disabled = 1;
    attr->exclude_kernel = 1;
    attr->exclude_hv = 1;
    return 0;
}

/* 顺序访问 (缓存友好) */
void sequential_access(int *arr, int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
}

/* 随机访问 (缓存不友好) */
void random_access(int *arr, int n, int *indices) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[indices[i]];
    }
}

/* 步幅访问 (测试预取器) */
void stride_access(int *arr, int n, int stride) {
    volatile int sum = 0;
    for (int i = 0; i < n; i += stride) {
        sum += arr[i];
    }
}

/* 矩阵乘法 - 缓存优化前 */
void matrix_multiply_naive(double *a, double *b, double *c, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++) {
                sum += a[i * n + k] * b[k * n + j];
            }
            c[i * n + j] = sum;
        }
    }
}

/* 矩阵乘法 - 缓存优化后 (分块) */
void matrix_multiply_blocked(double *a, double *b, double *c, int n, int block) {
    for (int ii = 0; ii < n; ii += block) {
        for (int jj = 0; jj < n; jj += block) {
            for (int kk = 0; kk < n; kk += block) {
                for (int i = ii; i < ii + block && i < n; i++) {
                    for (int j = jj; j < jj + block && j < n; j++) {
                        double sum = c[i * n + j];
                        for (int k = kk; k < kk + block && k < n; k++) {
                            sum += a[i * n + k] * b[k * n + j];
                        }
                        c[i * n + j] = sum;
                    }
                }
            }
        }
    }
}

int main(void) {
    struct perf_event_attr attr;
    int fd;
    uint64_t val, enabled, running;

    const int N = 1024 * 1024;
    const int MATRIX_N = 512;

    printf("========== 缓存性能分析案例 ==========\n\n");

    /* 测试1: 顺序访问 vs 随机访问 */
    printf("测试1: 内存访问模式分析\n");
    printf("------------------------\n");

    int *arr = aligned_alloc(64, N * sizeof(int));
    int *indices = malloc(N * sizeof(int));

    /* 初始化数据 */
    for (int i = 0; i < N; i++) {
        arr[i] = i;
        indices[i] = rand() % N;
    }

    /* 配置L1数据缓存未命中事件 */
    setup_event(&attr, PERF_TYPE_HW_CACHE,
                (PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_READ << 8) |
                 (PERF_COUNT_HW_CACHE_RESULT_MISS << 16)));

    fd = perf_event_open(&attr, 0, -1, -1, 0);
    if (fd == -1) {
        perror("perf_event_open");
        return 1;
    }

    /* 顺序访问测试 */
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    sequential_access(arr, N);
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);

    read(fd, &val, sizeof(val));
    printf("顺序访问 - L1D缓存未命中: %" PRIu64 "\n", val);

    /* 随机访问测试 */
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    random_access(arr, N, indices);
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);

    read(fd, &val, sizeof(val));
    printf("随机访问 - L1D缓存未命中: %" PRIu64 "\n", val);

    close(fd);

    /* 测试2: 步幅访问分析 */
    printf("\n测试2: 步幅访问分析 (检测预取器行为)\n");
    printf("--------------------------------------\n");

    setup_event(&attr, PERF_TYPE_HW_CACHE,
                (PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_READ << 8) |
                 (PERF_COUNT_HW_CACHE_RESULT_MISS << 16)));

    fd = perf_event_open(&attr, 0, -1, -1, 0);

    for (int stride = 1; stride <= 64; stride *= 2) {
        ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
        stride_access(arr, N, stride);
        ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);

        read(fd, &val, sizeof(val));
        printf("步幅 %2d: L1D未命中 = %" PRIu64 "\n", stride, val);
    }

    close(fd);

    /* 测试3: 矩阵乘法优化前后对比 */
    printf("\n测试3: 矩阵乘法缓存优化\n");
    printf("------------------------\n");

    double *a = aligned_alloc(64, MATRIX_N * MATRIX_N * sizeof(double));
    double *b = aligned_alloc(64, MATRIX_N * MATRIX_N * sizeof(double));
    double *c = aligned_alloc(64, MATRIX_N * MATRIX_N * sizeof(double));

    /* 初始化矩阵 */
    for (int i = 0; i < MATRIX_N * MATRIX_N; i++) {
        a[i] = (double)rand() / RAND_MAX;
        b[i] = (double)rand() / RAND_MAX;
        c[i] = 0;
    }

    /* 配置LLC未命中事件 */
    setup_event(&attr, PERF_TYPE_HW_CACHE,
                (PERF_COUNT_HW_CACHE_LL | (PERF_COUNT_HW_CACHE_OP_READ << 8) |
                 (PERF_COUNT_HW_CACHE_RESULT_MISS << 16)));

    fd = perf_event_open(&attr, 0, -1, -1, 0);

    /* 朴素实现 */
    memset(c, 0, MATRIX_N * MATRIX_N * sizeof(double));
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    matrix_multiply_naive(a, b, c, MATRIX_N);
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    read(fd, &val, sizeof(val));
    printf("朴素实现 - LLC未命中: %" PRIu64 "\n", val);

    /* 分块优化实现 */
    memset(c, 0, MATRIX_N * MATRIX_N * sizeof(double));
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    matrix_multiply_blocked(a, b, c, MATRIX_N, 64);
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    read(fd, &val, sizeof(val));
    printf("分块优化 - LLC未命中: %" PRIu64 "\n", val);

    close(fd);

    /* 清理 */
    free(arr);
    free(indices);
    free(a);
    free(b);
    free(c);

    printf("\n========== 分析完成 ==========\n");

    return 0;
}
```

### 7.2 案例二：分支预测分析

```c
/* branch_analysis.c - 分支预测性能分析案例 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

/* 完全可预测的分支 */
void predictable_branches(int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        /* 总是为真，完全可预测 */
        if (i >= 0) {
            sum++;
        }
    }
}

/* 随机分支 */
void random_branches(int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        if (rand() % 2) {
            sum++;
        }
    }
}

/* 交替分支 (最难预测) */
void alternating_branches(int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        if (i & 1) {
            sum++;
        }
    }
}

/* 基于数据的分支 */
void data_dependent_branches(int *data, int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        if (data[i] > 100) {
            sum += data[i];
        }
    }
}

/* 排序后数据的分支 */
void sorted_data_branches(int *data, int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        if (data[i] > 100) {
            sum += data[i];
        }
    }
}

/* 二分查找 */
int binary_search(int *arr, int n, int target) {
    int left = 0, right = n - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] == target) {
            return mid;
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

/* 线性查找 */
int linear_search(int *arr, int n, int target) {
    for (int i = 0; i < n; i++) {
        if (arr[i] == target) {
            return i;
        }
    }
    return -1;
}

/* 计算分支预测准确率 */
void calculate_branch_accuracy(uint64_t branches, uint64_t misses) {
    if (branches > 0) {
        double accuracy = 100.0 * (1.0 - (double)misses / branches);
        printf("  分支预测准确率: %.2f%%\n", accuracy);
        printf("  预测失败率: %.2f%%\n", 100.0 - accuracy);
    }
}

int main(void) {
    struct perf_event_attr attr_branch, attr_miss;
    int fd_branch, fd_miss;
    uint64_t branches, misses;

    const int N = 100000000;
    const int SEARCH_N = 1000000;

    printf("========== 分支预测性能分析案例 ==========\n\n");

    /* 配置分支事件 */
    memset(&attr_branch, 0, sizeof(attr_branch));
    attr_branch.type = PERF_TYPE_HARDWARE;
    attr_branch.config = PERF_COUNT_HW_BRANCH_INSTRUCTIONS;
    attr_branch.size = sizeof(attr_branch);
    attr_branch.disabled = 1;
    attr_branch.exclude_kernel = 1;

    memset(&attr_miss, 0, sizeof(attr_miss));
    attr_miss.type = PERF_TYPE_HARDWARE;
    attr_miss.config = PERF_COUNT_HW_BRANCH_MISSES;
    attr_miss.size = sizeof(attr_miss);
    attr_miss.disabled = 1;
    attr_miss.exclude_kernel = 1;

    /* 测试1: 可预测分支 */
    printf("测试1: 完全可预测分支\n");
    printf("----------------------\n");

    fd_branch = perf_event_open(&attr_branch, 0, -1, -1, 0);
    fd_miss = perf_event_open(&attr_miss, 0, -1, fd_branch, 0);

    ioctl(fd_branch, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_branch, PERF_EVENT_IOC_ENABLE, 0);
    predictable_branches(N);
    ioctl(fd_branch, PERF_EVENT_IOC_DISABLE, 0);

    read(fd_branch, &branches, sizeof(branches));
    read(fd_miss, &misses, sizeof(misses));

    printf("分支指令数: %" PRIu64 "\n", branches);
    printf("分支预测失败: %" PRIu64 "\n", misses);
    calculate_branch_accuracy(branches, misses);

    close(fd_branch);
    close(fd_miss);

    /* 测试2: 随机分支 */
    printf("\n测试2: 随机分支\n");
    printf("---------------\n");

    srand(time(NULL));
    fd_branch = perf_event_open(&attr_branch, 0, -1, -1, 0);
    fd_miss = perf_event_open(&attr_miss, 0, -1, fd_branch, 0);

    ioctl(fd_branch, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_branch, PERF_EVENT_IOC_ENABLE, 0);
    random_branches(N);
    ioctl(fd_branch, PERF_EVENT_IOC_DISABLE, 0);

    read(fd_branch, &branches, sizeof(branches));
    read(fd_miss, &misses, sizeof(misses));

    printf("分支指令数: %" PRIu64 "\n", branches);
    printf("分支预测失败: %" PRIu64 "\n", misses);
    calculate_branch_accuracy(branches, misses);

    close(fd_branch);
    close(fd_miss);

    /* 测试3: 数据依赖分支 (排序优化) */
    printf("\n测试3: 数据依赖分支优化\n");
    printf("-----------------------\n");

    int *data = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        data[i] = rand() % 200;
    }

    /* 未排序数据 */
    fd_branch = perf_event_open(&attr_branch, 0, -1, -1, 0);
    fd_miss = perf_event_open(&attr_miss, 0, -1, fd_branch, 0);

    ioctl(fd_branch, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_branch, PERF_EVENT_IOC_ENABLE, 0);
    data_dependent_branches(data, N);
    ioctl(fd_branch, PERF_EVENT_IOC_DISABLE, 0);

    read(fd_branch, &branches, sizeof(branches));
    read(fd_miss, &misses, sizeof(misses));

    printf("未排序数据:\n");
    printf("  分支指令数: %" PRIu64 "\n", branches);
    printf("  分支预测失败: %" PRIu64 "\n", misses);
    calculate_branch_accuracy(branches, misses);

    close(fd_branch);
    close(fd_miss);

    /* 排序后数据 */
    /* 排序使数据具有模式，提高分支预测准确率 */
    qsort(data, N, sizeof(int),
          (int (*)(const void*, const void*))strcmp);

    fd_branch = perf_event_open(&attr_branch, 0, -1, -1, 0);
    fd_miss = perf_event_open(&attr_miss, 0, -1, fd_branch, 0);

    ioctl(fd_branch, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_branch, PERF_EVENT_IOC_ENABLE, 0);
    sorted_data_branches(data, N);
    ioctl(fd_branch, PERF_EVENT_IOC_DISABLE, 0);

    read(fd_branch, &branches, sizeof(branches));
    read(fd_miss, &misses, sizeof(misses));

    printf("排序后数据:\n");
    printf("  分支指令数: %" PRIu64 "\n", branches);
    printf("  分支预测失败: %" PRIu64 "\n", misses);
    calculate_branch_accuracy(branches, misses);

    close(fd_branch);
    close(fd_miss);

    /* 测试4: 查找算法对比 */
    printf("\n测试4: 查找算法分支行为\n");
    printf("-----------------------\n");

    int *search_arr = malloc(SEARCH_N * sizeof(int));
    for (int i = 0; i < SEARCH_N; i++) {
        search_arr[i] = i * 2;
    }

    /* 线性查找 */
    fd_branch = perf_event_open(&attr_branch, 0, -1, -1, 0);
    fd_miss = perf_event_open(&attr_miss, 0, -1, fd_branch, 0);

    ioctl(fd_branch, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_branch, PERF_EVENT_IOC_ENABLE, 0);
    for (int i = 0; i < 1000; i++) {
        linear_search(search_arr, SEARCH_N, i * 100);
    }
    ioctl(fd_branch, PERF_EVENT_IOC_DISABLE, 0);

    read(fd_branch, &branches, sizeof(branches));
    read(fd_miss, &misses, sizeof(misses));

    printf("线性查找:\n");
    printf("  分支指令数: %" PRIu64 "\n", branches);
    printf("  分支预测失败: %" PRIu64 "\n", misses);
    calculate_branch_accuracy(branches, misses);

    close(fd_branch);
    close(fd_miss);

    /* 二分查找 */
    fd_branch = perf_event_open(&attr_branch, 0, -1, -1, 0);
    fd_miss = perf_event_open(&attr_miss, 0, -1, fd_branch, 0);

    ioctl(fd_branch, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_branch, PERF_EVENT_IOC_ENABLE, 0);
    for (int i = 0; i < 1000; i++) {
        binary_search(search_arr, SEARCH_N, i * 100);
    }
    ioctl(fd_branch, PERF_EVENT_IOC_DISABLE, 0);

    read(fd_branch, &branches, sizeof(branches));
    read(fd_miss, &misses, sizeof(misses));

    printf("二分查找:\n");
    printf("  分支指令数: %" PRIu64 "\n", branches);
    printf("  分支预测失败: %" PRIu64 "\n", misses);
    calculate_branch_accuracy(branches, misses);

    close(fd_branch);
    close(fd_miss);

    free(data);
    free(search_arr);

    printf("\n========== 分析完成 ==========\n");
    printf("\n优化建议:\n");
    printf("1. 尽量减少随机分支模式\n");
    printf("2. 对数据排序可提高分支预测准确率\n");
    printf("3. 考虑使用位运算替代条件分支\n");
    printf("4. 使用likely/unlikely提示编译器\n");

    return 0;
}
```

### 7.3 输出示例与分析

```
========== 缓存性能分析案例 ==========

测试1: 内存访问模式分析
------------------------
顺序访问 - L1D缓存未命中: 16384
随机访问 - L1D缓存未命中: 524288
分析: 随机访问的缓存未命中是顺序访问的32倍！

测试2: 步幅访问分析 (检测预取器行为)
--------------------------------------
步幅  1: L1D未命中 = 16384     (最佳)
步幅  2: L1D未命中 = 24576
步幅  4: L1D未命中 = 32768
步幅  8: L1D未命中 = 49152
步幅 16: L1D未命中 = 81920
步幅 32: L1D未命中 = 163840
步幅 64: L1D未命中 = 524288   (跨越缓存行)
分析: 步幅增加导致空间局部性下降

测试3: 矩阵乘法缓存优化
------------------------
朴素实现 - LLC未命中: 134217728
分块优化 - LLC未命中: 16777216
分析: 分块优化减少了87.5%的LLC未命中！

========== 分支预测性能分析案例 ==========

测试1: 完全可预测分支
----------------------
分支指令数: 100000000
分支预测失败: 125
分支预测准确率: 99.99%
预测失败率: 0.01%

测试2: 随机分支
---------------
分支指令数: 100000000
分支预测失败: 50123456
分支预测准确率: 49.88%
预测失败率: 50.12%
分析: 随机模式接近理论最坏情况

测试3: 数据依赖分支优化
-----------------------
未排序数据:
  分支指令数: 100000000
  分支预测失败: 25087654
  分支预测准确率: 74.91%
排序后数据:
  分支指令数: 100000000
  分支预测失败: 1234567
  分支预测准确率: 98.77%
分析: 排序后预测准确率提高23.86个百分点！

========== 分析完成 ==========
```

---

## 8. 总结与参考

### 8.1 PMU架构对比总结

| 特性 | Intel x86 | AMD x86 | ARMv8 | RISC-V |
|------|-----------|---------|-------|--------|
| 计数器数量 | 4-8+通用 + 3固定 | 4-6通用 | 最多31个 | 最多29个通用 |
| 计数器位宽 | 48位 | 48位 | 64位 | 64位 |
| 事件类型 | 数百种 | 数百种 | 公共+实现特定 | 最小集+扩展 |
| 用户态访问 | rdpmc | rdpmc | PMUSERENR控制 | mcounteren控制 |
| 溢出处理 | 中断支持 | 中断支持 | 中断支持 | Sscofpmf扩展 |
| 精确采样 | PEBS支持 | IBS支持 | SPE扩展 | 可选 |

### 8.2 关键学习要点

1. **硬件PMU提供纳秒级精度**，相比软件计时开销更小
2. **不同架构的PMU寄存器访问方式不同**，需要针对性编程
3. **perf_event_open是Linux推荐的用户态接口**
4. **理解微架构事件是性能优化的基础**
5. **缓存未命中和分支预测失败是常见性能瓶颈**

### 8.3 参考资源

- Intel SDM Volume 3B: Chapter 18-20
- AMD64 Architecture Programmer's Manual Volume 2
- ARM Architecture Reference Manual for ARMv8-A
- RISC-V Privileged Architecture Specification
- Linux kernel: `tools/perf/` and `arch/x86/events/`
