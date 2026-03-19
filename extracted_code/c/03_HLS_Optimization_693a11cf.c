/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 1158
 * Language: c
 * Block ID: 693a11cf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * C仿真通过但RTL仿真失败的原因
 */

// 原因1：未定义行为（UB）
void ub_example(int a[100]) {
    int x;
    // x未初始化
    a[0] = x + 1;  // C仿真可能得到任意值
    // RTL可能产生固定值或X
}

// 原因2：整数溢出
void overflow_example(int a[100]) {
    int sum = 0;
    for (int i = 0; i < 100; i++) {
        #pragma HLS UNROLL
        sum += a[i];  // 可能溢出
    }
    // C和RTL溢出行为可能不同
}
// 修正：使用更大的数据类型
void overflow_fixed(ap_int<40> a[100]) {
    ap_int<48> sum = 0;  // 足够容纳100个32位数
    for (int i = 0; i < 100; i++) {
        #pragma HLS UNROLL
        sum += a[i];
    }
}

// 原因3：浮点精度差异
void float_precision(float a[100], float b[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        // C使用IEEE 754，RTL可能使用近似
        b[i] = sqrt(a[i]);
    }
}
// 修正：使用容差比较
#ifndef __SYNTHESIS__
#define EPSILON 1e-5
int compare_float(float a, float b) {
    return fabs(a - b) < EPSILON;
}
#endif

// 原因4：数据依赖pragma错误
void false_dependency(int a[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS DEPENDENCE variable=a inter false
        // 如果实际有依赖，结果错误
        a[idx[i]] = a[idx[i]] + 1;
    }
}
