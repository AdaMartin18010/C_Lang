/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\03_Performance_Bottleneck.md
 * Line: 479
 * Language: c
 * Block ID: 9a8a9543
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// cache_optimization.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "benchmark.h"

#define MATRIX_SIZE 2048

// 原始版本: 缓存不友好
void matrix_multiply_naive(double *A, double *B, double *C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * n + j];  // B的访问不连续
            }
            C[i * n + j] = sum;
        }
    }
}

// 优化版本1: 矩阵转置
void matrix_multiply_transpose(double *A, double *B, double *C, int n) {
    // 转置B矩阵
    double *BT = malloc(n * n * sizeof(double));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            BT[j * n + i] = B[i * n + j];
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * BT[j * n + k];  // 连续访问
            }
            C[i * n + j] = sum;
        }
    }
    free(BT);
}

// 优化版本2: 分块算法 (Cache Blocking)
#define BLOCK_SIZE 64

void matrix_multiply_blocked(double *A, double *B, double *C, int n) {
    memset(C, 0, n * n * sizeof(double));

    for (int ii = 0; ii < n; ii += BLOCK_SIZE) {
        for (int jj = 0; jj < n; jj += BLOCK_SIZE) {
            for (int kk = 0; kk < n; kk += BLOCK_SIZE) {
                // 处理块
                int i_max = (ii + BLOCK_SIZE < n) ? ii + BLOCK_SIZE : n;
                int j_max = (jj + BLOCK_SIZE < n) ? jj + BLOCK_SIZE : n;
                int k_max = (kk + BLOCK_SIZE < n) ? kk + BLOCK_SIZE : n;

                for (int i = ii; i < i_max; i++) {
                    for (int k = kk; k < k_max; k++) {
                        double a = A[i * n + k];
                        for (int j = jj; j < j_max; j++) {
                            C[i * n + j] += a * B[k * n + j];
                        }
                    }
                }
            }
        }
    }
}

int main(void) {
    int n = 1024;  // 使用较小尺寸用于测试
    double *A = aligned_alloc(64, n * n * sizeof(double));
    double *B = aligned_alloc(64, n * n * sizeof(double));
    double *C = aligned_alloc(64, n * n * sizeof(double));

    // 初始化
    for (int i = 0; i < n * n; i++) {
        A[i] = (double)rand() / RAND_MAX;
        B[i] = (double)rand() / RAND_MAX;
    }

    printf("=== Cache Optimization Test (n=%d) ===\n", n);
    // BENCHMARK(matrix_multiply_naive(A, B, C, n), 1);  // 太慢
    BENCHMARK(matrix_multiply_transpose(A, B, C, n), 1);
    BENCHMARK(matrix_multiply_blocked(A, B, C, n), 1);

    free(A);
    free(B);
    free(C);

    return 0;
}
