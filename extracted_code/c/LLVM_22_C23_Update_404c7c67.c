/*
 * Auto-generated from: 00_VERSION_TRACKING\LLVM_22_C23_Update.md
 * Line: 440
 * Language: c
 * Block ID: 404c7c67
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// AVX10.2 提供统一的 256/512-bit 向量支持
#include <immintrin.h>

// 使用 AVX10.2 的向量化代码
void avx10_2_vector_add(float *a, float *b, float *c, size_t n) {
    size_t i = 0;

    // AVX10.2 支持 512-bit 操作
    for (; i + 16 <= n; i += 16) {
        __m512 va = _mm512_loadu_ps(&a[i]);
        __m512 vb = _mm512_loadu_ps(&b[i]);
        __m512 vc = _mm512_add_ps(va, vb);
        _mm512_storeu_ps(&c[i], vc);
    }

    // 处理剩余元素
    for (; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}
