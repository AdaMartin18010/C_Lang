/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\README.md
 * Line: 700
 * Language: c
 * Block ID: 54db9f57
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* nr_simd_x86.c - x86 SIMD实现 */
#include "nr_simd.h"

#ifdef USE_AVX
#include <immintrin.h>

/* AVX 256-bit复数乘法 (8个float = 4个复数) */
void simd_complex_multiply_avx(const cf_t *a, const cf_t *b, cf_t *c, uint32_t n) {
    const uint32_t simd_width = 4;  /* 256bit / 64bit per complex */
    uint32_t i = 0;

    for (; i + simd_width <= n; i += simd_width) {
        /* 加载4个复数 (8个float) */
        __m256 va = _mm256_loadu_ps((const float *)&a[i]);
        __m256 vb = _mm256_loadu_ps((const float *)&b[i]);

        /* 分离实部和虚部 */
        __m256 va_real = _mm256_shuffle_ps(va, va, _MM_SHUFFLE(2, 0, 2, 0));
        __m256 va_imag = _mm256_shuffle_ps(va, va, _MM_SHUFFLE(3, 1, 3, 1));
        __m256 vb_real = _mm256_shuffle_ps(vb, vb, _MM_SHUFFLE(2, 0, 2, 0));
        __m256 vb_imag = _mm256_shuffle_ps(vb, vb, _MM_SHUFFLE(3, 1, 3, 1));

        /* 复数乘法: (a+ib)(c+id) = (ac-bd) + i(ad+bc) */
        __m256 real_part = _mm256_sub_ps(
            _mm256_mul_ps(va_real, vb_real),
            _mm256_mul_ps(va_imag, vb_imag)
        );
        __m256 imag_part = _mm256_add_ps(
            _mm256_mul_ps(va_real, vb_imag),
            _mm256_mul_ps(va_imag, vb_real)
        );

        /* 交错合并结果 */
        __m256 vc = _mm256_unpacklo_ps(real_part, imag_part);
        _mm256_storeu_ps((float *)&c[i], vc);
    }

    /* 处理剩余元素 */
    for (; i < n; i++) {
        c[i] = a[i] * b[i];
    }
}

/* AVX能量计算 */
float simd_complex_energy_avx(const cf_t *a, uint32_t n) {
    const uint32_t simd_width = 8;  /* 8个float */
    __m256 sum_vec = _mm256_setzero_ps();
    uint32_t i = 0;

    for (; i + simd_width <= 2 * n; i += simd_width) {
        __m256 va = _mm256_loadu_ps((const float *)&a[i / 2]);
        sum_vec = _mm256_add_ps(sum_vec, _mm256_mul_ps(va, va));
    }

    /* 水平求和 */
    __m128 sum_lo = _mm256_castps256_ps128(sum_vec);
    __m128 sum_hi = _mm256_extractf128_ps(sum_vec, 1);
    __m128 sum = _mm_add_ps(sum_lo, sum_hi);
    sum = _mm_hadd_ps(sum, sum);
    sum = _mm_hadd_ps(sum, sum);

    float result = _mm_cvtss_f32(sum);

    /* 处理剩余元素 */
    for (; i / 2 < n; i += 2) {
        result += crealf(a[i / 2]) * crealf(a[i / 2]) +
                  cimagf(a[i / 2]) * cimagf(a[i / 2]);
    }

    return result;
}
#endif /* USE_AVX */

#ifdef USE_NEON
#include <arm_neon.h>

/* NEON复数乘法 */
void simd_complex_multiply_neon(const cf_t *a, const cf_t *b, cf_t *c, uint32_t n) {
    uint32_t i = 0;

    for (; i + 2 <= n; i += 2) {
        float32x4_t va = vld1q_f32((const float *)&a[i]);
        float32x4_t vb = vld1q_f32((const float *)&b[i]);

        /* NEON复数乘法 */
        float32x4_t vb_rev = vrev64q_f32(vb);
        float32x4_t real = vmulq_f32(va, vb);
        float32x4_t imag = vmulq_f32(va, vb_rev);

        /* 合并 */
        float32x2_t real_lo = vget_low_f32(real);
        float32x2_t real_hi = vget_high_f32(real);
        float32x2_t imag_lo = vget_low_f32(imag);
        float32x2_t imag_hi = vget_high_f32(imag);

        /* 实部: real - 交换后的imag */
        float32x2_t result_real = vsub_f32(real_lo, vrev64_f32(real_hi));
        /* 虚部: imag + 交换后的real */
        float32x2_t result_imag = vadd_f32(imag_lo, vrev64_f32(imag_hi));

        float32x4_t result = vcombine_f32(result_real, result_imag);
        vst1q_f32((float *)&c[i], result);
    }

    for (; i < n; i++) {
        c[i] = a[i] * b[i];
    }
}
#endif /* USE_NEON */
