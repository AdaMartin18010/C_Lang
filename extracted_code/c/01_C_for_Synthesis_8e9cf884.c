/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 415
 * Language: c
 * Block ID: 8e9cf884
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#include "ap_int.h"   // 任意精度整数
#include "ap_fixed.h" // 任意精度定点数

// ap_int<W>: 有符号W位整数
// ap_uint<W>: 无符号W位整数

void arbitrary_precision() {
    // 相比int(32位)，使用更精确的类型可以节省资源
    ap_int<8>   small_val;   // 8位有符号：-128 ~ 127
    ap_uint<4>  tiny_val;    // 4位无符号：0 ~ 15
    ap_int<17>  odd_size;    // 17位有符号：任意位宽
    ap_uint<256> big_val;    // 256位大整数

    // 运算自动处理溢出
    ap_uint<8> a = 200;
    ap_uint<8> b = 100;
    ap_uint<8> c = a + b;    // 结果：44（溢出回绕）

    // 扩展精度运算
    ap_uint<9> d = a + b;    // 使用9位避免溢出
}

// 实际应用：精确位宽优化
void optimized_counter(ap_uint<10> *count) {
    #pragma HLS INTERFACE mode=ap_vld port=count
    // 计数器只需要10位（0~1023），而不是32位
    static ap_uint<10> cnt = 0;
    cnt++;
    if (cnt >= 1000) cnt = 0;  // 模1000计数
    *count = cnt;
}
