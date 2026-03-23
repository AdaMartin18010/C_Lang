# eBPF性能追踪实战

> **层级定位**: 02 Formal Semantics and Physics / 16 Performance Analysis Hardware
> **难度级别**: L5 专家
> **预估学习时间**: 8-10 小时
> **前置知识**: eBPF基础、性能分析

---

## 1. eBPF在性能分析中的应用

### 1.1 性能分析层次

```
应用层性能分析
    ├── 函数调用耗时 (kprobe/fentry)
    ├── 系统调用频率 (tracepoint)
    ├── 内存分配追踪 (kmem)
    └── 文件I/O分析 (vfs)

系统层性能分析
    ├── CPU调度延迟 (sched)
    ├── 网络栈性能 (kprobes in network)
    ├── 块设备I/O (block)
    └── 定时器精度 (hrtimer)

硬件层性能分析
    ├── 缓存未命中 (PMC with BPF)
    ├── 分支预测 (perf_event)
    └── 内存带宽
```

### 1.2 与传统工具对比

| 场景 | perf | ftrace | eBPF |
|:-----|:-----|:-------|:-----|
| 低开销追踪 | ✅ | ✅ | ✅ |
| 自定义聚合 | ❌ | ❌ | ✅ |
| 实时分析 | ❌ | ⚠️ | ✅ |
| 用户态数据 | ⚠️ | ❌ | ✅ |
| 复杂逻辑 | ❌ | ❌ | ✅ |

---

## 2. 函数级性能分析

### 2.1 函数调用耗时统计

```c
// func_latency.bpf.c
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

char LICENSE[] SEC("license") = "GPL";

// 存储函数进入时间
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10000);
    __type(key, u64);       // PID+TID
    __type(value, u64);     // 进入时间戳
} start_time SEC(".maps");

// 延迟直方图
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 100);
    __type(key, u32);       // 函数ID
    __type(value, struct hist);
} latency_hist SEC(".maps");

struct hist {
    u64 slots[32];  // 对数直方图槽
};

SEC("fentry/do_syscall_64")
int BPF_PROG(trace_syscall_entry) {
    u64 id = bpf_get_current_pid_tgid();
    u64 ts = bpf_ktime_get_ns();
    bpf_map_update_elem(&start_time, &id, &ts, BPF_ANY);
    return 0;
}

SEC("fexit/do_syscall_64")
int BPF_PROG(trace_syscall_exit, long ret) {
    u64 id = bpf_get_current_pid_tgid();
    u64 *start = bpf_map_lookup_elem(&start_time, &id);
    if (!start) return 0;

    u64 delta = bpf_ktime_get_ns() - *start;

    // 计算对数槽位
    u32 slot = bpf_log2l(delta / 1000);  // 微秒
    if (slot >= 32) slot = 31;

    u32 func_id = 0;  // syscall入口
    struct hist *h = bpf_map_lookup_elem(&latency_hist, &func_id);
    if (h) {
        __sync_fetch_and_add(&h->slots[slot], 1);
    }

    bpf_map_delete_elem(&start_time, &id);
    return 0;
}
```

### 2.2 Off-CPU时间分析

```c
// offcputime.bpf.c
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

char LICENSE[] SEC("license") = "GPL";

struct event {
    u32 pid;
    u32 tgid;
    u64 offcpu_ns;
    int user_stack_id;
    int kernel_stack_id;
};

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10000);
    __type(key, u32);       // PID
    __type(value, u64);     // 切换出去的时间
} start SEC(".maps");

struct {
    __uint(type, BPF_MAP_TYPE_STACK_TRACE);
    __uint(key_size, sizeof(u32));
    __type(value, u64[127]);
} stackmap SEC(".maps");

struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 256 * 1024);
} rb SEC(".maps");

SEC("tp/sched/sched_switch")
int trace_sched_switch(struct trace_event_raw_sched_switch *ctx) {
    u64 ts = bpf_ktime_get_ns();

    // 当前进程切换出去
    u32 prev_pid = ctx->prev_pid;
    if (prev_pid > 0) {
        bpf_map_update_elem(&start, &prev_pid, &ts, BPF_ANY);
    }

    // 当前进程切换回来
    u32 next_pid = ctx->next_pid;
    u64 *tsp = bpf_map_lookup_elem(&start, &next_pid);
    if (tsp) {
        u64 delta = ts - *tsp;

        struct event *e = bpf_ringbuf_reserve(&rb, sizeof(*e), 0);
        if (e) {
            e->pid = next_pid;
            e->tgid = bpf_get_current_pid_tgid() >> 32;
            e->offcpu_ns = delta;
            e->user_stack_id = bpf_get_stackid(ctx, &stackmap, BPF_F_USER_STACK);
            e->kernel_stack_id = bpf_get_stackid(ctx, &stackmap, 0);
            bpf_ringbuf_submit(e, 0);
        }

        bpf_map_delete_elem(&start, &next_pid);
    }

    return 0;
}
```

---

## 3. 内存性能分析

### 3.1 内存分配追踪

```c
// memleak.bpf.c
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

char LICENSE[] SEC("license") = "GPL";

struct alloc_info {
    u64 size;
    u64 timestamp;
    int stack_id;
};

// 活跃分配
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 100000);
    __type(key, u64);       // 地址
    __type(value, struct alloc_info);
} allocs SEC(".maps");

// 栈跟踪
struct {
    __uint(type, BPF_MAP_TYPE_STACK_TRACE);
    __uint(key_size, sizeof(u32));
    __type(value, u64[127]);
} stacktraces SEC(".maps");

SEC("kprobe/__kmalloc")
int BPF_KPROBE(kmalloc_entry, size_t size, gfp_t flags) {
    u64 pid_tgid = bpf_get_current_pid_tgid();
    u64 timestamp = bpf_ktime_get_ns();

    // 暂存分配信息
    u64 stack_id = bpf_get_stackid(ctx, &stacktraces, 0);

    // 实际在kretprobe中记录地址
    return 0;
}

SEC("kretprobe/__kmalloc")
int BPF_KRETPROBE(kmalloc_return, void *ret) {
    if (!ret) return 0;

    u64 addr = (u64)ret;
    struct alloc_info info = {};
    info.size = /* 从kmalloc_entry传递 */ 0;
    info.timestamp = bpf_ktime_get_ns();
    info.stack_id = bpf_get_stackid(ctx, &stacktraces, 0);

    bpf_map_update_elem(&allocs, &addr, &info, BPF_ANY);
    return 0;
}

SEC("kprobe/kfree")
int BPF_KPROBE(kfree_entry, void *ptr) {
    u64 addr = (u64)ptr;
    bpf_map_delete_elem(&allocs, &addr);
    return 0;
}
```

### 3.2 页错误分析

```c
// page_fault.bpf.c
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

char LICENSE[] SEC("license") = "GPL";

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 1024);
    __type(key, u32);       // PID
    __type(value, u64);     // 页错误次数
} pf_count SEC(".maps");

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 1024);
    __type(key, u32);       // PID
    __type(value, u64);     // 总延迟
} pf_latency SEC(".maps");

SEC("fentry/handle_mm_fault")
int BPF_PROG(mm_fault_entry) {
    u32 pid = bpf_get_current_pid_tgid() >> 32;
    u64 *count = bpf_map_lookup_elem(&pf_count, &pid);
    if (count) {
        (*count)++;
    } else {
        u64 init = 1;
        bpf_map_update_elem(&pf_count, &pid, &init, BPF_ANY);
    }
    return 0;
}
```

---

## 4. 网络性能分析

### 4.1 TCP连接延迟

```c
// tcpconnlat.bpf.c
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>

char LICENSE[] SEC("license") = "GPL";

struct event {
    u32 pid;
    u32 saddr;
    u32 daddr;
    u16 sport;
    u16 dport;
    u64 delta_us;
};

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10000);
    __type(key, u64);       // socket指针
    __type(value, u64);     // 开始时间
} start SEC(".maps");

struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 256 * 1024);
} rb SEC(".maps");

SEC("fentry/tcp_v4_connect")
int BPF_PROG(tcp_connect_entry, struct sock *sk) {
    u64 ts = bpf_ktime_get_ns();
    u64 sk_addr = (u64)sk;
    bpf_map_update_elem(&start, &sk_addr, &ts, BPF_ANY);
    return 0;
}

SEC("fexit/tcp_v4_connect")
int BPF_PROG(tcp_connect_exit, struct sock *sk, int ret) {
    u64 sk_addr = (u64)sk;
    u64 *tsp = bpf_map_lookup_elem(&start, &sk_addr);
    if (!tsp) return 0;

    // 只关注已建立的连接
    if (BPF_CORE_READ(sk, __sk_common.skc_state) != TCP_SYN_SENT) {
        u64 delta = bpf_ktime_get_ns() - *tsp;

        struct event *e = bpf_ringbuf_reserve(&rb, sizeof(*e), 0);
        if (e) {
            e->pid = bpf_get_current_pid_tgid() >> 32;
            e->saddr = BPF_CORE_READ(sk, __sk_common.skc_rcv_saddr);
            e->daddr = BPF_CORE_READ(sk, __sk_common.skc_daddr);
            e->sport = BPF_CORE_READ(sk, __sk_common.skc_num);
            e->dport = BPF_CORE_READ(sk, __sk_common.skc_dport);
            e->delta_us = delta / 1000;
            bpf_ringbuf_submit(e, 0);
        }
    }

    bpf_map_delete_elem(&start, &sk_addr);
    return 0;
}
```

---

## 5. 用户态性能分析

### 5.1 USDT探针

```c
// usdt_sample.bpf.c
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

char LICENSE[] SEC("license") = "GPL";

SEC("usdt//usr/bin/python:python:function__entry")
int BPF_USDT(python_function_entry, void *arg) {
    bpf_printk("Python function entered\n");
    return 0;
}

SEC("usdt//usr/lib/libc.so.6:libc:setjmp")
int BPF_USDT(libc_setjmp) {
    bpf_printk("setjmp called\n");
    return 0;
}
```

---

## 6. 性能分析最佳实践

### 6.1 开销控制

| 技术 | 开销 | 适用场景 |
|:-----|:-----|:---------|
| fentry/fexit | 极低 | 函数追踪 |
| kprobe/kretprobe | 低 | 内核函数 |
| tracepoint | 低 | 静态事件 |
| USDT | 中 | 用户态事件 |
| uprobe | 较高 | 用户态函数 |

### 6.2 采样策略

```c
// 使用BPF_MAP_TYPE_PERCPU_ARRAY减少竞争
// 批量上报减少系统调用
// 使用BPF_F_CURRENT_CPU指定CPU
```

---

## 关联导航

### 前置知识

- [eBPF深度](../../03_System_Technology_Domains/01_System_Programming/06_eBPF_Deep.md)
- [性能分析硬件](./README.md)
- [Linux Perf教程](./02_Linux_Perf_Tutorial.md)

### 后续延伸

- [Top-Down分析方法](./03_Top_Down_Analysis_Method.md)
- [微架构基准测试](./04_Microarchitecture_Benchmarking.md)

### 实践工具

- [bpftrace](https://github.com/iovisor/bpftrace)
- [BCC工具集](https://github.com/iovisor/bcc)
