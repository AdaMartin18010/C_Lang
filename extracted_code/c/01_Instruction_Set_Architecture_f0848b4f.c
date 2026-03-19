/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\01_Instruction_Set_Architecture.md
 * Line: 539
 * Language: c
 * Block ID: f0848b4f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 特权指令（只能在Ring 0执行）
typedef enum {
    INSTR_HLT,      // 停机
    INSTR_CLI,      // 关中断
    INSTR_STI,      // 开中断
    INSTR_IN,       // 端口输入
    INSTR_OUT,      // 端口输出
    INSTR_LGDT,     // 加载GDT
    INSTR_LIDT,     // 加载IDT
    INSTR_MOV_CR,   // 访问控制寄存器
    INSTR_INVLPG,   // 刷新TLB
} Privileged_Instruction;

// 系统调用接口（从Ring 3到Ring 0的受控入口）
typedef enum {
    SYSCALL_READ = 0,
    SYSCALL_WRITE = 1,
    SYSCALL_OPEN = 2,
    SYSCALL_CLOSE = 3,
    SYSCALL_MMAP = 9,
    SYSCALL_EXIT = 60,
    // ... Linux x86-64 syscall numbers
} Syscall_Number;

// 系统调用执行
void execute_syscall(X86_64_State *state) {
    uint64_t syscall_num = state->rax;
    uint64_t arg1 = state->rdi;
    uint64_t arg2 = state->rsi;
    uint64_t arg3 = state->rdx;

    switch (syscall_num) {
        case SYSCALL_WRITE:
            // fd = arg1, buf = arg2, count = arg3
            state->rax = sys_write(arg1,
                                   state->memory + arg2,
                                   arg3);
            break;
        case SYSCALL_EXIT:
            // status = arg1
            exit((int)arg1);
            break;
        // ... 其他系统调用
    }
}
