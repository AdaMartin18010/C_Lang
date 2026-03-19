/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\04_SIMD_Optimization_Case.md
 * Line: 906
 * Language: c
 * Block ID: ac298fde
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Cooley-Tukey FFT - SIMD优化

#include <complex.h>
#include <math.h>

#ifdef __AVX__
#include <immintrin.h>

// 复数乘法: (a+bi)(c+di) = (ac-bd) + (ad+bc)i
static inline void complex_mult_avx(__m256 a_re, __m256 a_im,
                                     __m256 b_re, __m256 b_im,
                                     __m256* out_re, __m256* out_im) {
    *out_re = _mm256_sub_ps(_mm256_mul_ps(a_re, b_re),
                            _mm256_mul_ps(a_im, b_im));
    *out_im = _mm256_add_ps(_mm256_mul_ps(a_re, b_im),
                            _mm256_mul_ps(a_im, b_re));
}

// 迭代式FFT (蝶形运算)
void fft_avx(complex float* x, int N) {
    // 位反转置换
    // ... (省略位反转代码)

    for (int len = 2; len <= N; len <<= 1) {
        float ang = 2 * M_PI / len;
        complex float wlen = cosf(ang) - I * sinf(ang);

        for (int i = 0; i < N; i += len) {
            complex float w = 1.0f;

            for (int j = 0; j < len / 2; j += 8) {
                // 加载8个复数
                __m256 w_re = _mm256_set1_ps(crealf(w));
                __m256 w_im = _mm256_set1_ps(cimagf(w));

                __m256 u_re = _mm256_load_ps((float*)&x[i + j]);
                __m256 u_im = _mm256_load_ps((float*)&x[i + j] + 8);

                __m256 v_re = _mm256_load_ps((float*)&x[i + j + len/2]);
                __m256 v_im = _mm256_load_ps((float*)&x[i + j + len/2] + 8);

                // v = v * w
                __m256 v_re_new, v_im_new;
                complex_mult_avx(v_re, v_im, w_re, w_im, &v_re_new, &v_im_new);

                // u + v, u - v
                __m256 x_re = _mm256_add_ps(u_re, v_re_new);
                __m256 x_im = _mm256_add_ps(u_im, v_im_new);
                __m256 y_re = _mm256_sub_ps(u_re, v_re_new);
                __m256 y_im = _mm256_sub_ps(u_im, v_im_new);

                // 存储
                _mm256_store_ps((float*)&x[i + j], x_re);
                _mm256_store_ps((float*)&x[i + j] + 8, x_im);
                _mm256_store_ps((float*)&x[i + j + len/2], y_re);
                _mm256_store_ps((float*)&x[i + j + len/2] + 8, y_im);

                // 更新w
                w *= wlen;
            }
        }
    }
}
#endif

// 性能对比 (1M点FFT)
// FFTW库:     1.0x (基准)
// 朴素实现:    0.15x
// AVX优化:     0.85x
