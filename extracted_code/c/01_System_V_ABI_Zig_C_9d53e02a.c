/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
 * Line: 638
 * Language: c
 * Block ID: 9d53e02a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: x87 类型传递

// long double (80-bit extended precision)
// x86-64 Linux: 80位，通过 x87 FPU 栈传递
void x87_type(long double ld) {
    // ld: %st(0)
}

// 复数类型
#include <complex.h>

// double complex - 两个 double，作为两个 SSE 类传递
void complex_double(double complex cd) {
    // cd 实部: %xmm0
    // cd 虚部: %xmm1
}

// long double complex - 通过 x87 栈传递
void complex_x87(long double complex cld) {
    // cld: COMPLEX_X87 类
    // 实部: %st(0)
    // 虚部: %st(1)
}
