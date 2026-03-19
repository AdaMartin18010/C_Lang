/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 2198
 * Language: c
 * Block ID: cba0e21c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 循环内调用非内联函数
for (int i = 0; i < n; i++) {
    c[i] = sqrtf(a[i] * a[i] + b[i] * b[i]);  // sqrtf可能阻止向量化
}

// ✅ 解决方案1: 使用SIMD内在函数
#if defined(__AVX2__)
for (int i = 0; i < n; i += 8) {
    __m256 a_vec = _mm256_loadu_ps(&a[i]);
    __m256 b_vec = _mm256_loadu_ps(&b[i]);
    __m256 sum = _mm256_add_ps(_mm256_mul_ps(a_vec, a_vec),
                                _mm256_mul_ps(b_vec, b_vec));
    _mm256_storeu_ps(&c[i], _mm256_sqrt_ps(sum));
}
#endif

// ✅ 解决方案2: 使用fast-math编译选项
// gcc -O3 -ffast-math -ftree-vectorize
