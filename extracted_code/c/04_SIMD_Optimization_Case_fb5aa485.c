/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\04_SIMD_Optimization_Case.md
 * Line: 771
 * Language: c
 * Block ID: fb5aa485
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 循环展开减少循环开销，提高指令级并行性

// 4x循环展开版本
void vec_add_unrolled(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;

    // 每次处理32个float (4 x 8)
    for (; i + 32 <= n; i += 32) {
        __m256 va0 = _mm256_load_ps(&a[i + 0]);
        __m256 va1 = _mm256_load_ps(&a[i + 8]);
        __m256 va2 = _mm256_load_ps(&a[i + 16]);
        __m256 va3 = _mm256_load_ps(&a[i + 24]);

        __m256 vb0 = _mm256_load_ps(&b[i + 0]);
        __m256 vb1 = _mm256_load_ps(&b[i + 8]);
        __m256 vb2 = _mm256_load_ps(&b[i + 16]);
        __m256 vb3 = _mm256_load_ps(&b[i + 24]);

        _mm256_store_ps(&c[i + 0],  _mm256_add_ps(va0, vb0));
        _mm256_store_ps(&c[i + 8],  _mm256_add_ps(va1, vb1));
        _mm256_store_ps(&c[i + 16], _mm256_add_ps(va2, vb2));
        _mm256_store_ps(&c[i + 24], _mm256_add_ps(va3, vb3));
    }

    // 处理剩余
    for (; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

// 性能对比 (1亿元素)：
// 标量:      1.0x
// AVX:       6.5x
// AVX+展开:   7.8x (额外20%提升)
