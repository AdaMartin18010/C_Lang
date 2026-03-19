/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 835
 * Language: c
 * Block ID: ee85943f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * FIR滤波器完整优化案例
 * 目标：64抽头FIR，16位定点，200MHz时钟
 */

// 版本1：基础实现
void fir_v1_basic(int x, int *y, int coeff[64]) {
    static int shift_reg[64];

    // 移位
    for (int i = 63; i > 0; i--) {
        shift_reg[i] = shift_reg[i-1];
    }
    shift_reg[0] = x;

    // MAC
    int acc = 0;
    for (int i = 0; i < 64; i++) {
        acc += shift_reg[i] * coeff[i];
    }
    *y = acc;
}
// 性能：64*2 = 128周期/样本

// 版本2：流水线化
void fir_v2_pipeline(int x, int *y, int coeff[64]) {
    #pragma HLS INTERFACE mode=ap_vld port=x
    #pragma HLS INTERFACE mode=ap_vld port=y
    #pragma HLS ARRAY_PARTITION variable=coeff complete

    static int shift_reg[64];
    #pragma HLS ARRAY_PARTITION variable=shift_reg complete

    for (int i = 63; i > 0; i--) {
        #pragma HLS UNROLL
        shift_reg[i] = shift_reg[i-1];
    }
    shift_reg[0] = x;

    int acc = 0;
    for (int i = 0; i < 64; i++) {
        #pragma HLS UNROLL factor=8
        acc += shift_reg[i] * coeff[i];
    }
    *y = acc;
}
// 性能：2周期/样本（II=1有依赖问题）

// 版本3：完全优化
void fir_v3_optimized(int x, int *y, int coeff[64]) {
    #pragma HLS INTERFACE mode=ap_vld port=x
    #pragma HLS INTERFACE mode=ap_vld port=y
    #pragma HLS ARRAY_PARTITION variable=coeff complete
    #pragma HLS PIPELINE II=1

    static int shift_reg[64];
    #pragma HLS ARRAY_PARTITION variable=shift_reg complete

    // 移位
    for (int i = 63; i > 0; i--) {
        #pragma HLS UNROLL
        shift_reg[i] = shift_reg[i-1];
    }
    shift_reg[0] = x;

    // 并行MAC（8组，每组8个乘法）
    int partial[8];
    #pragma HLS ARRAY_PARTITION variable=partial complete

    for (int g = 0; g < 8; g++) {
        #pragma HLS UNROLL
        partial[g] = 0;
        for (int i = 0; i < 8; i++) {
            #pragma HLS UNROLL
            partial[g] += shift_reg[g*8+i] * coeff[g*8+i];
        }
    }

    // 树形归约
    int sum = 0;
    for (int i = 0; i < 8; i++) {
        #pragma HLS UNROLL
        sum += partial[i];
    }
    *y = sum;
}
// 性能：II=1，1周期/样本 ✓

// 优化对比表
/*
┌─────────┬────────┬────────┬────────┬────────┬─────────────────┐
│ 版本    │ 周期   │ LUT    │ FF     │ DSP    │ 说明            │
├─────────┼────────┼────────┼────────┼────────┼─────────────────┤
│ v1基础  │ 128    │ 200    │ 100    │ 1      │ 顺序执行        │
│ v2流水线│ 2      │ 1500   │ 800    │ 8      │ 部分展开        │
│ v3优化  │ 1      │ 2200   │ 1200   │ 8      │ 目标达成        │
└─────────┴────────┴────────┴────────┴────────┴─────────────────┘
*/
