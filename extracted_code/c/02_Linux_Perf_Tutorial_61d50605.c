/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\02_Linux_Perf_Tutorial.md
 * Line: 1243
 * Language: c
 * Block ID: 61d50605
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* matrix_naive.c - 朴素矩阵乘法 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1024

void matmul_naive(double *a, double *b, double *c, int n) {
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

int main() {
    double *a = aligned_alloc(64, N * N * sizeof(double));
    double *b = aligned_alloc(64, N * N * sizeof(double));
    double *c = aligned_alloc(64, N * N * sizeof(double));

    /* 初始化 */
    for (int i = 0; i < N * N; i++) {
        a[i] = (double)rand() / RAND_MAX;
        b[i] = (double)rand() / RAND_MAX;
        c[i] = 0;
    }

    printf("Running naive matrix multiplication...\n");
    matmul_naive(a, b, c, N);
    printf("Done\n");

    free(a); free(b); free(c);
    return 0;
}
