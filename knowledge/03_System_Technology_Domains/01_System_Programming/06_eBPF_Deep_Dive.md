# eBPF 深度实战：从XDP到CO-RE的可编译完整指南

> **标准**: eBPF (extended Berkeley Packet Filter), Linux Kernel 5.x-6.x, BCC/libbpf, CO-RE  
> **前置知识**: [系统编程基础](README.md), [内核基础](../../04_Industrial_Scenarios/13_Linux_Kernel/README.md)  
> **后续延伸**: [性能分析工具](../34_Systems_Performance/02_BPF_Performance_Tools.md), [网络安全](../25_Security/06_Memory_Security_Deep_Dive.md)  
> **对标资源**: Linux Kernel BPF文档, Brendan Gregg《BPF Performance Tools》, Cilium eBPF库  
> **难度**: 5/5 | **预计学习时间**: 8-10 小时

---

## 目录

1. [eBPF架构概述](#一架构概述)
2. [开发环境搭建](#二环境搭建)
3. [最小eBPF程序：Hello World](#三hello-world)
4. [XDP防火墙](#四xdp防火墙)
5. [kprobe函数追踪](#五kprobe追踪)
6. [BPF Maps与数据共享](#六bpf-maps)
7. [CO-RE：编译一次，到处运行](#七core)
8. [高级主题](#八高级主题)
9. [性能与安全](#九性能安全)
10. [参考资源](#十参考资源)

---

## 一、架构概述

### 1.1 eBPF是什么？

eBPF（extended Berkeley Packet Filter）是Linux内核的**可编程虚拟机**，允许在用户态编写小程序，经验证后**安全地运行在内核态**。

```
用户空间                              内核空间
┌─────────────┐                     ┌─────────────┐
│  C程序源码   │  ──► clang/llvm    │  eBPF验证器   │
│  (BPF C)    │      -target bpf   │  (安全检查)   │
└─────────────┘                     ├─────────────┤
        │                           │  eBPF JIT编译 │
        ▼                           │  (x86/ARM64) │
┌─────────────┐                     ├─────────────┤
│  ELF对象文件 │  ──► libbpf加载    │  eBPF程序挂载 │
│  (.o)       │      bpf() syscall │  (kprobe/XDP/ │
└─────────────┘                     │   tracepoint) │
        │                           ├─────────────┤
        ▼                           │  BPF Maps     │
┌─────────────┐◄───────────────────►│  (共享内存)   │
│  用户态程序   │      读写Maps      │               │
│  (Go/C/Rust)│                     └─────────────┘
└─────────────┘
```

### 1.2 eBPF程序类型

| 程序类型 | 挂载点 | 用途 | 触发时机 |
|:---------|:-------|:-----|:---------|
| **XDP** | 网卡驱动 | 包过滤/转发 | 网卡收到包时 |
| **TC (Traffic Control)** | 网络栈 | QoS/整形/过滤 | 包进入/离开网络栈 |
| **kprobe/kretprobe** | 内核函数 | 函数追踪 | 内核函数调用/返回 |
| **tracepoint** | 内核静态追踪点 | 事件追踪 | 内核事件发生时 |
| **uprobe/uretprobe** | 用户态函数 | 用户态追踪 | 用户函数调用/返回 |
| **socket filter** | socket | 包过滤（tcpdump） | socket接收数据时 |
| **LSM** | LSM钩子 | 安全策略（容器安全） | 安全检查时 |
| **fentry/fexit** | 内核函数入口/出口 | 高性能追踪（内核5.5+） | 函数进入/退出 |

### 1.3 eBPF字节码限制

| 限制 | 说明 | 原因 |
|:-----|:-----|:-----|
| **指令数 ≤ 100万** | 程序最多100万条指令 | 防止无限循环 |
| **无循环（早期）** | 内核5.3+支持有界循环 | 确保可终止 |
| **无空指针解引用** | 验证器强制检查 | 内核安全 |
| **无越界访问** | 所有内存访问需验证 | 内核安全 |
| **无未初始化变量** | 所有变量使用前必须赋值 | 确定性执行 |
| **无调用外部函数** | 仅限BPF helper函数 | 隔离性 |
| **栈大小 ≤ 512B** | 局部变量限制 | 内核栈保护 |

---

## 二、环境搭建

### 2.1 依赖安装

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    clang llvm libelf-dev libbpf-dev bpfcc-tools \
    linux-headers-$(uname -r) build-essential git

# 安装libbpf（新版）
git clone https://github.com/libbpf/libbpf.git
cd libbpf/src
make
sudo make install

# 安装bpftool（BPF工具）
git clone --recurse-submodules https://github.com/libbpf/bpftool.git
cd bpftool/src
make
sudo make install
```

### 2.2 验证环境

```bash
# 检查内核BPF支持
ls /sys/kernel/debug/tracing/
cat /proc/sys/kernel/bpf_stats_enabled

# 检查BTF支持（CO-RE必需）
ls /sys/kernel/btf/vmlinux
bpftool btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h

# 检查XDP支持
ip link show | grep -i xdp
```

---

## 三、最小eBPF程序：Hello World

### 3.1 eBPF程序（内核态）

```c
// hello_kern.c — 内核态eBPF程序
#include "vmlinux.h"          // 内核类型定义（由bpftool生成）
#include <bpf/bpf_helpers.h>   // BPF helper函数
#include <bpf/bpf_tracing.h>   // fentry/fexit宏

char LICENSE[] SEC("license") = "Dual BSD/GPL";

// 定义一个简单的Map用于计数
struct {
    __uint(type, BPF_MAP_TYPE_ARRAY);
    __uint(max_entries, 1);
    __type(key, __u32);
    __type(value, __u64);
} counter SEC(".maps");

// 挂载到fentry: do_nanosleep（内核5.5+）
SEC("fentry/do_nanosleep")
int BPF_PROG(hello_world, struct hrtimer_sleeper *t, enum hrtimer_mode mode)
{
    __u32 key = 0;
    __u64 *val;
    
    // 从Map中读取当前计数
    val = bpf_map_lookup_elem(&counter, &key);
    if (val) {
        // 原子递增
        __sync_fetch_and_add(val, 1);
    }
    
    // 输出调试信息到 /sys/kernel/debug/tracing/trace_pipe
    bpf_printk("Hello eBPF! count=%llu", val ? *val : 0);
    
    return 0;
}
```

### 3.2 用户态加载程序（libbpf skeleton）

```c
// hello_user.c — 用户态加载程序
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "hello_kern.skel.h"  // 由bpftool自动生成

static volatile sig_atomic_t exiting = 0;

void sig_handler(int sig) {
    exiting = 1;
}

int main(int argc, char **argv) {
    struct hello_kern *skel;
    int err;
    
    // 设置信号处理
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    
    // 打开并加载BPF程序
    skel = hello_kern__open_and_load();
    if (!skel) {
        fprintf(stderr, "Failed to open and load BPF skeleton\n");
        return 1;
    }
    
    // 附加程序到内核钩子
    err = hello_kern__attach(skel);
    if (err) {
        fprintf(stderr, "Failed to attach BPF skeleton\n");
        goto cleanup;
    }
    
    printf("Successfully started! Run `sudo cat /sys/kernel/debug/tracing/trace_pipe` to see output.\n");
    
    // 主循环：读取Map并打印统计
    while (!exiting) {
        __u32 key = 0;
        __u64 count;
        
        // 从Map读取计数
        err = bpf_map__lookup_elem(skel->maps.counter, &key, sizeof(key),
                                    &count, sizeof(count), 0);
        if (err == 0) {
            printf("Nanosleep intercepted: %llu times\r", count);
            fflush(stdout);
        }
        
        sleep(1);
    }
    
    printf("\nExiting...\n");
    
cleanup:
    hello_kern__destroy(skel);
    return err < 0 ? -err : 0;
}
```

### 3.3 Makefile

```makefile
# Makefile
CLANG ?= clang
CC ?= gcc

ARCH := $(shell uname -m | sed 's/x86_64/x86/' | sed 's/aarch64/arm64/')

BPF_CFLAGS := -g -O2 -target bpf -D__TARGET_ARCH_$(ARCH)
BPF_CFLAGS += -I/usr/include/$(shell uname -m)-linux-gnu
BPF_CFLAGS += -I./libbpf/src

USER_CFLAGS := -g -O2 -Wall -I./libbpf/src
USER_LDFLAGS := -L./libbpf/src -lbpf -lelf -lz

.PHONY: all clean vmlinux

all: hello_user

# 生成vmlinux.h（需要root）
vmlinux:
	bpftool btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h

# 编译eBPF程序 → ELF对象
hello_kern.o: hello_kern.c vmlinux.h
	$(CLANG) $(BPF_CFLAGS) -c hello_kern.c -o hello_kern.o

# 生成skeleton（需要bpftool）
hello_kern.skel.h: hello_kern.o
	bpftool gen skeleton hello_kern.o > hello_kern.skel.h

# 编译用户态程序
hello_user: hello_user.c hello_kern.skel.h
	$(CC) $(USER_CFLAGS) -c hello_user.c -o hello_user.o
	$(CC) hello_user.o $(USER_LDFLAGS) -o hello_user

clean:
	rm -f *.o *.skel.h hello_user vmlinux.h
```

**运行**：
```bash
make
sudo ./hello_user
# 另一个终端
sudo cat /sys/kernel/debug/tracing/trace_pipe
```

---

## 四、XDP防火墙

### 4.1 XDP程序（包过滤）

```c
// xdp_firewall_kern.c
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>

#define ETH_P_IP 0x0800

char LICENSE[] SEC("license") = "GPL";

// 黑名单Map：IP -> 封禁标志
struct {
    __uint(type, BPF_MAP_TYPE_LRU_HASH);
    __uint(max_entries, 10000);
    __type(key, __u32);      // IPv4地址
    __type(value, __u8);     // 1=blocked
} blacklist SEC(".maps");

// 统计Map
struct {
    __uint(type, BPF_MAP_TYPE_PERCPU_ARRAY);
    __uint(max_entries, 3);
    __type(key, __u32);
    __type(value, __u64);
} stats SEC(".maps");

enum {
    STAT_TOTAL = 0,
    STAT_PASSED = 1,
    STAT_BLOCKED = 2,
};

static __always_inline void increment_stat(__u32 stat_idx) {
    __u64 *val = bpf_map_lookup_elem(&stats, &stat_idx);
    if (val) __sync_fetch_and_add(val, 1);
}

SEC("xdp")
int xdp_firewall(struct xdp_md *ctx) {
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;
    
    increment_stat(STAT_TOTAL);
    
    // 解析以太网头
    struct ethhdr *eth = data;
    if ((void *)(eth + 1) > data_end)
        return XDP_DROP;
    
    // 只处理IPv4
    if (bpf_ntohs(eth->h_proto) != ETH_P_IP)
        return XDP_PASS;
    
    // 解析IP头
    struct iphdr *ip = (void *)(eth + 1);
    if ((void *)(ip + 1) > data_end)
        return XDP_DROP;
    
    // 检查源IP是否在黑名单
    __u32 src_ip = ip->saddr;
    __u8 *blocked = bpf_map_lookup_elem(&blacklist, &src_ip);
    if (blocked && *blocked) {
        increment_stat(STAT_BLOCKED);
        return XDP_DROP;  // 丢弃黑名单IP的包
    }
    
    // 可选：检查TCP端口
    if (ip->protocol == IPPROTO_TCP) {
        struct tcphdr *tcp = (void *)ip + (ip->ihl * 4);
        if ((void *)(tcp + 1) <= data_end) {
            __u16 dport = bpf_ntohs(tcp->dest);
            if (dport == 22 || dport == 443) {
                increment_stat(STAT_PASSED);
                return XDP_PASS;  // 放行SSH和HTTPS
            }
        }
    }
    
    increment_stat(STAT_PASSED);
    return XDP_PASS;
}
```

### 4.2 用户态控制程序

```c
// xdp_firewall_user.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#include "xdp_firewall_kern.skel.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <ifname> [block_ip]\n", argv[0]);
        return 1;
    }
    
    const char *ifname = argv[1];
    unsigned int ifindex = if_nametoindex(ifname);
    if (!ifindex) {
        perror("if_nametoindex");
        return 1;
    }
    
    struct xdp_firewall_kern *skel = xdp_firewall_kern__open_and_load();
    if (!skel) {
        fprintf(stderr, "Failed to load XDP program\n");
        return 1;
    }
    
    // 附加XDP程序到网卡
    int prog_fd = bpf_program__fd(skel->progs.xdp_firewall);
    int err = bpf_xdp_attach(ifindex, prog_fd, XDP_FLAGS_DRV_MODE, NULL);
    if (err) {
        // 回退到generic模式
        err = bpf_xdp_attach(ifindex, prog_fd, XDP_FLAGS_SKB_MODE, NULL);
        if (err) {
            fprintf(stderr, "Failed to attach XDP: %d\n", err);
            goto cleanup;
        }
        printf("Attached XDP in SKB (generic) mode\n");
    } else {
        printf("Attached XDP in native (driver) mode\n");
    }
    
    // 添加黑名单IP
    if (argc >= 3) {
        __u32 ip;
        inet_pton(AF_INET, argv[2], &ip);
        __u8 flag = 1;
        bpf_map__update_elem(skel->maps.blacklist, &ip, sizeof(ip), &flag, sizeof(flag), BPF_ANY);
        printf("Blocked IP: %s\n", argv[2]);
    }
    
    printf("XDP firewall running on %s. Press Ctrl+C to stop.\n", ifname);
    
    // 打印统计
    while (1) {
        sleep(2);
        for (int i = 0; i < 3; i++) {
            __u32 key = i;
            __u64 val;
            if (bpf_map__lookup_elem(skel->maps.stats, &key, sizeof(key), &val, sizeof(val), 0) == 0) {
                const char *name[] = {"Total", "Passed", "Blocked"};
                printf("%s: %llu  ", name[i], val);
            }
        }
        printf("\n");
    }
    
cleanup:
    bpf_xdp_detach(ifindex, XDP_FLAGS_SKB_MODE, NULL);
    xdp_firewall_kern__destroy(skel);
    return 0;
}
```

---

## 五、CO-RE：编译一次，到处运行

### 5.1 什么是CO-RE？

**CO-RE**（Compile Once, Run Everywhere）是eBPF的**可移植性解决方案**：

**问题**：eBPF程序访问内核数据结构（如 `struct task_struct`），但不同内核版本的字段偏移可能不同。

**CO-RE解决方案**：
1. 编译时：使用BTF（BPF Type Format）记录字段信息
2. 加载时：libbpf根据目标内核的BTF自动重定位字段偏移

```c
// 非CO-RE（硬编码偏移，不可移植）
// struct task_struct { int pid; ... }; // pid在偏移16处
bpf_probe_read(&pid, sizeof(pid), (void *)task + 16);  // ❌ 内核版本不同则崩溃

// CO-RE（自动重定位）
bpf_core_read(&pid, sizeof(pid), &task->pid);  // ✅ 自动适应不同内核版本
```

### 5.2 CO-RE核心宏

```c
#include <bpf/bpf_core_read.h>

// 读取内核结构字段（自动重定位）
struct task_struct *task = bpf_get_current_task_btf();
int pid = BPF_CORE_READ(task, pid);                    // 读取task->pid
int ppid = BPF_CORE_READ(task, real_parent, tgid);     // 读取task->real_parent->tgid

// 检查字段是否存在
if (bpf_core_field_exists(task->new_field)) {
    // 新内核有该字段
    val = BPF_CORE_READ(task, new_field);
} else {
    // 旧内核无该字段，使用默认值
    val = 0;
}

// 检查类型大小
if (bpf_core_type_size(struct task_struct) == 10000) {
    // 特定内核版本的大小
}
```

### 5.3 生成CO-RE兼容的vmlinux.h

```bash
# 从目标内核生成vmlinux.h
bpftool btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h

# 编译时使用-g（生成BTF信息）
clang -g -O2 -target bpf -c prog.c -o prog.o

# 验证BTF信息
bpftool gen object prog.btf.o prog.o
bpftool btf dump file prog.btf.o
```

---

## 六、高级主题

### 6.1 BPF Tail Calls

```c
// 尾调用：程序A结束时跳转到程序B
// 用途：状态机、协议解析、超出指令限制

struct bpf_elf_map {
    __uint(type, BPF_MAP_TYPE_PROG_ARRAY);
    __uint(max_entries, 10);
    __type(key, __u32);
    __type(value, __u32);
} jmp_table SEC(".maps");

SEC("xdp")
int xdp_parser_stage1(struct xdp_md *ctx) {
    // 解析第一层协议
    __u32 proto = parse_l2(ctx);
    
    // 尾调用到对应的解析器
    bpf_tail_call(ctx, &jmp_table, proto);
    
    // 如果tail_call失败，继续执行
    return XDP_PASS;
}

SEC("xdp")
int xdp_parser_ipv4(struct xdp_md *ctx) {
    // IPv4解析
    return XDP_PASS;
}

SEC("xdp")
int xdp_parser_ipv6(struct xdp_md *ctx) {
    // IPv6解析
    return XDP_PASS;
}
```

### 6.2 BPF与Perf Events

```c
// 使用perf_event输出结构化数据到用户态
struct event {
    __u32 pid;
    __u32 uid;
    char comm[16];
    __u64 duration_ns;
};

struct {
    __uint(type, BPF_MAP_TYPE_PERF_EVENT_ARRAY);
    __uint(key_size, sizeof(__u32));
    __uint(value_size, sizeof(__u32));
} events SEC(".maps");

SEC("fexit/do_nanosleep")
int BPF_PROG(track_sleep_exit, struct hrtimer_sleeper *t, 
             enum hrtimer_mode mode, int ret) {
    struct event e = {};
    e.pid = bpf_get_current_pid_tgid() >> 32;
    e.uid = bpf_get_current_uid_gid() & 0xFFFFFFFF;
    bpf_get_current_comm(&e.comm, sizeof(e.comm));
    // 计算睡眠时长...
    
    bpf_perf_event_output(ctx, &events, BPF_F_CURRENT_CPU, &e, sizeof(e));
    return 0;
}
```

---

## 七、性能与安全

### 7.1 eBPF vs 内核模块对比

| 维度 | 内核模块 | eBPF |
|:-----|:---------|:-----|
| **安全性** | 可能崩溃内核 | 验证器保证安全 |
| **加载速度** | 慢（需insmod） | 快（bpf() syscall） |
| **动态更新** | 需重启/卸载 | 原子替换（无停机） |
| **性能** | 原生 | JIT后接近原生 |
| **开发难度** | 高（内核编程） | 中（受限C） |
| **可移植性** | 差（内核版本绑定） | 好（CO-RE） |
| **生态** | 传统 | 现代（Cilium、Falco、Katran） |

### 7.2 性能优化技巧

| 技巧 | 说明 |
|:-----|:-----|
| **使用PERCPU_MAP** | 避免锁竞争 |
| **批量处理** | XDP一次处理多个包 |
| **BPF_RINGBUF** | 替代perf_event_array，更高吞吐 |
| **避免bpf_printk** | 生产环境移除调试输出 |
| **使用LPM_TRIE** | IP最长前缀匹配，O(log n) |
| **BPF_FOR_EACH** | 内核6.1+，循环优化 |

---

## 八、参考资源

### 8.1 官方资源

| 资源 | URL |
|:-----|:----|
| Linux Kernel BPF文档 | `Documentation/bpf/` in kernel tree |
| libbpf GitHub | https://github.com/libbpf/libbpf |
| bpftool文档 | https://github.com/libbpf/bpftool |
| Brendan Gregg博客 | https://www.brendangregg.com/ebpf.html |

### 8.2 书籍

| 书籍 | 作者 | 内容 |
|:-----|:-----|:-----|
| *BPF Performance Tools* | Brendan Gregg | BPF追踪与分析圣经 |
| *Learning eBPF* | Liz Rice | eBPF入门与实践 |
| *Linux Observability with BPF* | David Calavera, Lorenzo Fontana | 可观测性 |

### 8.3 项目案例

| 项目 | 用途 | 技术 |
|:-----|:-----|:-----|
| **Cilium** | Kubernetes网络/安全 | XDP, TC, LSM |
| **Falco** | 容器安全监控 | tracepoint, kprobe |
| **Katran** | Facebook L4负载均衡 | XDP |
| **BCC** | 动态追踪工具集 | kprobe, tracepoint |
| **Pixie** | Kubernetes可观测性 | uprobe, kprobe |

---

> **总结**: eBPF是Linux内核的**可编程革命**，通过安全的虚拟机实现了内核态的高性能编程。从XDP包处理到kprobe函数追踪，从简单的计数器到复杂的CO-RE可移植程序，eBPF已成为**现代Linux系统编程的核心技能**。配合libbpf和bpftool，C开发者可以安全高效地扩展内核功能，而无需编写高风险的内核模块。
