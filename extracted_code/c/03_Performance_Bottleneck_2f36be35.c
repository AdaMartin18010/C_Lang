/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\03_Performance_Bottleneck.md
 * Line: 111
 * Language: c
 * Block ID: 2f36be35
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
