/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Sandboxing.md
 * Line: 95
 * Language: c
 * Block ID: 2e0360ff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 沙箱类型枚举
typedef enum {
    SANDBOX_TYPE_CONTAINER,     // 容器级沙箱 (Docker, containerd)
    SANDBOX_TYPE_PROCESS,       // 进程级沙箱 (seccomp, AppArmor)
    SANDBOX_TYPE_VIRTUALIZED,   // 虚拟化沙箱 (KVM, Xen)
    SANDBOX_TYPE_WASM,          // WebAssembly沙箱
    SANDBOX_TYPE_BROWSER,       // 浏览器沙箱 (Chrome, Firefox)
    SANDBOX_TYPE_LANGUAGE,      // 语言运行时沙箱 (JVM, .NET)
} SandboxType;

// 沙箱配置结构
typedef struct {
    SandboxType type;
    uint64_t memory_limit;      // 内存限制 (字节)
    uint64_t cpu_quota;         // CPU配额 (微秒/周期)
    char* rootfs_path;          // 根文件系统路径
    char** allowed_syscalls;    // 允许的系统调用列表
    int syscall_count;
    bool network_isolated;      // 网络隔离标志
    bool mount_readonly;        // 只读挂载标志
} SandboxConfig;
