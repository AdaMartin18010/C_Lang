/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 922
 * Language: c
 * Block ID: cd032579
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * FIR滤波器 - 完整的可综合设计示例
 *
 * 功能：实现16抽头FIR滤波器
 * 目标：展示C到硬件映射的全过程
 */

#include "ap_int.h"
#include "ap_fixed.h"

// 设计参数
#define TAP 16
#define DATA_WIDTH 16
#define COEFF_WIDTH 16
#define ACCUM_WIDTH 32

// 定点数类型定义
typedef ap_fixed<DATA_WIDTH, DATA_WIDTH-4> data_t;
typedef ap_fixed<COEFF_WIDTH, 2> coeff_t;
typedef ap_fixed<ACCUM_WIDTH, ACCUM_WIDTH-8> accum_t;

// FIR滤波器核心
void fir_filter(
    data_t *y,           // 输出
    data_t x,            // 输入
    const coeff_t coeff[TAP]  // 系数（常量或ROM）
) {
    #pragma HLS INTERFACE mode=ap_vld port=y
    #pragma HLS INTERFACE mode=ap_vld port=x
    #pragma HLS INTERFACE mode=ap_memory port=coeff

    // 移位寄存器 - 映射为链式寄存器或RAM
    static data_t shift_reg[TAP];
    #pragma HLS ARRAY_PARTITION variable=shift_reg complete

    // 移位操作
    SHIFT_LOOP:
    for (int i = TAP-1; i > 0; i--) {
        #pragma HLS UNROLL
        shift_reg[i] = shift_reg[i-1];
    }
    shift_reg[0] = x;

    // 乘累加 - 完全展开实现并行
    accum_t acc = 0;
    MAC_LOOP:
    for (int i = 0; i < TAP; i++) {
        #pragma HLS UNROLL
        acc += shift_reg[i] * coeff[i];
    }

    *y = acc;
}
