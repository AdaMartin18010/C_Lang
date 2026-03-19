/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\04_Type_Qualifiers_Matrix.md
 * Line: 199
 * Language: c
 * Block ID: 2dc92141
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 矩阵乘法：使用 restrict 优化
void matmul(const double* restrict A,
            const double* restrict B,
            double* restrict C,
            int n);

// 纯函数：不修改输入，输出不重叠
void transform(const float* restrict input,
               float* restrict output,
               size_t n);
