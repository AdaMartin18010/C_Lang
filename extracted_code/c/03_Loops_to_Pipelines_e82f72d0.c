/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\03_Loops_to_Pipelines.md
 * Line: 155
 * Language: c
 * Block ID: e82f72d0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 原始循环
for (int i = 0; i < 8; i++) {
    sum += a[i] * b[i];
}

// 展开因子为2
#pragma HLS UNROLL factor=2
for (int i = 0; i < 8; i++) {
    sum += a[i] * b[i];
}

// 等价于：
for (int i = 0; i < 8; i += 2) {
    sum += a[i] * b[i];       // 第i次迭代
    sum += a[i+1] * b[i+1];   // 第i+1次迭代（并行执行）
}

// 完全展开
#pragma HLS UNROLL
for (int i = 0; i < 8; i++) {
    sum += a[i] * b[i];
}

// 等价于：
sum += a[0] * b[0];
sum += a[1] * b[1];
sum += a[2] * b[2];
// ...（所有8次迭代同时并行）
