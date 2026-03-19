/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
 * Line: 688
 * Language: c
 * Block ID: 38240ab0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: 向量类型 (使用 GCC/Clang 扩展或 immintrin.h)

#include <immintrin.h>

// 128-bit SSE 向量
void sse_vec(__m128 v) {
    // v: %xmm0
}

// 256-bit AVX 向量
void avx_vec(__m256 v) {
    // v: %ymm0
}

// 512-bit AVX-512 向量
void avx512_vec(__m512 v) {
    // v: %zmm0
}

// 多个向量
void multi_vec(__m128 a, __m128 b, __m128 c, __m128 d,
               __m128 e, __m128 f, __m128 g, __m128 h,
               __m128 i) {
    // a-h: %xmm0-%xmm7
    // i: 16(%rsp) - 栈传递
}
