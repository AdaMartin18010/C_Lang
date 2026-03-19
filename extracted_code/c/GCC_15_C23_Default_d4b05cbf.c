/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 2551
 * Language: c
 * Block ID: d4b05cbf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * GCC 15 C23 性能测试套件
 * 编译: gcc -std=c23 -O3 -march=native -o benchmark benchmark.c -lm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

#define ITERATIONS 10000000
#define ARRAY_SIZE 1000000

/* 高精度计时器 */
typedef struct {
    struct timespec start;
    struct timespec end;
    const char* name;
} timer_t;

void timer_start(timer_t* t) {
    timespec_get(&t->start, TIME_MONOTONIC);
}

void timer_stop(timer_t* t) {
    timespec_get(&t->end, TIME_MONOTONIC);
}

double timer_elapsed_ms(timer_t* t) {
    double sec = t->end.tv_sec - t->start.tv_sec;
    double nsec = t->end.tv_nsec - t->start.tv_nsec;
    return (sec * 1000.0) + (nsec / 1000000.0);
}

void timer_report(timer_t* t) {
    printf("%-40s: %8.3f ms\n", t->name, timer_elapsed_ms(t));
}

/* 测试 1: 循环向量化 */
void test_loop_vectorization(void) {
    printf("\n=== 循环向量化测试 ===\n");

    int* arr = aligned_alloc(64, ARRAY_SIZE * sizeof(int));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        arr[i] = i - ARRAY_SIZE / 2;
    }

    timer_t t;

    /* 简单累加 */
    t.name = "Simple sum";
    timer_start(&t);
    long long sum = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        sum += arr[i];
    }
    timer_stop(&t);
    timer_report(&t);
    (void)sum;

    /* 条件求和 (早期 break 模式) */
    t.name = "Conditional sum (early break pattern)";
    timer_start(&t);
    sum = 0;
    for (int i = 0; i < ARRAY_SIZE && sum < 1000000000LL; i++) {
        sum += arr[i];
    }
    timer_stop(&t);
    timer_report(&t);

    /* 查找第一个正值 */
    t.name = "Find first positive";
    volatile int found = -1;
    timer_start(&t);
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (arr[i] > 0) {
            found = i;
            break;
        }
    }
    timer_stop(&t);
    timer_report(&t);

    free(arr);
}

/* 测试 2: 数学函数性能 */
void test_math_performance(void) {
    printf("\n=== 数学函数性能测试 ===\n");

    timer_t t;
    volatile double result = 0.0;

    /* sin */
    t.name = "sin()";
    timer_start(&t);
    for (int i = 0; i < ITERATIONS; i++) {
        result = sin(0.5);
    }
    timer_stop(&t);
    timer_report(&t);

    /* sinpi (C23) */
    t.name = "sinpi() (C23)";
    timer_start(&t);
    for (int i = 0; i < ITERATIONS; i++) {
        result = sinpi(0.5);
    }
    timer_stop(&t);
    timer_report(&t);

    /* exp */
    t.name = "exp()";
    timer_start(&t);
    for (int i = 0; i < ITERATIONS; i++) {
        result = exp(1.0);
    }
    timer_stop(&t);
    timer_report(&t);

    /* log */
    t.name = "log()";
    timer_start(&t);
    for (int i = 0; i < ITERATIONS; i++) {
        result = log(2.0);
    }
    timer_stop(&t);
    timer_report(&t);

    /* pow */
    t.name = "pow()";
    timer_start(&t);
    for (int i = 0; i < ITERATIONS / 10; i++) {
        result = pow(2.0, 10.0);
    }
    timer_stop(&t);
    timer_report(&t);

    (void)result;
}

/* 测试 3: 字符串处理性能 */
void test_string_performance(void) {
    printf("\n=== 字符串处理性能测试 ===\n");

    char* str = malloc(100000);
    memset(str, 'a', 99999);
    str[99999] = '\0';

    timer_t t;
    volatile size_t len = 0;

    /* strlen */
    t.name = "strlen() (100KB)";
    timer_start(&t);
    for (int i = 0; i < 10000; i++) {
        len = strlen(str);
    }
    timer_stop(&t);
    timer_report(&t);

    /* strchr */
    t.name = "strchr() (100KB)";
    volatile char* found = NULL;
    timer_start(&t);
    for (int i = 0; i < 10000; i++) {
        found = strchr(str, 'z');  /* 不存在，扫描全部 */
    }
    timer_stop(&t);
    timer_report(&t);

    /* strdup */
    t.name = "strdup() (100KB)";
    timer_start(&t);
    for (int i = 0; i < 1000; i++) {
        char* copy = strdup(str);
        free(copy);
    }
    timer_stop(&t);
    timer_report(&t);

    free(str);
    (void)len;
    (void)found;
}

/* 测试 4: 内存操作性能 */
void test_memory_performance(void) {
    printf("\n=== 内存操作性能测试 ===\n");

    size_t size = 100 * 1024 * 1024;  /* 100 MB */
    char* src = aligned_alloc(64, size);
    char* dst = aligned_alloc(64, size);

    memset(src, 0xAB, size);

    timer_t t;

    /* memcpy */
    t.name = "memcpy() (100MB)";
    timer_start(&t);
    memcpy(dst, src, size);
    timer_stop(&t);
    timer_report(&t);

    /* memset */
    t.name = "memset() (100MB)";
    timer_start(&t);
    memset(dst, 0, size);
    timer_stop(&t);
    timer_report(&t);

    /* memcmp */
    t.name = "memcmp() (100MB)";
    volatile int cmp = 0;
    timer_start(&t);
    cmp = memcmp(src, dst, size);
    timer_stop(&t);
    timer_report(&t);

    /* memset_explicit (C23) */
    t.name = "memset_explicit() (100MB)";
    timer_start(&t);
    memset_explicit(dst, 0, size);
    timer_stop(&t);
    timer_report(&t);

    free(src);
    free(dst);
    (void)cmp;
}

/* 测试 5: 矩阵乘法性能 */
void test_matrix_performance(void) {
    printf("\n=== 矩阵乘法性能测试 ===\n");

    const int N = 512;
    float* A = aligned_alloc(64, N * N * sizeof(float));
    float* B = aligned_alloc(64, N * N * sizeof(float));
    float* C = aligned_alloc(64, N * N * sizeof(float));

    /* 初始化 */
    for (int i = 0; i < N * N; i++) {
        A[i] = (float)(i % 100) / 100.0f;
        B[i] = (float)(i % 100) / 100.0f;
        C[i] = 0.0f;
    }

    timer_t t;

    /* 简单矩阵乘法 */
    t.name = "Matrix multiply (512x512) naive";
    timer_start(&t);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float sum = 0.0f;
            for (int k = 0; k < N; k++) {
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
    timer_stop(&t);
    timer_report(&t);

    free(A);
    free(B);
    free(C);
}

/* 测试 6: 时间函数性能 */
void test_time_performance(void) {
    printf("\n=== 时间函数性能测试 ===\n");

    timer_t t;
    struct timespec ts;

    /* timespec_get */
    t.name = "timespec_get()";
    timer_start(&t);
    for (int i = 0; i < 1000000; i++) {
        timespec_get(&ts, TIME_MONOTONIC);
    }
    timer_stop(&t);
    timer_report(&t);

    /* timespec_getres (C23) */
    t.name = "timespec_getres() (C23)";
    timer_start(&t);
    for (int i = 0; i < 1000000; i++) {
        timespec_getres(&ts, TIME_MONOTONIC);
    }
    timer_stop(&t);
    timer_report(&t);

    /* clock_gettime */
    t.name = "clock_gettime()";
    timer_start(&t);
    for (int i = 0; i < 1000000; i++) {
        clock_gettime(CLOCK_MONOTONIC, &ts);
    }
    timer_stop(&t);
    timer_report(&t);
}

/* 主函数 */
int main(void) {
    printf("========================================\n");
    printf("GCC 15 C23 性能测试套件\n");
    printf("编译器: GCC %d.%d.%d\n",
           __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
    printf("C 标准: C%ld\n", __STDC_VERSION__);
    printf("========================================\n");

    test_loop_vectorization();
    test_math_performance();
    test_string_performance();
    test_memory_performance();
    test_matrix_performance();
    test_time_performance();

    printf("\n========================================\n");
    printf("测试完成\n");
    printf("========================================\n");

    return 0;
}
