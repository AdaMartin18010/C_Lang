/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\04_Microarchitecture_Benchmarking.md
 * Line: 490
 * Language: c
 * Block ID: b8a53da8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
