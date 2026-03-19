/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\03_JIT_Compilation.md
 * Line: 51
 * Language: c
 * Block ID: e9dd2e39
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// bytecode.h
#ifndef BYTECODE_H
#define BYTECODE_H

#include <stdint.h>

typedef enum {
    OP_NOP = 0,      // 无操作
    OP_PUSH,         // 压栈: PUSH const
    OP_POP,          // 出栈
    OP_ADD,          // 加法: pop b, pop a, push a+b
    OP_SUB,          // 减法
    OP_MUL,          // 乘法
    OP_DIV,          // 除法
    OP_NEG,          // 取负
    OP_LT,           // 小于比较
    OP_GT,           // 大于比较
    OP_EQ,           // 等于比较
    OP_JMP,          // 无条件跳转: JMP offset
    OP_JZ,           // 条件跳转(栈顶为0): JZ offset
    OP_CALL,         // 函数调用: CALL addr, argc
    OP_RET,          // 返回
    OP_LOAD,         // 加载变量: LOAD index
    OP_STORE,        // 存储变量: STORE index
    OP_PRINT,        // 打印栈顶
    OP_HALT          // 停止执行
} OpCode;

// 字节码结构
typedef struct {
    uint8_t *code;      // 指令数组
    int *constants;     // 常量池
    int code_size;      // 代码大小
    int const_count;    // 常量数量
} Bytecode;

#endif
