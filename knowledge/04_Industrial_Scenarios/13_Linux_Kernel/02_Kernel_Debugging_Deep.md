# Linux内核调试深度解析

> **层级定位**: 04 Industrial Scenarios / 13 Linux Kernel
> **难度级别**: L5 专家
> **预估学习时间**: 10-12 小时

---

## 目录

- [Linux内核调试深度解析](#linux内核调试深度解析)
  - [目录](#目录)
  - [1. 内核调试方法概览](#1-内核调试方法概览)
    - [1.1 调试方法对比](#11-调试方法对比)
  - [2. printk与动态打印](#2-printk与动态打印)
    - [2.1 printk级别](#21-printk级别)
    - [2.2 动态打印 (Dynamic Debug)](#22-动态打印-dynamic-debug)
    - [2.3 pr\_debug与dev\_dbg](#23-pr_debug与dev_dbg)
  - [3. ftrace深度使用](#3-ftrace深度使用)
    - [3.1 ftrace基础](#31-ftrace基础)
    - [3.2 条件跟踪](#32-条件跟踪)
    - [3.3 trace\_marker用户态标记](#33-trace_marker用户态标记)
  - [4. KGDB双机调试](#4-kgdb双机调试)
    - [4.1 配置KGDB](#41-配置kgdb)
    - [4.2 GDB连接](#42-gdb连接)
    - [4.3 QEMU调试](#43-qemu调试)
  - [5. eBPF内核追踪](#5-ebpf内核追踪)
    - [5.1 内核函数跟踪](#51-内核函数跟踪)
    - [5.2 kretprobe返回值](#52-kretprobe返回值)
  - [6. 内核崩溃分析](#6-内核崩溃分析)
    - [6.1 kdump配置](#61-kdump配置)
    - [6.2 crash工具分析](#62-crash工具分析)
    - [6.3 oops分析](#63-oops分析)
  - [7. 内存调试](#7-内存调试)
    - [7.1 KASAN (Kernel Address Sanitizer)](#71-kasan-kernel-address-sanitizer)
    - [7.2 SLUB调试](#72-slub调试)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. 内核调试方法概览

### 1.1 调试方法对比

| 方法 | 适用场景 | 侵入性 | 性能影响 |
|:-----|:---------|:-------|:---------|
| printk | 简单跟踪 | 低 | 低 |
| 动态打印 | 条件输出 | 低 | 低 |
| ftrace | 函数跟踪 | 中 | 中 |
| eBPF | 复杂分析 | 中 | 低 |
| KGDB | 源码调试 | 高 | 高 |
| QEMU | 开发调试 | 无 | 无 |
| crash | 事后分析 | 无 | 无 |

---

## 2. printk与动态打印

### 2.1 printk级别

```c
// 内核打印级别
#define KERN_EMERG   "<0>"   // 系统崩溃
#define KERN_ALERT   "<1>"   // 立即采取行动
#define KERN_CRIT    "<2>"   // 临界条件
#define KERN_ERR     "<3>"   // 错误
#define KERN_WARNING "<4>"   // 警告
#define KERN_NOTICE  "<5>"   // 正常但重要
#define KERN_INFO    "<6>"   // 信息
#define KERN_DEBUG   "<7>"   // 调试信息

// 使用
printk(KERN_ERR "Device %s failed\n", dev_name);
```

### 2.2 动态打印 (Dynamic Debug)

```c
// 开启特定文件的动态打印
echo 'file driver.c +p' > /sys/kernel/debug/dynamic_debug/control

// 开启特定函数的打印
echo 'func my_function +p' > /sys/kernel/debug/dynamic_debug/control

// 格式化打印
echo 'format "irq=%d" +p' > /sys/kernel/debug/dynamic_debug/control

// 查看当前设置
cat /sys/kernel/debug/dynamic_debug/control | grep mydriver
```

### 2.3 pr_debug与dev_dbg

```c
// 驱动中的调试宏
#include <linux/device.h>

static int my_probe(struct platform_device *pdev) {
    // 只在定义了DEBUG或动态打印开启时输出
    pr_debug("Probing device\n");

    // 带设备信息的调试
    dev_dbg(&pdev->dev, "Resource %pR\n", res);
    dev_info(&pdev->dev, "Device initialized\n");
    dev_err(&pdev->dev, "Failed to read: %d\n", ret);

    return 0;
}
```

---

## 3. ftrace深度使用

### 3.1 ftrace基础

```bash
# 挂载tracefs
mount -t tracefs tracefs /sys/kernel/tracing

# 查看可用跟踪器
cat /sys/kernel/tracing/available_tracers
# function function_graph blk mmiotrace wakeup wakeup_rt

# 启用函数跟踪
echo function > /sys/kernel/tracing/current_tracer
echo __kmalloc > /sys/kernel/tracing/set_ftrace_filter
echo 1 > /sys/kernel/tracing/tracing_on

# 查看结果
cat /sys/kernel/tracing/trace

# 图形化显示调用链
echo function_graph > /sys/kernel/tracing/current_tracer
echo 1 > /sys/kernel/tracing/options/funcgraph-tail
```

### 3.2 条件跟踪

```bash
# 只跟踪特定PID
echo $$ > /sys/kernel/tracing/set_ftrace_pid

# 跟踪特定函数及子调用
echo 'kmalloc:mod:mydriver' > /sys/kernel/tracing/set_ftrace_filter

# 使用通配符
echo '*kmalloc*' > /sys/kernel/tracing/set_ftrace_filter

# 排除函数
echo '!kfree' >> /sys/kernel/tracing/set_ftrace_notrace
```

### 3.3 trace_marker用户态标记

```c
// 用户程序中标记事件
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int trace_fd;

void trace_begin(const char *name) {
    char buf[256];
    int len = snprintf(buf, sizeof(buf), "B|%d|%s", getpid(), name);
    write(trace_fd, buf, len);
}

void trace_end(void) {
    write(trace_fd, "E", 1);
}

int main() {
    trace_fd = open("/sys/kernel/tracing/trace_marker", O_WRONLY);

    trace_begin("Initialization");
    init();
    trace_end();

    close(trace_fd);
    return 0;
}
```

---

## 4. KGDB双机调试

### 4.1 配置KGDB

```bash
# 内核配置
CONFIG_KGDB=y
CONFIG_KGDB_SERIAL_CONSOLE=y
CONFIG_DEBUG_KERNEL=y
CONFIG_DEBUG_INFO=y

# 启动参数
kgdboc=ttyS0,115200 nokaslr

# 进入KGDB模式
echo g > /proc/sysrq-trigger

# 或触发panic
echo c > /proc/sysrq-trigger
```

### 4.2 GDB连接

```bash
# 主机端
gdb vmlinux

(gdb) set remotebaud 115200
(gdb) target remote /dev/ttyUSB0

# 常用命令
(gdb) bt                    # 内核栈回溯
(gdb) info registers        # 寄存器
(gdb) list *(function+0x20) # 查看特定地址代码
(gdb) break do_sys_open     # 设置断点
(gdb) continue              # 继续执行
(gdb) step                  # 单步
(gdb) next                  # 单步跳过
```

### 4.3 QEMU调试

```bash
# 启动QEMU调试模式
qemu-system-x86_64 \
    -kernel arch/x86/boot/bzImage \
    -append "root=/dev/sda1 nokaslr" \
    -hda rootfs.img \
    -s -S  # -s: GDB端口1234, -S: 启动时暂停

# 连接GDB
gdb vmlinux
(gdb) target remote localhost:1234
(gdb) break start_kernel
(gdb) continue
```

---

## 5. eBPF内核追踪

### 5.1 内核函数跟踪

```c
// kprobe跟踪内核函数
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

char LICENSE[] SEC("license") = "GPL";

SEC("kprobe/do_sys_openat2")
int BPF_KPROBE(trace_openat, int dfd, const char *filename, struct open_how *how) {
    char name[256];
    bpf_probe_read_user_str(name, sizeof(name), filename);
    bpf_printk("Opening: %s\n", name);
    return 0;
}

// 跟踪TCP事件
SEC("tp/skb/tcp_retransmit_skb")
int trace_tcp_retransmit(struct trace_event_raw_tcp_event_sk_skb *ctx) {
    bpf_printk("TCP retransmit: sk=%p\n", ctx->skaddr);
    return 0;
}
```

### 5.2 kretprobe返回值

```c
SEC("kretprobe/__kmalloc")
int BPF_KRETPROBE(trace_kmalloc, void *ret) {
    u64 size = ctx->ax;  // 通过寄存器获取参数
    bpf_printk("Allocated %llu bytes at %p\n", size, ret);
    return 0;
}
```

---

## 6. 内核崩溃分析

### 6.1 kdump配置

```bash
# 安装kdump工具
sudo apt install linux-crashdump

# 配置保留内存
# /etc/default/grub.d/kdump-tools.cfg
GRUB_CMDLINE_LINUX_DEFAULT="crashkernel=512M"

# 启动kdump服务
sudo systemctl enable kdump-tools
sudo systemctl start kdump-tools

# 手动触发测试
echo c > /proc/sysrq-trigger

# 崩溃后vmcore保存在/var/crash/
```

### 6.2 crash工具分析

```bash
# 安装crash
sudo apt install crash

# 分析vmcore
crash vmlinux /var/crash/xxx/vmcore

# 常用命令
crash> bt                   # 崩溃时栈回溯
crash> sys                  # 系统信息
crash> ps                   # 进程列表
crash> files                # 打开的文件
crash> net                  # 网络状态
crash> mount                # 挂载点
crash> runq                 # 运行队列

# 查看特定结构
crash> struct task_struct <addr>
crash> struct module <addr>

# 反汇编
crash> dis -l function_name

# 搜索符号
crash> sym -l | grep my_symbol
```

### 6.3 oops分析

```
# 示例Oops
[   12.345678] BUG: unable to handle kernel NULL pointer dereference at 0000000000000000
[   12.345679] IP: my_function+0x25/0x50 [mydriver]
[   12.345680] PGD 0 P4D 0
[   12.345681] Oops: 0002 [#1] SMP PTI
[   12.345682] CPU: 2 PID: 1234 Comm: myapp Tainted: G           OE     5.4.0-generic
[   12.345683] RIP: 0010:my_function+0x25/0x50 [mydriver]
[   12.345684] RSP: 0018:ffffc900001c3d40 EFLAGS: 00010246
[   12.345685] RAX: 0000000000000000 RBX: ffff888123456789 RCX: 0000000000000000
...
[   12.345690] Call Trace:
[   12.345691]  ? my_function+0x25/0x50 [mydriver]
[   12.345692]  ? caller_function+0x30/0x60 [mydriver]
[   12.345693]  ? system_call+0x52/0xc0
```

**分析方法**:

1. 查看RIP确定崩溃位置
2. 检查寄存器值（RAX=0表示空指针）
3. 分析调用栈
4. 使用addr2line定位源码

```bash
addr2line -e mydriver.ko -f my_function+0x25
```

---

## 7. 内存调试

### 7.1 KASAN (Kernel Address Sanitizer)

```bash
# 配置
CONFIG_KASAN=y
CONFIG_KASAN_INLINE=y  # 或 KASAN_OUTLINE for smaller kernel

# 运行时输出
[   34.567890] ==================================================================
[   34.567891] BUG: KASAN: use-after-free in my_function+0x45/0x80
[   34.567892] Write of size 4 at addr ffff888123456789 by task myapp/1234
[   34.567893]
[   34.567894] CPU: 0 PID: 1234 Comm: myapp Tainted: G    B      OE     5.4.0
[   34.567895] Call Trace:
...
[   34.567900] Allocated by task 1234:
[   34.567901]  __kmalloc+0x123/0x200
[   34.567902]  my_alloc+0x25/0x50 [mydriver]
[   34.567903]
[   34.567904] Freed by task 1234:
[   34.567905]  kfree+0x123/0x200
[   34.567906]  my_free+0x15/0x30 [mydriver]
```

### 7.2 SLUB调试

```bash
# 开启SLUB调试
echo 1 > /sys/kernel/slab/kmalloc-256/validate
echo 1 > /sys/kernel/slab/kmalloc-256/store_user

# 查看对象信息
cat /sys/kernel/slab/kmalloc-256/alloc_calls
```

---

## 关联导航

### 前置知识

- [Linux内核开发](./README.md)
- [调试技术](../../01_Core_Knowledge_System/05_Engineering_Layer/02_Debug_Techniques.md)
- [eBPF深度](../../03_System_Technology_Domains/01_System_Programming/06_eBPF_Deep.md)

### 后续延伸

- [性能优化](../01_High_Performance/README.md)
- [内核安全](../06_Security/README.md)

### 参考

- [Kernel Debugging](https://www.kernel.org/doc/html/latest/dev-tools/index.html)
- [ftrace](https://www.kernel.org/doc/html/latest/trace/ftrace.html)
