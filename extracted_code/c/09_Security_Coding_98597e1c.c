/*
 * Auto-generated from: 12_Practice_Exercises\09_Security_Coding.md
 * Line: 269
 * Language: c
 * Block ID: 98597e1c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    // 允许的syscall白名单
    int *allowed_syscalls;
    size_t num_allowed;

    // 资源限制
    struct rlimit cpu_limit;
    struct rlimit memory_limit;
    struct rlimit file_limit;

    // 文件系统限制
    char *chroot_dir;
    bool read_only;
} SandboxConfig;

// 在沙箱中执行函数
int sandbox_execute(int (*func)(void *arg), void *arg,
                    const SandboxConfig *config);

// 示例：受限的计算
int sandbox_compute(void *arg) {
    // 这段代码在沙箱中运行
    // 只能使用白名单syscall
    int *nums = arg;
    return nums[0] + nums[1];
}
