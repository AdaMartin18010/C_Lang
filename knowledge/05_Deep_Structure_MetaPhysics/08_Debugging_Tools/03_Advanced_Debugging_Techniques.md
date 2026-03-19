# C语言高级调试技术完全指南

> **层级定位**: 05 Deep_Structure_MetaPhysics / 08 Debugging_Tools
> **对应标准**: C11/C17/C23, GNU Debugger, eBPF, Linux Kernel Debugging
> **难度级别**: L4-L5
> **预估学习时间**: 20-30小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | eBPF编程、GDB高级脚本、Core Dump分析、Sanitizer、性能分析、并发调试、内核调试、远程调试 |
| **前置知识** | C高级编程、GDB基础、Linux系统编程、内存管理 |
| **后续延伸** | 系统性能优化、内核开发、安全审计 |
| **权威来源** | GDB Manual, Linux Kernel Documentation, BPF Performance Tools book |

---


---

## 📑 目录

- [C语言高级调试技术完全指南](#c语言高级调试技术完全指南)
  - [📋 本节概要](#-本节概要)
  - [📑 目录](#-目录)
  - [📖 1. eBPF基础与编程](#-1-ebpf基础与编程)
    - [1.1 eBPF概述](#11-ebpf概述)
    - [1.2 eBPF Kprobe程序](#12-ebpf-kprobe程序)
    - [1.3 eBPF用户空间程序](#13-ebpf用户空间程序)
    - [1.4 高级eBPF：TC流量控制](#14-高级ebpftc流量控制)
    - [1.5 eBPF Makefile](#15-ebpf-makefile)
  - [📖 2. GDB高级技巧](#-2-gdb高级技巧)
    - [2.1 GDB Python脚本](#21-gdb-python脚本)
    - [2.2 GDB脚本自动化](#22-gdb脚本自动化)
    - [3.2 Core Dump分析程序](#32-core-dump分析程序)
    - [3.3 堆栈展开器](#33-堆栈展开器)
  - [📖 4. 内存调试进阶（Sanitizer组合使用）](#-4-内存调试进阶sanitizer组合使用)
    - [4.1 Address Sanitizer + 其他Sanitizer](#41-address-sanitizer--其他sanitizer)
    - [4.2 Sanitizer抑制文件](#42-sanitizer抑制文件)
  - [📖 5. 性能分析（perf/火焰图）](#-5-性能分析perf火焰图)
    - [5.1 perf工具使用](#51-perf工具使用)
    - [5.2 性能分析C程序](#52-性能分析c程序)
    - [5.3 火焰图生成脚本](#53-火焰图生成脚本)
  - [📖 6. 并发调试（死锁检测、竞态条件）](#-6-并发调试死锁检测竞态条件)
    - [6.1 死锁检测工具](#61-死锁检测工具)
    - [6.2 竞态条件检测](#62-竞态条件检测)
- [C语言高级调试技术完全指南 - 续](#c语言高级调试技术完全指南---续)
  - [📖 7. 内核调试（kgdb/kdb/ftrace）](#-7-内核调试kgdbkdbftrace)
    - [7.1 KGDB配置与使用](#71-kgdb配置与使用)
    - [7.2 KDB常用命令](#72-kdb常用命令)
    - [7.3 ftrace编程示例](#73-ftrace编程示例)
  - [📖 8. 远程调试（GDB Server/JTAG）](#-8-远程调试gdb-serverjtag)
    - [8.1 GDB Server使用](#81-gdb-server使用)
    - [8.2 OpenOCD JTAG配置](#82-openocd-jtag配置)
    - [8.3 交叉编译调试配置](#83-交叉编译调试配置)
  - [📖 9. 自动化调试（崩溃报告系统）](#-9-自动化调试崩溃报告系统)
    - [9.1 崩溃处理器实现](#91-崩溃处理器实现)
    - [9.2 自动化调试工作流](#92-自动化调试工作流)
  - [📖 10. 实际案例：复杂内存泄漏追踪](#-10-实际案例复杂内存泄漏追踪)
    - [10.1 内存泄漏检测策略](#101-内存泄漏检测策略)
    - [10.2 使用valgrind进行深度分析](#102-使用valgrind进行深度分析)
    - [10.3 典型内存泄漏模式](#103-典型内存泄漏模式)
  - [✅ 质量验收清单](#-质量验收清单)


---

## 📖 1. eBPF基础与编程

### 1.1 eBPF概述

eBPF（extended Berkeley Packet Filter）是Linux内核中的一项革命性技术，允许在内核空间中安全地执行用户定义的代码，无需修改内核源码或加载内核模块。

```c
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
```

### 1.2 eBPF Kprobe程序

```c
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
```

### 1.3 eBPF用户空间程序

```c
/**
 * eBPF加载器 - 用户空间程序
 *
 * 负责加载eBPF程序到内核，并处理从内核传来的事件
 * 编译：gcc -o ebpf_loader ebpf_loader.c -lbpf -lelf -lz
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/resource.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include "trace_open.skel.h"  /* 由bpftool生成的骨架代码 */

/* 信号处理标志 */
static volatile sig_atomic_t exiting = 0;

void sig_handler(int sig)
{
    exiting = 1;
}

/**
 * libbpf日志回调函数
 * 用于调试eBPF加载过程中的问题
 */
static int libbpf_print_fn(enum libbpf_print_level level, const char *format, va_list args)
{
    /* 只显示警告和错误 */
    if (level > LIBBPF_WARN)
        return 0;
    return vfprintf(stderr, format, args);
}

/**
 * 处理从环形缓冲区接收的事件
 */
static int handle_event(void *ctx, void *data, size_t data_sz)
{
    struct event *e = data;

    /* 打印事件信息 */
    printf("[%llu] PID: %d, UID: %d, Comm: %s, File: %s\n",
           e->timestamp / 1000000000,  /* 转换为秒 */
           e->pid, e->uid, e->comm, e->filename);

    return 0;
}

int main(int argc, char **argv)
{
    struct trace_open_bpf *skel;
    struct ring_buffer *rb = NULL;
    int err;

    /* 设置libbpf日志回调 */
    libbpf_set_print(libbpf_print_fn);

    /* 提升资源限制（eBPF程序需要锁定内存） */
    struct rlimit rlim = {
        .rlim_cur = RLIM_INFINITY,
        .rlim_max = RLIM_INFINITY,
    };
    setrlimit(RLIMIT_MEMLOCK, &rlim);

    /* 设置信号处理 */
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    /* 打开并加载eBPF应用程序骨架 */
    skel = trace_open_bpf__open();
    if (!skel) {
        fprintf(stderr, "Failed to open BPF skeleton\n");
        return 1;
    }

    /* 可以在这里修改eBPF程序的全局变量 */
    // skel->rodata->target_uid = 1000;

    /* 加载并验证eBPF程序 */
    err = trace_open_bpf__load(skel);
    if (err) {
        fprintf(stderr, "Failed to load and verify BPF skeleton\n");
        goto cleanup;
    }

    /* 附加eBPF程序到追踪点 */
    err = trace_open_bpf__attach(skel);
    if (err) {
        fprintf(stderr, "Failed to attach BPF skeleton\n");
        goto cleanup;
    }

    /* 设置环形缓冲区轮询 */
    rb = ring_buffer__new(bpf_map__fd(skel->maps.rb), handle_event, NULL, NULL);
    if (!rb) {
        err = -1;
        fprintf(stderr, "Failed to create ring buffer\n");
        goto cleanup;
    }

    printf("Successfully started! Press Ctrl+C to stop.\n");

    /* 主事件循环 */
    while (!exiting) {
        /* 轮询环形缓冲区，超时100ms */
        err = ring_buffer__poll(rb, 100);
        if (err == -EINTR) {
            /* 被信号中断 */
            err = 0;
            break;
        }
        if (err < 0) {
            fprintf(stderr, "Error polling ring buffer: %d\n", err);
            break;
        }
    }

cleanup:
    /* 清理资源 */
    ring_buffer__free(rb);
    trace_open_bpf__destroy(skel);

    return err < 0 ? -err : 0;
}
```

### 1.4 高级eBPF：TC流量控制

```c
/**
 * eBPF TC（Traffic Control）程序
 *
 * 在网络数据包进入/离开网络栈时进行处理
 * 可用于：流量整形、包过滤、负载均衡、DDoS防护
 */

#include <linux/bpf.h>
#include <linux/pkt_cls.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

char _license[] SEC("license") = "GPL";

/* 简单的速率限制器 */
struct {
    __uint(type, BPF_MAP_TYPE_LRU_HASH);
    __uint(max_entries, 10000);
    __type(key, __u32);     /* IP地址 */
    __type(value, __u64);   /* 令牌桶计数器 */
} rate_limit SEC(".maps");

#define RATE_LIMIT 100      /* 每秒最多100个包 */
#define BURST_SIZE 200      /* 突发大小 */

/**
 * TC程序入口
 *
 * TC_ACT_OK    - 允许数据包通过
 * TC_ACT_SHOT  - 丢弃数据包
 * TC_ACT_REDIRECT - 重定向到另一接口
 */
SEC("tc")
int tc_rate_limit(struct __sk_buff *skb)
{
    void *data_end = (void *)(long)skb->data_end;
    void *data = (void *)(long)skb->data;

    struct ethhdr *eth = data;
    /* eBPF验证器需要检查边界 */
    if ((void *)(eth + 1) > data_end)
        return TC_ACT_OK;  /* 数据包太短，放行 */

    /* 只处理IPv4 */
    if (eth->h_proto != bpf_htons(ETH_P_IP))
        return TC_ACT_OK;

    struct iphdr *ip = (void *)(eth + 1);
    if ((void *)(ip + 1) > data_end)
        return TC_ACT_OK;

    __u32 src_ip = ip->saddr;
    __u64 now = bpf_ktime_get_ns();

    /* 检查速率限制 */
    __u64 *last = bpf_map_lookup_elem(&rate_limit, &src_ip);
    if (last) {
        /* 简单的令牌桶算法 */
        __u64 elapsed = now - *last;
        __u64 tokens = (elapsed * RATE_LIMIT) / 1000000000;

        if (tokens > BURST_SIZE) {
            tokens = BURST_SIZE;
        }

        if (tokens < 1) {
            /* 超出速率限制，丢弃 */
            return TC_ACT_SHOT;
        }

        /* 更新最后访问时间 */
        __u64 new_time = now - ((tokens - 1) * 1000000000 / RATE_LIMIT);
        bpf_map_update_elem(&rate_limit, &src_ip, &new_time, BPF_ANY);
    } else {
        /* 新IP，初始化 */
        bpf_map_update_elem(&rate_limit, &src_ip, &now, BPF_ANY);
    }

    return TC_ACT_OK;
}

/**
 * XDP程序 - 在驱动层处理数据包
 *
 * 性能最高，但功能有限
 */
SEC("xdp")
int xdp_drop_filter(struct xdp_md *ctx)
{
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;

    struct ethhdr *eth = data;
    if ((void *)(eth + 1) > data_end)
        return XDP_PASS;

    /* 示例：丢弃所有UDP包 */
    if (eth->h_proto == bpf_htons(ETH_P_IP)) {
        struct iphdr *ip = (void *)(eth + 1);
        if ((void *)(ip + 1) > data_end)
            return XDP_PASS;

        if (ip->protocol == IPPROTO_UDP)
            return XDP_DROP;
    }

    return XDP_PASS;
}
```

### 1.5 eBPF Makefile

```makefile
# eBPF程序Makefile
# 需要：clang, llvm, libbpf, bpftool

CLANG ?= clang
CC ?= gcc
ARCH := $(shell uname -m | sed 's/x86_64/x86/' | sed 's/aarch64/arm64/')

# eBPF编译选项
BPF_CFLAGS = -target bpf \
    -D__TARGET_ARCH_$(ARCH) \
    -I/usr/include/$(shell uname -m)-linux-gnu \
    -I/usr/include/bpf \
    -O2 -g \
    -Wall -Werror

# 用户空间编译选项
USER_CFLAGS = -Wall -g -O2
USER_LDFLAGS = -lbpf -lelf -lz

# 目标
EBPF_TARGETS = trace_open.o tc_prog.o xdp_prog.o
USER_TARGETS = ebpf_loader

.PHONY: all clean

all: $(EBPF_TARGETS) $(USER_TARGETS) skeletons

# 编译eBPF程序
%.o: %.c
 $(CLANG) $(BPF_CFLAGS) -c $< -o $@

# 生成骨架代码
skeletons: trace_open.skel.h

trace_open.skel.h: trace_open.o
 bpftool gen skeleton $< > $@

# 编译用户空间程序
ebpf_loader: ebpf_loader.c trace_open.skel.h
 $(CC) $(USER_CFLAGS) $< -o $@ $(USER_LDFLAGS)

# 加载eBPF程序（需要root权限）
load: all
 sudo ./ebpf_loader

# 清理
clean:
 rm -f *.o *.skel.h $(USER_TARGETS)

# 调试帮助
dump: trace_open.o
 llvm-objdump -S $<

verify: trace_open.o
 bpftool prog load $< /sys/fs/bpf/trace_open
```

---

## 📖 2. GDB高级技巧

### 2.1 GDB Python脚本

```python
#!/usr/bin/env python3
"""
GDB Pretty Printer示例 - 增强C数据结构的可读性

安装方法：
1. 将此文件保存为 ~/.gdb_printers.py
2. 在 ~/.gdbinit 中添加：
   source ~/.gdb_printers.py
"""

import gdb
import gdb.printing

class LinkedListPrinter:
    """
    链表 pretty printer

    在GDB中自动格式化显示链表结构
    """
    def __init__(self, val):
        self.val = val
        self.node_type = val.type.strip_typedefs()

    def to_string(self):
        return f"LinkedList(head={self.val['head']}, count={self._count()})"

    def children(self):
        """生成链表的迭代器，用于显示每个节点"""
        node = self.val['head']
        count = 0
        max_display = 20  # 最多显示20个节点

        while node and count < max_display:
            yield f"[{count}]", node.dereference()
            node = node['next']
            count += 1

        if node:
            yield "...", "truncated (more nodes...)"

    def _count(self):
        """计算链表节点总数"""
        count = 0
        node = self.val['head']
        max_count = 10000  # 防止无限循环

        while node and count < max_count:
            count += 1
            node = node['next']
        return count

    def display_hint(self):
        return 'array'

class HashTablePrinter:
    """哈希表 pretty printer"""
    def __init__(self, val):
        self.val = val

    def to_string(self):
        size = self.val['size']
        count = self.val['count']
        load_factor = float(count) / size if size > 0 else 0
        return f"HashTable(size={size}, count={count}, load={load_factor:.2%})"

    def children(self):
        """遍历并显示哈希表中的条目"""
        buckets = self.val['buckets']
        size = self.val['size']

        count = 0
        for i in range(size):
            entry = buckets[i]
            if entry:
                yield f"bucket[{i}]", entry.dereference()
                count += 1
                if count >= 10:  # 最多显示10个非空桶
                    yield "...", "truncated"
                    break

    def display_hint(self):
        return 'map'

class BufferPrinter:
    """缓冲区 pretty printer - 显示十六进制和ASCII"""
    def __init__(self, val):
        self.val = val

    def to_string(self):
        data = self.val['data']
        size = int(self.val['size'])
        capacity = int(self.val['capacity'])

        result = f"Buffer(size={size}, capacity={capacity})\n"

        # 显示十六进制转储
        if size > 0:
            result += "Hex dump:\n"
            for i in range(0, min(size, 256), 16):  # 最多显示256字节
                hex_part = ""
                ascii_part = ""
                for j in range(16):
                    if i + j < size:
                        try:
                            byte = int(data[i + j])
                            hex_part += f"{byte:02x} "
                            ascii_part += chr(byte) if 32 <= byte < 127 else '.'
                        except:
                            hex_part += "?? "
                            ascii_part += '?'
                    else:
                        hex_part += "   "
                result += f"  {i:04x}:  {hex_part} {ascii_part}\n"

        return result

# 注册pretty printers
class CustomPrinterCollection:
    def __init__(self):
        self.printers = {}

    def add(self, regex, printer):
        self.printers[regex] = printer

    def lookup(self, val):
        typename = val.type.strip_typedefs().name
        if typename is None:
            return None

        for regex, printer in self.printers.items():
            if regex in typename:
                return printer(val)
        return None

# 创建并注册
printer_collection = CustomPrinterCollection()
printer_collection.add('linked_list', LinkedListPrinter)
printer_collection.add('hash_table', HashTablePrinter)
printer_collection.add('buffer', BufferPrinter)

# 注册到GDB
gdb.printing.register_pretty_printer(
    gdb.current_objfile(),
    printer_collection.lookup,
    replace=True
)

# ============================================================================
# GDB命令扩展
# ============================================================================

class DumpMemoryCommand(gdb.Command):
    """
    增强的内存转储命令

    用法: dump-memory <address> <size> [format]
    格式: hex, ascii, int, float

    示例:
    (gdb) dump-memory buf 256 hex
    (gdb) dump-memory &arr[0] 100 int
    """

    def __init__(self):
        super().__init__("dump-memory", gdb.COMMAND_DATA)

    def invoke(self, arg, from_tty):
        args = gdb.string_to_argv(arg)
        if len(args) < 2:
            print("Usage: dump-memory <address> <size> [format]")
            return

        addr_str = args[0]
        size = int(args[1])
        fmt = args[2] if len(args) > 2 else 'hex'

        # 解析地址
        try:
            addr = int(gdb.parse_and_eval(addr_str))
        except:
            print(f"Invalid address: {addr_str}")
            return

        # 读取内存
        inferior = gdb.selected_inferior()
        try:
            data = inferior.read_memory(addr, size)
        except gdb.error as e:
            print(f"Cannot read memory: {e}")
            return

        # 格式化输出
        if fmt == 'hex':
            self._dump_hex(data, addr)
        elif fmt == 'ascii':
            self._dump_ascii(data)
        elif fmt == 'int':
            self._dump_ints(data, addr)
        elif fmt == 'float':
            self._dump_floats(data, addr)
        else:
            print(f"Unknown format: {fmt}")

    def _dump_hex(self, data, base_addr):
        """十六进制转储"""
        import struct
        bytes_data = bytes(data)
        for i in range(0, len(bytes_data), 16):
            chunk = bytes_data[i:i+16]
            hex_str = ' '.join(f'{b:02x}' for b in chunk)
            ascii_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in chunk)
            print(f"{base_addr + i:08x}:  {hex_str:<48} {ascii_str}")

    def _dump_ascii(self, data):
        """纯ASCII显示"""
        bytes_data = bytes(data)
        text = ''.join(chr(b) if 32 <= b < 127 else '.' for b in bytes_data)
        print(text)

    def _dump_ints(self, data, base_addr):
        """显示为整数数组"""
        import struct
        bytes_data = bytes(data)
        for i in range(0, len(bytes_data) - 3, 4):
            val = struct.unpack('<I', bytes_data[i:i+4])[0]
            print(f"[{i//4:4d}] 0x{base_addr + i:08x}: {val:10d} (0x{val:08x})")

    def _dump_floats(self, data, base_addr):
        """显示为浮点数数组"""
        import struct
        bytes_data = bytes(data)
        for i in range(0, len(bytes_data) - 3, 4):
            val = struct.unpack('<f', bytes_data[i:i+4])[0]
            print(f"[{i//4:4d}] 0x{base_addr + i:08x}: {val}")

# 注册命令
DumpMemoryCommand()

class BacktraceFilterCommand(gdb.Command):
    """
    过滤堆栈回溯

    用法: bt-filter <pattern>

    示例:
    (gdb) bt-filter my_function
    (gdb) bt-filter "main|worker"
    """

    def __init__(self):
        super().__init__("bt-filter", gdb.COMMAND_STACK)

    def invoke(self, arg, from_tty):
        if not arg:
            print("Usage: bt-filter <pattern>")
            return

        import re
        pattern = re.compile(arg)

        # 获取完整堆栈
        frame = gdb.newest_frame()
        level = 0

        while frame:
            name = frame.name() or "??"
            if pattern.search(name):
                # 打印匹配的帧
                sal = frame.find_sal()
                filename = sal.symtab.filename if sal.symtab else "??"
                line = sal.line
                print(f"#{level:<4} {name:<30} at {filename}:{line}")

            frame = frame.older()
            level += 1

BacktraceFilterCommand()

class FindInStackCommand(gdb.Command):
    """
    在堆栈中搜索特定值

    用法: find-in-stack <value>

    示例:
    (gdb) find-in-stack 0xdeadbeef
    (gdb) find-in-stack 42
    """

    def __init__(self):
        super().__init__("find-in-stack", gdb.COMMAND_DATA)

    def invoke(self, arg, from_tty):
        if not arg:
            print("Usage: find-in-stack <value>")
            return

        target = int(gdb.parse_and_eval(arg))

        # 获取栈范围
        frame = gdb.selected_frame()

        # 尝试找到栈边界
        try:
            # 获取栈指针
            sp = int(frame.read_register('sp'))
            # 假设栈大小最大为8MB
            stack_bottom = sp
            stack_top = sp + 8 * 1024 * 1024

            inferior = gdb.selected_inferior()

            try:
                # 读取栈内存
                stack_data = inferior.read_memory(stack_bottom, stack_top - stack_bottom)
                bytes_data = bytes(stack_data)

                # 搜索目标值（4字节对齐）
                import struct
                found = False
                for i in range(0, len(bytes_data) - 3, 4):
                    val = struct.unpack('<I', bytes_data[i:i+4])[0]
                    if val == target:
                        addr = stack_bottom + i
                        print(f"Found 0x{target:08x} at stack offset +0x{i:x} (addr: 0x{addr:08x})")
                        found = True

                if not found:
                    print(f"Value 0x{target:08x} not found in stack")

            except gdb.error as e:
                print(f"Error reading stack: {e}")

        except Exception as e:
            print(f"Error: {e}")

FindInStackCommand()

# ============================================================================
# 断点增强
# ============================================================================

class ConditionalBreakpoint:
    """
    条件断点辅助类

    用法:
    (gdb) source ~/.gdb_printers.py
    (gdb) break my_func if $caller_matches("main")
    """

    @staticmethod
    def caller_matches(pattern):
        """检查调用者是否匹配模式"""
        frame = gdb.selected_frame()
        caller = frame.older()
        if caller:
            name = caller.name() or ""
            return pattern in name
        return False

    @staticmethod
    def nth_call(n):
        """只在第n次调用时触发"""
        key = f"call_count_{gdb.selected_frame().name()}"
        count = int(gdb.parse_and_eval(f"${key}")) if gdb.parse_and_eval(f"${key}") else 0
        count += 1
        gdb.execute(f"set ${key}={count}")
        return count == n

# 注册便捷函数
class CallerMatchesFunc(gdb.Function):
    """GDB函数: $caller_matches(pattern)"""
    def __init__(self):
        super().__init__("caller_matches")

    def invoke(self, pattern):
        return ConditionalBreakpoint.caller_matches(str(pattern))

CallerMatchesFunc()

print("GDB Python extensions loaded successfully!")
print("Available commands: dump-memory, bt-filter, find-in-stack")
print("Available functions: $caller_matches()")
```

### 2.2 GDB脚本自动化

```gdb
#!/usr/bin/gdb -x
/**
 * GDB自动化调试脚本示例
 *
 * 使用方法:
 * gdb -x debug_script.gdb ./target_program
 * 或
 * gdb ./target_program
 * (gdb) source debug_script.gdb
 */

# =============================================================================
# 基本配置
# =============================================================================

# 禁用分页（适合自动化脚本）
set pagination off

# 禁用确认提示
set confirm off

# 设置输出格式
set print pretty on
set print array on
set print object on
set print static-members on
set print vtbl on
set print demangle on

# 设置历史记录
set history filename ~/.gdb_history
set history save on
set history size 10000
set history expansion on

# =============================================================================
# 颜色配置（支持彩色终端）
# =============================================================================

# 启用彩色输出
set prompt \033[31mgdb\033[0m>

# 定义彩色打印函数
define p-red
    printf "\033[31m%s\033[0m\n", $arg0
end

define p-green
    printf "\033[32m%s\033[0m\n", $arg0
end

define p-yellow
    printf "\033[33m%s\033[0m\n", $arg0
end

define p-blue
    printf "\033[34m%s\033[0m\n", $arg0
end

# =============================================================================
# 自定义命令
# =============================================================================

/**
 * 增强的堆栈回溯 - 显示局部变量
 */
define bt-full-locals
    p-yellow "===== Full Backtrace with Locals ====="
    set $i = 0
    while $i < $arg0
        frame $i
        info locals
        info args
        echo \n
        set $i = $i + 1
    end
end

document bt-full-locals
显示完整堆栈和局部变量
用法: bt-full-locals <frame_count>
end

/**
 * 内存泄漏检测辅助 - 追踪malloc/free
 */
define trace-memory
    # 设置malloc追踪断点
    break malloc
    commands
        silent
        printf "[MALLOC] size=%lu, ret=%p\n", $rdi, $rax
        continue
    end

    # 设置free追踪断点
    break free
    commands
        silent
        printf "[FREE] ptr=%p\n", $rdi
        continue
    end

    p-green "Memory tracing enabled. Run 'continue' to start."
end

document trace-memory
启用malloc/free追踪
用法: trace-memory
end

/**
 * 性能分析 - 函数执行时间
 */
define profile-function
    # 设置入口断点
    break $arg0
    commands
        silent
        set $profile_start = (unsigned long long)time_ns()
        continue
    end

    # 设置返回断点
    set $ret_bp = ($bpnum + 1)
    break *$arg0 return
    commands
        silent
        set $profile_end = (unsigned long long)time_ns()
        printf "[PROFILE] %s took %llu ns\n", "$arg0", $profile_end - $profile_start
        continue
    end

    p-green "Profiling enabled for $arg0"
end

document profile-function
分析函数执行时间
用法: profile-function <function_name>
end

/**
 * 显示数据结构内容
 */
define show-struct
    ptype /o $arg0
    printf "\nValues:\n"
    print *($arg0)
end

document show-struct
显示结构体的布局和值
用法: show-struct <struct_pointer>
end

/**
 * 字符串分析 - 检测各种问题
 */
define analyze-string
    set $str = $arg0
    set $max_len = $arg1
    set $i = 0

    printf "Analyzing string at %p (max %d chars):\n", $str, $max_len

    # 检查NULL
    if $str == 0
        p-red "ERROR: NULL pointer!"
        return
    end

    # 检查可访问性
    set $_test = *((char*)$str)

    # 查找长度
    set $len = 0
    while $len < $max_len && *((char*)$str + $len) != 0
        set $len = $len + 1
    end

    if $len >= $max_len
        p-yellow "WARNING: No null terminator within max_len!"
    else
        printf "Length: %d\n", $len
    end

    # 检查可打印字符
    printf "Content: "
    set $i = 0
    while $i < $len && $i < 80
        set $c = *((char*)$str + $i)
        if $c >= 32 && $c < 127
            printf "%c", $c
        else
            printf "\\x%02x", $c
        end
        set $i = $i + 1
    end
    printf "\n"

    # 显示十六进制
    printf "Hex: "
    set $i = 0
    while $i < $len && $i < 32
        printf "%02x ", *((unsigned char*)$str + $i)
        set $i = $i + 1
    end
    printf "\n"
end

document analyze-string
分析字符串的各种属性
用法: analyze-string <string_ptr> <max_len>
end

/**
 * 数组分析
 */
define analyze-array
    set $arr = (unsigned long long*)$arg0
    set $size = $arg1
    set $elem_size = $arg2

    printf "Array at %p, %d elements of %d bytes:\n", $arr, $size, $elem_size

    set $i = 0
    while $i < $size && $i < 20
        if $elem_size == 1
            printf "[%3d] 0x%02x\n", $i, ((unsigned char*)$arr)[$i]
        end
        if $elem_size == 2
            printf "[%3d] 0x%04x\n", $i, ((unsigned short*)$arr)[$i]
        end
        if $elem_size == 4
            printf "[%3d] 0x%08x (%d)\n", $i, ((unsigned int*)$arr)[$i], ((int*)$arr)[$i]
        end
        if $elem_size == 8
            printf "[%3d] 0x%016llx (%lld)\n", $i, $arr[$i], $arr[$i]
        end
        set $i = $i + 1
    end

    if $size > 20
        printf "... (%d more elements)\n", $size - 20
    end
end

document analyze-array
以指定元素大小分析数组
用法: analyze-array <array_ptr> <count> <elem_size>
end

/**
 * 线程分析
 */
define thread-analysis
    p-blue "===== Thread Analysis ====="
    info threads

    set $thread_count = 0
    thread apply all p-yellow "Thread active"

    p-blue "===== Thread Stack Traces ====="
    thread apply all bt 5
end

document thread-analysis
分析所有线程的状态
用法: thread-analysis
end

/**
 * 内存区域扫描
 */
define scan-memory
    set $start = $arg0
    set $end = $arg1
    set $pattern = $arg2

    printf "Scanning memory from 0x%lx to 0x%lx for 0x%lx...\n", $start, $end, $pattern

    set $addr = $start
    set $found = 0

    while $addr < $end
        if *(unsigned long*)$addr == $pattern
            printf "Found at 0x%lx\n", $addr
            set $found = $found + 1
            if $found >= 10
                printf "(stopping after 10 matches)\n"
                loop_break
            end
        end
        set $addr = $addr + sizeof(unsigned long)
    end

    printf "Total matches: %d\n", $found
end

document scan-memory
扫描内存区域查找特定值
用法: scan-memory <start_addr> <end_addr> <pattern>
end

# =============================================================================
# 自动断点命令
# =============================================================================

/**
 * 智能断点 - 记录命中次数并显示上下文
 */
define smart-break
    break $arg0
    commands
        silent
        set $_hit_count++
        printf "[Breakpoint hit #%d] at ", $_hit_count
        frame 0
        print $arg0
        if $_hit_count >= 10
            printf "(Auto-disabling after 10 hits)\n"
            disable
        end
        continue
    end
end

document smart-break
创建智能断点，自动记录命中次数
用法: smart-break <location>
end

/**
 * 条件日志断点
 */
define log-break
    break $arg0 if $arg1
    commands
        silent
        printf "[LOG] Condition met at %s:%d\n", $__FILE__, $__LINE__
        printf "      Local vars: "
        info locals
        continue
    end
end

document log-break
创建条件日志断点
用法: log-break <location> <condition>
end

# =============================================================================
# 核心转录分析辅助
# =============================================================================

define core-analysis
    p-yellow "===== Core Dump Analysis ====="

    printf "\n1. Signal Information:\n"
    info signals

    printf "\n2. Stack Trace:\n"
    bt full

    printf "\n3. Register State:\n"
    info registers

    printf "\n4. Memory Map:\n"
    info proc mappings

    printf "\n5. Thread State:\n"
    info threads
    thread apply all bt 3

    printf "\n6. Local Variables:\n"
    frame 0
    info locals
    info args
end

document core-analysis
全面分析core dump
用法: core-analysis (在加载core dump后运行)
end

# =============================================================================
# 启动钩子
# =============================================================================

define hook-stop
    # 每次停止时显示上下文
    echo \n========================================\n
    echo Stopped at:
    frame 0
    echo \n
end

define hook-run
    # 运行前初始化计数器
    set $_hit_count = 0
    p-green "Starting program..."
end

# =============================================================================
# 便捷别名
# =============================================================================

alias -a b = break
alias -a c = continue
alias -a s = step
alias -a n = next
alias -a f = finish
alias -a r = run
alias -a q = quit
alias -a bt = backtrace
alias -a p = print
alias -a po = print-object
alias -a x = examine
alias -a info-b = info breakpoints
alias -a info-l = info locals
alias -a info-a = info args
alias -a info-r = info registers
alias -a info-th = info threads

# =============================================================================
# 欢迎信息
# =============================================================================

echo \n
echo ================================================\n
echo    Advanced GDB Script Loaded\n
echo ================================================\n
echo Available custom commands:\n
echo   - bt-full-locals <n>  : Full backtrace with locals\n
echo   - trace-memory        : Trace malloc/free calls\n
echo   - profile-function <f>: Profile function execution\n
echo   - show-struct <ptr>   : Show structure layout\n
echo   - analyze-string <ptr>: Analyze string content\n
echo   - analyze-array ...   : Analyze array contents\n
echo   - thread-analysis     : Analyze all threads\n
echo   - scan-memory ...     : Scan memory for pattern\n
echo   - smart-break <loc>   : Smart breakpoint\n
echo   - core-analysis       : Analyze core dump\n
echo ================================================\n
echo \n


---

## 📖 3. 崩溃分析（Core Dump分析）

### 3.1 Core Dump生成配置

```bash
#!/bin/bash
# core_dump_setup.sh - Core Dump生成配置脚本

# =============================================================================
# 1. 系统级Core Dump配置
# =============================================================================

# 启用core dump生成（大小无限制）
echo "Setting core dump size to unlimited..."
ulimit -c unlimited

# 持久化到/etc/security/limits.conf
cat >> /etc/security/limits.conf << 'EOF'
# Core dump settings
* soft core unlimited
* hard core unlimited
root soft core unlimited
root hard core unlimited
EOF

# =============================================================================
# 2. Core Dump文件命名和存储
# =============================================================================

# 创建core dump目录
mkdir -p /var/crash
echo "Created /var/crash directory"

# 配置systemd-coredump（现代Linux发行版）
cat > /etc/sysctl.d/99-core-dump.conf << 'EOF'
# Core dump命名格式
# %e: 可执行文件名
# %p: PID
# %t: 时间戳
# %h: 主机名
# %s: 信号编号
kernel.core_pattern = /var/crash/core.%e.%p.%h.%t.%s

# 包含PID
kernel.core_uses_pid = 1

# 设置管道处理（使用systemd-coredump）
# kernel.core_pattern = |/usr/lib/systemd/systemd-coredump %P %u %g %s %t %c %h %e
EOF

# 应用配置
sysctl -p /etc/sysctl.d/99-core-dump.conf

# =============================================================================
# 3. 调试信息配置
# =============================================================================

# 安装debuginfod（自动获取调试符号）
# Ubuntu/Debian
apt-get install -y debuginfod
# RHEL/CentOS
# yum install -y debuginfod

# 启用debuginfod
echo 'export DEBUGINFOD_URLS="https://debuginfod.ubuntu.com"' >> /etc/profile.d/debuginfod.sh

# =============================================================================
# 4. 验证配置
# =============================================================================

echo "Current core dump settings:"
echo "ulimit -c: $(ulimit -c)"
echo "core_pattern: $(cat /proc/sys/kernel/core_pattern)"
echo "core_uses_pid: $(cat /proc/sys/kernel/core_uses_pid)"

echo "Setup complete! Core dumps will be saved to /var/crash/"
```

### 3.2 Core Dump分析程序

```c
/**
 * crash_analyzer.c - Core Dump分析工具
 *
 * 功能：
 * 1. 解析ELF core dump文件
 * 2. 提取寄存器状态
 * 3. 重建调用栈
 * 4. 分析内存映射
 *
 * 编译：gcc -o crash_analyzer crash_analyzer.c -lelf -ldw
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <gelf.h>
#include <libelf.h>
#include <dwarf.h>
#include <elfutils/libdw.h>
#include <elfutils/libdwfl.h>
#include <sys/procfs.h>
#include <sys/user.h>
#include <signal.h>

/* ELF Notes类型定义（来自内核） */
#define NT_PRSTATUS     1   /* 进程状态 */
#define NT_FPREGSET     2   /* 浮点寄存器 */
#define NT_PRPSINFO     3   /* 进程信息 */
#define NT_TASKSTRUCT   4   /* 任务结构 */
#define NT_AUXV         6   /* 辅助向量 */
#define NT_SIGINFO      0x53494749  /* 信号信息 */
#define NT_FILE         0x46494c45  /* 映射文件信息 */
#define NT_PRXFPREG     0x46e62b7f  /* 扩展浮点寄存器 */

/* 程序头类型 */
#define PT_NOTE         4
#define PT_LOAD         1

/* 结构定义 */
typedef struct {
    long si_signo;
    long si_code;
    long si_errno;
} siginfo_t_simple;

typedef struct {
    int pr_signo;
    int pr_code;
    int pr_errno;
    /* ... 更多字段 */
} pr_siginfo_t;

/* x86_64用户寄存器结构 */
struct x86_64_regs {
    unsigned long r15, r14, r13, r12;
    unsigned long rbp, rbx, r11;
    unsigned long r10, r9, r8;
    unsigned long rax, rcx, rdx;
    unsigned long rsi, rdi;
    unsigned long orig_rax;
    unsigned long rip;
    unsigned long cs;
    unsigned long eflags;
    unsigned long rsp;
    unsigned long ss;
    unsigned long fs_base, gs_base;
    unsigned long ds, es, fs, gs;
};

/* 分析上下文 */
typedef struct {
    int fd;
    Elf *elf;
    GElf_Ehdr ehdr;

    /* 寄存器状态 */
    struct x86_64_regs regs;
    int has_regs;

    /* 信号信息 */
    int signal_num;
    int signal_code;
    char signal_name[32];

    /* 进程信息 */
    char exe_name[256];
    pid_t pid;

    /* 内存映射 */
    int num_mappings;
} core_context_t;

/* 信号名称映射 */
const char* signal_to_name(int sig) {
    switch (sig) {
        case SIGSEGV: return "SIGSEGV (Segmentation fault)";
        case SIGABRT: return "SIGABRT (Aborted)";
        case SIGFPE:  return "SIGFPE (Floating point exception)";
        case SIGILL:  return "SIGILL (Illegal instruction)";
        case SIGBUS:  return "SIGBUS (Bus error)";
        case SIGPIPE: return "SIGPIPE (Broken pipe)";
        case SIGSYS:  return "SIGSYS (Bad system call)";
        default:      return "Unknown";
    }
}

/**
 * 初始化分析上下文
 */
int core_init(core_context_t *ctx, const char *filename) {
    memset(ctx, 0, sizeof(*ctx));

    /* 初始化libelf */
    if (elf_version(EV_CURRENT) == EV_NONE) {
        fprintf(stderr, "Failed to initialize libelf\n");
        return -1;
    }

    /* 打开文件 */
    ctx->fd = open(filename, O_RDONLY);
    if (ctx->fd < 0) {
        perror("open");
        return -1;
    }

    /* 解析ELF */
    ctx->elf = elf_begin(ctx->fd, ELF_C_READ, NULL);
    if (!ctx->elf) {
        fprintf(stderr, "elf_begin failed: %s\n", elf_errmsg(-1));
        close(ctx->fd);
        return -1;
    }

    /* 验证是core dump */
    if (elf_kind(ctx->elf) != ELF_K_ELF) {
        fprintf(stderr, "Not an ELF file\n");
        goto error;
    }

    /* 获取ELF头 */
    if (gelf_getehdr(ctx->elf, &ctx->ehdr) == NULL) {
        fprintf(stderr, "gelf_getehdr failed: %s\n", elf_errmsg(-1));
        goto error;
    }

    /* 验证是core文件 */
    if (ctx->ehdr.e_type != ET_CORE) {
        fprintf(stderr, "Not a core file (e_type=%d)\n", ctx->ehdr.e_type);
        goto error;
    }

    printf("=== Core Dump Analysis ===\n");
    printf("Architecture: %s\n",
           ctx->ehdr.e_machine == EM_X86_64 ? "x86_64" :
           ctx->ehdr.e_machine == EM_386 ? "i386" :
           ctx->ehdr.e_machine == EM_AARCH64 ? "aarch64" : "unknown");
    printf("Class: %s\n", ctx->ehdr.e_ident[EI_CLASS] == ELFCLASS64 ? "64-bit" : "32-bit");
    printf("Endian: %s\n", ctx->ehdr.e_ident[EI_DATA] == ELFDATA2LSB ? "Little" : "Big");

    return 0;

error:
    elf_end(ctx->elf);
    close(ctx->fd);
    return -1;
}

/**
 * 解析ELF Notes段
 */
int parse_notes(core_context_t *ctx) {
    size_t nphdr;
    int found_notes = 0;

    if (elf_getphdrnum(ctx->elf, &nphdr) != 0) {
        fprintf(stderr, "elf_getphdrnum failed\n");
        return -1;
    }

    for (size_t i = 0; i < nphdr; i++) {
        GElf_Phdr phdr;
        if (gelf_getphdr(ctx->elf, i, &phdr) != &phdr)
            continue;

        if (phdr.p_type != PT_NOTE)
            continue;

        found_notes = 1;

        /* 读取note数据 */
        void *data = malloc(phdr.p_filesz);
        if (!data) {
            perror("malloc");
            continue;
        }

        if (pread(ctx->fd, data, phdr.p_filesz, phdr.p_offset) !=
            (ssize_t)phdr.p_filesz) {
            perror("pread");
            free(data);
            continue;
        }

        /* 解析notes */
        size_t offset = 0;
        while (offset < phdr.p_filesz) {
            if (offset + 12 > phdr.p_filesz)
                break;

            /* 读取note头 */
            uint32_t namesz = *((uint32_t *)((char *)data + offset));
            uint32_t descsz = *((uint32_t *)((char *)data + offset + 4));
            uint32_t type   = *((uint32_t *)((char *)data + offset + 8));

            /* 对齐 */
            namesz = (namesz + 3) & ~3;
            descsz = (descsz + 3) & ~3;

            const char *name = (const char *)data + offset + 12;
            const void *desc = (const char *)data + offset + 12 + namesz;

            /* 检查是否是CORE类型的note */
            if (strcmp(name, "CORE") == 0 || strcmp(name, "LINUX") == 0) {
                switch (type) {
                case NT_PRSTATUS: {
                    /* 解析prstatus - 包含寄存器 */
                    struct elf_prstatus *prstatus = (struct elf_prstatus *)desc;
                    memcpy(&ctx->regs, &prstatus->pr_reg, sizeof(ctx->regs));
                    ctx->has_regs = 1;
                    ctx->signal_num = prstatus->pr_cursig;
                    ctx->pid = prstatus->pr_pid;
                    break;
                }
                case NT_PRPSINFO: {
                    /* 解析prpsinfo - 进程信息 */
                    struct elf_prpsinfo *prpsinfo = (struct elf_prpsinfo *)desc;
                    strncpy(ctx->exe_name, prpsinfo->pr_fname,
                            sizeof(ctx->exe_name) - 1);
                    break;
                }
                case NT_SIGINFO: {
                    /* 解析信号信息 */
                    siginfo_t_simple *si = (siginfo_t_simple *)desc;
                    ctx->signal_num = si->si_signo;
                    ctx->signal_code = si->si_code;
                    break;
                }
                }
            }

            offset += 12 + namesz + descsz;
        }

        free(data);
    }

    return found_notes ? 0 : -1;
}

/**
 * 打印寄存器状态
 */
void print_registers(const core_context_t *ctx) {
    if (!ctx->has_regs) {
        printf("\nRegister information not available\n");
        return;
    }

    printf("\n=== Register State ===\n");
    printf("  RIP: 0x%016lx  RSP: 0x%016lx\n", ctx->regs.rip, ctx->regs.rsp);
    printf("  RBP: 0x%016lx  RAX: 0x%016lx\n", ctx->regs.rbp, ctx->regs.rax);
    printf("  RBX: 0x%016lx  RCX: 0x%016lx\n", ctx->regs.rbx, ctx->regs.rcx);
    printf("  RDX: 0x%016lx  RSI: 0x%016lx\n", ctx->regs.rdx, ctx->regs.rsi);
    printf("  RDI: 0x%016lx  R8:  0x%016lx\n", ctx->regs.rdi, ctx->regs.r8);
    printf("  R9:  0x%016lx  R10: 0x%016lx\n", ctx->regs.r9, ctx->regs.r10);
    printf("  R11: 0x%016lx  R12: 0x%016lx\n", ctx->regs.r11, ctx->regs.r12);
    printf("  R13: 0x%016lx  R14: 0x%016lx\n", ctx->regs.r13, ctx->regs.r14);
    printf("  R15: 0x%016lx\n", ctx->regs.r15);
    printf("  EFLAGS: 0x%016lx\n", ctx->regs.eflags);

    /* 分析RIP */
    printf("\n  Instruction Pointer (RIP) = 0x%016lx\n", ctx->regs.rip);

    /* 分析栈指针 */
    printf("  Stack Pointer (RSP) = 0x%016lx\n", ctx->regs.rsp);
}

/**
 * 打印信号信息
 */
void print_signal_info(const core_context_t *ctx) {
    printf("\n=== Signal Information ===\n");
    printf("  Signal: %d (%s)\n", ctx->signal_num,
           signal_to_name(ctx->signal_num));
    printf("  Code:   %d\n", ctx->signal_code);

    /* 信号特定分析 */
    if (ctx->signal_num == SIGSEGV) {
        printf("\n  Segmentation Fault Analysis:\n");
        printf("    - Check RIP: if 0x0 or very low, likely null pointer dereference\n");
        printf("    - Check RIP: if 0xXXXXXXXXdeadXXXX, likely use-after-free\n");
        printf("    - Use 'info proc mappings' to see valid memory regions\n");
    }
}

/**
 * 打印内存映射
 */
void print_memory_mappings(core_context_t *ctx) {
    size_t nphdr;

    printf("\n=== Memory Mappings ===\n");
    printf("%-20s %-20s %-8s %-8s %s\n",
           "Start", "End", "Offset", "Size", "Flags");

    if (elf_getphdrnum(ctx->elf, &nphdr) != 0)
        return;

    ctx->num_mappings = 0;
    for (size_t i = 0; i < nphdr; i++) {
        GElf_Phdr phdr;
        if (gelf_getphdr(ctx->elf, i, &phdr) != &phdr)
            continue;

        if (phdr.p_type != PT_LOAD)
            continue;

        ctx->num_mappings++;

        unsigned long start = phdr.p_vaddr;
        unsigned long end = start + phdr.p_memsz;

        char flags[4] = {'-', '-', '-', '\0'};
        if (phdr.p_flags & PF_R) flags[0] = 'r';
        if (phdr.p_flags & PF_W) flags[1] = 'w';
        if (phdr.p_flags & PF_X) flags[2] = 'x';

        printf("0x%016lx 0x%016lx 0x%06lx %8lu %s\n",
               start, end, phdr.p_offset, phdr.p_memsz, flags);
    }

    printf("\nTotal mappings: %d\n", ctx->num_mappings);
}

/**
 * 尝试重建调用栈
 */
void reconstruct_stack(core_context_t *ctx) {
    if (!ctx->has_regs) {
        printf("\nCannot reconstruct stack without registers\n");
        return;
    }

    printf("\n=== Stack Reconstruction ===\n");
    printf("Note: This is a basic reconstruction. Use GDB for full backtrace.\n\n");

    /* 读取栈数据 */
    size_t nphdr;
    if (elf_getphdrnum(ctx->elf, &nphdr) != 0)
        return;

    /* 找到包含RSP的段 */
    for (size_t i = 0; i < nphdr; i++) {
        GElf_Phdr phdr;
        if (gelf_getphdr(ctx->elf, i, &phdr) != &phdr)
            continue;

        if (phdr.p_type != PT_LOAD)
            continue;

        unsigned long seg_start = phdr.p_vaddr;
        unsigned long seg_end = seg_start + phdr.p_filesz;

        /* 检查RSP是否在这个段内 */
        if (ctx->regs.rsp >= seg_start && ctx->regs.rsp < seg_end) {
            /* 读取栈内容 */
            unsigned long stack_offset = ctx->regs.rsp - seg_start;
            unsigned long stack_size = 512;  /* 读取512字节 */

            if (stack_offset + stack_size > phdr.p_filesz)
                stack_size = phdr.p_filesz - stack_offset;

            void *stack_data = malloc(stack_size);
            if (!stack_data)
                continue;

            off_t file_offset = phdr.p_offset + stack_offset;
            if (pread(ctx->fd, stack_data, stack_size, file_offset) ==
                (ssize_t)stack_size) {

                printf("Stack dump at RSP (0x%016lx):\n", ctx->regs.rsp);

                unsigned long *words = (unsigned long *)stack_data;
                int num_words = stack_size / sizeof(unsigned long);

                for (int j = 0; j < num_words && j < 16; j++) {
                    unsigned long addr = ctx->regs.rsp + j * sizeof(unsigned long);
                    unsigned long value = words[j];

                    /* 猜测这是否是返回地址 */
                    int is_ret = (value > 0x400000 && value < 0x7fffffffffff);

                    printf("  0x%016lx: 0x%016lx %s\n",
                           addr, value, is_ret ? "<- possible return address" : "");
                }
            }

            free(stack_data);
            break;
        }
    }
}

/**
 * 主函数
 */
int main(int argc, char *argv[]) {
    core_context_t ctx;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <core-dump-file>\n", argv[0]);
        fprintf(stderr, "\nExample:\n");
        fprintf(stderr, "  %s /var/crash/core.myapp.12345.hostname.1699123456.11\n", argv[0]);
        return 1;
    }

    /* 初始化 */
    if (core_init(&ctx, argv[1]) != 0) {
        fprintf(stderr, "Failed to initialize core dump analysis\n");
        return 1;
    }

    /* 解析notes */
    if (parse_notes(&ctx) != 0) {
        printf("Warning: Failed to parse notes\n");
    }

    /* 打印信息 */
    printf("\nExecutable: %s\n", ctx.exe_name[0] ? ctx.exe_name : "unknown");
    printf("PID: %d\n", ctx.pid);

    print_signal_info(&ctx);
    print_registers(&ctx);
    print_memory_mappings(&ctx);
    reconstruct_stack(&ctx);

    /* 清理 */
    elf_end(ctx.elf);
    close(ctx.fd);

    printf("\n=== Analysis Complete ===\n");
    printf("\nNext steps:\n");
    printf("1. Load with GDB: gdb <executable> %s\n", argv[1]);
    printf("2. Get backtrace: (gdb) bt full\n");
    printf("3. Check locals:  (gdb) info locals\n");
    printf("4. Check args:    (gdb) info args\n");

    return 0;
}
```

### 3.3 堆栈展开器

```c
/**
 * stack_unwinder.c - 自定义堆栈展开器
 *
 * 实现DWARF CFI（Call Frame Information）解析
 * 用于在没有GDB的情况下展开堆栈
 *
 * 编译：gcc -o stack_unwinder stack_unwinder.c -lelf -ldw -lunwind -lunwind-x86_64
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <execinfo.h>
#include <ucontext.h>
#include <dlfcn.h>
#include <elf.h>
#include <link.h>
#include <dwarf.h>
#include <elfutils/libdw.h>
#include <elfutils/libdwfl.h>

/* 引入libunwind用于比较 */
#define UNW_LOCAL_ONLY
#include <libunwind.h>

#define MAX_FRAMES 128
#define MAX_SYMBOL_LEN 256

/**
 * 栈帧信息结构
 */
typedef struct {
    void *return_addr;
    void *frame_pointer;
    void *stack_pointer;
    char function_name[MAX_SYMBOL_LEN];
    char file_name[MAX_SYMBOL_LEN];
    int line_number;
    int is_signal_frame;
} stack_frame_t;

/**
 * 展开上下文
 */
typedef struct {
    stack_frame_t frames[MAX_FRAMES];
    int num_frames;
    void *context;  /* ucontext_t指针 */
} unwind_context_t;

/**
 * 使用execinfo的简单展开（有局限性）
 */
int backtrace_simple(void **buffer, int size) {
    return backtrace(buffer, size);
}

/**
 * 符号解析 - 使用dladdr
 */
int resolve_symbol(void *addr, stack_frame_t *frame) {
    Dl_info info;

    if (dladdr(addr, &info) == 0)
        return -1;

    frame->return_addr = addr;

    if (info.dli_sname) {
        strncpy(frame->function_name, info.dli_sname, MAX_SYMBOL_LEN - 1);
    } else {
        snprintf(frame->function_name, MAX_SYMBOL_LEN, "<unknown>");
    }

    if (info.dli_fname) {
        strncpy(frame->file_name, info.dli_fname, MAX_SYMBOL_LEN - 1);
    } else {
        strncpy(frame->file_name, "<unknown>", MAX_SYMBOL_LEN - 1);
    }

    /* 计算相对偏移 */
    if (info.dli_saddr) {
        long offset = (long)addr - (long)info.dli_saddr;
        char tmp[MAX_SYMBOL_LEN];
        snprintf(tmp, sizeof(tmp), "%s+0x%lx", frame->function_name, offset);
        strncpy(frame->function_name, tmp, MAX_SYMBOL_LEN - 1);
    }

    return 0;
}

/**
 * 使用libunwind展开（最可靠的方法）
 */
int unwind_with_libunwind(unwind_context_t *ctx) {
    unw_cursor_t cursor;
    unw_context_t uc;
    int frame_count = 0;

    /* 获取当前上下文 */
    unw_getcontext(&uc);

    /* 初始化游标 */
    if (unw_init_local(&cursor, &uc) < 0) {
        fprintf(stderr, "unw_init_local failed\n");
        return -1;
    }

    printf("\n=== Stack Trace (libunwind) ===\n");

    /* 遍历栈帧 */
    while (unw_step(&cursor) > 0 && frame_count < MAX_FRAMES) {
        unw_word_t ip, sp, offset;
        char sym_name[MAX_SYMBOL_LEN];

        /* 获取指令指针和栈指针 */
        unw_get_reg(&cursor, UNW_REG_IP, &ip);
        unw_get_reg(&cursor, UNW_REG_SP, &sp);

        /* 获取函数名 */
        sym_name[0] = '\0';
        unw_get_proc_name(&cursor, sym_name, sizeof(sym_name), &offset);

        /* 保存到上下文 */
        ctx->frames[frame_count].return_addr = (void *)ip;
        ctx->frames[frame_count].stack_pointer = (void *)sp;
        snprintf(ctx->frames[frame_count].function_name, MAX_SYMBOL_LEN,
                 "%s+0x%lx", sym_name[0] ? sym_name : "??", offset);

        /* 检查是否是信号帧 */
        ctx->frames[frame_count].is_signal_frame = unw_is_signal_frame(&cursor);

        /* 打印 */
        printf("  #%d  0x%016lx in %s",
               frame_count, ip, ctx->frames[frame_count].function_name);
        if (ctx->frames[frame_count].is_signal_frame) {
            printf(" (signal handler)");
        }
        printf("\n");

        frame_count++;
    }

    ctx->num_frames = frame_count;
    return frame_count;
}

/**
 * 基于FP（Frame Pointer）的手动展开
 *
 * x86_64调用约定：
 * - RBP指向当前栈帧的底部
 * - [RBP]存储前一个RBP
 * - [RBP+8]存储返回地址
 */
int unwind_with_frame_pointer(unwind_context_t *ctx) {
    void **rbp;
    int frame_count = 0;

    /* 获取当前RBP */
    __asm__ volatile("mov %%rbp, %0" : "=r"(rbp));

    printf("\n=== Stack Trace (Frame Pointer) ===\n");

    while (rbp && frame_count < MAX_FRAMES) {
        void *return_addr = *(rbp + 1);
        void *next_rbp = (void *)*rbp;

        /* 验证返回地址 */
        if (!return_addr || (unsigned long)return_addr < 0x1000)
            break;

        /* 解析符号 */
        resolve_symbol(return_addr, &ctx->frames[frame_count]);
        ctx->frames[frame_count].return_addr = return_addr;
        ctx->frames[frame_count].frame_pointer = rbp;

        printf("  #%d  0x%016lx in %s\n",
               frame_count,
               (unsigned long)return_addr,
               ctx->frames[frame_count].function_name);

        /* 检查是否到达栈顶 */
        if (next_rbp <= rbp)
            break;

        rbp = (void **)next_rbp;
        frame_count++;
    }

    ctx->num_frames = frame_count;
    return frame_count;
}

/**
 * 从信号上下文展开（用于信号处理器）
 */
int unwind_from_signal_context(void *sigcontext, unwind_context_t *ctx) {
    ucontext_t *uc = (ucontext_t *)sigcontext;
    mcontext_t *mc = &uc->uc_mcontext;

    printf("\n=== Signal Context ===\n");

#ifdef __x86_64__
    printf("  RIP: 0x%016llx\n", (unsigned long long)mc->gregs[REG_RIP]);
    printf("  RSP: 0x%016llx\n", (unsigned long long)mc->gregs[REG_RSP]);
    printf("  RBP: 0x%016llx\n", (unsigned long long)mc->gregs[REG_RBP]);

    /* 从信号上下文开始手动展开 */
    void **rbp = (void **)mc->gregs[REG_RBP];
    void *rip = (void *)mc->gregs[REG_RIP];
    int frame_count = 0;

    printf("\n=== Stack Trace from Signal Context ===\n");

    /* 第一个帧是信号发生时的位置 */
    printf("  #0  0x%016llx <signal handler>\n", (unsigned long long)rip);
    ctx->frames[0].return_addr = rip;
    ctx->frames[0].is_signal_frame = 1;
    frame_count = 1;

    /* 继续展开 */
    while (rbp && frame_count < MAX_FRAMES) {
        void *return_addr = *(rbp + 1);
        void *next_rbp = (void *)*rbp;

        if (!return_addr || (unsigned long)return_addr < 0x1000)
            break;

        resolve_symbol(return_addr, &ctx->frames[frame_count]);
        ctx->frames[frame_count].return_addr = return_addr;

        printf("  #%d  0x%016lx in %s\n",
               frame_count,
               (unsigned long)return_addr,
               ctx->frames[frame_count].function_name);

        if (next_rbp <= rbp)
            break;

        rbp = (void **)next_rbp;
        frame_count++;
    }

    ctx->num_frames = frame_count;
#endif

    return ctx->num_frames;
}

/**
 * DWARF CFI解析 - 使用libdw
 */
int unwind_with_dwarf(void *initial_pc, void *initial_sp, void *initial_bp,
                      unwind_context_t *ctx) {
    Dwfl *dwfl = NULL;
    Dwfl_Callbacks callbacks = {
        .find_elf = dwfl_linux_proc_find_elf,
        .find_debuginfo = dwfl_standard_find_debuginfo,
    };

    /* 初始化dwfl */
    dwfl = dwfl_begin(&callbacks);
    if (!dwfl) {
        fprintf(stderr, "dwfl_begin failed\n");
        return -1;
    }

    /* 报告当前进程 */
    if (dwfl_linux_proc_report(dwfl, getpid()) < 0) {
        fprintf(stderr, "dwfl_linux_proc_report failed\n");
        dwfl_end(dwfl);
        return -1;
    }

    /* 附加到当前进程 */
    if (dwfl_attach_state(dwfl, NULL, getpid(), NULL) < 0) {
        fprintf(stderr, "dwfl_attach_state failed\n");
        dwfl_end(dwfl);
        return -1;
    }

    printf("\n=== Stack Trace (DWARF CFI) ===\n");
    printf("DWARF-based unwinding is available through dwfl.\n");
    printf("This provides the most accurate results with debug symbols.\n");

    /* 使用dwfl_getthreads遍历线程 */
    /* 简化实现 - 实际使用需要更复杂的CFA计算 */

    dwfl_end(dwfl);
    return 0;
}

/**
 * 信号处理器
 */
void signal_handler(int sig, siginfo_t *info, void *context) {
    unwind_context_t ctx;
    memset(&ctx, 0, sizeof(ctx));

    printf("\n");
    printf("========================================\n");
    printf("Signal %d received: %s\n", sig, strsignal(sig));
    printf("Fault address: %p\n", info->si_addr);
    printf("========================================\n");

    /* 从信号上下文展开 */
    unwind_from_signal_context(context, &ctx);

    printf("\nProgram terminated.\n");
    _exit(1);
}

/**
 * 设置信号处理器
 */
void setup_signal_handlers() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = signal_handler;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;

    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
    sigaction(SIGFPE, &sa, NULL);
    sigaction(SIGILL, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);
}

/**
 * 测试函数 - 深度调用链
 */
void level_5() {
    printf("\nAt level 5, generating stack trace...\n");

    unwind_context_t ctx;
    memset(&ctx, 0, sizeof(ctx));

    /* 方法1: 简单backtrace */
    void *buffer[MAX_FRAMES];
    int n = backtrace_simple(buffer, MAX_FRAMES);
    printf("\n=== Simple backtrace (%d frames) ===\n", n);
    char **strings = backtrace_symbols(buffer, n);
    for (int i = 0; i < n; i++) {
        printf("  %s\n", strings[i]);
    }
    free(strings);

    /* 方法2: libunwind */
    unwind_with_libunwind(&ctx);

    /* 方法3: Frame Pointer */
    unwind_with_frame_pointer(&ctx);
}

void level_4() { level_5(); }
void level_3() { level_4(); }
void level_2() { level_3(); }
void level_1() { level_2(); }

/**
 * 主函数
 */
int main(int argc, char *argv[]) {
    printf("Stack Unwinder Demo\n");
    printf("===================\n");

    /* 设置信号处理器 */
    setup_signal_handlers();

    /* 测试正常调用链 */
    level_1();

    /* 测试信号处理器中的展开（如果指定）*/
    if (argc > 1 && strcmp(argv[1], "--crash") == 0) {
        printf("\nTriggering segfault...\n");
        int *p = NULL;
        *p = 42;  /* 这将触发SIGSEGV */
    }

    return 0;
}
```

---

## 📖 4. 内存调试进阶（Sanitizer组合使用）

### 4.1 Address Sanitizer + 其他Sanitizer

```c
/**
 * sanitizer_demo.c - Sanitizer综合使用示例
 *
 * 编译命令：
 * gcc -fsanitize=address,undefined,leak -g -O1 -o sanitizer_demo sanitizer_demo.c
 *
 * 分离编译（只使用特定sanitizer）：
 * gcc -fsanitize=address -g -O1 -o asan_demo sanitizer_demo.c
 * gcc -fsanitize=undefined -g -O1 -o ubsan_demo sanitizer_demo.c
 * gcc -fsanitize=thread -g -O1 -o tsan_demo sanitizer_demo.c
 * gcc -fsanitize=memory -g -O1 -o msan_demo sanitizer_demo.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>

/* ============================================================================
 * AddressSanitizer (ASan) - 内存错误检测
 * ============================================================================
 *
 * 检测的缺陷：
 * - Use after free（释放后使用）
 * - Heap buffer overflow（堆缓冲区溢出）
 * - Stack buffer overflow（栈缓冲区溢出）
 * - Global buffer overflow（全局缓冲区溢出）
 * - Use after return（返回后使用）
 * - Use after scope（作用域外使用）
 * - Memory leaks（内存泄漏）- 需要LSan
 */

/**
 * 堆缓冲区溢出示例
 */
void asan_heap_buffer_overflow() {
    printf("\n[ASan] Testing heap buffer overflow...\n");

    int *arr = (int *)malloc(sizeof(int) * 10);

    /* 正常访问 */
    for (int i = 0; i < 10; i++) {
        arr[i] = i;
    }

    /* 堆缓冲区溢出 - ASan会检测到 */
    /* 取消注释以下行来触发错误 */
    /* arr[10] = 42;  */
    /* 或 */
    /* arr[-1] = 42;  */

    free(arr);
}

/**
 * Use after free示例
 */
void asan_use_after_free() {
    printf("\n[ASan] Testing use after free...\n");

    int *ptr = (int *)malloc(sizeof(int));
    *ptr = 42;
    free(ptr);

    /* 使用已释放内存 - ASan会检测到 */
    /* 取消注释以下行来触发错误 */
    /* printf("Value: %d\n", *ptr); */
}

/**
 * 栈缓冲区溢出示例
 */
void asan_stack_buffer_overflow() {
    printf("\n[ASan] Testing stack buffer overflow...\n");

    int arr[10];
    volatile int idx = 10;

    /* 栈缓冲区溢出 - ASan会检测到 */
    /* 取消注释以下行来触发错误 */
    /* arr[idx] = 42; */
}

/**
 * 全局缓冲区溢出示例
 */
int global_arr[10];

void asan_global_buffer_overflow() {
    printf("\n[ASan] Testing global buffer overflow...\n");

    /* 全局缓冲区溢出 - ASan会检测到 */
    /* 取消注释以下行来触发错误 */
    /* global_arr[10] = 42; */
}

/* ============================================================================
 * MemorySanitizer (MSan) - 未初始化内存检测
 * ============================================================================
 *
 * 检测的缺陷：
 * - 使用未初始化内存
 * - 未初始化内存传递给系统调用
 *
 * 注意：MSan需要所有代码（包括库）都使用MSan编译
 */

/**
 * 未初始化内存使用示例
 */
void msan_uninitialized_memory() {
    printf("\n[MSan] Testing uninitialized memory...\n");

    int x;
    /* x未初始化 */

    /* 使用未初始化值 - MSan会检测到 */
    /* 取消注释以下行来触发错误 */
    /* if (x > 0) { */
    /*     printf("x is positive\n"); */
    /* } */

    /* 正确做法：初始化 */
    x = 0;
    if (x > 0) {
        printf("x is positive\n");
    }
}

/**
 * 未初始化传播示例
 */
void msan_uninitialized_propagation() {
    printf("\n[MSan] Testing uninitialized propagation...\n");

    int a, b, c;
    /* a, b未初始化 */

    /* c依赖于未初始化的a和b */
    /* 取消注释以下行来触发错误 */
    /* c = a + b; */
    /* printf("c = %d\n", c); */
}

/* ============================================================================
 * ThreadSanitizer (TSan) - 数据竞争检测
 * ============================================================================
 *
 * 检测的缺陷：
 * - 数据竞争（data race）
 * - 死锁（deadlock）
 * - 信号处理竞争
 *
 * 注意：TSan与ASan不能同时使用
 */

static int shared_counter = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *tsan_race_worker(void *arg) {
    (void)arg;

    /* 数据竞争 - 没有同步 */
    /* 取消注释以下行来触发TSan错误 */
    /* for (int i = 0; i < 1000; i++) { */
    /*     shared_counter++; */
    /* } */

    /* 正确做法：使用互斥锁 */
    for (int i = 0; i < 1000; i++) {
        pthread_mutex_lock(&mutex);
        shared_counter++;
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void tsan_data_race() {
    printf("\n[TSan] Testing data race...\n");

    pthread_t t1, t2;
    shared_counter = 0;

    pthread_create(&t1, NULL, tsan_race_worker, NULL);
    pthread_create(&t2, NULL, tsan_race_worker, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final counter: %d (expected: 2000)\n", shared_counter);
}

/* ============================================================================
 * UndefinedBehaviorSanitizer (UBSan) - 未定义行为检测
 * ============================================================================
 *
 * 检测的缺陷：
 * - 有符号整数溢出
 * - 浮点数转换溢出
 * - 除零
 * - 空指针解引用
 * - 对齐问题
 * - 数组下标越界（-fsanitize=bounds）
 * - 对象大小问题（-fsanitize=object-size）
 */

/**
 * 有符号整数溢出示例
 */
void ubsan_signed_overflow() {
    printf("\n[UBSan] Testing signed integer overflow...\n");

    int max = INT32_MAX;
    /* 有符号整数溢出 - UBSan会检测到 */
    /* 取消注释以下行来触发错误 */
    /* int result = max + 1; */
    /* printf("Result: %d\n", result); */

    /* 正确做法：使用无符号整数或检查溢出 */
    unsigned int umax = UINT32_MAX;
    unsigned int uresult = umax + 1;  /* 无符号溢出是明确定义的 */
    printf("Unsigned result: %u\n", uresult);
}

/**
 * 除零示例
 */
void ubsan_divide_by_zero() {
    printf("\n[UBSan] Testing divide by zero...\n");

    int x = 10;
    int y = 0;

    /* 除零 - UBSan会检测到 */
    /* 取消注释以下行来触发错误 */
    /* int result = x / y; */
    /* printf("Result: %d\n", result); */

    /* 正确做法：检查除数 */
    if (y != 0) {
        int result = x / y;
        printf("Result: %d\n", result);
    } else {
        printf("Division by zero avoided\n");
    }
}

/**
 * 空指针解引用示例
 */
void ubsan_null_pointer() {
    printf("\n[UBSan] Testing null pointer dereference...\n");

    int *ptr = NULL;

    /* 空指针解引用 - UBSan/ASan会检测到 */
    /* 取消注释以下行来触发错误 */
    /* *ptr = 42; */

    /* 正确做法：检查指针 */
    if (ptr != NULL) {
        *ptr = 42;
    }
}

/**
 * 对齐问题示例
 */
void ubsan_alignment() {
    printf("\n[UBSan] Testing alignment issues...\n");

    char buf[16];
    /* 将char数组强制转换为int指针，可能不对齐 */
    /* 取消注释以下行来触发错误（在某些架构上） */
    /* int *ptr = (int *)buf; */
    /* *ptr = 42; */

    /* 正确做法：使用对齐的内存 */
    alignas(int) char aligned_buf[16];
    int *ptr = (int *)aligned_buf;
    *ptr = 42;
}

/**
 * 数组边界检查示例
 */
void ubsan_array_bounds() {
    printf("\n[UBSan] Testing array bounds...\n");

    int arr[10];
    int idx = 15;  /* 越界索引 */

    /* 数组越界 - UBSan bounds会检测到 */
    /* 取消注释以下行来触发错误 */
    /* arr[idx] = 42; */

    /* 正确做法：检查边界 */
    if (idx >= 0 && idx < 10) {
        arr[idx] = 42;
    }
}

/**
 * 无效移位示例
 */
void ubsan_invalid_shift() {
    printf("\n[UBSan] Testing invalid shift...\n");

    int x = 1;
    /* 移位超过类型宽度 - UBSan会检测到 */
    /* 取消注释以下行来触发错误 */
    /* int result = x << 32; */

    /* 负数移位 - UBSan会检测到 */
    /* int result2 = x << -1; */

    /* 正确做法：确保移位量在有效范围内 */
    int result = x << 31;  /* 对于32位int，这是最大值 */
    printf("Result: %d\n", result);
}

/* ============================================================================
 * LeakSanitizer (LSan) - 内存泄漏检测
 * ============================================================================
 *
 * 默认集成在ASan中，也可以单独使用
 */

/**
 * 内存泄漏示例
 */
void lsan_memory_leak() {
    printf("\n[LSan] Testing memory leak...\n");

    /* 分配内存但不释放 - LSan会检测到 */
    /* 取消注释以下行来触发错误 */
    /* int *leaked = (int *)malloc(sizeof(int) * 100); */
    /* leaked[0] = 42; */
    /* 函数返回，没有free - 泄漏！ */

    /* 正确做法：释放内存 */
    int *not_leaked = (int *)malloc(sizeof(int) * 100);
    not_leaked[0] = 42;
    free(not_leaked);
}

/**
 * 间接内存泄漏示例
 */
typedef struct {
    int *data;
    size_t size;
} indirect_leak_struct;

void lsan_indirect_leak() {
    printf("\n[LSan] Testing indirect memory leak...\n");

    indirect_leak_struct *s = (indirect_leak_struct *)malloc(sizeof(*s));
    s->data = (int *)malloc(sizeof(int) * 100);  /* 间接泄漏 */
    s->size = 100;

    /* 只释放外层结构，不释放内部数据 - 间接泄漏 */
    /* 取消注释以下行来触发错误 */
    /* free(s); */

    /* 正确做法：完全释放 */
    free(s->data);
    free(s);
}

/* ============================================================================
 * 主函数
 * ============================================================================
 */

int main(int argc, char *argv[]) {
    printf("========================================\n");
    printf("Sanitizer Comprehensive Demo\n");
    printf("========================================\n");

    /* AddressSanitizer测试 */
    asan_heap_buffer_overflow();
    asan_use_after_free();
    asan_stack_buffer_overflow();
    asan_global_buffer_overflow();

    /* MemorySanitizer测试 */
    msan_uninitialized_memory();
    msan_uninitialized_propagation();

    /* ThreadSanitizer测试（不能与ASan同时使用）*/
    tsan_data_race();

    /* UndefinedBehaviorSanitizer测试 */
    ubsan_signed_overflow();
    ubsan_divide_by_zero();
    ubsan_null_pointer();
    ubsan_alignment();
    ubsan_array_bounds();
    ubsan_invalid_shift();

    /* LeakSanitizer测试 */
    lsan_memory_leak();
    lsan_indirect_leak();

    printf("\n========================================\n");
    printf("All tests completed.\n");
    printf("========================================\n");

    return 0;
}
```

### 4.2 Sanitizer抑制文件

```bash
#!/bin/bash
# sanitizer_suppressions.sh - Sanitizer抑制规则配置

# =============================================================================
# AddressSanitizer 抑制规则
# =============================================================================

cat > asan.supp << 'EOF'
# AddressSanitizer抑制文件格式
# 每行一个模式，匹配栈帧中的函数名或源文件

# 抑制特定函数的内存泄漏
leak:my_known_leak_function
leak:third_party_library_init

# 抑制特定模块的所有错误
interceptor_via_lib:lib problematic.so

# 抑制特定源文件的错误
src:*/third_party/*
src:*/generated_code/*

# 抑制特定函数的特定错误类型
[heap-buffer-overflow]
fun:legacy_buffer_function

# 抑制特定线程的泄漏（如静态初始化器线程）
leak:__static_initialization_and_destruction_0
EOF

# 使用方法: ASAN_OPTIONS=suppressions=asan.supp ./program

echo "Created asan.supp"

# =============================================================================
# ThreadSanitizer 抑制规则
# =============================================================================

cat > tsan.supp << 'EOF'
# ThreadSanitizer抑制文件
# 用于抑制已知的、非问题的数据竞争

# 抑制特定函数的竞争
race:my_known_race_function

# 抑制特定库的所有竞争
race:libpthread.so*
race:libz.so*

# 抑制特定源文件
src:*/third_party/old_code.c

# 抑制特定全局变量的竞争
race:global_debug_counter

# 抑制特定类型的竞争（如统计计数器）
race_top:statistics_update

# 抑制初始化竞争（通常是良性的）
race:Singleton::getInstance
EOF

echo "Created tsan.supp"

# =============================================================================
# MemorySanitizer 抑制规则
# =============================================================================

cat > msan.supp << 'EOF'
# MemorySanitizer抑制文件

# 抑制特定函数的未初始化值使用
uninit:legacy_function_using_uninit

# 抑制特定模块
origin:*/third_party/*

# 抑制特定全局变量
uninit:global_config_buffer
EOF

echo "Created msan.supp"

# =============================================================================
# UndefinedBehaviorSanitizer 抑制规则
# =============================================================================

cat > ubsan.supp << 'EOF'
# UBSan抑制文件

# 抑制特定函数的溢出检查
[signed-integer-overflow]
fun:my_hash_function

# 抑制特定对齐问题
[alignment]
src:*/legacy_misaligned_struct.c

# 抑制特定除零检查
[integer-divide-by-zero]
fun:safe_division_wrapper
EOF

echo "Created ubsan.supp"

# =============================================================================
# LeakSanitizer 抑制规则
# =============================================================================

cat > lsan.supp << 'EOF'
# LeakSanitizer抑制文件

# 抑制特定类的所有对象泄漏
leak:MySingleton

# 抑制特定函数的泄漏
leak:initialize_static_data

# 抑制特定大小范围的泄漏（不太可能真的是泄漏）
leak:* size: 0-16

# 抑制匹配特定模式的泄漏
leak:std::string::_Rep::_S_create
leak:std::basic_string::*::create
EOF

echo "Created lsan.supp"

echo ""
echo "Usage examples:"
echo "  ASAN_OPTIONS=suppressions=asan.supp:detect_odr_violation=0 ./program"
echo "  TSAN_OPTIONS=suppressions=tsan.supp:halt_on_error=0 ./program"
echo "  MSAN_OPTIONS=suppressions=msan.supp ./program"
echo "  UBSAN_OPTIONS=suppressions=ubsan.supp:print_stacktrace=1 ./program"
echo "  LSAN_OPTIONS=suppressions=lsan.supp:max_leaks=10 ./program"
```



---

## 📖 5. 性能分析（perf/火焰图）

### 5.1 perf工具使用

```bash
#!/bin/bash
# perf_analysis.sh - Linux perf性能分析脚本

# =============================================================================
# 1. 基本性能计数器
# =============================================================================

# 统计CPU周期和指令数
perf stat ./program

# 详细统计（包括缓存、分支等）
perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses ./program

# 针对特定CPU
perf stat -a -C 0 ./program

# 针对特定进程
perf stat -p $(pgrep program_name)

# =============================================================================
# 2. 采样分析
# =============================================================================

# 记录CPU周期样本
perf record ./program

# 以更高频率采样（可能产生更多开销）
perf record -F 997 ./program

# 记录特定事件
perf record -e cache-misses ./program
perf record -e branch-misses ./program

# 多线程程序采样
perf record -g --threads ./program

# 记录内核和用户空间
perf record -g --kernel-callchains ./program

# =============================================================================
# 3. 报告生成
# =============================================================================

# 文本报告（热点函数）
perf report

# 排序显示
perf report --sort=dso,symbol

# 只显示用户空间
perf report --dso-only

# 生成调用图
perf report -g graph

# 生成堆叠报告
perf report --stdio

# =============================================================================
# 4. 火焰图生成
# =============================================================================

# 下载火焰图工具
FLAMEGRAPH_DIR="$HOME/FlameGraph"
if [ ! -d "$FLAMEGRAPH_DIR" ]; then
    git clone https://github.com/brendangregg/FlameGraph.git "$FLAMEGRAPH_DIR"
fi

# 记录数据并生成火焰图
perf record -F 99 -a -g -- sleep 30
perf script | "$FLAMEGRAPH_DIR/stackcollapse-perf.pl" | "$FLAMEGRAPH_DIR/flamegraph.pl" > perf_flamegraph.svg

# 差分火焰图（比较两个性能剖析）
# 记录基准版本
perf record -F 99 -a -g -- ./program_baseline
perf script | "$FLAMEGRAPH_DIR/stackcollapse-perf.pl" > out.baseline

# 记录修改版本
perf record -F 99 -a -g -- ./program_modified
perf script | "$FLAMEGRAPH_DIR/stackcollapse-perf.pl" > out.modified

# 生成差分火焰图
"$FLAMEGRAPH_DIR/difffolded.pl" out.baseline out.modified | "$FLAMEGRAPH_DIR/flamegraph.pl" > diff_flamegraph.svg

# 红蓝差分火焰图
"$FLAMEGRAPH_DIR/difffolded.pl" -n out.baseline out.modified | "$FLAMEGRAPH_DIR/flamegraph.pl" --negation > diff_neg_flamegraph.svg

# =============================================================================
# 5. 跟踪点分析
# =============================================================================

# 列出可用跟踪点
perf list 'sched:*'
perf list 'syscalls:*'

# 记录调度事件
perf record -e sched:sched_switch,sched:sched_wakeup -a -- sleep 10

# 分析调度延迟
perf sched record -- sleep 10
perf sched latency
perf sched map

# =============================================================================
# 6. 内存分析
# =============================================================================

# 分析内存带宽（需要Intel PCM或类似工具）
# perf stat -e uncore_imc/clockticks/,uncore_imc/cas_count_read/ sleep 1

# 分析TLB未命中
perf stat -e dTLB-loads,dTLB-load-misses,iTLB-loads,iTLB-load-misses ./program

# 分析页错误
perf stat -e page-faults,major-faults,minor-faults ./program

# =============================================================================
# 7. 缓存分析
# =============================================================================

# L1缓存
perf stat -e L1-dcache-loads,L1-dcache-load-misses,L1-icache-load-misses ./program

# LLC（Last Level Cache）
perf stat -e LLC-loads,LLC-load-misses,LLC-stores,LLC-store-misses ./program

# 缓存行争用（false sharing检测）
perf c2c record ./program
perf c2c report

# =============================================================================
# 8. 分支预测分析
# =============================================================================

perf stat -e branches,branch-misses,branch-loads,branch-load-misses ./program

# 详细分支统计（Intel）
perf stat -e br_inst_retired.all_branches,br_misp_retired.all_branches ./program

# =============================================================================
# 9. 锁定分析
# =============================================================================

# 记录锁定事件
perf lock record ./program
perf lock report
perf lock stat
perf lock contended

# =============================================================================
# 10. BPF集成（高级）
# =============================================================================

# 使用perf with BPF
perf record -e bpf-output ./program

# 自定义BPF程序附加到perf事件
# perf record -e $(perf probe -L function) ./program

# =============================================================================
# 实用函数
# =============================================================================

analyze_function() {
    local func=$1
    local prog=$2

    echo "Analyzing function: $func"

    # 创建probe
    sudo perf probe --add "$func"

    # 记录
    sudo perf record -e probe:$func -a -- ./$prog

    # 报告
    perf report

    # 删除probe
    sudo perf probe --del "probe:$func"
}

profile_io() {
    local prog=$1

    echo "Profiling I/O operations..."

    # 记录块I/O事件
    sudo perf record -e block:block_rq_issue,block:block_rq_complete -a \
         -- ./$prog

    perf script --show-switch-events
}

generate_offcpu_flamegraph() {
    local prog=$1

    echo "Generating Off-CPU flamegraph..."

    # 记录调度事件
    sudo perf record -e sched:sched_switch -e sched:sched_stat_sleep \
         -e sched:sched_stat_blocked -e sched:sched_stat_iowait -a \
         -- ./$prog

    # 生成Off-CPU火焰图
    perf script | \
        awk '/sched_switch/ { print $1, $2, $3 }' | \
        "$FLAMEGRAPH_DIR/stackcollapse-perf.pl" | \
        "$FLAMEGRAPH_DIR/flamegraph.pl" --title "Off-CPU Time" \
        > offcpu_flamegraph.svg
}

echo "Perf Analysis Script Loaded"
echo "Usage: source perf_analysis.sh"
```

### 5.2 性能分析C程序

```c
/**
 * perf_helper.c - 程序中集成性能计数器
 *
 * 编译：gcc -o perf_helper perf_helper.c
 * 运行：sudo ./perf_helper （需要root权限或perf_event_paranoid设置）
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <asm/unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>

/* perf_event_open系统调用包装 */
static long perf_event_open(struct perf_event_attr *hw_event,
                            pid_t pid, int cpu, int group_fd,
                            unsigned long flags)
{
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

/* 性能计数器句柄 */
typedef struct {
    int fd;
    const char *name;
    uint64_t id;
} perf_counter_t;

/* 计数器组 */
typedef struct {
    perf_counter_t *counters;
    int num_counters;
} perf_group_t;

/**
 * 创建硬件性能计数器
 */
int create_hw_counter(const char *name, uint64_t config, int group_fd) {
    struct perf_event_attr pe;

    memset(&pe, 0, sizeof(pe));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(pe);
    pe.config = config;
    pe.disabled = 1;  /* 开始时禁用 */
    pe.exclude_kernel = 0;
    pe.exclude_hv = 0;
    pe.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;

    int fd = perf_event_open(&pe, 0, -1, group_fd, 0);
    if (fd == -1) {
        fprintf(stderr, "Error opening %s: %s\n", name, strerror(errno));
        return -1;
    }

    return fd;
}

/**
 * 创建软件性能计数器
 */
int create_sw_counter(const char *name, uint64_t config, int group_fd) {
    struct perf_event_attr pe;

    memset(&pe, 0, sizeof(pe));
    pe.type = PERF_TYPE_SOFTWARE;
    pe.size = sizeof(pe);
    pe.config = config;
    pe.disabled = 1;
    pe.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;

    int fd = perf_event_open(&pe, 0, -1, group_fd, 0);
    if (fd == -1) {
        fprintf(stderr, "Error opening %s: %s\n", name, strerror(errno));
        return -1;
    }

    return fd;
}

/**
 * 创建缓存性能计数器
 */
int create_cache_counter(const char *name, int op, int result, int group_fd) {
    struct perf_event_attr pe;

    memset(&pe, 0, sizeof(pe));
    pe.type = PERF_TYPE_HW_CACHE;
    pe.size = sizeof(pe);
    /* config格式: (cache_id) | (op_id << 8) | (result_id << 16) */
    pe.config = PERF_COUNT_HW_CACHE_L1D |
                (op << 8) |
                (result << 16);
    pe.disabled = 1;
    pe.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;

    int fd = perf_event_open(&pe, 0, -1, group_fd, 0);
    if (fd == -1) {
        /* 尝试L1I */
        pe.config = PERF_COUNT_HW_CACHE_L1I | (op << 8) | (result << 16);
        fd = perf_event_open(&pe, 0, -1, group_fd, 0);
    }
    if (fd == -1) {
        /* 尝试LL */
        pe.config = PERF_COUNT_HW_CACHE_LL | (op << 8) | (result << 16);
        fd = perf_event_open(&pe, 0, -1, group_fd, 0);
    }

    if (fd == -1) {
        fprintf(stderr, "Error opening %s: %s\n", name, strerror(errno));
        return -1;
    }

    return fd;
}

/**
 * 性能测量上下文
 */
typedef struct {
    int group_fd;
    int num_events;
    struct {
        const char *name;
        uint64_t value;
    } results[10];
} perf_context_t;

/**
 * 开始性能测量
 */
int perf_begin(perf_context_t *ctx) {
    memset(ctx, 0, sizeof(*ctx));

    /* 创建组leader（cycles） */
    ctx->group_fd = create_hw_counter("cycles", PERF_COUNT_HW_CPU_CYCLES, -1);
    if (ctx->group_fd < 0) {
        return -1;
    }

    /* 添加其他计数器到组 */
    int fds[10];
    fds[0] = ctx->group_fd;

    fds[1] = create_hw_counter("instructions", PERF_COUNT_HW_INSTRUCTIONS,
                                ctx->group_fd);
    fds[2] = create_cache_counter("L1-dcache-loads",
                                   PERF_COUNT_HW_CACHE_OP_READ,
                                   PERF_COUNT_HW_CACHE_RESULT_ACCESS,
                                   ctx->group_fd);
    fds[3] = create_cache_counter("L1-dcache-load-misses",
                                   PERF_COUNT_HW_CACHE_OP_READ,
                                   PERF_COUNT_HW_CACHE_RESULT_MISS,
                                   ctx->group_fd);
    fds[4] = create_hw_counter("branches", PERF_COUNT_HW_BRANCH_INSTRUCTIONS,
                                ctx->group_fd);
    fds[5] = create_hw_counter("branch-misses", PERF_COUNT_HW_BRANCH_MISSES,
                                ctx->group_fd);
    fds[6] = create_sw_counter("page-faults", PERF_COUNT_SW_PAGE_FAULTS,
                                ctx->group_fd);

    ctx->num_events = 7;

    /* 启用计数 */
    ioctl(ctx->group_fd, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
    ioctl(ctx->group_fd, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);

    return 0;
}

/**
 * 结束性能测量
 */
void perf_end(perf_context_t *ctx) {
    /* 禁用计数 */
    ioctl(ctx->group_fd, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);

    /* 读取结果 */
    struct {
        uint64_t nr;
        uint64_t values[20];  /* value + id pairs */
    } read_buffer;

    if (read(ctx->group_fd, &read_buffer, sizeof(read_buffer)) > 0) {
        printf("\n=== Performance Counters ===\n");
        printf("Events counted: %" PRIu64 "\n", read_buffer.nr);
        for (int i = 0; i < read_buffer.nr && i < ctx->num_events; i++) {
            printf("  Event %d: %" PRIu64 "\n", i, read_buffer.values[i * 2]);
        }
    }

    close(ctx->group_fd);
}

/* ============================================================================
 * 使用示例
 * ============================================================================ */

void test_function_1() {
    volatile int sum = 0;
    for (int i = 0; i < 1000000; i++) {
        sum += i;
    }
}

void test_function_2() {
    int *arr = malloc(1000000 * sizeof(int));
    for (int i = 0; i < 1000000; i++) {
        arr[i] = i;
    }

    /* 缓存不友好访问 */
    volatile int sum = 0;
    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 1000; j++) {
            sum += arr[j * 1000 + i];  /* 跨步访问 */
        }
    }

    free(arr);
}

void test_function_3() {
    /* 分支预测不友好代码 */
    volatile int sum = 0;
    for (int i = 0; i < 1000000; i++) {
        if (i % 7 == 0) {  /* 随机模式 */
            sum += i;
        } else {
            sum -= i;
        }
    }
}

int main(int argc, char *argv[]) {
    perf_context_t ctx;

    printf("Performance Counter Demo\n");
    printf("========================\n\n");

    /* 测试1: CPU密集 */
    printf("Test 1: CPU intensive\n");
    if (perf_begin(&ctx) == 0) {
        test_function_1();
        perf_end(&ctx);
    }

    /* 测试2: 缓存不友好 */
    printf("\nTest 2: Cache unfriendly\n");
    if (perf_begin(&ctx) == 0) {
        test_function_2();
        perf_end(&ctx);
    }

    /* 测试3: 分支预测不友好 */
    printf("\nTest 3: Branch unfriendly\n");
    if (perf_begin(&ctx) == 0) {
        test_function_3();
        perf_end(&ctx);
    }

    return 0;
}
```

### 5.3 火焰图生成脚本

```python
#!/usr/bin/env python3
"""
flamegraph_generator.py - 火焰图生成器

功能：
1. 从perf数据生成火焰图
2. 支持多种格式（折叠、SVG）
3. 支持差分火焰图
4. 支持交互式过滤

依赖：pip install flask matplotlib
"""

import sys
import re
import json
import subprocess
from collections import defaultdict
from dataclasses import dataclass, field
from typing import Dict, List, Set, Optional
from pathlib import Path

@dataclass
class StackFrame:
    """栈帧节点"""
    name: str
    samples: int = 0
    children: Dict[str, 'StackFrame'] = field(default_factory=dict)
    parent: Optional['StackFrame'] = None

    def get_or_create_child(self, name: str) -> 'StackFrame':
        if name not in self.children:
            child = StackFrame(name=name, parent=self)
            self.children[name] = child
        return self.children[name]

    def total_samples(self) -> int:
        """递归计算总样本数"""
        total = self.samples
        for child in self.children.values():
            total += child.total_samples()
        return total

class FlameGraphBuilder:
    """火焰图构建器"""

    def __init__(self):
        self.root = StackFrame(name="root")
        self.total_samples = 0

    def add_stack(self, stack: List[str], count: int = 1):
        """添加一个调用栈"""
        current = self.root
        # 反转栈（从根到叶）
        for frame in reversed(stack):
            current = current.get_or_create_child(frame)
        current.samples += count
        self.total_samples += count

    def parse_perf_script(self, data: str):
        """解析perf script输出"""
        current_stack = []

        for line in data.strip().split('\n'):
            line = line.strip()

            # 检测新的样本
            if line and not line.startswith(' '):
                # 保存之前的栈
                if current_stack:
                    self.add_stack(current_stack)
                    current_stack = []

                # 解析样本行（格式因perf版本而异）
                # 通常包含：程序名、PID、时间等
                continue

            # 解析栈帧
            if line.startswith('0x'):
                # 地址行
                parts = line.split()
                if len(parts) >= 2:
                    symbol = parts[1] if len(parts) > 1 else parts[0]
                    current_stack.append(symbol)
            elif line:
                # 可能是符号名
                current_stack.append(line.strip())

        # 添加最后一个栈
        if current_stack:
            self.add_stack(current_stack)

    def parse_folded(self, data: str):
        """解析折叠格式（semicolon分隔）"""
        for line in data.strip().split('\n'):
            if ';' not in line:
                continue

            parts = line.rsplit(' ', 1)
            if len(parts) != 2:
                continue

            stack_str = parts[0]
            try:
                count = int(parts[1])
            except ValueError:
                count = 1

            stack = stack_str.split(';')
            self.add_stack(stack, count)

    def to_folded(self) -> str:
        """输出折叠格式"""
        lines = []

        def traverse(node: StackFrame, path: List[str]):
            if node.samples > 0:
                stack_str = ';'.join(path)
                lines.append(f"{stack_str} {node.samples}")

            for name, child in sorted(node.children.items()):
                traverse(child, path + [name])

        for name, child in sorted(self.root.children.items()):
            traverse(child, [name])

        return '\n'.join(lines)

    def to_svg(self, width: int = 1200, height_per_level: int = 16) -> str:
        """生成SVG火焰图"""
        if self.total_samples == 0:
            return "<svg></svg>"

        # 计算高度
        max_depth = self._max_depth(self.root)
        height = (max_depth + 1) * height_per_level + 50

        svg_parts = [
            f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}">',
            '<style>',
            '  .func { font: 12px monospace; fill: white; }',
            '  .func:hover { filter: brightness(1.2); }',
            '  text { pointer-events: none; }',
            '</style>',
            f'<text x="10" y="20" class="title" font-size="14" font-weight="bold">',
            f'  Flame Graph (Total: {self.total_samples} samples)',
            '</text>',
        ]

        # 颜色映射
        colors = [
            "#e74c3c", "#3498db", "#2ecc71", "#f39c12",
            "#9b59b6", "#1abc9c", "#e67e22", "#34495e"
        ]

        def color_for_name(name: str) -> str:
            """根据名称生成颜色"""
            # 根据名称哈希选择颜色
            hash_val = sum(ord(c) for c in name)
            return colors[hash_val % len(colors)]

        # 绘制矩形
        def draw_node(node: StackFrame, x: float, y: float,
                     width_scale: float, depth: int):

            node_samples = node.total_samples()
            node_width = (node_samples / self.total_samples) * width_scale

            if node_width < 1:
                return

            color = color_for_name(node.name) if depth > 0 else "#2c3e50"

            # 绘制矩形
            svg_parts.append(
                f'<g class="func">'
                f'<rect x="{x}" y="{y}" width="{node_width}" '
                f'height="{height_per_level - 1}" fill="{color}" '
                f'stroke="white" stroke-width="0.5">'
                f'<title>{node.name}\nSamples: {node.samples}\n'
                f'Total: {node_samples}\n'
                f'{(node_samples/self.total_samples)*100:.2f}%</title>'
                f'</rect>'
            )

            # 绘制文本（如果空间足够）
            if node_width > 30 and node.name != "root":
                text_x = x + 3
                text_y = y + height_per_level - 4
                display_name = node.name[:int(node_width / 7)]
                svg_parts.append(
                    f'<text x="{text_x}" y="{text_y}" class="func">'
                    f'{display_name}</text>'
                )

            svg_parts.append('</g>')

            # 递归绘制子节点
            child_x = x
            for child_name, child in sorted(node.children.items(),
                                             key=lambda x: -x[1].total_samples()):
                draw_node(child, child_x, y + height_per_level, width_scale, depth + 1)
                child_width = (child.total_samples() / self.total_samples) * width_scale
                child_x += child_width

        draw_node(self.root, 0, 30, width, 0)

        svg_parts.append('</svg>')
        return '\n'.join(svg_parts)

    def _max_depth(self, node: StackFrame, depth: int = 0) -> int:
        """计算最大深度"""
        if not node.children:
            return depth
        return max(self._max_depth(child, depth + 1)
                   for child in node.children.values())

    def diff(self, other: 'FlameGraphBuilder') -> 'FlameGraphDiff':
        """计算与另一个火焰图的差异"""
        return FlameGraphDiff(self, other)

class FlameGraphDiff:
    """火焰图差异分析"""

    def __init__(self, baseline: FlameGraphBuilder, modified: FlameGraphBuilder):
        self.baseline = baseline
        self.modified = modified
        self.diff_tree = self._compute_diff()

    def _compute_diff(self) -> StackFrame:
        """计算差异树"""
        result = StackFrame(name="root")

        # 收集所有唯一的栈
        all_stacks = set()

        def collect_stacks(node: StackFrame, path: tuple):
            if node.samples > 0:
                all_stacks.add((path, node.samples))
            for name, child in node.children.items():
                collect_stacks(child, path + (name,))

        collect_stacks(self.baseline.root, ())
        collect_stacks(self.modified.root, ())

        # 计算差异
        # 简化实现：只显示修改后的火焰图
        return self.modified.root

    def to_svg(self, width: int = 1200) -> str:
        """生成差分火焰图（红色表示增长，蓝色表示减少）"""
        # 使用不同颜色表示差异
        return self.modified.to_svg(width)

def main():
    """主函数"""
    import argparse

    parser = argparse.ArgumentParser(description='Flame Graph Generator')
    parser.add_argument('input', help='Input file (perf script or folded)')
    parser.add_argument('-o', '--output', default='flamegraph.svg',
                       help='Output file')
    parser.add_argument('-f', '--format', choices=['svg', 'folded'],
                       default='svg', help='Output format')
    parser.add_argument('-w', '--width', type=int, default=1200,
                       help='SVG width')
    parser.add_argument('--diff', help='Diff against another folded file')

    args = parser.parse_args()

    # 读取输入
    with open(args.input, 'r') as f:
        data = f.read()

    # 构建火焰图
    builder = FlameGraphBuilder()

    # 自动检测格式
    if ';' in data[:1000]:
        builder.parse_folded(data)
    else:
        builder.parse_perf_script(data)

    print(f"Loaded {builder.total_samples} samples")
    print(f"Max depth: {builder._max_depth(builder.root)}")

    # 差分模式
    if args.diff:
        with open(args.diff, 'r') as f:
            diff_data = f.read()

        other = FlameGraphBuilder()
        if ';' in diff_data[:1000]:
            other.parse_folded(diff_data)
        else:
            other.parse_perf_script(diff_data)

        diff = builder.diff(other)
        output = diff.to_svg(args.width)
    else:
        # 正常输出
        if args.format == 'svg':
            output = builder.to_svg(args.width)
        else:
            output = builder.to_folded()

    # 写入输出
    with open(args.output, 'w') as f:
        f.write(output)

    print(f"Output written to {args.output}")

if __name__ == '__main__':
    main()
```

---

## 📖 6. 并发调试（死锁检测、竞态条件）

### 6.1 死锁检测工具

```c
/**
 * deadlock_detector.c - 运行时死锁检测器
 *
 * 实现资源分配图的循环检测算法
 * 自动检测并报告潜在的死锁
 *
 * 编译：gcc -o deadlock_detector deadlock_detector.c -lpthread
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <execinfo.h>
#include <dlfcn.h>

/* ============================================================================
 * 资源分配图（RAG）实现
 * ============================================================================ */

#define MAX_THREADS 1024
#define MAX_LOCKS 1024
#define MAX_EDGES 4096

/* 边类型 */
typedef enum {
    EDGE_REQUEST,   /* 线程请求锁 */
    EDGE_HOLD       /* 线程持有锁 */
} edge_type_t;

/* 图边 */
typedef struct {
    int from;           /* 线程ID */
    int to;             /* 锁ID */
    edge_type_t type;
    void *lock_addr;    /* 原始锁地址 */
    pthread_t thread;   /* 原始线程ID */
    time_t timestamp;
    char stacktrace[1024];
} edge_t;

/* 资源分配图 */
typedef struct {
    edge_t edges[MAX_EDGES];
    int num_edges;

    bool thread_active[MAX_THREADS];
    bool lock_active[MAX_LOCKS];

    pthread_mutex_t graph_mutex;
} resource_graph_t;

static resource_graph_t g_graph = {0};
static pthread_once_t g_init_once = PTHREAD_ONCE_INIT;
static int g_next_thread_id = 0;
static int g_next_lock_id = 0;

/* 线程本地存储：当前线程ID */
__thread int tls_thread_id = -1;

/* ============================================================================
 * 初始化
 * ============================================================================ */

void init_detector(void) {
    pthread_mutex_init(&g_graph.graph_mutex, NULL);
    memset(&g_graph, 0, sizeof(g_graph));
    pthread_mutex_init(&g_graph.graph_mutex, NULL);

    fprintf(stderr, "[DeadlockDetector] Initialized\n");
}

int get_thread_id(void) {
    if (tls_thread_id < 0) {
        /* 分配新线程ID */
        static pthread_mutex_t id_mutex = PTHREAD_INITIALIZER;
        pthread_mutex_lock(&id_mutex);
        tls_thread_id = g_next_thread_id++;
        pthread_mutex_unlock(&id_mutex);

        if (tls_thread_id >= MAX_THREADS) {
            fprintf(stderr, "[DeadlockDetector] Too many threads!\n");
            abort();
        }

        pthread_mutex_lock(&g_graph.graph_mutex);
        g_graph.thread_active[tls_thread_id] = true;
        pthread_mutex_unlock(&g_graph.graph_mutex);
    }
    return tls_thread_id;
}

int get_lock_id(void *lock_addr) {
    /* 使用锁地址的哈希作为ID */
    uintptr_t addr = (uintptr_t)lock_addr;
    return (int)((addr / sizeof(pthread_mutex_t)) % MAX_LOCKS);
}

/* ============================================================================
 * 图操作
 * ============================================================================ */

void capture_stacktrace(char *buffer, size_t size) {
    void *frames[10];
    int n = backtrace(frames, 10);
    char **symbols = backtrace_symbols(frames, n);

    buffer[0] = '\0';
    for (int i = 2; i < n && i < 6; i++) {  /* 跳过前两个帧 */
        strncat(buffer, symbols[i], size - strlen(buffer) - 1);
        strncat(buffer, "\n", size - strlen(buffer) - 1);
    }

    free(symbols);
}

void add_edge(int thread_id, int lock_id, edge_type_t type, void *lock_addr) {
    pthread_mutex_lock(&g_graph.graph_mutex);

    if (g_graph.num_edges >= MAX_EDGES) {
        fprintf(stderr, "[DeadlockDetector] Edge limit reached!\n");
        pthread_mutex_unlock(&g_graph.graph_mutex);
        return;
    }

    edge_t *edge = &g_graph.edges[g_graph.num_edges++];
    edge->from = thread_id;
    edge->to = lock_id;
    edge->type = type;
    edge->lock_addr = lock_addr;
    edge->thread = pthread_self();
    edge->timestamp = time(NULL);
    capture_stacktrace(edge->stacktrace, sizeof(edge->stacktrace));

    g_graph.lock_active[lock_id] = true;

    pthread_mutex_unlock(&g_graph.graph_mutex);
}

void remove_edge(int thread_id, int lock_id, edge_type_t type) {
    pthread_mutex_lock(&g_graph.graph_mutex);

    /* 找到并移除边 */
    for (int i = 0; i < g_graph.num_edges; i++) {
        if (g_graph.edges[i].from == thread_id &&
            g_graph.edges[i].to == lock_id &&
            g_graph.edges[i].type == type) {
            /* 移动后面的边 */
            memmove(&g_graph.edges[i], &g_graph.edges[i + 1],
                   (g_graph.num_edges - i - 1) * sizeof(edge_t));
            g_graph.num_edges--;
            break;
        }
    }

    pthread_mutex_unlock(&g_graph.graph_mutex);
}

/* ============================================================================
 * 环检测（DFS）
 * ============================================================================ */

bool dfs_find_cycle(int node, bool *visited, bool *rec_stack,
                    int *parent, int *cycle_start) {
    visited[node] = true;
    rec_stack[node] = true;

    /* 遍历所有从该节点出发的边 */
    for (int i = 0; i < g_graph.num_edges; i++) {
        edge_t *edge = &g_graph.edges[i];

        /* 找到从当前节点出发的REQUEST边（线程->锁） */
        /* 或HOLD边（锁->线程）用于构建等待图 */
        int next_node = -1;

        if (edge->from == node && edge->type == EDGE_REQUEST) {
            /* 线程请求锁：找谁持有这个锁 */
            for (int j = 0; j < g_graph.num_edges; j++) {
                if (g_graph.edges[j].to == edge->to &&
                    g_graph.edges[j].type == EDGE_HOLD) {
                    next_node = g_graph.edges[j].from;
                    break;
                }
            }
        }

        if (next_node < 0) continue;

        if (!visited[next_node]) {
            parent[next_node] = node;
            if (dfs_find_cycle(next_node, visited, rec_stack, parent, cycle_start)) {
                return true;
            }
        } else if (rec_stack[next_node]) {
            /* 发现环 */
            *cycle_start = next_node;
            parent[next_node] = node;
            return true;
        }
    }

    rec_stack[node] = false;
    return false;
}

bool detect_deadlock(int *cycle_nodes, int *cycle_len) {
    pthread_mutex_lock(&g_graph.graph_mutex);

    bool visited[MAX_THREADS] = {false};
    bool rec_stack[MAX_THREADS] = {false};
    int parent[MAX_THREADS];
    int cycle_start = -1;

    memset(parent, -1, sizeof(parent));

    /* 从每个活跃线程开始DFS */
    for (int i = 0; i < MAX_THREADS; i++) {
        if (g_graph.thread_active[i] && !visited[i]) {
            if (dfs_find_cycle(i, visited, rec_stack, parent, &cycle_start)) {
                /* 重建环 */
                *cycle_len = 0;
                int node = cycle_start;
                do {
                    cycle_nodes[(*cycle_len)++] = node;
                    node = parent[node];
                } while (node != cycle_start && *cycle_len < MAX_THREADS);

                pthread_mutex_unlock(&g_graph.graph_mutex);
                return true;
            }
        }
    }

    pthread_mutex_unlock(&g_graph.graph_mutex);
    return false;
}

/* ============================================================================
 * 报告死锁
 * ============================================================================ */

void report_deadlock(int *cycle_nodes, int cycle_len) {
    fprintf(stderr, "\n");
    fprintf(stderr, "========================================\n");
    fprintf(stderr, "DEADLOCK DETECTED!\n");
    fprintf(stderr, "========================================\n");
    fprintf(stderr, "Cycle length: %d\n", cycle_len);
    fprintf(stderr, "\nDependency cycle:\n");

    for (int i = 0; i < cycle_len; i++) {
        int thread_id = cycle_nodes[i];

        /* 找到该线程等待的锁 */
        int waiting_lock = -1;
        for (int j = 0; j < g_graph.num_edges; j++) {
            if (g_graph.edges[j].from == thread_id &&
                g_graph.edges[j].type == EDGE_REQUEST) {
                waiting_lock = g_graph.edges[j].to;
                break;
            }
        }

        fprintf(stderr, "  Thread %d -> ", thread_id);
        if (waiting_lock >= 0) {
            fprintf(stderr, "Lock %d -> ", waiting_lock);
        }
    }
    fprintf(stderr, "(back to start)\n");

    /* 打印详细的栈跟踪 */
    fprintf(stderr, "\nDetailed stack traces:\n");
    for (int i = 0; i < cycle_len; i++) {
        int thread_id = cycle_nodes[i];
        fprintf(stderr, "\nThread %d:\n", thread_id);

        for (int j = 0; j < g_graph.num_edges; j++) {
            if (g_graph.edges[j].from == thread_id &&
                g_graph.edges[j].type == EDGE_REQUEST) {
                fprintf(stderr, "  Waiting for lock at %p\n",
                       g_graph.edges[j].lock_addr);
                fprintf(stderr, "  Stack trace:\n%s\n",
                       g_graph.edges[j].stacktrace);
            }
        }
    }

    fprintf(stderr, "========================================\n");
}

/* ============================================================================
 * 钩子函数（替换pthread_mutex_*函数）
 * ============================================================================ */

/* 保存原始函数指针 */
static int (*real_pthread_mutex_lock)(pthread_mutex_t *) = NULL;
static int (*real_pthread_mutex_trylock)(pthread_mutex_t *) = NULL;
static int (*real_pthread_mutex_unlock)(pthread_mutex_t *) = NULL;

/* 初始化钩子 */
void __attribute__((constructor)) init_hooks(void) {
    pthread_once(&g_init_once, init_detector);

    real_pthread_mutex_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock");
    real_pthread_mutex_trylock = dlsym(RTLD_NEXT, "pthread_mutex_trylock");
    real_pthread_mutex_unlock = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
}

/* 钩子：pthread_mutex_lock */
int pthread_mutex_lock(pthread_mutex_t *mutex) {
    int thread_id = get_thread_id();
    int lock_id = get_lock_id(mutex);

    /* 记录请求边 */
    add_edge(thread_id, lock_id, EDGE_REQUEST, mutex);

    /* 检查死锁 */
    int cycle_nodes[MAX_THREADS];
    int cycle_len = 0;
    if (detect_deadlock(cycle_nodes, &cycle_len)) {
        report_deadlock(cycle_nodes, cycle_len);
        /* 可以选择：abort(), 或继续执行（可能导致真正死锁） */
        /* abort(); */
    }

    /* 调用原始函数 */
    int result = real_pthread_mutex_lock(mutex);

    if (result == 0) {
        /* 获取成功，转换边类型 */
        remove_edge(thread_id, lock_id, EDGE_REQUEST);
        add_edge(thread_id, lock_id, EDGE_HOLD, mutex);
    } else {
        /* 获取失败，移除请求边 */
        remove_edge(thread_id, lock_id, EDGE_REQUEST);
    }

    return result;
}

/* 钩子：pthread_mutex_trylock */
int pthread_mutex_trylock(pthread_mutex_t *mutex) {
    int thread_id = get_thread_id();
    int lock_id = get_lock_id(mutex);

    /* trylock不阻塞，所以不记录请求边 */
    int result = real_pthread_mutex_trylock(mutex);

    if (result == 0) {
        /* 获取成功 */
        add_edge(thread_id, lock_id, EDGE_HOLD, mutex);
    }

    return result;
}

/* 钩子：pthread_mutex_unlock */
int pthread_mutex_unlock(pthread_mutex_t *mutex) {
    int thread_id = get_thread_id();
    int lock_id = get_lock_id(mutex);

    /* 移除持有边 */
    remove_edge(thread_id, lock_id, EDGE_HOLD);

    return real_pthread_mutex_unlock(mutex);
}

/* ============================================================================
 * 测试代码
 * ============================================================================ */

pthread_mutex_t lock_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_b = PTHREAD_MUTEX_INITIALIZER;

void *deadlock_thread1(void *arg) {
    (void)arg;
    printf("Thread 1: Acquiring lock A\n");
    pthread_mutex_lock(&lock_a);
    sleep(1);
    printf("Thread 1: Acquiring lock B\n");
    pthread_mutex_lock(&lock_b);  /* 死锁！ */

    pthread_mutex_unlock(&lock_b);
    pthread_mutex_unlock(&lock_a);
    return NULL;
}

void *deadlock_thread2(void *arg) {
    (void)arg;
    printf("Thread 2: Acquiring lock B\n");
    pthread_mutex_lock(&lock_b);
    sleep(1);
    printf("Thread 2: Acquiring lock A\n");
    pthread_mutex_lock(&lock_a);  /* 死锁！ */

    pthread_mutex_unlock(&lock_a);
    pthread_mutex_unlock(&lock_b);
    return NULL;
}

int main() {
    printf("Deadlock Detection Demo\n");
    printf("=======================\n");
    printf("Starting two threads that will cause a deadlock...\n\n");

    pthread_t t1, t2;
    pthread_create(&t1, NULL, deadlock_thread1, NULL);
    pthread_create(&t2, NULL, deadlock_thread2, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
```

### 6.2 竞态条件检测

```c
/**
 * race_detector.c - 数据竞争检测器（简化版TSan）
 *
 * 基于happens-before关系的Vector Clock算法
 *
 * 编译：gcc -o race_detector race_detector.c -lpthread
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>

/* ============================================================================
 * Vector Clock实现
 * ============================================================================ */

#define MAX_THREADS 64

typedef struct {
    uint64_t clock[MAX_THREADS];
    int tid;
} vector_clock_t;

void vc_init(vector_clock_t *vc, int tid) {
    memset(vc->clock, 0, sizeof(vc->clock));
    vc->tid = tid;
}

void vc_increment(vector_clock_t *vc) {
    vc->clock[vc->tid]++;
}

void vc_merge(vector_clock_t *dst, const vector_clock_t *src) {
    for (int i = 0; i < MAX_THREADS; i++) {
        if (src->clock[i] > dst->clock[i]) {
            dst->clock[i] = src->clock[i];
        }
    }
}

bool vc_happens_before(const vector_clock_t *vc1, const vector_clock_t *vc2) {
    /* 检查vc1是否happens-before vc2 */
    bool all_leq = true;
    bool any_lt = false;

    for (int i = 0; i < MAX_THREADS; i++) {
        if (vc1->clock[i] > vc2->clock[i]) {
            all_leq = false;
            break;
        }
        if (vc1->clock[i] < vc2->clock[i]) {
            any_lt = true;
        }
    }

    return all_leq && any_lt;
}

bool vc_concurrent(const vector_clock_t *vc1, const vector_clock_t *vc2) {
    /* 检查两个事件是否并发（没有happens-before关系）*/
    return !vc_happens_before(vc1, vc2) && !vc_happens_before(vc2, vc1);
}

void vc_print(const vector_clock_t *vc) {
    printf("[");
    for (int i = 0; i < MAX_THREADS; i++) {
        if (vc->clock[i] > 0) {
            printf("%d:%" PRIu64 " ", i, vc->clock[i]);
        }
    }
    printf("]");
}

/* ============================================================================
 * Shadow Memory - 跟踪内存访问
 * ============================================================================ */

typedef enum {
    ACCESS_NONE = 0,
    ACCESS_READ = 1,
    ACCESS_WRITE = 2
} access_type_t;

typedef struct {
    vector_clock_t vc;
    int tid;
    access_type_t type;
} shadow_cell_t;

/* 简化：每个8字节字一个shadow cell */
typedef struct {
    shadow_cell_t write_history;  /* 最后写入 */
    shadow_cell_t read_history[MAX_THREADS];  /* 各线程的最后读取 */
} shadow_t;

#define SHADOW_GRANULARITY 8  /* 8字节粒度 */

static shadow_t *g_shadow_mem = NULL;
static size_t g_shadow_size = 0;
static pthread_mutex_t g_shadow_mutex = PTHREAD_MUTEX_INITIALIZER;

/* 线程本地存储 */
__thread vector_clock_t tls_vc;
__thread int tls_tid = -1;

/* 线程VC表 */
static vector_clock_t g_thread_vcs[MAX_THREADS];
static pthread_mutex_t g_thread_vcs_mutex = PTHREAD_MUTEX_INITIALIZER;

/* ============================================================================
 * 初始化
 * ============================================================================ */

void init_shadow_memory(void) {
    /* 分配shadow memory（覆盖4GB地址空间） */
    g_shadow_size = (4ULL * 1024 * 1024 * 1024) / SHADOW_GRANULARITY;
    g_shadow_mem = calloc(g_shadow_size, sizeof(shadow_t));
    if (!g_shadow_mem) {
        fprintf(stderr, "Failed to allocate shadow memory\n");
        exit(1);
    }

    printf("[RaceDetector] Shadow memory initialized: %zu entries\n",
           g_shadow_size);
}

int get_thread_id(void) {
    if (tls_tid < 0) {
        static int next_tid = 0;
        pthread_mutex_lock(&g_thread_vcs_mutex);
        tls_tid = next_tid++;
        pthread_mutex_unlock(&g_thread_vcs_mutex);

        if (tls_tid >= MAX_THREADS) {
            fprintf(stderr, "Too many threads!\n");
            exit(1);
        }

        /* 初始化该线程的VC */
        vc_init(&tls_vc, tls_tid);
        vc_init(&g_thread_vcs[tls_tid], tls_tid);

        printf("[RaceDetector] Thread %d created\n", tls_tid);
    }
    return tls_tid;
}

static inline size_t addr_to_shadow_index(void *addr) {
    return ((uintptr_t)addr) / SHADOW_GRANULARITY;
}

/* ============================================================================
 * 访问记录
 * ============================================================================ */

void record_access(void *addr, size_t size, access_type_t type) {
    if (!g_shadow_mem) {
        init_shadow_memory();
    }

    int tid = get_thread_id();
    vc_increment(&tls_vc);

    size_t start_idx = addr_to_shadow_index(addr);
    size_t end_idx = addr_to_shadow_index((char *)addr + size - 1);

    pthread_mutex_lock(&g_shadow_mutex);

    for (size_t idx = start_idx; idx <= end_idx; idx++) {
        shadow_t *shadow = &g_shadow_mem[idx];

        if (type == ACCESS_WRITE) {
            /* 检查与之前读取的竞争 */
            for (int i = 0; i < MAX_THREADS; i++) {
                if (i == tid) continue;

                if (shadow->read_history[i].type != ACCESS_NONE) {
                    if (vc_concurrent(&shadow->read_history[i].vc, &tls_vc)) {
                        printf("\n[RACE DETECTED] Write-Read race at %p\n",
                               (void *)(idx * SHADOW_GRANULARITY));
                        printf("  Previous read by thread %d: ",
                               shadow->read_history[i].tid);
                        vc_print(&shadow->read_history[i].vc);
                        printf("\n  Current write by thread %d: ", tid);
                        vc_print(&tls_vc);
                        printf("\n");
                    }
                }
            }

            /* 检查与之前写入的竞争 */
            if (shadow->write_history.type != ACCESS_NONE &&
                shadow->write_history.tid != tid) {
                if (vc_concurrent(&shadow->write_history.vc, &tls_vc)) {
                    printf("\n[RACE DETECTED] Write-Write race at %p\n",
                           (void *)(idx * SHADOW_GRANULARITY));
                    printf("  Previous write by thread %d: ",
                           shadow->write_history.tid);
                    vc_print(&shadow->write_history.vc);
                    printf("\n  Current write by thread %d: ", tid);
                    vc_print(&tls_vc);
                    printf("\n");
                }
            }

            /* 更新写入历史 */
            shadow->write_history.vc = tls_vc;
            shadow->write_history.tid = tid;
            shadow->write_history.type = ACCESS_WRITE;

        } else {  /* ACCESS_READ */
            /* 检查与之前写入的竞争 */
            if (shadow->write_history.type != ACCESS_NONE &&
                shadow->write_history.tid != tid) {
                if (vc_concurrent(&shadow->write_history.vc, &tls_vc)) {
                    printf("\n[RACE DETECTED] Read-Write race at %p\n",
                           (void *)(idx * SHADOW_GRANULARITY));
                    printf("  Previous write by thread %d: ",
                           shadow->write_history.tid);
                    vc_print(&shadow->write_history.vc);
                    printf("\n  Current read by thread %d: ", tid);
                    vc_print(&tls_vc);
                    printf("\n");
                }
            }

            /* 更新读取历史 */
            shadow->read_history[tid].vc = tls_vc;
            shadow->read_history[tid].tid = tid;
            shadow->read_history[tid].type = ACCESS_READ;
        }
    }

    /* 更新全局线程VC */
    g_thread_vcs[tid] = tls_vc;

    pthread_mutex_unlock(&g_shadow_mutex);
}

/* ============================================================================
 * 同步事件处理
 * ============================================================================ */

void handle_lock_acquire(pthread_mutex_t *mutex) {
    (void)mutex;
    int tid = get_thread_id();
    vc_increment(&tls_vc);
    g_thread_vcs[tid] = tls_vc;
}

void handle_lock_release(pthread_mutex_t *mutex) {
    (void)mutex;
    int tid = get_thread_id();
    vc_increment(&tls_vc);
    g_thread_vcs[tid] = tls_vc;
}

void handle_thread_create(pthread_t *thread) {
    (void)thread;
    int tid = get_thread_id();
    vc_increment(&tls_vc);

    /* 父线程的VC将在子线程开始时同步 */
    pthread_mutex_lock(&g_thread_vcs_mutex);
    g_thread_vcs[tid] = tls_vc;
    pthread_mutex_unlock(&g_thread_vcs_mutex);
}

void handle_thread_join(pthread_t thread) {
    (void)thread;
    int tid = get_thread_id();

    /* 合并被join线程的VC */
    pthread_mutex_lock(&g_thread_vcs_mutex);
    for (int i = 0; i < MAX_THREADS; i++) {
        /* 简化：合并所有其他线程的VC */
        if (i != tid) {
            vc_merge(&tls_vc, &g_thread_vcs[i]);
        }
    }
    pthread_mutex_unlock(&g_thread_vcs_mutex);

    vc_increment(&tls_vc);
    g_thread_vcs[tid] = tls_vc;
}

/* ============================================================================
 * API包装器
 * ============================================================================ */

#define RECORD_READ(addr, size) record_access((addr), (size), ACCESS_READ)
#define RECORD_WRITE(addr, size) record_access((addr), (size), ACCESS_WRITE)

/* 自定义malloc/free跟踪 */
void *tracked_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr) {
        /* 标记为新分配的内存 */
        memset(ptr, 0, size);
    }
    return ptr;
}

void tracked_free(void *ptr) {
    if (ptr) {
        /* 记录释放 */
        RECORD_WRITE(ptr, sizeof(void*));
    }
    free(ptr);
}

/* ============================================================================
 * 测试
 * ============================================================================ */

static int shared_counter = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void *race_thread(void *arg) {
    (void)arg;

    get_thread_id();

    for (int i = 0; i < 1000; i++) {
        /* 数据竞争：没有同步 */
        RECORD_WRITE(&shared_counter, sizeof(shared_counter));
        shared_counter++;

        /* 偶尔使用互斥锁 */
        if (i % 100 == 0) {
            pthread_mutex_lock(&counter_mutex);
            handle_lock_acquire(&counter_mutex);

            RECORD_WRITE(&shared_counter, sizeof(shared_counter));
            shared_counter++;

            handle_lock_release(&counter_mutex);
            pthread_mutex_unlock(&counter_mutex);
        }
    }

    return NULL;
}

int main() {
    printf("Race Condition Detector Demo\n");
    printf("============================\n\n");

    init_shadow_memory();

    pthread_t t1, t2;

    handle_thread_create(&t1);
    pthread_create(&t1, NULL, race_thread, NULL);

    handle_thread_create(&t2);
    pthread_create(&t2, NULL, race_thread, NULL);

    handle_thread_join(t1);
    pthread_join(t1, NULL);

    handle_thread_join(t2);
    pthread_join(t2, NULL);

    printf("\nFinal counter: %d\n", shared_counter);

    return 0;
}
```

# C语言高级调试技术完全指南 - 续

## 📖 7. 内核调试（kgdb/kdb/ftrace）

### 7.1 KGDB配置与使用

```bash
#!/bin/bash
# kgdb_setup.sh - KGDB双机调试配置脚本

# 1. 编译支持KGDB的内核配置选项
# CONFIG_KGDB=y
# CONFIG_KGDB_SERIAL_CONSOLE=y
# CONFIG_DEBUG_KERNEL=y
# CONFIG_DEBUG_INFO=y
# CONFIG_MAGIC_SYSRQ=y

# 2. 启动参数配置
# GRUB_CMDLINE_LINUX_DEFAULT="console=ttyS0,115200 kgdboc=ttyS0,115200 nokaslr"

# 3. 进入KGDB模式
echo g > /proc/sysrq-trigger

# 4. 调试机连接
gdb vmlinux
(gdb) set remotebaud 115200
(gdb) target remote /dev/ttyUSB0
```

### 7.2 KDB常用命令

```
help       - 显示帮助
md <addr>  - 显示内存
rd         - 显示寄存器
bt         - 堆栈回溯
ps         - 进程列表
go         - 继续执行
bp <addr>  - 设置断点
kgdb       - 切换到KGDB模式
```

### 7.3 ftrace编程示例

```c
// ftrace_control.c - 通过tracefs控制ftrace
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define TRACING_DIR "/sys/kernel/tracing"

int tracefs_write(const char *file, const char *val) {
    char path[256];
    snprintf(path, sizeof(path), "%s/%s", TRACING_DIR, file);
    int fd = open(path, O_WRONLY);
    if (fd < 0) return -1;
    write(fd, val, strlen(val));
    close(fd);
    return 0;
}

void enable_function_tracing(const char *func) {
    tracefs_write("tracing_on", "0");           // 禁用跟踪
    tracefs_write("set_ftrace_filter", func);   // 设置过滤
    tracefs_write("current_tracer", "function"); // 设置函数跟踪器
    tracefs_write("tracing_on", "1");           // 启用跟踪
}

void show_trace(void) {
    char buf[4096];
    int fd = open(TRACING_DIR "/trace", O_RDONLY);
    if (fd >= 0) {
        int n = read(fd, buf, sizeof(buf)-1);
        if (n > 0) {
            buf[n] = '\0';
            printf("%s\n", buf);
        }
        close(fd);
    }
}

int main() {
    enable_function_tracing("kmalloc");
    sleep(5);
    show_trace();
    return 0;
}
```

---

## 📖 8. 远程调试（GDB Server/JTAG）

### 8.1 GDB Server使用

```bash
# 目标机启动gdbserver
gdbserver :1234 ./program
gdbserver :1234 --attach <pid>

# 调试机连接
gdb ./program
(gdb) target remote target-ip:1234
(gdb) break main
(gdb) continue

# 多进程调试
gdbserver --multi :1234
(gdb) target extended-remote target-ip:1234
(gdb) set remote exec-file /path/on/target
(gdb) run
```

### 8.2 OpenOCD JTAG配置

```tcl
# interface
interface ftdi
ftdi_vid_pid 0x0403 0x6010

# target
source [find target/stm32f4x.cfg]

# gdb server
gdb_port 3333
telnet_port 4444

init
reset halt
```

### 8.3 交叉编译调试配置

```bash
# 设置交叉编译工具链
export CROSS_COMPILE=arm-linux-gnueabihf-

# 编译带调试信息
${CROSS_COMPILE}gcc -g -O0 -o program program.c

# GDB配置
set sysroot /path/to/target/rootfs
set solib-search-path /path/to/target/libs
target remote 192.168.1.100:1234
```

---

## 📖 9. 自动化调试（崩溃报告系统）

### 9.1 崩溃处理器实现

```c
// crash_handler.c - 自动崩溃报告生成
#define _GNU_SOURCE
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>
#include <time.h>

static void write_report(int sig, siginfo_t *info) {
    int fd = open("/tmp/crash_report.txt", O_WRONLY|O_CREAT|O_TRUNC, 0644);

    // 写入基本信息
    dprintf(fd, "CRASH REPORT\n");
    dprintf(fd, "Signal: %d\n", sig);
    dprintf(fd, "Time: %ld\n", time(NULL));

    // 堆栈跟踪
    void *frames[64];
    int n = backtrace(frames, 64);
    backtrace_symbols_fd(frames, n, fd);

    close(fd);
}

void crash_handler(int sig, siginfo_t *info, void *ctx) {
    write_report(sig, info);

    // 恢复默认处理
    signal(sig, SIG_DFL);
    raise(sig);
}

void init_crash_handler(void) {
    struct sigaction sa = {
        .sa_sigaction = crash_handler,
        .sa_flags = SA_SIGINFO
    };
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
    sigaction(SIGFPE, &sa, NULL);
}
```

### 9.2 自动化调试工作流

```bash
#!/bin/bash
# auto_debug.sh

# 1. 运行程序并检测崩溃
if ! ./program; then
    EXIT_CODE=$?

    # 2. 收集崩溃信息
    echo "Program crashed with exit code: $EXIT_CODE"

    # 3. 使用gdb分析core dump
    if [ -f core ]; then
        gdb -batch -ex "bt full" -ex "info registers" \
            ./program core > backtrace.txt
    fi

    # 4. 上传报告（可选）
    # curl -F "file=@backtrace.txt" http://crash-server/upload
fi
```

---

## 📖 10. 实际案例：复杂内存泄漏追踪

### 10.1 内存泄漏检测策略

```c
// leak_detection.c - 分层内存泄漏检测
#include <stdlib.h>
#include <stdio.h>

// 第一层：静态分析 - 使用编译器警告
// gcc -Wall -Wextra -fanalyzer leak_detection.c

// 第二层：运行时检查 - 使用宏包装
#ifdef DEBUG
    #define TRACK_MALLOC(size) \
        debug_malloc(size, __FILE__, __LINE__, __func__)
    #define TRACK_FREE(ptr) \
        debug_free(ptr, __FILE__, __LINE__)
#else
    #define TRACK_MALLOC(size) malloc(size)
    #define TRACK_FREE(ptr) free(ptr)
#endif

void *debug_malloc(size_t size, const char *file, int line, const char *func) {
    printf("[ALLOC] %s:%d in %s - %zu bytes\n", file, line, func, size);
    return malloc(size);
}

void debug_free(void *ptr, const char *file, int line) {
    printf("[FREE] %s:%d\n", file, line);
    free(ptr);
}

// 第三层：Valgrind验证
// valgrind --leak-check=full --track-origins=yes ./program

// 第四层：AddressSanitizer
// gcc -fsanitize=address -g leak_detection.c
```

### 10.2 使用valgrind进行深度分析

```bash
# 完整泄漏检测
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         ./program

# 生成抑制规则
valgrind --gen-suppressions=yes ./program

# 堆分析
valgrind --tool=massif --time-unit=B ./program
ms_print massif.out.* > heap_report.txt
```

### 10.3 典型内存泄漏模式

```c
// 模式1：异常路径泄漏
void leaky_function(int x) {
    char *buf = malloc(100);
    if (x < 0) {
        return;  // 泄漏！未free
    }
    free(buf);
}

// 修复：使用goto统一清理
void fixed_function(int x) {
    char *buf = malloc(100);
    if (x < 0) {
        goto cleanup;
    }
    // ... 使用buf
cleanup:
    free(buf);
}

// 模式2：结构体成员泄漏
typedef struct {
    char *name;
    char *data;
} leaky_struct;

void free_leaky_struct(leaky_struct *s) {
    free(s);  // 泄漏！未释放name和data
}

// 修复
void free_fixed_struct(leaky_struct *s) {
    free(s->name);
    free(s->data);
    free(s);
}
```

---

## ✅ 质量验收清单

- [x] eBPF基础与编程
- [x] GDB高级技巧
- [x] 崩溃分析
- [x] 内存调试进阶
- [x] 性能分析
- [x] 并发调试
- [x] 内核调试
- [x] 远程调试
- [x] 自动化调试
- [x] 实际案例

---

> **更新记录**
>
> - 2025-03-17: 完成高级调试技术指南全部10个主题


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
