/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 2345
 * Language: c
 * Block ID: 0065b522
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 频繁的int<->float转换
for (int i = 0; i < n; i++) {
    float f = (float)i;  // 每次迭代转换
    a[i] = f * 0.5f;
}

// ✅ 正确: 预计算或使用SIMD整数到浮点转换
#if defined(__AVX2__)
__m256 v_scale = _mm256_set1_ps(0.5f);
__m256i v_idx = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
__m256i v_inc = _mm256_set1_epi32(8);

for (int i = 0; i < n; i += 8) {
    __m256 f = _mm256_cvtepi32_ps(v_idx);  // 8个整数转float
    _mm256_storeu_ps(&a[i], _mm256_mul_ps(f, v_scale));
    v_idx = _mm256_add_epi32(v_idx, v_inc);
}
#endif
