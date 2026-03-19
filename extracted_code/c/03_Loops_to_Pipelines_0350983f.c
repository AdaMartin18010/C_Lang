/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\03_Loops_to_Pipelines.md
 * Line: 188
 * Language: c
 * Block ID: 0350983f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例：8元素向量点积
int dot_product(int a[8], int b[8]) {
    int sum = 0;

    // 策略1：不展开（顺序执行）
    // 周期数：约8 * T_process
    // 资源：1个乘法器 + 1个加法器
    for (int i = 0; i < 8; i++) {
        sum += a[i] * b[i];
    }

    // 策略2：展开因子2
    // 周期数：约4 * T_process
    // 资源：2个乘法器 + 2个加法器
    #pragma HLS UNROLL factor=2
    for (int i = 0; i < 8; i++) {
        sum += a[i] * b[i];
    }

    // 策略3：完全展开
    // 周期数：1个周期（组合逻辑）或T_process（流水线）
    // 资源：8个乘法器 + 8个加法器 + 加法树
    #pragma HLS UNROLL
    for (int i = 0; i < 8; i++) {
        #pragma HLS PIPELINE
        sum += a[i] * b[i];
    }

    return sum;
}
