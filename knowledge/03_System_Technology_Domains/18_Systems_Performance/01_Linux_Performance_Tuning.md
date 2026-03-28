# Linux 系统性能调优深度指南

> **层级定位**: 03_System_Technology_Domains / 18_Systems_Performance
> **难度级别**: L4 系统层
> **对应标准**: POSIX, Linux Kernel
> **最后更新**: 2026-03-28

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | CPU调度、内存管理、I/O优化、网络调优、性能分析 |
| **前置知识** | [内存管理](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md)、[进程调度](../../16_OS_Kernel/02_Process_Scheduling_Deep.md) |
| **后续延伸** | [BPF性能工具](02_BPF_Performance_Tools.md)、[eBPF深度](../../01_System_Programming/06_eBPF_Deep.md) |
| **横向关联** | [内核同步](../../21_OS_Internals/01_Kernel_Synchronization.md)、[高性能服务器](../../15_Network_Programming/03_High_Performance_Server.md) |
| **权威来源** | Linux Kernel Documentation, Brendan Gregg's Systems Performance |

---

## 📑 目录

- [Linux 系统性能调优深度指南](#linux-系统性能调优深度指南)
  - [📋 本节概要](#-本节概要)
  - [📑 目录](#-目录)
  - [🎯 性能调优概述](#-性能调优概述)
    - [性能调优方法论](#性能调优方法论)
    - [关键性能指标](#关键性能指标)
  - [🔧 CPU性能调优](#-cpu性能调优)
    - [CPU调度策略](#cpu调度策略)
    - [进程优先级与亲和性](#进程优先级与亲和性)
    - [CPU频率调节](#cpu频率调节)
  - [💾 内存性能调优](#-内存性能调优)
    - [虚拟内存参数](#虚拟内存参数)
    - [内存分配策略](#内存分配策略)
    - [透明大页](#透明大页)
  - [💽 I/O性能调优](#-io性能调优)
    - [I/O调度器](#io调度器)
    - [文件系统优化](#文件系统优化)
    - [磁盘调度](#磁盘调度)
  - [🌐 网络性能调优](#-网络性能调优)
    - [内核网络参数](#内核网络参数)
    - [网络接口优化](#网络接口优化)
    - [TCP/IP调优](#tcpip调优)
  - [📊 性能分析工具](#-性能分析工具)
    - [常用工具速查](#常用工具速查)
    - [perf工具详解](#perf工具详解)
  - [🔬 深入理解](#-深入理解)
    - [内核调度器原理](#内核调度器原理)
    - [内存管理原理](#内存管理原理)
    - [网络栈原理](#网络栈原理)
  - [✅ 质量验收清单](#-质量验收清单)
  - [📚 相关资源](#-相关资源)
    - [官方文档](#官方文档)
    - [推荐书籍](#推荐书籍)
    - [工具](#工具)

---

## 🎯 性能调优概述

### 性能调优方法论

```
Linux性能调优流程：

1. 建立基准线 (Baseline)
   ├── 记录当前系统性能指标
   ├── 确定性能目标
   └── 识别性能瓶颈

2. 监控与分析
   ├── 使用工具收集数据
   ├── 分析瓶颈所在
   └── 确定调优方向

3. 实施调优
   ├── 修改内核参数
   ├── 优化应用程序
   └── 调整硬件配置

4. 验证效果
   ├── 对比调优前后性能
   ├── 确保系统稳定性
   └── 记录调优参数
```

### 关键性能指标

| 指标 | 工具 | 说明 |
|:-----|:-----|:-----|
| **CPU使用率** | top, mpstat | 用户态/内核态/空闲比例 |
| **内存使用** | free, vmstat | 物理内存、交换空间 |
| **磁盘I/O** | iostat, iotop | IOPS、吞吐量、延迟 |
| **网络吞吐** | sar, nload | 带宽、包速率 |
| **上下文切换** | vmstat, pidstat | 进程切换频率 |

---

## 🔧 CPU性能调优

### CPU调度策略

```c
// 查看和设置调度策略
#include <sched.h>
#include <stdio.h>

void show_scheduler(int pid) {
    int policy = sched_getscheduler(pid);
    struct sched_param param;
    sched_getparam(pid, &param);

    printf("PID %d Scheduler: ", pid);
    switch (policy) {
        case SCHED_FIFO:  printf("FIFO (实时)"); break;
        case SCHED_RR:    printf("RR (实时轮转)"); break;
        case SCHED_OTHER: printf("OTHER (普通)"); break;
        case SCHED_BATCH: printf("BATCH (批处理)"); break;
        case SCHED_IDLE:  printf("IDLE (空闲)"); break;
        default:          printf("Unknown");
    }
    printf(", Priority: %d\n", param.sched_priority);
}

// 设置为实时调度策略
void set_realtime_scheduler(int pid, int priority) {
    struct sched_param param;
    param.sched_priority = priority;  // 1-99, 数值越大优先级越高
    sched_setscheduler(pid, SCHED_FIFO, &param);
}
```

### 进程优先级与亲和性

```bash
# 查看进程优先级
nice -n 10 ./my_program  # 以较低优先级运行（-20到19，越小越高）
renice -n 5 -p 1234      # 修改已有进程优先级

# CPU亲和性设置
taskset -c 0,1 ./my_program  # 绑定到CPU 0和1
taskset -pc 0-3 1234         # 将PID 1234绑定到CPU 0-3
```

```c
// CPU亲和性编程
#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>

void set_cpu_affinity(int cpu_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);

    if (sched_setaffinity(0, sizeof(cpuset), &cpuset) == -1) {
        perror("sched_setaffinity");
    }
}

// 绑定到特定NUMA节点
#include <numa.h>

void set_numa_affinity(int node) {
    if (numa_available() < 0) {
        printf("NUMA not available\n");
        return;
    }
    numa_run_on_node(node);
    numa_set_preferred(node);
}
```

### CPU频率调节

```bash
# 查看CPU频率调节器
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor

# 可选调节器：
# - performance: 始终最高频率
# - powersave: 始终最低频率
# - ondemand: 根据负载动态调整
# - conservative: 渐进式调整
# - schedutil: 基于调度器反馈（推荐）

# 设置为性能模式
echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# 查看CPU频率
cat /proc/cpuinfo | grep MHz
cpupower frequency-info
cpupower monitor
```

---

## 💾 内存性能调优

### 虚拟内存参数

```bash
# 查看内存使用情况
free -h
vmstat -s
cat /proc/meminfo

# 关键内核参数

# 交换倾向（0-100），越低越不容易使用交换空间
vm.swappiness = 10

# 脏页比率，达到该比例开始写入磁盘
vm.dirty_ratio = 40
vm.dirty_background_ratio = 10

# 内存过量使用策略
# 0: 启发式允许
# 1: 始终允许（默认）
# 2: 禁止，总量不能超过swap+RAM一定比例
vm.overcommit_memory = 2
vm.overcommit_ratio = 50

# 应用参数
sudo sysctl -w vm.swappiness=10
# 或编辑 /etc/sysctl.conf 持久化
echo "vm.swappiness=10" | sudo tee -a /etc/sysctl.conf
```

### 内存分配策略

```c
// 大页内存分配
#include <sys/mman.h>
#include <stdio.h>

void* alloc_hugepage(size_t size) {
    void* ptr = mmap(NULL, size,
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                     -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap hugepage");
        return NULL;
    }
    return ptr;
}

// 使用posix_memalign进行对齐分配
#include <stdlib.h>

void* alloc_aligned(size_t size, size_t alignment) {
    void* ptr;
    if (posix_memalign(&ptr, alignment, size) != 0) {
        return NULL;
    }
    return ptr;
}

// 示例：分配64字节对齐的内存（适合AVX-512）
float* array = (float*)alloc_aligned(1024 * sizeof(float), 64);
```

### 透明大页

```bash
# 查看透明大页状态
cat /sys/kernel/mm/transparent_hugepage/enabled

# 可选值：
# - always: 始终尝试使用大页
# - madvise: 仅对建议的区域使用（推荐）
# - never: 禁用

# 临时修改
echo madvise | sudo tee /sys/kernel/mm/transparent_hugepage/enabled

# 在程序中使用madvise
#include <sys/mman.h>
madvise(ptr, length, MADV_HUGEPAGE);  // 建议内核使用大页
```

---

## 💽 I/O性能调优

### I/O调度器

```bash
# 查看当前I/O调度器
cat /sys/block/sda/queue/scheduler

# 可用调度器：
# - none: 无调度（NVMe推荐）
# - mq-deadline: 多队列截止时间（SSD推荐）
# - kyber: 低延迟优化
# - bfq: 公平队列（桌面系统推荐）

# 设置I/O调度器
echo none | sudo tee /sys/block/nvme0n1/queue/scheduler

# I/O调度参数
cat /sys/block/sda/queue/nr_requests      # 队列深度
cat /sys/block/sda/queue/read_ahead_kb    # 预读大小
```

### 文件系统优化

```bash
# ext4挂载选项优化
mount -o noatime,nodiratime,data=writeback,nobarrier /dev/sda1 /data

# 解释：
# - noatime: 不更新访问时间
# - nodiratime: 不更新目录访问时间
# - data=writeback: 异步元数据日志（最快，但风险较高）
# - nobarrier: 禁用写屏障（有UPS时可用）

# 查看文件系统碎片
e4defrag -c /path

# TRIM支持（SSD必需）
fstrim -av /
```

### 磁盘调度

```c
// 使用fadvise优化文件访问
#include <fcntl.h>
#include <unistd.h>

void optimize_file_access(int fd) {
    // 顺序访问提示
    posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);

    // 即将访问
    posix_fadvise(fd, 0, 1024*1024, POSIX_FADV_WILLNEED);

    // 不需要缓存
    // posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED);
}

// 直接I/O（绕过页缓存）
#include <fcntl.h>

int open_direct(const char* path) {
    return open(path, O_RDWR | O_DIRECT);  // 需要对齐的缓冲区
}

// 异步I/O
#include <libaio.h>

void async_io_example() {
    io_context_t ctx = 0;
    io_setup(128, &ctx);

    struct iocb cb;
    struct iocb* cbs[1] = {&cb};

    // 设置异步读操作
    io_prep_pread(&cb, fd, buffer, size, offset);

    // 提交I/O请求
    io_submit(ctx, 1, cbs);

    // 等待完成
    struct io_event events[1];
    io_getevents(ctx, 1, 1, events, NULL);
}
```

---

## 🌐 网络性能调优

### 内核网络参数

```bash
# 网络缓冲区大小
# 增加缓冲区可提高高带宽延迟网络的吞吐量
net.core.rmem_max = 134217728    # 接收缓冲区最大值 (128MB)
net.core.wmem_max = 134217728    # 发送缓冲区最大值 (128MB)
net.core.rmem_default = 16777216 # 默认接收缓冲区 (16MB)
net.core.wmem_default = 16777216 # 默认发送缓冲区 (16MB)

# TCP优化
net.ipv4.tcp_rmem = 4096 87380 134217728  # min default max
net.ipv4.tcp_wmem = 4096 65536 134217728
net.ipv4.tcp_congestion_control = bbr  # 使用BBR拥塞控制算法
net.ipv4.tcp_notsent_lowat = 16384     # 减少TCP队列延迟

# 连接跟踪优化（高并发场景）
net.netfilter.nf_conntrack_max = 1000000
net.ipv4.tcp_max_syn_backlog = 65536
net.core.netdev_max_backlog = 65536

# 应用参数
sudo sysctl -p /etc/sysctl.conf
```

### 网络接口优化

```bash
# 查看网卡队列
ethtool -l eth0

# 设置队列数（多队列网卡）
ethtool -L eth0 combined 8

# 查看中断亲和性
cat /proc/interrupts | grep eth0

# 开启RSS（接收侧缩放）
ethtool -K eth0 rxhash on

# 开启TSO/GSO/GRO（分片卸载）
ethtool -K eth0 tso on gso on gro on

# 查看网卡 offload 状态
ethtool -k eth0
```

### TCP/IP调优

```c
// TCP socket选项优化
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

void optimize_tcp_socket(int sockfd) {
    int yes = 1;

    // TCP_NODELAY: 禁用Nagle算法，降低延迟
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));

    // SO_RCVBUF/SO_SNDBUF: 设置缓冲区大小
    int bufsize = 128 * 1024 * 1024;  // 128MB
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));

    // TCP_QUICKACK: 禁用延迟ACK
    setsockopt(sockfd, IPPROTO_TCP, TCP_QUICKACK, &yes, sizeof(yes));

    // TCP_FASTOPEN: 减少连接建立延迟（客户端）
    setsockopt(sockfd, IPPROTO_TCP, TCP_FASTOPEN_CONNECT, &yes, sizeof(yes));

    // SO_BUSY_POLL: 低延迟轮询（配合内核参数使用）
    int busy_poll_us = 50;
    int busy_budget = 64;
    setsockopt(sockfd, SOL_SOCKET, SO_BUSY_POLL, &busy_poll_us, sizeof(busy_poll_us));
    setsockopt(sockfd, SOL_SOCKET, SO_BUSY_POLL_BUDGET, &busy_budget, sizeof(busy_budget));
}
```

---

## 📊 性能分析工具

### 常用工具速查

| 工具 | 用途 | 示例命令 |
|:-----|:-----|:---------|
| **top/htop** | 实时进程监控 | `htop` |
| **vmstat** | 系统整体状态 | `vmstat 1 10` |
| **iostat** | 磁盘I/O统计 | `iostat -x 1` |
| **mpstat** | CPU统计 | `mpstat -P ALL 1` |
| **pidstat** | 进程统计 | `pidstat -u -p PID 1` |
| **sar** | 历史性能数据 | `sar -u 1 10` |
| **perf** | 硬件性能计数器 | `perf top` |
| **bcc/bpftrace** | 动态追踪 | `bpftrace -e 'kprobe:do_nanosleep { @[comm] = count(); }'` |

### perf工具详解

```bash
# 采样CPU周期，查看热点函数
perf record -g ./my_program
perf report --sort=dso,symbol

# 实时查看热点
perf top -g

# 统计缓存命中率
perf stat -e cache-references,cache-misses ./my_program

# 跟踪系统调用
perf trace -e 'syscalls:sys_enter_*' ./my_program
```

---

## 🔬 深入理解

### 内核调度器原理

```
Linux CFS（完全公平调度器）：

虚拟运行时间（vruntime）：
- 每个进程有一个vruntime值
- vruntime增长越慢的进程优先级越高
- 调度器选择vruntime最小的进程运行

权重计算：
weight = 1024 / (1.25 ^ nice)

时间片计算：
slice = (weight / total_weight) * period
```

### 内存管理原理

```
页回收策略：

1. 文件页（File-backed pages）
   ├── 干净页：直接丢弃
   └── 脏页：写回磁盘后释放

2. 匿名页（Anonymous pages）
   ├── 有交换空间：换出到swap
   └── 无交换空间：保留在内存

回收优先级：
Inactive File > Active File > Inactive Anon > Active Anon
```

### 网络栈原理

```
Linux网络栈接收路径：

网卡 → Ring Buffer → 硬中断 → 软中断(NAPI) → 协议栈 → Socket缓冲区 → 应用

优化点：
- Ring Buffer大小：ethtool -G eth0 rx 4096
- 软中断处理：调整net.core.netdev_budget
- NAPI权重：调整net.core.dev_weight
```

---

## ✅ 质量验收清单

- [x] CPU性能调优（调度策略、亲和性、频率调节）
- [x] 内存性能调优（虚拟内存、大页、分配策略）
- [x] I/O性能调优（调度器、文件系统、磁盘调度）
- [x] 网络性能调优（内核参数、接口优化、TCP调优）
- [x] 性能分析工具介绍
- [x] 代码示例丰富
- [x] 深入理解原理

---

## 📚 相关资源

### 官方文档

- **Linux Kernel Documentation**: <https://www.kernel.org/doc/html/latest/>
- **sysctl参数说明**: <https://www.kernel.org/doc/Documentation/sysctl/>

### 推荐书籍

- 《Systems Performance》 - Brendan Gregg
- 《Linux Kernel Development》 - Robert Love
- 《Understanding Linux Network Internals》 - Christian Benvenuti

### 工具

- **perf-tools**: <https://github.com/brendangregg/perf-tools>
- **bcc**: <https://github.com/iovisor/bcc>
- **sysstat**: `apt install sysstat`

---

> **最后更新**: 2026-03-28
> **版本**: 1.0 - 完整版
> **维护者**: C_Lang Knowledge Base Team
