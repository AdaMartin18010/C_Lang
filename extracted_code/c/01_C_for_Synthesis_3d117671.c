/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 453
 * Language: c
 * Block ID: 3d117671
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#include "ap_fixed.h"

// ap_fixed<W,I>: W位总宽度，I位整数部分（含符号位）
// ap_ufixed<W,I>: 无符号定点数
// W-I = 小数部分位宽

void fixed_point_example() {
    // ap_fixed<16,8>: 16位总宽，8位整数，8位小数
    // 范围: [-128, 127.99609375]
    // 精度: 1/256 ≈ 0.0039
    ap_fixed<16,8> a = 12.5;
    ap_fixed<16,8> b = 3.25;

    ap_fixed<16,8> sum = a + b;      // 加法
    ap_fixed<32,16> prod = a * b;    // 乘法会扩展位宽

    // 量化模式
    ap_fixed<8,4,AP_RND> c = prod;   // 四舍五入量化
    ap_fixed<8,4,AP_TRN> d = prod;   // 截断量化（默认）
}

// 定点数与浮点数对比
void compare_precision() {
    float f_val = 1.0 / 3.0;           // 浮点：高精度，高资源
    ap_fixed<16,8> fixed_val = 1.0/3;  // 定点：约0.0039精度，低资源

    // 多次运算后的误差累积
    float f_sum = 0;
    ap_fixed<16,8> fixed_sum = 0;

    for (int i = 0; i < 1000; i++) {
        #pragma HLS PIPELINE
        f_sum += f_val;
        fixed_sum += fixed_val;
    }
    // f_sum ≈ 333.333...
    // fixed_sum ≈ 333.0（有量化误差）
}
