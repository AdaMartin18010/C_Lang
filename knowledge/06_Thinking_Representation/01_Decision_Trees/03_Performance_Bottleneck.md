# 性能瓶颈分析决策树


---

## 📑 目录

- [性能瓶颈分析决策树](#性能瓶颈分析决策树)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [性能分析方法论](#性能分析方法论)
    - [1. 分析前的准备](#1-分析前的准备)
    - [2. 性能分析层次](#2-性能分析层次)
  - [CPU/内存/IO瓶颈识别](#cpu内存io瓶颈识别)
    - [1. CPU瓶颈特征](#1-cpu瓶颈特征)
    - [2. 内存瓶颈特征](#2-内存瓶颈特征)
    - [3. IO瓶颈特征](#3-io瓶颈特征)
  - [perf、gprof使用指南](#perfgprof使用指南)
    - [1. perf工具使用](#1-perf工具使用)
    - [2. gprof工具使用](#2-gprof工具使用)
  - [火焰图分析](#火焰图分析)
    - [生成火焰图](#生成火焰图)
    - [火焰图解读](#火焰图解读)
  - [决策树: 性能问题→原因→解决方案](#决策树-性能问题原因解决方案)
  - [C代码优化案例](#c代码优化案例)
    - [1. 缓存优化案例](#1-缓存优化案例)
    - [2. 循环展开优化](#2-循环展开优化)
    - [3. SIMD优化](#3-simd优化)
    - [编译和运行](#编译和运行)
  - [性能优化检查清单](#性能优化检查清单)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 概述

性能优化是C语言编程中的重要环节。
有效的性能分析需要系统化的方法论，从识别瓶颈类型到实施针对性优化。
本文提供完整的性能分析决策树和实战案例。

## 性能分析方法论

### 1. 分析前的准备

```c
// benchmark.h - 性能测试基础框架
#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdio.h>
#include <time.h>

// 高精度计时器
typedef struct {
    struct timespec start;
    struct timespec end;
    const char *name;
} timer_t;

static inline void timer_start(timer_t *t, const char *name) {
    t->name = name;
    clock_gettime(CLOCK_MONOTONIC, &t->start);
}

static inline double timer_end(timer_t *t) {
    clock_gettime(CLOCK_MONOTONIC, &t->end);
    double elapsed = (t->end.tv_sec - t->start.tv_sec) +
                     (t->end.tv_nsec - t->start.tv_nsec) / 1e9;
    printf("[%s] Elapsed: %.6f seconds\n", t->name, elapsed);
    return elapsed;
}

// 重复测试取平均值
#define BENCHMARK(func, iterations) do { \
    timer_t t; \
    timer_start(&t, #func); \
    for (int _i = 0; _i < (iterations); _i++) { \
        func(); \
    } \
    double total = timer_end(&t); \
    printf("  Average: %.9f seconds/iteration\n", total / (iterations)); \
} while(0)

#endif
```

### 2. 性能分析层次

```
┌─────────────────────────────────────────────────────────────┐
│                    性能分析层次模型                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │   系统层     │  │   应用层     │  │   代码层     │      │
│  │              │  │              │  │              │      │
│  │ • CPU使用率  │  │ • 算法复杂度 │  │ • 循环优化   │      │
│  │ • 内存使用   │  │ • 数据结构   │  │ • 函数内联   │      │
│  │ • IO吞吐量   │  │ • 缓存策略   │  │ • 分支预测   │      │
│  │ • 网络延迟   │  │ • 并发模型   │  │ • SIMD指令   │      │
│  │              │  │              │  │              │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## CPU/内存/IO瓶颈识别

### 1. CPU瓶颈特征

```c
// cpu_intensive.c - CPU密集型任务示例
#include <math.h>
#include <stdio.h>
#include "benchmark.h"

// 计算密集型: 素数检测
int is_prime(int n) {
    if (n < 2) return 0;
    for (int i = 2; i <= (int)sqrt(n); i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

// 优化版本: 减少平方根计算
int is_prime_optimized(int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    int limit = (int)sqrt(n);
    for (int i = 3; i <= limit; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

void test_cpu_bound(void) {
    int count = 0;
    for (int i = 2; i < 100000; i++) {
        if (is_prime(i)) count++;
    }
    printf("Found %d primes\n", count);
}

int main(void) {
    printf("=== CPU Bound Test ===\n");
    BENCHMARK(test_cpu_bound, 1);
    return 0;
}
```

### 2. 内存瓶颈特征

```c
// memory_bound.c - 内存密集型任务示例
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "benchmark.h"

#define ARRAY_SIZE (1024 * 1024 * 100)  // 100M elements

// 低效的内存访问模式 (缓存不友好)
void column_major_access(int *matrix, int n) {
    long long sum = 0;
    for (int col = 0; col < n; col++) {
        for (int row = 0; row < n; row++) {
            sum += matrix[row * n + col];  // 跨行访问，缓存未命中
        }
    }
}

// 优化的内存访问模式 (缓存友好)
void row_major_access(int *matrix, int n) {
    long long sum = 0;
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            sum += matrix[row * n + col];  // 顺序访问，缓存命中
        }
    }
}

// 内存分配测试
void memory_allocation_test(void) {
    for (int i = 0; i < 10000; i++) {
        int *arr = malloc(1024 * sizeof(int));
        arr[0] = i;
        free(arr);  // 频繁的分配释放
    }
}

// 优化: 使用内存池
#define POOL_SIZE 10000
typedef struct {
    int blocks[POOL_SIZE][1024];
    int used[POOL_SIZE];
} memory_pool_t;

memory_pool_t pool = {0};

int *pool_alloc(void) {
    for (int i = 0; i < POOL_SIZE; i++) {
        if (!pool.used[i]) {
            pool.used[i] = 1;
            return pool.blocks[i];
        }
    }
    return NULL;  // 池耗尽
}

void pool_free(int *p) {
    int index = (p - pool.blocks[0]) / 1024;
    if (index >= 0 && index < POOL_SIZE) {
        pool.used[index] = 0;
    }
}

void memory_pool_test(void) {
    for (int i = 0; i < 10000; i++) {
        int *arr = pool_alloc();
        if (arr) {
            arr[0] = i;
            pool_free(arr);
        }
    }
}

int main(void) {
    printf("=== Memory Bound Test ===\n");
    BENCHMARK(memory_allocation_test, 1);
    BENCHMARK(memory_pool_test, 1);
    return 0;
}
```

### 3. IO瓶颈特征

```c
// io_bound.c - IO密集型任务示例
#include <stdio.h>
#include <stdlib.h>
#include "benchmark.h"

#define BUFFER_SIZE 4096
#define FILE_SIZE (1024 * 1024 * 10)  // 10MB

// 低效的逐字节IO
void inefficient_io_write(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return;

    for (int i = 0; i < FILE_SIZE; i++) {
        char c = (char)(i % 256);
        fwrite(&c, 1, 1, fp);  // 每次写入1字节
    }
    fclose(fp);
}

// 优化: 缓冲写入
void optimized_io_write(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return;

    char buffer[BUFFER_SIZE];
    for (int i = 0; i < FILE_SIZE; i += BUFFER_SIZE) {
        for (int j = 0; j < BUFFER_SIZE && i + j < FILE_SIZE; j++) {
            buffer[j] = (char)((i + j) % 256);
        }
        fwrite(buffer, 1,
               (i + BUFFER_SIZE <= FILE_SIZE) ? BUFFER_SIZE : (FILE_SIZE - i),
               fp);
    }
    fclose(fp);
}

// 使用setvbuf优化
void buffered_io_write(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return;

    // 设置大缓冲区
    char *buf = malloc(BUFFER_SIZE * 16);
    setvbuf(fp, buf, _IOFBF, BUFFER_SIZE * 16);

    for (int i = 0; i < FILE_SIZE; i++) {
        char c = (char)(i % 256);
        fwrite(&c, 1, 1, fp);
    }

    fclose(fp);
    free(buf);
}

int main(void) {
    printf("=== IO Bound Test ===\n");
    BENCHMARK(inefficient_io_write("test1.bin"), 1);
    BENCHMARK(optimized_io_write("test2.bin"), 1);
    BENCHMARK(buffered_io_write("test3.bin"), 1);

    // 清理
    remove("test1.bin");
    remove("test2.bin");
    remove("test3.bin");

    return 0;
}
```

## perf、gprof使用指南

### 1. perf工具使用

```bash
# 安装perf (Ubuntu/Debian)
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
sudo apt-get install linux-tools-common linux-tools-generic

# 基本使用
perf stat ./program              # 统计性能事件
perf record ./program            # 记录性能数据
perf report                      # 查看分析报告
perf top                         # 实时性能监控

# 详细统计
perf stat -e cycles,instructions,cache-misses,cache-references ./program

# 生成火焰图
perf record -g ./program
perf script | ./stackcollapse-perf.pl | ./flamegraph.pl > output.svg
```

### 2. gprof工具使用

```bash
# 编译时添加-pg选项
gcc -pg -g -O2 program.c -o program

# 运行程序生成gmon.out
./program

# 查看分析结果
gprof ./program gmon.out > profile.txt
gprof ./program gmon.out -b      # 简洁模式

# 可视化
gprof ./program | gprof2dot | dot -Tpng -o output.png
```

```c
// gprof_example.c - 用于gprof分析的示例
#include <stdio.h>
#include <stdlib.h>

void slow_function(int n) {
    double sum = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            sum += i * j * 0.5;
        }
    }
    (void)sum;  // 抑制未使用警告
}

void medium_function(int n) {
    double sum = 0;
    for (int i = 0; i < n; i++) {
        sum += i * i;
    }
    (void)sum;
}

void fast_function(int n) {
    printf("Processing %d items\n", n);
}

int main(int argc, char *argv[]) {
    int n = (argc > 1) ? atoi(argv[1]) : 1000;

    for (int i = 0; i < 10; i++) {
        fast_function(n);
        medium_function(n);
        slow_function(n);
    }

    return 0;
}
```

## 火焰图分析

火焰图(Flame Graph)是可视化性能分析结果的工具，可以直观显示程序中各函数的CPU占用时间。

### 生成火焰图

```bash
# 1. 使用perf记录数据
perf record -F 99 -g -- ./program

# 2. 下载火焰图脚本
git clone https://github.com/brendangregg/FlameGraph.git
cd FlameGraph

# 3. 生成火焰图
perf script | ./stackcollapse-perf.pl | ./flamegraph.pl > flamegraph.svg

# 4. 用浏览器打开查看
firefox flamegraph.svg
```

### 火焰图解读

```
火焰图结构示例:

┌─────────────────────────────────────────────────────────────┐
│  main                                                       │
│  ┌───────────────────────────────────────────────────────┐ │
│  │  process_data                                         │ │
│  │  ┌─────────────────┐ ┌───────────────────────────────┐│ │
│  │  │ sort_array      │ │ search_database               ││ │
│  │  │ ┌───────────┐   │ │  ┌───────────┐ ┌───────────┐  ││ │
│  │  │ │ quicksort │   │ │  │ db_query  │ │ cache_get │  ││ │
│  │  │ │ ┌─────┐   │   │ │  │ ┌───────┐ │ │ ┌───────┐ │  ││ │
│  │  │ │ │swap │   │   │ │  │ │fetch  │ │ │ │hash   │ │  ││ │
│  │  │ │ └─────┘   │   │ │  │ └───────┘ │ │ └───────┘ │  ││ │
│  │  │ └───────────┘   │ │  └───────────┘ └───────────┘  ││ │
│  │  └─────────────────┘ └───────────────────────────────┘│ │
│  └───────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘

解读要点:
- 宽度 = 占用CPU时间比例 (越宽越需要优化)
- 高度 = 调用栈深度
- 同一函数可能出现在多个地方(不同调用路径)
- 从底部往上看调用链
```

## 决策树: 性能问题→原因→解决方案

```
                           ┌─────────────────────┐
                           │    发现性能问题      │
                           └──────────┬──────────┘
                                      │
                                      ▼
                    ┌─────────────────────────────────┐
                    │    使用perf/火焰图定位热点       │
                    │    确定是CPU/内存/IO瓶颈         │
                    └─────────────────┬───────────────┘
                                      │
          ┌───────────────────────────┼───────────────────────────┐
          ▼                           ▼                           ▼
   ┌───────────────┐          ┌───────────────┐          ┌───────────────┐
   │   CPU瓶颈     │          │   内存瓶颈    │          │   IO瓶颈      │
   └───────┬───────┘          └───────┬───────┘          └───────┬───────┘
           │                          │                          │
           ▼                          ▼                          ▼
  ┌────────────────┐        ┌────────────────┐        ┌────────────────┐
  │ • 算法复杂度高  │        │ • 缓存未命中   │        │ • 频繁小IO     │
  │ • 计算密集      │        │ • 内存分配过多 │        │ • 同步IO阻塞   │
  │ • 分支预测失败  │        │ • 数据结构不当 │        │ • 磁盘寻道     │
  └───────┬────────┘        └───────┬────────┘        └───────┬────────┘
          │                         │                         │
          ▼                         ▼                         ▼
  ┌────────────────┐        ┌────────────────┐        ┌────────────────┐
  │  解决方案:      │        │  解决方案:      │        │  解决方案:      │
  │                 │        │                 │        │                 │
  │ • 优化算法      │        │ • 优化数据布局  │        │ • 批量IO       │
  │ • 循环展开      │        │ • 使用缓存      │        │ • 异步IO       │
  │ • SIMD并行      │        │ • 内存池        │        │ • 使用缓冲区   │
  │ • 多线程并行    │        │ • 减少分配      │        │ • 内存映射文件 │
  └────────────────┘        └────────────────┘        └────────────────┘
```

## C代码优化案例

### 1. 缓存优化案例

```c
// cache_optimization.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "benchmark.h"

#define MATRIX_SIZE 2048

// 原始版本: 缓存不友好
void matrix_multiply_naive(double *A, double *B, double *C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * n + j];  // B的访问不连续
            }
            C[i * n + j] = sum;
        }
    }
}

// 优化版本1: 矩阵转置
void matrix_multiply_transpose(double *A, double *B, double *C, int n) {
    // 转置B矩阵
    double *BT = malloc(n * n * sizeof(double));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            BT[j * n + i] = B[i * n + j];
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * BT[j * n + k];  // 连续访问
            }
            C[i * n + j] = sum;
        }
    }
    free(BT);
}

// 优化版本2: 分块算法 (Cache Blocking)
#define BLOCK_SIZE 64

void matrix_multiply_blocked(double *A, double *B, double *C, int n) {
    memset(C, 0, n * n * sizeof(double));

    for (int ii = 0; ii < n; ii += BLOCK_SIZE) {
        for (int jj = 0; jj < n; jj += BLOCK_SIZE) {
            for (int kk = 0; kk < n; kk += BLOCK_SIZE) {
                // 处理块
                int i_max = (ii + BLOCK_SIZE < n) ? ii + BLOCK_SIZE : n;
                int j_max = (jj + BLOCK_SIZE < n) ? jj + BLOCK_SIZE : n;
                int k_max = (kk + BLOCK_SIZE < n) ? kk + BLOCK_SIZE : n;

                for (int i = ii; i < i_max; i++) {
                    for (int k = kk; k < k_max; k++) {
                        double a = A[i * n + k];
                        for (int j = jj; j < j_max; j++) {
                            C[i * n + j] += a * B[k * n + j];
                        }
                    }
                }
            }
        }
    }
}

int main(void) {
    int n = 1024;  // 使用较小尺寸用于测试
    double *A = aligned_alloc(64, n * n * sizeof(double));
    double *B = aligned_alloc(64, n * n * sizeof(double));
    double *C = aligned_alloc(64, n * n * sizeof(double));

    // 初始化
    for (int i = 0; i < n * n; i++) {
        A[i] = (double)rand() / RAND_MAX;
        B[i] = (double)rand() / RAND_MAX;
    }

    printf("=== Cache Optimization Test (n=%d) ===\n", n);
    // BENCHMARK(matrix_multiply_naive(A, B, C, n), 1);  // 太慢
    BENCHMARK(matrix_multiply_transpose(A, B, C, n), 1);
    BENCHMARK(matrix_multiply_blocked(A, B, C, n), 1);

    free(A);
    free(B);
    free(C);

    return 0;
}
```

### 2. 循环展开优化

```c
// loop_unrolling.c
#include <stdio.h>
#include <stdlib.h>
#include "benchmark.h"

#define ARRAY_SIZE 100000000

// 原始版本
long long sum_normal(int *arr, int n) {
    long long sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

// 循环展开版本 (2x)
long long sum_unroll2(int *arr, int n) {
    long long sum = 0;
    int i;
    for (i = 0; i < n - 1; i += 2) {
        sum += arr[i] + arr[i + 1];
    }
    // 处理剩余元素
    for (; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

// 循环展开版本 (4x)
long long sum_unroll4(int *arr, int n) {
    long long sum = 0;
    int i;
    for (i = 0; i < n - 3; i += 4) {
        sum += arr[i] + arr[i + 1] + arr[i + 2] + arr[i + 3];
    }
    for (; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

// 循环展开 + 多累加器 (减少依赖链)
long long sum_unroll4_multi(int *arr, int n) {
    long long sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    int i;
    for (i = 0; i < n - 3; i += 4) {
        sum1 += arr[i];
        sum2 += arr[i + 1];
        sum3 += arr[i + 2];
        sum4 += arr[i + 3];
    }
    long long sum = sum1 + sum2 + sum3 + sum4;
    for (; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

int main(void) {
    int *arr = aligned_alloc(64, ARRAY_SIZE * sizeof(int));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        arr[i] = i % 100;
    }

    printf("=== Loop Unrolling Test ===\n");
    BENCHMARK((void)sum_normal(arr, ARRAY_SIZE), 10);
    BENCHMARK((void)sum_unroll2(arr, ARRAY_SIZE), 10);
    BENCHMARK((void)sum_unroll4(arr, ARRAY_SIZE), 10);
    BENCHMARK((void)sum_unroll4_multi(arr, ARRAY_SIZE), 10);

    free(arr);
    return 0;
}
```

### 3. SIMD优化

```c
// simd_optimization.c
#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>  // AVX/SSE头文件
#include "benchmark.h"

#define ARRAY_SIZE 10000000

// 标量版本
float dot_product_scalar(float *a, float *b, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

// SSE版本 (4 floats per operation)
float dot_product_sse(float *a, float *b, int n) {
    __m128 sum_vec = _mm_setzero_ps();
    int i;

    for (i = 0; i <= n - 4; i += 4) {
        __m128 a_vec = _mm_loadu_ps(&a[i]);
        __m128 b_vec = _mm_loadu_ps(&b[i]);
        sum_vec = _mm_add_ps(sum_vec, _mm_mul_ps(a_vec, b_vec));
    }

    // 水平求和
    float result[4];
    _mm_storeu_ps(result, sum_vec);
    float sum = result[0] + result[1] + result[2] + result[3];

    // 处理剩余元素
    for (; i < n; i++) {
        sum += a[i] * b[i];
    }

    return sum;
}

// AVX版本 (8 floats per operation)
float dot_product_avx(float *a, float *b, int n) {
    __m256 sum_vec = _mm256_setzero_ps();
    int i;

    for (i = 0; i <= n - 8; i += 8) {
        __m256 a_vec = _mm256_loadu_ps(&a[i]);
        __m256 b_vec = _mm256_loadu_ps(&b[i]);
        sum_vec = _mm256_add_ps(sum_vec, _mm256_mul_ps(a_vec, b_vec));
    }

    // 水平求和
    float result[8];
    _mm256_storeu_ps(result, sum_vec);
    float sum = result[0] + result[1] + result[2] + result[3] +
                result[4] + result[5] + result[6] + result[7];

    for (; i < n; i++) {
        sum += a[i] * b[i];
    }

    return sum;
}

// AVX + FMA版本 (融合乘加)
float dot_product_avx_fma(float *a, float *b, float *c, int n) {
    __m256 sum_vec = _mm256_setzero_ps();
    int i;

    for (i = 0; i <= n - 8; i += 8) {
        __m256 a_vec = _mm256_loadu_ps(&a[i]);
        __m256 b_vec = _mm256_loadu_ps(&b[i]);
        // FMA: sum = a * b + sum (单指令)
        sum_vec = _mm256_fmadd_ps(a_vec, b_vec, sum_vec);
    }

    float result[8];
    _mm256_storeu_ps(result, sum_vec);
    float sum = result[0] + result[1] + result[2] + result[3] +
                result[4] + result[5] + result[6] + result[7];

    for (; i < n; i++) {
        sum += a[i] * b[i];
    }

    return sum;
}

int main(void) {
    // 32字节对齐以支持AVX
    float *a = aligned_alloc(32, ARRAY_SIZE * sizeof(float));
    float *b = aligned_alloc(32, ARRAY_SIZE * sizeof(float));

    for (int i = 0; i < ARRAY_SIZE; i++) {
        a[i] = (float)(i % 100) / 100.0f;
        b[i] = (float)((i + 50) % 100) / 100.0f;
    }

    printf("=== SIMD Optimization Test ===\n");
    BENCHMARK((void)dot_product_scalar(a, b, ARRAY_SIZE), 100);
    BENCHMARK((void)dot_product_sse(a, b, ARRAY_SIZE), 100);
    BENCHMARK((void)dot_product_avx(a, b, ARRAY_SIZE), 100);

    free(a);
    free(b);
    return 0;
}
```

### 编译和运行

```bash
# 基础编译
gcc -O2 -o cache_optimization cache_optimization.c

# 使用性能分析编译
gcc -pg -O2 -o loop_unrolling loop_unrolling.c

# SIMD编译 (需要支持AVX的CPU)
gcc -O3 -mavx2 -mfma -o simd_optimization simd_optimization.c

# 使用perf分析
perf stat -e cycles,instructions,cache-references,cache-misses ./cache_optimization
```

## 性能优化检查清单

- [ ] 使用性能分析工具确定真正的热点
- [ ] 优化算法复杂度(Big-O)
- [ ] 优化内存访问模式(空间局部性)
- [ ] 减少缓存未命中
- [ ] 使用循环展开减少分支
- [ ] 考虑SIMD向量化
- [ ] 减少动态内存分配
- [ ] 使用编译器优化选项(-O2/-O3/-march=native)
- [ ] 验证优化效果(不要猜测性能)


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
