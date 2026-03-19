/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\04_SIMD_Optimization_Case.md
 * Line: 108
 * Language: c
 * Block ID: 6b093e30
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// SSE/AVX加载操作

// 对齐加载 (地址必须是16/32/64字节对齐)
__m128  _mm_load_ps(const float* p);        // SSE, 4 floats
__m256  _mm256_load_ps(const float* p);     // AVX, 8 floats
__m512  _mm512_load_ps(const float* p);     // AVX-512, 16 floats

// 非对齐加载
__m128  _mm_loadu_ps(const float* p);
__m256  _mm256_loadu_ps(const float* p);

// 加载单个值到所有元素
__m256 _mm256_set1_ps(float a);             // 广播

// 从多个值加载
__m256 _mm256_set_ps(float e7, float e6, float e5, float e4,
                     float e3, float e2, float e1, float e0);

// 存储操作
void _mm256_store_ps(float* p, __m256 a);   // 对齐存储
void _mm256_storeu_ps(float* p, __m256 a);  // 非对齐存储

// 示例：向量加法
void vec_add_avx(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;

    // AVX每次处理8个float
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_load_ps(&a[i]);
        __m256 vb = _mm256_load_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_store_ps(&c[i], vc);
    }

    // 处理剩余元素
    for (; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}
