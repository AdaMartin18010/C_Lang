/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\02_Arithmetic_Logic_Unit\01_Binary_Arithmetic.md
 * Line: 1913
 * Language: c
 * Block ID: 0f0973be
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 现代处理器中的加法器选择由硬件决定
// 但编译器可以进行一些优化

// 示例：常量折叠
int x = 10 + 20;  // 编译时直接计算为30，无需运行时加法

// 示例：强度削弱
int y = x * 2;    // 可能被优化为 x + x 或 x << 1
int z = x * 15;   // 可能被优化为 (x << 4) - x

// 示例：向量化
int a[4], b[4], c[4];
for(int i=0; i<4; i++)
    c[i] = a[i] + b[i];  // 可能使用SIMD并行4个加法
