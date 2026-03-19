/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\03_Performance_Bottleneck.md
 * Line: 45
 * Language: c
 * Block ID: 44fa4b29
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
