# Linux运行时调试与探针技术

> **层级定位**: 07 Modern Toolchain
> **参考课程**: MIT 6.828, CMU 15-410
> **难度级别**: L5 专家
> **预估学习时间**: 15-20 小时

---

## 1. ptrace系统调用深度解析

### 1.1 ptrace架构

```
ptrace - Process Trace
┌─────────────────────────────────────────────────────────────┐
│                    ptrace架构                                │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   调试器进程 (Tracer)          被调试进程 (Tracee)          │
│        │                              │                     │
│        │  PTRACE_ATTACH/SEIZE         │                     │
│        │─────────────────────────────>│                     │
│        │                              │                     │
│        │  发送信号/系统调用通知        │                     │
│        │<─────────────────────────────│                     │
│        │                              │                     │
│        │  PTRACE_PEEK/POKE/GETREGS    │                     │
│        │─────────────────────────────>│                     │
│        │                              │                     │
│        │  PTRACE_SINGLESTEP/CONT      │                     │
│        │─────────────────────────────>│                     │
│        │                              │                     │
└─────────────────────────────────────────────────────────────┘

ptrace请求类型：
• PTRACE_TRACEME    - 子进程请求被跟踪
• PTRACE_ATTACH     - 附加到运行中的进程
• PTRACE_SEIZE      - 非阻塞附加（现代方式）
• PTRACE_PEEKDATA   - 读取内存
• PTRACE_POKEDATA   - 写入内存
• PTRACE_GETREGS    - 读取寄存器
• PTRACE_SETREGS    - 设置寄存器
• PTRACE_SINGLESTEP - 单步执行
• PTRACE_SYSCALL    - 在系统调用处停止
• PTRACE_CONT       - 继续执行
```

### 1.2 基于ptrace的调试器实现

```c
#define _GNU_SOURCE
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

// ========== 基础调试器框架 ==========

typedef struct {
    pid_t pid;
    int status;
    struct user_regs_struct regs;
    int stopped;
} debug_process_t;

// 附加到进程
int debugger_attach(debug_process_t *dbg, pid_t pid) {
    dbg->pid = pid;

    // 现代Linux使用PTRACE_SEIZE，更灵活
    if (ptrace(PTRACE_SEIZE, pid, NULL, PTRACE_O_TRACESYSGOOD) < 0) {
        // 回退到传统ATTACH
        if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) < 0) {
            perror("ptrace attach failed");
            return -1;
        }
    }

    // 等待进程停止
    waitpid(pid, &dbg->status, 0);
    dbg->stopped = 1;

    // 设置选项：跟踪系统调用、fork/clone等
    ptrace(PTRACE_SETOPTIONS, pid, NULL,
           PTRACE_O_TRACESYSGOOD |      // 系统调用通知
           PTRACE_O_TRACECLONE |        // 跟踪clone
           PTRACE_O_TRACEFORK |         // 跟踪fork
           PTRACE_O_TRACEVFORK |        // 跟踪vfork
           PTRACE_O_TRACEEXIT);         // 跟踪退出

    return 0;
}

// 启动程序并调试
int debugger_exec(debug_process_t *dbg, const char *path, char *const argv[]) {
    pid_t pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) {
        // 子进程
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        // 停止等待父进程
        raise(SIGSTOP);
        execv(path, argv);
        _exit(1);
    }

    // 父进程
    dbg->pid = pid;
    waitpid(pid, &dbg->status, 0);

    // 设置选项
    ptrace(PTRACE_SETOPTIONS, pid, NULL,
           PTRACE_O_EXITKILL |          // 调试器退出时杀死子进程
           PTRACE_O_TRACESYSGOOD |
           PTRACE_O_TRACECLONE |
           PTRACE_O_TRACEFORK);

    return 0;
}

// 等待事件
debug_event_t debugger_wait(debug_process_t *dbg) {
    pid_t pid = waitpid(dbg->pid, &dbg->status, 0);

    if (WIFEXITED(dbg->status)) {
        return EVENT_EXIT;
    }
    if (WIFSIGNALED(dbg->status)) {
        return EVENT_SIGNAL;
    }
    if (WIFSTOPPED(dbg->status)) {
        int sig = WSTOPSIG(dbg->status);

        // 系统调用停止
        if (sig == (SIGTRAP | 0x80)) {
            return EVENT_SYSCALL;
        }

        // 普通信号
        return EVENT_STOP;
    }

    return EVENT_UNKNOWN;
}

// 继续执行
void debugger_continue(debug_process_t *dbg, int sig) {
    ptrace(PTRACE_CONT, dbg->pid, NULL, sig);
    dbg->stopped = 0;
}

// 单步执行
void debugger_step(debug_process_t *dbg) {
    ptrace(PTRACE_SINGLESTEP, dbg->pid, NULL, NULL);
    debugger_wait(dbg);
}

// 系统调用跟踪
void debugger_trace_syscall(debug_process_t *dbg) {
    ptrace(PTRACE_SYSCALL, dbg->pid, NULL, NULL);
}
```

### 1.3 软件断点实现

```c
// 软件断点管理
typedef struct {
    void *addr;
    uint64_t original_byte;
    int enabled;
} breakpoint_t;

#define MAX_BREAKPOINTS 100
static breakpoint_t breakpoints[MAX_BREAKPOINTS];
static int bp_count = 0;

// x86断点指令
#define INT3 0xCC

// 设置断点
int set_breakpoint(debug_process_t *dbg, void *addr) {
    // 保存原始字节
    errno = 0;
    long data = ptrace(PTRACE_PEEKTEXT, dbg->pid, addr, NULL);
    if (data < 0 && errno != 0) return -1;

    // 保存到断点表
    breakpoints[bp_count].addr = addr;
    breakpoints[bp_count].original_byte = data & 0xFF;
    breakpoints[bp_count].enabled = 1;
    bp_count++;

    // 写入INT3 (0xCC)
    long trap = (data & ~0xFF) | INT3;
    if (ptrace(PTRACE_POKETEXT, dbg->pid, addr, trap) < 0) {
        return -1;
    }

    return 0;
}

// 移除断点
int remove_breakpoint(debug_process_t *dbg, void *addr) {
    for (int i = 0; i < bp_count; i++) {
        if (breakpoints[i].addr == addr && breakpoints[i].enabled) {
            // 恢复原始字节
            long data = ptrace(PTRACE_PEEKTEXT, dbg->pid, addr, NULL);
            long restored = (data & ~0xFF) | breakpoints[i].original_byte;
            ptrace(PTRACE_POKETEXT, dbg->pid, addr, restored);

            breakpoints[i].enabled = 0;
            return 0;
        }
    }
    return -1;
}

// 处理断点命中
void handle_breakpoint(debug_process_t *dbg) {
    // 获取当前指令指针
    ptrace(PTRACE_GETREGS, dbg->pid, NULL, &dbg->regs);
    void *pc = (void *)dbg->regs.rip;

    // PC指向断点指令之后（INT3是1字节）
    void *bp_addr = pc - 1;

    // 查找断点
    for (int i = 0; i < bp_count; i++) {
        if (breakpoints[i].addr == bp_addr && breakpoints[i].enabled) {
            printf("Breakpoint hit at %p\n", bp_addr);

            // 恢复原始指令
            long data = ptrace(PTRACE_PEEKTEXT, dbg->pid, bp_addr, NULL);
            long restored = (data & ~0xFF) | breakpoints[i].original_byte;
            ptrace(PTRACE_POKETEXT, dbg->pid, bp_addr, restored);

            // 回退指令指针
            dbg->regs.rip = (unsigned long)bp_addr;
            ptrace(PTRACE_SETREGS, dbg->pid, NULL, &dbg->regs);

            // 单步执行原始指令
            debugger_step(dbg);

            // 重新设置断点
            long trap = (restored & ~0xFF) | INT3;
            ptrace(PTRACE_POKETEXT, dbg->pid, bp_addr, trap);

            return;
        }
    }
}
```

### 1.4 硬件断点（调试寄存器）

```c
/*
 * x86-64调试寄存器：
 * DR0-DR3：断点地址
 * DR4-DR5：保留
 * DR6：调试状态
 * DR7：调试控制
 */

// DR7控制位布局
#define DR7_L0 (1 << 0)   // 本地断点0启用
#define DR7_G0 (1 << 1)   // 全局断点0启用
#define DR7_LE (1 << 8)   // 本地精确（已弃用）
#define DR7_GE (1 << 9)   // 全局精确（已弃用）
#define DR7_GD (1 << 13)  // 全局调试寄存器访问检测

// 断点类型（DR7 16-17位）
#define DR7_RW_EXECUTE  0x00  // 执行
#define DR7_RW_WRITE    0x01  // 写
#define DR7_RW_IO       0x02  // IO（未使用）
#define DR7_RW_ACCESS   0x03  // 读写

// 断点长度（DR7 18-19位）
#define DR7_LEN_1 0x00  // 1字节
#define DR7_LEN_2 0x01  // 2字节
#define DR7_LEN_4 0x03  // 4字节
#define DR7_LEN_8 0x02  // 8字节

// 通过ptrace设置硬件断点
int set_hardware_breakpoint(pid_t pid, int bp_num, void *addr,
                            int type, int len) {
    // 设置DR0-DR3
    unsigned long dr_addr = (unsigned long)addr;
    if (ptrace(PTRACE_POKEUSER, pid,
               offsetof(struct user, u_debugreg[bp_num]), dr_addr) < 0) {
        return -1;
    }

    // 读取DR7
    unsigned long dr7 = ptrace(PTRACE_PEEKUSER, pid,
                               offsetof(struct user, u_debugreg[7]), NULL);

    // 清除该断点的旧设置
    dr7 &= ~(0xF << (bp_num * 2));  // 清除L/G位
    dr7 &= ~(0xF << (16 + bp_num * 4));  // 清除RW/LEN

    // 设置新值
    dr7 |= (DR7_L0 << (bp_num * 2));  // 启用本地断点
    dr7 |= (type << (16 + bp_num * 4));  // 类型
    dr7 |= (len << (18 + bp_num * 4));  // 长度

    // 写入DR7
    if (ptrace(PTRACE_POKEUSER, pid,
               offsetof(struct user, u_debugreg[7]), dr7) < 0) {
        return -1;
    }

    return 0;
}

// 清除硬件断点
int clear_hardware_breakpoint(pid_t pid, int bp_num) {
    unsigned long dr7 = ptrace(PTRACE_PEEKUSER, pid,
                               offsetof(struct user, u_debugreg[7]), NULL);
    dr7 &= ~(0x3 << (bp_num * 2));  // 禁用L/G

    return ptrace(PTRACE_POKEUSER, pid,
                  offsetof(struct user, u_debugreg[7]), dr7);
}
```

---

## 2. 动态探针技术

### 2.1 Uprobes（用户空间探针）

```c
/*
 * Uprobes - 用户空间动态探针
 * • 无需重新编译程序
 * • 运行时插入探针点
 * • 可用于函数入口/出口跟踪
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <linux/bpf.h>

// 通过debugfs或perf_event_open使用uprobes

// 使用perf_event_open创建uprobe
int create_uprobe(const char *binary_path, uint64_t offset,
                  int is_return) {
    struct perf_event_attr attr = {
        .type = PERF_TYPE_PROBE,
        .size = sizeof(attr),
    };

    // 配置uprobe
    // attr.probe_type = is_return ? UPROBE_RETURN : UPROBE_ENTRY;
    // attr.uprobe_path = binary_path;
    // attr.probe_offset = offset;

    int fd = syscall(__NR_perf_event_open, &attr, -1, 0, -1, 0);
    return fd;
}

// 使用ftrace uprobes接口（更简单）
// echo 'p:myprobe /lib/x86_64-linux-gnu/libc.so.6:malloc' > /sys/kernel/debug/tracing/uprobe_events
// echo 1 > /sys/kernel/debug/tracing/events/uprobes/myprobe/enable
```

### 2.2 Kprobes（内核探针）

```c
/*
 * Kprobes - 内核动态探针
 * • 动态插入内核函数探针
 * • kprobe: 函数入口
 * • kretprobe: 函数返回
 * • 可用于内核调试和性能分析
 */

// 内核模块中的kprobe示例
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/ptrace.h>

// 目标函数：do_sys_open
static char symbol_name[KSYM_NAME_LEN] = "do_sys_open";
module_param_string(symbol, symbol_name, KSYM_NAME_LEN, S_IRUGO);

// 前置处理程序（进入函数时）
static int handler_pre(struct kprobe *p, struct pt_regs *regs) {
    // x86-64参数传递：rdi, rsi, rdx, rcx, r8, r9
    int dfd = regs->di;
    const char __user *filename = (const char __user *)regs->si;
    int flags = regs->dx;
    umode_t mode = regs->r10;

    char tmp[256];
    strncpy_from_user(tmp, filename, sizeof(tmp));

    printk(KERN_INFO "do_sys_open: dfd=%d, filename=%s, flags=0x%x\n",
           dfd, tmp, flags);

    return 0;
}

// 后置处理程序（函数返回时）
static void handler_post(struct kprobe *p, struct pt_regs *regs,
                         unsigned long flags) {
    // 返回值在rax
    long ret = regs->ax;
    printk(KERN_INFO "do_sys_open returned: %ld\n", ret);
}

// 错误处理程序
static int handler_fault(struct kprobe *p, struct pt_regs *regs, int trapnr) {
    printk(KERN_INFO "fault_handler: p->addr=0x%p, trap #%dn",
           p->addr, trapnr);
    return 0;
}

static struct kprobe kp = {
    .symbol_name = symbol_name,
    .pre_handler = handler_pre,
    .post_handler = handler_post,
    .fault_handler = handler_fault,
};

static int __init kprobe_init(void) {
    int ret = register_kprobe(&kp);
    if (ret < 0) {
        printk(KERN_INFO "register_kprobe failed: %d\n", ret);
        return ret;
    }
    printk(KERN_INFO "kprobe registered at %p\n", kp.addr);
    return 0;
}

static void __exit kprobe_exit(void) {
    unregister_kprobe(&kp);
    printk(KERN_INFO "kprobe unregistered\n");
}

module_init(kprobe_init);
module_exit(kprobe_exit);
MODULE_LICENSE("GPL");
```

### 2.3 使用eBPF进行动态追踪

```c
/*
 * eBPF uprobes/kprobes
 * • 更安全（验证器检查）
 * • 更高性能
 * • 更灵活的数据处理
 */

// hello.bpf.c
#include <vmlinux.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/usdt.bpf.h>

// 定义uprobe程序
SEC("uprobe//bin/bash:readline")
int BPF_KPROBE(trace_readline_entry, const char *prompt) {
    char buf[256];

    // 读取字符串参数
    bpf_probe_read_user_str(buf, sizeof(buf), prompt);

    // 输出到trace_pipe
    bpf_printk("bash readline: %s", buf);

    return 0;
}

// 定义uretprobe程序（函数返回）
SEC("uretprobe//bin/bash:readline")
int BPF_KPROBE(trace_readline_return, const char *ret) {
    char buf[256];

    if (ret) {
        bpf_probe_read_user_str(buf, sizeof(buf), ret);
        bpf_printk("bash input: %s", buf);
    }

    return 0;
}

// kprobe示例：跟踪open系统调用
SEC("kprobe/do_sys_openat2")
int BPF_KPROBE(trace_openat2, int dfd, const char *filename,
               struct open_how *how, size_t size) {
    char buf[256];

    bpf_probe_read_user_str(buf, sizeof(buf), filename);
    bpf_printk("openat: %s", buf);

    return 0;
}

char LICENSE[] SEC("license") = "GPL";
```

---

## 3. 系统调用拦截

### 3.1 系统调用劫持技术

```c
/*
 * 系统调用劫持方法：
 * 1. LD_PRELOAD - 用户空间库函数劫持
 * 2. ptrace - 跟踪系统调用
 * 3. 内核模块 - 修改系统调用表
 * 4. eBPF - 内核附着点
 */

// ========== 方法1: LD_PRELOAD ==========
// hook.c
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

// 原始系统调用类型
typedef int (*orig_open_t)(const char *pathname, int flags, mode_t mode);
typedef ssize_t (*orig_read_t)(int fd, void *buf, size_t count);
typedef ssize_t (*orig_write_t)(int fd, const void *buf, size_t count);

// 劫持open
int open(const char *pathname, int flags, mode_t mode) {
    orig_open_t orig = (orig_open_t)dlsym(RTLD_NEXT, "open");

    printf("[HOOK] open(%s, %d, %o)\n", pathname, flags, mode);

    return orig(pathname, flags, mode);
}

// 劫持openat
int openat(int dirfd, const char *pathname, int flags, mode_t mode) {
    typedef int (*orig_openat_t)(int, const char *, int, mode_t);
    orig_openat_t orig = (orig_openat_t)dlsym(RTLD_NEXT, "openat");

    printf("[HOOK] openat(%d, %s, %d)\n", dirfd, pathname, flags);

    return orig(dirfd, pathname, flags, mode);
}

// 劫持read
ssize_t read(int fd, void *buf, size_t count) {
    orig_read_t orig = (orig_read_t)dlsym(RTLD_NEXT, "read");

    ssize_t ret = orig(fd, buf, count);

    if (ret > 0) {
        printf("[HOOK] read(%d, ..., %zu) = %zd\n", fd, count, ret);
    }

    return ret;
}

// 编译：gcc -shared -fPIC hook.c -o hook.so -ldl
// 使用：LD_PRELOAD=./hook.so ./program

// ========== 方法2: ptrace系统调用跟踪 ==========

// 系统调用号和名称映射（x86-64）
const char *syscall_names[] = {
    [0]   = "read",
    [1]   = "write",
    [2]   = "open",
    [3]   = "close",
    [57]  = "fork",
    [59]  = "execve",
    // ... 更多
};

void trace_syscalls(pid_t pid) {
    int in_syscall = 0;
    struct user_regs_struct regs;

    while (1) {
        // 在系统调用进入和退出时停止
        ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        waitpid(pid, NULL, 0);

        ptrace(PTRACE_GETREGS, pid, NULL, &regs);

        if (!in_syscall) {
            // 系统调用进入
            long syscall_num = regs.orig_rax;
            printf("SYSCALL %s(%ld, %ld, %ld, %ld, %ld, %ld)\n",
                   syscall_names[syscall_num] ?: "unknown",
                   regs.rdi, regs.rsi, regs.rdx,
                   regs.r10, regs.r8, regs.r9);
            in_syscall = 1;
        } else {
            // 系统调用退出
            printf("RETURN = %ld\n", regs.rax);
            in_syscall = 0;
        }
    }
}
```

---

## 4. 内存监视

### 4.1 数据断点（Watchpoint）

```c
/*
 * 观察点：监视内存访问
 * • 写观察点
 * • 读观察点
 * • 访问观察点
 */

// 使用硬件观察点
int set_watchpoint(pid_t pid, void *addr, int type, int len) {
    // type: DR7_RW_WRITE, DR7_RW_ACCESS
    // len: DR7_LEN_1/2/4/8

    // 使用DR0-DR3设置观察点（与硬件断点共用）
    int dr_num = find_free_debug_register(pid);
    if (dr_num < 0) return -1;

    return set_hardware_breakpoint(pid, dr_num, addr, type, len);
}

// 通过mprotect实现软件观察点（粗粒度）
#include <sys/mman.h>
#include <signal.h>

void *watch_addr;
size_t watch_len;

void segv_handler(int sig, siginfo_t *info, void *context) {
    void *fault_addr = info->si_addr;

    // 检查是否在监视区域
    if (fault_addr >= watch_addr &&
        fault_addr < watch_addr + watch_len) {

        printf("Access to watched memory at %p\n", fault_addr);

        // 临时移除保护以允许访问
        mprotect(watch_addr, watch_len, PROT_READ | PROT_WRITE);

        // 设置单步标志，下一步后恢复保护
        // ...
    }
}

void setup_memory_watch(void *addr, size_t len) {
    watch_addr = addr;
    watch_len = len;

    // 设置SIGSEGV处理程序
    struct sigaction sa;
    sa.sa_sigaction = segv_handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);

    // 设置页面保护
    // 页面对齐
    void *page_start = (void *)(((unsigned long)addr + 4095) & ~4095);
    mprotect(page_start, len + (addr - page_start), PROT_NONE);
}
```

---

## 关联导航

### 前置知识

- [ptrace系统调用](../../03_System_Technology_Domains/01_System_Programming/readme.md)
- [ELF文件格式](../../02_Formal_Semantics_and_Physics/08_Linking_Loading_Topology/readme.md)
- [eBPF深度](../../03_System_Technology_Domains/01_System_Programming/06_eBPF_Deep.md)

### 后续延伸

- [动态分析工具](./05_Dynamic_Analysis_Tools.md)
- [性能追踪技术](./06_Performance_Tracing.md)

### 参考

- The Linux Programming Interface, Michael Kerrisk
- Systems Performance, Brendan Gregg
- GDB Internals Manual
