/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\README.md
 * Line: 254
 * Language: c
 * Block ID: ef6effaf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// bench/bench_matrix.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#define NTRIALS 10
#define WARMUP 3

typedef struct {
    const char* name;
    double min_time;
    double max_time;
    double avg_time;
    double std_dev;
    double gflops;
} bench_result;

static inline double get_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

bench_result benchmark_gemm(int m, int n, int k) {
    double *a = aligned_alloc(64, m * k * sizeof(double));
    double *b = aligned_alloc(64, k * n * sizeof(double));
    double *c = aligned_alloc(64, m * n * sizeof(double));

    // 初始化数据
    for (int i = 0; i < m*k; i++) a[i] = (double)rand() / RAND_MAX;
    for (int i = 0; i < k*n; i++) b[i] = (double)rand() / RAND_MAX;
    memset(c, 0, m*n*sizeof(double));

    double times[NTRIALS];

    for (int t = 0; t < NTRIALS + WARMUP; t++) {
        double start = get_time();

        // 矩阵乘法
        #pragma omp parallel for collapse(2)
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                double sum = 0.0;
                #pragma omp simd reduction(+:sum)
                for (int l = 0; l < k; l++) {
                    sum += a[i*k + l] * b[l*n + j];
                }
                c[i*n + j] = sum;
            }
        }

        double end = get_time();
        if (t >= WARMUP) {
            times[t - WARMUP] = end - start;
        }
    }

    // 计算统计信息
    bench_result res = {0};
    res.name = "GEMM";
    res.min_time = times[0];
    res.max_time = times[0];
    double sum = 0.0;

    for (int i = 0; i < NTRIALS; i++) {
        if (times[i] < res.min_time) res.min_time = times[i];
        if (times[i] > res.max_time) res.max_time = times[i];
        sum += times[i];
    }

    res.avg_time = sum / NTRIALS;

    // 计算标准差
    double var_sum = 0.0;
    for (int i = 0; i < NTRIALS; i++) {
        var_sum += (times[i] - res.avg_time) * (times[i] - res.avg_time);
    }
    res.std_dev = sqrt(var_sum / NTRIALS);

    // 计算 GFLOPS
    double ops = 2.0 * m * n * k;
    res.gflops = (ops / res.avg_time) / 1e9;

    free(a); free(b); free(c);
    return res;
}
