# 性能工程方法论

> **层级定位**: 01 Core Knowledge System / 05 Engineering Layer
> **参考课程**: MIT 6.172, Stanford CS149, CMU 15-418
> **难度级别**: L4-L5
> **预估学习时间**: 12-15 小时

---

## 目录

- [性能工程方法论](#性能工程方法论)
  - [目录](#目录)
  - [1. 性能工程概述](#1-性能工程概述)
    - [1.1 性能工程vs性能优化](#11-性能工程vs性能优化)
    - [1.2 Amdahl定律](#12-amdahl定律)
  - [2. 性能测量](#2-性能测量)
    - [2.1 测量方法论](#21-测量方法论)
    - [2.2 计时方法](#22-计时方法)
  - [3. 性能分析工具](#3-性能分析工具)
    - [3.1 采样分析器](#31-采样分析器)
    - [3.2 插桩分析](#32-插桩分析)
  - [4. 微架构优化](#4-微架构优化)
    - [4.1 CPU流水线](#41-cpu流水线)
    - [4.2 内存层次优化](#42-内存层次优化)
    - [4.3 向量化](#43-向量化)
  - [5. Roofline模型](#5-roofline模型)
    - [5.1 性能上限分析](#51-性能上限分析)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. 性能工程概述

### 1.1 性能工程vs性能优化

```
性能优化 (Performance Optimization)    性能工程 (Performance Engineering)
      │                                        │
      ▼                                        ▼
• 特定问题修复                        • 系统性方法论
• 点对点改进                          • 全生命周期管理
• 经验驱动                            • 数据驱动
• 事后补救                            • 设计时考虑
• 局部最优                            • 全局最优

性能工程生命周期：
┌─────────┐   ┌─────────┐   ┌─────────┐   ┌─────────┐   ┌─────────┐
│  设计    │ → │  实现    │ → │  测量    │ → │  分析    │ → │  优化    │ → ...
└─────────┘   └─────────┘   └─────────┘   └─────────┘   └─────────┘
     ↑                                                       │
     └───────────────────────────────────────────────────────┘
                         持续迭代
```

### 1.2 Amdahl定律

```
加速比公式：
                    1
S = ─────────────────────────────────
    (1 - P) + P/S_p

其中：
• P = 可并行化部分的比例
• S_p = 并行部分的加速比

示例：
程序总时间 = 100s
串行部分 = 20s (必须顺序执行)
可并行部分 = 80s

极限加速比（无限处理器）：
         1            1
S = ───────── = ─────────── = 5x
    0.2 + 0      0.2

结论：
• 串行部分决定了加速上限
• 优化收益递减
• 优先优化热点（80/20法则）
```

---

## 2. 性能测量

### 2.1 测量方法论

```
┌─────────────────────────────────────────────────────────────┐
│                    性能测量金字塔                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│                    ┌─────────┐                              │
│                    │  业务指标 │  (延迟、吞吐量、错误率)       │
│                    └────┬────┘                              │
│                   ┌─────┴─────┐                             │
│                  ┌┴───────────┴┐                            │
│                  │   系统指标   │  (CPU、内存、I/O、网络)     │
│                  └──────┬──────┘                            │
│              ┌─────────┼─────────┐                          │
│           ┌──┴──┐   ┌──┴──┐   ┌──┴──┐                       │
│           │微架构│   │代码级│   │调用链│  (PMU、Profiler)     │
│           └─────┘   └─────┘   └─────┘                       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 计时方法

```c
#include <time.h>
#include <stdint.h>

// ========== 高精度计时 ==========

// 方法1: clock_gettime (推荐)
int64_t nanoseconds() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

// 方法2: RDTSC (x86-64，需小心CPU频率变化)
static inline uint64_t rdtsc() {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

// 方法3: RDTSCP (序列化，更准确)
static inline uint64_t rdtscp() {
    unsigned int aux;
    return __builtin_ia32_rdtscp(&aux);
}

// ========== 统计测量 ==========

typedef struct {
    double *samples;
    size_t count;
    size_t capacity;
} timing_stats_t;

void timing_stats_add(timing_stats_t *stats, double sample) {
    if (stats->count < stats->capacity) {
        stats->samples[stats->count++] = sample;
    }
}

double timing_stats_median(timing_stats_t *stats) {
    // 排序后取中位数
    qsort(stats->samples, stats->count, sizeof(double), compare_double);
    return stats->samples[stats->count / 2];
}

double timing_stats_percentile(timing_stats_t *stats, double p) {
    size_t idx = (size_t)(p * stats->count);
    return stats->samples[idx];
}

// ========== 消除测量噪声 ==========

// 1. 预热缓存
void warmup(void (*fn)(void)) {
    for (int i = 0; i < 10; i++) {
        fn();
    }
}

// 2. 多次测量取平均
#define N_ITERATIONS 1000

double benchmark(void (*fn)(void)) {
    warmup(fn);

    int64_t start = nanoseconds();
    for (int i = 0; i < N_ITERATIONS; i++) {
        fn();
    }
    int64_t end = nanoseconds();

    return (double)(end - start) / N_ITERATIONS;
}

// 3. 使用性能计数器（Linux perf_event_open）
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <sys/syscall.h>
#include <unistd.h>

int setup_perf_counter(int type, int config) {
    struct perf_event_attr pe = {
        .type = type,
        .size = sizeof(pe),
        .config = config,
        .disabled = 1,
        .exclude_kernel = 1,
        .exclude_hv = 1
    };

    return syscall(__NR_perf_event_open, &pe, 0, -1, -1, 0);
}

// 使用：
// int fd = setup_perf_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES);
// ioctl(fd, PERF_EVENT_IOC_RESET, 0);
// ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
// ... 代码 ...
// ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
// read(fd, &count, sizeof(count));
```

---

## 3. 性能分析工具

### 3.1 采样分析器

```bash
# ========== perf (Linux) ==========

# CPU采样
perf record -g ./my_program
perf report                    # 交互式查看
perf annotate                  # 源码级分析

# 特定事件采样
perf record -e cache-misses -g ./my_program

# 火焰图生成
perf record -g ./my_program
perf script | ./stackcollapse-perf.pl | ./flamegraph.pl > out.svg

# 系统级分析
perf top                       # 实时热点
perf stat ./my_program         # 统计计数器

# ========== Intel VTune ==========
# 微架构分析：
# - 前端瓶颈
# - 后端瓶颈
# - 内存访问
# - 向量化效率

# ========== AMD uProf ==========

# ========== Apple Instruments ==========
```

### 3.2 插桩分析

```c
// 手动插桩
#define TRACE_BEGIN(name) \
    int64_t _trace_start_##name = nanoseconds();

#define TRACE_END(name) \
    do { \
        int64_t _trace_elapsed = nanoseconds() - _trace_start_##name; \
        trace_record(#name, _trace_elapsed); \
    } while(0)

void my_function() {
    TRACE_BEGIN(my_function);

    TRACE_BEGIN(part1);
    do_part1();
    TRACE_END(part1);

    TRACE_BEGIN(part2);
    do_part2();
    TRACE_END(part2);

    TRACE_END(my_function);
}

// 使用gcc -finstrument-functions自动插桩
void __cyg_profile_func_enter(void *func, void *caller) {
    trace_function_enter(func, nanoseconds());
}

void __cyg_profile_func_exit(void *func, void *caller) {
    trace_function_exit(func, nanoseconds());
}
```

---

## 4. 微架构优化

### 4.1 CPU流水线

```
超标量流水线（4-wide）：
周期  IF  ID  EX  MEM  WB
  1   I0  -   -   -    -
  2   I1  I0  -   -    -
  3   I2  I1  I0  -    -
  4   I3  I2  I1  I0   -
  5   I4  I3  I2  I1   I0
  6   I5  I4  I3  I2   I1

IF: 取指  ID: 解码  EX: 执行  MEM: 访存  WB: 写回

流水线停顿（Stall）：
┌─────────────────┬─────────────────────────────┐
│     类型        │          原因               │
├─────────────────┼─────────────────────────────┤
│ 数据依赖(RAW)   │ 指令需要前一条指令的结果     │
│ 结构冒险        │ 功能单元冲突                │
│ 控制冒险        │ 分支预测失败                │
│ 缓存未命中      │ 数据不在缓存中              │
└─────────────────┴─────────────────────────────┘

优化策略：
• 指令重排：将独立指令插入延迟槽
• 循环展开：减少分支，增加ILP
• 软件流水线：循环内指令调度
```

### 4.2 内存层次优化

```c
// ========== 缓存优化 ==========

// 问题：缓存未命中
void slow_matrix_multiply(int n, double **a, double **b, double **c) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++)
                c[i][j] += a[i][k] * b[j][k];  // b的访问是跳跃的！
}

// 优化1: 循环交换（提高空间局部性）
void fast_matrix_multiply(int n, double **a, double **b, double **c) {
    for (int i = 0; i < n; i++)
        for (int k = 0; k < n; k++)
            for (int j = 0; j < n; j++)
                c[i][j] += a[i][k] * b[k][j];  // 连续访问
}

// 优化2: 分块（提高时间局部性）
#define BLOCK_SIZE 64

void blocked_matrix_multiply(int n, double **a, double **b, double **c) {
    for (int ii = 0; ii < n; ii += BLOCK_SIZE)
        for (int jj = 0; jj < n; jj += BLOCK_SIZE)
            for (int kk = 0; kk < n; kk += BLOCK_SIZE)
                // 块内计算
                for (int i = ii; i < min(ii + BLOCK_SIZE, n); i++)
                    for (int k = kk; k < min(kk + BLOCK_SIZE, n); k++)
                        for (int j = jj; j < min(jj + BLOCK_SIZE, n); j++)
                            c[i][j] += a[i][k] * b[k][j];
}

// 优化3: 预取
#include <xmmintrin.h>

void prefetch_loop(int *data, int n) {
    for (int i = 0; i < n; i++) {
        // 预取未来数据
        if (i + 16 < n) {
            _mm_prefetch((const char*)&data[i + 16], _MM_HINT_T0);
        }
        process(data[i]);
    }
}
```

### 4.3 向量化

```c
// ========== SIMD向量化 ==========

// 标量版本
void scalar_add(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

// AVX向量化版本（8 floats）
#include <immintrin.h>

void avx_add(float *a, float *b, float *c, int n) {
    int i = 0;

    // 主循环：每次处理8个float
    for (; i <= n - 8; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&c[i], vc);
    }

    // 处理尾部
    for (; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

// 编译器自动向量化提示
void auto_vectorized(float *restrict a,
                     float *restrict b,
                     float *restrict c,
                     int n) {
    #pragma GCC ivdep  // 忽略数据依赖警告
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}
```

---

## 5. Roofline模型

### 5.1 性能上限分析

```
Roofline模型：

性能 (GFLOP/s)
    │
P_max ├──────────────────────────────────────── 峰值算力
    │                        ╱
    │                       ╱
    │                      ╱
    │                     ╱
    │                    ╱ 内存带宽限制区
    │                   ╱
    │                  ╱
    │                 ╱
    │                ╱
    │               ╱
    │              ╱
    │             ╱
    │            ╱
    │           ╱
    │          ╱
    │         ╱
    │        ╱
    │       ╱
    │      ╱
    │     ╱
    │    ╱
    │   ╱
    │  ╱
    │ ╱
    │╱
    └────────────────────────────────────────►
    0        I_c                运算强度 (FLOP/byte)

运算强度 = 总FLOP / 总字节数

带宽限制性能 = 内存带宽 × 运算强度

计算限制性能 = 峰值算力

实际性能 = min(带宽限制性能, 计算限制性能)
```

---

## 关联导航

### 前置知识

- [性能优化基础](./03_Performance_Optimization.md)
- [SIMD向量化编程](../../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/04_SIMD_Vectorization.md)
- [缓存一致性协议](../../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/03_Cache_Coherency_Protocols.md)

### 后续延伸

- [并行算法优化](../../03_System_Technology_Domains/06_High_Performance_Computing/readme.md)
- [性能分析硬件](../../02_Formal_Semantics_and_Physics/16_Performance_Analysis_Hardware/readme.md)

### 参考

- MIT 6.172: Performance Engineering of Software Systems
- "What Every Programmer Should Know About Memory", Ulrich Drepper
- Intel 64 and IA-32 Architectures Optimization Reference Manual
