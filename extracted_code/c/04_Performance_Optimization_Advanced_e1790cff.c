/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 1282
 * Language: c
 * Block ID: e1790cff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file simd_basics.c
 * @brief SIMD基础操作与优化
 *
 * SSE (Streaming SIMD Extensions): 128位寄存器 (XMM0-XMM15)
 * AVX (Advanced Vector Extensions): 256位寄存器 (YMM0-YMM15)
 * AVX-512: 512位寄存器 (ZMM0-ZMM31)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* SSE头文件 */
#include <xmmintrin.h>    /* SSE  */
#include <emmintrin.h>    /* SSE2 */
#include <pmmintrin.h>    /* SSE3 */
#include <tmmintrin.h>    /* SSSE3 */
#include <smmintrin.h>    /* SSE4.1 */
#include <nmmintrin.h>    /* SSE4.2 */

/* AVX头文件 */
#include <immintrin.h>    /* AVX, AVX2, FMA */

/* 禁用优化以确保准确测量 */
#define NO_OPT __attribute__((optimize("-O0")))
#define NO_INLINE __attribute__((noinline))

/**
 * @brief 向量加法 - 标量 vs SSE vs AVX
 */
NO_INLINE void add_scalar(float *dst, const float *a, const float *b, int n) {
    for (int i = 0; i < n; i++) {
        dst[i] = a[i] + b[i];
    }
}

NO_INLINE void add_sse(float *dst, const float *a, const float *b, int n) {
    /* SSE处理128位 = 4个float */
    int i = 0;
    for (; i <= n - 4; i += 4) {
        /* 加载4个float到XMM寄存器 */
        __m128 va = _mm_loadu_ps(&a[i]);  /* unaligned load */
        __m128 vb = _mm_loadu_ps(&b[i]);
        /* 向量加法 */
        __m128 vr = _mm_add_ps(va, vb);
        /* 存储结果 */
        _mm_storeu_ps(&dst[i], vr);
    }
    /* 处理剩余元素 */
    for (; i < n; i++) {
        dst[i] = a[i] + b[i];
    }
}

NO_INLINE void add_avx(float *dst, const float *a, const float *b, int n) {
    /* AVX处理256位 = 8个float */
    int i = 0;
    for (; i <= n - 8; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vr = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&dst[i], vr);
    }
    /* 处理剩余 */
    for (; i < n; i++) {
        dst[i] = a[i] + b[i];
    }
}

NO_INLINE void add_avx_aligned(float *dst, const float *a, const float *b, int n) {
    /* 假设32字节对齐的AVX版本 */
    int i = 0;
    for (; i <= n - 8; i += 8) {
        __m256 va = _mm256_load_ps(&a[i]);   /* aligned load */
        __m256 vb = _mm256_load_ps(&b[i]);
        __m256 vr = _mm256_add_ps(va, vb);
        _mm256_store_ps(&dst[i], vr);        /* aligned store */
    }
    for (; i < n; i++) {
        dst[i] = a[i] + b[i];
    }
}

void benchmark_vector_add(void) {
    printf("=== Vector Addition Benchmark ===\n\n");

    const int N = 100000000;  /* 100M floats */
    float *a = aligned_alloc(32, N * sizeof(float));
    float *b = aligned_alloc(32, N * sizeof(float));
    float *dst = aligned_alloc(32, N * sizeof(float));

    /* 初始化 */
    for (int i = 0; i < N; i++) {
        a[i] = (float)i;
        b[i] = (float)(N - i);
    }

    /* 标量版本 */
    clock_t start = clock();
    add_scalar(dst, a, b, N);
    clock_t scalar_time = clock() - start;
    printf("Scalar:  %ld ms (%.2f GFLOPS)\n",
           scalar_time * 1000 / CLOCKS_PER_SEC,
           (N / 1000000000.0) / (scalar_time / (double)CLOCKS_PER_SEC));

    /* SSE版本 */
    start = clock();
    add_sse(dst, a, b, N);
    clock_t sse_time = clock() - start;
    printf("SSE:     %ld ms (%.2f GFLOPS) - %.2fx speedup\n",
           sse_time * 1000 / CLOCKS_PER_SEC,
           (N / 1000000000.0) / (sse_time / (double)CLOCKS_PER_SEC),
           (double)scalar_time / sse_time);

    /* AVX版本 */
    start = clock();
    add_avx(dst, a, b, N);
    clock_t avx_time = clock() - start;
    printf("AVX:     %ld ms (%.2f GFLOPS) - %.2fx speedup\n",
           avx_time * 1000 / CLOCKS_PER_SEC,
           (N / 1000000000.0) / (avx_time / (double)CLOCKS_PER_SEC),
           (double)scalar_time / avx_time);

    /* AVX对齐版本 */
    start = clock();
    add_avx_aligned(dst, a, b, N);
    clock_t avx_aligned_time = clock() - start;
    printf("AVX(align): %ld ms (%.2f GFLOPS) - %.2fx speedup\n",
           avx_aligned_time * 1000 / CLOCKS_PER_SEC,
           (N / 1000000000.0) / (avx_aligned_time / (double)CLOCKS_PER_SEC),
           (double)scalar_time / avx_aligned_time);

    free(a);
    free(b);
    free(dst);
}

/**
 * @brief 点积计算 - FMA优化
 *
 * FMA (Fused Multiply-Add): 单指令完成 a*b+c
 * 提高精度（只舍入一次）和吞吐（2 FLOPS/指令）
 */
NO_INLINE float dot_scalar(const float *a, const float *b, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

NO_INLINE float dot_sse(const float *a, const float *b, int n) {
    __m128 sum4 = _mm_setzero_ps();
    int i = 0;
    for (; i <= n - 4; i += 4) {
        __m128 va = _mm_loadu_ps(&a[i]);
        __m128 vb = _mm_loadu_ps(&b[i]);
        __m128 prod = _mm_mul_ps(va, vb);
        sum4 = _mm_add_ps(sum4, prod);
    }
    /* 水平求和 */
    float result[4];
    _mm_storeu_ps(result, sum4);
    float sum = result[0] + result[1] + result[2] + result[3];

    for (; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

NO_INLINE float dot_avx_fma(const float *a, const float *b, int n) {
    __m256 sum8 = _mm256_setzero_ps();
    int i = 0;
    for (; i <= n - 8; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        /* FMA: sum = va * vb + sum */
        sum8 = _mm256_fmadd_ps(va, vb, sum8);
    }
    /* 水平求和 - 将高128位加到低128位 */
    __m128 sum_lo = _mm256_castps256_ps128(sum8);
    __m128 sum_hi = _mm256_extractf128_ps(sum8, 1);
    __m128 sum4 = _mm_add_ps(sum_lo, sum_hi);

    float result[4];
    _mm_storeu_ps(result, sum4);
    float sum = result[0] + result[1] + result[2] + result[3];

    for (; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

void benchmark_dot_product(void) {
    printf("\n=== Dot Product Benchmark ===\n\n");

    const int N = 100000000;
    float *a = aligned_alloc(32, N * sizeof(float));
    float *b = aligned_alloc(32, N * sizeof(float));

    for (int i = 0; i < N; i++) {
        a[i] = (float)(i % 100) / 100.0f;
        b[i] = (float)((N - i) % 100) / 100.0f;
    }

    clock_t start = clock();
    volatile float r1 = dot_scalar(a, b, N);
    clock_t scalar_time = clock() - start;
    printf("Scalar:   %ld ms (result=%.6f)\n",
           scalar_time * 1000 / CLOCKS_PER_SEC, r1);

    start = clock();
    volatile float r2 = dot_sse(a, b, N);
    clock_t sse_time = clock() - start;
    printf("SSE:      %ld ms (result=%.6f, %.2fx)\n",
           sse_time * 1000 / CLOCKS_PER_SEC, r2,
           (double)scalar_time / sse_time);

    start = clock();
    volatile float r3 = dot_avx_fma(a, b, N);
    clock_t avx_time = clock() - start;
    printf("AVX+FMA:  %ld ms (result=%.6f, %.2fx)\n",
           avx_time * 1000 / CLOCKS_PER_SEC, r3,
           (double)scalar_time / avx_time);

    free(a);
    free(b);
}

/**
 * @brief 矩阵乘法 - SIMD优化
 *
 * C = A * B，其中A是MxK，B是KxN，C是MxN
 */
#define MATRIX_SIZE 1024

void matmul_scalar(float *C, const float *A, const float *B, int M, int N, int K) {
    memset(C, 0, M * N * sizeof(float));
    for (int i = 0; i < M; i++) {
        for (int k = 0; k < K; k++) {
            for (int j = 0; j < N; j++) {
                C[i * N + j] += A[i * K + k] * B[k * N + j];
            }
        }
    }
}

/* 分块矩阵乘法 + SIMD */
#define BLOCK_SIZE 64
void matmul_blocked_simd(float *C, const float *A, const float *B, int M, int N, int K) {
    memset(C, 0, M * N * sizeof(float));

    for (int ii = 0; ii < M; ii += BLOCK_SIZE) {
        for (int kk = 0; kk < K; kk += BLOCK_SIZE) {
            for (int jj = 0; jj < N; jj += BLOCK_SIZE) {
                /* 处理一个块 */
                int i_max = (ii + BLOCK_SIZE < M) ? ii + BLOCK_SIZE : M;
                int k_max = (kk + BLOCK_SIZE < K) ? kk + BLOCK_SIZE : K;
                int j_max = (jj + BLOCK_SIZE < N) ? jj + BLOCK_SIZE : N;

                for (int i = ii; i < i_max; i++) {
                    for (int k = kk; k < k_max; k++) {
                        float a_ik = A[i * K + k];
                        __m256 a_vec = _mm256_set1_ps(a_ik);  /* 广播 */

                        int j = jj;
                        /* SIMD处理8个元素 */
                        for (; j <= j_max - 8; j += 8) {
                            __m256 c_vec = _mm256_loadu_ps(&C[i * N + j]);
                            __m256 b_vec = _mm256_loadu_ps(&B[k * N + j]);
                            c_vec = _mm256_fmadd_ps(a_vec, b_vec, c_vec);
                            _mm256_storeu_ps(&C[i * N + j], c_vec);
                        }
                        /* 剩余元素 */
                        for (; j < j_max; j++) {
                            C[i * N + j] += a_ik * B[k * N + j];
                        }
                    }
                }
            }
        }
    }
}

void benchmark_matrix_mul(void) {
    printf("\n=== Matrix Multiplication Benchmark (%dx%d) ===\n\n",
           MATRIX_SIZE, MATRIX_SIZE);

    int n = MATRIX_SIZE;
    float *A = aligned_alloc(32, n * n * sizeof(float));
    float *B = aligned_alloc(32, n * n * sizeof(float));
    float *C = aligned_alloc(32, n * n * sizeof(float));

    /* 初始化 */
    for (int i = 0; i < n * n; i++) {
        A[i] = (float)(i % 10) / 10.0f;
        B[i] = (float)((i * 7) % 10) / 10.0f;
    }

    /* 标量版本 - 可能较慢，减少迭代 */
    printf("Running scalar version...\n");
    clock_t start = clock();
    matmul_scalar(C, A, B, n, n, n);
    clock_t scalar_time = clock() - start;
    printf("Scalar:      %ld ms\n", scalar_time * 1000 / CLOCKS_PER_SEC);

    /* SIMD版本 */
    printf("Running SIMD version...\n");
    start = clock();
    matmul_blocked_simd(C, A, B, n, n, n);
    clock_t simd_time = clock() - start;
    printf("SIMD(Block): %ld ms (%.2fx speedup)\n",
           simd_time * 1000 / CLOCKS_PER_SEC,
           (double)scalar_time / simd_time);

    free(A);
    free(B);
    free(C);
}

int main(void) {
    benchmark_vector_add();
    benchmark_dot_product();
    benchmark_matrix_mul();
    return 0;
}
