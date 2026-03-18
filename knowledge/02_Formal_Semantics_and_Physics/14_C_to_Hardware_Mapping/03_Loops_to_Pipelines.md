# 循环到流水线的转换与优化

## 目录

- [循环到流水线的转换与优化](#循环到流水线的转换与优化)
  - [目录](#目录)
  - [1. 循环在硬件中的实现](#1-循环在硬件中的实现)
    - [1.1 循环的硬件映射概念](#11-循环的硬件映射概念)
    - [1.2 循环变量与计数器硬件](#12-循环变量与计数器硬件)
    - [1.3 嵌套循环的硬件结构](#13-嵌套循环的硬件结构)
  - [2. 循环展开（Unroll）](#2-循环展开unroll)
    - [2.1 循环展开原理](#21-循环展开原理)
    - [2.2 不同展开策略对比](#22-不同展开策略对比)
    - [2.3 展开的资源-性能权衡](#23-展开的资源-性能权衡)
    - [2.4 部分展开与循环重构](#24-部分展开与循环重构)
  - [3. 流水线（Pipeline）](#3-流水线pipeline)
    - [3.1 流水线基本原理](#31-流水线基本原理)
    - [3.2 流水线深度与延迟](#32-流水线深度与延迟)
    - [3.3 嵌套循环的流水线策略](#33-嵌套循环的流水线策略)
    - [3.4 PIPELINE指令详解](#34-pipeline指令详解)
  - [4. 启动间隔（II）优化](#4-启动间隔ii优化)
    - [4.1 II的基本概念](#41-ii的基本概念)
    - [4.2 影响II的因素](#42-影响ii的因素)
    - [4.3 II优化技术](#43-ii优化技术)
  - [5. 数据依赖处理](#5-数据依赖处理)
    - [5.1 数据依赖类型](#51-数据依赖类型)
    - [5.2 依赖分析与解决](#52-依赖分析与解决)
    - [5.3 复杂依赖场景](#53-复杂依赖场景)
  - [6. 对比分析：顺序vs并行](#6-对比分析顺序vs并行)
    - [6.1 执行模式对比表](#61-执行模式对比表)
    - [6.2 硬件实现对比](#62-硬件实现对比)
    - [6.3 综合报告解读](#63-综合报告解读)
  - [7. 完整案例研究](#7-完整案例研究)
    - [7.1 FIR滤波器的循环优化演进](#71-fir滤波器的循环优化演进)
    - [7.2 矩阵乘法的完整优化](#72-矩阵乘法的完整优化)
    - [7.3 优化决策流程图](#73-优化决策流程图)
  - [总结](#总结)

---

## 1. 循环在硬件中的实现

### 1.1 循环的硬件映射概念

在软件中，循环是重复执行相同指令序列的控制结构。在硬件中，循环可以映射为多种实现方式：

```
┌─────────────────────────────────────────────────────────────────────┐
│                    循环到硬件的映射方式                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  C代码：for (i=0; i<N; i++) { ... }                                 │
│                                                                      │
│  映射选项1：顺序执行（最小资源）                                     │
│  ┌─────────────────────────────────────────────────────────┐        │
│  │  迭代0 ──▶ 迭代1 ──▶ 迭代2 ──▶ ... ──▶ 迭代N-1         │        │
│  │  (顺序复用同一硬件单元)                                  │        │
│  └─────────────────────────────────────────────────────────┘        │
│                                                                      │
│  映射选项2：完全展开（最大并行）                                     │
│  ┌─────────────────────────────────────────────────────────┐        │
│  │  迭代0                                                  │        │
│  │  迭代1    （同时并行执行）                               │        │
│  │  迭代2                                                   │        │
│  │  ...                                                     │        │
│  │  迭代N-1  （需要N份硬件资源）                            │        │
│  └─────────────────────────────────────────────────────────┘        │
│                                                                      │
│  映射选项3：流水线（吞吐优化）                                       │
│  ┌─────────────────────────────────────────────────────────┐        │
│  │  周期0: 迭代0启动                                        │        │
│  │  周期1: 迭代0执行中 + 迭代1启动                          │        │
│  │  周期2: 迭代0完成   + 迭代1执行中 + 迭代2启动            │        │
│  │  ...                                                    │        │
│  │  （重叠执行，每周期完成一次迭代）                         │        │
│  └─────────────────────────────────────────────────────────┘        │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 1.2 循环变量与计数器硬件

```c
// C循环
for (int i = 0; i < 100; i++) {
    sum += array[i];
}

// 对应的硬件结构
/*
┌─────────────────────────────────────────────────────────────┐
│                    循环计数器硬件                           │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│    ┌──────────┐     ┌──────────┐     ┌──────────┐          │
│    │ 计数器   │────▶│ 比较器   │────▶│ 控制逻辑 │          │
│    │ (i_reg)  │     │ (i<100)  │     │          │          │
│    └────┬─────┘     └──────────┘     └────┬─────┘          │
│         │                                 │                 │
│         │         ┌──────────┐            │                 │
│         └────────▶│  +1      │◀───────────┘                 │
│                   │ 加法器   │                               │
│                   └──────────┘                               │
│                                                              │
│  状态机：IDLE ──▶ INIT ──▶ EXEC ──▶ INC ──▶ CHECK ──▶ DONE  │
│                                                              │
└─────────────────────────────────────────────────────────────┘
*/
```

### 1.3 嵌套循环的硬件结构

```c
// 嵌套循环
for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
        result[i][j] = process(data[i][j]);
    }
}

/*
┌─────────────────────────────────────────────────────────────────────┐
│                    嵌套循环硬件实现                                  │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  外层循环计数器 (i)                                                  │
│  ┌─────────────────┐                                                 │
│  │ 计数范围: 0~M-1 │                                                 │
│  │ 状态: i_count   │───▶ 触发内层循环开始                           │
│  └─────────────────┘                                                 │
│           │                                                          │
│           ▼                                                          │
│  内层循环计数器 (j)                                                  │
│  ┌─────────────────┐     ┌──────────────────┐                       │
│  │ 计数范围: 0~N-1 │───▶  │ 数据处理单元     │                       │
│  │ 状态: j_count   │     │ (process函数)    │                       │
│  └─────────────────┘     └──────────────────┘                       │
│           │                                                          │
│           └───────────────────────────────▶ 完成时外层计数器+1       │
│                                                                      │
│  地址生成：addr = i * N + j                                          │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
*/
```

---

## 2. 循环展开（Unroll）

### 2.1 循环展开原理

循环展开通过复制循环体来提高并行性，减少循环控制开销：

```c
// 原始循环
for (int i = 0; i < 8; i++) {
    sum += a[i] * b[i];
}

// 展开因子为2
#pragma HLS UNROLL factor=2
for (int i = 0; i < 8; i++) {
    sum += a[i] * b[i];
}

// 等价于：
for (int i = 0; i < 8; i += 2) {
    sum += a[i] * b[i];       // 第i次迭代
    sum += a[i+1] * b[i+1];   // 第i+1次迭代（并行执行）
}

// 完全展开
#pragma HLS UNROLL
for (int i = 0; i < 8; i++) {
    sum += a[i] * b[i];
}

// 等价于：
sum += a[0] * b[0];
sum += a[1] * b[1];
sum += a[2] * b[2];
// ...（所有8次迭代同时并行）
```

### 2.2 不同展开策略对比

```c
// 示例：8元素向量点积
int dot_product(int a[8], int b[8]) {
    int sum = 0;

    // 策略1：不展开（顺序执行）
    // 周期数：约8 * T_process
    // 资源：1个乘法器 + 1个加法器
    for (int i = 0; i < 8; i++) {
        sum += a[i] * b[i];
    }

    // 策略2：展开因子2
    // 周期数：约4 * T_process
    // 资源：2个乘法器 + 2个加法器
    #pragma HLS UNROLL factor=2
    for (int i = 0; i < 8; i++) {
        sum += a[i] * b[i];
    }

    // 策略3：完全展开
    // 周期数：1个周期（组合逻辑）或T_process（流水线）
    // 资源：8个乘法器 + 8个加法器 + 加法树
    #pragma HLS UNROLL
    for (int i = 0; i < 8; i++) {
        #pragma HLS PIPELINE
        sum += a[i] * b[i];
    }

    return sum;
}
```

### 2.3 展开的资源-性能权衡

```
┌─────────────────────────────────────────────────────────────────────┐
│              循环展开的资源-性能对比（点积运算）                      │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  展开因子    乘法器    加法器    LUT    FF    周期数    吞吐         │
│  ────────────────────────────────────────────────────────────────  │
│  1 (基础)    1         1         50     30     8         低         │
│  2           2         2         95     55     4         中         │
│  4           4         4         180    105    2         高         │
│  8 (完全)    8         7*        320    200    1         最高       │
│                                                                      │
│  *使用加法树减少加法器数量                                           │
│                                                                      │
│  关键观察：                                                          │
│  - 资源与展开因子近似线性关系                                        │
│  - 周期数与展开因子成反比                                            │
│  - 存在收益递减点（受限于存储器端口）                                │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
*/
```

### 2.4 部分展开与循环重构

```c
// 问题：循环边界不能被展开因子整除
void imperfect_loop(int a[100], int b[100]) {
    int sum = 0;

    // 方法1：调整循环边界
    #pragma HLS UNROLL factor=8
    for (int i = 0; i < 96; i++) {  // 调整为8的倍数
        sum += a[i] * b[i];
    }
    // 处理剩余4个元素
    for (int i = 96; i < 100; i++) {
        sum += a[i] * b[i];
    }
}

// 方法2：使用HLS自动处理（skip_exit_check）
void auto_unroll(int a[100], int b[100]) {
    int sum = 0;

    #pragma HLS UNROLL factor=8 skip_exit_check
    for (int i = 0; i < 100; i++) {
        sum += a[i] * b[i];
    }
}

// 方法3：显式的展开循环
void manual_unroll(int a[100], int b[100]) {
    int sum = 0;

    // 主展开部分
    for (int i = 0; i < 96; i += 8) {
        #pragma HLS PIPELINE
        #pragma HLS UNROLL
        sum += a[i+0] * b[i+0];
        sum += a[i+1] * b[i+1];
        sum += a[i+2] * b[i+2];
        sum += a[i+3] * b[i+3];
        sum += a[i+4] * b[i+4];
        sum += a[i+5] * b[i+5];
        sum += a[i+6] * b[i+6];
        sum += a[i+7] * b[i+7];
    }

    // 收尾部分
    #pragma HLS PIPELINE
    for (int i = 96; i < 100; i++) {
        sum += a[i] * b[i];
    }
}
```

---

## 3. 流水线（Pipeline）

### 3.1 流水线基本原理

流水线允许循环迭代的重叠执行，提高整体吞吐率：

```c
// 简单流水线示例
void simple_pipeline(int in[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        int a = in[i];
        int b = a * 2;
        int c = b + 1;
        out[i] = c;
    }
}

/*
流水线执行时序（II=1）：

周期     1      2      3      4      5      6      7      8
        ─────────────────────────────────────────────────────
迭代0   │读a   │乘2   │加1   │写out │      │      │      │
迭代1   │      │读a   │乘2   │加1   │写out │      │      │
迭代2   │      │      │读a   │乘2   │加1   │写out │      │
迭代3   │      │      │      │读a   │乘2   │加1   │写out │
        ─────────────────────────────────────────────────────

每个周期启动一个新迭代，7个周期后开始每周期输出一个结果
总周期数 = 启动间隔(II) * 迭代次数 + 流水线深度 - 1
         = 1 * 100 + 3 = 103周期
*/
```

### 3.2 流水线深度与延迟

```c
// 多层流水线示例
void deep_pipeline(int in[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1

        // 阶段1：内存读取（2周期延迟）
        int data = in[i];

        // 阶段2：乘法运算（3周期延迟）
        int mult = data * 7;

        // 阶段3：加法运算（1周期延迟）
        int add = mult + 3;

        // 阶段4：比较选择（1周期延迟）
        int result = (add > 100) ? add : 100;

        // 阶段5：内存写入（2周期延迟）
        out[i] = result;
    }
}

/*
流水线结构：

┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐
│ 读数据   │───▶│ 乘法器  │───▶│ 加法器  │───▶│ 选择器  │───▶│ 写数据  │
│ (2级)   │    │ (3级)   │    │ (1级)   │    │ (1级)   │    │ (2级)   │
└─────────┘    └─────────┘    └─────────┘    └─────────┘    └─────────┘
    │              │              │              │              │
    └──────────────┴──────────────┴──────────────┴──────────────┘
                           流水线寄存器

总流水线深度 = 2 + 3 + 1 + 1 + 2 = 9级
启动间隔(II) = 1
首次输出延迟 = 9周期
总执行时间 = 100 + 9 - 1 = 108周期
*/
```

### 3.3 嵌套循环的流水线策略

```c
// 策略1：仅内层流水线
void inner_pipeline(int a[10][10], int b[10][10], int c[10][10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            #pragma HLS PIPELINE II=1
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}
// 性能：外层串行，内层流水线

// 策略2：完美嵌套循环展平
void flattened_pipeline(int a[10][10], int b[10][10], int c[10][10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            #pragma HLS PIPELINE II=1
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}

// 策略3：DATAFLOW优化（函数级并行）
void dataflow_pipeline(int a[10][10], int b[10][10], int c[10][10]) {
    #pragma HLS DATAFLOW

    int buf1[10][10], buf2[10][10];

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            #pragma HLS PIPELINE
            buf1[i][j] = a[i][j] * 2;
        }
    }

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            #pragma HLS PIPELINE
            buf2[i][j] = buf1[i][j] + b[i][j];
        }
    }

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            #pragma HLS PIPELINE
            c[i][j] = buf2[i][j] - 1;
        }
    }
}

// 策略4：循环变换优化
void optimized_nested(int a[10][10], int b[10][10], int c[10][10]) {
    #pragma HLS ARRAY_PARTITION variable=a complete dim=2
    #pragma HLS ARRAY_PARTITION variable=b complete dim=2
    #pragma HLS ARRAY_PARTITION variable=c complete dim=2

    for (int i = 0; i < 10; i++) {
        #pragma HLS PIPELINE II=1
        for (int j = 0; j < 10; j++) {
            #pragma HLS UNROLL
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}
```

### 3.4 PIPELINE指令详解

```c
// II (Initiation Interval) - 启动间隔
void pipeline_ii(int in[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=2  // 每2周期启动一次迭代
        out[i] = in[i] * 3;
    }
}
// 资源减半，吞吐减半

// rewind - 循环间流水线
void pipeline_rewind(int in[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE rewind
        out[i] = complex_function(in[i]);
    }
}
// 当前循环结束时启动下一次循环

// off - 禁用流水线
void no_pipeline(int in[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE off
        out[i] = in[i] * 3;
    }
}
// 完全顺序执行
```

---

## 4. 启动间隔（II）优化

### 4.1 II的基本概念

启动间隔（Initiation Interval）是流水线中连续两次迭代启动之间的时钟周期数。

```c
// II=1：理想情况
void ideal_ii(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        c[i] = a[i] + b[i];  // 无依赖，可实现II=1
    }
}

// II>1：存在资源或数据依赖限制
void limited_ii(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=2  // 可能需要II=2
        c[i] = a[i] * b[i];  // 乘法器资源限制
    }
}
```

### 4.2 影响II的因素

```c
// 因素1：资源限制
void resource_limited(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        // 如果不展开，II=1需要多个乘法器
        #pragma HLS PIPELINE II=1
        c[i] = a[i] * b[i] * 3;  // 需要2个乘法器
    }
}

// 解决方案：资源分配或放宽II
void resource_solved(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=2  // 允许资源复用
        #pragma HLS ALLOCATION instances=mul limit=1 operation
        c[i] = a[i] * b[i] * 3;
    }
}

// 因素2：存储器访问限制
void memory_limited(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        // 每个周期需要2读1写，可能需要双端口RAM
        c[i] = a[i] + b[i];
    }
}

// 解决方案：数组分区
void memory_solved(int a[100], int b[100], int c[100]) {
    #pragma HLS ARRAY_PARTITION variable=a complete
    #pragma HLS ARRAY_PARTITION variable=b complete
    #pragma HLS ARRAY_PARTITION variable=c complete

    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS UNROLL
        c[i] = a[i] + b[i];
    }
}

// 因素3：数据依赖（真依赖/RAW）
void dependency_limited(int a[100], int *sum) {
    int s = 0;
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1  // 无法达到，存在循环间依赖
        s = s + a[i];  // RAW依赖：s的写依赖上一次迭代的读
    }
    *sum = s;
}

// 解决方案：展开+树形归约
void dependency_solved(int a[100], int *sum) {
    int partial[4] = {0};
    #pragma HLS ARRAY_PARTITION variable=partial complete

    for (int i = 0; i < 100; i += 4) {
        #pragma HLS PIPELINE II=1
        #pragma HLS UNROLL factor=4
        partial[0] += a[i+0];
        partial[1] += a[i+1];
        partial[2] += a[i+2];
        partial[3] += a[i+3];
    }

    *sum = partial[0] + partial[1] + partial[2] + partial[3];
}
```

### 4.3 II优化技术

```c
// 技术1：循环展开提高II
void unroll_for_ii(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i += 4) {
        #pragma HLS PIPELINE II=1
        #pragma HLS UNROLL factor=4
        c[i+0] = a[i+0] * b[i+0];
        c[i+1] = a[i+1] * b[i+1];
        c[i+2] = a[i+2] * b[i+2];
        c[i+3] = a[i+3] * b[i+3];
    }
}

// 技术2：操作符调度优化
void schedule_optimization(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        int temp1 = a[i] * 3;  // 乘法
        int temp2 = b[i] + 5;  // 加法（无依赖，可并行）
        c[i] = temp1 + temp2;  // 加法
    }
}

// 技术3：存储器bank优化
void bank_optimization(int a[100][4], int b[100]) {
    #pragma HLS ARRAY_PARTITION variable=a dim=2 factor=4

    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS UNROLL factor=4
        for (int j = 0; j < 4; j++) {
            b[i] += a[i][j];
        }
    }
}

// 技术4：数据打包减少访问
void data_packing(ap_uint<128> *packed, int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        ap_uint<128> p = packed[i];
        // 解包4个32位整数
        int a = p.range(31, 0);
        int b = p.range(63, 32);
        int c = p.range(95, 64);
        int d = p.range(127, 96);
        out[i] = a + b + c + d;
    }
}
```

---

## 5. 数据依赖处理

### 5.1 数据依赖类型

```c
// 类型1：RAW (Read After Write) - 真依赖
void raw_dependency(int a[100], int *result) {
    int acc = 0;
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        // 迭代i+1读取acc，迭代i写入acc
        acc = acc + a[i];  // RAW依赖
    }
    *result = acc;
}

// 类型2：WAR (Write After Read) - 反依赖
void war_dependency(int a[100], int b[100]) {
    int prev = 0;
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        int temp = prev + a[i];  // 读取prev
        prev = b[i];             // 写入prev（反依赖）
        b[i] = temp;
    }
}

// 类型3：WAW (Write After Write) - 输出依赖
void waw_dependency(int a[100], int *result) {
    int x;
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        x = a[i] * 2;  // WAW：连续写入x
    }
    *result = x;
}

// 类型4：数组下标依赖
void array_dependency(int a[100], int idx[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        a[idx[i]] = a[idx[i]] + 1;  // 可能存在依赖
    }
}
```

### 5.2 依赖分析与解决

```c
// 解决方案1：依赖指示
void explicit_dep_pragma(int a[100], int *result) {
    int acc = 0;
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS DEPENDENCE variable=acc inter false
        // 告诉编译器acc在迭代间无依赖（如果不正确会导致错误结果）
        acc = acc + a[i];
    }
    *result = acc;
}

// 解决方案2：循环变换（循环展开归约）
void reduction_tree(int a[100], int *result) {
    // 第一层：并行归约
    int level1[10];
    #pragma HLS ARRAY_PARTITION variable=level1 complete

    for (int i = 0; i < 10; i++) {
        #pragma HLS PIPELINE
        level1[i] = 0;
        for (int j = 0; j < 10; j++) {
            #pragma HLS UNROLL
            level1[i] += a[i * 10 + j];
        }
    }

    // 第二层：求和
    int sum = 0;
    for (int i = 0; i < 10; i++) {
        #pragma HLS UNROLL
        sum += level1[i];
    }

    *result = sum;
}

// 解决方案3：标量替换消除依赖
void scalar_replacement(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        // 使用局部变量消除存储器依赖
        int ai = a[i];  // 只读一次
        int bi = b[i];  // 只读一次
        int ci = ai * ai + bi * bi;  // 计算
        c[i] = ci;      // 只写一次
    }
}

// 解决方案4：循环分块
void loop_tiling(int a[1000], int *result) {
    int sum = 0;
    #define TILE_SIZE 10

    for (int tile = 0; tile < 100; tile++) {
        int tile_sum = 0;

        // 分块内并行归约
        for (int i = 0; i < TILE_SIZE; i++) {
            #pragma HLS PIPELINE
            #pragma HLS UNROLL
            tile_sum += a[tile * TILE_SIZE + i];
        }

        // 块间串行累加
        sum += tile_sum;
    }

    *result = sum;
}
```

### 5.3 复杂依赖场景

```c
// 场景1：FIR滤波器（滑动窗口依赖）
void fir_with_dependency(int x[100], int y[100], int coeff[4]) {
    int shift_reg[4];
    #pragma HLS ARRAY_PARTITION variable=shift_reg complete

    for (int n = 0; n < 100; n++) {
        #pragma HLS PIPELINE II=1

        // 移位操作 - 迭代间依赖
        for (int i = 3; i > 0; i--) {
            #pragma HLS UNROLL
            shift_reg[i] = shift_reg[i-1];
        }
        shift_reg[0] = x[n];

        // 乘累加
        int acc = 0;
        for (int i = 0; i < 4; i++) {
            #pragma HLS UNROLL
            acc += shift_reg[i] * coeff[i];
        }
        y[n] = acc;
    }
}

// 场景2：循环携带依赖的矩阵运算
void matrix_carry_dependency(int a[100][100]) {
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            #pragma HLS PIPELINE
            // 对角线依赖
            if (i > 0 && j > 0) {
                a[i][j] = a[i][j] + a[i-1][j-1];
            }
        }
    }
}

// 场景3：间接索引依赖
void indirect_indexing(int data[100], int idx[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        #pragma HLS DEPENDENCE variable=data inter false
        // 假设idx没有重复，手动声明无依赖
        int addr = idx[i];
        data[addr] = data[addr] * 2;
        out[i] = data[addr];
    }
}
```

---

## 6. 对比分析：顺序vs并行

### 6.1 执行模式对比表

```
┌────────────────────────────────────────────────────────────────────────┐
│                     循环执行模式对比                                    │
├────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  特性              顺序执行      循环展开      流水线      DATAFLOW     │
│  ────────────────────────────────────────────────────────────────────  │
│  资源使用          最低         高           中等         高            │
│  吞吐率            最低         高           高           最高          │
│  延迟              最高         低           中等         低            │
│  控制逻辑          简单         复杂         中等         复杂          │
│  适用场景          小数据量     低迭代次数   大数据流     多阶段处理    │
│                                                                         │
├────────────────────────────────────────────────────────────────────────┤
│  性能指标（假设100次迭代，每迭代3个操作）                                │
│                                                                         │
│  顺序：    100 * 3 = 300周期                                           │
│  展开x4：  25 * 3 = 75周期（资源x4）                                    │
│  流水线：  100 + 3 - 1 = 102周期（II=1）                                │
│  DATAFLOW：约100周期（3阶段重叠）                                       │
│                                                                         │
└────────────────────────────────────────────────────────────────────────┘
*/
```

### 6.2 硬件实现对比

```c
// 示例：向量加法
void vector_add(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        c[i] = a[i] + b[i];
    }
}

// 版本A：顺序实现
/*
硬件结构：
┌────────────────────────────────────────┐
│           顺序执行单元                  │
├────────────────────────────────────────┤
│  计数器 ──▶ 地址生成 ──▶ 存储器访问    │
│                              │         │
│                              ▼         │
│                         加法器（复用）  │
│                              │         │
│                              ▼         │
│                         写回存储器     │
└────────────────────────────────────────┘
资源：~50 LUTs, ~30 FFs, 1个加法器
周期：~300周期
*/

// 版本B：完全展开
#pragma HLS UNROLL
/*
硬件结构：
┌────────────────────────────────────────────────────────────────┐
│                    完全并行实现                                 │
├────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────┐  ┌─────┐  ┌─────┐        ┌─────┐  ┌─────┐  ┌─────┐   │
│  │ADD0 │  │ADD1 │  │ADD2 │  ...   │ADD97│  │ADD98│  │ADD99│   │
│  └──┬──┘  └──┬──┘  └──┬──┘        └──┬──┘  └──┬──┘  └──┬──┘   │
│     │        │        │              │        │        │       │
│     └────────┴────────┴──────┬───────┴────────┴────────┘       │
│                              ▼                                  │
│                      组合逻辑或1周期完成                        │
│                                                                 │
└────────────────────────────────────────────────────────────────┘
资源：~5000 LUTs, ~3000 FFs, 100个加法器
周期：1周期（组合逻辑）或几周期（流水线）
*/

// 版本C：流水线
#pragma HLS PIPELINE II=1
/*
硬件结构：
┌────────────────────────────────────────────────────────────────┐
│                    流水线实现                                   │
├────────────────────────────────────────────────────────────────┤
│                                                                 │
│   周期0    周期1    周期2    周期3    周期4    ...              │
│     │        │        │        │        │                       │
│     ▼        ▼        ▼        ▼        ▼                       │
│  ┌─────┐  ┌─────┐  ┌─────┐  ┌─────┐  ┌─────┐                  │
│  │读a  │─▶│读b  │─▶│加法 │─▶│写c  │  │      │                  │
│  └─────┘  └─────┘  └─────┘  └─────┘  └─────┘                  │
│     │        │        │        │                                │
│  ┌─────┐  ┌─────┐  ┌─────┐  ┌─────┐                            │
│  │读a  │─▶│读b  │─▶│加法 │─▶│写c  │   （下一次迭代）            │
│  └─────┘  └─────┘  └─────┘  └─────┘                            │
│     │        │        │        │                                │
│     ▼        ▼        ▼        ▼                                │
│   重叠执行，每周期输出一个结果                                   │
│                                                                 │
└────────────────────────────────────────────────────────────────┘
资源：~200 LUTs, ~150 FFs, 1个加法器 + 流水线寄存器
周期：~102周期（100次迭代 + 2周期流水线深度）
吞吐：每周期1个结果
*/
```

### 6.3 综合报告解读

```
┌─────────────────────────────────────────────────────────────────────┐
│                    综合报告示例（向量加法）                          │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  == 性能估计 ==                                                       │
│  +---------------------+---------+---------+----------+              │
│  |       模块          |  延迟   | 间隔    |  类型    |              │
│  +---------------------+---------+---------+----------+              │
│  | vector_add          | 103     | 103     | 无       |  // 顺序    │
│  | vector_add_unroll4  | 28      | 28      | 无       |  // 展开x4  │
│  | vector_add_pipeline | 103     | 1       | 流水线   |  // 流水线  │
│  +---------------------+---------+---------+----------+              │
│                                                                      │
│  == 资源使用 ==                                                       │
│  +---------------------+------+------+------+------+                 │
│  |       模块          | BRAM | DSP  |  FF  |  LUT |                 │
│  +---------------------+------+------+------+------+                 │
│  | vector_add          |   0  |   0  |  45  |  52  |                 │
│  | vector_add_unroll4  |   0  |   0  |  180 |  210 |                 │
│  | vector_add_pipeline |   0  |   0  |  156 |  178 |                 │
│  +---------------------+------+------+------+------+                 │
│                                                                      │
│  == 流水线分析（vector_add_pipeline）==                               │
│  +--------------+---------+---------+--------+--------+              │
│  |   循环       | II      | 深度    | 瓶颈   | 建议   |              │
│  +--------------+---------+---------+--------+--------+              │
│  | Loop1        | 1       | 2       | 无     | -      |              │
│  +--------------+---------+---------+--------+--------+              │
│                                                                      │
│  瓶颈分析：                                                           │
│  - 达到目标II=1                                                       │
│  - 限制因素：存储器访问延迟                                           │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
*/
```

---

## 7. 完整案例研究

### 7.1 FIR滤波器的循环优化演进

```c
// 版本1：基础实现
void fir_basic(int x[1000], int y[1000], int coeff[16]) {
    for (int n = 0; n < 1000; n++) {
        int acc = 0;
        for (int k = 0; k < 16; k++) {
            if (n - k >= 0) {
                acc += x[n - k] * coeff[k];
            }
        }
        y[n] = acc;
    }
}
// 性能：16000周期，资源少，启动间隔高

// 版本2：流水线优化
void fir_pipeline(int x[1000], int y[1000], int coeff[16]) {
    int shift_reg[16];

    for (int n = 0; n < 1000; n++) {
        #pragma HLS PIPELINE II=1

        // 移位
        for (int k = 15; k > 0; k--) {
            #pragma HLS UNROLL
            shift_reg[k] = shift_reg[k-1];
        }
        shift_reg[0] = x[n];

        // MAC
        int acc = 0;
        for (int k = 0; k < 16; k++) {
            #pragma HLS UNROLL
            acc += shift_reg[k] * coeff[k];
        }
        y[n] = acc;
    }
}
// 性能：~1000周期，II=1，16个乘法器

// 版本3：资源优化（乘法器共享）
void fir_resource_opt(int x[1000], int y[1000], int coeff[16]) {
    #pragma HLS ARRAY_PARTITION variable=coeff complete

    int shift_reg[16];
    #pragma HLS ARRAY_PARTITION variable=shift_reg complete

    for (int n = 0; n < 1000; n++) {
        #pragma HLS PIPELINE II=4  // 放宽II以共享资源

        // 移位
        for (int k = 15; k > 0; k--) {
            #pragma HLS UNROLL
            shift_reg[k] = shift_reg[k-1];
        }
        shift_reg[0] = x[n];

        // MAC - 每周期4次乘法，复用4个乘法器
        int acc = 0;
        for (int k = 0; k < 16; k++) {
            #pragma HLS UNROLL factor=4
            acc += shift_reg[k] * coeff[k];
        }
        y[n] = acc;
    }
}
// 性能：~4000周期，4个乘法器，面积减少75%

// 版本4：对称系数优化
void fir_symmetric(int x[1000], int y[1000], int coeff[8]) {
    // 利用FIR系数对称性：h[k] = h[N-1-k]
    int shift_reg[16];
    #pragma HLS ARRAY_PARTITION variable=shift_reg complete

    for (int n = 0; n < 1000; n++) {
        #pragma HLS PIPELINE II=1

        // 移位
        for (int k = 15; k > 0; k--) {
            #pragma HLS UNROLL
            shift_reg[k] = shift_reg[k-1];
        }
        shift_reg[0] = x[n];

        // 对称MAC - 预加减少乘法器
        int acc = 0;
        for (int k = 0; k < 8; k++) {
            #pragma HLS UNROLL
            acc += (shift_reg[k] + shift_reg[15-k]) * coeff[k];
        }
        y[n] = acc;
    }
}
// 性能：~1000周期，8个乘法器，利用对称性减少一半
```

### 7.2 矩阵乘法的完整优化

```c
// 版本1：基础三重循环
void matmul_basic(int A[32][32], int B[32][32], int C[32][32]) {
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            int sum = 0;
            for (int k = 0; k < 32; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}
// 周期：32*32*32 = 32768

// 版本2：内层流水线
void matmul_inner_pipeline(int A[32][32], int B[32][32], int C[32][32]) {
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            #pragma HLS PIPELINE II=1
            int sum = 0;
            for (int k = 0; k < 32; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}
// 周期：32*32 + 32 = 1056，但k循环串行

// 版本3：循环交换 + 展开
void matmul_optimized(int A[32][32], int B[32][32], int C[32][32]) {
    #pragma HLS ARRAY_PARTITION variable=A dim=2 factor=8 cyclic
    #pragma HLS ARRAY_PARTITION variable=B dim=1 factor=8 cyclic
    #pragma HLS ARRAY_PARTITION variable=C dim=0

    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            #pragma HLS PIPELINE II=1

            int sum = 0;
            for (int k = 0; k < 32; k += 8) {
                #pragma HLS UNROLL factor=8
                sum += A[i][k+0] * B[k+0][j] +
                       A[i][k+1] * B[k+1][j] +
                       A[i][k+2] * B[k+2][j] +
                       A[i][k+3] * B[k+3][j] +
                       A[i][k+4] * B[k+4][j] +
                       A[i][k+5] * B[k+5][j] +
                       A[i][k+6] * B[k+6][j] +
                       A[i][k+7] * B[k+7][j];
            }
            C[i][j] = sum;
        }
    }
}
// 周期：32*32 + 4 = 1028，8个并行乘法器

// 版本4：分块矩阵乘法（Cache友好）
void matmul_blocked(int A[32][32], int B[32][32], int C[32][32]) {
    #define BLOCK 8

    for (int ii = 0; ii < 32; ii += BLOCK) {
        for (int jj = 0; jj < 32; jj += BLOCK) {
            for (int kk = 0; kk < 32; kk += BLOCK) {

                // 加载块到本地存储
                int A_block[BLOCK][BLOCK];
                int B_block[BLOCK][BLOCK];
                #pragma HLS ARRAY_PARTITION variable=A_block complete
                #pragma HLS ARRAY_PARTITION variable=B_block complete

                for (int i = 0; i < BLOCK; i++) {
                    for (int k = 0; k < BLOCK; k++) {
                        #pragma HLS PIPELINE
                        A_block[i][k] = A[ii+i][kk+k];
                        B_block[i][k] = B[kk+k][jj+k];
                    }
                }

                // 块内矩阵乘法
                for (int i = 0; i < BLOCK; i++) {
                    for (int j = 0; j < BLOCK; j++) {
                        #pragma HLS PIPELINE II=1
                        int sum = C[ii+i][jj+j];
                        for (int k = 0; k < BLOCK; k++) {
                            #pragma HLS UNROLL
                            sum += A_block[i][k] * B_block[k][j];
                        }
                        C[ii+i][jj+j] = sum;
                    }
                }
            }
        }
    }
}
// 更好的数据局部性，适合大规模矩阵
```

### 7.3 优化决策流程图

```
                    开始优化循环
                         │
                         ▼
            循环是否满足PIPELINE条件？
                         │
           ┌─────────────┴─────────────┐
           │是                         │否
           ▼                           ▼
    应用PIPELINE II=1          循环迭代次数小？
           │                           │
           │           ┌───────────────┴───────────────┐
           │           │是                             │否
           ▼           ▼                               ▼
    能达到目标II？   完全展开循环                 保持顺序执行
           │           │                               │
      ┌────┴────┐      ▼                               │
      │是      │否   资源可接受？                       │
      ▼        ▼          │                            │
   完成优化   分析瓶颈    ├─────┐                       │
               │          │是   │否                     │
               ▼          ▼     ▼                      │
        ┌──────┴──────┐ 完成  部分展开                  │
        ▼             ▼                                │
    资源限制      数据依赖                               │
        │             │                                │
        ▼             ▼                                │
   放宽II或      循环变换/重排序                         │
   数组分区      或接受II>1                              │
```

---

## 总结

本章深入探讨了循环到硬件流水线的转换与优化：

1. **循环映射**：理解循环在硬件中的多种实现方式
2. **循环展开**：通过空间并行提高性能，需要权衡资源消耗
3. **流水线技术**：实现时间并行，提高整体吞吐率
4. **II优化**：分析并解决限制启动间隔的因素
5. **数据依赖**：识别和处理各种依赖关系
6. **性能对比**：顺序、展开、流水线的综合比较
7. **实战案例**：FIR滤波器和矩阵乘法的完整优化过程

掌握这些技术是成为高效HLS设计者的关键。
