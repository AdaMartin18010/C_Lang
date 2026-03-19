/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\03_JIT_Compilation.md
 * Line: 97
 * Language: c
 * Block ID: 95f26e57
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// jit_compiler.h
#ifndef JIT_COMPILER_H
#define JIT_COMPILER_H

#include <stdint.h>
#include <sys/mman.h>
#include "bytecode.h"

#define MAX_JIT_CODE_SIZE 65536
#define STACK_SIZE 1024

// JIT编译上下文
typedef struct {
    uint8_t *code_buffer;       // 代码缓冲区
    uint8_t *code_ptr;          // 当前写入位置
    int code_size;              // 生成代码大小
    void **const_table;         // 常量表地址
    int *stack_map;             // 栈映射（用于调试）
} JITContext;

// JIT编译函数原型
typedef int (*JITFunction)(void);

// 初始化JIT上下文
JITContext* jit_init(void);
void jit_free(JITContext *ctx);

// 编译字节码为机器码
JITFunction jit_compile(JITContext *ctx, Bytecode *bc);

// x86-64代码生成辅助函数
void emit_bytes(JITContext *ctx, const uint8_t *bytes, int len);
void emit_u8(JITContext *ctx, uint8_t val);
void emit_u32(JITContext *ctx, uint32_t val);
void emit_u64(JITContext *ctx, uint64_t val);

#endif
