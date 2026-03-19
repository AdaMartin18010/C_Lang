/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2662
 * Language: c
 * Block ID: 86199bdf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 反模式：简单的返回值也用defer
int add(int a, int b) {
    int result = a + b;
    defer printf("Result: %d\n", result);
    return result;
}

// ✅ 简单函数不需要defer
int add_simple(int a, int b) {
    return a + b;
}
