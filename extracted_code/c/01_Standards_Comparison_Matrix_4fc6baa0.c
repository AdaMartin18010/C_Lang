/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 858
 * Language: c
 * Block ID: 4fc6baa0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

constexpr int buffer_size = 1024;  /* 编译时常量 */
constexpr int factorial(int n) {   /* 编译期计算函数 */
    return n <= 1 ? 1 : n * factorial(n - 1);
}

int main(void) {
    int buffer[buffer_size];  /* VLA替代方案 */

    constexpr int f5 = factorial(5);  /* 120, 编译期计算 */

    return 0;
}
