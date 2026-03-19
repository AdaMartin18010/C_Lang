/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
 * Line: 1978
 * Language: c
 * Block ID: 85e88dbc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: AVX-512 调用约定

#include <immintrin.h>

// AVX-512 函数使用 ZMM 寄存器
__m512 add_avx512(__m512 a, __m512 b, __m512 c, __m512 d,
                  __m512 e, __m512 f, __m512 g, __m512 h) {
    // a-h: %zmm0-%zmm7
    __m512 sum1 = _mm512_add_ps(a, b);
    __m512 sum2 = _mm512_add_ps(c, d);
    // ...
    return sum1;
}

// 更多参数使用栈
__m512 many_avx512(__m512 a, __m512 b, __m512 c, __m512 d,
                   __m512 e, __m512 f, __m512 g, __m512 h,
                   __m512 i) {
    // a-h: %zmm0-%zmm7
    // i: 通过栈传递
    return i;
}
