/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 404
 * Language: c
 * Block ID: 5a132503
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: constexpr 与类型系统交互
#include <stdint.h>
#include <stdbool.h>

// constexpr 可用于任何整数常量上下文
constexpr size_t ARRAY_SIZE = 100;

// 但不支持类型参数
// ❌ 不可能: constexpr T max(T a, T b) { ... }

// 变通: 使用 _Generic（有限）
#define MAX(a, b) _Generic((a), \
    int: max_int, \
    double: max_double \
)(a, b)

constexpr int max_int(int a, int b) { return a > b ? a : b; }
constexpr double max_double(double a, double b) { return a > b ? a : b; }

// 类型特征（C23 有限支持）
constexpr size_t INT_SIZE = sizeof(int);
constexpr size_t PTR_ALIGN = alignof(void*);

// ❌ 不支持: 根据类型生成代码
// ❌ 不支持: 编译时类型查询
