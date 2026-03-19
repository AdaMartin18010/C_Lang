/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 4038
 * Language: c
 * Block ID: a7fc1daf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file parallel_algorithms.c
 * @brief 并行算法设计与实现
 *
 * 使用OpenMP实现并行算法
 * 编译: gcc -fopenmp -O3 parallel_algorithms.c -o parallel_algorithms
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

/**
 * @brief 并行归约 - 求和
 */
long long parallel_sum(const int *arr, int n) {
    long long sum = 0;

    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }

    return sum;
}

/**
 * @brief 并行扫描（前缀和）
 */
void parallel_scan(int *dst, const int *src, int n) {
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int nthreads = omp_get_num_threads();

        /* 计算每个线程的范围 */
        int chunk = n / nthreads;
        int start = tid * chunk;
        int end = (tid == nthreads - 1) ? n : start + chunk;

        /* 线程内扫描 */
        dst[start] = src[start];
        for (int i = start + 1; i < end; i++) {
            dst[i] = dst[i-1] + src[i];
        }
    }

    /* 块间修正（串行） */
    int *corrections = malloc(omp_get_max_threads() * sizeof(int));
    corrections[0] = 0;
    int chunk = n / omp_get_max_threads();
    for (int t = 1; t < omp_get_max_threads(); t++) {
        corrections[t] = corrections[t-1] + dst[t * chunk - 1];
    }

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int start = tid * chunk;
        int end = (tid == omp_get_max_threads() - 1) ? n : start + chunk;

        for (int i = start; i < end; i++) {
            dst[i] += corrections[tid];
        }
    }

    free(corrections);
}

/**
 * @brief 并行快速排序
 */
void parallel_quicksort(int *arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[(left + right) / 2];
    int i = left, j = right;

    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            int tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
            j--;
        }
    }

    /* 任务并行递归 */
    #pragma omp task shared(arr)
    parallel_quicksort(arr, left, j);

    #pragma omp task shared(arr)
    parallel_quicksort(arr, i, right);
}

void parallel_sort(int *arr, int n) {
    #pragma omp parallel
    {
        #pragma omp single
        parallel_quicksort(arr, 0, n - 1);
    }
}

/**
 * @brief 并行矩阵乘法
 */
void matmul_parallel(float *C, const float *A, const float *B, int n) {
    #pragma omp parallel for collapse(2) schedule(static)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            float sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }
}

/* 优化版本：循环交换 + SIMD */
void matmul_parallel_optimized(float *C, const float *A, const float *B, int n) {
    memset(C, 0, n * n * sizeof(float));

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            float a = A[i * n + k];
            #pragma omp simd
            for (int j = 0; j < n; j++) {
                C[i * n + j] += a * B[k * n + j];
            }
        }
    }
}

void benchmark_parallel_algorithms(void) {
    printf("=== Parallel Algorithms Benchmark ===\n\n");

    const int N = 100 * 1024 * 1024;  /* 100M元素 */
    int *arr = malloc(N * sizeof(int));

    for (int i = 0; i < N; i++) {
        arr[i] = i + 1;
    }

    /* 串行求和 */
    clock_t start = clock();
    long long sum_serial = 0;
    for (int i = 0; i < N; i++) {
        sum_serial += arr[i];
    }
    clock_t serial_time = clock() - start;
    printf("Serial sum:   %ld ms (sum=%lld)\n",
           serial_time * 1000 / CLOCKS_PER_SEC, sum_serial);

    /* 并行求和 */
    start = clock();
    volatile long long sum_parallel = parallel_sum(arr, N);
    clock_t parallel_time = clock() - start;
    printf("Parallel sum: %ld ms (sum=%lld, %.2fx speedup with %d threads)\n",
           parallel_time * 1000 / CLOCKS_PER_SEC, sum_parallel,
           (double)serial_time / parallel_time, omp_get_max_threads());

    /* 矩阵乘法测试 */
    printf("\nMatrix multiplication (2048x2048):\n");
    int n = 2048;
    float *A = aligned_alloc(32, n * n * sizeof(float));
    float *B = aligned_alloc(32, n * n * sizeof(float));
    float *C = aligned_alloc(32, n * n * sizeof(float));

    for (int i = 0; i < n * n; i++) {
        A[i] = (float)(i % 10) / 10.0f;
        B[i] = (float)((i * 7) % 10) / 10.0f;
    }

    start = clock();
    matmul_parallel(C, A, B, n);
    clock_t matmul_time = clock() - start;
    printf("  Parallel:   %ld ms\n", matmul_time * 1000 / CLOCKS_PER_SEC);

    start = clock();
    matmul_parallel_optimized(C, A, B, n);
    clock_t opt_time = clock() - start;
    printf("  Optimized:  %ld ms (%.2fx)\n",
           opt_time * 1000 / CLOCKS_PER_SEC,
           (double)matmul_time / opt_time);

    free(arr);
    free(A);
    free(B);
    free(C);
}

int main(void) {
    benchmark_parallel_algorithms();
    return 0;
}
