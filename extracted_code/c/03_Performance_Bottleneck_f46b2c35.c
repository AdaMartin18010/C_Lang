/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\03_Performance_Bottleneck.md
 * Line: 155
 * Language: c
 * Block ID: f46b2c35
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
