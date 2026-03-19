/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 793
 * Language: c
 * Block ID: cb99147f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C2y auto推导规则 (提案)

auto x = expression;

// 推导规则:
// 1. 移除顶层const/volatile (如C++，但C的const规则不同)
// 2. 数组退化为指针
// 3. 函数保持为函数指针

// 与C++的差异:
// C++: auto保留引用，需要auto&显式引用
// C2y: 暂无引用，auto就是值类型
