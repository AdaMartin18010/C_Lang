/*
 * Auto-generated from: 07_Modern_Toolchain\05_Code_Quality_Toolchain\02_Static_Analysis_Deep_Dive.md
 * Line: 343
 * Language: c
 * Block ID: 43665b21
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 源代码
int divide(int x, int y) {
    if (y == 0) {           // 路径约束: y == 0
        return -1;          // 错误路径
    }
    return x / y;           // 路径约束: y != 0
}

// 符号执行分析
// Input: x = α, y = β (符号)
//
// Path 1: β == 0
//   → return -1
//   → 报告: 需要处理除零
//
// Path 2: β != 0
//   → return α / β
//   → 约束: β != 0
