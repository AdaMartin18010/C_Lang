/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 2456
 * Language: c
 * Block ID: fbc3894b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C89: 隐式函数声明允许，但危险 */
int main(void) {
    /* 没有声明strlen，编译器假设返回int */
    int len = strlen("hello");  /* 在64位系统上可能截断指针！ */
    return 0;
}

/* C99+: 必须显式声明 */
#include <string.h>

int main(void) {
    size_t len = strlen("hello");  /* 正确 */
    return 0;
}

/* C23: 隐式函数声明是错误 */
