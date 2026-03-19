/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\README.md
 * Line: 314
 * Language: c
 * Block ID: 17257d63
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 编译器提示
// __builtin_expect 告诉编译器分支概率

if (__builtin_expect(ptr != NULL, 1)) {
    // 预期经常执行：代码布局优化
    process(ptr);
} else {
    // 预期很少执行
    handle_error();
}

// GCC扩展语法
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
