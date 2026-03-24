# Linux性能追踪与观测技术


> **版本**: v1.0.0
> **最后更新**: 2026-03-25
> **作者**: C_Lang Team
> **难度**: L4 高级

---
> **层级定位**: 07 Modern Toolchain
> **参考工具**: perf, ftrace, LTTng, eBPF/BCC
> **难度级别**: L4-L5
> **预估学习时间**: 12-15 小时

---

## 1. Linux性能观测架构

### 1.1 观测技术层次

```
Linux性能观测层次：

┌─────────────────────────────────────────────────────────────┐
│  应用层                                                      │
│  • 应用日志、APM工具                                         │
│  • LTTng UST (用户空间跟踪)                                  │
├─────────────────────────────────────────────────────────────┤
│  系统调用层                                                  │
│  • strace - 系统调用跟踪                                     │
│  • audit - 审计框架                                          │
├─────────────────────────────────────────────────────────────┤
│  库层                                                        │
│  • LD_PRELOAD钩子                                            │
│  • uprobes - 用户空间探针                                    │
├─────────────────────────────────────────────────────────────┤
│  内核层                                                      │
│  • kprobes - 内核动态探针                                    │
│  • tracepoints - 静态内核跟踪点                              │
│  • ftrace - 内核函数跟踪                                     │
│  • perf - 性能计数器                                         │
├─────────────────────────────────────────────────────────────┤
│  硬件层                                                      │
│  • PMU (Performance Monitoring Unit)                         │
│  • CPU性能计数器                                             │
│  • 缓存、分支预测器事件                                       │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. perf性能分析器

### 2.1 perf架构

```
perf架构：

用户空间                        内核空间
┌─────────────┐               ┌─────────────────────────────┐
│ perf命令    │               │ perf_event_open()           │
│ perf record │──────────────>│  • 创建性能监控事件          │
│ perf stat   │               │  • 配置PMU计数器             │
│ perf top    │               │  • 设置采样参数              │
│ perf report │               │                             │
└─────────────┘               │ perf_event 子系统            │
       ▲                      │  • 调度事件                  │
       │                      │  • 收集样本                  │
       │                      │  • 写入ring buffer           │
       └──────────────────────│                             │
           读取数据           │ PMU (硬件性能监控单元)        │
                              │  • CPU周期                   │
                              │  • 缓存未命中                │
                              │  • 分支预测                  │
                              └─────────────────────────────┘
```

### 2.2 perf常用命令

```bash
# ========== 基本统计 ==========

# CPU周期统计
perf stat ./program

# 详细统计
perf stat -d ./program

# 特定事件统计
perf stat -e cycles,instructions,cache-misses ./program

# 系统范围统计（所有CPU）
perf stat -a sleep 5

# ========== 采样分析 ==========

# 记录CPU样本（默认事件）
perf record ./program

# 记录特定事件
perf record -e cache-misses ./program

# 记录调用图
perf record -g ./program

# 频率采样（每秒1000次）
perf record -F 1000 ./program

# 分析结果
perf report
perf report --sort=comm,dso

# 生成火焰图
perf record -g ./program
perf script | ./stackcollapse-perf.pl | ./flamegraph.pl > flamegraph.svg

# ========== 实时监控 ==========

# top模式
perf top

# 查看特定进程
perf top -p $(pidof mysqld)

# 查看特定事件
perf top -e cache-misses

# ========== 事件列表 ==========

# 列出所有事件
perf list

# 按类别列出
perf list 'cache:*'
perf list 'sched:*'
```

### 2.3 perf事件类型

```bash
# ========== 硬件事件 ==========

cycles                  # CPU周期
instructions            # 指令数
cache-references        # 缓存引用
cache-misses            # 缓存未命中
branches                # 分支指令
branch-misses           # 分支预测失败
bus-cycles              # 总线周期
ref-cycles              # 参考时钟周期

# ========== 软件事件 ==========

cpu-clock               # CPU时钟
task-clock              # 任务时钟
page-faults             # 页错误
context-switches        # 上下文切换
cpu-migrations          # CPU迁移
minor-faults            # 次要页错误
major-faults            # 主要页错误
alignment-faults        # 对齐错误

# ========== 跟踪点事件 ==========

sched:sched_switch      # 调度切换
sched:sched_process_fork # 进程fork
syscalls:sys_enter_open  # 进入open系统调用
block:block_rq_issue     # 块设备请求

# ========== 自定义事件 ==========

# 使用原始事件编码
perf stat -e r1c2 ./program  # 事件0x1c2

# 使用内核pmu事件
perf stat -e cpu/event=0x3c,umask=0x00/ ./program
```

### 2.4 perf进阶使用

```bash
# ========== 进程树分析 ==========

# 跟踪进程及其子进程
perf record -g --follow-forks ./program

# ========== 内核符号 ==========

# 需要内核调试信息
perf record -g --kallsyms=/proc/kallsyms ./program

# ========== 动态探针 ==========

# 添加uprobe
perf probe --add 'malloc'
perf record -e probe:malloc ./program

# 添加kprobe
perf probe --add 'tcp_sendmsg'
perf record -e probe:tcp_sendmsg -a

# ========== 脚本分析 ==========

# 使用perf脚本
perf script -g python

# 生成自定义报告
perf script --header > out.perf

# ========== 比较分析 ==========

# 基准测试
perf record -o baseline.data ./program_old
perf record -o optimized.data ./program_new

# 对比
perf diff baseline.data optimized.data
```

---

## 3. ftrace内核跟踪

### 3.1 ftrace架构

```
ftrace架构：

┌─────────────────────────────────────────────────────────────┐
│  TraceFS (/sys/kernel/debug/tracing/)                       │
│  • available_tracers    - 可用跟踪器                        │
│  • current_tracer       - 当前跟踪器                        │
│  • available_events     - 可用事件                          │
│  • events/              - 事件配置                          │
│  • tracing_on           - 开关                              │
│  • trace                - 输出                              │
└─────────────────────────────────────────────────────────────┘

跟踪器类型：
• function      - 函数入口跟踪
• function_graph - 函数调用图
• mmiotrace     - MMIO访问
• blk           - 块设备I/O
• nop           - 无（用于事件跟踪）
```

### 3.2 ftrace使用

```bash
# ========== 基本设置 ==========

# 挂载tracefs
sudo mount -t tracefs none /sys/kernel/debug/tracing

# 查看可用跟踪器
cat /sys/kernel/debug/tracing/available_tracers

# 启用函数跟踪
echo function > /sys/kernel/debug/tracing/current_tracer
echo 1 > /sys/kernel/debug/tracing/tracing_on

# 运行程序...

# 查看结果
cat /sys/kernel/debug/tracing/trace

# 停止
echo 0 > /sys/kernel/debug/tracing/tracing_on

# ========== 函数过滤 ==========

# 只跟踪特定函数
echo 'kfree*' > /sys/kernel/debug/tracing/set_ftrace_filter
echo 'kmalloc*' >> /sys/kernel/debug/tracing/set_ftrace_filter

# 排除特定函数
echo 'rcu_*' > /sys/kernel/debug/tracing/set_ftrace_notrace

# 清除过滤
echo > /sys/kernel/debug/tracing/set_ftrace_filter

# ========== 函数图跟踪 ==========

echo function_graph > /sys/kernel/debug/tracing/current_tracer

# 设置最大深度
echo 5 > /sys/kernel/debug/tracing/max_graph_depth

# ========== 事件跟踪 ==========

# 查看可用事件
cat /sys/kernel/debug/tracing/available_events

# 启用调度事件
echo 1 > /sys/kernel/debug/tracing/events/sched/sched_switch/enable
echo 1 > /sys/kernel/debug/tracing/events/sched/sched_process_fork/enable

# 启用所有调度事件
echo 1 > /sys/kernel/debug/tracing/events/sched/enable

# 启用系统调用事件
echo 1 > /sys/kernel/debug/tracing/events/syscalls/enable

# 使用nop跟踪器进行纯事件跟踪
echo nop > /sys/kernel/debug/tracing/current_tracer
```

### 3.3 trace-cmd工具

```bash
# ========== 记录跟踪 ==========

# 基本记录
sudo trace-cmd record -p function ./program

# 记录函数图
sudo trace-cmd record -p function_graph ./program

# 记录特定事件
sudo trace-cmd record -e sched_switch -e sched_wakeup ./program

# 记录所有CPU
sudo trace-cmd record -a ./program

# ========== 报告分析 ==========

# 查看报告
trace-cmd report

# 启动GUI分析器
trace-cmd report --gui  # 或 kernelshark

# ========== 实时查看 ==========

# 实时监控
trace-cmd start -p function
trace-cmd show
trace-cmd stop
```

---

## 4. eBPF高级跟踪

### 4.1 eBPF性能工具

```bash
# ========== BCC工具集 ==========

# 安装: sudo apt-get install bpfcc-tools

# 功能跟踪
sudo funccount-bpfcc 'tcp_send*'    # 统计函数调用次数
sudo funccount-bpfcc -i 1 'vfs_*'   # 每秒统计

# 调用栈采样
sudo profile-bpfcc -F 99 -a 5       # 99Hz采样，5秒

# 延迟分析
sudo funclatency-bpfcc vfs_read     # 函数延迟直方图
sudo biolatency-bpfcc               # 块I/O延迟

# 文件系统分析
sudo ext4slower-bpfcc               # 显示慢于阈值的ext4操作
sudo xfsslower-bpfcc

# 网络分析
sudo tcptop-bpfcc                   # TCP流量top
sudo tcplife-bpfcc                  # TCP连接生命周期

# 内存分析
sudo memleak-bpfcc -p $(pidof app)  # 内存泄漏检测

# ========== bpftrace ==========

# 单行命令
sudo bpftrace -e 'kprobe:do_nanosleep { printf("sleep by %s\n", comm); }'

# 脚本文件
sudo bpftrace script.bt
```

### 4.2 bpftrace脚本示例

```bpftrace
#!/usr/bin/env bpftrace

// 跟踪open系统调用
kprobe:do_sys_openat2
{
    printf("open: %s by %s (pid %d)\n",
           str(arg1), comm, pid);
}

// 跟踪文件读取延迟
kprobe:vfs_read
{
    @start[tid] = nsecs;
}

kretprobe:vfs_read /@start[tid]/
{
    @latency_us = hist((nsecs - @start[tid]) / 1000);
    delete(@start[tid]);
}

// 跟踪TCP连接
kprobe:tcp_connect
{
    printf("TCP connect from %s (pid %d)\n", comm, pid);
}

// 跟踪内存分配
kprobe:__kmalloc
{
    @allocations[comm] = count();
    @bytes[comm] = sum(arg0);
}

// 定时输出
interval:s:5
{
    printf("\n=== Memory Allocations (last 5s) ===\n");
    print(@allocations);
    print(@bytes);
    clear(@allocations);
    clear(@bytes);
}
```

---

## 5. LTTng用户空间跟踪

### 5.1 LTTng架构

```
LTTng (Linux Trace Toolkit Next Generation):

┌─────────────────────────────────────────────────────────────┐
│  会话守护进程 (lttng-sessiond)                              │
│  • 管理跟踪会话                                             │
│  • 协调内核和用户空间跟踪                                   │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        ▼                     ▼                     ▼
┌───────────────┐    ┌───────────────┐    ┌───────────────┐
│  内核跟踪      │    │  用户空间跟踪  │    │  快照/直播     │
│  lttng-modules │    │  lttng-ust    │    │  lttng-relayd │
└───────────────┘    └───────────────┘    └───────────────┘
```

### 5.2 LTTng使用

```bash
# ========== 基本跟踪 ==========

# 创建会话
lttng create my-session

# 启用内核事件
lttng enable-event -k sched_switch,sched_wakeup
lttng enable-event -k --syscall open,close,read,write

# 启用所有内核事件
lttng enable-event -k -a

# 开始跟踪
lttng start

# ... 运行程序 ...

# 停止跟踪
lttng stop

# 查看结果
lttng view

# 销毁会话
lttng destroy

# ========== 用户空间跟踪 ==========

# 应用程序需要链接lttng-ust并包含跟踪点

# 启用UST事件
lttng enable-event -u myapp:my_tracepoint

# ========== 高级功能 ==========

# 快照模式
lttng create my-session --snapshot
lttng snapshot record

# 跟踪过滤
lttng add-context -k -t pid -t tid -t comm
lttng enable-event -k sched_switch --filter 'ctx_pid == 1234'

# 网络流式输出
lttng create my-session --live
lttng enable-event -k -a
lttng start
# 使用Trace Compass实时查看
```

---

## 6. 工具选择指南

```
场景选择：

CPU性能分析：
• 热点函数 → perf record -g
• 调用图 → perf + FlameGraph
• 微架构 → perf stat + PMU事件

内存分析：
• 分配跟踪 → bpftrace/memleak-bpfcc
• 访问模式 → perf mem
• 页面错误 → perf stat -e page-faults

I/O分析：
• 磁盘I/O → iostat, biolatency-bpfcc
• 文件系统 → ext4slower-bpfcc
• I/O栈 → blktrace

网络分析：
• 连接跟踪 → ss, tcplife-bpfcc
• 包分析 → tcpdump, Wireshark
• 性能 → tcptop-bpfcc

调度延迟：
• 上下文切换 → perf sched
• 调度分析 → schedtool, chrt
• 实时跟踪 → ftrace sched事件

综合诊断：
• 系统概览 → dstat, vmstat
• 深度分析 → eBPF/BCC工具集
• 长时间跟踪 → LTTng
```

---

## 关联导航

### 前置知识

- [eBPF深度](../../03_System_Technology_Domains/01_System_Programming/06_eBPF_Deep.md)
- [性能工程](../../01_Core_Knowledge_System/05_Engineering_Layer/06_Performance_Engineering.md)

### 后续延伸

- [动态分析工具](./06_Dynamic_Analysis_Tools.md)
- [内核性能调优](../../03_System_Technology_Domains/16_OS_Kernel/06_Kernel_Performance_Tuning.md)

### 参考

- Brendan Gregg's Linux Performance
- Linux perf Examples
- BPF Performance Tools
