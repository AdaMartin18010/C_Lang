/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 1563
 * Language: c
 * Block ID: b7ffd6c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 有限的代码生成能力
#include <stdio.h>

// ========== 宏生成重复代码 ==========

// 生成重复 switch case
#define CASE_RETURN(x) case x: return #x

const char* error_string(int code) {
    switch (code) {
        CASE_RETURN(0);
        CASE_RETURN(1);
        CASE_RETURN(2);
        CASE_RETURN(3);
        default: return "unknown";
    }
}

// ========== constexpr 生成查找表 ==========

// 预计算查找表
constexpr int SQUARE_TABLE[100] = {
    #define SQUARE_ENTRY(n) [n] = (n) * (n)
    SQUARE_ENTRY(0), SQUARE_ENTRY(1), /* ... */ SQUARE_ENTRY(99)
    #undef SQUARE_ENTRY
};

// 更优雅的方案：初始化器
constexpr int init_square_table(void) {
    int table[100];
    for (int i = 0; i < 100; i++) {
        table[i] = i * i;
    }
    // ❌ C23 无法从 constexpr 函数返回数组
    return table[0];  // 只能返回单个值
}

// 变通：使用全局 constexpr 数组
constexpr int MAKE_SQUARE(int n) { return n * n; }
constexpr int SQUARES[] = {
    MAKE_SQUARE(0), MAKE_SQUARE(1), MAKE_SQUARE(2),
    // ... 手动或代码生成
};

// ========== X-Macro 模式 ==========

// 定义操作列表
#define OPERATION_LIST \
    X(ADD, +) \
    X(SUB, -) \
    X(MUL, *) \
    X(DIV, /)

// 生成枚举
enum OpCode {
    #define X(name, op) OP_##name,
    OPERATION_LIST
    #undef X
    OP_COUNT
};

// 生成字符串表
const char* OP_NAMES[] = {
    #define X(name, op) #name,
    OPERATION_LIST
    #undef X
};

// ❌ 无法根据类型自动生成操作
