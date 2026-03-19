/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 277
 * Language: c
 * Block ID: b7bd197a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 选择合适的数据类型可以显著提升性能
 */

// 问题：过度使用32位整数
void datatype_wasteful(int in[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        int val = in[i];  // 32位
        if (val > 100) val = 100;  // 范围0-100
        out[i] = val;
    }
}
// 资源：32位比较器、多路选择器

// 优化：使用精确位宽
#include "ap_int.h"
void datatype_optimized(ap_uint<7> in[100], ap_uint<7> out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        ap_uint<7> val = in[i];  // 7位足够表示0-127
        if (val > 100) val = 100;
        out[i] = val;
    }
}
// 资源：7位比较器，更少的LUT

// 定点数替代浮点数
#include "ap_fixed.h"
void datatype_fixed_point(float in[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        // 浮点乘法：高延迟，大量资源
        float result_float = in[i] * 3.14159f;
        out[i] = (int)result_float;
    }
}

void datatype_fixed_optimized(ap_fixed<16,8> in[100], ap_int<16> out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        // 定点乘法：低延迟，可预测资源
        ap_fixed<16,8> pi = 3.14159;
        ap_fixed<16,8> result = in[i] * pi;
        out[i] = result.range(15, 0);
    }
}
// 性能提升：5-10x，资源减少：50-80%

// 数据打包优化带宽
void datatype_packing(ap_uint<8> in0[100], ap_uint<8> in1[100],
                      ap_uint<8> in2[100], ap_uint<8> in3[100],
                      ap_uint<32> out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS UNROLL factor=4
        // 打包4个8位到32位
        ap_uint<32> packed;
        packed.range(7, 0)   = in0[i];
        packed.range(15, 8)  = in1[i];
        packed.range(23, 16) = in2[i];
        packed.range(31, 24) = in3[i];
        out[i] = packed;
    }
}
// 输出带宽：4x
