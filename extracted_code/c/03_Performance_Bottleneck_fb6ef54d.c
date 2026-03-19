/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\03_Performance_Bottleneck.md
 * Line: 577
 * Language: c
 * Block ID: fb6ef54d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
