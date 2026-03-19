/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\08_Debugging_Tools\03_Advanced_Debugging_Techniques.md
 * Line: 174
 * Language: c
 * Block ID: ef02dfc9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * Kprobe eBPF程序 - 函数入口追踪
 *
 * 用于追踪任意内核函数的入口点
 * 比tracepoint更灵活，但稳定性较差（内核API可能变化）
 */

#include <linux/bpf.h>
#include <linux/ptrace.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

char _license[] SEC("license") = "GPL";

/* 统计结构 */
struct stats {
    u64 count;
    u64 total_ns;
    u64 max_ns;
    u64 min_ns;
};

/* 按函数名统计性能 */
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 1024);
    __type(key, u64);       /* 函数地址 */
    __type(value, struct stats);
} func_stats SEC(".maps");

/* 暂存开始时间 */
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10240);
    __type(key, u64);       /* PID/TID */
    __type(value, u64);     /* 开始时间 */
} start_times SEC(".maps");

/**
 * 通用的函数入口追踪器
 * 使用kprobe可以附加到任意内核函数
 */
SEC("kprobe/vfs_read")
int BPF_KPROBE(vfs_read_entry, struct file *file, char __user *buf, size_t count, loff_t *pos)
{
    u64 pid_tgid = bpf_get_current_pid_tgid();
    u64 start_ns = bpf_ktime_get_ns();

    /* 记录函数开始时间 */
    bpf_map_update_elem(&start_times, &pid_tgid, &start_ns, BPF_ANY);

    return 0;
}

/**
 * 对应的kretprobe - 函数返回追踪器
 */
SEC("kretprobe/vfs_read")
int BPF_KRETPROBE(vfs_read_exit, ssize_t ret)
{
    u64 pid_tgid = bpf_get_current_pid_tgid();

    /* 查找开始时间 */
    u64 *start_ns = bpf_map_lookup_elem(&start_times, &pid_tgid);
    if (!start_ns)
        return 0;

    /* 计算执行时间 */
    u64 duration = bpf_ktime_get_ns() - *start_ns;

    /* 删除暂存数据 */
    bpf_map_delete_elem(&start_times, &pid_tgid);

    /* 更新统计信息 */
    u64 func_addr = PT_REGS_IP_CORE(ctx);
    struct stats *st = bpf_map_lookup_elem(&func_stats, &func_addr);

    if (st) {
        /* 更新现有统计 */
        st->count++;
        st->total_ns += duration;
        if (duration > st->max_ns)
            st->max_ns = duration;
        if (duration < st->min_ns)
            st->min_ns = duration;
    } else {
        /* 初始化新统计 */
        struct stats new_st = {
            .count = 1,
            .total_ns = duration,
            .max_ns = duration,
            .min_ns = duration
        };
        bpf_map_update_elem(&func_stats, &func_addr, &new_st, BPF_ANY);
    }

    return 0;
}
