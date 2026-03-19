/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\02_Arithmetic_Logic_Unit\04_Floating_Point_Unit.md
 * Line: 1131
 * Language: c
 * Block ID: 91b637a2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 编译器优化示例

// 1. 常量折叠 (编译时计算)
float a = 3.14159f * 2.0f;   // 直接计算为6.28318

// 2. 强度削弱
float b = x * 2.0f;          // 可能优化为 x + x (更快)
float c = x / 2.0f;          // 优化为 x * 0.5f

// 3. 循环优化
// 原始代码:
for (int i = 0; i < n; i++) {
    result[i] = input[i] * 2.5f;
}

// 优化后 (向量化，一次处理4个):
// 使用SIMD指令同时计算4个乘法

// 4. FMA优化 (融合乘加)
// 原始: a = b * c + d;  (两次舍入)
// FMA: a = fma(b, c, d);  (一次舍入，更高精度)

// 5. 自动向量化提示
void process(float* __restrict a,
             const float* __restrict b,
             int n) {
    // __restrict 告诉编译器指针不重叠
    for (int i = 0; i < n; i++) {
        a[i] = a[i] + b[i];
    }
}
