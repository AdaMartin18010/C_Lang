/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\02_Linux_Perf_Tutorial.md
 * Line: 1336
 * Language: c
 * Block ID: 9392f7f1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* matrix_optimized.c - 优化的矩阵乘法 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1024
#define BLOCK 64  /* 缓存块大小 */

/* 分块矩阵乘法 */
void matmul_blocked(double *a, double *b, double *c, int n, int block) {
    for (int ii = 0; ii < n; ii += block) {
        for (int jj = 0; jj < n; jj += block) {
            for (int kk = 0; kk < n; kk += block) {
                /* 块内计算 */
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

/* 转置优化版本 */
void matmul_transposed(double *a, double *b_transposed, double *c, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++) {
                /* 访问b_transposed[j][k]代替b[k][j]，提高空间局部性 */
                sum += a[i * n + k] * b_transposed[j * n + k];
            }
            c[i * n + j] = sum;
        }
    }
}

int main() {
    double *a = aligned_alloc(64, N * N * sizeof(double));
    double *b = aligned_alloc(64, N * N * sizeof(double));
    double *b_transposed = aligned_alloc(64, N * N * sizeof(double));
    double *c = aligned_alloc(64, N * N * sizeof(double));

    /* 初始化 */
    for (int i = 0; i < N * N; i++) {
        a[i] = (double)rand() / RAND_MAX;
        b[i] = (double)rand() / RAND_MAX;
        c[i] = 0;
    }

    /* 转置b矩阵 */
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            b_transposed[j * N + i] = b[i * N + j];
        }
    }

    printf("Running blocked matrix multiplication...\n");
    matmul_blocked(a, b, c, N, BLOCK);
    printf("Done\n");

    free(a); free(b); free(b_transposed); free(c);
    return 0;
}
