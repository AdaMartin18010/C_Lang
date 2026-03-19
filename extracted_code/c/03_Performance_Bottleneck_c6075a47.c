/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\03_Performance_Bottleneck.md
 * Line: 657
 * Language: c
 * Block ID: c6075a47
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// simd_optimization.c
#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>  // AVX/SSE头文件
#include "benchmark.h"

#define ARRAY_SIZE 10000000

// 标量版本
float dot_product_scalar(float *a, float *b, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

// SSE版本 (4 floats per operation)
float dot_product_sse(float *a, float *b, int n) {
    __m128 sum_vec = _mm_setzero_ps();
    int i;

    for (i = 0; i <= n - 4; i += 4) {
        __m128 a_vec = _mm_loadu_ps(&a[i]);
        __m128 b_vec = _mm_loadu_ps(&b[i]);
        sum_vec = _mm_add_ps(sum_vec, _mm_mul_ps(a_vec, b_vec));
    }

    // 水平求和
    float result[4];
    _mm_storeu_ps(result, sum_vec);
    float sum = result[0] + result[1] + result[2] + result[3];

    // 处理剩余元素
    for (; i < n; i++) {
        sum += a[i] * b[i];
    }

    return sum;
}

// AVX版本 (8 floats per operation)
float dot_product_avx(float *a, float *b, int n) {
    __m256 sum_vec = _mm256_setzero_ps();
    int i;

    for (i = 0; i <= n - 8; i += 8) {
        __m256 a_vec = _mm256_loadu_ps(&a[i]);
        __m256 b_vec = _mm256_loadu_ps(&b[i]);
        sum_vec = _mm256_add_ps(sum_vec, _mm256_mul_ps(a_vec, b_vec));
    }

    // 水平求和
    float result[8];
    _mm256_storeu_ps(result, sum_vec);
    float sum = result[0] + result[1] + result[2] + result[3] +
                result[4] + result[5] + result[6] + result[7];

    for (; i < n; i++) {
        sum += a[i] * b[i];
    }

    return sum;
}

// AVX + FMA版本 (融合乘加)
float dot_product_avx_fma(float *a, float *b, float *c, int n) {
    __m256 sum_vec = _mm256_setzero_ps();
    int i;

    for (i = 0; i <= n - 8; i += 8) {
        __m256 a_vec = _mm256_loadu_ps(&a[i]);
        __m256 b_vec = _mm256_loadu_ps(&b[i]);
        // FMA: sum = a * b + sum (单指令)
        sum_vec = _mm256_fmadd_ps(a_vec, b_vec, sum_vec);
    }

    float result[8];
    _mm256_storeu_ps(result, sum_vec);
    float sum = result[0] + result[1] + result[2] + result[3] +
                result[4] + result[5] + result[6] + result[7];

    for (; i < n; i++) {
        sum += a[i] * b[i];
    }

    return sum;
}

int main(void) {
    // 32字节对齐以支持AVX
    float *a = aligned_alloc(32, ARRAY_SIZE * sizeof(float));
    float *b = aligned_alloc(32, ARRAY_SIZE * sizeof(float));

    for (int i = 0; i < ARRAY_SIZE; i++) {
        a[i] = (float)(i % 100) / 100.0f;
        b[i] = (float)((i + 50) % 100) / 100.0f;
    }

    printf("=== SIMD Optimization Test ===\n");
    BENCHMARK((void)dot_product_scalar(a, b, ARRAY_SIZE), 100);
    BENCHMARK((void)dot_product_sse(a, b, ARRAY_SIZE), 100);
    BENCHMARK((void)dot_product_avx(a, b, ARRAY_SIZE), 100);

    free(a);
    free(b);
    return 0;
}
