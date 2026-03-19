/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\04_Microarchitecture_Benchmarking.md
 * Line: 684
 * Language: c
 * Block ID: 8f2819e1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
