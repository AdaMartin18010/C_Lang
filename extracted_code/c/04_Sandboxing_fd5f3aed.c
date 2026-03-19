/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Sandboxing.md
 * Line: 205
 * Language: c
 * Block ID: fd5f3aed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 更复杂的seccomp策略 - 带参数检查
struct sock_filter advanced_filter[] = {
    // 验证架构
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS,
             offsetof(struct seccomp_data, arch)),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, AUDIT_ARCH_X86_64, 1, 0),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL),

    // 加载系统调用号
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS,
             offsetof(struct seccomp_data, nr)),

    // 允许的基本调用列表
    ALLOW_SYSCALL(exit_group),
    ALLOW_SYSCALL(exit),
    ALLOW_SYSCALL(read),
    ALLOW_SYSCALL(write),
    ALLOW_SYSCALL(close),

    // 对openat进行参数检查 - 只允许只读打开
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_openat, 0, 8),
    // 检查 flags 参数
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS,
             offsetof(struct seccomp_data, args[2])),
    // 只允许 O_RDONLY (0)
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, O_RDONLY, 1, 0),
    // 或者 O_RDONLY | O_CLOEXEC
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, O_RDONLY | O_CLOEXEC, 0, 1),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
    // 其他flags - 记录并拒绝
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_LOG),

    // 允许带有路径检查的特定文件访问
    // ... 更多规则

    // 默认动作：记录并拒绝
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_LOG),
};

// seccomp操作码宏定义
#define ALLOW_SYSCALL(name) \
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_##name, 0, 1), \
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW)
