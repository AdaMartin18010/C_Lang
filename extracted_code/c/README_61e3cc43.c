/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\README.md
 * Line: 287
 * Language: c
 * Block ID: 61e3cc43
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 超标量执行示例 (4-wide 处理器)
void superscalar_demo() {
    int a = 1;      // 周期 1: ALU0
    int b = 2;      // 周期 1: ALU1 (并行)
    int c = 3;      // 周期 1: ALU2 (并行)
    int d = 4;      // 周期 1: ALU3 (并行)

    int e = a + b;  // 周期 2: ALU0 (依赖 a,b, 不能周期1执行)
    int f = c + d;  // 周期 2: ALU1 (并行)
}
