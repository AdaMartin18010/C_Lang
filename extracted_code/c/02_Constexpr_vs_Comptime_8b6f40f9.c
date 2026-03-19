/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 1268
 * Language: c
 * Block ID: 8b6f40f9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 内存限制主要是栈空间
// constexpr 函数不能使用动态内存分配

// 大数组需要在编译时常量中定义
constexpr int LARGE_TABLE[1000] = { /* ... */ };  // 在可执行文件的 .rodata

// 无法在 constexpr 函数中分配大数组
// ❌ 错误：动态分配
// constexpr int* make_table() {
//     int* table = malloc(1000 * sizeof(int));  // 错误
//     return table;
// }

// 变通：使用全局常量
constexpr int PRECOMPUTED_TABLE[10000] = {
    // 预计算值...
};
