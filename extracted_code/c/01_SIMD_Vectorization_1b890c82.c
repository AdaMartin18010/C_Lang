/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 1283
 * Language: c
 * Block ID: 1b890c82
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * FFT蝶形运算向量化实现
 * 5G NR要求支持FFT大小: 128, 256, 512, 1024, 2048, 4096, 8192
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <time.h>

#if defined(__x86_64__)
    #include <immintrin.h>
#endif
#if defined(__aarch64__)
    #include <arm_neon.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 复数类型定义
typedef struct {
    float re;
    float im;
} cplx_float;

/**
 * 位反转置换
 */
void bit_reverse(cplx_float *x, int n) {
    int j = 0;
    for (int i = 0; i < n - 1; i++) {
        if (i < j) {
            cplx_float temp = x[i];
            x[i] = x[j];
            x[j] = temp;
        }
        int k = n >> 1;
        while (k <= j) {
            j -= k;
            k >>= 1;
        }
        j += k;
    }
}

/**
 * 基2蝶形运算: (X', Y') = (X + W*Y, X - W*Y)
 * 其中W = e^(-j*2*PI*k/N) 是旋转因子
 */
void butterfly_scalar(cplx_float *x, cplx_float *y, float wr, float wi) {
    // temp = W * Y
    float tr = wr * y->re - wi * y->im;
    float ti = wr * y->im + wi * y->re;

    // Y' = X - temp
    float yr = x->re - tr;
    float yi = x->im - ti;

    // X' = X + temp
    x->re = x->re + tr;
    x->im = x->im + ti;
    y->re = yr;
    y->im = yi;
}

/**
 * NEON优化的4路并行蝶形运算
 */
#if defined(__aarch64__)
void butterfly4_neon(float32x4_t *xr, float32x4_t *xi,
                     float32x4_t *yr, float32x4_t *yi,
                     float32x4_t wr, float32x4_t wi) {
    // temp = W * Y
    // tr = wr*yr - wi*yi
    float32x4_t tr = vmlsq_f32(vmulq_f32(wr, *yr), wi, *yi);
    // ti = wr*yi + wi*yr
    float32x4_t ti = vmlaq_f32(vmulq_f32(wr, *yi), wi, *yr);

    // Y' = X - temp
    *yr = vsubq_f32(*xr, tr);
    *yi = vsubq_f32(*xi, ti);

    // X' = X + temp
    *xr = vaddq_f32(*xr, tr);
    *xi = vaddq_f32(*xi, ti);
}

void fft_neon(cplx_float *x, int n) {
    bit_reverse(x, n);

    // FFT阶段迭代
    for (int stage = 1; stage < n; stage <<= 1) {
        float angle = -(float)M_PI / stage;
        float wpr = cosf(angle * 2);
        float wpi = sinf(angle * 2);

        for (int group = 0; group < n; group += (stage << 1)) {
            float wr = 1.0f;
            float wi = 0.0f;

            // 每次处理4个蝶形
            for (int butterfly = 0; butterfly < stage; butterfly += 4) {
                int i = group + butterfly;

                // 加载4个复数X和4个复数Y
                float xr_vals[4], xi_vals[4], yr_vals[4], yi_vals[4];
                float wr_vals[4], wi_vals[4];

                for (int b = 0; b < 4 && butterfly + b < stage; b++) {
                    xr_vals[b] = x[i + b].re;
                    xi_vals[b] = x[i + b].im;
                    yr_vals[b] = x[i + b + stage].re;
                    yi_vals[b] = x[i + b + stage].im;

                    // 计算旋转因子
                    float angle_b = angle * (butterfly + b);
                    wr_vals[b] = cosf(angle_b);
                    wi_vals[b] = sinf(angle_b);
                }

                float32x4_t xr = vld1q_f32(xr_vals);
                float32x4_t xi = vld1q_f32(xi_vals);
                float32x4_t yr = vld1q_f32(yr_vals);
                float32x4_t yi = vld1q_f32(yi_vals);
                float32x4_t wr_v = vld1q_f32(wr_vals);
                float32x4_t wi_v = vld1q_f32(wi_vals);

                butterfly4_neon(&xr, &xi, &yr, &yi, wr_v, wi_v);

                // 存储结果
                vst1q_f32(xr_vals, xr);
                vst1q_f32(xi_vals, xi);
                vst1q_f32(yr_vals, yr);
                vst1q_f32(yi_vals, yi);

                for (int b = 0; b < 4 && butterfly + b < stage; b++) {
                    x[i + b].re = xr_vals[b];
                    x[i + b].im = xi_vals[b];
                    x[i + b + stage].re = yr_vals[b];
                    x[i + b + stage].im = yi_vals[b];
                }
            }
        }
    }
}
#endif

/**
 * AVX-512优化的16路并行蝶形运算
 */
#if defined(__AVX512F__)
void butterfly16_avx512(__m512 *xr, __m512 *xi,
                        __m512 *yr, __m512 *yi,
                        __m512 wr, __m512 wi) {
    // temp_real = wr*yr - wi*yi (FMSUB)
    __m512 tr = _mm512_fmsub_ps(wr, *yr, _mm512_mul_ps(wi, *yi));
    // temp_imag = wr*yi + wi*yr (FMADD)
    __m512 ti = _mm512_fmadd_ps(wr, *yi, _mm512_mul_ps(wi, *yr));

    // Y' = X - temp
    *yr = _mm512_sub_ps(*xr, tr);
    *yi = _mm512_sub_ps(*xi, ti);

    // X' = X + temp
    *xr = _mm512_add_ps(*xr, tr);
    *xi = _mm512_add_ps(*xi, ti);
}
#endif

// 标量FFT完整实现
void fft_scalar(cplx_float *x, int n) {
    bit_reverse(x, n);

    for (int stage = 1; stage < n; stage <<= 1) {
        float angle = -(float)M_PI / stage;
        float wpr = cosf(angle * 2);
        float wpi = sinf(angle * 2);

        for (int group = 0; group < n; group += (stage << 1)) {
            float wr = 1.0f;
            float wi = 0.0f;

            for (int butterfly = 0; butterfly < stage; butterfly++) {
                int i = group + butterfly;
                butterfly_scalar(&x[i], &x[i + stage], wr, wi);

                float temp = wr;
                wr = wr * wpr - wi * wpi;
                wi = wi * wpr + temp * wpi;
            }
        }
    }
}

int main() {
    const int N = 4096;  // 5G NR支持的最大FFT大小

    cplx_float *x1, *x2;
    posix_memalign((void**)&x1, 64, N * sizeof(cplx_float));
    posix_memalign((void**)&x2, 64, N * sizeof(cplx_float));

    // 初始化测试信号
    for (int i = 0; i < N; i++) {
        x1[i].re = cosf(2 * M_PI * i / N);
        x1[i].im = sinf(2 * M_PI * i / N);
        x2[i] = x1[i];
    }

    printf("=== FFT性能对比 (N=%d) ===\n\n", N);

    struct timespec start, end;

    // 标量FFT
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int iter = 0; iter < 100; iter++) {
        fft_scalar(x1, N);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double t = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("标量FFT:   %.3f ms, %.1f FFTs/sec\n", t, 100.0 / t);

#if defined(__aarch64__)
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int iter = 0; iter < 100; iter++) {
        fft_neon(x2, N);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    t = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("NEON FFT:  %.3f ms, %.1f FFTs/sec\n", t, 100.0 / t);
#endif

    free(x1); free(x2);
    return 0;
}
