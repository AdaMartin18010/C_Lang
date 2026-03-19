/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\08_Debugging_Tools\03_Advanced_Debugging_Techniques.md
 * Line: 77
 * Language: c
 * Block ID: a3b064ea
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * eBPF程序基本结构 - 系统调用追踪器
 *
 * 功能：追踪所有openat系统调用，记录进程名和文件名
 * 编译：clang -O2 -target bpf -c trace_open.c -o trace_open.o
 * 加载：bpftool prog load trace_open.o /sys/fs/bpf/trace_open
 */

#include <linux/bpf.h>
#include <linux/ptrace.h>
#include <linux/sched.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

/* 定义eBPF程序许可证，GPL是必需的 */
char _license[] SEC("license") = "GPL";

/* 定义最大文件名长度 */
#define MAX_FILENAME_LEN 256
#define MAX_COMM_LEN 16

/**
 * eBPF Map定义 - 用于内核与用户空间通信
 *
 * 类型：BPF_MAP_TYPE_HASH - 哈希表
 * 键：u32（进程ID）
 * 值：自定义结构体（包含文件名和进程名）
 */
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10240);
    __type(key, u32);
    __type(value, struct event);
} events SEC(".maps");

/* 事件数据结构 */
struct event {
    u32 pid;
    u32 uid;
    char comm[MAX_COMM_LEN];
    char filename[MAX_FILENAME_LEN];
    u64 timestamp;
};

/* 环形缓冲区，用于向用户空间发送事件 */
struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 256 * 1024);  /* 256KB环形缓冲区 */
} rb SEC(".maps");

/**
 * eBPF程序入口点 - 追踪openat系统调用
 *
 * SEC宏用于指定程序类型和附加点
 * "tp/syscalls/sys_enter_openat" 表示tracepoint
 */
SEC("tp/syscalls/sys_enter_openat")
int trace_openat(struct trace_event_raw_sys_enter *ctx)
{
    /* 获取当前进程信息 */
    u32 pid = bpf_get_current_pid_tgid() >> 32;
    u32 uid = bpf_get_current_uid_gid() & 0xFFFFFFFF;

    /* 过滤条件：只追踪UID > 1000的普通用户进程 */
    if (uid < 1000)
        return 0;

    /* 从环形缓冲区预留空间 */
    struct event *e = bpf_ringbuf_reserve(&rb, sizeof(*e), 0);
    if (!e)
        return 0;  /* 缓冲区满，丢弃事件 */

    /* 填充事件数据 */
    e->pid = pid;
    e->uid = uid;
    e->timestamp = bpf_ktime_get_ns();

    /* 获取进程名 */
    bpf_get_current_comm(&e->comm, sizeof(e->comm));

    /* 读取系统调用参数（文件名）
     * ctx->args[1] 是openat的第二个参数（pathname）
     * bpf_probe_read_user_str 安全地从用户空间读取字符串
     */
    const char *filename = (const char *)ctx->args[1];
    bpf_probe_read_user_str(e->filename, sizeof(e->filename), filename);

    /* 提交事件到环形缓冲区 */
    bpf_ringbuf_submit(e, 0);

    return 0;
}
