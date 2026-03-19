/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\03_Loops_to_Pipelines.md
 * Line: 754
 * Language: c
 * Block ID: e0a10787
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 场景1：FIR滤波器（滑动窗口依赖）
void fir_with_dependency(int x[100], int y[100], int coeff[4]) {
    int shift_reg[4];
    #pragma HLS ARRAY_PARTITION variable=shift_reg complete

    for (int n = 0; n < 100; n++) {
        #pragma HLS PIPELINE II=1

        // 移位操作 - 迭代间依赖
        for (int i = 3; i > 0; i--) {
            #pragma HLS UNROLL
            shift_reg[i] = shift_reg[i-1];
        }
        shift_reg[0] = x[n];

        // 乘累加
        int acc = 0;
        for (int i = 0; i < 4; i++) {
            #pragma HLS UNROLL
            acc += shift_reg[i] * coeff[i];
        }
        y[n] = acc;
    }
}

// 场景2：循环携带依赖的矩阵运算
void matrix_carry_dependency(int a[100][100]) {
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            #pragma HLS PIPELINE
            // 对角线依赖
            if (i > 0 && j > 0) {
                a[i][j] = a[i][j] + a[i-1][j-1];
            }
        }
    }
}

// 场景3：间接索引依赖
void indirect_indexing(int data[100], int idx[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        #pragma HLS DEPENDENCE variable=data inter false
        // 假设idx没有重复，手动声明无依赖
        int addr = idx[i];
        data[addr] = data[addr] * 2;
        out[i] = data[addr];
    }
}
