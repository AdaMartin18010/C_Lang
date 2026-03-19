/*
 * Auto-generated from: 03_System_Technology_Domains\03_Hardware_Acceleration.md
 * Line: 550
 * Language: c
 * Block ID: 7ff51760
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Vivado HLS: FIR滤波器
#include <ap_int.h>

typedef ap_int<16> data_t;
typedef ap_int<32> acc_t;

#define TAP_NUM 16

// FIR滤波器
void fir_filter(data_t x, data_t h[TAP_NUM], data_t& y) {
    #pragma HLS INTERFACE ap_ctrl_chain port=return
    #pragma HLS INTERFACE s_axilite port=h
    #pragma HLS INTERFACE ap_vld port=x
    #pragma HLS INTERFACE ap_vld port=y

    static data_t shift_reg[TAP_NUM];
    #pragma HLS ARRAY_PARTITION variable=shift_reg complete

    acc_t acc = 0;

    // 移位
    SHIFT:
    for (int i = TAP_NUM - 1; i > 0; i--) {
        #pragma HLS UNROLL
        shift_reg[i] = shift_reg[i - 1];
    }
    shift_reg[0] = x;

    // 乘累加
    MAC:
    for (int i = 0; i < TAP_NUM; i++) {
        #pragma HLS PIPELINE II=1
        acc += shift_reg[i] * h[i];
    }

    y = acc >> 16;  // 量化
}
