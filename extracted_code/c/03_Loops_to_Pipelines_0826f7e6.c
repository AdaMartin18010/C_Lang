/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\03_Loops_to_Pipelines.md
 * Line: 248
 * Language: c
 * Block ID: 0826f7e6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 问题：循环边界不能被展开因子整除
void imperfect_loop(int a[100], int b[100]) {
    int sum = 0;

    // 方法1：调整循环边界
    #pragma HLS UNROLL factor=8
    for (int i = 0; i < 96; i++) {  // 调整为8的倍数
        sum += a[i] * b[i];
    }
    // 处理剩余4个元素
    for (int i = 96; i < 100; i++) {
        sum += a[i] * b[i];
    }
}

// 方法2：使用HLS自动处理（skip_exit_check）
void auto_unroll(int a[100], int b[100]) {
    int sum = 0;

    #pragma HLS UNROLL factor=8 skip_exit_check
    for (int i = 0; i < 100; i++) {
        sum += a[i] * b[i];
    }
}

// 方法3：显式的展开循环
void manual_unroll(int a[100], int b[100]) {
    int sum = 0;

    // 主展开部分
    for (int i = 0; i < 96; i += 8) {
        #pragma HLS PIPELINE
        #pragma HLS UNROLL
        sum += a[i+0] * b[i+0];
        sum += a[i+1] * b[i+1];
        sum += a[i+2] * b[i+2];
        sum += a[i+3] * b[i+3];
        sum += a[i+4] * b[i+4];
        sum += a[i+5] * b[i+5];
        sum += a[i+6] * b[i+6];
        sum += a[i+7] * b[i+7];
    }

    // 收尾部分
    #pragma HLS PIPELINE
    for (int i = 96; i < 100; i++) {
        sum += a[i] * b[i];
    }
}
