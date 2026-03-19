/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\02_C17_C23_Features.md
 * Line: 324
 * Language: c
 * Block ID: c881a8b3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ constexpr函数不能有副作用
constexpr int bad(int x) {
    static int counter = 0;  // 错误！静态存储
    return x + counter++;
}

// ✅ 纯计算
constexpr int good(int x) {
    return x * x + 2 * x + 1;
}
