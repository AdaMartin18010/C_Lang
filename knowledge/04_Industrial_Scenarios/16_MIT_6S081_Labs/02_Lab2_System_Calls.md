# MIT 6.S081 Lab 2: System Calls

## 1. 实验概述

### 1.1 目标

- 理解xv6系统调用机制
- 学习添加新的系统调用
- 掌握内核与用户态的数据传递

### 1.2 系统调用流程

```
用户程序
    |
    | 调用系统调用函数 (如trace)
    v
用户库 (usys.S)
    |
    | 执行ecall指令 (陷入内核)
    v
内核陷阱处理 (trap.c)
    |
    | 分发到syscall()
    v
系统调用处理 (sysproc.c等)
    |
    | 执行内核代码
    v
返回用户态
```

---

## 2. trace系统调用

### 2.1 任务描述

实现`trace`系统调用，允许程序跟踪其调用的系统调用。

```c
// 使用示例
$ trace 2147483647 grep hello README
3: syscall read -> 1023
3: syscall read -> 961
3: syscall read -> 70
3: syscall read -> 0
```

### 2.2 实现步骤

#### 步骤1: 添加系统调用号

```c
// kernel/syscall.h
#define SYS_fork    1
#define SYS_exit    2
#define SYS_wait    3
// ...
#define SYS_trace  22  // 新增
```

#### 步骤2: 更新syscall函数表

```c
// kernel/syscall.c
extern uint64 sys_trace(void);  // 声明

static uint64 (*syscalls[])(void) = {
    [SYS_fork]    sys_fork,
    [SYS_exit]    sys_exit,
    // ...
    [SYS_trace]   sys_trace,  // 添加
};

// 添加系统调用名称（用于调试输出）
static char *syscall_names[] = {
    [SYS_fork]    "fork",
    [SYS_exit]    "exit",
    [SYS_wait]    "wait",
    [SYS_pipe]    "pipe",
    [SYS_read]    "read",
    [SYS_kill]    "kill",
    [SYS_exec]    "exec",
    [SYS_fstat]   "fstat",
    [SYS_chdir]   "chdir",
    [SYS_dup]     "dup",
    [SYS_getpid]  "getpid",
    [SYS_sbrk]    "sbrk",
    [SYS_sleep]   "sleep",
    [SYS_uptime]  "uptime",
    [SYS_open]    "open",
    [SYS_write]   "write",
    [SYS_mknod]   "mknod",
    [SYS_unlink]  "unlink",
    [SYS_link]    "link",
    [SYS_mkdir]   "mkdir",
    [SYS_close]   "close",
    [SYS_trace]   "trace",  // 添加
};
```

#### 步骤3: 修改proc结构

```c
// kernel/proc.h
struct proc {
    struct spinlock lock;

    // p->lock must be held when using these:
    enum procstate state;
    void *chan;
    int killed;
    int xstate;
    int pid;

    // wait_lock must be held when using this:
    struct proc *parent;

    // these are private to the process, so p->lock need not be held.
    uint64 kstack;
    uint64 sz;
    pagetable_t pagetable;
    struct trapframe *trapframe;
    struct context context;
    struct file *ofile[NOFILE];
    struct inode *cwd;
    char name[16];

    // 新增：跟踪掩码
    int trace_mask;
};
```

#### 步骤4: 实现sys_trace

```c
// kernel/sysproc.c
uint64
sys_trace(void)
{
    int mask;

    // 从用户态获取参数
    if (argint(0, &mask) < 0)
        return -1;

    // 设置当前进程的跟踪掩码
    myproc()->trace_mask = mask;

    return 0;
}
```

#### 步骤5: 修改syscall()函数

```c
// kernel/syscall.c
void
syscall(void)
{
    int num;
    struct proc *p = myproc();

    num = p->trapframe->a7;
    if (num > 0 && num < NELEM(syscalls) && syscalls[num]) {
        // 执行系统调用
        p->trapframe->a0 = syscalls[num]();

        // 新增：如果设置了trace，打印系统调用信息
        if ((p->trace_mask >> num) & 1) {
            printf("%d: syscall %s -> %d\n",
                   p->pid, syscall_names[num], p->trapframe->a0);
        }
    } else {
        printf("%d %s: unknown sys call %d\n",
               p->pid, p->name, num);
        p->trapframe->a0 = -1;
    }
}
```

#### 步骤6: 用户态库函数

```c
// user/user.h
int trace(int);

// user/usys.pl
entry("trace");

// user/trace.c (测试程序)
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int i;
    char *nargv[MAXARG];

    if (argc < 3) {
        fprintf(2, "Usage: trace mask command [args...]\n");
        exit(1);
    }

    // 设置跟踪掩码
    if (trace(atoi(argv[1])) < 0) {
        fprintf(2, "trace: trace failed\n");
        exit(1);
    }

    // 执行命令
    for (i = 2; i < argc && i < MAXARG; i++)
        nargv[i-2] = argv[i];
    nargv[argc-2] = 0;

    exec(nargv[0], nargv);

    fprintf(2, "trace: exec %s failed\n", nargv[0]);
    exit(1);
}
```

---

## 3. sysinfo系统调用

### 3.1 任务描述

实现`sysinfo`系统调用，收集系统信息（空闲内存、进程数）。

```c
struct sysinfo {
    uint64 freemem;   // 空闲内存字节数
    uint64 nproc;     // 进程数
};
```

### 3.2 实现步骤

#### 步骤1: 添加结构体定义

```c
// kernel/sysinfo.h
struct sysinfo {
    uint64 freemem;
    uint64 nproc;
};

// kernel/syscall.h
#define SYS_sysinfo 23
```

#### 步骤2: 实现空闲内存计数

```c
// kernel/kalloc.c
struct run {
    struct run *next;
};

struct {
    struct spinlock lock;
    struct run *freelist;
} kmem;

// 新增：统计空闲内存
uint64
count_freemem(void)
{
    uint64 count = 0;
    struct run *r;

    acquire(&kmem.lock);
    r = kmem.freelist;
    while (r) {
        count += PGSIZE;
        r = r->next;
    }
    release(&kmem.lock);

    return count;
}
```

#### 步骤3: 实现进程计数

```c
// kernel/proc.c
uint64
count_nproc(void)
{
    uint64 count = 0;
    struct proc *p;

    for (p = proc; p < &proc[NPROC]; p++) {
        acquire(&p->lock);
        if (p->state != UNUSED)
            count++;
        release(&p->lock);
    }

    return count;
}
```

#### 步骤4: 实现sys_sysinfo

```c
// kernel/sysproc.c
#include "sysinfo.h"

uint64
sys_sysinfo(void)
{
    uint64 addr;
    struct sysinfo info;

    // 获取用户态地址参数
    if (argaddr(0, &addr) < 0)
        return -1;

    // 填充信息
    info.freemem = count_freemem();
    info.nproc = count_nproc();

    // 复制到用户态
    if (copyout(myproc()->pagetable, addr, (char *)&info, sizeof(info)) < 0)
        return -1;

    return 0;
}
```

#### 步骤5: 用户态测试程序

```c
// user/sysinfotest.c
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/sysinfo.h"

int main() {
    struct sysinfo info;

    if (sysinfo(&info) < 0) {
        printf("sysinfo failed\n");
        exit(1);
    }

    printf("free memory: %l\n", info.freemem);
    printf("number of processes: %l\n", info.nproc);

    exit(0);
}
```

---

## 4. 关键概念详解

### 4.1 参数传递机制

```c
// 从用户态获取整数参数
int argint(int n, int *ip);

// 从用户态获取指针参数
int argaddr(int n, uint64 *ip);

// 从用户态获取字符串
int argstr(int n, char *buf, int max);

// 例子: sys_trace使用argint(0, &mask)
// a0寄存器存储第一个参数
```

### 4.2 用户/内核态数据传递

```c
// 从内核复制到用户空间
int copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len);

// 从用户空间复制到内核
int copyin(pagetable_t pagetable, char *dst, uint64 srcva, uint64 len);

// 复制字符串
int copyinstr(pagetable_t pagetable, char *dst, uint64 srcva, uint64 max);
```

### 4.3 进程结构

```c
struct proc {
    // ...
    pagetable_t pagetable;    // 用户页表
    struct trapframe *trapframe;  // 保存用户寄存器
    // ...
};

// myproc()获取当前进程
struct proc* myproc(void);
```

---

## 5. 系统调用流程图

```
用户程序调用trace(mask)
          |
          v
+----------------------------+
|  用户库函数 (user/usys.S)  |
|  li a7, SYS_trace          |
|  ecall                     |
+----------------------------+
          |
          | 陷入内核模式
          v
+----------------------------+
|  陷阱处理 (kernel/trap.c)  |
|  usertrap()                |
|  -> syscall()              |
+----------------------------+
          |
          v
+----------------------------+
|  系统调用分发             |
|  syscalls[a7]()            |
|  -> sys_trace()            |
+----------------------------+
          |
          v
+----------------------------+
|  执行内核代码             |
|  修改进程状态             |
+----------------------------+
          |
          v
+----------------------------+
|  返回用户态               |
|  usertrapret()            |
+----------------------------+
          |
          v
    回到用户程序
```

---

## 6. 测试

```bash
# 测试trace
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
$ trace 32 grep hello README
3: syscall read -> 1023
3: syscall read -> 961
...

# 测试sysinfo
$ sysinfotest
free memory: 12345678
number of processes: 5
```

---

## 7. 常见问题

### Q1: 系统调用号错误

A: 确保syscall.h、syscall.c、usys.pl中的编号一致

### Q2: 内核崩溃

A: 检查锁的获取/释放是否配对

### Q3: 用户态看到错误数据

A: 确保copyout正确使用

---

**难度**: ⭐⭐
**预计时间**: 3-5小时


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
