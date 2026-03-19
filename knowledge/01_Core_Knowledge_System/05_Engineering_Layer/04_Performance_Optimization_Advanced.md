# C语言性能优化深度指南

> 本指南深入探讨C语言高性能编程的核心技术，涵盖从CPU微架构到系统级优化的完整知识体系。

---

## 目录

- [C语言性能优化深度指南](#c语言性能优化深度指南)
  - [目录](#目录)
  - [1. CPU微架构基础](#1-cpu微架构基础)
    - [1.1 现代CPU架构概述](#11-现代cpu架构概述)
    - [1.2 汇编代码分析](#12-汇编代码分析)
    - [1.3 分支预测器内部机制](#13-分支预测器内部机制)
    - [1.4 执行端口与发射带宽](#14-执行端口与发射带宽)
  - [2. 缓存优化技术](#2-缓存优化技术)
    - [2.1 CPU缓存层次结构](#21-cpu缓存层次结构)
    - [2.2 缓存预取优化](#22-缓存预取优化)
    - [2.3 数据对齐优化](#23-数据对齐优化)
  - [3. SIMD优化指南](#3-simd优化指南)
    - [3.1 SSE/AVX基础](#31-sseavx基础)
    - [3.2 ARM NEON优化](#32-arm-neon优化)
    - [3.3 SIMD汇编级优化](#33-simd汇编级优化)
  - [4. 编译器优化内部](#4-编译器优化内部)
    - [4.1 GCC/Clang优化Pass](#41-gccclang优化pass)
    - [4.2 编译器优化报告解读](#42-编译器优化报告解读)
    - [4.3 汇编输出分析](#43-汇编输出分析)
  - [5. 内存优化技术](#5-内存优化技术)
    - [5.1 HugePages大页内存](#51-hugepages大页内存)
    - [5.2 NUMA内存优化](#52-numa内存优化)
    - [5.3 内存池与自定义分配器](#53-内存池与自定义分配器)
  - [6. I/O优化技术](#6-io优化技术)
    - [6.1 零拷贝技术](#61-零拷贝技术)
    - [6.2 异步I/O与io\_uring](#62-异步io与io_uring)
  - [7. 并发优化技术](#7-并发优化技术)
    - [7.1 无锁编程](#71-无锁编程)
    - [7.2 RCU（Read-Copy-Update）](#72-rcuread-copy-update)
  - [8. 算法优化策略](#8-算法优化策略)
    - [8.1 向量化算法](#81-向量化算法)
    - [8.2 并行化算法](#82-并行化算法)
  - [9. 性能测量与分析](#9-性能测量与分析)
    - [9.1 PMC（性能监控计数器）](#91-pmc性能监控计数器)
    - [9.2 PEBS（Processor Event-Based Sampling）](#92-pebsprocessor-event-based-sampling)
  - [10. 实战案例：高频交易系统优化](#10-实战案例高频交易系统优化)
    - [10.1 系统概述与优化目标](#101-系统概述与优化目标)
    - [10.2 完整优化清单](#102-完整优化清单)
  - [总结](#总结)
    - [性能优化层次](#性能优化层次)
    - [关键原则](#关键原则)
    - [推荐工具](#推荐工具)

---

## 1. CPU微架构基础

### 1.1 现代CPU架构概述

现代CPU采用复杂的多级流水线架构，理解这些特性对于编写高性能代码至关重要。

```c
/**
 * @file cpu_architecture_demo.c
 * @brief CPU微架构特性演示程序
 * @details 展示流水线、分支预测、乱序执行对性能的影响
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#ifdef _MSC_VER
    #include <intrin.h>
    #pragma intrinsic(__rdtsc)
#else
    #include <x86intrin.h>
#endif

/* 获取高精度时间戳（CPU周期计数器） */
static inline uint64_t rdtsc(void) {
#ifdef _MSC_VER
    return __rdtsc();
#else
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
#endif
}

/* 内存屏障，防止编译器重排序 */
#define MEMORY_BARRIER() __asm__ __volatile__("" ::: "memory")

/* 防止CPU乱序执行 */
#define CPU_BARRIER() __asm__ __volatile__("mfence" ::: "memory")

/**
 * @brief 流水线冒险演示 - 数据依赖链
 *
 * 展示数据依赖如何阻塞流水线
 * 每条指令依赖前一条指令的结果，导致无法并行执行
 */
double pipeline_hazard_demo(int iterations) {
    double result = 1.0;
    uint64_t start = rdtsc();

    /* 循环依赖链 - 每条指令必须等待前一条完成 */
    for (int i = 0; i < iterations; i++) {
        result = result * 1.000001 + 0.000001;  /* 依赖链: result(i) = f(result(i-1)) */
    }

    uint64_t end = rdtsc();
    printf("Pipeline hazard: %lu cycles/iter\n", (end - start) / iterations);
    return result;
}

/**
 * @brief 流水线优化 - 循环展开减少依赖
 *
 * 通过循环展开，创建多个独立的计算链
 * 允许CPU同时执行多个独立操作
 */
double pipeline_optimized_demo(int iterations) {
    /* 四个独立的累加器，打破依赖链 */
    double a = 1.0, b = 2.0, c = 3.0, d = 4.0;
    uint64_t start = rdtsc();

    /* 每次迭代处理4个元素，4条独立的流水线 */
    for (int i = 0; i < iterations; i += 4) {
        a = a * 1.000001 + 0.000001;  /* 独立链1 */
        b = b * 1.000001 + 0.000001;  /* 独立链2 */
        c = c * 1.000001 + 0.000001;  /* 独立链3 */
        d = d * 1.000001 + 0.000001;  /* 独立链4 */
    }

    uint64_t end = rdtsc();
    printf("Pipeline optimized: %lu cycles/iter (4-way)\n", (end - start) / iterations);
    return a + b + c + d;
}

/**
 * @brief 分支预测失败惩罚演示
 *
 * 展示分支预测失败对性能的影响
 * 随机数据导致预测失败率接近50%
 */
void branch_misprediction_demo(void) {
    const int N = 10000000;
    int *data = malloc(N * sizeof(int));
    volatile long long sum = 0;

    /* 初始化：有序数据 - 分支预测成功率高 */
    for (int i = 0; i < N; i++) {
        data[i] = i % 100;  /* 0-99循环 */
    }

    uint64_t start = rdtsc();
    for (int i = 0; i < N; i++) {
        /* 模式固定，分支预测器能快速学习 */
        if (data[i] < 50) {
            sum += data[i];
        }
    }
    uint64_t ordered_end = rdtsc();

    /* 重新初始化：随机数据 - 分支预测失败率高 */
    srand(42);
    for (int i = 0; i < N; i++) {
        data[i] = rand() % 100;
    }

    sum = 0;
    start = rdtsc();
    for (int i = 0; i < N; i++) {
        /* 随机模式，分支预测器无法学习 */
        if (data[i] < 50) {
            sum += data[i];
        }
    }
    uint64_t random_end = rdtsc();

    printf("Ordered data:   %lu cycles\n", ordered_end - start);
    printf("Random data:    %lu cycles\n", random_end - start);
    printf("Misprediction penalty: ~%lu cycles per branch\n",
           (random_end - ordered_end) / N);

    free(data);
}

/**
 * @brief 分支预测优化 - 使用条件移动
 *
 * 通过cmov指令避免分支，消除预测失败风险
 * 适用于无法预测的数据模式
 */
void branchless_optimization_demo(void) {
    const int N = 10000000;
    int *data = malloc(N * sizeof(int));
    volatile long long sum = 0;

    srand(42);
    for (int i = 0; i < N; i++) {
        data[i] = rand() % 100;
    }

    uint64_t start = rdtsc();
    for (int i = 0; i < N; i++) {
        /* 使用条件移动代替分支 - 无分支预测风险 */
        int val = data[i];
        int add = (val < 50) ? val : 0;  /* 编译器可能生成cmov */
        sum += add;
    }
    uint64_t end = rdtsc();

    printf("Branchless (ternary): %lu cycles\n", end - start);
    free(data);
}

/**
 * @brief 乱序执行演示 - 指令级并行
 *
 * 展示CPU如何重排序指令以最大化并行度
 */
void out_of_order_demo(void) {
    const int N = 10000000;
    volatile double result = 0;

    /* 场景1：强数据依赖 - 无法并行 */
    uint64_t start = rdtsc();
    double a = 1.0;
    for (int i = 0; i < N; i++) {
        a = a + 1.0;           /* 依赖: a(i) = a(i-1) + 1 */
        a = a * 1.000001;      /* 依赖: a(i) = a(i-1) * 1.000001 */
        a = a - 0.000001;      /* 依赖: a(i) = a(i-1) - 0.000001 */
    }
    result = a;
    uint64_t dep_cycles = rdtsc() - start;

    /* 场景2：独立操作 - 可乱序并行执行 */
    start = rdtsc();
    double x = 1.0, y = 2.0, z = 3.0;
    for (int i = 0; i < N; i++) {
        x = x + 1.0;           /* 独立操作，可与y,z操作并行 */
        y = y * 1.000001;      /* 独立操作 */
        z = z - 0.000001;      /* 独立操作 */
    }
    result = x + y + z;
    uint64_t ind_cycles = rdtsc() - start;

    printf("Dependent ops:  %lu cycles\n", dep_cycles);
    printf("Independent ops: %lu cycles (%.2fx speedup)\n",
           ind_cycles, (double)dep_cycles / ind_cycles);
}

int main(void) {
    printf("=== CPU Microarchitecture Demo ===\n\n");

    printf("--- Pipeline Demo ---\n");
    pipeline_hazard_demo(10000000);
    pipeline_optimized_demo(10000000);

    printf("\n--- Branch Prediction Demo ---\n");
    branch_misprediction_demo();

    printf("\n--- Branchless Optimization Demo ---\n");
    branchless_optimization_demo();

    printf("\n--- Out-of-Order Execution Demo ---\n");
    out_of_order_demo();

    return 0;
}
```

### 1.2 汇编代码分析

让我们分析上述代码的汇编输出，理解CPU如何执行：

```asm
; 流水线冒险版本（简化）
pipeline_hazard:
    movsd   xmm0, QWORD PTR [result]    ; 加载初始值
.Lloop:
    mulsd   xmm0, QWORD PTR [LC0]       ; result * 1.000001
    addsd   xmm0, QWORD PTR [LC1]       ; + 0.000001
    dec     rcx                         ; 递减计数器
    jne     .Lloop                      ; 如果非零则跳转
    ; 每次迭代必须等待前一次乘法和加法完成
    ; 延迟：mulsd(4 cycles) + addsd(3 cycles) = 7 cycles/iter
```

```asm
; 优化版本 - 4路展开
pipeline_optimized:
    movsd   xmm0, QWORD PTR [a]         ; 加载a
    movsd   xmm1, QWORD PTR [b]         ; 加载b
    movsd   xmm2, QWORD PTR [c]         ; 加载c
    movsd   xmm3, QWORD PTR [d]         ; 加载d
.Lloop:
    mulsd   xmm0, QWORD PTR [LC0]       ; a = a * 1.000001
    mulsd   xmm1, QWORD PTR [LC0]       ; b = b * 1.000001
    mulsd   xmm2, QWORD PTR [LC0]       ; c = c * 1.000001
    mulsd   xmm3, QWORD PTR [LC0]       ; d = d * 1.000001
    addsd   xmm0, QWORD PTR [LC1]       ; a = a + 0.000001
    addsd   xmm1, QWORD PTR [LC1]       ; b = b + 0.000001
    addsd   xmm2, QWORD PTR [LC1]       ; c = c + 0.000001
    addsd   xmm3, QWORD PTR [LC1]       ; d = d + 0.000001
    sub     rcx, 4
    jne     .Lloop
    ; 4个mulsd可以并行发射（假设端口足够）
    ; 理论加速比接近4x
```

### 1.3 分支预测器内部机制

```c
/**
 * @file branch_predictor_analysis.c
 * @brief 现代CPU分支预测器行为分析
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

/* 分支预测器类型模拟 */
typedef enum {
    PREDICTOR_STATIC,       /* 静态预测：总是预测不跳转 */
    PREDICTOR_BIMODAL,      /* 双模态预测器：基于历史 */
    PREDICTOR_GSHARE,       /* Gshare：全局历史异或PC */
    PREDICTOR_PERCEPTRON    /* 感知机预测器：神经网络 */
} PredictorType;

/**
 * @brief 分支模式分析
 *
 * 不同模式对分支预测器的影响：
 * - T-T-T-T: 100%预测成功（学习后）
 * - N-N-N-N: 100%预测成功（学习后）
 * - T-N-T-N: 可能振荡，取决于历史位数
 * - 随机: ~50%成功率
 */
void analyze_branch_patterns(void) {
    const int N = 100000000;
    int *patterns = malloc(N * sizeof(int));

    printf("=== Branch Pattern Analysis ===\n\n");

    /* 模式1: 总是跳转 */
    printf("Pattern 1: Always Taken\n");
    volatile long long sum = 0;
    clock_t start = clock();
    for (int i = 0; i < N; i++) {
        if (1) {  /* 编译器可能优化掉 */
            sum += i;
        }
    }
    printf("  Cycles: %ld\n\n", clock() - start);

    /* 模式2: 交替跳转 (T-N-T-N) */
    printf("Pattern 2: Alternating (T-N-T-N)\n");
    sum = 0;
    start = clock();
    for (int i = 0; i < N; i++) {
        if (i & 1) {  /* 依赖最低位 */
            sum += i;
        }
    }
    printf("  Cycles: %ld\n\n", clock() - start);

    /* 模式3: 每4次跳转一次 */
    printf("Pattern 3: Every 4th iteration\n");
    sum = 0;
    start = clock();
    for (int i = 0; i < N; i++) {
        if ((i & 3) == 0) {
            sum += i;
        }
    }
    printf("  Cycles: %ld\n\n", clock() - start);

    /* 模式4: 随机 */
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        patterns[i] = rand() & 1;
    }

    printf("Pattern 4: Random\n");
    sum = 0;
    start = clock();
    for (int i = 0; i < N; i++) {
        if (patterns[i]) {
            sum += i;
        }
    }
    printf("  Cycles: %ld\n\n", clock() - start);

    free(patterns);
}

/**
 * @brief 间接跳转预测
 *
 * 间接跳转（如函数指针、虚函数表）更难预测
 * 现代CPU使用目标地址缓存（BTB）
 */
typedef void (*func_ptr_t)(int);

void func_a(int x) { volatile int y = x * 2; (void)y; }
void func_b(int x) { volatile int y = x * 3; (void)y; }
void func_c(int x) { volatile int y = x * 5; (void)y; }
void func_d(int x) { volatile int y = x * 7; (void)y; }

void indirect_branch_analysis(void) {
    const int N = 50000000;
    func_ptr_t funcs[] = {func_a, func_b, func_c, func_d};

    printf("=== Indirect Branch Analysis ===\n\n");

    /* 有序调用 - 模式可学习 */
    printf("Ordered indirect calls:\n");
    clock_t start = clock();
    for (int i = 0; i < N; i++) {
        funcs[i % 4](i);  /* A-B-C-D-A-B-C-D模式 */
    }
    printf("  Cycles: %ld\n\n", clock() - start);

    /* 随机调用 - 难以预测 */
    srand(42);
    int *indices = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        indices[i] = rand() % 4;
    }

    printf("Random indirect calls:\n");
    start = clock();
    for (int i = 0; i < N; i++) {
        funcs[indices[i]](i);
    }
    printf("  Cycles: %ld\n", clock() - start);

    free(indices);
}

int main(void) {
    analyze_branch_patterns();
    indirect_branch_analysis();
    return 0;
}
```

### 1.4 执行端口与发射带宽

```c
/**
 * @file execution_ports.c
 * @brief CPU执行端口分析与优化
 *
 * Intel Skylake微架构端口分配：
 * - Port 0: ALU, Vec ALU, Vec Mul, Branch
 * - Port 1: ALU, Vec ALU, Vec Mul, Vec Shuffle
 * - Port 2: Load
 * - Port 3: Load
 * - Port 4: Store
 * - Port 5: ALU, Vec ALU, Vec Shuffle
 * - Port 6: ALU, Branch
 * - Port 7: Store AGU
 */

#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

/* 确保编译器不优化 */
#define NO_INLINE __attribute__((noinline))
#define OPTIMIZE_O3 __attribute__((optimize("-O3")))

/**
 * @brief 端口压力测试 - 整数ALU
 *
 * Skylake有4个ALU端口(0,1,5,6)
 * 最多每周期执行4个独立整数操作
 */
NO_INLINE uint64_t alu_port_pressure_test(int n) {
    uint64_t a = 1, b = 2, c = 3, d = 4;
    uint64_t start = __rdtsc();

    for (int i = 0; i < n; i++) {
        /* 4个独立加法，可并行执行 */
        a = a + 1;
        b = b + 2;
        c = c + 3;
        d = d + 4;
    }

    uint64_t end = __rdtsc();
    printf("4-way ALU: %lu cycles/iter\n", (end - start) / n);
    return a + b + c + d;
}

/**
 * @brief 端口压力测试 - 超过4个ALU操作
 */
NO_INLINE uint64_t alu_port_overload_test(int n) {
    uint64_t a = 1, b = 2, c = 3, d = 4, e = 5;
    uint64_t start = __rdtsc();

    for (int i = 0; i < n; i++) {
        /* 5个加法需要至少2个周期 */
        a = a + 1;
        b = b + 2;
        c = c + 3;
        d = d + 4;
        e = e + 5;
    }

    uint64_t end = __rdtsc();
    printf("5-way ALU: %lu cycles/iter\n", (end - start) / n);
    return a + b + c + d + e;
}

/**
 * @brief Load/Store端口测试
 *
 * 2个Load端口 + 1个Store端口
 * 理论上每周期可执行2 loads + 1 store
 */
NO_INLINE uint64_t memory_port_test(int n) {
    volatile uint64_t data[8] = {1,2,3,4,5,6,7,8};
    uint64_t sum = 0;
    uint64_t start = __rdtsc();

    for (int i = 0; i < n; i++) {
        /* 2 loads + 1 store */
        uint64_t t1 = data[0];
        uint64_t t2 = data[1];
        data[2] = t1 + t2;

        /* 指针算术保持循环依赖 */
        sum += data[2];
    }

    uint64_t end = __rdtsc();
    printf("Load/Store mix: %lu cycles/iter\n", (end - start) / n);
    return sum;
}

/**
 * @brief 乘除法端口压力
 *
 * 乘法使用端口0和1
 * 延迟3-4周期，吞吐量0.5（每周期2个）
 */
NO_INLINE uint64_t multiplication_throughput_test(int n) {
    uint64_t a = 1, b = 2, c = 3, d = 4;
    uint64_t start = __rdtsc();

    for (int i = 0; i < n; i++) {
        /* 4个独立乘法，2个端口 */
        a = a * 3;
        b = b * 5;
        c = c * 7;
        d = d * 11;
    }

    uint64_t end = __rdtsc();
    printf("4-way multiply: %lu cycles/iter (expected ~2)\n", (end - start) / n);
    return a + b + c + d;
}

int main(void) {
    printf("=== CPU Execution Port Analysis ===\n\n");

    printf("--- ALU Port Pressure ---\n");
    volatile uint64_t r1 = alu_port_pressure_test(100000000);
    volatile uint64_t r2 = alu_port_overload_test(100000000);
    (void)r1; (void)r2;

    printf("\n--- Memory Port Pressure ---\n");
    volatile uint64_t r3 = memory_port_test(100000000);
    (void)r3;

    printf("\n--- Multiplication Throughput ---\n");
    volatile uint64_t r4 = multiplication_throughput_test(100000000);
    (void)r4;

    return 0;
}
```

---

## 2. 缓存优化技术

### 2.1 CPU缓存层次结构

```c
/**
 * @file cache_hierarchy.c
 * @brief CPU缓存层次结构分析与优化
 *
 * 典型x86-64缓存结构：
 * L1 Data:    32KB, 8-way, 64B line, 4 cycle latency
 * L1 Inst:    32KB, 8-way, 64B line
 * L2 Unified: 256KB-512KB, 4-way, 12 cycle latency
 * L3 Unified: 8MB-32MB, 16-way, 40 cycle latency
 * DRAM:       GBs, 200+ cycle latency
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <x86intrin.h>

/* 缓存行大小 */
#define CACHE_LINE_SIZE 64

/* 各缓存层级大小（典型值，实际需通过CPUID检测） */
#define L1D_SIZE (32 * 1024)      /* 32KB L1数据缓存 */
#define L2_SIZE (256 * 1024)      /* 256KB L2缓存 */
#define L3_SIZE (8 * 1024 * 1024) /* 8MB L3缓存 */

/* 驱逐缓存行的辅助函数 */
static inline void flush_cache_line(void *p) {
    _mm_clflush(p);
}

/* 内存屏障 */
static inline void mfence(void) {
    _mm_mfence();
}

/* 获取时间戳 */
static inline uint64_t rdtscp(uint32_t *aux) {
    return __rdtscp(aux);
}

/**
 * @brief 缓存延迟测试
 *
 * 使用指针追踪（pointer chasing）测量各级缓存延迟
 */
void cache_latency_test(void) {
    printf("=== Cache Latency Test ===\n\n");

    const size_t sizes[] = {
        4 * 1024,           /* 4KB - L1 */
        32 * 1024,          /* 32KB - L1边界 */
        64 * 1024,          /* 64KB - L2 */
        256 * 1024,         /* 256KB - L2边界 */
        512 * 1024,         /* 512KB - L2/L3 */
        4 * 1024 * 1024,    /* 4MB - L3 */
        16 * 1024 * 1024,   /* 16MB - L3边界 */
        64 * 1024 * 1024    /* 64MB - 内存 */
    };
    const char *labels[] = {
        "4KB (L1)", "32KB (L1)", "64KB (L2)",
        "256KB (L2)", "512KB (L2/3)", "4MB (L3)",
        "16MB (L3)", "64MB (DRAM)"
    };

    for (size_t t = 0; t < sizeof(sizes)/sizeof(sizes[0]); t++) {
        size_t size = sizes[t];
        int num_elements = size / sizeof(void*);

        /* 分配对齐内存 */
        void **buffer = aligned_alloc(CACHE_LINE_SIZE, size);
        if (!buffer) continue;

        /* 创建随机访问模式（避免预取） */
        srand(42);
        int *indices = malloc(num_elements * sizeof(int));
        for (int i = 0; i < num_elements; i++) {
            indices[i] = i;
        }
        /* Fisher-Yates shuffle */
        for (int i = num_elements - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            int tmp = indices[i];
            indices[i] = indices[j];
            indices[j] = tmp;
        }

        /* 构建指针链 */
        for (int i = 0; i < num_elements; i++) {
            buffer[indices[i]] = &buffer[indices[(i + 1) % num_elements]];
        }

        /* 预热 */
        volatile void **p = buffer;
        for (int i = 0; i < num_elements; i++) {
            p = *(void **)p;
        }

        /* 测量 */
        p = buffer;
        uint32_t aux;
        uint64_t start = rdtscp(&aux);

        for (int i = 0; i < num_elements; i++) {
            p = *(void **)p;  /* 内存加载依赖 */
        }

        uint64_t end = rdtscp(&aux);
        double cycles_per_access = (double)(end - start) / num_elements;

        printf("%s: %.2f cycles/access\n", labels[t], cycles_per_access);

        free(indices);
        free(buffer);
    }
}

/**
 * @brief 缓存带宽测试
 *
 * 顺序访问可最大化带宽，利用预取和宽向量加载
 */
void cache_bandwidth_test(void) {
    printf("\n=== Cache Bandwidth Test ===\n\n");

    const int N = 100 * 1024 * 1024;  /* 100MB */
    char *src = aligned_alloc(CACHE_LINE_SIZE, N);
    char *dst = aligned_alloc(CACHE_LINE_SIZE, N);

    if (!src || !dst) {
        printf("Memory allocation failed\n");
        return;
    }

    memset(src, 'A', N);

    /* 测试1: memcpy（可能使用rep movsb或SIMD） */
    printf("Testing memcpy bandwidth...\n");
    clock_t start = clock();
    memcpy(dst, src, N);
    clock_t end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    double bandwidth_mb = (N / (1024.0 * 1024.0)) / seconds;
    printf("  memcpy: %.2f MB/s\n", bandwidth_mb);

    /* 测试2: 简单字节拷贝 */
    printf("Testing byte-by-byte copy...\n");
    start = clock();
    for (int i = 0; i < N; i++) {
        dst[i] = src[i];
    }
    end = clock();
    seconds = (double)(end - start) / CLOCKS_PER_SEC;
    bandwidth_mb = (N / (1024.0 * 1024.0)) / seconds;
    printf("  Byte copy: %.2f MB/s\n", bandwidth_mb);

    /* 测试3: 8字节拷贝 */
    printf("Testing 8-byte copy...\n");
    uint64_t *src64 = (uint64_t*)src;
    uint64_t *dst64 = (uint64_t*)dst;
    int n64 = N / 8;
    start = clock();
    for (int i = 0; i < n64; i++) {
        dst64[i] = src64[i];
    }
    end = clock();
    seconds = (double)(end - start) / CLOCKS_PER_SEC;
    bandwidth_mb = (N / (1024.0 * 1024.0)) / seconds;
    printf("  8-byte copy: %.2f MB/s\n", bandwidth_mb);

    free(src);
    free(dst);
}

/**
 * @brief 缓存行伪共享（False Sharing）演示
 *
 * 当不同CPU核心修改同一缓存行内的不同变量时，
 * 会导致缓存行在核心间来回传递，严重降低性能
 */

#include <pthread.h>

/* 未填充结构体 - 容易发生伪共享 */
typedef struct {
    volatile long long counter;
} UnpaddedCounter;

/* 填充到缓存行大小 - 避免伪共享 */
typedef struct {
    volatile long long counter;
    char padding[CACHE_LINE_SIZE - sizeof(long long)];
} PaddedCounter;

#define NUM_THREADS 4
#define ITERATIONS 10000000

void *unpadded_worker(void *arg) {
    UnpaddedCounter *counters = (UnpaddedCounter*)arg;
    int id = (int)(size_t)pthread_self() % NUM_THREADS;  /* 简化 */

    for (int i = 0; i < ITERATIONS; i++) {
        counters[id].counter++;
    }
    return NULL;
}

void *padded_worker(void *arg) {
    PaddedCounter *counters = (PaddedCounter*)arg;
    int id = (int)(size_t)pthread_self() % NUM_THREADS;

    for (int i = 0; i < ITERATIONS; i++) {
        counters[id].counter++;
    }
    return NULL;
}

void false_sharing_demo(void) {
    printf("\n=== False Sharing Demo ===\n\n");

    pthread_t threads[NUM_THREADS];

    /* 未填充版本 */
    printf("Unpadded counters:\n");
    UnpaddedCounter *unpadded = calloc(NUM_THREADS, sizeof(UnpaddedCounter));
    clock_t start = clock();

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, unpadded_worker, unpadded);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("  Time: %ld ms\n", (clock() - start) * 1000 / CLOCKS_PER_SEC);
    free(unpadded);

    /* 填充版本 */
    printf("Padded counters:\n");
    PaddedCounter *padded = calloc(NUM_THREADS, sizeof(PaddedCounter));
    start = clock();

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, padded_worker, padded);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("  Time: %ld ms\n", (clock() - start) * 1000 / CLOCKS_PER_SEC);
    free(padded);
}

int main(void) {
    cache_latency_test();
    cache_bandwidth_test();
    false_sharing_demo();
    return 0;
}
```

### 2.2 缓存预取优化

```c
/**
 * @file cache_prefetching.c
 * @brief 软件预取优化技术
 *
 * 硬件预取：现代CPU自动识别顺序访问模式
 * 软件预取：程序员显式插入预取指令，适用于非线性访问
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <x86intrin.h>

#define ARRAY_SIZE (16 * 1024 * 1024)  /* 16MB - 超出L3 */
#define PREFETCH_DISTANCE 64            /* 预取距离（元素数） */

/* 普通求和 - 依赖硬件预取 */
long long sum_naive(const int *arr, int n) {
    long long sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

/* 软件预取版本 */
long long sum_prefetch(const int *arr, int n) {
    long long sum = 0;

    /* 预热前几个元素 */
    for (int i = 0; i < PREFETCH_DISTANCE && i < n; i++) {
        sum += arr[i];
    }

    /* 主循环：访问当前，预取未来 */
    for (int i = PREFETCH_DISTANCE; i < n; i++) {
        /* _mm_prefetch: 提示CPU预取指定地址到缓存
         * _MM_HINT_T0: 预取到L1
         * _MM_HINT_T1: 预取到L2
         * _MM_HINT_T2: 预取到L3
         * _MM_HINT_NTA: 非临时访问（流式）
         */
        _mm_prefetch((const char*)&arr[i], _MM_HINT_T0);
        sum += arr[i - PREFETCH_DISTANCE];
    }

    /* 处理剩余元素 */
    for (int i = n - PREFETCH_DISTANCE; i < n; i++) {
        sum += arr[i];
    }

    return sum;
}

/* 非线性访问 - 链表遍历 */
typedef struct Node {
    struct Node *next;
    int data;
    char padding[64 - sizeof(void*) - sizeof(int)];  /* 对齐到64字节 */
} Node;

/* 创建随机链表 */
Node* create_random_list(int n) {
    Node *nodes = aligned_alloc(64, n * sizeof(Node));
    if (!nodes) return NULL;

    /* 初始化数组 */
    for (int i = 0; i < n; i++) {
        nodes[i].data = i;
        nodes[i].next = NULL;
    }

    /* 随机打乱并链接 */
    srand(42);
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        /* 交换 */
        Node tmp = nodes[i];
        nodes[i] = nodes[j];
        nodes[j] = tmp;
    }

    /* 建立链表 */
    for (int i = 0; i < n - 1; i++) {
        nodes[i].next = &nodes[i + 1];
    }
    nodes[n - 1].next = NULL;

    return nodes;
}

/* 普通链表遍历 */
long long list_sum_naive(Node *head) {
    long long sum = 0;
    while (head) {
        sum += head->data;
        head = head->next;
    }
    return sum;
}

/* 预取优化链表遍历 */
long long list_sum_prefetch(Node *head) {
    long long sum = 0;
    Node *curr = head;
    Node *prefetch_ptr = head;

    /* 提前移动预取指针 */
    for (int i = 0; i < 8 && prefetch_ptr; i++) {
        prefetch_ptr = prefetch_ptr->next;
    }

    while (curr) {
        /* 预取8个节点之后的节点 */
        if (prefetch_ptr) {
            _mm_prefetch((const char*)prefetch_ptr, _MM_HINT_T0);
            prefetch_ptr = prefetch_ptr->next;
        }

        sum += curr->data;
        curr = curr->next;
    }

    return sum;
}

/* 矩阵转置 - 缓存不友好访问模式 */
void transpose_naive(const float *src, float *dst, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dst[j * n + i] = src[i * n + j];  /* 列访问不连续 */
        }
    }
}

/* 分块转置 + 预取 */
#define BLOCK_SIZE 64
void transpose_blocked_prefetch(const float *src, float *dst, int n) {
    for (int ii = 0; ii < n; ii += BLOCK_SIZE) {
        for (int jj = 0; jj < n; jj += BLOCK_SIZE) {
            /* 处理一个块 */
            for (int i = ii; i < ii + BLOCK_SIZE && i < n; i++) {
                /* 预取下一行 */
                if (i + 1 < ii + BLOCK_SIZE && i + 1 < n) {
                    _mm_prefetch((const char*)&src[(i + 1) * n + jj],
                                _MM_HINT_T0);
                }

                for (int j = jj; j < jj + BLOCK_SIZE && j < n; j++) {
                    dst[j * n + i] = src[i * n + j];
                }
            }
        }
    }
}

void benchmark_prefetch(void) {
    printf("=== Cache Prefetching Benchmark ===\n\n");

    /* 测试1: 数组求和 */
    int *arr = aligned_alloc(64, ARRAY_SIZE * sizeof(int));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        arr[i] = i;
    }

    printf("Array sum (16M elements):\n");

    /* 预热 */
    volatile long long s = sum_naive(arr, ARRAY_SIZE);
    (void)s;

    clock_t start = clock();
    volatile long long sum1 = sum_naive(arr, ARRAY_SIZE);
    clock_t end = clock();
    printf("  Naive: %ld ms (sum=%lld)\n",
           (end - start) * 1000 / CLOCKS_PER_SEC, sum1);

    start = clock();
    volatile long long sum2 = sum_prefetch(arr, ARRAY_SIZE);
    end = clock();
    printf("  Prefetch: %ld ms (sum=%lld)\n",
           (end - start) * 1000 / CLOCKS_PER_SEC, sum2);

    /* 测试2: 链表遍历 */
    printf("\nLinked list traversal (1M nodes):\n");
    Node *list = create_random_list(1000000);

    start = clock();
    volatile long long ls1 = list_sum_naive(list);
    end = clock();
    printf("  Naive: %ld ms\n", (end - start) * 1000 / CLOCKS_PER_SEC);

    start = clock();
    volatile long long ls2 = list_sum_prefetch(list);
    end = clock();
    printf("  Prefetch: %ld ms\n", (end - start) * 1000 / CLOCKS_PER_SEC);

    /* 测试3: 矩阵转置 */
    printf("\nMatrix transpose (4096x4096):\n");
    int n = 4096;
    float *m1 = aligned_alloc(64, n * n * sizeof(float));
    float *m2 = aligned_alloc(64, n * n * sizeof(float));

    start = clock();
    transpose_naive(m1, m2, n);
    end = clock();
    printf("  Naive: %ld ms\n", (end - start) * 1000 / CLOCKS_PER_SEC);

    start = clock();
    transpose_blocked_prefetch(m1, m2, n);
    end = clock();
    printf("  Blocked+Prefetch: %ld ms\n", (end - start) * 1000 / CLOCKS_PER_SEC);

    free(arr);
    free(list);
    free(m1);
    free(m2);
}

int main(void) {
    benchmark_prefetch();
    return 0;
}
```

### 2.3 数据对齐优化

```c
/**
 * @file alignment_optimization.c
 * @brief 数据对齐优化技术
 *
 * 对齐原则：
 * - 基本类型按其自然边界对齐
 * - 结构体按最大成员对齐
 * - 缓存行对齐（64字节）用于并发访问
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <immintrin.h>
#include <time.h>

#define CACHE_LINE 64

/* 未对齐的结构体 - 可能有内部填充 */
typedef struct {
    char a;       /* 1字节 */
    int b;        /* 4字节，可能需要3字节填充 */
    char c;       /* 1字节 */
    double d;     /* 8字节，可能需要7字节填充 */
} BadAlignment;

/* 重新排列的结构体 - 最小化填充 */
typedef struct {
    double d;     /* 8字节 */
    int b;        /* 4字节 */
    char a;       /* 1字节 */
    char c;       /* 1字节 */
    /* 2字节填充到8字节对齐 */
} GoodAlignment;

/* 显式对齐的结构体 */
typedef struct __attribute__((aligned(CACHE_LINE))) {
    char data[56];  /* 填充到64字节 */
} CacheLineAligned;

void print_structure_info(void) {
    printf("=== Structure Alignment Analysis ===\n\n");

    printf("BadAlignment:\n");
    printf("  Size: %zu bytes\n", sizeof(BadAlignment));
    printf("  offsetof(a): %zu\n", __builtin_offsetof(BadAlignment, a));
    printf("  offsetof(b): %zu\n", __builtin_offsetof(BadAlignment, b));
    printf("  offsetof(c): %zu\n", __builtin_offsetof(BadAlignment, c));
    printf("  offsetof(d): %zu\n", __builtin_offsetof(BadAlignment, d));
    printf("  Wasted space: ~%zu bytes\n\n",
           sizeof(BadAlignment) - (1 + 4 + 1 + 8));

    printf("GoodAlignment:\n");
    printf("  Size: %zu bytes\n", sizeof(GoodAlignment));
    printf("  offsetof(d): %zu\n", __builtin_offsetof(GoodAlignment, d));
    printf("  offsetof(b): %zu\n", __builtin_offsetof(GoodAlignment, b));
    printf("  offsetof(a): %zu\n", __builtin_offsetof(GoodAlignment, a));
    printf("  offsetof(c): %zu\n", __builtin_offsetof(GoodAlignment, c));
    printf("  Wasted space: %zu bytes\n\n",
           sizeof(GoodAlignment) - (8 + 4 + 1 + 1));
}

/* 未对齐内存访问的性能影响 */
void misaligned_access_demo(void) {
    printf("\n=== Misaligned Access Performance ===\n\n");

    const int N = 100000000;
    char *raw = malloc(N * sizeof(int) + 64);

    /* 对齐地址 */
    int *aligned = (int*)(((uintptr_t)raw + 63) & ~63);
    /* 非对齐地址（偏移1字节） */
    int *misaligned = (int*)((char*)aligned + 1);

    /* 初始化 */
    for (int i = 0; i < N; i++) {
        aligned[i] = i;
    }
    memcpy(misaligned, aligned, N * sizeof(int));

    /* 对齐访问 */
    volatile long long sum1 = 0;
    clock_t start = clock();
    for (int i = 0; i < N; i++) {
        sum1 += aligned[i];
    }
    clock_t aligned_time = clock() - start;

    /* 非对齐访问 */
    volatile long long sum2 = 0;
    start = clock();
    for (int i = 0; i < N; i++) {
        sum2 += misaligned[i];  /* 跨缓存行边界 */
    }
    clock_t misaligned_time = clock() - start;

    printf("Aligned access:   %ld ms\n", aligned_time * 1000 / CLOCKS_PER_SEC);
    printf("Misaligned access: %ld ms (%.2fx slower)\n",
           misaligned_time * 1000 / CLOCKS_PER_SEC,
           (double)misaligned_time / aligned_time);

    free(raw);
}

/* SIMD对齐要求演示 */
void simd_alignment_demo(void) {
    printf("\n=== SIMD Alignment Requirements ===\n\n");

    const int N = 10000000;
    float *unaligned = malloc(N * sizeof(float) + 64);
    float *aligned = (float*)(((uintptr_t)unaligned + 31) & ~31);

    for (int i = 0; i < N; i++) {
        aligned[i] = (float)i;
    }

    /* 未对齐的SIMD加载 */
    __m256 sum_vec = _mm256_setzero_ps();
    clock_t start = clock();

    for (int i = 0; i < N; i += 8) {
        /* _mm256_loadu_ps: 非对齐加载，可能跨缓存行 */
        __m256 vec = _mm256_loadu_ps(&aligned[i]);
        sum_vec = _mm256_add_ps(sum_vec, vec);
    }

    clock_t unaligned_simd_time = clock() - start;

    /* 对齐的SIMD加载 */
    sum_vec = _mm256_setzero_ps();
    start = clock();

    for (int i = 0; i < N; i += 8) {
        /* _mm256_load_ps: 对齐加载，要求32字节对齐 */
        __m256 vec = _mm256_load_ps(&aligned[i]);
        sum_vec = _mm256_add_ps(sum_vec, vec);
    }

    clock_t aligned_simd_time = clock() - start;

    printf("Unaligned SIMD (_mm256_loadu_ps): %ld ms\n",
           unaligned_simd_time * 1000 / CLOCKS_PER_SEC);
    printf("Aligned SIMD (_mm256_load_ps):    %ld ms (%.2fx faster)\n",
           aligned_simd_time * 1000 / CLOCKS_PER_SEC,
           (double)unaligned_simd_time / aligned_simd_time);

    /* 水平求和 */
    float result[8];
    _mm256_storeu_ps(result, sum_vec);
    volatile float total = 0;
    for (int i = 0; i < 8; i++) total += result[i];
    (void)total;

    free(unaligned);
}

/* 自定义对齐分配器 */
void* aligned_malloc(size_t size, size_t alignment) {
    void *ptr = NULL;
    #ifdef _WIN32
        ptr = _aligned_malloc(size, alignment);
    #else
        posix_memalign(&ptr, alignment, size);
    #endif
    return ptr;
}

void aligned_free(void *ptr) {
    #ifdef _WIN32
        _aligned_free(ptr);
    #else
        free(ptr);
    #endif
}

int main(void) {
    print_structure_info();
    misaligned_access_demo();
    simd_alignment_demo();
    return 0;
}
```

---

## 3. SIMD优化指南

### 3.1 SSE/AVX基础

```c
/**
 * @file simd_basics.c
 * @brief SIMD基础操作与优化
 *
 * SSE (Streaming SIMD Extensions): 128位寄存器 (XMM0-XMM15)
 * AVX (Advanced Vector Extensions): 256位寄存器 (YMM0-YMM15)
 * AVX-512: 512位寄存器 (ZMM0-ZMM31)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* SSE头文件 */
#include <xmmintrin.h>    /* SSE  */
#include <emmintrin.h>    /* SSE2 */
#include <pmmintrin.h>    /* SSE3 */
#include <tmmintrin.h>    /* SSSE3 */
#include <smmintrin.h>    /* SSE4.1 */
#include <nmmintrin.h>    /* SSE4.2 */

/* AVX头文件 */
#include <immintrin.h>    /* AVX, AVX2, FMA */

/* 禁用优化以确保准确测量 */
#define NO_OPT __attribute__((optimize("-O0")))
#define NO_INLINE __attribute__((noinline))

/**
 * @brief 向量加法 - 标量 vs SSE vs AVX
 */
NO_INLINE void add_scalar(float *dst, const float *a, const float *b, int n) {
    for (int i = 0; i < n; i++) {
        dst[i] = a[i] + b[i];
    }
}

NO_INLINE void add_sse(float *dst, const float *a, const float *b, int n) {
    /* SSE处理128位 = 4个float */
    int i = 0;
    for (; i <= n - 4; i += 4) {
        /* 加载4个float到XMM寄存器 */
        __m128 va = _mm_loadu_ps(&a[i]);  /* unaligned load */
        __m128 vb = _mm_loadu_ps(&b[i]);
        /* 向量加法 */
        __m128 vr = _mm_add_ps(va, vb);
        /* 存储结果 */
        _mm_storeu_ps(&dst[i], vr);
    }
    /* 处理剩余元素 */
    for (; i < n; i++) {
        dst[i] = a[i] + b[i];
    }
}

NO_INLINE void add_avx(float *dst, const float *a, const float *b, int n) {
    /* AVX处理256位 = 8个float */
    int i = 0;
    for (; i <= n - 8; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vr = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&dst[i], vr);
    }
    /* 处理剩余 */
    for (; i < n; i++) {
        dst[i] = a[i] + b[i];
    }
}

NO_INLINE void add_avx_aligned(float *dst, const float *a, const float *b, int n) {
    /* 假设32字节对齐的AVX版本 */
    int i = 0;
    for (; i <= n - 8; i += 8) {
        __m256 va = _mm256_load_ps(&a[i]);   /* aligned load */
        __m256 vb = _mm256_load_ps(&b[i]);
        __m256 vr = _mm256_add_ps(va, vb);
        _mm256_store_ps(&dst[i], vr);        /* aligned store */
    }
    for (; i < n; i++) {
        dst[i] = a[i] + b[i];
    }
}

void benchmark_vector_add(void) {
    printf("=== Vector Addition Benchmark ===\n\n");

    const int N = 100000000;  /* 100M floats */
    float *a = aligned_alloc(32, N * sizeof(float));
    float *b = aligned_alloc(32, N * sizeof(float));
    float *dst = aligned_alloc(32, N * sizeof(float));

    /* 初始化 */
    for (int i = 0; i < N; i++) {
        a[i] = (float)i;
        b[i] = (float)(N - i);
    }

    /* 标量版本 */
    clock_t start = clock();
    add_scalar(dst, a, b, N);
    clock_t scalar_time = clock() - start;
    printf("Scalar:  %ld ms (%.2f GFLOPS)\n",
           scalar_time * 1000 / CLOCKS_PER_SEC,
           (N / 1000000000.0) / (scalar_time / (double)CLOCKS_PER_SEC));

    /* SSE版本 */
    start = clock();
    add_sse(dst, a, b, N);
    clock_t sse_time = clock() - start;
    printf("SSE:     %ld ms (%.2f GFLOPS) - %.2fx speedup\n",
           sse_time * 1000 / CLOCKS_PER_SEC,
           (N / 1000000000.0) / (sse_time / (double)CLOCKS_PER_SEC),
           (double)scalar_time / sse_time);

    /* AVX版本 */
    start = clock();
    add_avx(dst, a, b, N);
    clock_t avx_time = clock() - start;
    printf("AVX:     %ld ms (%.2f GFLOPS) - %.2fx speedup\n",
           avx_time * 1000 / CLOCKS_PER_SEC,
           (N / 1000000000.0) / (avx_time / (double)CLOCKS_PER_SEC),
           (double)scalar_time / avx_time);

    /* AVX对齐版本 */
    start = clock();
    add_avx_aligned(dst, a, b, N);
    clock_t avx_aligned_time = clock() - start;
    printf("AVX(align): %ld ms (%.2f GFLOPS) - %.2fx speedup\n",
           avx_aligned_time * 1000 / CLOCKS_PER_SEC,
           (N / 1000000000.0) / (avx_aligned_time / (double)CLOCKS_PER_SEC),
           (double)scalar_time / avx_aligned_time);

    free(a);
    free(b);
    free(dst);
}

/**
 * @brief 点积计算 - FMA优化
 *
 * FMA (Fused Multiply-Add): 单指令完成 a*b+c
 * 提高精度（只舍入一次）和吞吐（2 FLOPS/指令）
 */
NO_INLINE float dot_scalar(const float *a, const float *b, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

NO_INLINE float dot_sse(const float *a, const float *b, int n) {
    __m128 sum4 = _mm_setzero_ps();
    int i = 0;
    for (; i <= n - 4; i += 4) {
        __m128 va = _mm_loadu_ps(&a[i]);
        __m128 vb = _mm_loadu_ps(&b[i]);
        __m128 prod = _mm_mul_ps(va, vb);
        sum4 = _mm_add_ps(sum4, prod);
    }
    /* 水平求和 */
    float result[4];
    _mm_storeu_ps(result, sum4);
    float sum = result[0] + result[1] + result[2] + result[3];

    for (; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

NO_INLINE float dot_avx_fma(const float *a, const float *b, int n) {
    __m256 sum8 = _mm256_setzero_ps();
    int i = 0;
    for (; i <= n - 8; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        /* FMA: sum = va * vb + sum */
        sum8 = _mm256_fmadd_ps(va, vb, sum8);
    }
    /* 水平求和 - 将高128位加到低128位 */
    __m128 sum_lo = _mm256_castps256_ps128(sum8);
    __m128 sum_hi = _mm256_extractf128_ps(sum8, 1);
    __m128 sum4 = _mm_add_ps(sum_lo, sum_hi);

    float result[4];
    _mm_storeu_ps(result, sum4);
    float sum = result[0] + result[1] + result[2] + result[3];

    for (; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

void benchmark_dot_product(void) {
    printf("\n=== Dot Product Benchmark ===\n\n");

    const int N = 100000000;
    float *a = aligned_alloc(32, N * sizeof(float));
    float *b = aligned_alloc(32, N * sizeof(float));

    for (int i = 0; i < N; i++) {
        a[i] = (float)(i % 100) / 100.0f;
        b[i] = (float)((N - i) % 100) / 100.0f;
    }

    clock_t start = clock();
    volatile float r1 = dot_scalar(a, b, N);
    clock_t scalar_time = clock() - start;
    printf("Scalar:   %ld ms (result=%.6f)\n",
           scalar_time * 1000 / CLOCKS_PER_SEC, r1);

    start = clock();
    volatile float r2 = dot_sse(a, b, N);
    clock_t sse_time = clock() - start;
    printf("SSE:      %ld ms (result=%.6f, %.2fx)\n",
           sse_time * 1000 / CLOCKS_PER_SEC, r2,
           (double)scalar_time / sse_time);

    start = clock();
    volatile float r3 = dot_avx_fma(a, b, N);
    clock_t avx_time = clock() - start;
    printf("AVX+FMA:  %ld ms (result=%.6f, %.2fx)\n",
           avx_time * 1000 / CLOCKS_PER_SEC, r3,
           (double)scalar_time / avx_time);

    free(a);
    free(b);
}

/**
 * @brief 矩阵乘法 - SIMD优化
 *
 * C = A * B，其中A是MxK，B是KxN，C是MxN
 */
#define MATRIX_SIZE 1024

void matmul_scalar(float *C, const float *A, const float *B, int M, int N, int K) {
    memset(C, 0, M * N * sizeof(float));
    for (int i = 0; i < M; i++) {
        for (int k = 0; k < K; k++) {
            for (int j = 0; j < N; j++) {
                C[i * N + j] += A[i * K + k] * B[k * N + j];
            }
        }
    }
}

/* 分块矩阵乘法 + SIMD */
#define BLOCK_SIZE 64
void matmul_blocked_simd(float *C, const float *A, const float *B, int M, int N, int K) {
    memset(C, 0, M * N * sizeof(float));

    for (int ii = 0; ii < M; ii += BLOCK_SIZE) {
        for (int kk = 0; kk < K; kk += BLOCK_SIZE) {
            for (int jj = 0; jj < N; jj += BLOCK_SIZE) {
                /* 处理一个块 */
                int i_max = (ii + BLOCK_SIZE < M) ? ii + BLOCK_SIZE : M;
                int k_max = (kk + BLOCK_SIZE < K) ? kk + BLOCK_SIZE : K;
                int j_max = (jj + BLOCK_SIZE < N) ? jj + BLOCK_SIZE : N;

                for (int i = ii; i < i_max; i++) {
                    for (int k = kk; k < k_max; k++) {
                        float a_ik = A[i * K + k];
                        __m256 a_vec = _mm256_set1_ps(a_ik);  /* 广播 */

                        int j = jj;
                        /* SIMD处理8个元素 */
                        for (; j <= j_max - 8; j += 8) {
                            __m256 c_vec = _mm256_loadu_ps(&C[i * N + j]);
                            __m256 b_vec = _mm256_loadu_ps(&B[k * N + j]);
                            c_vec = _mm256_fmadd_ps(a_vec, b_vec, c_vec);
                            _mm256_storeu_ps(&C[i * N + j], c_vec);
                        }
                        /* 剩余元素 */
                        for (; j < j_max; j++) {
                            C[i * N + j] += a_ik * B[k * N + j];
                        }
                    }
                }
            }
        }
    }
}

void benchmark_matrix_mul(void) {
    printf("\n=== Matrix Multiplication Benchmark (%dx%d) ===\n\n",
           MATRIX_SIZE, MATRIX_SIZE);

    int n = MATRIX_SIZE;
    float *A = aligned_alloc(32, n * n * sizeof(float));
    float *B = aligned_alloc(32, n * n * sizeof(float));
    float *C = aligned_alloc(32, n * n * sizeof(float));

    /* 初始化 */
    for (int i = 0; i < n * n; i++) {
        A[i] = (float)(i % 10) / 10.0f;
        B[i] = (float)((i * 7) % 10) / 10.0f;
    }

    /* 标量版本 - 可能较慢，减少迭代 */
    printf("Running scalar version...\n");
    clock_t start = clock();
    matmul_scalar(C, A, B, n, n, n);
    clock_t scalar_time = clock() - start;
    printf("Scalar:      %ld ms\n", scalar_time * 1000 / CLOCKS_PER_SEC);

    /* SIMD版本 */
    printf("Running SIMD version...\n");
    start = clock();
    matmul_blocked_simd(C, A, B, n, n, n);
    clock_t simd_time = clock() - start;
    printf("SIMD(Block): %ld ms (%.2fx speedup)\n",
           simd_time * 1000 / CLOCKS_PER_SEC,
           (double)scalar_time / simd_time);

    free(A);
    free(B);
    free(C);
}

int main(void) {
    benchmark_vector_add();
    benchmark_dot_product();
    benchmark_matrix_mul();
    return 0;
}
```

### 3.2 ARM NEON优化

```c
/**
 * @file neon_optimization.c
 * @brief ARM NEON SIMD优化指南
 *
 * NEON是ARM架构的SIMD扩展，支持128位向量操作
 * 适用于移动设备、嵌入式系统和ARM服务器
 */

#ifdef __ARM_NEON
#include <arm_neon.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* NEON类型说明：
 * float32x4_t: 4个float的向量
 * int32x4_t: 4个int32的向量
 * uint8x16_t: 16个uint8的向量
 */

#ifdef __ARM_NEON

/**
 * @brief NEON向量加法
 */
void add_neon(float *dst, const float *a, const float *b, int n) {
    int i = 0;
    /* 每次处理4个float（128位） */
    for (; i <= n - 4; i += 4) {
        float32x4_t va = vld1q_f32(&a[i]);  /* 加载4个float */
        float32x4_t vb = vld1q_f32(&b[i]);
        float32x4_t vr = vaddq_f32(va, vb); /* 向量加法 */
        vst1q_f32(&dst[i], vr);              /* 存储结果 */
    }
    /* 剩余元素 */
    for (; i < n; i++) {
        dst[i] = a[i] + b[i];
    }
}

/**
 * @brief NEON点积 - 使用乘累加
 */
float dot_neon(const float *a, const float *b, int n) {
    float32x4_t sum4 = vdupq_n_f32(0.0f);  /* 初始化为0 */
    int i = 0;

    for (; i <= n - 4; i += 4) {
        float32x4_t va = vld1q_f32(&a[i]);
        float32x4_t vb = vld1q_f32(&b[i]);
        /* 乘累加: sum += a * b */
        sum4 = vfmaq_f32(sum4, va, vb);
    }

    /* 水平求和 */
    float32x2_t sum2 = vadd_f32(vget_low_f32(sum4), vget_high_f32(sum4));
    float32x2_t sum1 = vpadd_f32(sum2, sum2);
    float sum = vget_lane_f32(sum1, 0);

    for (; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

/**
 * @brief NEON图像处理 - RGB转灰度
 *
 * 灰度 = 0.299*R + 0.587*G + 0.114*B
 * NEON可同时处理多个像素
 */
void rgb_to_gray_neon(const uint8_t *rgb, uint8_t *gray, int num_pixels) {
    /* 系数向量 */
    uint8x8_t r_coeff = vdup_n_u8(77);   /* 0.299 * 256 */
    uint8x8_t g_coeff = vdup_n_u8(150);  /* 0.587 * 256 */
    uint8x8_t b_coeff = vdup_n_u8(29);   /* 0.114 * 256 */

    int i = 0;
    for (; i <= num_pixels - 8; i += 8) {
        /* 加载24字节（8个RGB像素） */
        uint8x8x3_t rgb_vec = vld3_u8(&rgb[i * 3]);

        /* 分别提取R, G, B通道 */
        uint8x8_t r = rgb_vec.val[0];
        uint8x8_t g = rgb_vec.val[1];
        uint8x8_t b = rgb_vec.val[2];

        /* 计算加权和 */
        uint16x8_t sum = vmull_u8(r, r_coeff);
        sum = vmlal_u8(sum, g, g_coeff);
        sum = vmlal_u8(sum, b, b_coeff);

        /* 右移8位（相当于除以256）并缩小到8位 */
        uint8x8_t gray_vec = vshrn_n_u16(sum, 8);

        /* 存储8个灰度值 */
        vst1_u8(&gray[i], gray_vec);
    }

    /* 剩余像素 */
    for (; i < num_pixels; i++) {
        gray[i] = (77 * rgb[i*3] + 150 * rgb[i*3+1] + 29 * rgb[i*3+2]) >> 8;
    }
}

/**
 * @brief NEON内存拷贝 - 使用宽向量加载/存储
 */
void memcpy_neon(void *dst, const void *src, size_t n) {
    uint8_t *d = dst;
    const uint8_t *s = src;

    size_t i = 0;
    /* 每次拷贝64字节（4个128位寄存器） */
    for (; i <= n - 64; i += 64) {
        uint8x16_t v0 = vld1q_u8(&s[i]);
        uint8x16_t v1 = vld1q_u8(&s[i + 16]);
        uint8x16_t v2 = vld1q_u8(&s[i + 32]);
        uint8x16_t v3 = vld1q_u8(&s[i + 48]);

        vst1q_u8(&d[i], v0);
        vst1q_u8(&d[i + 16], v1);
        vst1q_u8(&d[i + 32], v2);
        vst1q_u8(&d[i + 48], v3);
    }
    /* 剩余字节 */
    for (; i < n; i++) {
        d[i] = s[i];
    }
}

#endif /* __ARM_NEON */

/* 标量参考实现 */
void rgb_to_gray_scalar(const uint8_t *rgb, uint8_t *gray, int num_pixels) {
    for (int i = 0; i < num_pixels; i++) {
        uint16_t r = rgb[i * 3];
        uint16_t g = rgb[i * 3 + 1];
        uint16_t b = rgb[i * 3 + 2];
        gray[i] = (uint8_t)((77 * r + 150 * g + 29 * b) >> 8);
    }
}

void benchmark_neon(void) {
    printf("=== ARM NEON Benchmark ===\n\n");

#ifdef __ARM_NEON
    printf("NEON is available on this platform\n\n");

    /* RGB转灰度测试 */
    const int NUM_PIXELS = 1920 * 1080;  /* 1080p图像 */
    uint8_t *rgb = malloc(NUM_PIXELS * 3);
    uint8_t *gray_scalar = malloc(NUM_PIXELS);
    uint8_t *gray_neon = malloc(NUM_PIXELS);

    /* 填充测试数据 */
    for (int i = 0; i < NUM_PIXELS * 3; i++) {
        rgb[i] = (uint8_t)(i % 256);
    }

    /* 标量版本 */
    clock_t start = clock();
    rgb_to_gray_scalar(rgb, gray_scalar, NUM_PIXELS);
    clock_t scalar_time = clock() - start;
    printf("RGB->Gray Scalar: %ld ms\n", scalar_time * 1000 / CLOCKS_PER_SEC);

    /* NEON版本 */
    start = clock();
    rgb_to_gray_neon(rgb, gray_neon, NUM_PIXELS);
    clock_t neon_time = clock() - start;
    printf("RGB->Gray NEON:   %ld ms (%.2fx speedup)\n",
           neon_time * 1000 / CLOCKS_PER_SEC,
           (double)scalar_time / neon_time);

    /* 验证结果 */
    int diff = 0;
    for (int i = 0; i < NUM_PIXELS; i++) {
        if (abs((int)gray_scalar[i] - (int)gray_neon[i]) > 1) {
            diff++;
        }
    }
    printf("Differences: %d\n", diff);

    free(rgb);
    free(gray_scalar);
    free(gray_neon);
#else
    printf("NEON not available on this platform (x86/x64)\n");
    printf("This code demonstrates NEON syntax for ARM platforms\n");
#endif
}

int main(void) {
    benchmark_neon();
    return 0;
}
```

### 3.3 SIMD汇编级优化

```asm
; @file simd_optimized.asm
; @brief 手写汇编SIMD优化示例
; 使用NASM语法

section .data
    align 32
    coeff_0_5 dd 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5

section .text
    global dot_product_avx_asm
    global saxpy_avx_asm

; float dot_product_avx_asm(const float *a, const float *b, int n)
; 参数: RDI=a, RSI=b, RDX=n
; 返回: XMM0=result
dot_product_avx_asm:
    vxorps      ymm0, ymm0, ymm0        ; sum = 0 (8 floats)
    vxorps      ymm1, ymm1, ymm1        ; 额外累加器，打破依赖

    mov         rax, rdx
    shr         rax, 4                  ; n / 16 (每次处理16个float)
    test        rax, rax
    jz          .remainder

.loop16:
    ; 加载16个float (2个ymm寄存器)
    vmovups     ymm2, [rdi]             ; a[0..7]
    vmovups     ymm3, [rdi + 32]        ; a[8..15]
    vmovups     ymm4, [rsi]             ; b[0..7]
    vmovups     ymm5, [rsi + 32]        ; b[8..15]

    ; FMA: sum += a * b
    vfmadd231ps ymm0, ymm2, ymm4        ; ymm0 += ymm2 * ymm4
    vfmadd231ps ymm1, ymm3, ymm5        ; ymm1 += ymm3 * ymm5

    add         rdi, 64                 ; a += 16
    add         rsi, 64                 ; b += 16
    dec         rax
    jnz         .loop16

    vaddps      ymm0, ymm0, ymm1        ; 合并两个累加器

.remainder:
    ; 处理剩余的 (n % 16) 个元素
    mov         rax, rdx
    and         rax, 15                 ; n % 16
    test        rax, rax
    jz          .reduce

.loop1:
    vmulss      xmm2, xmm2, xmm2        ; 标量乘加
    vaddss      xmm0, xmm0, xmm2
    add         rdi, 4
    add         rsi, 4
    dec         rax
    jnz         .loop1

.reduce:
    ; 水平求和 ymm0 -> xmm0 -> 标量
    vextractf128 xmm1, ymm0, 1          ; 提取高128位
    vaddps      xmm0, xmm0, xmm1        ; 128位向量相加
    vhaddps     xmm0, xmm0, xmm0        ; 水平相加
    vhaddps     xmm0, xmm0, xmm0        ; 再次水平相加

    vzeroupper                          ; 清理YMM状态
    ret

; void saxpy_avx_asm(float *y, const float *x, float a, int n)
; 参数: RDI=y, RSI=x, XMM2=a, RDX=n
; 计算: y[i] = a * x[i] + y[i]
saxpy_avx_asm:
    vbroadcastss ymm2, xmm2             ; 广播a到8个元素

    mov         rax, rdx
    shr         rax, 3                  ; n / 8
    test        rax, rax
    jz          .remainder

.loop8:
    vmovups     ymm0, [rsi]             ; 加载x[0..7]
    vmovups     ymm1, [rdi]             ; 加载y[0..7]
    vfmadd231ps ymm1, ymm0, ymm2        ; y = x * a + y
    vmovups     [rdi], ymm1             ; 存储结果

    add         rsi, 32
    add         rdi, 32
    dec         rax
    jnz         .loop8

.remainder:
    mov         rax, rdx
    and         rax, 7
    test        rax, rax
    jz          .done

.loop1:
    vmovss      xmm0, [rsi]
    vmovss      xmm1, [rdi]
    vfmadd231ss xmm1, xmm0, xmm2
    vmovss      [rdi], xmm1
    add         rsi, 4
    add         rdi, 4
    dec         rax
    jnz         .loop1

.done:
    vzeroupper
    ret
```

---

## 4. 编译器优化内部

### 4.1 GCC/Clang优化Pass

```c
/**
 * @file compiler_optimization.c
 * @brief 编译器优化技术详解
 *
 * 主要优化Pass：
 * - 内联展开 (Inlining)
 * - 常量传播 (Constant Propagation)
 * - 死代码消除 (Dead Code Elimination)
 * - 循环优化 (Loop Unrolling, Loop Invariant Code Motion)
 * - 向量化 (Auto-vectorization)
 * - 链接时优化 (LTO)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* 优化属性演示 */

/* 强制内联 */
static inline __attribute__((always_inline))
int add_inline(int a, int b) {
    return a + b;
}

/* 禁止内联 */
__attribute__((noinline))
int add_no_inline(int a, int b) {
    return a + b;
}

/* 纯函数 - 无副作用，结果仅依赖参数 */
__attribute__((pure))
int square_pure(int x) {
    return x * x;
}

/* 常量函数 - 纯函数 + 不访问内存 */
__attribute__((const))
int add_const(int a, int b) {
    return a + b;
}

/* 热点函数优化建议 */
__attribute__((hot))
void frequently_called(void) {
    /* 编译器会将此函数放在代码段的hot区域 */
}

__attribute__((cold))
void rarely_called(void) {
    /* 编译器会将此函数放在代码段的cold区域 */
}

/* 对齐建议 */
__attribute__((aligned(64)))
int aligned_array[100];

/* 分支预测提示 */
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

void branch_hints_demo(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        /* 假设错误条件是罕见的 */
        if (unlikely(arr[i] < 0)) {
            printf("Error: negative value at %d\n", i);
            continue;
        }
        /* 正常处理 */
        arr[i] *= 2;
    }
}

/**
 * @brief 循环优化演示
 */

/* 普通循环 */
int sum_normal(int *arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

/* 手动展开循环 - 帮助编译器优化 */
int sum_unrolled(int *arr, int n) {
    int sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    int i = 0;

    /* 每次处理4个元素 */
    for (; i <= n - 4; i += 4) {
        sum0 += arr[i];
        sum1 += arr[i + 1];
        sum2 += arr[i + 2];
        sum3 += arr[i + 3];
    }

    /* 剩余元素 */
    for (; i < n; i++) {
        sum0 += arr[i];
    }

    return sum0 + sum1 + sum2 + sum3;
}

/* 循环不变量外提 */
int loop_invariant_demo(int *arr, int n, int multiplier) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        /* multiplier * 2 是循环不变量 */
        sum += arr[i] * (multiplier * 2);  /* 编译器会外提 */
    }
    return sum;
}

/* 强度削弱：乘除法转移位 */
int strength_reduction(int x) {
    /* 编译器会将这些优化为移位 */
    return (x * 8) + (x / 4) + (x % 2);
}

/**
 * @brief 别名分析优化
 *
 * __restrict 关键字告诉编译器指针不会别名
 */

/* 无restrict - 编译器必须保守假设 */
void add_no_restrict(int *a, int *b, int *c, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = b[i] + c[i];
        /* 编译器必须假设a,b,c可能重叠 */
    }
}

/* 有restrict - 允许更多优化 */
void add_restrict(int *__restrict a,
                  const int *__restrict b,
                  const int *__restrict c, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = b[i] + c[i];
        /* 编译器知道b[i]和c[i]不会被a[i]写入影响 */
    }
}

/**
 * @brief 自动向量化示例
 *
 * 编译选项: -O3 -march=native -ftree-vectorize
 * 查看向量化报告: -fopt-info-vec-optimized
 */

/* 可向量化的循环 */
void vectorizable_loop(float *dst, const float *a, const float *b, int n) {
    /* 简单循环，无依赖，连续访问 */
    for (int i = 0; i < n; i++) {
        dst[i] = a[i] + b[i];
    }
}

/* 不可向量化的循环 - 数据依赖 */
void non_vectorizable_loop(float *arr, int n) {
    for (int i = 1; i < n; i++) {
        /* 依赖前一次迭代的结果 */
        arr[i] = arr[i] + arr[i-1];
    }
}

/* 使用OpenMP SIMD指令提示 */
#ifdef _OPENMP
#include <omp.h>
void omp_simd_loop(float *dst, const float *a, const float *b, int n) {
    #pragma omp simd
    for (int i = 0; i < n; i++) {
        dst[i] = a[i] + b[i];
    }
}
#endif

/**
 * @brief LTO（链接时优化）示例
 *
 * 编译选项: -flto (GCC) 或 -flto=thin (Clang)
 *
 * LTO允许跨翻译单元优化：
 * - 跨文件内联
 * - 全局死代码消除
 * - 更好的别名分析
 */

/* 在单独文件中定义，LTO后可内联 */
extern int external_function(int x);

void lto_demo(void) {
    /* 如果没有LTO，无法内联external_function */
    /* 启用LTO后，编译器可以看到定义并内联 */
    int result = 0;
    for (int i = 0; i < 1000000; i++) {
        result += external_function(i);
    }
}

/**
 * @brief Profile-Guided Optimization (PGO)
 *
 * 编译步骤：
 * 1. gcc -fprofile-generate source.c -o program
 * 2. ./program  (运行代表性工作负载)
 * 3. gcc -fprofile-use source.c -o optimized_program
 */

__attribute__((noinline))
int pgo_function(int x) {
    /* PGO后，编译器知道哪些分支更可能执行 */
    if (x < 100) {
        /* 假设95%的情况走这里 */
        return x * 2;
    } else {
        return x * 3;
    }
}

void benchmark_compiler_opts(void) {
    printf("=== Compiler Optimization Demo ===\n\n");

    const int N = 100000000;
    int *arr = malloc(N * sizeof(int));

    for (int i = 0; i < N; i++) {
        arr[i] = i;
    }

    /* 测试不同优化级别 */
    volatile int sum = 0;

    clock_t start = clock();
    sum = sum_normal(arr, N);
    clock_t t1 = clock() - start;
    printf("Normal sum: %ld ms (result=%d)\n", t1 * 1000 / CLOCKS_PER_SEC, sum);

    start = clock();
    sum = sum_unrolled(arr, N);
    clock_t t2 = clock() - start;
    printf("Unrolled sum: %ld ms (result=%d, %.2fx)\n",
           t2 * 1000 / CLOCKS_PER_SEC, sum, (double)t1 / t2);

    free(arr);
}

int main(void) {
    benchmark_compiler_opts();
    return 0;
}
```

### 4.2 编译器优化报告解读

```bash
#!/bin/bash
# @file optimization_analysis.sh
# @brief 编译器优化分析脚本

# 生成详细的向量化报告
echo "=== Auto-vectorization Report ==="
gcc -O3 -march=native -fopt-info-vec-optimized -c source.c 2>&1 | head -50

# 生成失败的向量化报告
echo -e "\n=== Failed Vectorization ==="
gcc -O3 -march=native -fopt-info-vec-missed -c source.c 2>&1 | head -50

# 生成内联报告
echo -e "\n=== Inlining Report ==="
gcc -O3 -fopt-info-inline-optimized -c source.c 2>&1 | head -30

# 生成完整的优化报告
echo -e "\n=== Full Optimization Report ==="
gcc -O3 -fopt-info-all -c source.c 2>&1 > full_report.txt
echo "Full report saved to full_report.txt"
```

### 4.3 汇编输出分析

```bash
# 查看生成的汇编代码
gcc -O3 -S -fverbose-asm -o output.s source.c

# 查看特定函数的汇编
gcc -O3 -S -o output.s source.c
objdump -d --no-show-raw-insn optimized_binary | grep -A 50 "function_name"
```

---

## 5. 内存优化技术

### 5.1 HugePages大页内存

```c
/**
 * @file hugepages_demo.c
 * @brief HugePages大页内存优化
 *
 * 大页内存优势：
 * - 减少TLB miss（每页更大，同样内存需要更少的页表项）
 * - 减少页表遍历开销
 * - 适用于大内存工作集应用
 *
 * 标准页：4KB，x86 HugePages：2MB 或 1GB
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

/* 检测大页支持 */
void check_hugepage_support(void) {
    printf("=== HugePage Support ===\n\n");

    long default_size = sysconf(_SC_PAGESIZE);
    printf("Default page size: %ld KB\n", default_size / 1024);

    /* 检查/proc/meminfo中的大页信息 */
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, "HugePages") || strstr(line, "Hugepagesize")) {
                printf("%s", line);
            }
        }
        fclose(fp);
    }
}

/**
 * @brief 使用mmap分配大页内存
 */
void* allocate_hugepage(size_t size) {
    /* MAP_HUGETLB: 请求2MB大页 */
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);

    if (ptr == MAP_FAILED) {
        perror("mmap hugepage failed");
        return NULL;
    }

    return ptr;
}

/**
 * @brief 透明大页(THP)分配
 *
 * Linux透明大页自动将普通分配合并为大页
 * 通过madvise提示内核使用THP
 */
void* allocate_thp(size_t size) {
    /* 标准分配 */
    void *ptr = aligned_alloc(2 * 1024 * 1024, size);  /* 2MB对齐 */
    if (!ptr) return NULL;

    /* 提示内核使用透明大页 */
    madvise(ptr, size, MADV_HUGEPAGE);

    return ptr;
}

/**
 * @brief 内存访问性能测试
 */
void memory_access_test(void *memory, size_t size, const char *label) {
    volatile long long sum = 0;
    int *arr = (int*)memory;
    int num_elements = size / sizeof(int);

    /* 初始化 */
    for (int i = 0; i < num_elements; i++) {
        arr[i] = i;
    }

    /* 顺序访问测试 */
    clock_t start = clock();
    for (int iter = 0; iter < 10; iter++) {
        for (int i = 0; i < num_elements; i++) {
            sum += arr[i];
        }
    }
    clock_t end = clock();

    double ms = (end - start) * 1000.0 / CLOCKS_PER_SEC;
    double bandwidth = (10.0 * size / (1024 * 1024)) / (ms / 1000.0);

    printf("%s: %.2f ms, %.2f MB/s (sum=%lld)\n",
           label, ms, bandwidth, sum);
}

void benchmark_hugepages(void) {
    printf("\n=== HugePage Performance Test ===\n\n");

    const size_t SIZE = 512 * 1024 * 1024;  /* 512MB */

    /* 标准内存 */
    void *normal = aligned_alloc(4096, SIZE);
    if (normal) {
        memory_access_test(normal, SIZE, "Normal pages (4KB)");
        free(normal);
    }

    /* 透明大页 */
    void *thp = allocate_thp(SIZE);
    if (thp) {
        memory_access_test(thp, SIZE, "Transparent HugePage");
        free(thp);
    }

    /* 显式大页 - 需要root权限和预留 */
    void *huge = allocate_hugepage(SIZE);
    if (huge) {
        memory_access_test(huge, SIZE, "Explicit HugePage (2MB)");
        munmap(huge, SIZE);
    }
}

/**
 * @brief TLB性能分析
 *
 * TLB (Translation Lookaside Buffer)缓存虚拟到物理地址映射
 * 大页减少TLB条目数量，提高TLB命中率
 */
void tlb_analysis(void) {
    printf("\n=== TLB Analysis ===\n\n");

    /* 计算不同页大小的TLB条目需求 */
    size_t memory_size = 1 * 1024 * 1024 * 1024;  /* 1GB */

    printf("Memory size: 1GB\n");
    printf("4KB pages:   %zu TLB entries needed\n", memory_size / (4 * 1024));
    printf("2MB pages:   %zu TLB entries needed\n", memory_size / (2 * 1024 * 1024));
    printf("1GB pages:   %zu TLB entries needed\n", memory_size / (1024 * 1024 * 1024));

    printf("\nTypical TLB sizes:\n");
    printf("L1 dTLB: 64 entries (4KB), 32 entries (2MB)\n");
    printf("L2 sTLB: 1024+ entries\n");
    printf("\nWith 4KB pages and 1GB memory: %zu TLB misses expected\n",
           memory_size / (4 * 1024) / 64);
    printf("With 2MB pages and 1GB memory: %zu TLB misses expected\n",
           memory_size / (2 * 1024 * 1024) / 32);
}

int main(void) {
    check_hugepage_support();
    benchmark_hugepages();
    tlb_analysis();
    return 0;
}
```

### 5.2 NUMA内存优化

```c
/**
 * @file numa_optimization.c
 * @brief NUMA (Non-Uniform Memory Access) 优化
 *
 * NUMA架构特点：
 * - 每个CPU有本地内存，访问更快
 * - 访问远程内存需要跨互连，延迟更高
 * - 线程应使用本地节点的内存
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <numa.h>
#include <numaif.h>
#include <sched.h>
#include <unistd.h>

/**
 * @brief 检测NUMA拓扑
 */
void detect_numa_topology(void) {
    printf("=== NUMA Topology ===\n\n");

    if (numa_available() < 0) {
        printf("NUMA not available on this system\n");
        return;
    }

    int num_nodes = numa_num_configured_nodes();
    int num_cpus = numa_num_configured_cpus();

    printf("NUMA nodes: %d\n", num_nodes);
    printf("CPUs: %d\n", num_cpus);

    for (int node = 0; node < num_nodes; node++) {
        long long free_mem, total_mem;
        numa_node_size64(node, &free_mem);
        total_mem = numa_node_size(node, NULL);

        printf("\nNode %d:\n", node);
        printf("  Memory: %.2f GB total, %.2f GB free\n",
               total_mem / (1024.0 * 1024 * 1024),
               free_mem / (1024.0 * 1024 * 1024));

        /* CPU掩码 */
        struct bitmask *cpus = numa_node_to_cpus(node);
        printf("  CPUs: ");
        for (int i = 0; i < num_cpus; i++) {
            if (numa_bitmask_isbitset(cpus, i)) {
                printf("%d ", i);
            }
        }
        printf("\n");
        numa_bitmask_free(cpus);
    }
}

/**
 * @brief NUMA本地 vs 远程内存访问性能
 */
typedef struct {
    int node;
    char *memory;
    size_t size;
    volatile long long sum;
} ThreadArgs;

void* memory_worker(void *arg) {
    ThreadArgs *args = (ThreadArgs*)arg;

    /* 绑定到指定节点 */
    struct bitmask *mask = numa_allocate_nodemask();
    numa_bitmask_setbit(mask, args->node);
    numa_bind(mask);
    numa_bitmask_free(mask);

    int *arr = (int*)args->memory;
    int n = args->size / sizeof(int);
    long long sum = 0;

    /* 多次访问 */
    for (int iter = 0; iter < 10; iter++) {
        for (int i = 0; i < n; i++) {
            sum += arr[i];
        }
    }

    args->sum = sum;
    return NULL;
}

void numa_locality_test(void) {
    printf("\n=== NUMA Locality Test ===\n\n");

    if (numa_available() < 0) {
        printf("NUMA not available\n");
        return;
    }

    int num_nodes = numa_max_node() + 1;
    if (num_nodes < 2) {
        printf("Single NUMA node, skipping test\n");
        return;
    }

    const size_t SIZE = 256 * 1024 * 1024;  /* 256MB */

    /* 在节点0分配内存 */
    char *mem_node0 = numa_alloc_onnode(SIZE, 0);
    for (int i = 0; i < SIZE / sizeof(int); i++) {
        ((int*)mem_node0)[i] = i;
    }

    pthread_t thread;
    ThreadArgs args = {0, mem_node0, SIZE, 0};

    /* 测试1: 节点0线程访问节点0内存（本地） */
    printf("Node 0 thread accessing Node 0 memory (LOCAL):\n");
    args.node = 0;
    clock_t start = clock();
    pthread_create(&thread, NULL, memory_worker, &args);
    pthread_join(thread, NULL);
    clock_t local_time = clock() - start;
    printf("  Time: %ld ms (sum=%lld)\n",
           local_time * 1000 / CLOCKS_PER_SEC, args.sum);

    /* 测试2: 节点1线程访问节点0内存（远程） */
    if (num_nodes > 1) {
        printf("Node 1 thread accessing Node 0 memory (REMOTE):\n");
        args.node = 1;
        args.sum = 0;
        start = clock();
        pthread_create(&thread, NULL, memory_worker, &args);
        pthread_join(thread, NULL);
        clock_t remote_time = clock() - start;
        printf("  Time: %ld ms (sum=%lld)\n",
               remote_time * 1000 / CLOCKS_PER_SEC, args.sum);
        printf("  Remote access penalty: %.2fx\n",
               (double)remote_time / local_time);
    }

    numa_free(mem_node0, SIZE);
}

/**
 * @brief NUMA感知的内存分配策略
 */
void numa_allocation_policies(void) {
    printf("\n=== NUMA Allocation Policies ===\n\n");

    if (numa_available() < 0) return;

    const size_t SIZE = 100 * 1024 * 1024;

    /* 默认策略 - 在本地节点分配 */
    printf("1. Default (local preferred):\n");
    char *p1 = numa_alloc(SIZE);
    printf("   Allocated on node: %d\n", numa_node_of_cpu(sched_getcpu()));
    numa_free(p1, SIZE);

    /* 绑定到特定节点 */
    printf("2. Bind to node 0:\n");
    char *p2 = numa_alloc_onnode(SIZE, 0);
    printf("   Explicitly allocated on node 0\n");
    numa_free(p2, SIZE);

    /* 交错分配 - 在所有节点间轮询 */
    printf("3. Interleaved (all nodes):\n");
    char *p3 = numa_alloc_interleaved(SIZE);
    printf("   Pages distributed across all nodes\n");
    numa_free(p3, SIZE);
}

int main(void) {
    detect_numa_topology();
    numa_locality_test();
    numa_allocation_policies();
    return 0;
}
```

### 5.3 内存池与自定义分配器

```c
/**
 * @file memory_pool.c
 * @brief 高性能内存池实现
 *
 * 内存池优势：
 * - 减少malloc/free系统调用开销
 * - 减少内存碎片
 * - 提高缓存局部性
 * - 避免锁竞争（线程本地池）
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdalign.h>
#include <time.h>
#include <pthread.h>

#define CACHE_LINE_SIZE 64
#define POOL_BLOCK_SIZE 64              /* 每个块的大小 */
#define POOL_BLOCKS_PER_CHUNK 1024      /* 每批分配的块数 */

/**
 * @brief 简单内存池
 */
typedef struct PoolBlock {
    struct PoolBlock *next;
} PoolBlock;

typedef struct {
    PoolBlock *free_list;               /* 空闲块链表 */
    char *chunk_base;                   /* 当前chunk基址 */
    size_t chunk_offset;                /* 当前chunk偏移 */
    size_t chunk_size;                  /* chunk大小 */
    pthread_mutex_t lock;               /* 线程安全锁 */
} MemoryPool;

/* 初始化内存池 */
void pool_init(MemoryPool *pool) {
    pool->free_list = NULL;
    pool->chunk_base = NULL;
    pool->chunk_offset = 0;
    pool->chunk_size = POOL_BLOCK_SIZE * POOL_BLOCKS_PER_CHUNK;
    pthread_mutex_init(&pool->lock, NULL);
}

/* 分配新chunk */
static int pool_alloc_chunk(MemoryPool *pool) {
    pool->chunk_base = aligned_alloc(CACHE_LINE_SIZE, pool->chunk_size);
    if (!pool->chunk_base) return -1;

    pool->chunk_offset = 0;

    /* 将新chunk分割成块，加入空闲链表 */
    for (int i = 0; i < POOL_BLOCKS_PER_CHUNK; i++) {
        PoolBlock *block = (PoolBlock*)(pool->chunk_base + i * POOL_BLOCK_SIZE);
        block->next = pool->free_list;
        pool->free_list = block;
    }

    return 0;
}

/* 从池分配 */
void* pool_alloc(MemoryPool *pool) {
    pthread_mutex_lock(&pool->lock);

    if (!pool->free_list) {
        if (pool_alloc_chunk(pool) < 0) {
            pthread_mutex_unlock(&pool->lock);
            return NULL;
        }
    }

    PoolBlock *block = pool->free_list;
    pool->free_list = block->next;

    pthread_mutex_unlock(&pool->lock);

    /* 清空块（调试用） */
    memset(block, 0, POOL_BLOCK_SIZE);
    return block;
}

/* 归还到池 */
void pool_free(MemoryPool *pool, void *ptr) {
    if (!ptr) return;

    PoolBlock *block = (PoolBlock*)ptr;

    pthread_mutex_lock(&pool->lock);
    block->next = pool->free_list;
    pool->free_list = block;
    pthread_mutex_unlock(&pool->lock);
}

/* 销毁池 */
void pool_destroy(MemoryPool *pool) {
    /* 注意：这里简化处理，实际需要跟踪所有chunk */
    pthread_mutex_destroy(&pool->lock);
}

/**
 * @brief 线程本地内存池（无锁）
 */
__thread MemoryPool *tls_pool = NULL;

void* tls_pool_alloc(void) {
    if (!tls_pool) {
        tls_pool = malloc(sizeof(MemoryPool));
        pool_init(tls_pool);
    }
    return pool_alloc(tls_pool);
}

void tls_pool_free(void *ptr) {
    if (tls_pool) {
        pool_free(tls_pool, ptr);
    }
}

/**
 * @brief 性能基准测试
 */
#define NUM_ALLOCATIONS 10000000
#define NUM_THREADS 4

void* malloc_bench(void *arg) {
    void **ptrs = malloc(NUM_ALLOCATIONS * sizeof(void*));

    clock_t start = clock();
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        ptrs[i] = malloc(POOL_BLOCK_SIZE);
    }
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        free(ptrs[i]);
    }
    clock_t end = clock();

    free(ptrs);
    return (void*)(size_t)(end - start);
}

void* pool_bench(void *arg) {
    MemoryPool pool;
    pool_init(&pool);

    void **ptrs = malloc(NUM_ALLOCATIONS * sizeof(void*));

    clock_t start = clock();
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        ptrs[i] = pool_alloc(&pool);
    }
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        pool_free(&pool, ptrs[i]);
    }
    clock_t end = clock();

    free(ptrs);
    pool_destroy(&pool);
    return (void*)(size_t)(end - start);
}

void benchmark_memory_pool(void) {
    printf("=== Memory Pool Benchmark ===\n\n");

    /* 单线程测试 */
    printf("Single-threaded (%d allocations of %d bytes):\n",
           NUM_ALLOCATIONS, POOL_BLOCK_SIZE);

    clock_t malloc_time = (clock_t)(size_t)malloc_bench(NULL);
    printf("  malloc/free: %ld ms\n", malloc_time * 1000 / CLOCKS_PER_SEC);

    clock_t pool_time = (clock_t)(size_t)pool_bench(NULL);
    printf("  Memory pool: %ld ms (%.2fx speedup)\n",
           pool_time * 1000 / CLOCKS_PER_SEC,
           (double)malloc_time / pool_time);

    /* 多线程测试 */
    printf("\nMulti-threaded (%d threads):\n", NUM_THREADS);
    pthread_t threads[NUM_THREADS];

    /* malloc多线程 */
    clock_t start = clock();
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, malloc_bench, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_t mt_malloc_time = clock() - start;
    printf("  malloc/free: %ld ms\n", mt_malloc_time * 1000 / CLOCKS_PER_SEC);

    /* 每个线程自己的pool（模拟TLS池） */
    start = clock();
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, pool_bench, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_t mt_pool_time = clock() - start;
    printf("  Memory pool: %ld ms (%.2fx speedup)\n",
           mt_pool_time * 1000 / CLOCKS_PER_SEC,
           (double)mt_malloc_time / mt_pool_time);
}

int main(void) {
    benchmark_memory_pool();
    return 0;
}
```

---

## 6. I/O优化技术

### 6.1 零拷贝技术

```c
/**
 * @file zero_copy.c
 * @brief 零拷贝I/O优化技术
 *
 * 传统I/O: 磁盘 -> 内核页缓存 -> 用户缓冲区 -> 内核socket缓冲区 -> 网卡
 * 零拷贝:   磁盘 -> 内核页缓存 -> 网卡 (通过sendfile)
 *
 * 减少数据复制次数，降低CPU使用和内存带宽
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <errno.h>

#define FILE_SIZE (100 * 1024 * 1024)  /* 100MB */

/**
 * @brief 传统文件复制（read/write）
 */
long traditional_copy(int src_fd, int dst_fd, size_t size) {
    char *buffer = malloc(64 * 1024);  /* 64KB缓冲区 */
    if (!buffer) return -1;

    ssize_t total = 0;
    while (total < (ssize_t)size) {
        ssize_t n = read(src_fd, buffer, 64 * 1024);
        if (n <= 0) break;

        ssize_t written = write(dst_fd, buffer, n);
        if (written < 0) break;

        total += written;
    }

    free(buffer);
    return total;
}

/**
 * @brief 零拷贝文件传输（sendfile）
 */
long zero_copy_sendfile(int src_fd, int dst_fd, size_t size) {
    off_t offset = 0;
    ssize_t total = 0;

    while (total < (ssize_t)size) {
        ssize_t n = sendfile(dst_fd, src_fd, &offset, size - total);
        if (n <= 0) {
            if (errno == EINTR) continue;
            break;
        }
        total += n;
    }

    return total;
}

/**
 * @brief 使用mmap的零拷贝
 *
 * 文件直接映射到进程地址空间，无需read/write
 */
long mmap_copy(int src_fd, int dst_fd, size_t size) {
    void *src = mmap(NULL, size, PROT_READ, MAP_PRIVATE, src_fd, 0);
    if (src == MAP_FAILED) return -1;

    /* 扩展目标文件 */
    ftruncate(dst_fd, size);

    void *dst = mmap(NULL, size, PROT_WRITE, MAP_SHARED, dst_fd, 0);
    if (dst == MAP_FAILED) {
        munmap(src, size);
        return -1;
    }

    /* 直接内存复制（单拷贝） */
    memcpy(dst, src, size);

    munmap(src, size);
    munmap(dst, size);

    return size;
}

/**
 * @brief 使用splice的管道零拷贝
 *
 * splice: 在内核地址空间之间移动数据，无需用户空间参与
 */
long splice_copy(int src_fd, int dst_fd, size_t size) {
    int pipefd[2];
    if (pipe(pipefd) < 0) return -1;

    ssize_t total = 0;
    while (total < (ssize_t)size) {
        /* 从文件到管道 */
        ssize_t n = splice(src_fd, NULL, pipefd[1], NULL,
                          64 * 1024, SPLICE_F_MOVE);
        if (n <= 0) break;

        /* 从管道到目标 */
        ssize_t m = splice(pipefd[0], NULL, dst_fd, NULL, n, SPLICE_F_MOVE);
        if (m <= 0) break;

        total += m;
    }

    close(pipefd[0]);
    close(pipefd[1]);
    return total;
}

void create_test_file(const char *filename, size_t size) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return;

    /* 快速创建大文件 */
    ftruncate(fd, size);

    /* 写入一些数据 */
    char *buf = malloc(1024 * 1024);
    for (size_t i = 0; i < size; i += 1024 * 1024) {
        memset(buf, 'A' + (i / (1024 * 1024)) % 26, 1024 * 1024);
        write(fd, buf, 1024 * 1024);
    }

    free(buf);
    close(fd);
}

void benchmark_zero_copy(void) {
    printf("=== Zero Copy Benchmark (%d MB file) ===\n\n", FILE_SIZE / (1024*1024));

    const char *src_file = "/tmp/zero_copy_src.dat";
    const char *dst_file = "/tmp/zero_copy_dst.dat";

    /* 创建源文件 */
    printf("Creating test file...\n");
    create_test_file(src_file, FILE_SIZE);

    struct stat st;
    stat(src_file, &st);
    size_t file_size = st.st_size;

    /* 预热磁盘缓存 */
    int fd = open(src_file, O_RDONLY);
    char *tmp = malloc(file_size);
    read(fd, tmp, file_size);
    close(fd);
    free(tmp);

    /* 测试1: 传统复制 */
    printf("\n1. Traditional read/write:\n");
    fd = open(src_file, O_RDONLY);
    int dst_fd = open(dst_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    clock_t start = clock();
    traditional_copy(fd, dst_fd, file_size);
    clock_t trad_time = clock() - start;
    close(fd);
    close(dst_fd);
    printf("   Time: %ld ms (%.2f MB/s)\n",
           trad_time * 1000 / CLOCKS_PER_SEC,
           (file_size / (1024.0 * 1024)) / (trad_time / (double)CLOCKS_PER_SEC));

    /* 测试2: sendfile零拷贝 */
    printf("\n2. Zero-copy sendfile:\n");
    fd = open(src_file, O_RDONLY);
    dst_fd = open(dst_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    start = clock();
    zero_copy_sendfile(fd, dst_fd, file_size);
    clock_t sendfile_time = clock() - start;
    close(fd);
    close(dst_fd);
    printf("   Time: %ld ms (%.2f MB/s, %.2fx speedup)\n",
           sendfile_time * 1000 / CLOCKS_PER_SEC,
           (file_size / (1024.0 * 1024)) / (sendfile_time / (double)CLOCKS_PER_SEC),
           (double)trad_time / sendfile_time);

    /* 测试3: mmap复制 */
    printf("\n3. mmap copy:\n");
    fd = open(src_file, O_RDONLY);
    dst_fd = open(dst_file, O_RDWR | O_CREAT | O_TRUNC, 0644);
    start = clock();
    mmap_copy(fd, dst_fd, file_size);
    clock_t mmap_time = clock() - start;
    close(fd);
    close(dst_fd);
    printf("   Time: %ld ms (%.2f MB/s, %.2fx speedup)\n",
           mmap_time * 1000 / CLOCKS_PER_SEC,
           (file_size / (1024.0 * 1024)) / (mmap_time / (double)CLOCKS_PER_SEC),
           (double)trad_time / mmap_time);

    /* 清理 */
    unlink(src_file);
    unlink(dst_file);
}

int main(void) {
    benchmark_zero_copy();
    return 0;
}
```

### 6.2 异步I/O与io_uring

```c
/**
 * @file async_io.c
 * @brief 异步I/O与io_uring高性能实现
 *
 * io_uring: Linux 5.1+引入的高性能异步I/O接口
 * 优势：
 * - 无需系统调用提交请求（通过共享内存ring buffer）
 * - 支持批处理
 * - 低延迟、高吞吐
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <liburing.h>  /* 需要liburing库 */
#include <aio.h>       /* POSIX AIO */
#include <errno.h>

#define NUM_OPS 10000
#define BLOCK_SIZE 4096
#define QUEUE_DEPTH 256

/**
 * @brief 同步I/O基准
 */
long sync_io_bench(const char *filename) {
    int fd = open(filename, O_RDONLY | O_DIRECT);
    if (fd < 0) return -1;

    void *buffer;
    posix_memalign(&buffer, 4096, BLOCK_SIZE);

    clock_t start = clock();
    off_t offset = 0;

    for (int i = 0; i < NUM_OPS; i++) {
        pread(fd, buffer, BLOCK_SIZE, offset);
        offset += BLOCK_SIZE;
    }

    clock_t elapsed = clock() - start;

    free(buffer);
    close(fd);
    return elapsed;
}

/**
 * @brief POSIX AIO基准
 */
long posix_aio_bench(const char *filename) {
    int fd = open(filename, O_RDONLY | O_DIRECT);
    if (fd < 0) return -1;

    struct aiocb *cbs = calloc(NUM_OPS, sizeof(struct aiocb));
    void **buffers = calloc(NUM_OPS, sizeof(void*));

    for (int i = 0; i < NUM_OPS; i++) {
        posix_memalign(&buffers[i], 4096, BLOCK_SIZE);
    }

    clock_t start = clock();

    /* 提交所有请求 */
    for (int i = 0; i < NUM_OPS; i++) {
        memset(&cbs[i], 0, sizeof(struct aiocb));
        cbs[i].aio_fildes = fd;
        cbs[i].aio_buf = buffers[i];
        cbs[i].aio_nbytes = BLOCK_SIZE;
        cbs[i].aio_offset = i * BLOCK_SIZE;

        aio_read(&cbs[i]);
    }

    /* 等待完成 */
    for (int i = 0; i < NUM_OPS; i++) {
        struct aiocb *cb_list[1] = {&cbs[i]};
        aio_suspend(cb_list, 1, NULL);
    }

    clock_t elapsed = clock() - start;

    for (int i = 0; i < NUM_OPS; i++) {
        free(buffers[i]);
    }
    free(buffers);
    free(cbs);
    close(fd);

    return elapsed;
}

/**
 * @brief io_uring基准
 */
long io_uring_bench(const char *filename) {
    int fd = open(filename, O_RDONLY | O_DIRECT);
    if (fd < 0) return -1;

    struct io_uring ring;
    if (io_uring_queue_init(QUEUE_DEPTH, &ring, 0) < 0) {
        close(fd);
        return -1;
    }

    void **buffers = calloc(NUM_OPS, sizeof(void*));
    for (int i = 0; i < NUM_OPS; i++) {
        posix_memalign(&buffers[i], 4096, BLOCK_SIZE);
    }

    clock_t start = clock();

    /* 批量提交请求 */
    int submitted = 0;
    while (submitted < NUM_OPS) {
        int batch = (NUM_OPS - submitted < QUEUE_DEPTH) ?
                    (NUM_OPS - submitted) : QUEUE_DEPTH;

        for (int i = 0; i < batch; i++) {
            struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
            io_uring_prep_read(sqe, fd, buffers[submitted],
                              BLOCK_SIZE, submitted * BLOCK_SIZE);
            submitted++;
        }

        io_uring_submit(&ring);

        /* 等待这批完成 */
        for (int i = 0; i < batch; i++) {
            struct io_uring_cqe *cqe;
            io_uring_wait_cqe(&ring, &cqe);
            io_uring_cqe_seen(&ring, cqe);
        }
    }

    clock_t elapsed = clock() - start;

    for (int i = 0; i < NUM_OPS; i++) {
        free(buffers[i]);
    }
    free(buffers);
    io_uring_queue_exit(&ring);
    close(fd);

    return elapsed;
}

/**
 * @brief io_uring高级用法 - 轮询模式
 *
 * IORING_SETUP_IOPOLL: 内核轮询完成
 * IORING_SETUP_SQPOLL: 内核轮询提交队列
 */
long io_uring_poll_bench(const char *filename) {
    int fd = open(filename, O_RDONLY | O_DIRECT);
    if (fd < 0) return -1;

    struct io_uring ring;
    struct io_uring_params params = {};
    params.flags = IORING_SETUP_IOPOLL;  /* 内核轮询模式 */

    if (io_uring_queue_init_params(QUEUE_DEPTH, &ring, &params) < 0) {
        close(fd);
        return -1;
    }

    void **buffers = calloc(NUM_OPS, sizeof(void*));
    for (int i = 0; i < NUM_OPS; i++) {
        posix_memalign(&buffers[i], 4096, BLOCK_SIZE);
    }

    clock_t start = clock();

    /* 提交并立即轮询完成 */
    int submitted = 0;
    while (submitted < NUM_OPS) {
        struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        io_uring_prep_read(sqe, fd, buffers[submitted],
                          BLOCK_SIZE, submitted * BLOCK_SIZE);
        sqe->user_data = submitted;

        io_uring_submit(&ring);

        /* 轮询等待这个请求完成 */
        struct io_uring_cqe *cqe;
        do {
            io_uring_peek_cqe(&ring, &cqe);
        } while (!cqe);

        io_uring_cqe_seen(&ring, cqe);
        submitted++;
    }

    clock_t elapsed = clock() - start;

    for (int i = 0; i < NUM_OPS; i++) {
        free(buffers[i]);
    }
    free(buffers);
    io_uring_queue_exit(&ring);
    close(fd);

    return elapsed;
}

void create_test_file(const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return;

    /* 创建足够大的文件 */
    ftruncate(fd, (off_t)NUM_OPS * BLOCK_SIZE);
    close(fd);
}

void benchmark_async_io(void) {
    printf("=== Async I/O Benchmark (%d ops) ===\n\n", NUM_OPS);

    const char *filename = "/tmp/async_io_test.dat";
    create_test_file(filename);

    /* 同步I/O */
    printf("1. Synchronous pread:\n");
    long sync_time = sync_io_bench(filename);
    printf("   Time: %ld ms\n", sync_time * 1000 / CLOCKS_PER_SEC);

    /* POSIX AIO */
    printf("\n2. POSIX AIO:\n");
    long aio_time = posix_aio_bench(filename);
    if (aio_time > 0) {
        printf("   Time: %ld ms (%.2fx)\n",
               aio_time * 1000 / CLOCKS_PER_SEC,
               (double)sync_time / aio_time);
    } else {
        printf("   Not supported or failed\n");
    }

    /* io_uring */
    printf("\n3. io_uring:\n");
    long uring_time = io_uring_bench(filename);
    if (uring_time > 0) {
        printf("   Time: %ld ms (%.2fx vs sync)\n",
               uring_time * 1000 / CLOCKS_PER_SEC,
               (double)sync_time / uring_time);
    } else {
        printf("   Not supported (requires liburing)\n");
    }

    unlink(filename);
}

int main(void) {
    benchmark_async_io();
    return 0;
}
```

---

## 7. 并发优化技术

### 7.1 无锁编程

```c
/**
 * @file lock_free.c
 * @brief 无锁数据结构与算法
 *
 * 无锁编程使用原子操作代替锁，避免：
 * - 锁竞争
 * - 线程阻塞/唤醒开销
 * - 优先级反转
 * - 死锁风险
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

/* 内存序说明：
 * memory_order_relaxed: 无顺序约束，仅保证原子性
 * memory_order_acquire: 加载操作，后续操作不会重排序到前面
 * memory_order_release: 存储操作，前面操作不会重排序到后面
 * memory_order_acq_rel: 读写操作都应用acquire/release语义
 * memory_order_seq_cst: 顺序一致性，最强约束
 */

/**
 * @brief 无锁栈（Treiber栈）
 */
typedef struct Node {
    void *data;
    _Atomic(struct Node*) next;
} Node;

typedef struct {
    _Atomic(Node*) top;
} LockFreeStack;

void stack_init(LockFreeStack *stack) {
    atomic_init(&stack->top, NULL);
}

void stack_push(LockFreeStack *stack, void *data) {
    Node *new_node = malloc(sizeof(Node));
    new_node->data = data;

    Node *old_top;
    do {
        old_top = atomic_load_explicit(&stack->top, memory_order_relaxed);
        atomic_store_explicit(&new_node->next, old_top, memory_order_relaxed);
        /* CAS: 如果top仍是old_top，则更新为new_node */
    } while (!atomic_compare_exchange_weak_explicit(
        &stack->top, &old_top, new_node,
        memory_order_release,  /* 成功时使用release */
        memory_order_relaxed   /* 失败时使用relaxed */
    ));
}

void* stack_pop(LockFreeStack *stack) {
    Node *old_top;
    do {
        old_top = atomic_load_explicit(&stack->top, memory_order_acquire);
        if (old_top == NULL) {
            return NULL;  /* 空栈 */
        }
        /* CAS: 如果top仍是old_top，则更新为next */
    } while (!atomic_compare_exchange_weak_explicit(
        &stack->top, &old_top,
        atomic_load_explicit(&old_top->next, memory_order_relaxed),
        memory_order_release,
        memory_order_acquire   /* 失败时重新获取 */
    ));

    void *data = old_top->data;
    /* 注意：这里简化处理，实际需要安全内存回收（如Hazard Pointers）*/
    free(old_top);
    return data;
}

/**
 * @brief 无锁队列（Michael-Scott队列）
 */
typedef struct {
    _Atomic(Node*) head;
    _Atomic(Node*) tail;
} LockFreeQueue;

void queue_init(LockFreeQueue *queue) {
    Node *dummy = malloc(sizeof(Node));
    dummy->data = NULL;
    atomic_init(&dummy->next, NULL);
    atomic_init(&queue->head, dummy);
    atomic_init(&queue->tail, dummy);
}

void queue_enqueue(LockFreeQueue *queue, void *data) {
    Node *new_node = malloc(sizeof(Node));
    new_node->data = data;
    atomic_init(&new_node->next, NULL);

    Node *tail, *next;
    while (1) {
        tail = atomic_load_explicit(&queue->tail, memory_order_acquire);
        next = atomic_load_explicit(&tail->next, memory_order_acquire);

        /* 检查tail是否仍然是tail */
        if (tail == atomic_load_explicit(&queue->tail, memory_order_relaxed)) {
            if (next == NULL) {
                /* 尝试链接新节点 */
                if (atomic_compare_exchange_weak_explicit(
                    &tail->next, &next, new_node,
                    memory_order_release, memory_order_relaxed)) {
                    break;  /* 入队成功 */
                }
            } else {
                /* tail落后，尝试推进 */
                atomic_compare_exchange_weak_explicit(
                    &queue->tail, &tail, next,
                    memory_order_release, memory_order_relaxed);
            }
        }
    }
    /* 尝试推进tail */
    atomic_compare_exchange_weak_explicit(
        &queue->tail, &tail, new_node,
        memory_order_release, memory_order_relaxed);
}

void* queue_dequeue(LockFreeQueue *queue) {
    Node *head, *tail, *next;
    void *data;

    while (1) {
        head = atomic_load_explicit(&queue->head, memory_order_acquire);
        tail = atomic_load_explicit(&queue->tail, memory_order_acquire);
        next = atomic_load_explicit(&head->next, memory_order_acquire);

        if (head == atomic_load_explicit(&queue->head, memory_order_relaxed)) {
            if (head == tail) {
                if (next == NULL) {
                    return NULL;  /* 空队列 */
                }
                /* tail落后，推进它 */
                atomic_compare_exchange_weak_explicit(
                    &queue->tail, &tail, next,
                    memory_order_release, memory_order_relaxed);
            } else {
                data = next->data;
                /* 尝试推进head */
                if (atomic_compare_exchange_weak_explicit(
                    &queue->head, &head, next,
                    memory_order_release, memory_order_relaxed)) {
                    break;
                }
            }
        }
    }

    /* 注意：这里简化处理，实际需要延迟释放 */
    free(head);
    return data;
}

/**
 * @brief 有锁对比实现
 */
typedef struct {
    Node *top;
    pthread_mutex_t lock;
} LockedStack;

void locked_stack_init(LockedStack *stack) {
    stack->top = NULL;
    pthread_mutex_init(&stack->lock, NULL);
}

void locked_stack_push(LockedStack *stack, void *data) {
    Node *new_node = malloc(sizeof(Node));
    new_node->data = data;

    pthread_mutex_lock(&stack->lock);
    new_node->next = stack->top;
    stack->top = new_node;
    pthread_mutex_unlock(&stack->lock);
}

void* locked_stack_pop(LockedStack *stack) {
    pthread_mutex_lock(&stack->lock);
    Node *node = stack->top;
    if (node) {
        stack->top = node->next;
    }
    pthread_mutex_unlock(&stack->lock);

    if (node) {
        void *data = node->data;
        free(node);
        return data;
    }
    return NULL;
}

/**
 * @brief 性能基准测试
 */
#define NUM_THREADS 8
#define OPS_PER_THREAD 1000000

typedef struct {
    void *stack;
    int ops;
    bool use_lock_free;
} ThreadArg;

void* stack_worker(void *arg) {
    ThreadArg *targ = (ThreadArg*)arg;

    for (int i = 0; i < targ->ops; i++) {
        void *data = (void*)(size_t)i;
        if (targ->use_lock_free) {
            stack_push((LockFreeStack*)targ->stack, data);
            stack_pop((LockFreeStack*)targ->stack);
        } else {
            locked_stack_push((LockedStack*)targ->stack, data);
            locked_stack_pop((LockedStack*)targ->stack);
        }
    }
    return NULL;
}

void benchmark_lock_free(void) {
    printf("=== Lock-Free vs Locked Stack Benchmark ===\n\n");
    printf("Threads: %d, Ops per thread: %d\n\n", NUM_THREADS, OPS_PER_THREAD);

    pthread_t threads[NUM_THREADS];
    ThreadArg args[NUM_THREADS];

    /* 测试有锁栈 */
    LockedStack locked_stack;
    locked_stack_init(&locked_stack);

    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].stack = &locked_stack;
        args[i].ops = OPS_PER_THREAD;
        args[i].use_lock_free = false;
    }

    clock_t start = clock();
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, stack_worker, &args[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_t locked_time = clock() - start;

    printf("Locked stack:   %ld ms\n", locked_time * 1000 / CLOCKS_PER_SEC);

    /* 测试无锁栈 */
    LockFreeStack lock_free_stack;
    stack_init(&lock_free_stack);

    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].stack = &lock_free_stack;
        args[i].use_lock_free = true;
    }

    start = clock();
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, stack_worker, &args[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_t lock_free_time = clock() - start;

    printf("Lock-free stack: %ld ms (%.2fx speedup)\n",
           lock_free_time * 1000 / CLOCKS_PER_SEC,
           (double)locked_time / lock_free_time);
}

int main(void) {
    benchmark_lock_free();
    return 0;
}
```

### 7.2 RCU（Read-Copy-Update）

```c
/**
 * @file rcu_demo.c
 * @brief RCU无锁读优化技术
 *
 * RCU特点：
 * - 读者无锁、无原子操作、无内存屏障
 * - 写者复制修改，原子替换指针
 * - 延迟回收旧数据
 *
 * 适用场景：读多写少的数据结构
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

/* 简化版RCU实现 */

static _Atomic(int) rcu_readers_active = 0;
static pthread_mutex_t rcu_write_lock = PTHREAD_MUTEX_INITIALIZER;

#define rcu_read_lock() \
    atomic_fetch_add_explicit(&rcu_readers_active, 1, memory_order_acquire)

#define rcu_read_unlock() \
    atomic_fetch_sub_explicit(&rcu_readers_active, 1, memory_order_release)

#define rcu_synchronize() \
    do { \
        while (atomic_load_explicit(&rcu_readers_active, memory_order_acquire) > 0) \
            ; \
    } while(0)

/**
 * @brief RCU保护的数据结构
 */
typedef struct DataStructure {
    int value;
    char data[1024];
} DataStructure;

static _Atomic(DataStructure*) g_data = NULL;

/* 读者 */
void* reader_thread(void *arg) {
    int iterations = *(int*)arg;
    long long sum = 0;

    for (int i = 0; i < iterations; i++) {
        rcu_read_lock();

        DataStructure *data = atomic_load_explicit(&g_data, memory_order_relaxed);
        if (data) {
            sum += data->value;
        }

        rcu_read_unlock();
    }

    return (void*)(size_t)sum;
}

/* 写者 */
void* writer_thread(void *arg) {
    int iterations = *(int*)arg;

    for (int i = 0; i < iterations; i++) {
        pthread_mutex_lock(&rcu_write_lock);

        /* 读取旧数据 */
        DataStructure *old_data = atomic_load_explicit(&g_data, memory_order_relaxed);

        /* 复制并修改 */
        DataStructure *new_data = malloc(sizeof(DataStructure));
        if (old_data) {
            memcpy(new_data, old_data, sizeof(DataStructure));
        } else {
            memset(new_data, 0, sizeof(DataStructure));
        }
        new_data->value = i;

        /* 原子替换指针 */
        atomic_store_explicit(&g_data, new_data, memory_order_release);

        pthread_mutex_unlock(&rcu_write_lock);

        /* 等待所有读者完成 */
        rcu_synchronize();

        /* 安全释放旧数据 */
        if (old_data) {
            free(old_data);
        }
    }

    return NULL;
}

/**
 * @brief 线程亲和性设置
 */
void set_thread_affinity(pthread_t thread, int cpu) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
}

void benchmark_rcu(void) {
    printf("=== RCU Read-Mostly Benchmark ===\n\n");

    const int NUM_READERS = 7;
    const int NUM_WRITERS = 1;
    const int READ_ITERATIONS = 10000000;
    const int WRITE_ITERATIONS = 10000;

    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];

    /* 初始化数据 */
    DataStructure *initial = calloc(1, sizeof(DataStructure));
    atomic_store(&g_data, initial);

    clock_t start = clock();

    /* 启动读者 */
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, reader_thread,
                      (void*)&READ_ITERATIONS);
    }

    /* 启动写者 */
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_create(&writers[i], NULL, writer_thread,
                      (void*)&WRITE_ITERATIONS);
    }

    /* 等待完成 */
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    clock_t elapsed = clock() - start;

    printf("Total time: %ld ms\n", elapsed * 1000 / CLOCKS_PER_SEC);
    printf("Readers: %d x %d ops = %d reads\n",
           NUM_READERS, READ_ITERATIONS, NUM_READERS * READ_ITERATIONS);
    printf("Writers: %d x %d ops = %d writes\n",
           NUM_WRITERS, WRITE_ITERATIONS, NUM_WRITERS * WRITE_ITERATIONS);
    printf("Read throughput: %.2f M ops/sec\n",
           (NUM_READERS * READ_ITERATIONS / 1000000.0) /
           (elapsed / (double)CLOCKS_PER_SEC));

    /* 清理 */
    DataStructure *final = atomic_load(&g_data);
    free(final);
}

int main(void) {
    benchmark_rcu();
    return 0;
}
```

---


## 8. 算法优化策略

### 8.1 向量化算法

```c
/**
 * @file vectorized_algorithms.c
 * @brief 算法级向量化优化
 *
 * 向量化算法设计原则：
 * - 数据布局：结构体数组(AoS) -> 数组结构体(SoA)
 * - 减少分支：分支预测失败代价高
 * - 合并循环：提高数据重用
 * - 分块处理：适应缓存层次
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <immintrin.h>

/**
 * @brief 数据布局转换：AoS -> SoA
 *
 * AoS (Array of Structs): {x,y,z},{x,y,z}... - 不利于SIMD
 * SoA (Struct of Arrays): xxxx...,yyyy...,zzzz... - 适合SIMD
 */

typedef struct {
    float x, y, z;
} Vec3_AoS;

typedef struct {
    float *x, *y, *z;
} Vec3_SoA;

/* AoS向量加法 */
void add_aos(Vec3_AoS *dst, const Vec3_AoS *a, const Vec3_AoS *b, int n) {
    for (int i = 0; i < n; i++) {
        dst[i].x = a[i].x + b[i].x;
        dst[i].y = a[i].y + b[i].y;
        dst[i].z = a[i].z + b[i].z;
    }
}

/* SoA向量加法 - 可向量化 */
void add_soa(Vec3_SoA *dst, const Vec3_SoA *a, const Vec3_SoA *b, int n) {
    /* x分量 - 可向量化 */
    for (int i = 0; i < n; i++) {
        dst->x[i] = a->x[i] + b->x[i];
    }
    /* y分量 - 可向量化 */
    for (int i = 0; i < n; i++) {
        dst->y[i] = a->y[i] + b->y[i];
    }
    /* z分量 - 可向量化 */
    for (int i = 0; i < n; i++) {
        dst->z[i] = a->z[i] + b->z[i];
    }
}

/* SoA + AVX向量化 */
void add_soa_avx(Vec3_SoA *dst, const Vec3_SoA *a, const Vec3_SoA *b, int n) {
    int i = 0;
    for (; i <= n - 8; i += 8) {
        __m256 ax = _mm256_loadu_ps(&a->x[i]);
        __m256 bx = _mm256_loadu_ps(&b->x[i]);
        _mm256_storeu_ps(&dst->x[i], _mm256_add_ps(ax, bx));

        __m256 ay = _mm256_loadu_ps(&a->y[i]);
        __m256 by = _mm256_loadu_ps(&b->y[i]);
        _mm256_storeu_ps(&dst->y[i], _mm256_add_ps(ay, by));

        __m256 az = _mm256_loadu_ps(&a->z[i]);
        __m256 bz = _mm256_loadu_ps(&b->z[i]);
        _mm256_storeu_ps(&dst->z[i], _mm256_add_ps(az, bz));
    }
    /* 剩余元素 */
    for (; i < n; i++) {
        dst->x[i] = a->x[i] + b->x[i];
        dst->y[i] = a->y[i] + b->y[i];
        dst->z[i] = a->z[i] + b->z[i];
    }
}

/**
 * @brief 向量化查找表
 *
 * 使用 gather 指令从非连续位置加载数据
 */
void lookup_table_simd(const float *table, const int *indices,
                       float *output, int n) {
    int i = 0;
    /* AVX2支持gather，但这里使用AVX模拟 */
    for (; i <= n - 8; i += 8) {
        /* 手动gather（无AVX2时） */
        for (int j = 0; j < 8; j++) {
            output[i + j] = table[indices[i + j]];
        }
    }
    for (; i < n; i++) {
        output[i] = table[indices[i]];
    }
}

/**
 * @brief 向量化前缀和（扫描）
 *
 * 使用Kogge-Stone算法进行SIMD前缀和
 */
void prefix_sum_scalar(float *dst, const float *src, int n) {
    dst[0] = src[0];
    for (int i = 1; i < n; i++) {
        dst[i] = dst[i-1] + src[i];
    }
}

/* SIMD前缀和 - 分块处理 */
void prefix_sum_simd(float *dst, const float *src, int n) {
    const int BLOCK = 1024;  /* 每块大小 */

    /* 块内扫描 */
    for (int block_start = 0; block_start < n; block_start += BLOCK) {
        int block_end = (block_start + BLOCK < n) ? block_start + BLOCK : n;

        /* 标量扫描块内 */
        dst[block_start] = src[block_start];
        for (int i = block_start + 1; i < block_end; i++) {
            dst[i] = dst[i-1] + src[i];
        }
    }

    /* 块间修正（需要二次扫描） */
    float correction = 0;
    for (int block_start = 0; block_start < n; block_start += BLOCK) {
        int block_end = (block_start + BLOCK < n) ? block_start + BLOCK : n;
        float block_sum = dst[block_end - 1];

        for (int i = block_start; i < block_end; i++) {
            dst[i] += correction;
        }
        correction += block_sum;
    }
}

/**
 * @brief 向量化归约操作
 *
 * 使用多层级归约最大化并行
 */
float sum_reduction_scalar(const float *arr, int n) {
    float sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

float sum_reduction_simd(const float *arr, int n) {
    __m256 sum8 = _mm256_setzero_ps();
    int i = 0;

    /* 第一级：8路SIMD归约 */
    for (; i <= n - 8; i += 8) {
        __m256 v = _mm256_loadu_ps(&arr[i]);
        sum8 = _mm256_add_ps(sum8, v);
    }

    /* 第二级：水平归约到4元素 */
    __m128 low = _mm256_castps256_ps128(sum8);
    __m128 high = _mm256_extractf128_ps(sum8, 1);
    __m128 sum4 = _mm_add_ps(low, high);

    /* 第三级：水平归约到标量 */
    sum4 = _mm_hadd_ps(sum4, sum4);
    sum4 = _mm_hadd_ps(sum4, sum4);
    float sum = _mm_cvtss_f32(sum4);

    /* 剩余元素 */
    for (; i < n; i++) {
        sum += arr[i];
    }

    return sum;
}

/**
 * @brief 性能基准测试
 */
#define N (10 * 1024 * 1024)  /* 10M元素 */

void benchmark_vectorized_algorithms(void) {
    printf("=== Vectorized Algorithms Benchmark ===\n\n");

    /* 分配内存 */
    Vec3_AoS *aos_a = aligned_alloc(32, N * sizeof(Vec3_AoS));
    Vec3_AoS *aos_b = aligned_alloc(32, N * sizeof(Vec3_AoS));
    Vec3_AoS *aos_dst = aligned_alloc(32, N * sizeof(Vec3_AoS));

    Vec3_SoA soa_a = {
        aligned_alloc(32, N * sizeof(float)),
        aligned_alloc(32, N * sizeof(float)),
        aligned_alloc(32, N * sizeof(float))
    };
    Vec3_SoA soa_b = {
        aligned_alloc(32, N * sizeof(float)),
        aligned_alloc(32, N * sizeof(float)),
        aligned_alloc(32, N * sizeof(float))
    };
    Vec3_SoA soa_dst = {
        aligned_alloc(32, N * sizeof(float)),
        aligned_alloc(32, N * sizeof(float)),
        aligned_alloc(32, N * sizeof(float))
    };

    /* 初始化 */
    for (int i = 0; i < N; i++) {
        aos_a[i].x = aos_a[i].y = aos_a[i].z = (float)i;
        aos_b[i].x = aos_b[i].y = aos_b[i].z = (float)(N - i);
        soa_a.x[i] = soa_a.y[i] = soa_a.z[i] = (float)i;
        soa_b.x[i] = soa_b.y[i] = soa_b.z[i] = (float)(N - i);
    }

    /* 测试AoS */
    clock_t start = clock();
    add_aos(aos_dst, aos_a, aos_b, N);
    clock_t aos_time = clock() - start;
    printf("AoS addition:        %ld ms\n", aos_time * 1000 / CLOCKS_PER_SEC);

    /* 测试SoA */
    start = clock();
    add_soa(&soa_dst, &soa_a, &soa_b, N);
    clock_t soa_time = clock() - start;
    printf("SoA addition:        %ld ms (%.2fx)\n",
           soa_time * 1000 / CLOCKS_PER_SEC, (double)aos_time / soa_time);

    /* 测试SoA+AVX */
    start = clock();
    add_soa_avx(&soa_dst, &soa_a, &soa_b, N);
    clock_t soa_avx_time = clock() - start;
    printf("SoA+AVX addition:    %ld ms (%.2fx)\n",
           soa_avx_time * 1000 / CLOCKS_PER_SEC, (double)aos_time / soa_avx_time);

    /* 测试归约 */
    float *arr = aligned_alloc(32, N * sizeof(float));
    for (int i = 0; i < N; i++) arr[i] = 1.0f;

    start = clock();
    volatile float sum1 = sum_reduction_scalar(arr, N);
    clock_t scalar_sum_time = clock() - start;
    printf("\nScalar sum:          %ld ms (sum=%.0f)\n",
           scalar_sum_time * 1000 / CLOCKS_PER_SEC, sum1);

    start = clock();
    volatile float sum2 = sum_reduction_simd(arr, N);
    clock_t simd_sum_time = clock() - start;
    printf("SIMD sum:            %ld ms (sum=%.0f, %.2fx)\n",
           simd_sum_time * 1000 / CLOCKS_PER_SEC, sum2,
           (double)scalar_sum_time / simd_sum_time);

    /* 清理 */
    free(aos_a); free(aos_b); free(aos_dst);
    free(soa_a.x); free(soa_a.y); free(soa_a.z);
    free(soa_b.x); free(soa_b.y); free(soa_b.z);
    free(soa_dst.x); free(soa_dst.y); free(soa_dst.z);
    free(arr);
}

int main(void) {
    benchmark_vectorized_algorithms();
    return 0;
}
```

### 8.2 并行化算法

```c
/**
 * @file parallel_algorithms.c
 * @brief 并行算法设计与实现
 *
 * 使用OpenMP实现并行算法
 * 编译: gcc -fopenmp -O3 parallel_algorithms.c -o parallel_algorithms
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

/**
 * @brief 并行归约 - 求和
 */
long long parallel_sum(const int *arr, int n) {
    long long sum = 0;

    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }

    return sum;
}

/**
 * @brief 并行扫描（前缀和）
 */
void parallel_scan(int *dst, const int *src, int n) {
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int nthreads = omp_get_num_threads();

        /* 计算每个线程的范围 */
        int chunk = n / nthreads;
        int start = tid * chunk;
        int end = (tid == nthreads - 1) ? n : start + chunk;

        /* 线程内扫描 */
        dst[start] = src[start];
        for (int i = start + 1; i < end; i++) {
            dst[i] = dst[i-1] + src[i];
        }
    }

    /* 块间修正（串行） */
    int *corrections = malloc(omp_get_max_threads() * sizeof(int));
    corrections[0] = 0;
    int chunk = n / omp_get_max_threads();
    for (int t = 1; t < omp_get_max_threads(); t++) {
        corrections[t] = corrections[t-1] + dst[t * chunk - 1];
    }

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int start = tid * chunk;
        int end = (tid == omp_get_max_threads() - 1) ? n : start + chunk;

        for (int i = start; i < end; i++) {
            dst[i] += corrections[tid];
        }
    }

    free(corrections);
}

/**
 * @brief 并行快速排序
 */
void parallel_quicksort(int *arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[(left + right) / 2];
    int i = left, j = right;

    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            int tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
            j--;
        }
    }

    /* 任务并行递归 */
    #pragma omp task shared(arr)
    parallel_quicksort(arr, left, j);

    #pragma omp task shared(arr)
    parallel_quicksort(arr, i, right);
}

void parallel_sort(int *arr, int n) {
    #pragma omp parallel
    {
        #pragma omp single
        parallel_quicksort(arr, 0, n - 1);
    }
}

/**
 * @brief 并行矩阵乘法
 */
void matmul_parallel(float *C, const float *A, const float *B, int n) {
    #pragma omp parallel for collapse(2) schedule(static)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            float sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }
}

/* 优化版本：循环交换 + SIMD */
void matmul_parallel_optimized(float *C, const float *A, const float *B, int n) {
    memset(C, 0, n * n * sizeof(float));

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            float a = A[i * n + k];
            #pragma omp simd
            for (int j = 0; j < n; j++) {
                C[i * n + j] += a * B[k * n + j];
            }
        }
    }
}

void benchmark_parallel_algorithms(void) {
    printf("=== Parallel Algorithms Benchmark ===\n\n");

    const int N = 100 * 1024 * 1024;  /* 100M元素 */
    int *arr = malloc(N * sizeof(int));

    for (int i = 0; i < N; i++) {
        arr[i] = i + 1;
    }

    /* 串行求和 */
    clock_t start = clock();
    long long sum_serial = 0;
    for (int i = 0; i < N; i++) {
        sum_serial += arr[i];
    }
    clock_t serial_time = clock() - start;
    printf("Serial sum:   %ld ms (sum=%lld)\n",
           serial_time * 1000 / CLOCKS_PER_SEC, sum_serial);

    /* 并行求和 */
    start = clock();
    volatile long long sum_parallel = parallel_sum(arr, N);
    clock_t parallel_time = clock() - start;
    printf("Parallel sum: %ld ms (sum=%lld, %.2fx speedup with %d threads)\n",
           parallel_time * 1000 / CLOCKS_PER_SEC, sum_parallel,
           (double)serial_time / parallel_time, omp_get_max_threads());

    /* 矩阵乘法测试 */
    printf("\nMatrix multiplication (2048x2048):\n");
    int n = 2048;
    float *A = aligned_alloc(32, n * n * sizeof(float));
    float *B = aligned_alloc(32, n * n * sizeof(float));
    float *C = aligned_alloc(32, n * n * sizeof(float));

    for (int i = 0; i < n * n; i++) {
        A[i] = (float)(i % 10) / 10.0f;
        B[i] = (float)((i * 7) % 10) / 10.0f;
    }

    start = clock();
    matmul_parallel(C, A, B, n);
    clock_t matmul_time = clock() - start;
    printf("  Parallel:   %ld ms\n", matmul_time * 1000 / CLOCKS_PER_SEC);

    start = clock();
    matmul_parallel_optimized(C, A, B, n);
    clock_t opt_time = clock() - start;
    printf("  Optimized:  %ld ms (%.2fx)\n",
           opt_time * 1000 / CLOCKS_PER_SEC,
           (double)matmul_time / opt_time);

    free(arr);
    free(A);
    free(B);
    free(C);
}

int main(void) {
    benchmark_parallel_algorithms();
    return 0;
}
```

---

## 9. 性能测量与分析

### 9.1 PMC（性能监控计数器）

```c
/**
 * @file pmc_analysis.c
 * @brief 性能监控计数器（PMC）使用指南
 *
 * PMC提供CPU内部事件计数：
 * - 时钟周期、指令 retired
 * - 缓存命中/未命中
 * - 分支预测成功/失败
 * - TLB miss
 * - 等等
 *
 * Linux接口：/proc/sys/kernel/perf_event_paranoid
 * 工具：perf, PAPI库
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <asm/unistd.h>
#include <time.h>

/* 系统调用包装 */
static long perf_event_open(struct perf_event_attr *hw_event,
                            pid_t pid, int cpu, int group_fd,
                            unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

/* 配置PMC事件 */
struct perf_event_attr create_event_config(int type, unsigned long config) {
    struct perf_event_attr pe = {
        .type = type,
        .size = sizeof(struct perf_event_attr),
        .config = config,
        .disabled = 1,
        .exclude_kernel = 1,
        .exclude_hv = 1
    };
    return pe;
}

/**
 * @brief 测量基本性能指标
 */
void measure_basic_metrics(void) {
    printf("=== Basic PMC Metrics ===\n\n");

    /* 配置事件：CPU周期 */
    struct perf_event_attr pe_cycles = create_event_config(
        PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES);

    /* 配置事件：指令数 */
    struct perf_event_attr pe_instructions = create_event_config(
        PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS);

    /* 配置事件：缓存引用 */
    struct perf_event_attr pe_cache_ref = create_event_config(
        PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_REFERENCES);

    /* 配置事件：缓存未命中 */
    struct perf_event_attr pe_cache_miss = create_event_config(
        PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES);

    /* 打开事件 */
    int fd_cycles = perf_event_open(&pe_cycles, 0, -1, -1, 0);
    int fd_instr = perf_event_open(&pe_instructions, 0, -1, -1, 0);
    int fd_cache_ref = perf_event_open(&pe_cache_ref, 0, -1, -1, 0);
    int fd_cache_miss = perf_event_open(&pe_cache_miss, 0, -1, -1, 0);

    if (fd_cycles < 0 || fd_instr < 0) {
        printf("perf_event_open failed (try running as root or adjust perf_event_paranoid)\n");
        return;
    }

    /* 启动测量 */
    ioctl(fd_cycles, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_instr, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_cache_ref, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_cache_miss, PERF_EVENT_IOC_RESET, 0);

    ioctl(fd_cycles, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(fd_instr, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(fd_cache_ref, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(fd_cache_miss, PERF_EVENT_IOC_ENABLE, 0);

    /* 执行被测代码 */
    const int N = 100000000;
    volatile long long sum = 0;
    for (int i = 0; i < N; i++) {
        sum += i;
    }
    (void)sum;

    /* 停止测量 */
    ioctl(fd_cycles, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(fd_instr, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(fd_cache_ref, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(fd_cache_miss, PERF_EVENT_IOC_DISABLE, 0);

    /* 读取结果 */
    long long cycles, instructions, cache_refs, cache_misses;
    read(fd_cycles, &cycles, sizeof(cycles));
    read(fd_instr, &instructions, sizeof(instructions));
    read(fd_cache_ref, &cache_refs, sizeof(cache_refs));
    read(fd_cache_miss, &cache_misses, sizeof(cache_misses));

    /* 计算指标 */
    printf("Execution metrics:\n");
    printf("  CPU cycles:      %lld\n", cycles);
    printf("  Instructions:    %lld\n", instructions);
    printf("  IPC:             %.2f\n", (double)instructions / cycles);
    printf("  CPI:             %.2f\n", (double)cycles / instructions);
    printf("  Cache refs:      %lld\n", cache_refs);
    printf("  Cache misses:    %lld\n", cache_misses);
    printf("  Cache miss rate: %.2f%%\n",
           cache_refs > 0 ? 100.0 * cache_misses / cache_refs : 0);

    close(fd_cycles);
    close(fd_instr);
    close(fd_cache_ref);
    close(fd_cache_miss);
}

/**
 * @brief 测量分支预测
 */
void measure_branch_prediction(void) {
    printf("\n=== Branch Prediction Metrics ===\n\n");

    struct perf_event_attr pe_branches = create_event_config(
        PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS);
    struct perf_event_attr pe_branch_miss = create_event_config(
        PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_MISSES);

    int fd_br = perf_event_open(&pe_branches, 0, -1, -1, 0);
    int fd_miss = perf_event_open(&pe_branch_miss, 0, -1, -1, 0);

    if (fd_br < 0) {
        printf("Branch events not available\n");
        return;
    }

    /* 有序数据（预测成功率高） */
    const int N = 10000000;
    int *ordered = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        ordered[i] = i % 100;
    }

    ioctl(fd_br, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_miss, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_br, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(fd_miss, PERF_EVENT_IOC_ENABLE, 0);

    volatile long long sum = 0;
    for (int i = 0; i < N; i++) {
        if (ordered[i] < 50) {
            sum += ordered[i];
        }
    }

    ioctl(fd_br, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(fd_miss, PERF_EVENT_IOC_DISABLE, 0);

    long long branches, misses;
    read(fd_br, &branches, sizeof(branches));
    read(fd_miss, &misses, sizeof(misses));

    printf("Ordered data:\n");
    printf("  Branches:       %lld\n", branches);
    printf("  Mispredictions: %lld\n", misses);
    printf("  Miss rate:      %.2f%%\n",
           100.0 * misses / branches);

    /* 随机数据（预测失败率高） */
    srand(42);
    int *random = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        random[i] = rand() % 100;
    }

    ioctl(fd_br, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_miss, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_br, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(fd_miss, PERF_EVENT_IOC_ENABLE, 0);

    sum = 0;
    for (int i = 0; i < N; i++) {
        if (random[i] < 50) {
            sum += random[i];
        }
    }

    ioctl(fd_br, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(fd_miss, PERF_EVENT_IOC_DISABLE, 0);

    read(fd_br, &branches, sizeof(branches));
    read(fd_miss, &misses, sizeof(misses));

    printf("\nRandom data:\n");
    printf("  Branches:       %lld\n", branches);
    printf("  Mispredictions: %lld\n", misses);
    printf("  Miss rate:      %.2f%%\n",
           100.0 * misses / branches);

    free(ordered);
    free(random);
    close(fd_br);
    close(fd_miss);
}

int main(void) {
    measure_basic_metrics();
    measure_branch_prediction();
    return 0;
}
```

### 9.2 PEBS（Processor Event-Based Sampling）

```c
/**
 * @file pebs_analysis.c
 * @brief PEBS精确事件采样分析
 *
 * PEBS在事件发生时自动保存处理器状态
 * 提供精确的指令指针和延迟信息
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

#define PEBS_SAMPLE_SIZE 4096

/**
 * @brief PEBS数据采集
 *
 * 使用PERF_SAMPLE_IP捕获精确指令指针
 * 使用PERF_SAMPLE_ADDR捕获数据地址（用于缓存分析）
 */
void setup_pebs_sampling(void) {
    printf("=== PEBS Sampling Setup ===\n\n");

    struct perf_event_attr pe = {
        .type = PERF_TYPE_HARDWARE,
        .size = sizeof(struct perf_event_attr),
        .config = PERF_COUNT_HW_CACHE_MISSES,
        .sample_period = 10000,        /* 每10000个事件采样一次 */
        .sample_type = PERF_SAMPLE_IP | PERF_SAMPLE_ADDR | PERF_SAMPLE_WEIGHT,
        .disabled = 1,
        .exclude_kernel = 1,
        .precise_ip = 3,               /* 精确IP级别3（PEBS） */
        .wakeup_events = 1
    };

    printf("PEBS configuration:\n");
    printf("  Event: Cache misses\n");
    printf("  Sample period: %d\n", (int)pe.sample_period);
    printf("  Sample types: IP, ADDR, WEIGHT\n");
    printf("  Precise IP: %d (highest precision)\n", pe.precise_ip);
}

/**
 * @brief 使用perf进行PEBS分析
 *
 * 命令行示例：
 * perf record -e cache-misses:pp ./program
 * perf report --sort=symbol
 * perf annotate
 */
void perf_pebs_commands(void) {
    printf("\n=== PEBS perf Commands ===\n\n");

    printf("1. Record cache misses with PEBS:\n");
    printf("   perf record -e cache-misses:pp -c 10000 ./program\n");
    printf("   (:pp = precise, -c = count between samples)\n\n");

    printf("2. Analyze results:\n");
    printf("   perf report --sort=symbol,dso\n\n");

    printf("3. Annotate source:\n");
    printf("   perf annotate --stdio\n\n");

    printf("4. Profile specific function:\n");
    printf("   perf record -e cycles:pp -g -- ./program\n");
    printf("   perf report --call-graph=fractal\n\n");

    printf("5. Memory access profiling:\n");
    printf("   perf mem record ./program\n");
    printf("   perf mem report\n");
}

int main(void) {
    setup_pebs_sampling();
    perf_pebs_commands();
    return 0;
}
```

---

## 10. 实战案例：高频交易系统优化

### 10.1 系统概述与优化目标

```c
/**
 * @file hft_optimized.c
 * @brief 高频交易系统性能优化完整案例
 *
 * 高频交易（HFT）性能要求：
 * - 延迟：微秒级（<10μs）
 * - 吞吐量：每秒百万订单
 * - 抖动：极低（99.9% < 5μs）
 *
 * 优化策略：
 * 1. CPU亲和性绑定
 * 2. 禁用超线程/SMT
 * 3. 禁用CPU频率调节
 * 4. 大页内存
 * 5. 内核旁路网络（DPDK/RDMA）
 * 6. 无锁数据结构
 * 7. 预计算/查表
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <x86intrin.h>

/* 高精度计时器（RDTSC） */
static inline uint64_t rdtsc(void) {
    return __rdtsc();
}

static inline uint64_t rdtscp(uint32_t *aux) {
    return __rdtscp(aux);
}

/* CPU频率校准 */
double calibrate_tsc_frequency(void) {
    uint64_t tsc_start = rdtsc();
    clock_t clock_start = clock();

    /* 休眠100ms */
    struct timespec ts = {0, 100000000};
    nanosleep(&ts, NULL);

    uint64_t tsc_end = rdtsc();
    clock_t clock_end = clock();

    double clock_seconds = (clock_end - clock_start) / (double)CLOCKS_PER_SEC;
    uint64_t tsc_elapsed = tsc_end - tsc_start;

    return tsc_elapsed / clock_seconds;  /* TSC频率（cycles/second） */
}

/* 微秒级延迟测量 */
double tsc_to_us(uint64_t tsc_delta, double tsc_freq) {
    return (tsc_delta / tsc_freq) * 1000000.0;
}

/**
 * @brief 订单簿数据结构（高度优化）
 */
#define MAX_PRICE_LEVELS 1000
#define MAX_ORDERS_PER_LEVEL 1000

typedef struct Order {
    uint64_t order_id;
    uint32_t quantity;
    uint32_t price;
    uint8_t side;  /* 0=bid, 1=ask */
} __attribute__((packed)) Order;

typedef struct PriceLevel {
    uint32_t price;
    uint32_t total_quantity;
    uint32_t order_count;
    Order orders[MAX_ORDERS_PER_LEVEL];
    pthread_spinlock_t lock;  /* 每个价格级别独立锁 */
} __attribute__((aligned(64))) PriceLevel;  /* 缓存行对齐 */

typedef struct {
    PriceLevel bids[MAX_PRICE_LEVELS];  /* 买单 */
    PriceLevel asks[MAX_PRICE_LEVELS];  /* 卖单 */
    uint32_t best_bid;
    uint32_t best_ask;
    _Atomic(uint64_t) sequence;  /* 序列号 */
} OrderBook;

/* 初始化订单簿 */
void orderbook_init(OrderBook *book) {
    memset(book, 0, sizeof(OrderBook));
    for (int i = 0; i < MAX_PRICE_LEVELS; i++) {
        pthread_spin_init(&book->bids[i].lock, PTHREAD_PROCESS_PRIVATE);
        pthread_spin_init(&book->asks[i].lock, PTHREAD_PROCESS_PRIVATE);
    }
    book->best_bid = 0;
    book->best_ask = UINT32_MAX;
}

/* 添加订单（优化版本） */
static inline uint64_t add_order_fast(OrderBook *book, uint32_t price,
                                       uint32_t qty, uint8_t side) {
    uint64_t seq = atomic_fetch_add(&book->sequence, 1);

    PriceLevel *level = (side == 0) ? &book->bids[price] : &book->asks[price];

    /* 快速路径：无竞争时无锁添加 */
    if (level->order_count < MAX_ORDERS_PER_LEVEL - 1) {
        uint32_t idx = level->order_count;
        level->orders[idx].order_id = seq;
        level->orders[idx].quantity = qty;
        level->orders[idx].price = price;
        level->orders[idx].side = side;

        /* 内存屏障确保数据可见 */
        __atomic_thread_fence(__ATOMIC_RELEASE);
        level->order_count++;
        level->total_quantity += qty;
    }

    /* 更新最优价格 */
    if (side == 0 && price > book->best_bid) {
        book->best_bid = price;
    } else if (side == 1 && price < book->best_ask) {
        book->best_ask = price;
    }

    return seq;
}

/**
 * @brief 网络优化 - 低延迟socket设置
 */
int create_low_latency_socket(void) {
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) return -1;

    /* TCP_NODELAY - 禁用Nagle算法 */
    int nodelay = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

    /* TCP_QUICKACK - 立即发送ACK */
    int quickack = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_QUICKACK, &quickack, sizeof(quickack));

    /* SO_BUSY_POLL - 忙轮询接收（减少中断延迟） */
    int busy_poll = 50;  /* 50μs忙轮询 */
    setsockopt(sock, SOL_SOCKET, SO_BUSY_POLL, &busy_poll, sizeof(busy_poll));

    /* 设置高优先级 */
    int priority = 6;  /* DSCP EF (Expedited Forwarding) */
    setsockopt(sock, IPPROTO_IP, IP_TOS, &priority, sizeof(priority));

    return sock;
}

/**
 * @brief 内存预取优化
 */
void prefetch_orderbook(OrderBook *book) {
    /* 预取常用价格级别到L1 */
    for (int i = 0; i < 10; i++) {
        _mm_prefetch((const char*)&book->bids[i], _MM_HINT_T0);
        _mm_prefetch((const char*)&book->asks[i], _MM_HINT_T0);
    }
}

/**
 * @brief 延迟测试
 */
void latency_benchmark(void) {
    printf("=== HFT System Latency Benchmark ===\n\n");

    /* 校准TSC */
    double tsc_freq = calibrate_tsc_frequency();
    printf("TSC Frequency: %.2f GHz\n\n", tsc_freq / 1e9);

    OrderBook book;
    orderbook_init(&book);

    /* 预热 */
    for (int i = 0; i < 1000; i++) {
        add_order_fast(&book, 100 + (i % 100), 100, i % 2);
    }

    /* 延迟测试 */
    const int N = 1000000;
    uint64_t *latencies = malloc(N * sizeof(uint64_t));

    printf("Measuring order insertion latency...\n");

    for (int i = 0; i < N; i++) {
        uint64_t start = rdtsc();
        add_order_fast(&book, 100 + (i % 100), 100, i % 2);
        uint64_t end = rdtsc();
        latencies[i] = end - start;
    }

    /* 统计分析 */
    uint64_t min_lat = latencies[0];
    uint64_t max_lat = latencies[0];
    uint64_t sum_lat = 0;

    for (int i = 0; i < N; i++) {
        if (latencies[i] < min_lat) min_lat = latencies[i];
        if (latencies[i] > max_lat) max_lat = latencies[i];
        sum_lat += latencies[i];
    }

    /* 计算百分位数 */
    qsort(latencies, N, sizeof(uint64_t),
          (int(*)(const void*,const void*))memcmp);

    uint64_t p50 = latencies[N / 2];
    uint64_t p99 = latencies[(int)(N * 0.99)];
    uint64_t p999 = latencies[(int)(N * 0.999)];

    printf("\nLatency Statistics:\n");
    printf("  Min:     %.2f ns\n", tsc_to_us(min_lat, tsc_freq) * 1000);
    printf("  Max:     %.2f ns\n", tsc_to_us(max_lat, tsc_freq) * 1000);
    printf("  Mean:    %.2f ns\n", tsc_to_us(sum_lat / N, tsc_freq) * 1000);
    printf("  P50:     %.2f ns\n", tsc_to_us(p50, tsc_freq) * 1000);
    printf("  P99:     %.2f ns\n", tsc_to_us(p99, tsc_freq) * 1000);
    printf("  P99.9:   %.2f ns\n", tsc_to_us(p999, tsc_freq) * 1000);

    printf("\nPerformance:\n");
    printf("  Throughput: %.2f M orders/sec\n", N / (sum_lat / tsc_freq) / 1e6);

    free(latencies);
}

/**
 * @brief 系统调优建议
 */
void system_tuning_guide(void) {
    printf("\n=== HFT System Tuning Guide ===\n\n");

    printf("1. CPU隔离:\n");
    printf("   isolcpus=2,3 nohz_full=2,3 rcu_nocbs=2,3\n");
    printf("   (隔离CPU 2,3，禁用tick，RCU offload)\n\n");

    printf("2. 禁用超线程:\n");
    printf("   /sys/devices/system/cpu/smt/control = off\n\n");

    printf("3. CPU频率锁定:\n");
    printf("   cpupower frequency-set -g performance\n\n");

    printf("4. 禁用交换:\n");
    printf("   swapoff -a\n\n");
    printf("   vm.swappiness = 0\n\n");

    printf("5. 网络调优:\n");
    printf("   net.core.busy_poll = 50\n");
    printf("   net.core.busy_budget = 200\n");
    printf("   net.ipv4.tcp_low_latency = 1\n\n");

    printf("6. 中断亲和性:\n");
    printf("   /proc/irq/<num>/smp_affinity = <isolated_cpu_mask>\n\n");

    printf("7. 大页内存:\n");
    printf("   vm.nr_hugepages = 1024\n\n");
}

int main(void) {
    latency_benchmark();
    system_tuning_guide();
    return 0;
}
```

### 10.2 完整优化清单

```c
/**
 * @file hft_checklist.c
 * @brief 高频交易系统优化检查清单
 *
 * 编译优化标志：
 * gcc -O3 -march=native -mtune=native -flto -funroll-loops \
 *     -fno-stack-protector -fomit-frame-pointer \
 *     -DNDEBUG hft_optimized.c -o hft_optimized
 */

#ifndef HFT_CHECKLIST_H
#define HFT_CHECKLIST_H

/* 1. 编译器优化 */
#define COMPILE_FLAGS "-O3 -march=native -mtune=native -flto"

/* 2. 内存优化 */
#define USE_HUGEPAGES 1
#define USE_NUMA_LOCAL 1
#define CACHE_LINE_ALIGN __attribute__((aligned(64)))

/* 3. 数据结构设计 */
#define AVOID_FALSE_SHARING 1
#define PREFETCH_HOT_DATA 1
#define USE_SOA_LAYOUT 1

/* 4. 并发优化 */
#define USE_LOCK_FREE 1
#define USE_THREAD_AFFINITY 1
#define AVOID_SHARED_STATE 1

/* 5. I/O优化 */
#define USE_BUSY_POLLING 1
#define DISABLE_NAGLE 1
#define KERNEL_BYPASS 0  /* DPDK/RDMA需要专门硬件 */

/* 6. 算法优化 */
#define PRECOMPUTE_LOOKUP 1
#define USE_SIMD 1
#define BRANCHLESS_CODE 1

/* 7. 测量与监控 */
#define MEASURE_LATENCY 1
#define TRACK_PERCENTILES 1
#define PMC_PROFILING 0  /* 仅在调试时启用 */

#endif /* HFT_CHECKLIST_H */
```

---

## 总结

本文档全面覆盖了C语言性能优化的核心技术领域：

### 性能优化层次

1. **微架构级**：理解CPU流水线、分支预测、乱序执行，编写CPU友好的代码
2. **缓存级**：优化数据布局、预取策略、避免伪共享
3. **指令级**：SIMD向量化、编译器优化、汇编级调优
4. **内存级**：HugePages、NUMA优化、内存池
5. **系统级**：零拷贝I/O、异步I/O、io_uring
6. **并发级**：无锁编程、RCU、线程亲和性
7. **算法级**：向量化算法、并行化算法
8. **测量级**：PMC、PEBS精确性能分析

### 关键原则

- **测量优先**：不要假设，始终用性能数据指导优化
- **关注热点**：80%时间花在20%代码上，专注优化热点
- **内存墙**：现代CPU瓶颈通常在内存，优化数据访问模式
- **复杂度权衡**：算法复杂度比常数因子更重要（对于大数据）
- **可维护性**：性能优化不应牺牲代码可读性和可维护性

### 推荐工具

- **编译器**：GCC/Clang优化报告 (`-fopt-info`)
- **剖析器**：perf, Intel VTune, AMD uProf
- **静态分析**：Compiler Explorer (godbolt.org)
- **内存分析**：valgrind, Intel Inspector

---

*文档版本：1.0*
*最后更新：2026年3月*
*作者：C语言知识库*


---

## 深入理解

### 技术原理

深入探讨相关技术原理和实现细节。

### 实践指南

- 步骤1：理解基础概念
- 步骤2：掌握核心原理
- 步骤3：应用实践

### 相关资源

- 文档链接
- 代码示例
- 参考文章

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
