/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 2102
 * Language: c
 * Block ID: fb17e015
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 剩余元素处理策略
 */

// 方法1: 标量收尾 (最简单)
void epilogue_scalar(float *out, const float *in, int n) {
    // 主SIMD循环处理 n & ~3 个元素
    // ...

    // 标量处理剩余元素
    for (int i = n & ~3; i < n; i++) {
        out[i] = in[i] * 2.0f;
    }
}

// 方法2: 掩码加载 (AVX-512)
#if defined(__AVX512F__)
#include <immintrin.h>
void epilogue_mask(float *out, const float *in, int n) {
    int i = 0;
    for (; i <= n - 16; i += 16) {
        __m512 v = _mm512_loadu_ps(&in[i]);
        _mm512_storeu_ps(&out[i], _mm512_mul_ps(v, _mm512_set1_ps(2.0f)));
    }

    // 使用掩码处理剩余元素
    if (i < n) {
        __mmask16 mask = (1 << (n - i)) - 1;
        __m512 v = _mm512_maskz_loadu_ps(mask, &in[i]);
        _mm512_mask_storeu_ps(&out[i], mask, _mm512_mul_ps(v, _mm512_set1_ps(2.0f)));
    }
}
#endif

// 方法3: SVE谓词 (ARM)
#if defined(__ARM_FEATURE_SVE)
#include <arm_sve.h>
void epilogue_sve(float *out, const float *in, int n) {
    int i = 0;
    svfloat32_t v2 = svdup_f32(2.0f);

    while (i < n) {
        svbool_t pg = svwhilelt_b32(i, n);  // 生成谓词
        svfloat32_t v = svld1_f32(pg, &in[i]);
        svst1_f32(pg, &out[i], svmul_f32_x(pg, v, v2));
        i += svcntw();
    }
}
#endif
