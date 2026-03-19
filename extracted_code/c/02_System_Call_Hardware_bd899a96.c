/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\02_System_Call_Hardware.md
 * Line: 1631
 * Language: c
 * Block ID: bd899a96
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 系统调用审计框架
 */

// 审计入口点
void audit_syscall_entry(unsigned int arch,
                         unsigned long major,
                         unsigned long a1, unsigned long a2,
                         unsigned long a3, unsigned long a4)
{
    struct audit_context *ctx = current->audit_context;

    if (!ctx || ctx->dummy)
        return;

    ctx->arch = arch;
    ctx->major = major;
    ctx->argv[0] = a1;
    ctx->argv[1] = a2;
    ctx->argv[2] = a3;
    ctx->argv[3] = a4;

    // 记录系统调用开始时间
    ctx->syscall_entrytime = ktime_get();
}

// Seccomp BPF过滤
static int __seccomp_filter(int this_syscall, const struct seccomp_data *sd)
{
    u32 filter_ret, action;
    int data;

    // 执行BPF程序
    filter_ret = seccomp_run_filters(sd);
    action = filter_ret & SECCOMP_RET_ACTION_FULL;
    data = filter_ret & SECCOMP_RET_DATA;

    switch (action) {
    case SECCOMP_RET_ERRNO:
        // 返回错误码而不执行系统调用
        syscall_set_return_value(current, task_pt_regs(current),
                                  -data, 0);
        return -1;

    case SECCOMP_RET_TRAP:
        // 发送SIGSYS信号
        seccomp_send_sigsys(this_syscall, data);
        return -1;

    case SECCOMP_RET_TRACE:
        // 通知ptracer
        return ptrace_event(PTRACE_EVENT_SECCOMP, data);

    case SECCOMP_RET_ALLOW:
        // 允许执行
        return 0;

    case SECCOMP_RET_KILL_PROCESS:
    case SECCOMP_RET_KILL_THREAD:
    default:
        // 终止进程
        seccomp_log(this_syscall, SIGSYS, action, true);
        do_exit(SIGSYS);
    }
}
