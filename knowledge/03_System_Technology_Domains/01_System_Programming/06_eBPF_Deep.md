# eBPF深度解析与实战

> **层级定位**: 03 System Technology Domains / 01 System Programming
> **难度级别**: L5 专家
> **预估学习时间**: 12-15 小时

---

## 1. eBPF概述

eBPF (extended Berkeley Packet Filter) 是一种内核技术，允许用户态程序在内核中安全执行自定义代码。

### 1.1 演进历程

- **1992**: BSD Packet Filter (cBPF)
- **2014**: extended BPF (eBPF) - 64位、无限指令、Maps
- **2020+**: eBPF生态系统 - Cilium、Falco、BCC、CO-RE

### 1.2 架构

```
用户空间 C代码 → LLVM/Clang → BPF字节码 → 验证器 → JIT → 内核执行
     ↑                                                            │
     └──────────── Maps (数据交换) ←───────────────────────────────┘
```

---

## 2. eBPF程序类型

| 类型 | 用途 |
|:-----|:-----|
| XDP | 极高性能包处理（驱动层）|
| kprobe/kretprobe | 内核函数跟踪 |
| tracepoint | 静态跟踪点 |
| fentry/fexit | 函数入口/出口 |
| LSM | 安全钩子 |
| cgroup_skb | cgroup网络控制 |

---

## 3. BPF Maps

Maps是内核和用户空间通信机制：

```c
// Hash Map
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 1024);
    __type(key, u32);
    __type(value, u64);
} pid_count SEC(".maps");

// 使用
u32 pid = bpf_get_current_pid_tgid() >> 32;
u64 *count = bpf_map_lookup_elem(&pid_count, &pid);
```

---

## 4. BPF验证器

确保程序安全的检查：

- 无无限循环
- 内存访问边界检查
- 寄存器类型跟踪
- 只读访问ctx

---

## 5. 开发工具链

### BCC (Python接口)

```python
from bcc import BPF
prog = """
BPF_HASH(start, u32);
int do_entry(struct pt_regs *ctx) {
    u32 pid = bpf_get_current_pid_tgid();
    u64 ts = bpf_ktime_get_ns();
    start.update(&pid, &ts);
    return 0;
}
"""
b = BPF(text=prog)
b.attach_kprobe(event="sys_open", fn_name="do_entry")
```

### libbpf + CO-RE

```c
SEC("tp/syscalls/sys_enter_execve")
int trace_execve(struct trace_event_raw_sys_enter *ctx) {
    // CO-RE: 一次编译到处运行
    bpf_printk("execve called\n");
    return 0;
}
```

---

## 6. 实战案例

### 6.1 XDP防火墙

```c
SEC("xdp")
int xdp_firewall(struct xdp_md *ctx) {
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;

    struct ethhdr *eth = data;
    if ((void *)(eth + 1) > data_end)
        return XDP_DROP;

    // 阻止特定IP
    if (eth->h_proto == __constant_htons(ETH_P_IP)) {
        struct iphdr *ip = (void *)(eth + 1);
        if (ip->saddr == __constant_htonl(0x0A000001))
            return XDP_DROP;
    }
    return XDP_PASS;
}
```

### 6.2 系统调用跟踪

```c
SEC("tracepoint/raw_syscalls/sys_enter")
int trace_sys_enter(struct bpf_raw_tracepoint_args *ctx) {
    u32 pid = bpf_get_current_pid_tgid() >> 32;
    u64 *count, init_val = 1;

    count = bpf_map_lookup_elem(&sc_count, &pid);
    if (count)
        __sync_fetch_and_add(count, 1);
    else
        bpf_map_update_elem(&sc_count, &pid, &init_val, BPF_ANY);
    return 0;
}
```

---

## 关联导航

### 前置知识

- [系统调用](./04_System_Call_Interface.md)
- [Linux内核](../../04_Industrial_Scenarios/13_Linux_Kernel/README.md)
- [性能分析](../../02_Formal_Semantics_and_Physics/16_Performance_Analysis_Hardware/README.md)

### 后续延伸

- [网络编程](../15_Network_Programming/README.md)
- [Cilium容器网络](https://cilium.io)
- [Falco安全监控](https://falco.org)

### 工具

- [BCC工具集](https://github.com/iovisor/bcc)
- [bpftrace](https://github.com/iovisor/bpftrace)
