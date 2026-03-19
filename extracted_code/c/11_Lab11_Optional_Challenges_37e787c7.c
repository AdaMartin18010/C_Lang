/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\11_Lab11_Optional_Challenges.md
 * Line: 102
 * Language: c
 * Block ID: 37e787c7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 命名空间隔离
struct namespace {
    struct pid_namespace *pid_ns;
    struct mount_namespace *mnt_ns;
    struct network_namespace *net_ns;
};

// cgroup资源限制
struct cgroup {
    uint64_t max_memory;
    uint64_t max_cpu;
    uint64_t max_io;
};

// 容器进程
struct container {
    char name[32];
    struct namespace ns;
    struct cgroup cg;
    struct proc *init_proc;
};
