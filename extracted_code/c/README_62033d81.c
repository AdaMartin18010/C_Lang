/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\12_Self_Modifying_Code\README.md
 * Line: 395
 * Language: c
 * Block ID: 62033d81
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用seccomp限制JIT编译器系统调用

#include <seccomp.h>

void setup_jit_sandbox(void) {
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_KILL);

    // 允许基本操作
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);

    // 允许mmap/mprotect用于代码生成
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mmap), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mprotect), 0);

    // 禁止网络、文件系统等危险操作
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(socket), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(open), 0);

    seccomp_load(ctx);
}
