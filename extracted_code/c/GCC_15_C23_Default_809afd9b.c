/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 723
 * Language: c
 * Block ID: 809afd9b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* glibc 2.43 FMA 优化示例 */
#include <math.h>
#include <stdio.h>
#include <time.h>

/* 编译: gcc -O3 -march=znver4 -mfma -o fma_test fma_test.c -lm */

/* 多项式求值 - FMA 优化 */
double polynomial_fma(double x) {
    /* 使用 FMA 计算多项式 */
    /* 传统: result = a + b * c (两次舍入) */
    /* FMA:  result = fma(b, c, a) (一次舍入) */

    double coeffs[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double result = coeffs[4];

    for (int i = 3; i >= 0; i--) {
        result = fma(result, x, coeffs[i]);
    }

    return result;
}

/* 传统多项式求值 */
double polynomial_traditional(double x) {
    double coeffs[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double result = coeffs[4];

    for (int i = 3; i >= 0; i--) {
        result = result * x + coeffs[i];
    }

    return result;
}

/* 矩阵乘法 - FMA 优化 */
void matmul_fma(double* C, const double* A, const double* B,
                int M, int N, int K) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < K; k++) {
                /* 使用 FMA 累加 */
                sum = fma(A[i * K + k], B[k * N + j], sum);
            }
            C[i * N + j] = sum;
        }
    }
}

/* 传统矩阵乘法 */
void matmul_traditional(double* C, const double* A, const double* B,
                        int M, int N, int K) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < K; k++) {
                sum += A[i * K + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}

/* 性能测试 */
void benchmark(void) {
    const int N = 1000000;
    const int MAT_SIZE = 256;

    clock_t start, end;
    double cpu_time;

    /* 测试数学函数 */
    printf("=== 数学函数性能测试 ===\n");

    volatile double result = 0.0;

    start = clock();
    for (int i = 0; i < N; i++) {
        result = sin(0.5);
    }
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("sin(): %.3f sec (result=%.6f)\n", cpu_time, result);

    start = clock();
    for (int i = 0; i < N; i++) {
        result = exp(1.0);
    }
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("exp(): %.3f sec (result=%.6f)\n", cpu_time, result);

    /* 测试多项式 */
    printf("\n=== 多项式性能测试 ===\n");

    start = clock();
    for (int i = 0; i < N; i++) {
        result = polynomial_traditional(2.0);
    }
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("传统多项式: %.3f sec\n", cpu_time);

    start = clock();
    for (int i = 0; i < N; i++) {
        result = polynomial_fma(2.0);
    }
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("FMA 多项式: %.3f sec\n", cpu_time);
}

int main(void) {
    benchmark();
    return 0;
}
