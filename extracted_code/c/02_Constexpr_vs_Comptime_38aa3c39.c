/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 214
 * Language: c
 * Block ID: 38aa3c39
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: constexpr 函数
#include <stdbool.h>

// 基本 constexpr 函数
constexpr int square(int x) {
    return x * x;
}

// 递归 constexpr 函数
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// 多语句 constexpr 函数（C23 允许）
constexpr int fibonacci(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

// 使用 constexpr 结果
constexpr int FIB_10 = fibonacci(10);  // 55
int fib_array[FIB_10];  // 数组大小
