/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\03_Top_Down_Analysis_Method.md
 * Line: 534
 * Language: c
 * Block ID: e7809ed6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
