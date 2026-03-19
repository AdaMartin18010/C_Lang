/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 1129
 * Language: c
 * Block ID: f979b7e8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: constexpr 递归限制
#include <stdio.h>

// 递归 constexpr 函数
constexpr int recursive_add(int n) {
    if (n <= 0) return 0;
    return n + recursive_add(n - 1);
}

// 通常限制在 512 层左右（实现定义）
constexpr int SUM_100 = recursive_add(100);  // 通常 OK
// constexpr int SUM_1000 = recursive_add(1000);  // 可能编译错误

// 尾递归优化依赖编译器
constexpr int factorial(int n, int acc) {
    return n <= 1 ? acc : factorial(n - 1, n * acc);
}

// 斐波那契 - 指数级复杂度，深度限制更严重
constexpr int fib(int n) {
    return n <= 1 ? n : fib(n - 1) + fib(n - 2);
}

// fib(20) 可能达到递归限制
// constexpr int FIB_20 = fib(20);

// 解决方案：迭代版本
constexpr int fib_iter(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

constexpr int FIB_50 = fib_iter(50);  // OK：迭代不增加递归深度
