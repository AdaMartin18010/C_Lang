/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\02_System_Call_Hardware.md
 * Line: 818
 * Language: c
 * Block ID: 79a172a5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux内核系统调用分发
 * arch/x86/entry/common.c
 */

// 系统调用表定义 (arch/x86/entry/syscall_64.c)
asmlinkage const sys_call_ptr_t sys_call_table[] = {
    [0 ... __NR_syscall_max] = &sys_ni_syscall,
#include <asm/syscalls_64.h>
};

// 系统调用入口处理
long do_syscall_64(struct pt_regs *regs)
{
    unsigned long nr = regs->orig_ax;

    // 1. 系统调用追踪
    if (test_thread_flag(TIF_SYSCALL_TRACE))
        tracehook_report_syscall(regs, PTRACE_SYSCALL_ENTER);

    // 2. 审计入口
    audit_syscall_entry(regs->orig_ax, regs->di, regs->si,
                        regs->dx, regs->r10);

    // 3. seccomp过滤
    if (secure_computing() == -1)
        return -1;

    // 4. 系统调用分发
    if (likely(nr < NR_syscalls)) {
        // 调用系统调用处理函数
        regs->ax = sys_call_table[nr](
            regs->di, regs->si, regs->dx,
            regs->r10, regs->r8, regs->r9
        );
    } else {
        regs->ax = -ENOSYS;  // 无效系统调用
    }

    // 5. 系统调用返回追踪
    if (test_thread_flag(TIF_SYSCALL_TRACE))
        tracehook_report_syscall(regs, PTRACE_SYSCALL_EXIT);

    // 6. 审计出口
    audit_syscall_exit(regs);

    return regs->ax;
}

// 系统调用定义宏
#define SYSCALL_DEFINE0(sname)                  \
    SYSCALL_METADATA(_##sname, 0);              \
    asmlinkage long sys_##sname(void)

#define SYSCALL_DEFINE1(sname, ...)             \
    SYSCALL_DEFINEx(1, _##sname, __VA_ARGS__)

#define SYSCALL_DEFINE2(sname, ...)             \
    SYSCALL_DEFINEx(2, _##sname, __VA_ARGS__)

#define SYSCALL_DEFINE3(sname, ...)             \
    SYSCALL_DEFINEx(3, _##sname, __VA_ARGS__)

#define SYSCALL_DEFINE4(sname, ...)             \
    SYSCALL_DEFINEx(4, _##sname, __VA_ARGS__)

#define SYSCALL_DEFINE5(sname, ...)             \
    SYSCALL_DEFINEx(5, _##sname, __VA_ARGS__)

#define SYSCALL_DEFINE6(sname, ...)             \
    SYSCALL_DEFINEx(6, _##sname, __VA_ARGS__)
