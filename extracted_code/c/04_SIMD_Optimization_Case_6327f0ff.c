/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\04_SIMD_Optimization_Case.md
 * Line: 810
 * Language: c
 * Block ID: 6327f0ff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 软件预取减少缓存缺失

#include <immintrin.h>

void vec_add_prefetch(const float* a, const float* b, float* c, size_t n) {
    const int PREFETCH_DISTANCE = 16;  // 预取距离 (64个float = 256字节)

    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        // 预取未来的数据
        if (i + PREFETCH_DISTANCE * 8 < n) {
            _mm_prefetch((const char*)&a[i + PREFETCH_DISTANCE * 8], _MM_HINT_T0);
            _mm_prefetch((const char*)&b[i + PREFETCH_DISTANCE * 8], _MM_HINT_T0);
        }

        __m256 va = _mm256_load_ps(&a[i]);
        __m256 vb = _mm256_load_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_store_ps(&c[i], vc);
    }

    for (; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}
