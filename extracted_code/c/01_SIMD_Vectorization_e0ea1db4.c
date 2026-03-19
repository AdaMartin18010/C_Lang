/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 2221
 * Language: c
 * Block ID: e0ea1db4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 循环内条件分支
for (int i = 0; i < n; i++) {
    if (a[i] > threshold) {
        b[i] = sqrtf(a[i]);
    } else {
        b[i] = a[i] * a[i];
    }
}

// ✅ 解决方案1: 使用blend指令 (AVX-512)
#if defined(__AVX512F__)
__m512 vthresh = _mm512_set1_ps(threshold);
for (int i = 0; i < n; i += 16) {
    __m512 va = _mm512_loadu_ps(&a[i]);
    __mmask16 mask = _mm512_cmp_ps_mask(va, vthresh, _CMP_GT_OQ);
    __m512 vsqrt = _mm512_sqrt_ps(va);
    __m512 vsqr = _mm512_mul_ps(va, va);
    _mm512_storeu_ps(&b[i], _mm512_mask_blend_ps(mask, vsqr, vsqrt));
}
#endif

// ✅ 解决方案2: 分支预测友好的数据重排
// 先分离数据，分别处理，再合并
