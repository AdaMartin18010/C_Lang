/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 1097
 * Language: c
 * Block ID: 5012b0b6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * FIR滤波器SIMD实现
 * 广泛应用于5G脉冲整形、匹配滤波、信道化
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#if defined(__x86_64__)
    #include <immintrin.h>
#endif
#if defined(__aarch64__)
    #include <arm_neon.h>
#endif

#define FIR_TAPS 64  // 滤波器阶数
#define ALIGN64 __attribute__((aligned(64)))

// 转置系数用于向量化（FIR专用优化）
// 将系数按向量宽度重排，避免gather操作
void transpose_coeffs(const float *h, float *h_transposed, int taps, int vec_width) {
    int blocks = taps / vec_width;
    for (int b = 0; b < blocks; b++) {
        for (int i = 0; i < vec_width; i++) {
            h_transposed[b * vec_width + i] = h[b * vec_width + vec_width - 1 - i];
        }
    }
    // 复制剩余系数
    for (int i = blocks * vec_width; i < taps; i++) {
        h_transposed[i] = h[taps - 1 - i];
    }
}

/**
 * 标量FIR: y[n] = sum(h[k] * x[n-k])
 */
void fir_scalar(const float *x, const float *h, float *y, int len, int taps) {
    for (int n = 0; n < len; n++) {
        float sum = 0.0f;
        for (int k = 0; k < taps && n - k >= 0; k++) {
            sum += h[k] * x[n - k];
        }
        y[n] = sum;
    }
}

/**
 * NEON优化的FIR
 * 利用乘累加指令和循环展开
 */
#if defined(__aarch64__)
void fir_neon(const float *x, const float *h, float *y, int len, int taps) {
    // taps必须是4的倍数
    int taps_aligned = (taps + 3) & ~3;

    for (int n = taps - 1; n < len; n++) {
        float32x4_t sum_vec = vdupq_n_f32(0.0f);
        const float *x_ptr = &x[n - taps + 1];

        // 每次处理4个抽头
        for (int k = 0; k < taps_aligned; k += 4) {
            float32x4_t h_vec = vld1q_f32(&h[k]);
            float32x4_t x_vec = vld1q_f32(x_ptr + k);
            sum_vec = vfmaq_f32(sum_vec, h_vec, x_vec);
        }

        // 水平归约
        y[n] = vaddvq_f32(sum_vec);
    }
}
#endif

/**
 * AVX2 FIR优化版本
 * 8-way并行，支持非对齐加载
 */
#if defined(__AVX2__)
void fir_avx2(const float *x, const float *h, float *y, int len, int taps) {
    // taps必须是8的倍数
    int taps_aligned = (taps + 7) & ~7;

    for (int n = taps - 1; n < len; n++) {
        __m256 sum_vec = _mm256_setzero_ps();
        const float *x_ptr = &x[n - taps + 1];

        // 每次处理8个抽头
        for (int k = 0; k < taps_aligned; k += 8) {
            __m256 h_vec = _mm256_loadu_ps(&h[k]);
            __m256 x_vec = _mm256_loadu_ps(x_ptr + k);
            sum_vec = _mm256_fmadd_ps(h_vec, x_vec, sum_vec);
        }

        // 水平归约
        __m256 hsum = _mm256_hadd_ps(sum_vec, sum_vec);
        hsum = _mm256_hadd_ps(hsum, hsum);
        y[n] = _mm_cvtss_f32(_mm256_castps256_ps128(hsum)) +
               _mm_cvtss_f32(_mm256_extractf128_ps(hsum, 1));
    }
}
#endif

/**
 * 批处理FIR: 同时处理多个样本点
 * 利用SIMD的向量宽度并行计算多个输出
 */
#if defined(__AVX2__)
void fir_batch_avx2(const float *x, const float *h, float *y, int len, int taps) {
    // 每次处理8个输出样本
    for (int n = taps - 1; n <= len - 8; n += 8) {
        __m256 sum[8];
        for (int i = 0; i < 8; i++) sum[i] = _mm256_setzero_ps();

        for (int k = 0; k < taps; k++) {
            __m256 h_broadcast = _mm256_broadcast_ss(&h[k]);
            for (int i = 0; i < 8; i++) {
                __m256 x_vec = _mm256_loadu_ps(&x[n + i - taps + 1 + k]);
                sum[i] = _mm256_fmadd_ps(h_broadcast, x_vec, sum[i]);
            }
        }

        // 存储结果（简化版，实际需要归约）
        for (int i = 0; i < 8; i++) {
            y[n + i] = _mm_cvtss_f32(_mm256_castps256_ps128(sum[i]));
        }
    }
}
#endif

int main() {
    const int LEN = 10240;
    const int TAPS = 64;

    float *x, *h, *y1, *y2;
    posix_memalign((void**)&x, 64, LEN * sizeof(float));
    posix_memalign((void**)&h, 64, TAPS * sizeof(float));
    posix_memalign((void**)&y1, 64, LEN * sizeof(float));
    posix_memalign((void**)&y2, 64, LEN * sizeof(float));

    // 初始化: 根升余弦滤波器系数
    for (int i = 0; i < TAPS; i++) {
        float t = (i - TAPS/2) / 10.0f;
        h[i] = sinf(3.14159f * t) / (3.14159f * t + 0.0001f);
    }
    for (int i = 0; i < LEN; i++) {
        x[i] = sinf(i * 0.01f);
    }

    printf("=== FIR滤波器性能对比 ===\n");
    printf("输入长度: %d, 抽头数: %d\n\n", LEN, TAPS);

    struct timespec start, end;

    // 标量实现
    clock_gettime(CLOCK_MONOTONIC, &start);
    fir_scalar(x, h, y1, LEN, TAPS);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double t = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("标量FIR:    %.3f ms, %.2f MSPS\n", t * 1000, LEN / t / 1e6);

#if defined(__aarch64__)
    clock_gettime(CLOCK_MONOTONIC, &start);
    fir_neon(x, h, y2, LEN, TAPS);
    clock_gettime(CLOCK_MONOTONIC, &end);
    t = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("NEON FIR:   %.3f ms, %.2f MSPS\n", t * 1000, LEN / t / 1e6);
#endif

#if defined(__AVX2__)
    clock_gettime(CLOCK_MONOTONIC, &start);
    fir_avx2(x, h, y2, LEN, TAPS);
    clock_gettime(CLOCK_MONOTONIC, &end);
    t = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("AVX2 FIR:   %.3f ms, %.2f MSPS\n", t * 1000, LEN / t / 1e6);
#endif

    free(x); free(h); free(y1); free(y2);
    return 0;
}
