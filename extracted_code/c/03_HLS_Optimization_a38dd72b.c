/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 715
 * Language: c
 * Block ID: a38dd72b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 识别并优化关键路径
 */

// 问题：长组合逻辑链
void timing_long_path(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        int t1 = a[i] * b[i];
        int t2 = t1 + c[i];
        int t3 = t2 * 3;
        int t4 = t3 + a[i];
        int t5 = t4 * b[i];  // 关键路径过长
        c[i] = t5;
    }
}

// 优化1：插入流水线级数
void timing_pipeline(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=2  // 增加II，允许更多流水线级
        int t1 = a[i] * b[i];
        int t2 = t1 + c[i];
        int t3 = t2 * 3;
        int t4 = t3 + a[i];
        int t5 = t4 * b[i];
        c[i] = t5;
    }
}

// 优化2：代码重构减少关键路径
void timing_restructure(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        // 并行计算减少依赖链
        int p1 = a[i] * b[i];
        int p2 = a[i] * b[i] + c[i];
        int t3 = p2 * 3;
        int t4 = t3 + a[i];
        c[i] = t4 * b[i];
    }
}

// 优化3：使用更高速的操作符
void timing_fast_op(int a[100], int b[100], int c[100]) {
    #include "ap_fixed.h"

    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        // 定点乘法比浮点快
        ap_fixed<16,8> af = a[i];
        ap_fixed<16,8> bf = b[i];
        ap_fixed<16,8> cf = c[i];
        cf = af * bf + cf;
        c[i] = cf.to_int();
    }
}
