/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 2137
 * Language: c
 * Block ID: 38f6c325
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 编译时数学计算
#include <math.h>
#include <stdint.h>

// 编译时阶乘
constexpr uint64_t factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// 编译时幂运算
constexpr double power(double base, int exp) {
    if (exp == 0) return 1.0;
    if (exp < 0) return 1.0 / power(base, -exp);
    return base * power(base, exp - 1);
}

// 编译时平方根（牛顿迭代）
constexpr double sqrt_newton(double x, double guess, int iterations) {
    if (iterations == 0) return guess;
    double next = (guess + x / guess) / 2.0;
    return sqrt_newton(x, next, iterations - 1);
}

constexpr double sqrt_ct(double x) {
    return sqrt_newton(x, x / 2.0, 20);
}

// 使用
constexpr double PI = 3.14159265358979323846;
constexpr double SQRT_2 = sqrt_ct(2.0);
constexpr uint64_t FACT_20 = factorial(20);

// 查找表生成
constexpr int SIN_TABLE_SIZE = 360;

// ❌ C23 无法生成完整的查找表数组
// 只能在运行时填充或使用外部代码生成工具
