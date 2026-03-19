/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 1711
 * Language: c
 * Block ID: cf8f2a9c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 5G NR自适应调制编码(AMC) SIMD优化
 * 包括: SNR估计、CQI计算、MCS选择
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <time.h>

#if defined(__x86_64__)
    #include <immintrin.h>
#endif
#if defined(__aarch64__)
    #include <arm_neon.h>
#endif

// 5G NR调制阶数: QPSK=2, 16QAM=4, 64QAM=6, 256QAM=8
typedef enum { MOD_QPSK = 2, MOD_16QAM = 4, MOD_64QAM = 6, MOD_256QAM = 8 } ModOrder;

/**
 * 基于信道估计的SNR计算
 * SNR = |H|^2 / sigma^2
 */
void calculate_snr_scalar(const float *h_re, const float *h_im,
                          float sigma2, float *snr, int n) {
    for (int i = 0; i < n; i++) {
        float h2 = h_re[i] * h_re[i] + h_im[i] * h_im[i];
        snr[i] = 10.0f * log10f(h2 / sigma2 + 1e-10f);
    }
}

/**
 * NEON优化的SNR计算
 */
#if defined(__aarch64__)
float32x4_t logvq_f32(float32x4_t x) {
    // 简化实现: 使用vgetq_lane + scalar log
    float vals[4];
    vst1q_f32(vals, x);
    for (int i = 0; i < 4; i++) vals[i] = logf(vals[i]);
    return vld1q_f32(vals);
}

void calculate_snr_neon(const float *h_re, const float *h_im,
                        float sigma2, float *snr, int n) {
    float32x4_t v_sigma2 = vdupq_n_f32(sigma2);
    float32x4_t v_epsilon = vdupq_n_f32(1e-10f);
    float32x4_t v_ten = vdupq_n_f32(10.0f);
    float32x4_t v_log10e = vdupq_n_f32(log10f(expf(1.0f)));  // 1/ln(10)

    int i = 0;
    for (; i <= n - 4; i += 4) {
        float32x4_t h_r = vld1q_f32(&h_re[i]);
        float32x4_t h_i = vld1q_f32(&h_im[i]);

        // |H|^2 = h_r^2 + h_i^2
        float32x4_t h2 = vmlaq_f32(vmulq_f32(h_r, h_r), h_i, h_i);

        // SNR线性值 = |H|^2 / sigma^2
        float32x4_t snr_lin = vdivq_f32(h2, v_sigma2);
        snr_lin = vaddq_f32(snr_lin, v_epsilon);

        // log10(x) = ln(x) / ln(10) ≈ ln(x) * 0.4343
        // 使用近似: log2(x) * log10(2)
        float32x4_t snr_log = vmulq_f32(logvq_f32(snr_lin), v_log10e);
        snr_log = vmulq_f32(snr_log, v_ten);

        vst1q_f32(&snr[i], snr_log);
    }

    // 尾部处理
    for (; i < n; i++) {
        float h2 = h_re[i] * h_re[i] + h_im[i] * h_im[i];
        snr[i] = 10.0f * log10f(h2 / sigma2 + 1e-10f);
    }
}
#endif

/**
 * CQI到MCS的映射表 (简化版)
 * 3GPP TS 38.214 Table 5.1.3.1-1
 */
static const uint8_t cqi_to_mcs[16] = {
    0, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 28
};

static const ModOrder cqi_to_mod[16] = {
    MOD_QPSK, MOD_QPSK, MOD_QPSK, MOD_QPSK,
    MOD_16QAM, MOD_16QAM, MOD_16QAM, MOD_16QAM,
    MOD_64QAM, MOD_64QAM, MOD_64QAM, MOD_64QAM,
    MOD_256QAM, MOD_256QAM, MOD_256QAM, MOD_256QAM
};

/**
 * SNR到CQI的映射 (简化线性映射)
 * 实际系统使用更复杂的EESM或MI映射
 */
uint8_t snr_to_cqi_scalar(float snr) {
    if (snr < -6.0f) return 0;
    if (snr > 20.0f) return 15;
    return (uint8_t)((snr + 6.0f) * 15.0f / 26.0f);
}

/**
 * 向量化CQI计算
 */
#if defined(__AVX2__)
void snr_to_cqi_avx2(const float *snr, uint8_t *cqi, int n) {
    __m256 v_min = _mm256_set1_ps(-6.0f);
    __m256 v_max = _mm256_set1_ps(20.0f);
    __m256 v_scale = _mm256_set1_ps(15.0f / 26.0f);
    __m256 v_offset = _mm256_set1_ps(6.0f);

    int i = 0;
    for (; i <= n - 8; i += 8) {
        __m256 v_snr = _mm256_loadu_ps(&snr[i]);

        // 裁剪
        v_snr = _mm256_max_ps(v_snr, v_min);
        v_snr = _mm256_min_ps(v_snr, v_max);

        // 映射到CQI
        __m256 v_cqi = _mm256_mul_ps(_mm256_add_ps(v_snr, v_offset), v_scale);

        // 转换为整数
        __m256i v_cqi_i = _mm256_cvtps_epi32(v_cqi);

        // 存储 (需要pack)
        // 简化: 单独存储
        int cqi_vals[8];
        _mm256_storeu_si256((__m256i*)cqi_vals, v_cqi_i);
        for (int j = 0; j < 8; j++) cqi[i+j] = (uint8_t)cqi_vals[j];
    }

    for (; i < n; i++) cqi[i] = snr_to_cqi_scalar(snr[i]);
}
#endif

/**
 * 有效SNR计算 (指数有效SINR映射, EESM)
 * 用于宽带CQI反馈
 */
float effective_snr_scalar(const float *snr, int n, float beta) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += expf(-snr[i] / beta);
    }
    return -beta * logf(sum / n);
}

int main() {
    const int N_RB = 273;  // 最大RB数
    const int N_SC = N_RB * 12;  // 子载波数

    float *h_re, *h_im, *snr;
    uint8_t *cqi;

    posix_memalign((void**)&h_re, 64, N_SC * sizeof(float));
    posix_memalign((void**)&h_im, 64, N_SC * sizeof(float));
    posix_memalign((void**)&snr, 64, N_SC * sizeof(float));
    posix_memalign((void**)&cqi, 64, N_SC * sizeof(uint8_t));

    // 初始化信道系数
    for (int i = 0; i < N_SC; i++) {
        h_re[i] = (float)rand() / RAND_MAX;
        h_im[i] = (float)rand() / RAND_MAX;
    }
    float sigma2 = 0.1f;

    printf("=== 5G NR AMC性能测试 ===\n");
    printf("RB数: %d, 子载波数: %d\n\n", N_RB, N_SC);

    struct timespec start, end;

    // 标量SNR计算
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int iter = 0; iter < 10000; iter++) {
        calculate_snr_scalar(h_re, h_im, sigma2, snr, N_SC);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double t = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("标量SNR计算:  %.3f ms, %.1f Mops/sec\n", t, 10000.0 * N_SC / t / 1e6);

#if defined(__aarch64__)
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int iter = 0; iter < 10000; iter++) {
        calculate_snr_neon(h_re, h_im, sigma2, snr, N_SC);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    t = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("NEON SNR计算: %.3f ms, %.1f Mops/sec\n", t, 10000.0 * N_SC / t / 1e6);
#endif

    // 有效SNR计算
    clock_gettime(CLOCK_MONOTONIC, &start);
    float eff_snr = effective_snr_scalar(snr, N_SC, 1.0f);
    clock_gettime(CLOCK_MONOTONIC, &end);
    t = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("\n有效SNR: %.2f dB (计算时间: %.3f ms)\n", eff_snr, t * 1000);

    free(h_re); free(h_im); free(snr); free(cqi);
    return 0;
}
