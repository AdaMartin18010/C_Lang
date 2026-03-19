/*
 * Auto-generated from: 00_VERSION_TRACKING\LLVM_22_C23_Update.md
 * Line: 315
 * Language: c
 * Block ID: 047e24cc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C2y 之前：这是未定义行为或编译错误
// C2y：明确定义的行为

// static inline 定义
static inline int helper(int x) {
    return x * x;
}

// extern inline 可以调用 static inline
extern inline int compute(int x) {
    return helper(x) + helper(x + 1);  // C2y 允许
}

// 在另一个文件中定义外部可见版本
extern inline int compute(int x);  // 生成外部链接版本
