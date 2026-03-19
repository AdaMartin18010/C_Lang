/*
 * Auto-generated from: 12_Practice_Exercises\10_Debugging_Puzzles.md
 * Line: 158
 * Language: c
 * Block ID: 2ba3fa59
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 方案1: 使用静态变量
int* get_static(void) {
    static int x = 42;
    return &x;
}

// 方案2: 动态分配
int* get_heap(void) {
    int *x = malloc(sizeof(int));
    *x = 42;
    return x;
}

// 方案3: 通过参数传出
void get_value(int *out) {
    *out = 42;
}
