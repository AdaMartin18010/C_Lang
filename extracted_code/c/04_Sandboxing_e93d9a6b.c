/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Sandboxing.md
 * Line: 953
 * Language: c
 * Block ID: e93d9a6b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 沙箱设计原则实现示例

// 原则1: 默认拒绝 (Default Deny)
SeccompPolicy CreateDefaultDenyPolicy() {
    SeccompPolicy policy;

    // 默认动作：拒绝并记录
    policy.default_action = SECCOMP_RET_LOG;

    // 只允许明确需要的系统调用
    policy.allowed_syscalls = {
        __NR_exit,
        __NR_exit_group,
        __NR_read,   // 仅从stdin
        __NR_write,  // 仅向stdout/stderr
    };

    return policy;
}

// 原则2: 最小权限 (Least Privilege)
void ApplyLeastPrivilege(SandboxContext* ctx) {
    // 仅授予完成任务所需的最小能力集
    cap_value_t minimal_caps[] = {
        // 对于只读服务，不需要任何能力
    };

    retain_capabilities_only(minimal_caps, 0);
}

// 原则3: 纵深防御 (Defense in Depth)
void SetupDefenseInDepth() {
    // 第一层：Namespace隔离
    unshare(CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWNET | CLONE_NEWUSER);

    // 第二层：资源限制
    struct rlimit limits = {
        .rlim_cur = 100 * 1024 * 1024,  // 100MB内存
        .rlim_max = 100 * 1024 * 1024,
    };
    setrlimit(RLIMIT_AS, &limits);

    // 第三层：Seccomp过滤
    install_seccomp_filter();

    // 第四层：Capabilities丢弃
    drop_all_capabilities();

    // 第五层：chroot jail
    chroot("/empty");

    // 第六层：文件系统只读挂载
    mount(NULL, "/", NULL, MS_REMOUNT | MS_RDONLY, NULL);
}

// 原则4: 快速失败 (Fail Fast)
void ConfigureFailFast() {
    // 资源限制设置为硬限制
    prctl(PR_SET_SECCOMP, SECCOMP_MODE_STRICT);

    // 内存过量使用控制
    // 防止OOM时影响系统其他部分
}
