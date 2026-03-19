/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\03_JIT_Compilation.md
 * Line: 139
 * Language: c
 * Block ID: 98d5a9ed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// jit_compiler.c
#include "jit_compiler.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// x86-64寄存器分配
// RAX - 临时计算结果
// RCX, RDX - 辅助寄存器
// RSP - 栈指针（运行时栈）
// RBP - 基址指针
// RSI, RDI - 用于参数传递

// 常用的x86-64指令编码
#define PUSH_RAX        0x50
#define POP_RAX         0x58
#define MOV_RAX_IMM32   0xB8        // mov rax, imm32 (实际用48 C7 C0 imm32)
#define ADD_RAX_RBX     0x01, 0xD8  // add rax, rbx
#define SUB_RAX_RBX     0x29, 0xD8  // sub rax, rbx
#define IMUL_RAX_RBX    0x0F, 0xAF, 0xC3  // imul rax, rbx
#define PUSH_IMM32      0x68        // push imm32
#define RET             0xC3
#define NOP             0x90

// 初始化JIT上下文
JITContext* jit_init(void) {
    JITContext *ctx = malloc(sizeof(JITContext));
    if (!ctx) return NULL;

    // 分配可执行内存
    ctx->code_buffer = mmap(NULL, MAX_JIT_CODE_SIZE,
                            PROT_READ | PROT_WRITE | PROT_EXEC,
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ctx->code_buffer == MAP_FAILED) {
        free(ctx);
        return NULL;
    }

    ctx->code_ptr = ctx->code_buffer;
    ctx->code_size = 0;
    ctx->const_table = NULL;
    ctx->stack_map = NULL;

    return ctx;
}

void jit_free(JITContext *ctx) {
    if (ctx) {
        if (ctx->code_buffer) {
            munmap(ctx->code_buffer, MAX_JIT_CODE_SIZE);
        }
        free(ctx->const_table);
        free(ctx->stack_map);
        free(ctx);
    }
}

// 辅助函数：写入字节
void emit_bytes(JITContext *ctx, const uint8_t *bytes, int len) {
    memcpy(ctx->code_ptr, bytes, len);
    ctx->code_ptr += len;
    ctx->code_size += len;
}

void emit_u8(JITContext *ctx, uint8_t val) {
    *ctx->code_ptr++ = val;
    ctx->code_size++;
}

void emit_u32(JITContext *ctx, uint32_t val) {
    memcpy(ctx->code_ptr, &val, 4);
    ctx->code_ptr += 4;
    ctx->code_size += 4;
}

// 生成函数序言（设置栈帧）
static void emit_prologue(JITContext *ctx) {
    // push rbp
    emit_u8(ctx, 0x55);
    // mov rbp, rsp
    emit_bytes(ctx, (uint8_t[]){0x48, 0x89, 0xE5}, 3);
    // sub rsp, STACK_SIZE  (分配栈空间)
    emit_bytes(ctx, (uint8_t[]){0x48, 0x81, 0xEC}, 3);
    emit_u32(ctx, STACK_SIZE);
}

// 生成函数尾声
static void emit_epilogue(JITContext *ctx) {
    // mov rsp, rbp
    emit_bytes(ctx, (uint8_t[]){0x48, 0x89, 0xEC}, 3);
    // pop rbp
    emit_u8(ctx, 0x5D);
    // ret
    emit_u8(ctx, RET);
}

// 生成 push 立即数指令
static void emit_push_imm(JITContext *ctx, int32_t value) {
    // mov eax, imm32
    emit_u8(ctx, 0xB8);
    emit_u32(ctx, (uint32_t)value);
    // push rax
    emit_u8(ctx, PUSH_RAX);
}

// 生成 pop 到寄存器
static void emit_pop_rax(JITContext *ctx) {
    emit_u8(ctx, POP_RAX);
}

// 生成二元运算（栈顶两元素）
static void emit_binary_op(JITContext *ctx, uint8_t op) {
    // pop rbx (第二个操作数)
    emit_u8(ctx, 0x5B);
    // pop rax (第一个操作数)
    emit_u8(ctx, POP_RAX);
    // 执行运算 (结果在rax)
    emit_u8(ctx, op);
    // push rax
    emit_u8(ctx, PUSH_RAX);
}

// 主要编译函数
JITFunction jit_compile(JITContext *ctx, Bytecode *bc) {
    ctx->code_ptr = ctx->code_buffer;
    ctx->code_size = 0;

    // 生成函数序言
    emit_prologue(ctx);

    int pc = 0;
    int *jump_targets = calloc(bc->code_size, sizeof(int));

    // 第一遍：确定跳转目标
    for (pc = 0; pc < bc->code_size; ) {
        OpCode op = bc->code[pc];
        if (op == OP_JMP || op == OP_JZ) {
            int16_t offset = (int16_t)(bc->code[pc+1] | (bc->code[pc+2] << 8));
            int target = pc + 3 + offset;
            jump_targets[target] = 1;
        }
        pc += get_instruction_size(op);
    }

    // 第二遍：生成代码
    pc = 0;
    while (pc < bc->code_size) {
        OpCode op = bc->code[pc];

        // 记录跳转目标位置
        if (jump_targets[pc]) {
            jump_targets[pc] = ctx->code_size;
        }

        switch (op) {
            case OP_NOP:
                emit_u8(ctx, NOP);
                pc++;
                break;

            case OP_PUSH: {
                int const_idx = bc->code[pc + 1];
                int32_t value = bc->constants[const_idx];
                emit_push_imm(ctx, value);
                pc += 2;
                break;
            }

            case OP_POP:
                // pop rax (丢弃栈顶)
                emit_u8(ctx, POP_RAX);
                pc++;
                break;

            case OP_ADD: {
                // pop rbx, pop rax, add rax, rbx, push rax
                emit_u8(ctx, 0x5B);
                emit_u8(ctx, POP_RAX);
                emit_bytes(ctx, (uint8_t[]){0x48, 0x01, 0xD8}, 3);
                emit_u8(ctx, PUSH_RAX);
                pc++;
                break;
            }

            case OP_SUB: {
                emit_u8(ctx, 0x5B);
                emit_u8(ctx, POP_RAX);
                emit_bytes(ctx, (uint8_t[]){0x48, 0x29, 0xD8}, 3);
                emit_u8(ctx, PUSH_RAX);
                pc++;
                break;
            }

            case OP_MUL: {
                // 乘法需要更复杂的处理
                emit_u8(ctx, 0x5B);  // pop rbx
                emit_u8(ctx, POP_RAX);  // pop rax
                emit_bytes(ctx, (uint8_t[]){0x48, 0x0F, 0xAF, 0xC3}, 4); // imul rax, rbx
                emit_u8(ctx, PUSH_RAX);
                pc++;
                break;
            }

            case OP_NEG: {
                // pop rax, neg rax, push rax
                emit_u8(ctx, POP_RAX);
                emit_bytes(ctx, (uint8_t[]){0x48, 0xF7, 0xD8}, 3); // neg rax
                emit_u8(ctx, PUSH_RAX);
                pc++;
                break;
            }

            case OP_PRINT: {
                // 调用 C 的 printf 函数（简化版，仅用于演示）
                // 实际实现需要设置调用约定和参数
                emit_u8(ctx, POP_RAX);
                // 将结果保留在栈上（通过RAX返回值）
                emit_u8(ctx, PUSH_RAX);
                pc++;
                break;
            }

            case OP_HALT:
                // 生成函数尾声
                emit_epilogue(ctx);
                pc++;
                break;

            default:
                pc++;
                break;
        }
    }

    free(jump_targets);

    // 刷新指令缓存（x86-64需要）
    __builtin___clear_cache(ctx->code_buffer, ctx->code_ptr);

    return (JITFunction)ctx->code_buffer;
}

// 获取指令大小（用于跳转计算）
static int get_instruction_size(OpCode op) {
    switch (op) {
        case OP_PUSH:
        case OP_LOAD:
        case OP_STORE:
            return 2;
        case OP_JMP:
        case OP_JZ:
        case OP_CALL:
            return 3;
        default:
            return 1;
    }
}
