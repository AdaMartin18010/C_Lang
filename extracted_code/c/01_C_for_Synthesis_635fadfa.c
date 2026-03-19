/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 258
 * Language: c
 * Block ID: 635fadfa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 不可综合：标准库函数
#include <math.h>
void bad_math(float x) {
    float y = sqrt(x);      // 标准库sqrt
    float z = sin(x);       // 标准库sin
    printf("%f\n", y);      // I/O操作
}

// ✅ 可综合：使用HLS数学库或自定义实现
#include "hls_math.h"  // Vivado HLS数学库
void good_math(float x) {
    float y = hls::sqrt(x);  // HLS优化的sqrt
    float z = hls::sin(x);   // HLS优化的sin
    // 避免I/O操作
}

// ✅ 定点数实现（推荐用于资源优化）
#include "ap_fixed.h"
void fixed_math(ap_fixed<16,8> x) {
    ap_fixed<16,8> y = hls::sqrt(x);  // 定点sqrt更高效
}
