/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 2031
 * Language: c
 * Block ID: 710d11d9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* AMD Zen 4 AVX-512 优化示例 */
#include <immintrin.h>
#include <stddef.h>

/* 512-bit 向量加法 */
void vector_add_znver4(float* result, const float* a,
                       const float* b, size_t n) {
    const size_t simd_width = 16;  /* 512-bit / 32-bit = 16 */

    size_t i = 0;
    /* 主循环: 每次处理 16 个 float */
    for (; i + simd_width <= n; i += simd_width) {
        __m512 va = _mm512_loadu_ps(&a[i]);
        __m512 vb = _mm512_loadu_ps(&b[i]);
        __m512 vr = _mm512_add_ps(va, vb);
        _mm512_storeu_ps(&result[i], vr);
    }

    /* 收尾处理 */
    for (; i < n; i++) {
        result[i] = a[i] + b[i];
    }
}

/* 512-bit 矩阵乘法优化 */
void matmul_512(float* C, const float* A, const float* B,
                int M, int N, int K) {
    const int block_size = 64;  /* L1 缓存优化 */

    for (int ii = 0; ii < M; ii += block_size) {
        for (int jj = 0; jj < N; jj += block_size) {
            for (int kk = 0; kk < K; kk += block_size) {
                /* 分块矩阵乘法 */
                int i_max = ii + block_size < M ? ii + block_size : M;
                int j_max = jj + block_size < N ? jj + block_size : N;
                int k_max = kk + block_size < K ? kk + block_size : K;

                for (int i = ii; i < i_max; i++) {
                    for (int j = jj; j < j_max; j += 16) {
                        __m512 c_vec = _mm512_loadu_ps(&C[i * N + j]);

                        for (int k = kk; k < k_max; k++) {
                            __m512 a_broadcast = _mm512_set1_ps(A[i * K + k]);
                            __m512 b_vec = _mm512_loadu_ps(&B[k * N + j]);
                            c_vec = _mm512_fmadd_ps(a_broadcast, b_vec, c_vec);
                        }

                        _mm512_storeu_ps(&C[i * N + j], c_vec);
                    }
                }
            }
        }
    }
}

/* VNNI 指令用于 AI/ML */
#include <immintrin.h>

/* 8-bit 整数点积 (VNNI) */
int32_t dot_product_vnni(const int8_t* a, const int8_t* b, size_t n) {
    __m512i sum = _mm512_setzero_si512();

    size_t i = 0;
    /* 每次处理 64 字节 (512-bit) */
    for (; i + 64 <= n; i += 64) {
        __m512i va = _mm512_loadu_si512((__m512i*)&a[i]);
        __m512i vb = _mm512_loadu_si512((__m512i*)&b[i]);
        sum = _mm512_dpbusds_epi32(sum, va, vb);
    }

    /* 水平求和 */
    int32_t result[16];
    _mm512_storeu_si512((__m512i*)result, sum);

    int32_t total = 0;
    for (int j = 0; j < 16; j++) {
        total += result[j];
    }

    /* 收尾 */
    for (; i < n; i++) {
        total += (int32_t)a[i] * (int32_t)b[i];
    }

    return total;
}
