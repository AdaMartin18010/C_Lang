/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 2987
 * Language: c
 * Block ID: 88f098af
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 运行时性能特征

// constexpr 确保编译时常量，可能产生更好的优化
constexpr int TABLE_SIZE = 1024;
int table[TABLE_SIZE];  // 编译时已知大小，可能更好的优化

// constexpr 函数在编译时求值，运行时调用退化为普通函数
constexpr int compute(int x) {
    return x * x + 2 * x + 1;
}

// 使用场景：
// compile_time: compute(5) → 直接替换为 36
// runtime: compute(variable) → 普通函数调用

// 优化提示：
// - constexpr 数组索引可能产生更好的代码生成
// - 编译时常量允许更多编译器优化
