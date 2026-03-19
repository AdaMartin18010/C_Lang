/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\03_Loops_to_Pipelines.md
 * Line: 961
 * Language: c
 * Block ID: 13c913f9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 版本1：基础实现
void fir_basic(int x[1000], int y[1000], int coeff[16]) {
    for (int n = 0; n < 1000; n++) {
        int acc = 0;
        for (int k = 0; k < 16; k++) {
            if (n - k >= 0) {
                acc += x[n - k] * coeff[k];
            }
        }
        y[n] = acc;
    }
}
// 性能：16000周期，资源少，启动间隔高

// 版本2：流水线优化
void fir_pipeline(int x[1000], int y[1000], int coeff[16]) {
    int shift_reg[16];

    for (int n = 0; n < 1000; n++) {
        #pragma HLS PIPELINE II=1

        // 移位
        for (int k = 15; k > 0; k--) {
            #pragma HLS UNROLL
            shift_reg[k] = shift_reg[k-1];
        }
        shift_reg[0] = x[n];

        // MAC
        int acc = 0;
        for (int k = 0; k < 16; k++) {
            #pragma HLS UNROLL
            acc += shift_reg[k] * coeff[k];
        }
        y[n] = acc;
    }
}
// 性能：~1000周期，II=1，16个乘法器

// 版本3：资源优化（乘法器共享）
void fir_resource_opt(int x[1000], int y[1000], int coeff[16]) {
    #pragma HLS ARRAY_PARTITION variable=coeff complete

    int shift_reg[16];
    #pragma HLS ARRAY_PARTITION variable=shift_reg complete

    for (int n = 0; n < 1000; n++) {
        #pragma HLS PIPELINE II=4  // 放宽II以共享资源

        // 移位
        for (int k = 15; k > 0; k--) {
            #pragma HLS UNROLL
            shift_reg[k] = shift_reg[k-1];
        }
        shift_reg[0] = x[n];

        // MAC - 每周期4次乘法，复用4个乘法器
        int acc = 0;
        for (int k = 0; k < 16; k++) {
            #pragma HLS UNROLL factor=4
            acc += shift_reg[k] * coeff[k];
        }
        y[n] = acc;
    }
}
// 性能：~4000周期，4个乘法器，面积减少75%

// 版本4：对称系数优化
void fir_symmetric(int x[1000], int y[1000], int coeff[8]) {
    // 利用FIR系数对称性：h[k] = h[N-1-k]
    int shift_reg[16];
    #pragma HLS ARRAY_PARTITION variable=shift_reg complete

    for (int n = 0; n < 1000; n++) {
        #pragma HLS PIPELINE II=1

        // 移位
        for (int k = 15; k > 0; k--) {
            #pragma HLS UNROLL
            shift_reg[k] = shift_reg[k-1];
        }
        shift_reg[0] = x[n];

        // 对称MAC - 预加减少乘法器
        int acc = 0;
        for (int k = 0; k < 8; k++) {
            #pragma HLS UNROLL
            acc += (shift_reg[k] + shift_reg[15-k]) * coeff[k];
        }
        y[n] = acc;
    }
}
// 性能：~1000周期，8个乘法器，利用对称性减少一半
