/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\01_PMU_Architecture.md
 * Line: 1921
 * Language: c
 * Block ID: 2e3fb411
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* cache_analysis.c - 缓存性能分析案例 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <asm/unistd.h>

/* perf_event_open系统调用 */
static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

/* 配置事件 */
static int setup_event(struct perf_event_attr *attr, uint32_t type,
                       uint64_t config) {
    memset(attr, 0, sizeof(struct perf_event_attr));
    attr->type = type;
    attr->size = sizeof(struct perf_event_attr);
    attr->config = config;
    attr->disabled = 1;
    attr->exclude_kernel = 1;
    attr->exclude_hv = 1;
    return 0;
}

/* 顺序访问 (缓存友好) */
void sequential_access(int *arr, int n) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
}

/* 随机访问 (缓存不友好) */
void random_access(int *arr, int n, int *indices) {
    volatile int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[indices[i]];
    }
}

/* 步幅访问 (测试预取器) */
void stride_access(int *arr, int n, int stride) {
    volatile int sum = 0;
    for (int i = 0; i < n; i += stride) {
        sum += arr[i];
    }
}

/* 矩阵乘法 - 缓存优化前 */
void matrix_multiply_naive(double *a, double *b, double *c, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++) {
                sum += a[i * n + k] * b[k * n + j];
            }
            c[i * n + j] = sum;
        }
    }
}

/* 矩阵乘法 - 缓存优化后 (分块) */
void matrix_multiply_blocked(double *a, double *b, double *c, int n, int block) {
    for (int ii = 0; ii < n; ii += block) {
        for (int jj = 0; jj < n; jj += block) {
            for (int kk = 0; kk < n; kk += block) {
                for (int i = ii; i < ii + block && i < n; i++) {
                    for (int j = jj; j < jj + block && j < n; j++) {
                        double sum = c[i * n + j];
                        for (int k = kk; k < kk + block && k < n; k++) {
                            sum += a[i * n + k] * b[k * n + j];
                        }
                        c[i * n + j] = sum;
                    }
                }
            }
        }
    }
}

int main(void) {
    struct perf_event_attr attr;
    int fd;
    uint64_t val, enabled, running;

    const int N = 1024 * 1024;
    const int MATRIX_N = 512;

    printf("========== 缓存性能分析案例 ==========\n\n");

    /* 测试1: 顺序访问 vs 随机访问 */
    printf("测试1: 内存访问模式分析\n");
    printf("------------------------\n");

    int *arr = aligned_alloc(64, N * sizeof(int));
    int *indices = malloc(N * sizeof(int));

    /* 初始化数据 */
    for (int i = 0; i < N; i++) {
        arr[i] = i;
        indices[i] = rand() % N;
    }

    /* 配置L1数据缓存未命中事件 */
    setup_event(&attr, PERF_TYPE_HW_CACHE,
                (PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_READ << 8) |
                 (PERF_COUNT_HW_CACHE_RESULT_MISS << 16)));

    fd = perf_event_open(&attr, 0, -1, -1, 0);
    if (fd == -1) {
        perror("perf_event_open");
        return 1;
    }

    /* 顺序访问测试 */
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    sequential_access(arr, N);
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);

    read(fd, &val, sizeof(val));
    printf("顺序访问 - L1D缓存未命中: %" PRIu64 "\n", val);

    /* 随机访问测试 */
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    random_access(arr, N, indices);
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);

    read(fd, &val, sizeof(val));
    printf("随机访问 - L1D缓存未命中: %" PRIu64 "\n", val);

    close(fd);

    /* 测试2: 步幅访问分析 */
    printf("\n测试2: 步幅访问分析 (检测预取器行为)\n");
    printf("--------------------------------------\n");

    setup_event(&attr, PERF_TYPE_HW_CACHE,
                (PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_READ << 8) |
                 (PERF_COUNT_HW_CACHE_RESULT_MISS << 16)));

    fd = perf_event_open(&attr, 0, -1, -1, 0);

    for (int stride = 1; stride <= 64; stride *= 2) {
        ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
        stride_access(arr, N, stride);
        ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);

        read(fd, &val, sizeof(val));
        printf("步幅 %2d: L1D未命中 = %" PRIu64 "\n", stride, val);
    }

    close(fd);

    /* 测试3: 矩阵乘法优化前后对比 */
    printf("\n测试3: 矩阵乘法缓存优化\n");
    printf("------------------------\n");

    double *a = aligned_alloc(64, MATRIX_N * MATRIX_N * sizeof(double));
    double *b = aligned_alloc(64, MATRIX_N * MATRIX_N * sizeof(double));
    double *c = aligned_alloc(64, MATRIX_N * MATRIX_N * sizeof(double));

    /* 初始化矩阵 */
    for (int i = 0; i < MATRIX_N * MATRIX_N; i++) {
        a[i] = (double)rand() / RAND_MAX;
        b[i] = (double)rand() / RAND_MAX;
        c[i] = 0;
    }

    /* 配置LLC未命中事件 */
    setup_event(&attr, PERF_TYPE_HW_CACHE,
                (PERF_COUNT_HW_CACHE_LL | (PERF_COUNT_HW_CACHE_OP_READ << 8) |
                 (PERF_COUNT_HW_CACHE_RESULT_MISS << 16)));

    fd = perf_event_open(&attr, 0, -1, -1, 0);

    /* 朴素实现 */
    memset(c, 0, MATRIX_N * MATRIX_N * sizeof(double));
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    matrix_multiply_naive(a, b, c, MATRIX_N);
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    read(fd, &val, sizeof(val));
    printf("朴素实现 - LLC未命中: %" PRIu64 "\n", val);

    /* 分块优化实现 */
    memset(c, 0, MATRIX_N * MATRIX_N * sizeof(double));
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    matrix_multiply_blocked(a, b, c, MATRIX_N, 64);
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    read(fd, &val, sizeof(val));
    printf("分块优化 - LLC未命中: %" PRIu64 "\n", val);

    close(fd);

    /* 清理 */
    free(arr);
    free(indices);
    free(a);
    free(b);
    free(c);

    printf("\n========== 分析完成 ==========\n");

    return 0;
}
