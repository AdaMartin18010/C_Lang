/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 467
 * Language: c
 * Block ID: fd23eef7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C89函数原型 */
int add(int a, int b);  /* 声明时指定参数类型 */

int main(void) {
    int result = add(5, 3);  /* 编译器会检查参数类型 */
    return 0;
}

int add(int a, int b) {  /* 定义时使用相同原型 */
    return a + b;
}
