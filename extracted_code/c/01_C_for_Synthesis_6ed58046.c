/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 132
 * Language: c
 * Block ID: 6ed58046
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 不可综合：动态内存分配
void bad_example(int n) {
    int *array = malloc(n * sizeof(int));  // 错误！
    // ...
    free(array);  // 错误！
}

// ✅ 可综合：静态数组或参数化大小
#define MAX_SIZE 1024
void good_example(int n) {
    int array[MAX_SIZE];  // 静态分配
    // 或者使用参数化模板（C++）
}

// ✅ 更好的做法：通过接口传递数组
void better_example(int array[1024], int n) {
    // array大小在函数接口中声明
}
