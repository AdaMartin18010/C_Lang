/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Sandboxing.md
 * Line: 1021
 * Language: c
 * Block ID: bdf9d11d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 常见沙箱逃逸途径及防护

// 1. 防止通过/proc/self/mem修改代码
void ProtectCodeIntegrity() {
    // 使用mprotect使代码段只读
    const char* maps_path = "/proc/self/maps";
    // 解析maps，找到代码段
    // 设置 PROT_READ | PROT_EXEC (无 PROT_WRITE)
}

// 2. 防止通过ptrace调试其他进程
void PreventPtraceEscape() {
    // 使用PR_SET_DUMPABLE
    prctl(PR_SET_DUMPABLE, 0);

    // 在seccomp中禁止ptrace系统调用
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(ptrace), 0);
}

// 3. 防止通过fork/exec逃逸
void RestrictProcessCreation() {
    // 限制fork
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(fork), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(vfork), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(clone), 0);

    // 限制execve
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(execve), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(execveat), 0);
}

// 4. 防止通过信号逃逸
void ConfigureSignalRestrictions() {
    // 只允许必要的信号
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGSEGV);  // 段错误
    sigdelset(&mask, SIGILL);   // 非法指令
    sigdelset(&mask, SIGTERM);  // 终止信号
    sigprocmask(SIG_BLOCK, &mask, NULL);
}
