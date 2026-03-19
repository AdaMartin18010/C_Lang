# 微架构基准测试实战指南

## 目录

- [微架构基准测试实战指南](#微架构基准测试实战指南)
  - [目录](#目录)
  - [1. 微架构基准测试概述](#1-微架构基准测试概述)
    - [1.1 什么是微架构基准测试](#11-什么是微架构基准测试)
    - [1.2 微架构测试的核心指标](#12-微架构测试的核心指标)
    - [1.3 测试方法学](#13-测试方法学)
  - [2. Cache延迟测试](#2-cache延迟测试)
    - [2.1 Cache延迟测试原理](#21-cache延迟测试原理)
    - [2.2 Cache延迟测试代码](#22-cache延迟测试代码)
    - [2.3 编译和运行](#23-编译和运行)
    - [2.4 高级Cache测试：预取器分析](#24-高级cache测试预取器分析)
  - [3. 分支预测测试](#3-分支预测测试)
    - [3.1 分支预测测试原理](#31-分支预测测试原理)
    - [3.2 分支预测测试代码](#32-分支预测测试代码)
  - [4. TLB测试](#4-tlb测试)
    - [4.1 TLB测试原理](#41-tlb测试原理)
    - [4.2 TLB测试代码](#42-tlb测试代码)
  - [5. 内存带宽测试](#5-内存带宽测试)
    - [5.1 内存带宽测试原理](#51-内存带宽测试原理)
    - [5.2 内存带宽测试代码](#52-内存带宽测试代码)
  - [6. 自定义PMU驱动开发](#6-自定义pmu驱动开发)
    - [6.1 Linux内核PMU驱动框架](#61-linux内核pmu驱动框架)
    - [6.2 用户空间驱动程序](#62-用户空间驱动程序)
  - [7. 综合微架构分析工具](#7-综合微架构分析工具)
    - [7.1 完整微架构基准测试套件](#71-完整微架构基准测试套件)
    - [7.2 Makefile](#72-makefile)
  - [8. 总结与参考](#8-总结与参考)
    - [8.1 微架构基准测试要点](#81-微架构基准测试要点)
    - [8.2 参考资源](#82-参考资源)

---

## 1. 微架构基准测试概述

### 1.1 什么是微架构基准测试

微架构基准测试(Microarchitecture Benchmarking)是通过精确设计的测试程序，测量处理器微架构特性的方法。
与宏基准测试(Macro Benchmark)关注整体性能不同，微架构测试聚焦于特定硬件组件的行为特征。

```
┌─────────────────────────────────────────────────────────────────┐
│                    基准测试分类对比                             │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  宏基准测试 (Macro Benchmark)                                   │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │ • 关注整体系统性能                                        │ │
│  │ • 模拟真实工作负载                                        │ │
│  │ • 受多种因素影响                                          │ │
│  │ • 难以定位具体问题                                        │ │
│  │                                                           │ │
│  │ 示例: SPEC CPU, Geekbench, Cinebench                      │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
│  微架构基准测试 (Microarchitecture Benchmark)                   │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │ • 关注特定硬件组件                                        │ │
│  │ • 精确控制测试条件                                        │ │
│  │ • 测量特定性能指标                                        │ │
│  │ • 便于性能问题诊断                                        │ │
│  │                                                           │ │
│  │ 示例: lmbench, tinymembench, CoreMark-PRO                 │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 1.2 微架构测试的核心指标

```
┌─────────────────────────────────────────────────────────────────┐
│                    微架构测试核心指标                           │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  1. 延迟 (Latency)                                              │
│     ┌─────────────────────────────────────────────────────┐    │
│     │ • L1 Cache访问延迟                                  │    │
│     │ • L2 Cache访问延迟                                  │    │
│     │ • L3 Cache访问延迟                                  │    │
│     │ • 主内存访问延迟                                    │    │
│     │ • TLB未命中处理延迟                                 │    │
│     │ • 分支预测失败惩罚                                  │    │
│     └─────────────────────────────────────────────────────┘    │
│                                                                 │
│  2. 带宽 (Bandwidth)                                            │
│     ┌─────────────────────────────────────────────────────┐    │
│     │ • L1 Cache带宽                                      │    │
│     │ • L2 Cache带宽                                      │    │
│     │ • L3 Cache带宽                                      │    │
│     │ • 主内存带宽                                        │    │
│     │ • 内存总线带宽                                      │    │
│     │ • 预取器效率                                        │    │
│     └─────────────────────────────────────────────────────┘    │
│                                                                 │
│  3. 吞吐率 (Throughput)                                         │
│     ┌─────────────────────────────────────────────────────┐    │
│     │ • 整数运算吞吐率                                    │    │
│     │ • 浮点运算吞吐率                                    │    │
│     │ • 向量运算吞吐率                                    │    │
│     │ • 内存操作吞吐率                                    │    │
│     └─────────────────────────────────────────────────────┘    │
│                                                                 │
│  4. 预测准确率 (Accuracy)                                       │
│     ┌─────────────────────────────────────────────────────┐    │
│     │ • 分支预测准确率                                    │    │
│     │ • 返回栈缓冲区命中率                                │    │
│     │ • 间接分支预测准确率                                │    │
│     │ • 预取器命中率                                      │    │
│     └─────────────────────────────────────────────────────┘    │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 1.3 测试方法学

```
┌─────────────────────────────────────────────────────────────────┐
│                    微架构测试方法学                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  1. 消除干扰因素                                                │
│     ┌─────────────────────────────────────────────────────┐    │
│     │ • 禁用CPU频率调节 (固定频率)                        │    │
│     │ • 禁用超线程/SMT                                    │    │
│     │ • 绑定到特定CPU核心                                 │    │
│     │ • 禁用地址空间随机化 (ASLR)                         │    │
│     │ • 清空缓存/TLB (冷缓存测试)                         │    │
│     │ • 预热缓存 (热缓存测试)                             │    │
│     └─────────────────────────────────────────────────────┘    │
│                                                                 │
│  2. 测量技术                                                    │
│     ┌─────────────────────────────────────────────────────┐    │
│     │ • RDTSC/RDTSCP指令 (周期级精度)                     │    │
│     │ • 性能监控计数器 (PMC)                              │    │
│     │ • 多次测量取中位数 (消除异常值)                     │    │
│     │ • 最小化测量开销                                    │    │
│     │ • 循环展开减少循环开销                              │    │
│     └─────────────────────────────────────────────────────┘    │
│                                                                 │
│  3. 数据验证                                                    │
│     ┌─────────────────────────────────────────────────────┐    │
│     │ • 与已知参考值对比                                  │    │
│     │ • 不同方法交叉验证                                  │    │
│     │ • 理论极限对比                                      │    │
│     │ • 统计显著性检验                                    │    │
│     └─────────────────────────────────────────────────────┘    │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 2. Cache延迟测试

### 2.1 Cache延迟测试原理

```
┌─────────────────────────────────────────────────────────────────┐
│                    Cache延迟测试原理                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  方法: 指针追踪 (Pointer Chasing)                               │
│                                                                 │
│  原理: 创建一个随机链接的指针链表，强制串行访问                 │
│                                                                 │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                                                           │ │
│  │   内存布局:                                               │ │
│  │   ┌─────────┐    ┌─────────┐    ┌─────────┐              │ │
│  │   │ Node A  │───►│ Node C  │───►│ Node E  │───► ...       │ │
│  │   │ [data]  │    │ [data]  │    │ [data]  │                │ │
│  │   │ [next] ─┘    │ [next] ─┘    │ [next] ─┘               │ │
│  │   └─────────┘    └─────────┘    └─────────┘              │ │
│  │        ▲                                    ┌─────────┐   │ │
│  │        └────────────────────────────────────│ Node B  │   │ │
│  │                                             │ [data]  │   │ │
│  │                                             │ [next] ─┘   │ │
│  │                                             └─────────┘   │ │
│  │                                                           │ │
│  │   访问模式: 完全串行，无法并行化或预取                    │ │
│  │   latency = 总时间 / 访问次数                             │ │
│  │                                                           │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
│  优点:                                                          │
│  • 完全串行，避免并行执行干扰                                   │
│  • 随机访问，消除预取器影响                                     │
│  • 精确的单个访问时间测量                                       │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 2.2 Cache延迟测试代码

```c
/* cache_latency_test.c - Cache延迟测试 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>

/* RDTSC指令封装 */
static inline uint64_t rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

static inline uint64_t rdtscp(uint32_t *aux) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtscp" : "=a" (lo), "=d" (hi), "=c" (*aux));
    return ((uint64_t)hi << 32) | lo;
}

/* CPUID序列化 */
static inline void cpuid(void) {
    __asm__ __volatile__ ("cpuid" ::: "eax", "ebx", "ecx", "edx");
}

/* 内存屏障 */
static inline void mfence(void) {
    __asm__ __volatile__ ("mfence" ::: "memory");
}

/* 链表节点 */
struct node {
    struct node *next;
    uint64_t padding[7];  /* 64字节对齐，填充到缓存行大小 */
};

/* 创建随机链表 */
struct node* create_random_list(size_t num_nodes) {
    struct node *nodes = aligned_alloc(64, num_nodes * sizeof(struct node));
    if (!nodes) {
        perror("aligned_alloc");
        return NULL;
    }

    /* 初始化next为NULL */
    for (size_t i = 0; i < num_nodes; i++) {
        nodes[i].next = NULL;
    }

    /* 创建索引数组并随机打乱 */
    size_t *indices = malloc(num_nodes * sizeof(size_t));
    for (size_t i = 0; i < num_nodes; i++) {
        indices[i] = i;
    }

    /* Fisher-Yates洗牌 */
    for (size_t i = num_nodes - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);
        size_t temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    /* 创建随机链表 */
    for (size_t i = 0; i < num_nodes - 1; i++) {
        nodes[indices[i]].next = &nodes[indices[i + 1]];
    }
    nodes[indices[num_nodes - 1]].next = &nodes[indices[0]];  /* 循环 */

    free(indices);
    return nodes;
}

/* 测量链表遍历延迟 */
uint64_t measure_latency(struct node *head, size_t iterations) {
    struct node *p = head;
    uint32_t aux;

    /* 预热 */
    for (size_t i = 0; i < 1000; i++) {
        p = p->next;
    }

    /* 序列化 */
    cpuid();
    mfence();

    uint64_t start = rdtscp(&aux);

    /* 遍历链表 - 关键循环 */
    for (size_t i = 0; i < iterations; i++) {
        p = p->next;  /* 依赖链，无法并行 */
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;
        p = p->next;  /* 16次展开 */
    }

    uint64_t end = rdtscp(&aux);

    /* 防止优化 */
    volatile void *dummy = p;
    (void)dummy;

    return (end - start) / (iterations * 16);
}

/* 绑定到特定CPU */
void bind_to_cpu(int cpu) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);

    if (sched_setaffinity(0, sizeof(cpuset), &cpuset) != 0) {
        perror("sched_setaffinity");
    }
}

/* 获取CPU频率 */
double get_cpu_frequency(void) {
    /* 尝试从/proc读取 */
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "cpu MHz", 7) == 0) {
                double freq;
                sscanf(line, "cpu MHz : %lf", &freq);
                fclose(fp);
                return freq * 1e6;
            }
        }
        fclose(fp);
    }

    /* 默认假设3GHz */
    return 3.0e9;
}

/* 清空缓存 */
void flush_cache(void) {
    static size_t flush_size = 64 * 1024 * 1024;  /* 64MB */
    static char *flush_buffer = NULL;

    if (!flush_buffer) {
        flush_buffer = malloc(flush_size);
    }

    volatile char sum = 0;
    for (size_t i = 0; i < flush_size; i += 64) {
        sum += flush_buffer[i];
    }
    (void)sum;
}

/* 运行单次测试 */
void run_cache_test(const char *name, size_t size_kb, int is_cold) {
    size_t num_nodes = (size_kb * 1024) / sizeof(struct node);
    const size_t iterations = 1000000;

    struct node *list = create_random_list(num_nodes);
    if (!list) return;

    if (is_cold) {
        flush_cache();
    }

    uint64_t cycles = measure_latency(list, iterations);
    double freq = get_cpu_frequency();
    double nanoseconds = (cycles / freq) * 1e9;

    printf("%-20s %6zu KB  %s: %6.2f cycles (%6.2f ns)\n",
           name, size_kb, is_cold ? "Cold" : "Hot ",
           (double)cycles, nanoseconds);

    free(list);
}

int main(int argc, char *argv[]) {
    /* 设置随机种子 */
    srand(time(NULL));

    /* 绑定到CPU 0 */
    bind_to_cpu(0);

    printf("========================================\n");
    printf("      Cache延迟测试                     \n");
    printf("========================================\n\n");

    double freq_mhz = get_cpu_frequency() / 1e6;
    printf("CPU频率: %.0f MHz\n\n", freq_mhz);

    /* 热缓存测试 */
    printf("热缓存测试 (数据在Cache中):\n");
    printf("%-20s %10s  %6s: %10s (%10s)\n",
           "测试名称", "大小", "状态", "周期", "纳秒");
    printf("--------------------------------------------------------------------------------\n");

    run_cache_test("L1 Cache", 16, 0);
    run_cache_test("L1 Cache", 32, 0);
    run_cache_test("L2 Cache", 256, 0);
    run_cache_test("L2 Cache", 512, 0);
    run_cache_test("L3 Cache", 2048, 0);
    run_cache_test("L3 Cache", 4096, 0);
    run_cache_test("L3 Cache", 8192, 0);
    run_cache_test("Memory", 32768, 0);
    run_cache_test("Memory", 65536, 0);

    printf("\n");

    /* 冷缓存测试 */
    printf("冷缓存测试 (数据从内存加载):\n");
    printf("%-20s %10s  %6s: %10s (%10s)\n",
           "测试名称", "大小", "状态", "周期", "纳秒");
    printf("--------------------------------------------------------------------------------\n");

    run_cache_test("L1 Cache", 16, 1);
    run_cache_test("L1 Cache", 32, 1);
    run_cache_test("L2 Cache", 256, 1);
    run_cache_test("L2 Cache", 512, 1);
    run_cache_test("L3 Cache", 2048, 1);
    run_cache_test("L3 Cache", 4096, 1);
    run_cache_test("L3 Cache", 8192, 1);
    run_cache_test("Memory", 32768, 1);
    run_cache_test("Memory", 65536, 1);

    printf("\n========================================\n");
    printf("预期参考值:\n");
    printf("  L1 Cache:  3-5  周期 (约 1-2  ns)\n");
    printf("  L2 Cache:  10-15 周期 (约 3-5  ns)\n");
    printf("  L3 Cache:  30-50 周期 (约 10-15 ns)\n");
    printf("  Memory:    200-300 周期 (约 60-100 ns)\n");
    printf("========================================\n");

    return 0;
}
```

### 2.3 编译和运行

```bash
# 编译
gcc -O2 -o cache_latency_test cache_latency_test.c

# 运行 (需要root权限清除缓存)
sudo ./cache_latency_test

# 预期输出示例:
========================================
      Cache延迟测试
========================================

CPU频率: 3400 MHz

热缓存测试 (数据在Cache中):
测试名称                     大小    状态:       周期 (       纳秒)
--------------------------------------------------------------------------------
L1 Cache                    16 KB  Hot :   4.12 cycles (   1.21 ns)
L1 Cache                    32 KB  Hot :   4.15 cycles (   1.22 ns)
L2 Cache                   256 KB  Hot :  11.23 cycles (   3.30 ns)
L2 Cache                   512 KB  Hot :  11.45 cycles (   3.37 ns)
L3 Cache                  2048 KB  Hot :  38.67 cycles (  11.37 ns)
L3 Cache                  4096 KB  Hot :  39.12 cycles (  11.51 ns)
L3 Cache                  8192 KB  Hot :  41.56 cycles (  12.22 ns)
Memory                   32768 KB  Hot : 245.78 cycles (  72.29 ns)
Memory                   65536 KB  Hot : 248.34 cycles (  73.04 ns)

冷缓存测试 (数据从内存加载):
--------------------------------------------------------------------------------
L1 Cache                    16 KB  Cold:   4.89 cycles (   1.44 ns)
...
```

### 2.4 高级Cache测试：预取器分析

```c
/* prefetcher_test.c - 预取器效率测试 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

/* RDTSC */
static inline uint64_t rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

static inline uint64_t rdtscp(uint32_t *aux) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtscp" : "=a" (lo), "=d" (hi), "=c" (*aux));
    return ((uint64_t)hi << 32) | lo;
}

/* 测试1: 顺序访问 (预取友好) */
uint64_t test_sequential_access(int *arr, size_t n) {
    volatile int sum = 0;
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (size_t i = 0; i < n; i++) {
        sum += arr[i];
    }
    uint64_t end = rdtscp(&aux);

    (void)sum;
    return end - start;
}

/* 测试2: 步幅访问 (测试不同步幅的预取效果) */
uint64_t test_stride_access(int *arr, size_t n, int stride) {
    volatile int sum = 0;
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (size_t i = 0; i < n; i += stride) {
        sum += arr[i];
    }
    uint64_t end = rdtscp(&aux);

    (void)sum;
    return end - start;
}

/* 测试3: 随机访问 (预取不友好) */
uint64_t test_random_access(int *arr, size_t *indices, size_t n) {
    volatile int sum = 0;
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (size_t i = 0; i < n; i++) {
        sum += arr[indices[i]];
    }
    uint64_t end = rdtscp(&aux);

    (void)sum;
    return end - start;
}

/* 测试4: 双路访问 (测试并发预取) */
uint64_t test_dual_stream(int *arr1, int *arr2, size_t n) {
    volatile int sum = 0;
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (size_t i = 0; i < n; i++) {
        sum += arr1[i] + arr2[i];
    }
    uint64_t end = rdtscp(&aux);

    (void)sum;
    return end - start;
}

int main() {
    const size_t N = 100 * 1024 * 1024 / sizeof(int);  /* 100MB */

    printf("========================================\n");
    printf("      预取器效率测试                     \n");
    printf("========================================\n\n");

    /* 分配数组 */
    int *arr = aligned_alloc(64, N * sizeof(int));
    int *arr2 = aligned_alloc(64, N * sizeof(int));
    size_t *indices = malloc(N * sizeof(size_t));

    /* 初始化 */
    for (size_t i = 0; i < N; i++) {
        arr[i] = i;
        arr2[i] = i;
        indices[i] = i;
    }

    /* 打乱索引 */
    for (size_t i = N - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);
        size_t tmp = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }

    /* 测试 */
    printf("访问模式性能对比 (越低越好):\n\n");

    /* 顺序访问 */
    uint64_t seq_cycles = test_sequential_access(arr, N);
    double seq_time = seq_cycles / 3.0e9 * 1e9 / N;  /* ns per access */
    printf("顺序访问:     %10.2f cycles, %.3f ns/访问\n",
           (double)seq_cycles, seq_time);

    /* 步幅访问 */
    printf("\n步幅访问:\n");
    for (int stride = 1; stride <= 64; stride *= 2) {
        uint64_t stride_cycles = test_stride_access(arr, N, stride);
        double stride_time = stride_cycles / 3.0e9 * 1e9 / (N / stride);
        printf("  步幅 %2d:   %10.2f cycles, %.3f ns/访问\n",
               stride, (double)stride_cycles, stride_time);
    }

    /* 随机访问 */
    uint64_t rand_cycles = test_random_access(arr, indices, N);
    double rand_time = rand_cycles / 3.0e9 * 1e9 / N;
    printf("\n随机访问:     %10.2f cycles, %.3f ns/访问\n",
           (double)rand_cycles, rand_time);

    /* 双路访问 */
    uint64_t dual_cycles = test_dual_stream(arr, arr2, N);
    double dual_time = dual_cycles / 3.0e9 * 1e9 / N;
    printf("双路访问:     %10.2f cycles, %.3f ns/访问\n",
           (double)dual_cycles, dual_time);

    /* 计算加速比 */
    printf("\n========================================\n");
    printf("预取效果分析:\n");
    printf("  顺序 vs 随机: %.2fx 加速\n", rand_time / seq_time);
    printf("  双路效率: %.1f%%\n", 200.0 * seq_time / dual_time);
    printf("========================================\n");

    free(arr);
    free(arr2);
    free(indices);

    return 0;
}
```

---

## 3. 分支预测测试

### 3.1 分支预测测试原理

```
┌─────────────────────────────────────────────────────────────────┐
│                    分支预测测试原理                             │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  测试方法:                                                      │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                                                           │ │
│  │  1. 可预测模式测试                                        │ │
│  │     • 总是真/总是假                                       │ │
│  │     • 交替模式 (T,F,T,F...)                               │ │
│  │     • 简单循环模式                                        │ │
│  │                                                           │ │
│  │  2. 不可预测模式测试                                      │ │
│  │     • 随机数据                                            │ │
│  │     • 伪随机序列                                          │ │
│  │                                                           │ │
│  │  3. 混合模式测试                                          │ │
│  │     • 可预测段 + 不可预测段                               │ │
│  │     • 测试预测器适应速度                                  │ │
│  │                                                           │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
│  测量指标:                                                      │
│  • 分支预测失败率 (Branch Misprediction Rate)                   │
│  • 分支预测失败惩罚 (Misprediction Penalty)                     │
│  • 预测器状态机行为                                             │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 3.2 分支预测测试代码

```c
/* branch_prediction_test.c - 分支预测测试 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

/* RDTSC */
static inline uint64_t rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

static inline uint64_t rdtscp(uint32_t *aux) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtscp" : "=a" (lo), "=d" (hi), "=c" (*aux));
    return ((uint64_t)hi << 32) | lo;
}

#define ITERATIONS 100000000

/* 测试1: 总是为真的分支 (100%可预测) */
uint64_t test_always_true(void) {
    volatile int sum = 0;
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int i = 0; i < ITERATIONS; i++) {
        if (i >= 0) {  /* 总是真 */
            sum++;
        }
    }
    uint64_t end = rdtscp(&aux);

    (void)sum;
    return end - start;
}

/* 测试2: 总是为假的分支 (100%可预测) */
uint64_t test_always_false(void) {
    volatile int sum = 0;
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int i = 0; i < ITERATIONS; i++) {
        if (i < 0) {  /* 总是假 */
            sum++;
        }
    }
    uint64_t end = rdtscp(&aux);

    (void)sum;
    return end - start;
}

/* 测试3: 严格交替 (对2-bit预测器最难) */
uint64_t test_alternating(void) {
    volatile int sum = 0;
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int i = 0; i < ITERATIONS; i++) {
        if (i & 1) {  /* 交替: F,T,F,T... */
            sum++;
        }
    }
    uint64_t end = rdtscp(&aux);

    (void)sum;
    return end - start;
}

/* 测试4: 重复模式 (T,T,T,T,F,F,F,F) */
uint64_t test_repeating_pattern(void) {
    volatile int sum = 0;
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int i = 0; i < ITERATIONS; i++) {
        if ((i % 8) < 4) {  /* T,T,T,T,F,F,F,F */
            sum++;
        }
    }
    uint64_t end = rdtscp(&aux);

    (void)sum;
    return end - start;
}

/* 测试5: 随机分支 (不可预测) */
uint64_t test_random(void) {
    volatile int sum = 0;
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int i = 0; i < ITERATIONS; i++) {
        if (rand() % 2) {  /* 随机 */
            sum++;
        }
    }
    uint64_t end = rdtscp(&aux);

    (void)sum;
    return end - start;
}

/* 测试6: 数据相关分支 - 已排序数据 */
uint64_t test_sorted_data(int *data) {
    volatile int sum = 0;
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int i = 0; i < ITERATIONS; i++) {
        if (data[i] > 100) {
            sum++;
        }
    }
    uint64_t end = rdtscp(&aux);

    (void)sum;
    return end - start;
}

/* 测试7: 数据相关分支 - 随机数据 */
uint64_t test_random_data(int *data) {
    volatile int sum = 0;
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int i = 0; i < ITERATIONS; i++) {
        if (data[i] > 100) {
            sum++;
        }
    }
    uint64_t end = rdtscp(&aux);

    (void)sum;
    return end - start;
}

/* 测试8: 间接跳转测试 */
typedef int (*func_ptr_t)(int);

int func_a(int x) { return x + 1; }
int func_b(int x) { return x - 1; }
int func_c(int x) { return x * 2; }
int func_d(int x) { return x / 2; }

uint64_t test_indirect_jump(int *pattern, int pattern_len) {
    func_ptr_t funcs[] = {func_a, func_b, func_c, func_d};
    volatile int sum = 0;
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int i = 0; i < ITERATIONS; i++) {
        int idx = pattern[i % pattern_len];
        sum = funcs[idx](sum);
    }
    uint64_t end = rdtscp(&aux);

    (void)sum;
    return end - start;
}

/* 测试9: 无分支版本 (作为基准) */
uint64_t test_branchless(void) {
    volatile int sum = 0;
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int i = 0; i < ITERATIONS; i++) {
        sum += (i & 1);  /* 等价于 if(i&1) sum++，但无分支 */
    }
    uint64_t end = rdtscp(&aux);

    (void)sum;
    return end - start;
}

int main() {
    printf("========================================\n");
    printf("      分支预测测试                       \n");
    printf("========================================\n\n");

    /* 准备数据 */
    int *sorted_data = malloc(ITERATIONS * sizeof(int));
    int *random_data = malloc(ITERATIONS * sizeof(int));

    for (int i = 0; i < ITERATIONS; i++) {
        sorted_data[i] = i % 200;
        random_data[i] = rand() % 200;
    }

    printf("测试配置: %d 次迭代\n\n", ITERATIONS);
    printf("%-30s %15s %15s\n", "测试名称", "周期数", "相对开销");
    printf("--------------------------------------------------------------------------------\n");

    /* 获取基准 (无分支) */
    uint64_t baseline = test_branchless();
    printf("%-30s %15" PRIu64 " %15.2fx\n", "基准 (无分支)", baseline, 1.0);

    /* 可预测模式测试 */
    uint64_t cycles;

    cycles = test_always_true();
    printf("%-30s %15" PRIu64 " %15.2fx\n", "总是为真", cycles, (double)cycles / baseline);

    cycles = test_always_false();
    printf("%-30s %15" PRIu64 " %15.2fx\n", "总是为假", cycles, (double)cycles / baseline);

    cycles = test_repeating_pattern();
    printf("%-30s %15" PRIu64 " %15.2fx\n", "重复模式(TTTTFFFF)", cycles, (double)cycles / baseline);

    cycles = test_alternating();
    printf("%-30s %15" PRIu64 " %15.2fx\n", "严格交替(FTFT)", cycles, (double)cycles / baseline);

    /* 不可预测测试 */
    cycles = test_random();
    printf("%-30s %15" PRIu64 " %15.2fx\n", "随机分支", cycles, (double)cycles / baseline);

    /* 数据相关测试 */
    cycles = test_sorted_data(sorted_data);
    printf("%-30s %15" PRIu64 " %15.2fx\n", "排序数据", cycles, (double)cycles / baseline);

    cycles = test_random_data(random_data);
    printf("%-30s %15" PRIu64 " %15.2fx\n", "随机数据", cycles, (double)cycles / baseline);

    /* 间接跳转测试 */
    int sequential_pattern[] = {0, 1, 2, 3};
    int random_pattern[16];
    for (int i = 0; i < 16; i++) random_pattern[i] = rand() % 4;

    cycles = test_indirect_jump(sequential_pattern, 4);
    printf("%-30s %15" PRIu64 " %15.2fx\n", "间接跳转(顺序)", cycles, (double)cycles / baseline);

    cycles = test_indirect_jump(random_pattern, 16);
    printf("%-30s %15" PRIu64 " %15.2fx\n", "间接跳转(随机)", cycles, (double)cycles / baseline);

    printf("\n========================================\n");
    printf("分析:\n");
    printf("  • 相对开销接近1.0表示分支预测完美\n");
    printf("  • 开销>1.5表示明显预测失败\n");
    printf("  • 排序数据 vs 随机数据对比显示数据布局影响\n");
    printf("========================================\n");

    free(sorted_data);
    free(random_data);

    return 0;
}
```

---

## 4. TLB测试

### 4.1 TLB测试原理

```
┌─────────────────────────────────────────────────────────────────┐
│                    TLB测试原理                                  │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  TLB (Translation Lookaside Buffer) 是页表缓存                  │
│                                                                 │
│  测试方法:                                                      │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                                                           │ │
│  │  1. 计算覆盖N个TLB条目所需的内存大小                      │ │
│  │     • 4KB页: N * 4KB                                    │ │
│  │     • 2MB大页: N * 2MB                                  │ │
│  │                                                           │ │
│  │  2. 创建内存区域并随机访问                                │ │
│  │     • 小于TLB容量: 全部命中                             │ │
│  │     • 大于TLB容量: 触发TLB未命中                        │ │
│  │                                                           │ │
│  │  3. 测量访问延迟变化                                      │ │
│  │     • 延迟突增点即为TLB容量边界                         │ │
│  │                                                           │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 4.2 TLB测试代码

```c
/* tlb_test.c - TLB容量和延迟测试 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>

/* RDTSC */
static inline uint64_t rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

static inline uint64_t rdtscp(uint32_t *aux) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtscp" : "=a" (lo), "=d" (hi), "=c" (*aux));
    return ((uint64_t)hi << 32) | lo;
}

#define PAGE_SIZE 4096
#define NUM_PAGES 8192  /* 测试32MB */
#define ITERATIONS 1000000

/* 测试TLB访问延迟 */
void test_tlb_latency(void) {
    printf("========================================\n");
    printf("      TLB容量测试                       \n");
    printf("========================================\n\n");

    /* 分配页数组 */
    char **pages = malloc(NUM_PAGES * sizeof(char*));

    /* 分配内存页 */
    for (int i = 0; i < NUM_PAGES; i++) {
        pages[i] = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (pages[i] == MAP_FAILED) {
            perror("mmap");
            return;
        }
        /* 写入数据，确保页被分配 */
        pages[i][0] = i;
    }

    /* 创建不同TLB覆盖范围的测试 */
    int test_sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
    int num_tests = sizeof(test_sizes) / sizeof(test_sizes[0]);

    printf("页数      覆盖大小      访问延迟(cycles)      注释\n");
    printf("----------------------------------------------------------------\n");

    for (int t = 0; t < num_tests; t++) {
        int num_pages = test_sizes[t];

        /* 创建随机访问序列 */
        int *sequence = malloc(num_pages * sizeof(int));
        for (int i = 0; i < num_pages; i++) {
            sequence[i] = i;
        }

        /* 打乱顺序 */
        for (int i = num_pages - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            int tmp = sequence[i];
            sequence[i] = sequence[j];
            sequence[j] = tmp;
        }

        /* 预热TLB */
        for (int i = 0; i < num_pages; i++) {
            volatile char c = pages[sequence[i]][0];
            (void)c;
        }

        /* 测量访问时间 */
        uint32_t aux;
        uint64_t start = rdtscp(&aux);

        volatile char sum = 0;
        for (int iter = 0; iter < ITERATIONS; iter++) {
            for (int i = 0; i < num_pages; i++) {
                sum += pages[sequence[i]][0];
            }
        }

        uint64_t end = rdtscp(&aux);
        (void)sum;

        double avg_cycles = (double)(end - start) / (ITERATIONS * num_pages);

        /* 判断TLB状态 */
        const char *note = "";
        if (avg_cycles < 10) {
            note = "(TLB命中)";
        } else if (avg_cycles < 50) {
            note = "(L1 TLB未命中)";
        } else {
            note = "(L2 TLB/页表遍历)";
        }

        printf("%-9d %-13s %18.2f      %s\n",
               num_pages,
               num_pages <= 4 ? "<64KB" :
               num_pages <= 64 ? "<256KB" :
               num_pages <= 512 ? "<2MB" :
               num_pages <= 2048 ? "<8MB" : ">32MB",
               avg_cycles,
               note);

        free(sequence);
    }

    /* 清理 */
    for (int i = 0; i < NUM_PAGES; i++) {
        munmap(pages[i], PAGE_SIZE);
    }
    free(pages);

    printf("\n========================================\n");
    printf("典型TLB配置:\n");
    printf("  L1 DTLB: 64-entries (覆盖256KB with 4KB pages)\n");
    printf("  L2 TLB:  1024-entries (覆盖4MB with 4KB pages)\n");
    printf("  使用2MB大页可显著增加覆盖范围\n");
    printf("========================================\n");
}

/* 大页TLB测试 */
void test_hugepage_tlb(void) {
    printf("\n========================================\n");
    printf("      大页TLB测试                       \n");
    printf("========================================\n\n");

    /* 尝试分配2MB大页 */
    size_t hugepage_size = 2 * 1024 * 1024;  /* 2MB */
    int num_hugepages = 32;  /* 64MB */

    char **hugepages = malloc(num_hugepages * sizeof(char*));

    printf("分配 %d x 2MB 大页...\n", num_hugepages);

    for (int i = 0; i < num_hugepages; i++) {
        hugepages[i] = mmap(NULL, hugepage_size, PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                           -1, 0);
        if (hugepages[i] == MAP_FAILED) {
            printf("大页分配失败 (可能需要配置hugetlbfs)\n");
            free(hugepages);
            return;
        }
        hugepages[i][0] = i;
    }

    printf("大页分配成功!\n\n");

    /* 测试大页TLB访问 */
    int test_sizes[] = {4, 8, 16, 32};

    printf("大页数    覆盖大小      访问延迟(cycles)\n");
    printf("----------------------------------------------------------------\n");

    for (int t = 0; t < sizeof(test_sizes)/sizeof(test_sizes[0]); t++) {
        int num = test_sizes[t];

        /* 随机顺序 */
        int *seq = malloc(num * sizeof(int));
        for (int i = 0; i < num; i++) seq[i] = i;
        for (int i = num-1; i > 0; i--) {
            int j = rand() % (i+1);
            int tmp = seq[i]; seq[i] = seq[j]; seq[j] = tmp;
        }

        /* 测量 */
        uint32_t aux;
        volatile char sum = 0;

        uint64_t start = rdtscp(&aux);
        for (int iter = 0; iter < 100000; iter++) {
            for (int i = 0; i < num; i++) {
                sum += hugepages[seq[i]][0];
            }
        }
        uint64_t end = rdtscp(&aux);
        (void)sum;

        double avg = (double)(end - start) / (100000 * num);

        char size_str[32];
        snprintf(size_str, sizeof(size_str), "%dMB", num * 2);

        printf("%-9d %-13s %18.2f\n", num, size_str, avg);

        free(seq);
    }

    /* 清理 */
    for (int i = 0; i < num_hugepages; i++) {
        munmap(hugepages[i], hugepage_size);
    }
    free(hugepages);
}

int main() {
    srand(time(NULL));

    test_tlb_latency();
    test_hugepage_tlb();

    return 0;
}
```

---

## 5. 内存带宽测试

### 5.1 内存带宽测试原理

```
┌─────────────────────────────────────────────────────────────────┐
│                    内存带宽测试原理                             │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  目标: 测量内存子系统的最大数据传输速率                          │
│                                                                 │
│  方法:                                                          │
│  ┌───────────────────────────────────────────────────────────┐ │
│  │                                                           │ │
│  │  1. 顺序读写测试                                          │ │
│  │     • 大块连续内存访问                                    │ │
│  │     • 最大化预取器和突发传输效率                          │ │
│  │     • 使用SIMD指令提高带宽                                │ │
│  │                                                           │ │
│  │  2. 随机访问测试                                          │ │
│  │     • 测量实际应用中的有效带宽                            │ │
│  │     • 反映TLB和缓存影响                                   │ │
│  │                                                           │ │
│  │  3. 多线程测试                                            │ │
│  │     • 测试内存控制器并发能力                              │ │
│  │     • 测量NUMA影响                                        │ │
│  │                                                           │ │
│  └───────────────────────────────────────────────────────────┘ │
│                                                                 │
│  计算公式: 带宽 = 传输数据量 / 耗时                             │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 5.2 内存带宽测试代码

```c
/* memory_bandwidth_test.c - 内存带宽测试 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <emmintrin.h>  /* SSE2 */
#include <immintrin.h>  /* AVX */

/* RDTSC */
static inline uint64_t rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

static inline uint64_t rdtscp(uint32_t *aux) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtscp" : "=a" (lo), "=d" (hi), "=c" (*aux));
    return ((uint64_t)hi << 32) | lo;
}

#define TEST_SIZE (1024 * 1024 * 256)  /* 256MB */
#define ITERATIONS 10

/* 标量读取带宽测试 */
double test_scalar_read(char *src, size_t size) {
    volatile uint64_t sum = 0;
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int iter = 0; iter < ITERATIONS; iter++) {
        uint64_t local_sum = 0;
        for (size_t i = 0; i < size; i += 8) {
            local_sum += *(uint64_t*)(src + i);
        }
        sum += local_sum;
    }
    uint64_t end = rdtscp(&aux);
    (void)sum;

    double seconds = (end - start) / 3.0e9;
    double gb_per_sec = (double)(size * ITERATIONS) / (1024 * 1024 * 1024) / seconds;
    return gb_per_sec;
}

/* 标量写入带宽测试 */
double test_scalar_write(char *dst, size_t size) {
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (size_t i = 0; i < size; i += 8) {
            *(uint64_t*)(dst + i) = iter;
        }
    }
    uint64_t end = rdtscp(&aux);

    double seconds = (end - start) / 3.0e9;
    double gb_per_sec = (double)(size * ITERATIONS) / (1024 * 1024 * 1024) / seconds;
    return gb_per_sec;
}

/* 标量复制带宽测试 */
double test_scalar_copy(char *dst, char *src, size_t size) {
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int iter = 0; iter < ITERATIONS; iter++) {
        memcpy(dst, src, size);
    }
    uint64_t end = rdtscp(&aux);

    double seconds = (end - start) / 3.0e9;
    /* 复制涉及读取+写入，所以数据量要×2 */
    double gb_per_sec = (double)(size * 2 * ITERATIONS) / (1024 * 1024 * 1024) / seconds;
    return gb_per_sec;
}

/* SSE读取带宽测试 */
double test_sse_read(char *src, size_t size) {
    volatile __m128i sum = _mm_setzero_si128();
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (size_t i = 0; i < size; i += 64) {
            __m128i r0 = _mm_load_si128((__m128i*)(src + i));
            __m128i r1 = _mm_load_si128((__m128i*)(src + i + 16));
            __m128i r2 = _mm_load_si128((__m128i*)(src + i + 32));
            __m128i r3 = _mm_load_si128((__m128i*)(src + i + 48));
            sum = _mm_add_epi64(sum, r0);
            sum = _mm_add_epi64(sum, r1);
            sum = _mm_add_epi64(sum, r2);
            sum = _mm_add_epi64(sum, r3);
        }
    }
    uint64_t end = rdtscp(&aux);
    (void)sum;

    double seconds = (end - start) / 3.0e9;
    double gb_per_sec = (double)(size * ITERATIONS) / (1024 * 1024 * 1024) / seconds;
    return gb_per_sec;
}

/* AVX读取带宽测试 */
double test_avx_read(char *src, size_t size) {
    volatile __m256i sum = _mm256_setzero_si256();
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (size_t i = 0; i < size; i += 64) {
            __m256i r0 = _mm256_load_si256((__m256i*)(src + i));
            __m256i r1 = _mm256_load_si256((__m256i*)(src + i + 32));
            sum = _mm256_add_epi64(sum, r0);
            sum = _mm256_add_epi64(sum, r1);
        }
    }
    uint64_t end = rdtscp(&aux);
    (void)sum;

    double seconds = (end - start) / 3.0e9;
    double gb_per_sec = (double)(size * ITERATIONS) / (1024 * 1024 * 1024) / seconds;
    return gb_per_sec;
}

/* 内存填充测试 (memset) */
double test_memset(char *dst, size_t size) {
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int iter = 0; iter < ITERATIONS; iter++) {
        memset(dst, iter & 0xFF, size);
    }
    uint64_t end = rdtscp(&aux);

    double seconds = (end - start) / 3.0e9;
    double gb_per_sec = (double)(size * ITERATIONS) / (1024 * 1024 * 1024) / seconds;
    return gb_per_sec;
}

/* 随机访问带宽测试 */
double test_random_read(char *src, size_t *indices, size_t num_indices) {
    volatile uint64_t sum = 0;
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (size_t i = 0; i < num_indices; i++) {
            sum += *(uint64_t*)(src + indices[i]);
        }
    }
    uint64_t end = rdtscp(&aux);
    (void)sum;

    double seconds = (end - start) / 3.0e9;
    double total_bytes = num_indices * sizeof(uint64_t) * ITERATIONS;
    double gb_per_sec = total_bytes / (1024 * 1024 * 1024) / seconds;
    return gb_per_sec;
}

int main() {
    printf("========================================\n");
    printf("      内存带宽测试                       \n");
    printf("========================================\n\n");
    printf("测试数据大小: %d MB\n\n", TEST_SIZE / (1024 * 1024));

    /* 分配对齐内存 */
    char *src = aligned_alloc(64, TEST_SIZE);
    char *dst = aligned_alloc(64, TEST_SIZE);

    /* 初始化 */
    memset(src, 0xAA, TEST_SIZE);

    /* 准备随机访问索引 */
    size_t num_indices = TEST_SIZE / 64;
    size_t *indices = malloc(num_indices * sizeof(size_t));
    for (size_t i = 0; i < num_indices; i++) {
        indices[i] = (rand() % num_indices) * 64;
    }

    printf("%-30s %15s\n", "测试项目", "带宽 (GB/s)");
    printf("--------------------------------------------------------------------------------\n");

    /* 顺序访问测试 */
    printf("\n【顺序访问测试】\n");
    printf("%-30s %15.2f\n", "标量读取", test_scalar_read(src, TEST_SIZE));
    printf("%-30s %15.2f\n", "标量写入", test_scalar_write(dst, TEST_SIZE));
    printf("%-30s %15.2f\n", "标量复制 (memcpy)", test_scalar_copy(dst, src, TEST_SIZE));
    printf("%-30s %15.2f\n", "SSE 128-bit读取", test_sse_read(src, TEST_SIZE));
    printf("%-30s %15.2f\n", "AVX 256-bit读取", test_avx_read(src, TEST_SIZE));
    printf("%-30s %15.2f\n", "memset", test_memset(dst, TEST_SIZE));

    /* 随机访问测试 */
    printf("\n【随机访问测试 (缓存行级别)】\n");
    printf("%-30s %15.2f\n", "随机读取 (64B步幅)", test_random_read(src, indices, num_indices));

    printf("\n========================================\n");
    printf("参考值:\n");
    printf("  DDR4-3200双通道理论带宽: ~50 GB/s\n");
    printf("  实际顺序读取带宽: 40-45 GB/s (80-90%%效率)\n");
    printf("  随机读取带宽: 通常 <5 GB/s (受延迟限制)\n");
    printf("========================================\n");

    free(src);
    free(dst);
    free(indices);

    return 0;
}
```

---

## 6. 自定义PMU驱动开发

### 6.1 Linux内核PMU驱动框架

```c
/* my_pmu_driver.c - 简单PMU驱动示例 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <asm/msr.h>

#define DEVICE_NAME "mypmu"
#define CLASS_NAME "mypmu_class"

/* PMU设备结构体 */
struct my_pmu_device {
    struct perf_event_attr attr;
    struct perf_event *event;
    int active;
};

static struct class *mypmu_class = NULL;
static struct device *mypmu_device = NULL;
static dev_t dev_num;
static struct cdev mypmu_cdev;
static struct my_pmu_device pmu_dev;

/* 配置PMU事件 */
static int mypmu_config_event(struct my_pmu_device *dev, u64 config) {
    memset(&dev->attr, 0, sizeof(struct perf_event_attr));
    dev->attr.type = PERF_TYPE_HARDWARE;
    dev->attr.size = sizeof(struct perf_event_attr);
    dev->attr.config = config;
    dev->attr.disabled = 1;
    dev->attr.exclude_kernel = 0;
    dev->attr.exclude_hv = 1;

    return 0;
}

/* 打开设备 */
static int mypmu_open(struct inode *inode, struct file *file) {
    pr_info("mypmu: device opened\n");
    return 0;
}

/* 释放设备 */
static int mypmu_release(struct inode *inode, struct file *file) {
    if (pmu_dev.event) {
        perf_event_release_kernel(pmu_dev.event);
        pmu_dev.event = NULL;
        pmu_dev.active = 0;
    }
    pr_info("mypmu: device released\n");
    return 0;
}

/* IOCTL处理 */
static long mypmu_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    u64 value;
    u64 config;

    switch (cmd) {
    case MYPMU_IOC_CONFIG:
        /* 配置事件 */
        if (copy_from_user(&config, (void __user *)arg, sizeof(u64)))
            return -EFAULT;

        if (pmu_dev.event) {
            perf_event_release_kernel(pmu_dev.event);
            pmu_dev.event = NULL;
        }

        mypmu_config_event(&pmu_dev, config);
        pmu_dev.event = perf_event_create_kernel_counter(&pmu_dev.attr, 0, NULL, NULL, NULL);
        if (IS_ERR(pmu_dev.event)) {
            pr_err("mypmu: failed to create perf event\n");
            return PTR_ERR(pmu_dev.event);
        }

        pmu_dev.active = 0;
        pr_info("mypmu: configured event 0x%llx\n", config);
        break;

    case MYPMU_IOC_START:
        /* 启动计数 */
        if (!pmu_dev.event)
            return -EINVAL;

        perf_event_enable(pmu_dev.event);
        pmu_dev.active = 1;
        pr_info("mypmu: started counting\n");
        break;

    case MYPMU_IOC_STOP:
        /* 停止计数 */
        if (!pmu_dev.event)
            return -EINVAL;

        perf_event_disable(pmu_dev.event);
        pmu_dev.active = 0;
        pr_info("mypmu: stopped counting\n");
        break;

    case MYPMU_IOC_READ:
        /* 读取计数值 */
        if (!pmu_dev.event)
            return -EINVAL;

        value = perf_event_read_value(pmu_dev.event, &value, &value);

        if (copy_to_user((void __user *)arg, &value, sizeof(u64)))
            return -EFAULT;
        break;

    case MYPMU_IOC_RESET:
        /* 重置计数器 */
        if (!pmu_dev.event)
            return -EINVAL;

        perf_event_reset(pmu_dev.event);
        pr_info("mypmu: counter reset\n");
        break;

    default:
        return -EINVAL;
    }

    return 0;
}

static struct file_operations mypmu_fops = {
    .owner = THIS_MODULE,
    .open = mypmu_open,
    .release = mypmu_release,
    .unlocked_ioctl = mypmu_ioctl,
};

/* IOCTL命令定义 */
#define MYPMU_IOC_MAGIC  'P'
#define MYPMU_IOC_CONFIG  _IOW(MYPMU_IOC_MAGIC, 0, u64)
#define MYPMU_IOC_START   _IO(MYPMU_IOC_MAGIC, 1)
#define MYPMU_IOC_STOP    _IO(MYPMU_IOC_MAGIC, 2)
#define MYPMU_IOC_READ    _IOR(MYPMU_IOC_MAGIC, 3, u64)
#define MYPMU_IOC_RESET   _IO(MYPMU_IOC_MAGIC, 4)

static int __init mypmu_init(void) {
    int ret;

    /* 分配设备号 */
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("mypmu: failed to allocate device number\n");
        return ret;
    }

    /* 初始化cdev */
    cdev_init(&mypmu_cdev, &mypmu_fops);
    mypmu_cdev.owner = THIS_MODULE;

    ret = cdev_add(&mypmu_cdev, dev_num, 1);
    if (ret) {
        pr_err("mypmu: failed to add cdev\n");
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    /* 创建类 */
    mypmu_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(mypmu_class)) {
        pr_err("mypmu: failed to create class\n");
        cdev_del(&mypmu_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(mypmu_class);
    }

    /* 创建设备 */
    mypmu_device = device_create(mypmu_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(mypmu_device)) {
        pr_err("mypmu: failed to create device\n");
        class_destroy(mypmu_class);
        cdev_del(&mypmu_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(mypmu_device);
    }

    memset(&pmu_dev, 0, sizeof(pmu_dev));

    pr_info("mypmu: driver loaded (major=%d)\n", MAJOR(dev_num));
    return 0;
}

static void __exit mypmu_exit(void) {
    if (pmu_dev.event) {
        perf_event_release_kernel(pmu_dev.event);
    }

    device_destroy(mypmu_class, dev_num);
    class_destroy(mypmu_class);
    cdev_del(&mypmu_cdev);
    unregister_chrdev_region(dev_num, 1);

    pr_info("mypmu: driver unloaded\n");
}

module_init(mypmu_init);
module_exit(mypmu_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Performance Analysis Team");
MODULE_DESCRIPTION("Custom PMU Driver Example");
```

### 6.2 用户空间驱动程序

```c
/* mypmu_user.c - PMU驱动用户空间程序 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>

/* IOCTL命令 */
#define MYPMU_IOC_MAGIC  'P'
#define MYPMU_IOC_CONFIG  _IOW(MYPMU_IOC_MAGIC, 0, uint64_t)
#define MYPMU_IOC_START   _IO(MYPMU_IOC_MAGIC, 1)
#define MYPMU_IOC_STOP    _IO(MYPMU_IOC_MAGIC, 2)
#define MYPMU_IOC_READ    _IOR(MYPMU_IOC_MAGIC, 3, uint64_t)
#define MYPMU_IOC_RESET   _IO(MYPMU_IOC_MAGIC, 4)

/* perf事件类型 */
#define PERF_COUNT_HW_CPU_CYCLES        0
#define PERF_COUNT_HW_INSTRUCTIONS      1
#define PERF_COUNT_HW_CACHE_REFERENCES  2
#define PERF_COUNT_HW_CACHE_MISSES      3
#define PERF_COUNT_HW_BRANCH_INSTRUCTIONS 4
#define PERF_COUNT_HW_BRANCH_MISSES     5

int main(int argc, char *argv[]) {
    int fd = open("/dev/mypmu", O_RDWR);
    if (fd < 0) {
        perror("open /dev/mypmu");
        printf("请确保驱动已加载: sudo insmod my_pmu_driver.ko\n");
        return 1;
    }

    printf("自定义PMU驱动测试程序\n");
    printf("=====================\n\n");

    /* 测试1: CPU周期计数 */
    printf("测试1: CPU周期计数\n");

    uint64_t config = PERF_COUNT_HW_CPU_CYCLES;
    if (ioctl(fd, MYPMU_IOC_CONFIG, &config) < 0) {
        perror("ioctl CONFIG");
        close(fd);
        return 1;
    }

    ioctl(fd, MYPMU_IOC_RESET);
    ioctl(fd, MYPMU_IOC_START);

    /* 执行一些工作 */
    volatile long sum = 0;
    for (int i = 0; i < 1000000; i++) {
        sum += i;
    }
    (void)sum;

    ioctl(fd, MYPMU_IOC_STOP);

    uint64_t cycles;
    if (ioctl(fd, MYPMU_IOC_READ, &cycles) < 0) {
        perror("ioctl READ");
    } else {
        printf("  周期数: %lu\n", cycles);
    }

    /* 测试2: 指令计数 */
    printf("\n测试2: 指令计数\n");

    config = PERF_COUNT_HW_INSTRUCTIONS;
    ioctl(fd, MYPMU_IOC_CONFIG, &config);
    ioctl(fd, MYPMU_IOC_RESET);
    ioctl(fd, MYPMU_IOC_START);

    /* 执行一些工作 */
    sum = 0;
    for (int i = 0; i < 1000000; i++) {
        sum += i;
    }

    ioctl(fd, MYPMU_IOC_STOP);

    uint64_t instructions;
    ioctl(fd, MYPMU_IOC_READ, &instructions);
    printf("  指令数: %lu\n", instructions);
    printf("  CPI: %.2f\n", (double)cycles / instructions);

    /* 测试3: 缓存未命中 */
    printf("\n测试3: 缓存未命中测试\n");

    config = PERF_COUNT_HW_CACHE_MISSES;
    ioctl(fd, MYPMU_IOC_CONFIG, &config);
    ioctl(fd, MYPMU_IOC_RESET);
    ioctl(fd, MYPMU_IOC_START);

    /* 随机访问大数组 */
    int size = 100 * 1024 * 1024;  /* 100MB */
    char *data = malloc(size);
    for (int i = 0; i < 100000; i++) {
        int idx = rand() % size;
        data[idx] = i;
    }
    free(data);

    ioctl(fd, MYPMU_IOC_STOP);

    uint64_t cache_misses;
    ioctl(fd, MYPMU_IOC_READ, &cache_misses);
    printf("  缓存未命中: %lu\n", cache_misses);

    close(fd);

    printf("\n测试完成!\n");
    return 0;
}
```

---

## 7. 综合微架构分析工具

### 7.1 完整微架构基准测试套件

```c
/* comprehensive_microbench.c - 综合微架构基准测试 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sched.h>

/* RDTSC封装 */
static inline uint64_t rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

static inline uint64_t rdtscp(uint32_t *aux) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtscp" : "=a" (lo), "=d" (hi), "=c" (*aux));
    return ((uint64_t)hi << 32) | lo;
}

static inline void cpuid(void) {
    __asm__ __volatile__ ("cpuid" ::: "eax", "ebx", "ecx", "edx");
}

static inline void mfence(void) {
    __asm__ __volatile__ ("mfence" ::: "memory");
}

/* 绑定CPU */
void bind_cpu(int cpu) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    sched_setaffinity(0, sizeof(cpuset), &cpuset);
}

/* ==================== 测试函数 ==================== */

/* 1. CPU基础性能 */
void test_cpu_baseline(void) {
    printf("\n【1. CPU基础性能测试】\n");
    printf("--------------------------------------------------\n");

    const int N = 100000000;
    volatile long sum = 0;
    uint32_t aux;

    /* 整数加法 */
    cpuid(); mfence();
    uint64_t start = rdtscp(&aux);
    for (int i = 0; i < N; i++) {
        sum += i;
    }
    uint64_t end = rdtscp(&aux);
    (void)sum;

    double cycles_per_add = (double)(end - start) / N;
    printf("整数加法: %.3f 周期/操作\n", cycles_per_add);

    /* 整数乘法 */
    sum = 1;
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (int i = 0; i < N; i++) {
        sum *= (i % 100 + 1);
    }
    end = rdtscp(&aux);
    (void)sum;

    double cycles_per_mul = (double)(end - start) / N;
    printf("整数乘法: %.3f 周期/操作\n", cycles_per_mul);

    /* 整数除法 */
    sum = 1000000000;
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (int i = 0; i < N / 100; i++) {
        sum /= (i % 100 + 1);
        sum += 1000000000;
    }
    end = rdtscp(&aux);
    (void)sum;

    double cycles_per_div = (double)(end - start) / (N / 100);
    printf("整数除法: %.3f 周期/操作\n", cycles_per_div);

    /* 浮点运算 */
    volatile double fsum = 0.0;
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (int i = 0; i < N; i++) {
        fsum += (double)i * 0.001;
    }
    end = rdtscp(&aux);
    (void)fsum;

    double cycles_per_fadd = (double)(end - start) / N;
    printf("浮点加法: %.3f 周期/操作\n", cycles_per_fadd);

    /* 平方根 */
    fsum = 1.0;
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (int i = 0; i < N / 10; i++) {
        fsum = sqrt(fsum + 1.0);
    }
    end = rdtscp(&aux);
    (void)fsum;

    double cycles_per_sqrt = (double)(end - start) / (N / 10);
    printf("平方根: %.3f 周期/操作\n", cycles_per_sqrt);
}

/* 2. 内存延迟测试 */
void test_memory_latency(void) {
    printf("\n【2. 内存延迟测试】\n");
    printf("--------------------------------------------------\n");

    struct node {
        struct node *next;
        uint64_t pad[7];
    };

    int sizes_kb[] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
    int num_tests = sizeof(sizes_kb) / sizeof(sizes_kb[0]);

    for (int t = 0; t < num_tests; t++) {
        size_t num_nodes = (sizes_kb[t] * 1024) / sizeof(struct node);
        struct node *nodes = aligned_alloc(64, num_nodes * sizeof(struct node));

        /* 创建随机链表 */
        size_t *indices = malloc(num_nodes * sizeof(size_t));
        for (size_t i = 0; i < num_nodes; i++) indices[i] = i;
        for (size_t i = num_nodes - 1; i > 0; i--) {
            size_t j = rand() % (i + 1);
            size_t tmp = indices[i];
            indices[i] = indices[j];
            indices[j] = tmp;
        }
        for (size_t i = 0; i < num_nodes - 1; i++) {
            nodes[indices[i]].next = &nodes[indices[i + 1]];
        }
        nodes[indices[num_nodes - 1]].next = &nodes[indices[0]];
        free(indices);

        /* 测量 */
        struct node *p = nodes;
        const int iterations = 1000000;
        uint32_t aux;

        /* 预热 */
        for (int i = 0; i < 1000; i++) p = p->next;

        cpuid(); mfence();
        uint64_t start = rdtscp(&aux);
        for (int i = 0; i < iterations; i++) {
            p = p->next; p = p->next; p = p->next; p = p->next;
        }
        uint64_t end = rdtscp(&aux);
        (void)p;

        double latency = (double)(end - start) / (iterations * 4);

        char level[32];
        if (sizes_kb[t] <= 32) strcpy(level, "L1");
        else if (sizes_kb[t] <= 512) strcpy(level, "L2");
        else if (sizes_kb[t] <= 8192) strcpy(level, "L3");
        else strcpy(level, "Memory");

        printf("%5d KB [%s]: %.2f 周期\n", sizes_kb[t], level, latency);

        free(nodes);
    }
}

/* 3. 内存带宽测试 */
void test_memory_bandwidth(void) {
    printf("\n【3. 内存带宽测试】\n");
    printf("--------------------------------------------------\n");

    size_t size = 100 * 1024 * 1024;  /* 100MB */
    char *src = aligned_alloc(64, size);
    char *dst = aligned_alloc(64, size);
    memset(src, 0xAA, size);

    /* 顺序读取 */
    volatile uint64_t sum = 0;
    uint32_t aux;
    cpuid(); mfence();
    uint64_t start = rdtscp(&aux);
    for (size_t i = 0; i < size; i += 64) {
        sum += *(uint64_t*)(src + i);
    }
    uint64_t end = rdtscp(&aux);
    (void)sum;

    double read_bw = (double)size / ((end - start) / 3.0e9) / (1024 * 1024 * 1024);
    printf("顺序读取带宽: %.2f GB/s\n", read_bw);

    /* 顺序写入 */
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (size_t i = 0; i < size; i += 64) {
        *(uint64_t*)(dst + i) = i;
    }
    end = rdtscp(&aux);

    double write_bw = (double)size / ((end - start) / 3.0e9) / (1024 * 1024 * 1024);
    printf("顺序写入带宽: %.2f GB/s\n", write_bw);

    /* 复制带宽 */
    cpuid(); mfence();
    start = rdtscp(&aux);
    memcpy(dst, src, size);
    end = rdtscp(&aux);

    double copy_bw = (double)(size * 2) / ((end - start) / 3.0e9) / (1024 * 1024 * 1024);
    printf("复制带宽: %.2f GB/s\n", copy_bw);

    free(src);
    free(dst);
}

/* 4. 分支预测测试 */
void test_branch_prediction(void) {
    printf("\n【4. 分支预测测试】\n");
    printf("--------------------------------------------------\n");

    const int N = 100000000;
    int *sorted = malloc(N * sizeof(int));
    int *random = malloc(N * sizeof(int));

    for (int i = 0; i < N; i++) {
        sorted[i] = i % 200;
        random[i] = rand() % 200;
    }

    volatile int count = 0;
    uint32_t aux;
    uint64_t start, end;

    /* 排序数据 */
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (int i = 0; i < N; i++) {
        if (sorted[i] > 100) count++;
    }
    end = rdtscp(&aux);

    double sorted_time = (double)(end - start) / N;
    printf("排序数据分支: %.3f 周期/迭代\n", sorted_time);

    /* 随机数据 */
    count = 0;
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (int i = 0; i < N; i++) {
        if (random[i] > 100) count++;
    }
    end = rdtscp(&aux);
    (void)count;

    double random_time = (double)(end - start) / N;
    printf("随机数据分支: %.3f 周期/迭代\n", random_time);
    printf("预测失败惩罚: %.1f 周期\n", random_time - sorted_time);

    free(sorted);
    free(random);
}

/* 5. 预取器测试 */
void test_prefetcher(void) {
    printf("\n【5. 预取器效率测试】\n");
    printf("--------------------------------------------------\n");

    size_t size = 100 * 1024 * 1024;
    int *arr = aligned_alloc(64, size);

    for (size_t i = 0; i < size / sizeof(int); i++) {
        arr[i] = i;
    }

    uint32_t aux;
    uint64_t start, end;

    /* 顺序访问 */
    volatile int sum = 0;
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (size_t i = 0; i < size / sizeof(int); i++) {
        sum += arr[i];
    }
    end = rdtscp(&aux);

    double seq_cycles_per_access = (double)(end - start) / (size / sizeof(int));
    printf("顺序访问: %.3f 周期/访问\n", seq_cycles_per_access);

    /* 步幅64访问 (缓存行步幅) */
    sum = 0;
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (size_t i = 0; i < size / sizeof(int); i += 16) {  /* 64字节步幅 */
        sum += arr[i];
    }
    end = rdtscp(&aux);
    (void)sum;

    double stride64_cycles = (double)(end - start) / (size / sizeof(int) / 16);
    printf("步幅64访问: %.3f 周期/访问\n", stride64_cycles);

    /* 步幅4096访问 (页面步幅，预取器失效) */
    sum = 0;
    cpuid(); mfence();
    start = rdtscp(&aux);
    for (size_t i = 0; i < size / sizeof(int); i += 1024) {  /* 4096字节步幅 */
        sum += arr[i];
    }
    end = rdtscp(&aux);

    double stride4k_cycles = (double)(end - start) / (size / sizeof(int) / 1024);
    printf("步幅4K访问: %.3f 周期/访问\n", stride4k_cycles);

    printf("预取器加速比: %.1fx\n", stride4k_cycles / seq_cycles_per_access);

    free(arr);
}

/* 主函数 */
int main(int argc, char *argv[]) {
    printf("========================================\n");
    printf("    综合微架构基准测试套件               \n");
    printf("========================================\n");

    /* 绑定CPU并设置优先级 */
    bind_cpu(0);

    /* 禁用输出缓冲 */
    setbuf(stdout, NULL);

    /* 运行所有测试 */
    test_cpu_baseline();
    test_memory_latency();
    test_memory_bandwidth();
    test_branch_prediction();
    test_prefetcher();

    printf("\n========================================\n");
    printf("测试完成!\n");
    printf("========================================\n");

    return 0;
}
```

### 7.2 Makefile

```makefile
# Makefile for microarchitecture benchmarking

CC = gcc
CFLAGS = -O2 -g -Wall -march=native
LDFLAGS = -lm

TARGETS = cache_latency_test prefetcher_test branch_prediction_test \
          tlb_test memory_bandwidth_test comprehensive_microbench \
          mypmu_user

all: $(TARGETS)

cache_latency_test: cache_latency_test.c
 $(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

prefetcher_test: prefetcher_test.c
 $(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

branch_prediction_test: branch_prediction_test.c
 $(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

tlb_test: tlb_test.c
 $(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

memory_bandwidth_test: memory_bandwidth_test.c
 $(CC) $(CFLAGS) -mavx2 -o $@ $< $(LDFLAGS)

comprehensive_microbench: comprehensive_microbench.c
 $(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

mypmu_user: mypmu_user.c
 $(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# 内核模块
obj-m += my_pmu_driver.o

kernel_module:
 make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
 rm -f $(TARGETS)
 make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

# 运行测试
test: all
 @echo "运行Cache延迟测试..."
 ./cache_latency_test
 @echo ""
 @echo "运行综合测试..."
 ./comprehensive_microbench

.PHONY: all clean test kernel_module
```

---

## 8. 总结与参考

### 8.1 微架构基准测试要点

```
┌─────────────────────────────────────────────────────────────────┐
│                    微架构测试最佳实践                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  1. 测试环境控制                                                │
│     • 固定CPU频率，禁用Turbo Boost                             │
│     • 绑定到单个CPU核心                                         │
│     • 禁用超线程/SMT                                            │
│     • 最小化系统干扰                                            │
│                                                                 │
│  2. 测量精度                                                    │
│     • 使用RDTSCP而非RDTSC (考虑序列化)                          │
│     • 多次测量取中位数                                          │
│     • 消除测量开销                                              │
│     • 足够的迭代次数                                            │
│                                                                 │
│  3. 结果验证                                                    │
│     • 与已知参考值对比                                          │
│     • 多种方法交叉验证                                          │
│     • 考虑处理器架构差异                                        │
│                                                                 │
│  4. 常见陷阱                                                    │
│     • 编译器优化干扰                                            │
│     • 硬件预取器影响                                            │
│     • 乱序执行导致的非确定性                                    │
│     • 功耗管理引入的变化                                        │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 8.2 参考资源

- Intel 64 and IA-32 Architectures Optimization Reference Manual
- Agner Fog's Microarchitecture Manuals
- lmbench: Portable tools for performance analysis
- tinymembench: Simple benchmark for memory throughput and latency
- PMUDemo: <https://github.com/andikleen/pmu-tools>
