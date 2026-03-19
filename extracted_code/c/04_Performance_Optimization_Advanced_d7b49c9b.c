/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 1939
 * Language: c
 * Block ID: d7b49c9b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
