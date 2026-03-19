/*
 * Auto-generated from: 12_Practice_Exercises\10_Debugging_Puzzles.md
 * Line: 608
 * Language: c
 * Block ID: 7e36443f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用内联函数（C99）或泛型选择（C11）
static inline int max_int(int a, int b) {
    return a > b ? a : b;
}

#define MAX(a, b) _Generic((a), \
    int: max_int, \
    long: max_long, \
    float: max_float, \
    double: max_double \
)(a, b)
