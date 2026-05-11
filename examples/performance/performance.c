/*
 * 性能优化案例：缓存行优化、分支预测、循环展开
 * 编译: gcc -Wall -Wextra -O2 -std=c11 -o performance performance.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

typedef struct { int x; } Item1;
typedef struct { int x; int padding[15]; } Item64;

#define N 10000000

/* 跨平台计时 */
static double now_ms(void) {
    return (double)clock() * 1000.0 / CLOCKS_PER_SEC;
}

/* ================================================================
 * CASE 1: 缓存行优化（False Sharing 避免）
 * ================================================================ */

void false_sharing_slow(Item1 *items, int count) {
    for (int r = 0; r < 100; r++) {
        for (int i = 0; i < count; i++) {
            items[i].x += 1;
        }
    }
}

void cache_line_fast(Item64 *items, int count) {
    for (int r = 0; r < 100; r++) {
        for (int i = 0; i < count; i++) {
            items[i].x += 1;
        }
    }
}

/* ================================================================
 * CASE 2: 分支预测友好化
 * ================================================================ */

int branch_unpredictable(const int *data, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        if (data[i] > 128) {
            sum += data[i];
        }
    }
    return sum;
}

int branchless(const int *data, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        int mask = -(data[i] > 128);
        sum += data[i] & mask;
    }
    return sum;
}

/* ================================================================
 * CASE 3: 循环展开 + 指针遍历
 * ================================================================ */

int sum_basic(const int *arr, int n) {
    int s = 0;
    for (int i = 0; i < n; i++) s += arr[i];
    return s;
}

int sum_unroll4(const int *arr, int n) {
    int s = 0;
    int i = 0;
    for (; i + 3 < n; i += 4) {
        s += arr[i] + arr[i+1] + arr[i+2] + arr[i+3];
    }
    for (; i < n; i++) s += arr[i];
    return s;
}

int sum_pointer(const int *arr, int n) {
    int s = 0;
    const int *end = arr + n;
    while (arr < end) s += *arr++;
    return s;
}

/* ================================================================
 * 基准测试
 * ================================================================ */

typedef struct { const char *name; double ms; } Result;

#define RUN_BENCH(results, count, nm, expr) do { \
    double t0 = now_ms(); \
    expr; \
    double t1 = now_ms(); \
    results[count].name = nm; \
    results[count].ms = t1 - t0; \
    count++; \
} while(0)

int main(void) {
    Result results[16];
    int r_count = 0;
    volatile int sink = 0;

    printf("=== Performance Optimization Cases ===\n\n");

    /* --- CASE 1 --- */
    printf("--- CASE 1: Cache Line Optimization ---\n");
    int count = 1000;
    Item1 *items1 = calloc(count, sizeof(Item1));
    Item64 *items64 = calloc(count, sizeof(Item64));

    RUN_BENCH(results, r_count, "False Sharing (Item1)", false_sharing_slow(items1, count));
    RUN_BENCH(results, r_count, "Cache Line Aligned (Item64)", cache_line_fast(items64, count));

    for (int i = r_count - 2; i < r_count; i++) {
        printf("  %-30s: %8.3f ms\n", results[i].name, results[i].ms);
    }
    printf("  Note: Multi-threaded false sharing causes 10x-100x slowdown\n\n");
    free(items1); free(items64);

    /* --- CASE 2 --- */
    printf("--- CASE 2: Branch Prediction ---\n");
    int *data = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) data[i] = rand() % 256;

    RUN_BENCH(results, r_count, "Random Branch", sink = branch_unpredictable(data, N));
    RUN_BENCH(results, r_count, "Branchless", sink = branchless(data, N));

    for (int i = r_count - 2; i < r_count; i++) {
        printf("  %-30s: %8.3f ms\n", results[i].name, results[i].ms);
    }
    printf("\n");
    free(data);

    /* --- CASE 3 --- */
    printf("--- CASE 3: Loop Unrolling ---\n");
    int *arr = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) arr[i] = i;

    RUN_BENCH(results, r_count, "Basic Loop", sink = sum_basic(arr, N));
    RUN_BENCH(results, r_count, "Unroll x4", sink = sum_unroll4(arr, N));
    RUN_BENCH(results, r_count, "Pointer Traverse", sink = sum_pointer(arr, N));

    for (int i = r_count - 3; i < r_count; i++) {
        printf("  %-30s: %8.3f ms\n", results[i].name, results[i].ms);
    }
    printf("\n");
    free(arr);

    (void)sink;
    return 0;
}
