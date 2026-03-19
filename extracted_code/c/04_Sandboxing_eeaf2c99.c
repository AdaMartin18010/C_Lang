/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Sandboxing.md
 * Line: 452
 * Language: c
 * Block ID: eeaf2c99
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    char* rootfs;
    char* hostname;
    uid_t uid_inside;
    gid_t gid_inside;
    bool isolate_network;
    bool readonly_root;
    char** mount_binds;
    int mount_count;
} SandboxContext;

int init_sandbox(SandboxContext* ctx) {
    // 1. 创建新的namespace
    int flags = CLONE_NEWNS | CLONE_NEWUTS | CLONE_NEWIPC |
                CLONE_NEWPID | CLONE_NEWUSER;
    if (ctx->isolate_network) {
        flags |= CLONE_NEWNET;
    }

    if (unshare(flags) < 0) {
        perror("unshare");
        return -1;
    }

    // 2. 设置主机名
    sethostname(ctx->hostname, strlen(ctx->hostname));

    // 3. 配置user namespace映射
    setup_user_namespace_mapping(ctx->uid_inside, ctx->gid_inside);

    // 4. 设置root文件系统
    setup_rootfs(ctx->rootfs, ctx->readonly_root);

    // 5. 绑定挂载额外目录
    for (int i = 0; i < ctx->mount_count; i++) {
        bind_mount(ctx->mount_binds[i], ctx->rootfs);
    }

    // 6. 配置网络(如果需要)
    if (!ctx->isolate_network) {
        // 保持宿主网络访问，但限制能力
    }

    // 7. 设置seccomp过滤器
    setup_seccomp_filter();

    // 8. 设置capabilities
    drop_capabilities();

    return 0;
}
