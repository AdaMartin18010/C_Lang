/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\04_SIMD_Optimization_Case.md
 * Line: 720
 * Language: c
 * Block ID: 62f6adee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 内存对齐对SIMD性能至关重要

// 推荐的分配方式
#include <stdlib.h>

// POSIX对齐分配
float* aligned_malloc(size_t size, size_t alignment) {
    void* ptr;
    if (posix_memalign(&ptr, alignment, size) != 0) {
        return NULL;
    }
    return (float*)ptr;
}

// C11标准
#include <stdalign.h>
alignas(64) float buffer[1024];  // 64字节对齐

// 检查对齐
static inline int is_aligned(const void* ptr, size_t alignment) {
    return ((uintptr_t)ptr & (alignment - 1)) == 0;
}

// 处理未对齐数据
void vec_add_unaligned(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;

    // 处理前缀未对齐部分
    while (i < n && !is_aligned(&a[i], 32)) {
        c[i] = a[i] + b[i];
        i++;
    }

    // SIMD处理对齐部分
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_load_ps(&a[i]);   // 对齐加载
        __m256 vb = _mm256_loadu_ps(&b[i]);  // b可能不对齐
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_store_ps(&c[i], vc);
    }

    // 处理尾部
    for (; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}
