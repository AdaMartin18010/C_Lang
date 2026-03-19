/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\01_Language_Extensions.md
 * Line: 172
 * Language: c
 * Block ID: 1db0b46c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 向量类型
typedef int v4si __attribute__((vector_size(16)));  // 4 x int
typedef float v4sf __attribute__((vector_size(16))); // 4 x float
typedef double v2df __attribute__((vector_size(16))); // 2 x double

// 向量运算
v4sf a = {1.0, 2.0, 3.0, 4.0};
v4sf b = {5.0, 6.0, 7.0, 8.0};
v4sf sum = a + b;  // {6.0, 8.0, 10.0, 12.0}
v4sf prod = a * b; // {5.0, 12.0, 21.0, 32.0}

// 索引访问
float first = sum[0];

// 向量内建函数
int mask = __builtin_ia32_movmskps(sum);  // 提取符号位

// 类型转换
v4si int_vec = __builtin_ia32_cvtps2dq(prod);  // float转int
