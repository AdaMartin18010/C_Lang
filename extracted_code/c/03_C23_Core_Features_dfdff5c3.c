/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\03_C23_Core_Features.md
 * Line: 241
 * Language: c
 * Block ID: dfdff5c3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 实现定义的递归深度限制
constexpr int deep_recursion(int n) {
    if (n <= 0) return 0;
    return 1 + deep_recursion(n - 1);
}

// 通常限制在 512 层或更高
constexpr int DEEP = deep_recursion(1000);  // 可能编译错误
