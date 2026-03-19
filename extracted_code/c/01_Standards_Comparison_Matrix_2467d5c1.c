/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 2485
 * Language: c
 * Block ID: 2467d5c1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 可能的C26模块语法 */
module;  /* 模块声明开始 */

export module math_utils;  /* 导出模块 */

export int add(int a, int b) {
    return a + b;
}

export template<typename T>
T max(T a, T b) {
    return a > b ? a : b;
}

/* 导入模块 */
import math_utils;

int main(void) {
    int result = add(5, 3);
    return 0;
}
