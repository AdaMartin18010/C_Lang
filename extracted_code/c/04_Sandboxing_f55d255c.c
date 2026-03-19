/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Sandboxing.md
 * Line: 264
 * Language: c
 * Block ID: f55d255c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用libseccomp简化seccomp编程
#include <seccomp.h>

int setup_seccomp_advanced() {
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_LOG);  // 默认记录
    if (ctx == NULL) {
        return -1;
    }

    // 添加基本允许规则
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(close), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);

    // 添加带参数检查的规则
    // 只允许从标准输入读取
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 1,
                     SCMP_A0(SCMP_CMP_EQ, STDIN_FILENO));

    // 只允许写入标准输出和标准错误
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 1,
                     SCMP_A0(SCMP_CMP_LE, STDERR_FILENO));

    // 限制mmap - 不允许可执行内存
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mmap), 2,
                     SCMP_A2(SCMP_CMP_MASKED_EQ, PROT_EXEC, 0),
                     SCMP_A2(SCMP_CMP_MASKED_EQ, PROT_WRITE, PROT_WRITE));

    // 网络相关的系统调用默认拒绝
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(socket), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(connect), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(bind), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(listen), 0);

    // 加载过滤器
    int rc = seccomp_load(ctx);

    // 释放上下文
    seccomp_release(ctx);

    return rc;
}
