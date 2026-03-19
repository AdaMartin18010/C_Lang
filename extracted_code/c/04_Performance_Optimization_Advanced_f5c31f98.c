/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 60
 * Language: c
 * Block ID: f5c31f98
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
