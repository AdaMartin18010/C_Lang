/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\02_Arithmetic_Logic_Unit\04_Floating_Point_Unit.md
 * Line: 1239
 * Language: c
 * Block ID: cdcb4a2f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 1. 数据对齐
alignas(32) float array[100];  // 32字节对齐，利于AVX

// 2. 避免数据依赖
// 低效 (数据依赖链):
float a = x * y;
float b = a * z;  // 等待a计算完成
float c = b * w;  // 等待b计算完成

// 高效 (并行):
float a = x1 * y1;
float b = x2 * y2;  // 独立计算
float c = x3 * y3;  // 独立计算

// 3. 使用FMA
// 传统: result = a * b + c;  // 两次舍入
// FMA:  result = fma(a, b, c);  // 一次舍入，更快更准

// 4. 内存访问模式
// 高效: 连续访问
for (int i = 0; i < n; i++) {
    sum += arr[i];  // 连续内存，缓存友好
}

// 低效: 随机访问
for (int i = 0; i < n; i++) {
    sum += arr[idx[i]];  // 随机访问，缓存不友好
}

// 5. 精度与速度权衡
// 单精度更快，但精度较低
// 在精度和速度之间选择合适类型
