/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\02_Data_Type_System.md
 * Line: 823
 * Language: c
 * Block ID: 87dbafc7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ UNSAFE: 有符号溢出是UB
int mul(int a, int b) {
    return a * b;  // 溢出 = UB
}

// ❌ 看似安全的检查实际上可能被优化掉！
int safe_add(int a, int b) {
    // 编译器可能优化：如果溢出是UB，那么溢出不会发生，检查冗余
    if (a > 0 && b > INT_MAX - a) return INT_MAX;
    return a + b;
}

// ✅ SAFE: 使用无符号类型检查
#include <limits.h>
#include <stdbool.h>

bool safe_add_int(int a, int b, int *result) {
    // 转换为无符号进行溢出检测
    if (b > 0) {
        if (a > INT_MAX - b) return false;
    } else if (b < 0) {
        if (a < INT_MIN - b) return false;
    }
    *result = a + b;
    return true;
}

// ✅ SAFE: GCC/Clang内置函数（编译器不可优化）
int safe_add_builtin(int a, int b, int *result) {
    if (__builtin_add_overflow(a, b, result)) {
        return -1;  // 溢出
    }
    return 0;  // 成功
}
