# Intel Top-Down性能分析方法实战

## 目录

- [Intel Top-Down性能分析方法实战](#intel-top-down性能分析方法实战)
  - [目录](#目录)
  - [1. Top-Down分析方法概述](#1-top-down分析方法概述)
    - [1.1 什么是Top-Down分析方法](#11-什么是top-down分析方法)
    - [1.2 Top-Down相比传统方法的优势](#12-top-down相比传统方法的优势)
    - [1.3 流水线槽位(Pipeline Slot)概念](#13-流水线槽位pipeline-slot概念)
  - [2. Top-Down层级结构详解](#2-top-down层级结构详解)
    - [2.1 第一层(Level 1): 四大类别](#21-第一层level-1-四大类别)
      - [2.1.1 类别定义](#211-类别定义)
    - [2.2 第二层(Level 2): 细分类别](#22-第二层level-2-细分类别)
    - [2.3 第三层(Level 3): 具体指标](#23-第三层level-3-具体指标)
  - [3. 各层级详细分析](#3-各层级详细分析)
    - [3.1 Frontend Bound分析](#31-frontend-bound分析)
      - [3.1.1 Frontend Bound C代码示例](#311-frontend-bound-c代码示例)
    - [3.2 Backend Bound分析](#32-backend-bound分析)
      - [3.2.1 Memory Bound C代码示例](#321-memory-bound-c代码示例)
      - [3.2.2 Core Bound C代码示例](#322-core-bound-c代码示例)
    - [3.3 Bad Speculation分析](#33-bad-speculation分析)
      - [3.3.1 Bad Speculation C代码示例](#331-bad-speculation-c代码示例)
    - [3.4 Retiring分析](#34-retiring分析)
  - [4. 使用Perf进行Top-Down分析](#4-使用perf进行top-down分析)
    - [4.1 基础Top-Down分析命令](#41-基础top-down分析命令)
    - [4.2 完整Top-Down分析脚本](#42-完整top-down分析脚本)
    - [4.3 使用toplev工具](#43-使用toplev工具)
    - [4.4 toplev输出示例](#44-toplev输出示例)
  - [5. 实际优化案例](#5-实际优化案例)
    - [5.1 案例一：图像处理算法优化](#51-案例一图像处理算法优化)
      - [初始代码分析](#初始代码分析)
      - [优化版本](#优化版本)
    - [5.2 案例二：数据库查询优化](#52-案例二数据库查询优化)
    - [5.3 优化决策流程图](#53-优化决策流程图)
  - [6. 自动化分析工具](#6-自动化分析工具)
    - [6.1 Python自动化分析脚本](#61-python自动化分析脚本)
  - [7. 总结与最佳实践](#7-总结与最佳实践)
    - [7.1 Top-Down分析最佳实践](#71-top-down分析最佳实践)
    - [7.2 常见问题阈值参考](#72-常见问题阈值参考)
    - [7.3 参考资源](#73-参考资源)

---

## 1. Top-Down分析方法概述

### 1.1 什么是Top-Down分析方法

Intel Top-Down分析方法是由Intel工程师Ahmad Yasin在2014年提出的系统性微架构性能分析方法。
它通过层次化的方式，从CPU流水线的最高层级开始，逐步定位性能瓶颈的具体位置。

```
┌─────────────────────────────────────────────────────────────────┐
│                    Top-Down分析层次结构                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  Level 1: 顶层分类                                               │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                    Frontend Bound                         │ │
│  │                      前端瓶颈                              │ │
│  ├───────────────────────────────────────────────────────────┤ │
│  │                    Backend Bound                          │ │
│  │                      后端瓶颈                              │ │
│  │  ┌─────────────────┐  ┌─────────────────┐                 │ │
│  │  │  Memory Bound   │  │  Core Bound     │                 │ │
│  │  │   内存瓶颈      │  │   核心瓶颈      │                 │ │
│  │  └─────────────────┘  └─────────────────┘                 │ │
│  ├───────────────────────────────────────────────────────────┤ │
│  │                  Bad Speculation                          │ │
│  │                   错误推测                                 │ │
│  ├───────────────────────────────────────────────────────────┤ │
│  │                  Retiring                                 │ │
│  │                   有效执行                                 │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
│  Level 2-4: 逐层细化                                           │
│  └── 每个类别进一步分解为更具体的事件和指标                     │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 1.2 Top-Down相比传统方法的优势

```
┌─────────────────────────────────────────────────────────────────┐
│              Top-Down vs 传统性能分析方法                        │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  传统方法:                                                      │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │ • 关注原始计数器值 (cycles, instructions, cache-misses)   │ │
│  │ • 难以确定优先级 (哪个事件最重要?)                         │ │
│  │ • 指标之间关系不明确                                       │ │
│  │ • 难以定位根因                                             │ │
│  │                                                           │ │
│  │ 示例: cache-misses = 100万                                 │ │
│  │      问题是: 这是好是坏? 需要优化吗?                       │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
│  Top-Down方法:                                                  │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │ • 以流水线槽位(slot)为基本单位                             │ │
│  │ • 所有指标都是百分比 (归一化)                              │ │
│  │ • 明确的层次结构 (从粗到细)                                │ │
│  │ • 直接指向优化方向                                         │ │
│  │                                                           │ │
│  │ 示例: Backend_Bound.Memory_Bound.L3_Bound = 15%           │ │
│  │      明确指示: L3缓存是瓶颈，需要优化内存访问模式          │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 1.3 流水线槽位(Pipeline Slot)概念

Top-Down分析的核心是**流水线槽位**(Pipeline Slot)概念：

```
┌─────────────────────────────────────────────────────────────────┐
│                    流水线槽位概念                               │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  超标量处理器每周期可处理多个微操作(uops):                       │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                                                           │ │
│  │  时钟周期  │  流水线槽位 (假设每周期4个)                   │ │
│  │  ─────────┼────────────────────────────────               │ │
│  │  Cycle 1  │  [Slot 0] [Slot 1] [Slot 2] [Slot 3]          │ │
│  │  Cycle 2  │  [Slot 0] [Slot 1] [Slot 2] [Slot 3]          │ │
│  │  Cycle 3  │  [Slot 0] [Slot 1] [Slot 2] [Slot 3]          │ │
│  │           │                                             │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
│  对于4-wide超标量处理器:                                        │
│  • 每周期有4个槽位可供执行                                      │
│  • Top-Down分析这4个槽位的使用情况                              │
│  • 总槽位数 = CPU周期数 × 处理器宽度                            │
│                                                                 │
│  槽位状态分类:                                                  │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐         │
│  │   Retiring   │  │  Bad Spec    │  │   Stalled    │         │
│  │   成功执行   │  │  错误推测    │  │    停顿      │         │
│  │   (有效)     │  │   (浪费)     │  │   (未使用)   │         │
│  └──────────────┘  └──────────────┘  └──────────────┘         │
│                                                                 │
│  目标: 最大化Retiring槽位比例                                   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 2. Top-Down层级结构详解

### 2.1 第一层(Level 1): 四大类别

```
┌─────────────────────────────────────────────────────────────────┐
│              Level 1: 顶层四大类别                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  所有流水线槽位被划分为四个互斥类别:                            │
│                                                                 │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                                                           │ │
│  │                    ┌─────────────────┐                    │ │
│  │         ┌─────────│  Frontend Bound │─────────┐          │ │
│  │         │         │    前端瓶颈      │         │          │ │
│  │         │         │   (25% slots)    │         │          │ │
│  │         │         └─────────────────┘         │          │ │
│  │         │                   │                 │          │ │
│  │  ┌──────┴──────┐           │          ┌──────┴──────┐   │ │
│  │  │Bad Speculation│          │          │  Retiring   │   │ │
│  │  │   错误推测    │          │          │  有效执行   │   │ │
│  │  │  (10% slots) │          │          │ (30% slots) │   │ │
│  │  └──────┬──────┘           │          └──────┬──────┘   │ │
│  │         │                   │                 │          │ │
│  │         │         ┌─────────────────┐         │          │ │
│  │         └─────────│  Backend Bound  │─────────┘          │ │
│  │                   │    后端瓶颈      │                    │ │
│  │                   │   (35% slots)    │                    │ │
│  │                   └─────────────────┘                    │ │
│  │                                                           │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
│  数学关系: Frontend + Backend + Bad_Speculation + Retiring = 100%│
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

#### 2.1.1 类别定义

| 类别 | 定义 | 理想值 | 说明 |
|------|------|--------|------|
| **Frontend Bound** | 前端未能及时提供微操作 | < 10% | 前端(取指/解码)是瓶颈 |
| **Backend Bound** | 后端资源不足导致停顿 | < 20% | 执行单元或内存是瓶颈 |
| **Bad Speculation** | 错误推测导致的浪费 | < 5% | 分支预测失败、流水线刷新 |
| **Retiring** | 成功退役的微操作 | > 50% | 有效执行的指令比例 |

### 2.2 第二层(Level 2): 细分类别

```
┌─────────────────────────────────────────────────────────────────┐
│              Level 2: 第二层级细分                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  Frontend Bound (Level 2)                                       │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                                                           │ │
│  │  ┌─────────────────┐      ┌─────────────────┐            │ │
│  │  │ Frontend Latency│      │ Frontend Bandwidth│           │ │
│  │  │    前端延迟     │      │     前端带宽      │           │ │
│  │  │                 │      │                   │           │ │
│  │  │ ICache/ITLB Miss│      │  取指带宽不足     │           │ │
│  │  │  解码停顿       │      │  指令缓存带宽     │           │ │
│  │  │  微码序列化     │      │  解码带宽         │           │ │
│  │  └─────────────────┘      └─────────────────┘            │ │
│  │                                                           │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
│  Backend Bound (Level 2)                                        │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                                                           │ │
│  │  ┌─────────────────┐      ┌─────────────────┐            │ │
│  │  │  Memory Bound   │      │   Core Bound    │            │ │
│  │  │    内存瓶颈     │      │    核心瓶颈     │            │ │
│  │  │                 │      │                 │            │ │
│  │  │  L1 Bound       │      │  除法/整形运算  │            │ │
│  │  │  L2 Bound       │      │  浮点运算       │            │ │
│  │  │  L3 Bound       │      │  执行端口压力   │            │ │
│  │  │  DRAM Bound     │      │  数据依赖性     │            │ │
│  │  │  Store Bound    │      │                 │            │ │
│  │  └─────────────────┘      └─────────────────┘            │ │
│  │                                                           │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
│  Bad Speculation (Level 2)                                      │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                                                           │ │
│  │  ┌─────────────────┐      ┌─────────────────┐            │ │
│  │  │ Branch Mispredict│     │  Machine Clears │            │ │
│  │  │   分支预测失败  │       │    机器清除     │            │ │
│  │  │                 │      │                 │            │ │
│  │  │  条件分支       │      │  自修改代码     │            │ │
│  │  │  间接分支       │      │  异常/中断      │            │ │
│  │  │  返回栈错误     │      │  内存排序违规   │            │ │
│  │  └─────────────────┘      └─────────────────┘            │ │
│  │                                                           │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 2.3 第三层(Level 3): 具体指标

```
┌─────────────────────────────────────────────────────────────────┐
│              Level 3: 具体性能指标                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  Memory Bound -> Level 3                                        │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                                                           │ │
│  │  L1_Bound (Level 3)                                       │ │
│  │  ├── DTLB_Load                                             │ │
│  │  ├── DTLB_Store                                            │ │
│  │  ├── L1D_Pend_Miss                                         │ │
│  │  └── Store_Fwd_Blk                                         │ │
│  │                                                           │ │
│  │  L2_Bound (Level 3)                                       │ │
│  │  ├── L2_Hit                                                │ │
│  │  ├── L2_Miss                                               │ │
│  │  └── L2_Reject                                             │ │
│  │                                                           │ │
│  │  L3_Bound (Level 3)                                       │ │
│  │  ├── L3_Hit                                               │ │
│  │  ├── L3_HitM (命中修改行)                                  │ │
│  │  ├── L3_Miss                                              │ │
│  │  └── Contested_Accesses (多核竞争)                         │ │
│  │                                                           │ │
│  │  DRAM_Bound (Level 3)                                     │ │
│  │  ├── Memory_Bandwidth                                     │ │
│  │  ├── Memory_Latency                                       │ │
│  │  └── NUMA_Remote_Access                                   │ │
│  │                                                           │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
│  Core Bound -> Level 3                                          │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                                                           │ │
│  │  Divider (除法运算)                                        │ │
│  │  ├── Divider_Cycles_Active                                 │ │
│  │  └── Divider_Accurate                                      │ │
│  │                                                           │ │
│  │  Ports_Utilization (执行端口利用率)                        │ │
│  │  ├── Cycles_0_Ports_Utilized                               │ │
│  │  ├── Cycles_1_Port_Utilized                                │ │
│  │  ├── Cycles_2_Ports_Utilized                               │ │
│  │  └── Cycles_3m_Ports_Utilized                              │ │
│  │                                                           │ │
│  │  Flags_Merge (标志合并停顿)                                 │ │
│  │  └── Slow_Flags_Merge                                      │ │
│  │                                                           │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 3. 各层级详细分析

### 3.1 Frontend Bound分析

```
┌─────────────────────────────────────────────────────────────────┐
│                    Frontend Bound 详细分析                       │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  定义: 前端(取指/解码单元)未能及时提供微操作给后端               │
│                                                                 │
│  Frontend_Bound = 1 - (Slots_Delivered / Slots_Issued)          │
│                                                                 │
│  子分类:                                                        │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                                                           │ │
│  │  ICache_Misses (指令缓存未命中)                           │ │
│  │  ├── 典型症状: Frontend_Latency高                         │ │
│  │  ├── 检测事件: ICACHE_16B.IFDATA_STALL, ICACHE_MISS       │ │
│  │  └── 优化方向: 代码布局优化、函数内联、循环展开             │ │
│  │                                                           │ │
│  │  ITLB_Misses (指令TLB未命中)                              │ │
│  │  ├── 典型症状: ITLB_MISSES.MISS_CAUSES_A_WALK高           │ │
│  │  ├── 检测事件: ITLB_MISSES.WALK_PENDING                   │ │
│  │  └── 优化方向: 大页支持、代码段紧凑                       │ │
│  │                                                           │ │
│  │  Decode_Bandwidth (解码带宽)                              │ │
│  │  ├── 典型症状: DSB2MITE_SWITCHES高                        │ │
│  │  ├── 检测事件: IDQ.DSB_UOPS, IDQ.MITE_UOPS                │ │
│  │  └── 优化方向: 减少复杂指令、使用DSB微操作缓存            │ │
│  │                                                           │ │
│  │  Microcode_Sequencer (微码序列化)                         │ │
│  │  ├── 典型症状: IDQ.MS_UOPS高                              │ │
│  │  ├── 检测事件: IDQ.MS_CYCLES, IDQ.MS_SWITCHES             │ │
│  │  └── 优化方向: 避免复杂指令(DIV, SQRT, 字符串操作)        │ │
│  │                                                           │ │
│  │  Branch_Resteers (分支重新定向)                           │ │
│  │  ├── 典型症状: BACLEARS.ANY高                             │ │
│  │  ├── 检测事件: BACLEARS.ANY, BPU_CLEARS.EARLY             │ │
│  │  └── 优化方向: 分支对齐、减少分支密度                     │ │
│  │                                                           │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

#### 3.1.1 Frontend Bound C代码示例

```c
/* frontend_bound_test.c - Frontend Bound测试程序 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 测试1: ICache友好代码 (顺序执行) */
void icache_friendly(int *data, int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += data[i];
    }
}

/* 测试2: ICache不友好代码 (大量条件分支) */
void icache_unfriendly(int *data, int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        /* 大量分散的分支降低ICache效率 */
        if (i % 2 == 0) sum += data[i];
        else if (i % 3 == 0) sum -= data[i];
        else if (i % 5 == 0) sum += data[i] * 2;
        else if (i % 7 == 0) sum -= data[i] * 2;
        else sum += data[i] / 2;
    }
}

/* 测试3: 微码密集型操作 (字符串处理) */
void microcode_heavy(char *dest, const char *src, int n) {
    for (int i = 0; i < n; i++) {
        /* strcpy等字符串操作使用微码序列 */
        dest[i] = src[i];
        if (src[i] == '\0') break;
    }
}

/* 测试4: 宏融合友好的代码 */
void macro_fusion_friendly(int *data, int n) {
    volatile int count = 0;
    for (int i = 0; i < n; i++) {
        /* 比较+分支可宏融合为单个微操作 */
        if (data[i] > 100) {
            count++;
        }
    }
}

/* 测试5: 宏融合不友好的代码 */
void macro_fusion_unfriendly(int *data, int n) {
    volatile int count = 0;
    for (int i = 0; i < n; i++) {
        /* 复杂条件难以宏融合 */
        if (data[i] > 100 && data[i] < 200 && data[i] % 2 == 0) {
            count++;
        }
    }
}

/* 测试6: 复杂指令测试 */
void complex_instructions(double *data, int n) {
    volatile double result = 0;
    for (int i = 0; i < n; i++) {
        /* 除法和开方需要微码序列化 */
        result += data[i] / 3.14159;
        if (i % 100 == 0) {
            result = __builtin_sqrt(result);
        }
    }
}

int main() {
    const int N = 10000000;
    int *int_data = aligned_alloc(64, N * sizeof(int));
    double *double_data = aligned_alloc(64, N * sizeof(double));
    char *str_src = malloc(1000);
    char *str_dst = malloc(1000);

    /* 初始化 */
    for (int i = 0; i < N; i++) {
        int_data[i] = rand() % 300;
        double_data[i] = (double)rand() / RAND_MAX;
    }
    memset(str_src, 'A', 999);
    str_src[999] = '\0';

    printf("Frontend Bound测试开始...\n");
    printf("请使用: perf stat -e cycles,instructions,IDQ.MS_UOPS,IDQ.MITE_UOPS \\\n");
    printf("         -e ICACHE_16B.IFDATA_STALL,BACLEARS.ANY \\\n");
    printf("         ./frontend_bound_test\n\n");

    /* 运行测试 */
    icache_friendly(int_data, N);
    icache_unfriendly(int_data, N);
    microcode_heavy(str_dst, str_src, 1000);
    macro_fusion_friendly(int_data, N);
    macro_fusion_unfriendly(int_data, N);
    complex_instructions(double_data, N);

    printf("测试完成!\n");

    free(int_data);
    free(double_data);
    free(str_src);
    free(str_dst);

    return 0;
}
```

### 3.2 Backend Bound分析

```
┌─────────────────────────────────────────────────────────────────┐
│                    Backend Bound 详细分析                        │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  定义: 后端执行单元因资源不足或数据依赖而无法执行微操作          │
│                                                                 │
│  Backend_Bound = 1 - (uops_executed / uops_issued)              │
│                                                                 │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                                                           │ │
│  │  Memory Bound (内存子系统瓶颈)                            │ │
│  │  ═══════════════════════════════════════════════════════  │ │
│  │                                                           │ │
│  │  1. L1_Bound (一级缓存瓶颈)                               │ │
│  │     ├── DTLB_Load_Misses: L1D_PEND_MISS.PENDING           │ │
│  │     ├── L1D_Store_Buffer_Full: L1D_PEND_MISS.FB_FULL      │ │
│  │     └── Address_Generation_Stalls                         │ │
│  │                                                           │ │
│  │  2. L2_Bound (二级缓存瓶颈)                               │ │
│  │     ├── L2_Misses: L2_RQSTS.MISS                          │ │
│  │     ├── L2_Hits: L2_RQSTS.REFERENCES - L2_RQSTS.MISS      │ │
│  │     └── L2_Reject: L2_RQSTS.ALL_DEMAND_DATA_RD            │ │
│  │                                                           │ │
│  │  3. L3_Bound (三级缓存瓶颈)                               │ │
│  │     ├── L3_Hits: LONGEST_LAT_CACHE.REFERENCE              │ │
│  │     ├── L3_Misses: LONGEST_LAT_CACHE.MISS                 │ │
│  │     └── L3_Contested: OFFCORE_RESPONSE                    │ │
│  │                                                           │ │
│  │  4. DRAM_Bound (内存瓶颈)                                 │ │
│  │     ├── Memory_Bandwidth: UNC_M_CAS_COUNT.RD              │ │
│  │     ├── Memory_Latency: OFFCORE_RESPONSE                  │ │
│  │     └── NUMA_Remote: UNC_QHL_REQUESTS.REMOTE_READS        │ │
│  │                                                           │ │
│  │  5. Store_Bound (存储瓶颈)                                │ │
│  │     └── Store_Buffer_Full: RESOURCE_STALLS.SB             │ │
│  │                                                           │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                                                           │ │
│  │  Core Bound (执行单元瓶颈)                                │ │
│  │  ═══════════════════════════════════════════════════════  │ │
│  │                                                           │ │
│  │  1. Divider (除法运算瓶颈)                                │ │
│  │     └── ARITH.DIVIDER_ACTIVE                              │ │
│  │                                                           │ │
│  │  2. Ports_Utilization (执行端口利用率)                    │ │
│  │     ├── Cycles_0_Ports: 资源严重不足                      │ │
│  │     ├── Cycles_1_Port: 轻度资源压力                       │ │
│  │     ├── Cycles_2_Ports: 中度利用                          │ │
│  │     └── Cycles_3m_Ports: 高度利用(理想)                   │ │
│  │                                                           │ │
│  │  3. Flags_Merge (标志合并停顿)                            │ │
│  │     └── PARTIAL_RAT_STALLS.FLAGS_MERGE_UOP_CYCLES         │ │
│  │                                                           │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

#### 3.2.1 Memory Bound C代码示例

```c
/* memory_bound_test.c - Memory Bound测试程序 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE (1024 * 1024 * 100)  /* 100MB */

/* 测试1: L1友好访问 (顺序访问) */
void l1_friendly(int *arr, int n) {
    volatile int sum = 0;
    /* 访问模式: 顺序，充分利用预取器 */
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
}

/* 测试2: L1不友好访问 (随机访问) */
void l1_unfriendly(int *arr, int n, int *indices) {
    volatile int sum = 0;
    /* 访问模式: 随机，导致L1频繁未命中 */
    for (int i = 0; i < n; i++) {
        sum += arr[indices[i]];
    }
}

/* 测试3: L2/L3测试 - 大数组顺序访问 */
void l2_l3_test(int *arr, int n) {
    volatile int sum = 0;
    /* 100MB超过L1/L2，触发L3访问 */
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
}

/* 测试4: DRAM Bound测试 - 超大数组随机访问 */
void dram_bound_test(int *arr, int n, int *indices) {
    volatile int sum = 0;
    /* 随机访问100MB数据，导致频繁DRAM访问 */
    for (int i = 0; i < n / 100; i++) {
        sum += arr[indices[i]];
    }
}

/* 测试5: Store Bound测试 - 大量写操作 */
void store_bound_test(int *arr, int n) {
    /* 密集的存储操作可能导致存储缓冲区满 */
    for (int i = 0; i < n; i++) {
        arr[i] = i * 2;
    }
}

/* 测试6: 内存带宽测试 - 优化版本 */
void bandwidth_optimized(double *dest, double *src, int n) {
    /* 使用memcpy利用硬件预取 */
    memcpy(dest, src, n * sizeof(double));
}

/* 测试7: 内存带宽测试 - 非优化版本 */
void bandwidth_unoptimized(double *dest, double *src, int n) {
    /* 逐字节复制，带宽利用率低 */
    char *d = (char*)dest;
    char *s = (char*)src;
    for (int i = 0; i < n * sizeof(double); i++) {
        d[i] = s[i];
    }
}

/* 测试8: 伪共享测试 */
void false_sharing_test(int *counters, int num_threads, int iterations) {
    /* counters数组元素间距小，导致多核间缓存行跳动 */
    for (int t = 0; t < num_threads; t++) {
        for (int i = 0; i < iterations; i++) {
            counters[t]++;  /* 相邻counters可能在同一缓存行 */
        }
    }
}

/* 测试9: NUMA感知 vs 非NUMA感知 */
void numa_unaware_access(int *arr, int n) {
    /* 随机访问模式，可能导致跨NUMA节点访问 */
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        int idx = (i * 2654435761u) % n;  /* 伪随机 */
        sum += arr[idx];
    }
}

int main() {
    int *array = aligned_alloc(64, ARRAY_SIZE * sizeof(int));
    int *indices = malloc(ARRAY_SIZE * sizeof(int));
    double *double_array1 = aligned_alloc(64, ARRAY_SIZE * sizeof(double));
    double *double_array2 = aligned_alloc(64, ARRAY_SIZE * sizeof(double));

    /* 初始化 */
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = i;
        indices[i] = rand() % ARRAY_SIZE;
        double_array1[i] = (double)i;
    }

    printf("Memory Bound测试开始...\n");
    printf("建议监控事件:\n");
    printf("  - L1D_PEND_MISS.PENDING\n");
    printf("  - L2_RQSTS.MISS\n");
    printf("  - LONGEST_LAT_CACHE.MISS\n");
    printf("  - OFFCORE_RESPONSE\n");
    printf("  - UNC_M_CAS_COUNT.RD\n\n");

    /* 执行测试 */
    l1_friendly(array, 1024 * 1024);  /* 4MB，适合L1/L2 */
    l1_unfriendly(array, 1024 * 1024, indices);
    l2_l3_test(array, ARRAY_SIZE);
    dram_bound_test(array, ARRAY_SIZE, indices);
    store_bound_test(array, ARRAY_SIZE);
    bandwidth_optimized(double_array2, double_array1, ARRAY_SIZE);
    bandwidth_unoptimized(double_array2, double_array1, ARRAY_SIZE);

    printf("测试完成!\n");

    free(array);
    free(indices);
    free(double_array1);
    free(double_array2);

    return 0;
}
```

#### 3.2.2 Core Bound C代码示例

```c
/* core_bound_test.c - Core Bound测试程序 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* 测试1: 除法运算密集型 (Divider瓶颈) */
void divider_heavy(double *data, int n) {
    volatile double result = 1.0;
    for (int i = 0; i < n; i++) {
        /* 整数除法非常慢 */
        result = result / (1.0 + data[i] * 0.0001);
    }
}

/* 测试2: 除法优化版本 (使用乘法代替) */
void divider_optimized(double *data, int n) {
    volatile double result = 1.0;
    for (int i = 0; i < n; i++) {
        /* 预计算倒数，使用乘法 */
        double inv = 1.0 / (1.0 + data[i] * 0.0001);
        result = result * inv;
    }
}

/* 测试3: 端口压力测试 - 大量独立整数运算 */
void port_pressure_test(int *data, int n) {
    volatile int r1 = 0, r2 = 0, r3 = 0, r4 = 0;
    for (int i = 0; i < n; i++) {
        /* 多个独立运算，可同时使用多个端口 */
        r1 += data[i] * 2;
        r2 -= data[i] * 3;
        r3 += data[i] << 2;
        r4 ^= data[i] >> 1;
    }
    (void)r1; (void)r2; (void)r3; (void)r4;
}

/* 测试4: 数据依赖性链 (Core Bound) */
void dependency_chain(double *data, int n) {
    volatile double result = 0;
    for (int i = 0; i < n; i++) {
        /* 长依赖链，每个操作依赖前一个结果 */
        result = result + data[i];
        result = result * 1.0001;
        result = result - data[i] * 0.5;
        result = result * 0.9999;
    }
}

/* 测试5: 打破依赖链 */
void dependency_broken(double *data, int n) {
    volatile double r1 = 0, r2 = 0, r3 = 0, r4 = 0;
    for (int i = 0; i < n; i += 4) {
        /* 4个独立依赖链，可同时执行 */
        r1 = r1 + data[i];
        r2 = r2 + data[i+1];
        r3 = r3 + data[i+2];
        r4 = r4 + data[i+3];
    }
    volatile double result = r1 + r2 + r3 + r4;
    (void)result;
}

/* 测试6: 浮点运算压力 */
void fp_pressure_test(double *data, int n) {
    volatile double r1 = 0, r2 = 0, r3 = 0;
    for (int i = 0; i < n; i++) {
        /* 密集浮点运算 */
        r1 = r1 * 1.1 + data[i];
        r2 = r2 * 0.9 - data[i] * 0.5;
        r3 = r3 + r1 * r2;
    }
    (void)r1; (void)r2; (void)r3;
}

/* 测试7: 向量运算优化 (SIMD) */
void vector_optimized(float *data, int n) {
    /* 假设使用AVX，一次处理8个float */
    volatile float sum[8] = {0};
    for (int i = 0; i < n; i += 8) {
        for (int j = 0; j < 8; j++) {
            sum[j] += data[i + j];
        }
    }
    volatile float total = sum[0] + sum[1] + sum[2] + sum[3] +
                          sum[4] + sum[5] + sum[6] + sum[7];
    (void)total;
}

int main() {
    const int N = 10000000;
    double *double_data = aligned_alloc(64, N * sizeof(double));
    float *float_data = aligned_alloc(64, N * sizeof(float));
    int *int_data = aligned_alloc(64, N * sizeof(int));

    for (int i = 0; i < N; i++) {
        double_data[i] = (double)rand() / RAND_MAX;
        float_data[i] = (float)rand() / RAND_MAX;
        int_data[i] = rand();
    }

    printf("Core Bound测试开始...\n");
    printf("建议监控事件:\n");
    printf("  - CYCLE_ACTIVITY.STALLS_TOTAL\n");
    printf("  - UOPS_EXECUTED.CORE\n");
    printf("  - UOPS_DISPATCHED.PORT_0/1/2/3/4/5/6/7\n");
    printf("  - ARITH.DIVIDER_ACTIVE\n\n");

    divider_heavy(double_data, N);
    divider_optimized(double_data, N);
    port_pressure_test(int_data, N);
    dependency_chain(double_data, N);
    dependency_broken(double_data, N);
    fp_pressure_test(double_data, N);
    vector_optimized(float_data, N);

    printf("测试完成!\n");

    free(double_data);
    free(float_data);
    free(int_data);

    return 0;
}
```

### 3.3 Bad Speculation分析

```
┌─────────────────────────────────────────────────────────────────┐
│                    Bad Speculation 详细分析                      │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  定义: 由于错误推测执行而导致的流水线槽位浪费                    │
│                                                                 │
│  Bad_Speculation = (Slots_Wasted_by_Bad_Spec / Total_Slots)     │
│                                                                 │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                                                           │ │
│  │  Branch Mispredicts (分支预测失败)                        │ │
│  │  ═══════════════════════════════════════════════════════  │ │
│  │                                                           │ │
│  │  1. Conditional Branches (条件分支)                       │ │
│  │     ├── 检测: BR_MISP_RETIRED.COND                        │ │
│  │     └── 优化: 排序数据、使用likely/unlikely               │ │
│  │                                                           │ │
│  │  2. Indirect Branches (间接分支)                          │ │
│  │     ├── 检测: BR_MISP_RETIRED.INDIRECT                    │ │
│  │     └── 优化: 转换为switch、使用BTB提示                   │ │
│  │                                                           │ │
│  │  3. Returns (函数返回)                                    │ │
│  │     ├── 检测: BR_MISP_RETIRED.RETURN                    │ │
│  │     └── 优化: 减少调用深度、内联小函数                    │ │
│  │                                                           │ │
│  │  4. Calls (函数调用)                                      │ │
│  │     └── 检测: BR_MISP_RETIRED.CALL                      │ │
│  │                                                           │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                                                           │ │
│  │  Machine Clears (机器清除)                                │ │
│  │  ═══════════════════════════════════════════════════════  │ │
│  │                                                           │ │
│  │  1. Self-Modifying Code (自修改代码)                      │ │
│  │     └── 避免运行时修改代码段                              │ │
│  │                                                           │ │
│  │  2. Memory Ordering Violations (内存序违规)               │ │
│  │     ├── 检测: MACHINE_CLEARS.MEMORY_ORDERING              │ │
│  │     └── 优化: 正确使用内存屏障、避免错误共享              │ │
│  │                                                           │ │
│  │  3. FP Assists (浮点辅助)                                 │ │
│  │     └── 检测: PARTIAL_RAT_STALLS.SCOREBOARD               │ │
│  │                                                           │ │
│  │  4. Execution Pipeline Errors (执行流水线错误)            │ │
│  │     └── 检测: MACHINE_CLEARS.ALL                          │ │
│  │                                                           │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

#### 3.3.1 Bad Speculation C代码示例

```c
/* bad_speculation_test.c - Bad Speculation测试程序 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* 测试1: 可预测分支 (高效) */
void predictable_branch(int *data, int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        /* 总是为真，完全可预测 */
        if (i >= 0) {
            sum += data[i];
        }
    }
}

/* 测试2: 随机分支 (低效) */
void random_branch(int *data, int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        /* 随机模式，难以预测 */
        if (rand() % 2) {
            sum += data[i];
        }
    }
}

/* 测试3: 交替分支 (最难预测) */
void alternating_branch(int *data, int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        /* 严格交替: T, F, T, F... */
        if (i & 1) {
            sum += data[i];
        }
    }
}

/* 测试4: 数据相关分支 - 未排序 */
void unsorted_data_branch(int *data, int n) {
    volatile int count = 0;
    for (int i = 0; i < n; i++) {
        /* 分支方向取决于数据值 */
        if (data[i] > 100) {
            count++;
        }
    }
}

/* 测试5: 数据相关分支 - 已排序 (优化) */
void sorted_data_branch(int *data, int n) {
    volatile int count = 0;
    /* 先排序数据 - 使分支更可预测 */
    qsort(data, n, sizeof(int),
          (int (*)(const void*, const void*))strcmp);

    for (int i = 0; i < n; i++) {
        if (data[i] > 100) {
            count++;
        }
    }
}

/* 测试6: 间接跳转 - 随机 */
void indirect_jump_random(int n) {
    void (*funcs[])(void) = {
        (void(*)(void))puts,
        (void(*)(void))exit,
        (void(*)(void))malloc,
        (void(*)(void))free
    };

    for (int i = 0; i < n; i++) {
        /* 随机选择函数，难以预测 */
        funcs[rand() % 4]();
    }
}

/* 测试7: 间接跳转 - 模式化 (更好预测) */
void indirect_jump_pattern(int n) {
    void (*funcs[])(void) = {
        (void(*)(void))puts,
        (void(*)(void))exit,
        (void(*)(void))malloc,
        (void(*)(void))free
    };

    for (int i = 0; i < n; i++) {
        /* 循环模式，易于预测 */
        funcs[i % 4]();
    }
}

/* 测试8: 多分支switch - 随机 */
int switch_random(int n) {
    volatile int result = 0;
    for (int i = 0; i < n; i++) {
        int r = rand() % 8;
        switch (r) {
            case 0: result += 1; break;
            case 1: result += 2; break;
            case 2: result += 3; break;
            case 3: result += 4; break;
            case 4: result += 5; break;
            case 5: result += 6; break;
            case 6: result += 7; break;
            case 7: result += 8; break;
        }
    }
    return result;
}

/* 测试9: 内联vs函数调用 */
static inline int inline_add(int a, int b) {
    return a + b;
}

int normal_add(int a, int b) {
    return a + b;
}

void test_inline(int n) {
    volatile int result = 0;
    for (int i = 0; i < n; i++) {
        result = inline_add(result, i);
    }
}

void test_normal_call(int n) {
    volatile int result = 0;
    for (int i = 0; i < n; i++) {
        result = normal_add(result, i);
    }
}

int main() {
    const int N = 100000000;
    int *data = malloc(N * sizeof(int));

    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        data[i] = rand() % 200;
    }

    printf("Bad Speculation测试开始...\n");
    printf("建议监控事件:\n");
    printf("  - BR_MISP_RETIRED.ALL_BRANCHES\n");
    printf("  - BR_MISP_RETIRED.COND\n");
    printf("  - BR_MISP_RETIRED.INDIRECT\n");
    printf("  - MACHINE_CLEARS.ALL\n");
    printf("  - INT_MISC.RECOVERY_CYCLES\n\n");

    predictable_branch(data, N);
    random_branch(data, N);
    alternating_branch(data, N);
    unsorted_data_branch(data, N);

    /* 复制数据用于排序测试 */
    int *data_copy = malloc(N * sizeof(int));
    memcpy(data_copy, data, N * sizeof(int));
    sorted_data_branch(data_copy, N);
    free(data_copy);

    test_inline(N);
    test_normal_call(N);

    printf("测试完成!\n");

    free(data);
    return 0;
}
```

### 3.4 Retiring分析

```
┌─────────────────────────────────────────────────────────────────┐
│                    Retiring 详细分析                             │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  定义: 成功执行并退役的微操作占总槽位的比例                      │
│                                                                 │
│  Retiring = Slots_Retired / Total_Slots                         │
│                                                                 │
│  这是"好事" - 表示CPU有效执行工作的比例                          │
│                                                                 │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                                                           │ │
│  │  理想情况: Retiring > 50%                                 │ │
│  │                                                           │ │
│  │  子分类:                                                  │ │
│  │                                                           │ │
│  │  1. Base (基础指令)                                       │ │
│  │     ├── 普通整数运算                                      │ │
│  │     ├── 简单内存访问                                      │ │
│  │     └── 高效: IPC接近理论最大值                           │ │
│  │                                                           │ │
│  │  2. Microcode_Sequencer (微码序列)                        │ │
│  │     ├── 复杂指令 (DIV, SQRT)                              │ │
│  │     ├── 字符串操作                                        │ │
│  │     └── 注意: 虽然Retiring高，但效率可能不高              │ │
│  │                                                           │ │
│  │  3. FP_Arith (浮点运算)                                   │ │
│  │     └── FP_Retired相关事件                                │ │
│  │                                                           │ │
│  │  4. Other (其他)                                          │ │
│  │     └── 不常使用指令                                      │ │
│  │                                                           │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
│  注意: 高Retiring不一定表示高性能!                               │
│  - 如果伴随高Cycles，可能是执行了大量低效率指令                  │
│  - 检查CPI (Cycles Per Instruction)                             │
│  - 理想: Retiring高 + CPI低                                     │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 4. 使用Perf进行Top-Down分析

### 4.1 基础Top-Down分析命令

```bash
# 基础Top-Down分析 (Intel Sandy Bridge及更新)
perf stat -e cycles,instructions \
          -e uops_issued.any,uops_retired.retire_slots \
          -e int_misc.recovery_cycles \
          -e br_misp_retired.all_branches \
          ./program

# 使用topdown事件组 (更精确)
perf stat -e '{cycles,instructions},\
              {uops_issued.any,uops_retired.retire_slots},\
              {int_misc.recovery_cycles},\
              {br_misp_retired.all_branches}' \
          ./program
```

### 4.2 完整Top-Down分析脚本

```bash
#!/bin/bash
# topdown_analysis.sh - 完整的Top-Down分析脚本

PROGRAM=${1:-"./program"}
OUTPUT_DIR=${2:-"topdown_results"}

mkdir -p $OUTPUT_DIR

echo "========== Top-Down分析开始 =========="
echo "程序: $PROGRAM"
echo "输出目录: $OUTPUT_DIR"
echo ""

# Level 1: 顶层分析
echo "[1/5] Level 1: 顶层分析..."
perf stat -e cycles,instructions \
          -e uops_issued.any,uops_retired.retire_slots \
          -e int_misc.recovery_cycles \
          -e br_misp_retired.all_branches \
          -o $OUTPUT_DIR/level1.txt \
          $PROGRAM 2>&1

# 计算顶层指标
echo "计算Top-Down指标..."
cat > $OUTPUT_DIR/calculate_metrics.py << 'EOF'
import re
import sys

def parse_perf_output(filename):
    values = {}
    with open(filename, 'r') as f:
        for line in f:
            # 匹配 perf stat 输出格式
            match = re.match(r'\s*([\d,]+)\s+([\w\.]+)', line)
            if match:
                value = int(match.group(1).replace(',', ''))
                event = match.group(2)
                values[event] = value
    return values

def calculate_topdown(values):
    cycles = values.get('cycles', 1)
    instructions = values.get('instructions', 0)
    uops_issued = values.get('uops_issued.any', 0)
    uops_retired = values.get('uops_retired.retire_slots', 0)
    recovery = values.get('int_misc.recovery_cycles', 0)
    br_misp = values.get('br_misp_retired.all_branches', 0)

    # 假设4-wide处理器
    width = 4
    total_slots = cycles * width

    # 计算各项指标 (简化计算)
    retiring = min(100.0, (uops_retired / total_slots) * 100) if total_slots > 0 else 0
    bad_speculation = (recovery / cycles) * 100 if cycles > 0 else 0
    frontend_bound = max(0, 100 - retiring - bad_speculation -
                        ((uops_issued - uops_retired) / total_slots) * 100)
    backend_bound = max(0, 100 - retiring - bad_speculation - frontend_bound)

    return {
        'Frontend_Bound': frontend_bound,
        'Backend_Bound': backend_bound,
        'Bad_Speculation': bad_speculation,
        'Retiring': retiring,
        'CPI': cycles / instructions if instructions > 0 else 0,
        'IPC': instructions / cycles if cycles > 0 else 0
    }

if __name__ == '__main__':
    values = parse_perf_output(sys.argv[1])
    metrics = calculate_topdown(values)

    print("\n========== Top-Down分析结果 ==========\n")
    print("Level 1 指标:")
    print(f"  Frontend Bound:  {metrics['Frontend_Bound']:.2f}%")
    print(f"  Backend Bound:   {metrics['Backend_Bound']:.2f}%")
    print(f"  Bad Speculation: {metrics['Bad_Speculation']:.2f}%")
    print(f"  Retiring:        {metrics['Retiring']:.2f}%")
    print(f"\n基础指标:")
    print(f"  CPI: {metrics['CPI']:.3f}")
    print(f"  IPC: {metrics['IPC']:.3f}")

    print("\n瓶颈分析:")
    max_metric = max(metrics, key=lambda k: metrics[k] if k != 'CPI' and k != 'IPC' else 0)
    if max_metric == 'Frontend_Bound':
        print("  -> 主要瓶颈: 前端 (指令获取/解码)")
        print("     建议: 优化代码布局、减少分支、使用DSB")
    elif max_metric == 'Backend_Bound':
        print("  -> 主要瓶颈: 后端 (执行/内存)")
        print("     建议: 优化内存访问、减少数据依赖")
    elif max_metric == 'Bad_Speculation':
        print("  -> 主要瓶颈: 错误推测 (分支预测失败)")
        print("     建议: 优化分支模式、使用likely/unlikely")
    else:
        print("  -> 执行情况良好 (高Retiring)")
        if metrics['CPI'] > 1.0:
            print("     注意: 虽然Retiring高，但CPI较高，可能存在效率问题")
EOF

python3 $OUTPUT_DIR/calculate_metrics.py $OUTPUT_DIR/level1.txt

# Level 2-3: 详细分析
echo ""
echo "[2/5] Level 2-3: 详细事件分析..."

# Frontend详细分析
perf stat -e cycles \
          -e icache_16b.ifdata_stall \
          -e idq.ms_uops \
          -e idq.mite_uops \
          -e idq.dsb_uops \
          -e bacler.any \
          -o $OUTPUT_DIR/frontend_detail.txt \
          $PROGRAM 2>&1

# Backend详细分析
perf stat -e cycles \
          -e cycle_activity.stalls_l2_miss \
          -e cycle_activity.stalls_l3_miss \
          -e cycle_activity.stalls_mem_any \
          -e resource_stalls.any \
          -e resource_stalls.sb \
          -o $OUTPUT_DIR/backend_detail.txt \
          $PROGRAM 2>&1

# Memory详细分析
perf stat -e cycles \
          -e l1d_pend_miss.pending \
          -e l2_rqsts.miss \
          -e llc_load_misses \
          -e offcore_response.all_data_rd.llc_miss.local_dram \
          -o $OUTPUT_DIR/memory_detail.txt \
          $PROGRAM 2>&1

# Branch详细分析
echo ""
echo "[3/5] Branch预测分析..."
perf stat -e cycles \
          -e br_inst_retired.all_branches \
          -e br_misp_retired.all_branches \
          -e br_misp_retired.cond \
          -e br_misp_retired.indirect \
          -e machine_clears.all \
          -o $OUTPUT_DIR/branch_detail.txt \
          $PROGRAM 2>&1

# 生成报告
echo ""
echo "[4/5] 生成详细报告..."
cat > $OUTPUT_DIR/report.txt << EOF
Top-Down性能分析报告
====================
分析时间: $(date)
分析程序: $PROGRAM

$(python3 $OUTPUT_DIR/calculate_metrics.py $OUTPUT_DIR/level1.txt)

详细数据:
---------
Level 1 原始数据:
$(cat $OUTPUT_DIR/level1.txt)

Frontend 详情:
$(cat $OUTPUT_DIR/frontend_detail.txt)

Backend 详情:
$(cat $OUTPUT_DIR/backend_detail.txt)

Memory 详情:
$(cat $OUTPUT_DIR/memory_detail.txt)

Branch 详情:
$(cat $OUTPUT_DIR/branch_detail.txt)
EOF

# 火焰图生成 (可选)
echo ""
echo "[5/5] 生成火焰图..."
if command -v FlameGraph/flamegraph.pl &> /dev/null; then
    perf record -F 99 -g -o $OUTPUT_DIR/perf.data -- $PROGRAM 2>&1
    perf script -i $OUTPUT_DIR/perf.data | \
        FlameGraph/stackcollapse-perf.pl | \
        FlameGraph/flamegraph.pl > $OUTPUT_DIR/flamegraph.svg
    echo "火焰图已生成: $OUTPUT_DIR/flamegraph.svg"
else
    echo "FlameGraph工具未找到，跳过火焰图生成"
fi

echo ""
echo "========== 分析完成 =========="
echo "完整报告: $OUTPUT_DIR/report.txt"
```

### 4.3 使用toplev工具

```bash
# 安装pmu-tools (包含toplev)
git clone https://github.com/andikleen/pmu-tools.git
cd pmu-tools

# 基础Top-Down分析
./toplev.py -l1 -- ./program

# 二级详细分析
./toplev.py -l2 -- ./program

# 三级完整分析
./toplev.py -l3 -- ./program

# 输出到文件
./toplev.py -l3 -o results.txt -- ./program

# 显示调用栈
./toplev.py -l2 --perf-args="record -g" -- ./program
./toplev.py -l2 -I -- ./program
```

### 4.4 toplev输出示例

```
$ ./toplev.py -l2 -- ./matrix_multiply

Using level 2.
-running
warning: xxx
metric_counts 19
cpus 8 8
frontend_latency   Frontend_Latency_SMT   % Slots < 5.2
frontend_bandwidth Frontend_Bandwidth_SMT % Slots < 1.0
bad_speculation    Bad_Speculation        % Slots 2.8
machine_clears     Machine_Clears         % Slots 0.3
backend_bound      Backend_Bound          % Slots 35.4
memory_bound       Memory_Bound           % Slots 28.7
l1_bound           L1_Bound               % Slots 5.2
l2_bound           L2_Bound               % Slots 3.1
l3_bound           L3_Bound               % Slots 8.9
dram_bound         DRAM_Bound             % Slots 11.5
store_bound        Store_Bound            % Slots 0.4
core_bound         Core_Bound             % Slots 6.7
divider            Divider                % Slots 0.0
ports_utilization  Ports_Utilization      % Slots 6.7
retiring           Retiring               % Slots 56.5

Add --run-sample to find locations for the causes of bottlenecks.

瓶颈分析:
- Backend_Bound 35.4% 是最主要瓶颈
  - Memory_Bound 28.7% 占主导地位
    - DRAM_Bound 11.5% 需要优化内存访问模式
    - L3_Bound 8.9% 考虑缓存分块
- Frontend_Bound 较低，前端不是问题
- Bad_Speculation 2.8% 在可接受范围
- Retiring 56.5% 有提升空间

优化建议:
1. 应用缓存分块技术减少DRAM访问
2. 优化数据布局提高空间局部性
3. 考虑使用预取指令
```

---

## 5. 实际优化案例

### 5.1 案例一：图像处理算法优化

#### 初始代码分析

```c
/* image_filter_original.c - 原始版本 */
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 4096
#define HEIGHT 4096

void blur_filter_original(unsigned char *src, unsigned char *dst,
                          int width, int height) {
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            /* 3x3模糊核 */
            int sum = 0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    sum += src[(y + ky) * width + (x + kx)];
                }
            }
            dst[y * width + x] = sum / 9;
        }
    }
}
```

```bash
# 分析原始版本
$ ./toplev.py -l3 -- ./image_filter_original

# 输出:
frontend_bound      Frontend_Bound          % Slots 8.2
backend_bound       Backend_Bound           % Slots 65.3
  memory_bound        Memory_Bound          % Slots 52.1
    l1_bound            L1_Bound            % Slots 5.3
    l2_bound            L2_Bound            % Slots 8.7
    l3_bound            L3_Bound            % Slots 15.2
    dram_bound          DRAM_Bound          % Slots 22.9
  core_bound          Core_Bound            % Slots 13.2
retiring            Retiring                % Slots 26.5

# 问题: Backend_Bound 65.3%，其中DRAM_Bound 22.9%严重
```

#### 优化版本

```c
/* image_filter_optimized.c - 优化版本 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 4096
#define HEIGHT 4096
#define BLOCK_SIZE 64

/* 分块处理 + SIMD友好的内存布局 */
void blur_filter_optimized(unsigned char *src, unsigned char *dst,
                           int width, int height) {
    /* 使用行缓冲区减少缓存未命中 */
    unsigned char row_buf[3][WIDTH];

    for (int by = 1; by < height - 1; by += BLOCK_SIZE) {
        int by_end = (by + BLOCK_SIZE < height - 1) ? by + BLOCK_SIZE : height - 1;

        for (int bx = 1; bx < width - 1; bx += BLOCK_SIZE) {
            int bx_end = (bx + BLOCK_SIZE < width - 1) ? bx + BLOCK_SIZE : width - 1;

            /* 预加载行到缓冲区 */
            memcpy(row_buf[0], &src[(by - 1) * width], width);
            memcpy(row_buf[1], &src[by * width], width);
            memcpy(row_buf[2], &src[(by + 1) * width], width);

            /* 处理块 */
            for (int y = by; y < by_end; y++) {
                for (int x = bx; x < bx_end; x++) {
                    /* 从缓冲区读取，提高缓存局部性 */
                    int sum = row_buf[0][x-1] + row_buf[0][x] + row_buf[0][x+1] +
                              row_buf[1][x-1] + row_buf[1][x] + row_buf[1][x+1] +
                              row_buf[2][x-1] + row_buf[2][x] + row_buf[2][x+1];
                    dst[y * width + x] = sum / 9;
                }

                /* 滚动缓冲区 */
                if (y + 1 < by_end) {
                    memcpy(row_buf[0], row_buf[1], width);
                    memcpy(row_buf[1], row_buf[2], width);
                    memcpy(row_buf[2], &src[(y + 2) * width], width);
                }
            }
        }
    }
}
```

```bash
# 分析优化版本
$ ./toplev.py -l3 -- ./image_filter_optimized

# 输出:
frontend_bound      Frontend_Bound          % Slots 5.1
backend_bound       Backend_Bound           % Slots 28.3
  memory_bound        Memory_Bound          % Slots 18.7
    dram_bound          DRAM_Bound          % Slots 4.2
  core_bound          Core_Bound            % Slots 9.6
retiring            Retiring                % Slots 66.6

# 优化效果:
# - Backend_Bound: 65.3% -> 28.3% (减少37个百分点)
# - DRAM_Bound: 22.9% -> 4.2% (减少18.7个百分点)
# - Retiring: 26.5% -> 66.6% (提高40个百分点)
# - 执行时间: 2.1s -> 0.6s (3.5x加速)
```

### 5.2 案例二：数据库查询优化

```c
/* database_query.c - 数据库查询处理 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[32];
    int category;
    double value;
} Record;

/* 原始版本 - 行存储处理 */
double query_sum_original(Record *records, int n, int target_category) {
    double sum = 0;
    for (int i = 0; i < n; i++) {
        if (records[i].category == target_category) {
            sum += records[i].value;
        }
    }
    return sum;
}

/* 分析结果: Frontend_Bound高(18%)，Cache_Misses严重 */
/* 原因: 每次迭代加载整个Record结构体，但只有部分字段使用 */

/* 优化版本 - 列存储处理 */
typedef struct {
    int *categories;
    double *values;
    int n;
} ColumnStore;

double query_sum_columnar(ColumnStore *store, int target_category) {
    double sum = 0;
    /* 顺序访问categories数组，缓存友好 */
    for (int i = 0; i < store->n; i++) {
        if (store->categories[i] == target_category) {
            sum += store->values[i];
        }
    }
    return sum;
}

/* 进一步优化 - 分支预测优化 */
double query_sum_optimized(ColumnStore *store, int target_category) {
    double sum = 0;
    /* 先收集匹配索引，然后处理 */
    int *matches = malloc(store->n * sizeof(int));
    int match_count = 0;

    for (int i = 0; i < store->n; i++) {
        matches[match_count] = i;
        match_count += (store->categories[i] == target_category);
    }

    for (int j = 0; j < match_count; j++) {
        sum += store->values[matches[j]];
    }

    free(matches);
    return sum;
}
```

### 5.3 优化决策流程图

```
┌─────────────────────────────────────────────────────────────────┐
│                    Top-Down优化决策流程                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  开始分析                                                       │
│      │                                                          │
│      ▼                                                          │
│  ┌─────────────┐                                                │
│  │ Frontend >  │──Yes──► 分析Frontend子指标                     │
│  │    15%?     │           │                                    │
│  └─────────────┘           ▼                                    │
│      No              ┌─────────────┐                            │
│                      │ Latency高?  │──Yes──► 优化ICache布局     │
│                      └─────────────┘      减少代码体积          │
│                           │ No                                  │
│                           ▼                                     │
│                      ┌─────────────┐                            │
│                      │ Bandwidth低?│──Yes──► 简化指令序列       │
│                      └─────────────┘      减少复杂指令          │
│                           │ No                                  │
│                           ▼                                     │
│                      继续检查其他指标                           │
│      │                                                          │
│      ▼                                                          │
│  ┌─────────────┐                                                │
│  │  Backend >  │──Yes──► 分析Backend子指标                      │
│  │    20%?     │           │                                    │
│  └─────────────┘           ▼                                    │
│      No              ┌─────────────┐                            │
│                      │ Memory >    │──Yes──► 分析Memory子指标   │
│                      │ Core?       │           │                 │
│                      └─────────────┘           ▼                 │
│                           No             ┌─────────────┐        │
│                                          │ L1/L2高?    │        │
│                                          └─────────────┘        │
│                                               │                 │
│                          ┌────────────────────┼────────────────┐│
│                          │Yes                 │No              ││
│                          ▼                    ▼                ││
│                   ┌─────────────┐      ┌─────────────┐         ││
│                   │ 分块/预取   │      │ DRAM高?     │         ││
│                   │ 数据重排    │      └─────────────┘         ││
│                   └─────────────┘           │                  ││
│                                        ┌────┴────┐             ││
│                                        ▼         ▼             ││
│                                   ┌────────┐  ┌────────┐        ││
│                                   │NUMA优化│  │带宽优化│        ││
│                                   │大页支持│  │预取策略│        ││
│                                   └────────┘  └────────┘        ││
│                                                                 │
│      │                                                          │
│      ▼                                                          │
│  ┌─────────────┐                                                │
│  │ Bad_Spec >  │──Yes──► 分析分支预测失败                       │
│  │    10%?     │           │                                    │
│  └─────────────┘           ▼                                    │
│      No              ┌─────────────┐                            │
│                      │ 数据相关?   │──Yes──► 排序数据           │
│                      └─────────────┘      使用位运算替代分支    │
│                           │ No                                  │
│                           ▼                                     │
│                      ┌─────────────┐                            │
│                      │ 间接跳转多? │──Yes──► 转换为switch       │
│                      └─────────────┘      使用函数指针缓存      │
│                                                                 │
│      │                                                          │
│      ▼                                                          │
│  完成初步优化，重新测量验证                                     │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 6. 自动化分析工具

### 6.1 Python自动化分析脚本

```python
#!/usr/bin/env python3
"""
topdown_analyzer.py - Top-Down分析自动化工具
"""

import subprocess
import json
import re
import sys
from dataclasses import dataclass
from typing import Dict, List, Optional

@dataclass
class TopDownMetrics:
    frontend_bound: float
    backend_bound: float
    bad_speculation: float
    retiring: float

    def get_primary_bottleneck(self) -> str:
        metrics = {
            'Frontend Bound': self.frontend_bound,
            'Backend Bound': self.backend_bound,
            'Bad Speculation': self.bad_speculation,
            'Retiring': self.retiring
        }
        # 排除Retiring，找瓶颈
        bottlenecks = {k: v for k, v in metrics.items() if k != 'Retiring'}
        return max(bottlenecks, key=bottlenecks.get)

    def is_healthy(self) -> bool:
        return (self.frontend_bound < 15 and
                self.backend_bound < 20 and
                self.bad_speculation < 5 and
                self.retiring > 50)

class TopDownAnalyzer:
    def __init__(self, program: str):
        self.program = program
        self.metrics: Optional[TopDownMetrics] = None
        self.raw_data: Dict[str, int] = {}

    def run_perf_stat(self) -> bool:
        """运行perf stat收集数据"""
        events = [
            'cycles',
            'instructions',
            'uops_issued.any',
            'uops_retired.retire_slots',
            'int_misc.recovery_cycles',
            'br_misp_retired.all_branches'
        ]

        cmd = ['perf', 'stat', '-e', ','.join(events), '--', self.program]

        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=300
            )

            # 解析stderr (perf输出到stderr)
            self._parse_perf_output(result.stderr)
            return True
        except Exception as e:
            print(f"Error running perf: {e}")
            return False

    def _parse_perf_output(self, output: str):
        """解析perf输出"""
        patterns = {
            'cycles': r'(\d[\d,]*)\s+cycles',
            'instructions': r'(\d[\d,]*)\s+instructions',
            'uops_issued': r'(\d[\d,]*)\s+uops_issued\.any',
            'uops_retired': r'(\d[\d,]*)\s+uops_retired\.retire_slots',
            'recovery_cycles': r'(\d[\d,]*)\s+int_misc\.recovery_cycles',
            'br_misp': r'(\d[\d,]*)\s+br_misp_retired\.all_branches'
        }

        for key, pattern in patterns.items():
            match = re.search(pattern, output)
            if match:
                value = match.group(1).replace(',', '')
                self.raw_data[key] = int(value)

    def calculate_metrics(self) -> TopDownMetrics:
        """计算Top-Down指标"""
        cycles = self.raw_data.get('cycles', 1)
        uops_issued = self.raw_data.get('uops_issued', 0)
        uops_retired = self.raw_data.get('uops_retired', 0)
        recovery = self.raw_data.get('recovery_cycles', 0)

        # 假设4-wide处理器
        width = 4
        total_slots = cycles * width

        retiring = min(100.0, (uops_retired / total_slots) * 100) if total_slots > 0 else 0
        bad_spec = (recovery / cycles) * 100 if cycles > 0 else 0
        frontend = max(0, (uops_issued - uops_retired) / total_slots * 100) if total_slots > 0 else 0
        backend = max(0, 100 - retiring - bad_spec - frontend)

        self.metrics = TopDownMetrics(
            frontend_bound=frontend,
            backend_bound=backend,
            bad_speculation=bad_spec,
            retiring=retiring
        )

        return self.metrics

    def generate_report(self) -> str:
        """生成分析报告"""
        if not self.metrics:
            return "No metrics available"

        report = []
        report.append("=" * 60)
        report.append("Top-Down性能分析报告")
        report.append("=" * 60)
        report.append("")

        # Level 1指标
        report.append("Level 1 指标:")
        report.append(f"  Frontend Bound:  {self.metrics.frontend_bound:.2f}%")
        report.append(f"  Backend Bound:   {self.metrics.backend_bound:.2f}%")
        report.append(f"  Bad Speculation: {self.metrics.bad_speculation:.2f}%")
        report.append(f"  Retiring:        {self.metrics.retiring:.2f}%")
        report.append("")

        # 瓶颈分析
        bottleneck = self.metrics.get_primary_bottleneck()
        report.append(f"主要瓶颈: {bottleneck}")
        report.append("")

        # 优化建议
        report.append("优化建议:")
        report.extend(self._get_recommendations())
        report.append("")

        # 健康状态
        if self.metrics.is_healthy():
            report.append("✓ 整体性能状态良好")
        else:
            report.append("✗ 存在明显性能瓶颈，建议优化")

        return '\n'.join(report)

    def _get_recommendations(self) -> List[str]:
        """获取优化建议"""
        recommendations = []

        if self.metrics.frontend_bound > 15:
            recommendations.append("  Frontend优化:")
            recommendations.append("    - 优化代码布局，减少ICache未命中")
            recommendations.append("    - 内联小函数，减少分支")
            recommendations.append("    - 使用__builtin_expect优化分支预测")

        if self.metrics.backend_bound > 20:
            recommendations.append("  Backend优化:")
            if self.metrics.backend_bound > self.metrics.frontend_bound:
                recommendations.append("    - 检查Memory Bound子指标")
                recommendations.append("    - 应用缓存分块技术")
                recommendations.append("    - 优化数据访问模式")
            else:
                recommendations.append("    - 检查Core Bound子指标")
                recommendations.append("    - 减少数据依赖性")
                recommendations.append("    - 使用SIMD指令")

        if self.metrics.bad_speculation > 5:
            recommendations.append("  推测优化:")
            recommendations.append("    - 排序数据使分支更可预测")
            recommendations.append("    - 使用位运算替代条件分支")
            recommendations.append("    - 考虑使用无分支算法")

        if self.metrics.retiring < 40:
            recommendations.append("  执行效率:")
            recommendations.append("    - 检查指令效率")
            recommendations.append("    - 考虑算法复杂度优化")

        if not recommendations:
            recommendations.append("  当前性能状态良好，继续保持")

        return recommendations

def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <program> [args...]")
        sys.exit(1)

    program = sys.argv[1]

    print(f"Analyzing: {program}")
    print("Running perf stat...")

    analyzer = TopDownAnalyzer(program)

    if not analyzer.run_perf_stat():
        print("Failed to run perf stat")
        sys.exit(1)

    print("Calculating metrics...")
    analyzer.calculate_metrics()

    print()
    print(analyzer.generate_report())

if __name__ == '__main__':
    main()
```

---

## 7. 总结与最佳实践

### 7.1 Top-Down分析最佳实践

```
┌─────────────────────────────────────────────────────────────────┐
│                    Top-Down分析最佳实践                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  1. 始终从Level 1开始                                            │
│     - 不要直接深入细节                                          │
│     - 先确定主要瓶颈类别                                        │
│     - 避免"计数器驱动"的盲目分析                                │
│                                                                 │
│  2. 关注百分比而非原始值                                         │
│     - "100万cache-misses"无法判断严重程度                       │
│     - "15% DRAM Bound"明确指示优化方向                          │
│                                                                 │
│  3. 迭代式分析                                                   │
│     - Level 1 → Level 2 → Level 3                              │
│     - 每次只关注当前最大的瓶颈                                  │
│     - 优化后重新测量验证                                        │
│                                                                 │
│  4. 结合其他工具                                                 │
│     - Top-Down + 火焰图 = 定位具体热点函数                      │
│     - Top-Down + perf annotate = 代码级分析                     │
│     - Top-Down + toplev = 自动化层级分析                        │
│                                                                 │
│  5. 理解Retiring的含义                                           │
│     - 高Retiring ≠ 高性能                                       │
│     - 检查CPI确认执行效率                                       │
│     - 关注Retiring中的微码序列化比例                            │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 7.2 常见问题阈值参考

| 指标 | 警告阈值 | 严重阈值 | 优化优先级 |
|------|----------|----------|------------|
| Frontend Bound | > 15% | > 30% | 高 |
| Backend Bound | > 20% | > 40% | 高 |
| Bad Speculation | > 5% | > 15% | 中 |
| Retiring | < 40% | < 20% | 低 |
| DRAM Bound | > 10% | > 25% | 高 |
| Branch Mispredict | > 2% | > 8% | 中 |

### 7.3 参考资源

- Intel 64 and IA-32 Architectures Optimization Reference Manual
- A Top-Down Method for Performance Analysis and Counters Architecture (Ahmad Yasin)
- pmu-tools/toplev documentation: <https://github.com/andikleen/pmu-tools>
- Brendan Gregg's Performance Analysis Methodologies
