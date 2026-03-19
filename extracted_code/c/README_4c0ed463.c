/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\README.md
 * Line: 347
 * Language: c
 * Block ID: 4c0ed463
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 基于栈的虚拟机 JIT 编译器示例
// 将字节码编译为 x86-64 机器码

typedef enum {
    OP_PUSH,    // push immediate
    OP_ADD,     // pop two, push sum
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_LOAD,    // load from local variable
    OP_STORE,   // store to local variable
    OP_CALL,    // call function
    OP_RET,     // return
} OpCode;

typedef struct {
    OpCode opcode;
    int operand;
} Instruction;

typedef struct {
    Instruction* code;
    int code_size;
    int stack_depth;
    int max_stack;
} BytecodeFunction;

// JIT 编译状态
typedef struct {
    Assembler* asm;
    int stack_offset;  // 当前栈在临时内存中的偏移
    int locals_offset; // 局部变量在栈帧中的偏移
    void** const_pool; // 常量池（用于函数指针等）
} JitState;

// 将字节码编译为机器码
void* jit_compile(BytecodeFunction* func) {
    JitCode* jit = jit_code_create(4096);
    JitState state = {
        .asm = assembler_create(4096),
        .stack_offset = 0,
        .locals_offset = -8,  // 从 rbp-8 开始
        .const_pool = malloc(256 * sizeof(void*))
    };

    // 函数序言
    emit_push_rbp(&state);
    emit_mov_rbp_rsp(&state);
    emit_sub_rsp(&state, func->max_stack * 8);  // 分配栈空间

    for (int i = 0; i < func->code_size; i++) {
        Instruction* inst = &func->code[i];

        switch (inst->opcode) {
            case OP_PUSH:
                // push imm32
                emit_push_imm32(&state, inst->operand);
                state.stack_offset += 8;
                break;

            case OP_ADD: {
                // pop %rax
                emit_pop_rax(&state);
                // pop %rbx
                emit_pop_rbx(&state);
                // add %rbx, %rax
                emit_add_rax_rbx(&state);
                // push %rax
                emit_push_rax(&state);
                state.stack_offset -= 8;  // net -8 (pop 2, push 1)
                break;
            }

            case OP_LOAD: {
                // mov -offset(%rbp), %rax
                int var_offset = state.locals_offset - inst->operand * 8;
                emit_mov_rax_mem_rbp(&state, var_offset);
                emit_push_rax(&state);
                state.stack_offset += 8;
                break;
            }

            case OP_STORE: {
                // pop %rax
                emit_pop_rax(&state);
                // mov %rax, -offset(%rbp)
                int var_offset = state.locals_offset - inst->operand * 8;
                emit_mov_mem_rbp_rax(&state, var_offset);
                state.stack_offset -= 8;
                break;
            }

            case OP_CALL: {
                // 准备参数（从栈中弹出）
                int num_args = inst->operand;
                // System V AMD64: rdi, rsi, rdx, rcx, r8, r9
                const int arg_regs[] = {RDI, RSI, RDX, RCX, R8, R9};

                for (int j = num_args - 1; j >= 0; j--) {
                    emit_pop_reg(&state, arg_regs[j]);
                }

                // call function
                emit_call(&state, state.const_pool[inst->operand]);

                // 推送返回值
                emit_push_rax(&state);
                state.stack_offset += 8 - num_args * 8;
                break;
            }

            case OP_RET:
                // mov %rbp, %rsp
                emit_mov_rsp_rbp(&state);
                // pop %rbp
                emit_pop_rbp(&state);
                // ret
                emit_ret(&state);
                break;
        }
    }

    // 如果没有显式 ret，添加默认返回
    emit_mov_rsp_rbp(&state);
    emit_pop_rbp(&state);
    emit_ret(&state);

    // 复制到可执行内存
    memcpy(jit->code_buffer, state.asm->buffer, state.asm->size);
    jit_code_finalize(jit);
    flush_instruction_cache(jit->code_buffer, state.asm->size);

    free(state.asm->buffer);
    free(state.asm);
    free(state.const_pool);

    return jit->code_buffer;
}
