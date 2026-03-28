# BPF性能工具深度指南

> **层级定位**: 03_System_Technology_Domains / 18_Systems_Performance
> **难度级别**: L4 系统层
> **对应标准**: Linux Kernel BPF, eBPF
> **最后更新**: 2026-03-28

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | BPF/eBPF架构、BPF程序类型、性能分析、跟踪工具 |
| **前置知识** | [Linux内核](../../16_OS_Kernel/)、[系统编程](../../01_System_Programming/) |
| **后续延伸** | [内核调试](../../13_Linux_Kernel/02_Kernel_Debugging_Deep.md)、[网络编程](../../15_Network_Programming/) |
| **横向关联** | [eBPF深度](../../01_System_Programming/06_eBPF_Deep.md)、[性能调优](01_Linux_Performance_Tuning.md) |
| **权威来源** | Linux Kernel BPF文档, Brendan Gregg's BPF book |

---

## 📑 目录

- [BPF性能工具深度指南](#bpf性能工具深度指南)
  - [📋 本节概要](#-本节概要)
  - [📑 目录](#-目录)
  - [🎯 BPF概述](#-bpf概述)
    - [什么是BPF？](#什么是bpf)
    - [BPF vs 传统工具](#bpf-vs-传统工具)
  - [🏗️ BPF架构](#️-bpf架构)
    - [整体架构](#整体架构)
    - [BPF程序类型](#bpf程序类型)
  - [🔧 BCC工具集](#-bcc工具集)
    - [BCC简介](#bcc简介)
    - [常用BCC工具示例](#常用bcc工具示例)
    - [编写BCC程序](#编写bcc程序)
  - [⚡ bpftrace](#-bpftrace)
    - [bpftrace简介](#bpftrace简介)
    - [bpftrace语法](#bpftrace语法)
    - [bpftrace脚本示例](#bpftrace脚本示例)
  - [📊 实战案例](#-实战案例)
    - [案例1：分析系统调用热点](#案例1分析系统调用热点)
    - [案例2：跟踪TCP连接问题](#案例2跟踪tcp连接问题)
    - [案例3：内存泄漏检测](#案例3内存泄漏检测)
  - [🔬 深入理解](#-深入理解)
    - [BPF验证器](#bpf验证器)
    - [BPF Maps](#bpf-maps)
  - [✅ 质量验收清单](#-质量验收清单)
  - [📚 相关资源](#-相关资源)
    - [官方文档](#官方文档)
    - [推荐书籍](#推荐书籍)
    - [工具](#工具)

---

## 🎯 BPF概述

### 什么是BPF？

**BPF**（Berkeley Packet Filter）最初是为网络包过滤设计的，后演进为**eBPF**（extended BPF），成为Linux内核中通用的执行引擎。

```
BPF演进历程：

1992: cBPF (classic BPF)
   ├── 网络包过滤
   └── 简单指令集

2014: eBPF (extended BPF)
   ├── 扩展到通用执行引擎
   ├── 10+寄存器
   ├── 无限制指令数
   ├── 调用内核函数
   └── JIT编译

2020+: eBPF生态系统
   ├── 网络（XDP, TC）
   ├── 跟踪（kprobe, tracepoint）
   ├── 安全（LSM, seccomp）
   └── 内核可编程
```

### BPF vs 传统工具

| 特性 | 传统工具（strace, tcpdump） | BPF工具 |
|:-----|:---------------------------|:--------|
| **开销** | 高（上下文切换） | 极低（内核执行） |
| **灵活性** | 固定功能 | 可编程 |
| **实时性** | 延迟较高 | 纳秒级 |
| **安全性** | 依赖外部程序 | 内核验证器保证 |
| **生产环境** | 谨慎使用 | 安全可靠 |

---

## 🏗️ BPF架构

### 整体架构

```
用户空间                        内核空间
─────────                      ─────────

BPF程序(C/LLVM)                 BPF验证器
      │                              │
      ▼                              ▼
LLVM编译 ──→ BPF字节码 ──→ 验证 ──→ JIT编译
                                          │
                                    ┌─────┴─────┐
                                    ▼           ▼
                              BPF运行时     辅助函数
                                    │
                              ┌─────┴─────┐
                              ▼           ▼
                          映射(Maps)   程序附加点
                              │           │
                        用户态读取    kprobe/tracepoint
```

### BPF程序类型

| 类型 | 用途 | 触发点 |
|:-----|:-----|:-------|
| **kprobe** | 内核函数跟踪 | 任意内核函数入口 |
| **kretprobe** | 内核函数返回跟踪 | 内核函数返回 |
| **tracepoint** | 静态跟踪点 | 预定义内核事件 |
| **uprobe** | 用户态程序跟踪 | 用户程序函数 |
| **xdp** | 网络包处理 | 网卡驱动层 |
| **tc** | 流量控制 | 网络栈 |
| **cgroup/skb** | cgroup网络过滤 | 网络包 |
| **perf_event** | 硬件性能事件 | PMU事件 |

---

## 🔧 BCC工具集

### BCC简介

**BCC**（BPF Compiler Collection）是一个使用eBPF进行跟踪和监控的工具集。

```bash
# 安装BCC
sudo apt-get install bpfcc-tools linux-headers-$(uname -r)

# 常用BCC工具
/usr/share/bcc/tools/
├── biosnoop      # 块设备I/O跟踪
├── biolatency    # 块设备I/O延迟直方图
├── execsnoop     # 进程执行跟踪
├── opensnoop     # 文件打开跟踪
├── funccount     # 函数调用计数
├── funclatency   # 函数延迟
├── stackcount    # 内核栈计数
├── trace         # 通用跟踪
└── ...
```

### 常用BCC工具示例

```bash
# 1. 查看进程执行（排查启动问题）
sudo execsnoop

# 输出示例：
# PCOMM            PID    PPID   RET ARGS
# bash             12345  100    0   /bin/bash script.sh
# python3          12346  12345  0   /usr/bin/python3 app.py

# 2. 查看文件打开（诊断文件访问问题）
sudo opensnoop -p 1234

# 3. 块设备I/O延迟直方图
sudo biolatency -D

# 输出示例：
# usecs               : count     distribution
# 0 -> 1          : 3        |                                      |
# 2 -> 3          : 0        |                                      |
# 4 -> 7          : 0        |                                      |
# 8 -> 15         : 1212     |************************************* |

# 4. 跟踪TCP连接建立
sudo tcplife

# 输出：PID COMM LADDR LPORT RADDR RPORT TX_KB RX_KB MS
```

### 编写BCC程序

```python
#!/usr/bin/env python3
# filetrace.py - 跟踪文件访问

from bcc import BPF

# BPF程序代码
prog = """
#include <uapi/linux/ptrace.h>
#include <linux/fs.h>

struct data_t {
    u32 pid;
    char comm[16];
    char fname[256];
};

BPF_PERF_OUTPUT(events);

int trace_open(struct pt_regs *ctx, struct file *file) {
    struct data_t data = {};

    data.pid = bpf_get_current_pid_tgid() >> 32;
    bpf_get_current_comm(&data.comm, sizeof(data.comm));

    // 读取文件名
    bpf_probe_read_kernel_str(&data.fname, sizeof(data.fname),
                               file->f_path.dentry->d_name.name);

    events.perf_submit(ctx, &data, sizeof(data));
    return 0;
}
"""

# 加载BPF程序
b = BPF(text=prog)
b.attach_kprobe(event="vfs_open", fn_name="trace_open")

# 打印输出
print("%s %6s %16s %s" % ("TIME", "PID", "COMM", "FILE"))
while True:
    try:
        (task, pid, cpu, flags, ts, msg) = b.perf_buffer_poll()
    except KeyboardInterrupt:
        break
```

---

## ⚡ bpftrace

### bpftrace简介

**bpftrace**是一个高级BPF跟踪语言，语法类似awk和DTrace。

```bash
# 安装
sudo apt-get install bpftrace

# 一行命令跟踪
sudo bpftrace -e 'kprobe:do_nanosleep { printf("sleep by %s\n", comm); }'
```

### bpftrace语法

```bash
# 基本语法结构
probe /filter/ { action }

# 1. 跟踪系统调用
sudo bpftrace -e 'tracepoint:raw_syscalls:sys_enter { @[comm] = count(); }'

# 2. 函数延迟直方图
sudo bpftrace -e 'kprobe:do_sys_open { @start[tid] = nsecs; }
                  kretprobe:do_sys_open /@start[tid]/
                  { @us = hist((nsecs - @start[tid]) / 1000); delete(@start[tid]); }'

# 3. 跟踪特定进程的内存分配
sudo bpftrace -e 'uprobe:/lib/x86_64-linux-gnu/libc.so.6:malloc /pid == 1234/
                  { printf("malloc(%d)\n", arg0); }'

# 4. 堆栈跟踪
sudo bpftrace -e 'kprobe:__kmalloc { @[kstack] = count(); }'
```

### bpftrace脚本示例

```bash
#!/usr/bin/bpftrace

# filewatch.bt - 监控文件系统操作

#include <linux/fs.h>

BEGIN {
    printf("Tracing file operations... Hit Ctrl-C to end.\n");
    printf("%-8s %-6s %-16s %6s %s\n", "TIME", "PID", "COMM", "OP", "FILE");
}

kprobe:vfs_read,
kprobe:vfs_write {
    $file = (struct file *)arg0;
    $dentry = $file->f_path.dentry;

    time("%H:%M:%S ");
    printf("%-6d %-16s %6s ", pid, comm, probe == "kprobe:vfs_read" ? "READ" : "WRITE");
    printf("%s\n", str($dentry->d_name.name));
}

END {
    printf("\nDone.\n");
}
```

---

## 📊 实战案例

### 案例1：分析系统调用热点

```bash
# 统计各进程的系统调用次数
sudo bpftrace -e 'tracepoint:raw_syscalls:sys_enter { @[comm] = count(); }'

# 输出示例：
# @[sshd]: 1234
# @[bash]: 5678
# @[python3]: 9012

# 查看特定进程的系统调用类型
sudo bpftrace -e 'tracepoint:raw_syscalls:sys_enter /comm == "python3"/
                  { @[args->id] = count(); }'
```

### 案例2：跟踪TCP连接问题

```bash
# 跟踪TCP连接生命周期
sudo bpftrace -e '
#include <net/sock.h>
#include <bcc/proto.h>

kprobe:tcp_v4_connect {
    @connect_start[nsecs] = tid;
}

kretprobe:tcp_v4_connect /@connect_start[nsecs]/ {
    @latency_us = hist((nsecs - @connect_start[nsecs]) / 1000);
    delete(@connect_start[nsecs]);
}
'
```

### 案例3：内存泄漏检测

```bash
# 跟踪内存分配和释放
sudo bpftrace -e '
uprobe:/lib/x86_64-linux-gnu/libc.so.6:malloc {
    @alloc[arg1] = tid;
}

uprobe:/lib/x86_64-linux-gnu/libc.so.6:free {
    delete(@alloc[arg0]);
}

END {
    printf("Possible memory leaks (tid -> address):\n");
    print(@alloc);
}
'
```

---

## 🔬 深入理解

### BPF验证器

```
BPF验证器安全检查：

1. 类型检查
   - 寄存器类型跟踪
   - 内存访问边界检查

2. 控制流检查
   - 无死循环（指令数限制）
   - 无不可达代码

3. 内存安全
   - 空指针检查
   - 栈边界检查
   - Map访问验证

4. 禁止操作
   - 无全局变量（除Maps）
   - 无递归调用
   - 无未初始化变量
```

### BPF Maps

```c
// BPF Map类型

// 1. Hash表（最常用的key-value存储）
struct bpf_map_def SEC("maps") my_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(u32),
    .value_size = sizeof(u64),
    .max_entries = 1024,
};

// 2. 数组
struct bpf_map_def SEC("maps") my_array = {
    .type = BPF_MAP_TYPE_ARRAY,
    .key_size = sizeof(u32),
    .value_size = sizeof(u64),
    .max_entries = 256,
};

// 3. 直方图（性能分析常用）
struct bpf_map_def SEC("maps") latency_hist = {
    .type = BPF_MAP_TYPE_HISTOGRAM,
    .key_size = sizeof(u64),
    .value_size = sizeof(u64),
    .max_entries = 64,
};

// 4. 栈跟踪
struct bpf_map_def SEC("maps") stack_traces = {
    .type = BPF_MAP_TYPE_STACK_TRACE,
    .key_size = sizeof(u32),
    .value_size = 127 * sizeof(u64),
    .max_entries = 1024,
};
```

---

## ✅ 质量验收清单

- [x] BPF概述与演进
- [x] BPF架构详解
- [x] BCC工具集介绍
- [x] bpftrace语法与示例
- [x] 实战案例（热点分析、TCP跟踪、内存泄漏）
- [x] BPF验证器原理
- [x] BPF Maps类型

---

## 📚 相关资源

### 官方文档

- **Linux Kernel BPF**: <https://www.kernel.org/doc/html/latest/bpf/>
- **BCC GitHub**: <https://github.com/iovisor/bcc>
- **bpftrace**: <https://github.com/iovisor/bpftrace>

### 推荐书籍

- 《BPF Performance Tools》 - Brendan Gregg
- 《Linux Observability with BPF》 - David Calavera

### 工具

- **bpftool**: 内核自带的BPF工具
- **libbpf-bootstrap**: BPF程序开发脚手架

---

> **最后更新**: 2026-03-28
> **版本**: 1.0 - 完整版
> **维护者**: C_Lang Knowledge Base Team
