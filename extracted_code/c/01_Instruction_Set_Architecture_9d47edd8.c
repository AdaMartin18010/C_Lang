/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\01_Instruction_Set_Architecture.md
 * Line: 413
 * Language: c
 * Block ID: 9d47edd8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 函数调用约定实现

typedef struct {
    // 参数寄存器
    uint64_t rdi, rsi, rdx, rcx, r8, r9;  // 整数参数（最多6个）
    double xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;  // 浮点参数

    // 返回值
    uint64_t rax, rdx;  // 整数返回（RAX为主，RDX为扩展）
    double xmm0_ret;     // 浮点返回

    // 调用者保存（volatile）
    // RAX, RCX, RDX, RSI, RDI, R8-R11, XMM0-XMM15

    // 被调用者保存（non-volatile）
    // RBX, RBP, R12-R15

    // 栈对齐要求：16字节边界
    uint64_t stack_alignment;
} Function_Call_Context;

// 模拟函数调用
void simulate_function_call(X86_64_State *state,
                            uint64_t func_addr,
                            uint64_t args[],
                            int num_args) {
    // 保存返回地址
    state->rsp -= 8;
    *(uint64_t*)(state->memory + state->rsp) = state->rip;

    // 设置参数（根据ABI）
    const int ARG_REGS[] = {REG_RDI, REG_RSI, REG_RDX,
                            REG_RCX, REG_R8, REG_R9};
    for (int i = 0; i < num_args && i < 6; i++) {
        set_register(state, ARG_REGS[i], args[i]);
    }

    // 超过6个的参数压栈
    for (int i = num_args - 1; i >= 6; i--) {
        state->rsp -= 8;
        *(uint64_t*)(state->memory + state->rsp) = args[i];
    }

    // 对齐到16字节（返回地址已占8字节，所以额外需要8字节对齐）
    if ((state->rsp % 16) != 0) {
        state->rsp -= 8;
    }

    // 跳转
    state->rip = func_addr;
}

// 模拟函数返回
void simulate_function_return(X86_64_State *state, uint64_t return_value) {
    state->rax = return_value;

    // 恢复栈
    state->rsp += 8;  // 跳过对齐填充（如果有）

    // 恢复返回地址
    state->rip = *(uint64_t*)(state->memory + state->rsp);
    state->rsp += 8;
}
