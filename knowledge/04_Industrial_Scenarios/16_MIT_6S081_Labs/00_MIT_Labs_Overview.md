# MIT 6.S081 Operating System Engineering Labs

## 1. 课程概述

### 1.1 MIT 6.S081简介

MIT 6.S081（原6.828）是麻省理工学院的操作系统工程课程，使用xv6操作系统作为教学平台：

| 项目 | 内容 |
|------|------|
| **课程名称** | Operating System Engineering |
| **教材** | xv6: a simple, Unix-like teaching operating system (RISC-V版) |
| **目标** | 理解操作系统核心概念：进程、内存、文件系统、并发 |
| **语言** | C + RISC-V汇编 |

### 1.2 xv6架构

```
xv6操作系统架构
├── 内核态 (Kernel Mode)
│   ├── 进程管理 (scheduler, fork, exec, wait)
│   ├── 内存管理 (paging, kalloc)
│   ├── 文件系统 (inode, buffer cache, logging)
│   ├── 系统调用接口
│   └── 设备驱动 (console, disk)
│
└── 用户态 (User Mode)
    ├── shell
    ├── 系统工具 (ls, cat, echo, etc.)
    └── 用户程序
```

---

## 2. Labs导航

### 2.1 Lab路线图

```
Lab 1: Xv6 and Unix utilities     [难度: ⭐]
Lab 2: System calls               [难度: ⭐⭐]
Lab 3: Page tables                [难度: ⭐⭐⭐]
Lab 4: Traps                      [难度: ⭐⭐⭐]
Lab 5: Copy-on-Write Fork         [难度: ⭐⭐⭐⭐]
Lab 6: Multithreading             [难度: ⭐⭐⭐]
Lab 7: Network driver             [难度: ⭐⭐⭐⭐]
Lab 8: Lock                       [难度: ⭐⭐⭐⭐]
Lab 9: File system                [难度: ⭐⭐⭐]
Lab 10: mmap                      [难度: ⭐⭐⭐⭐]
```

---

## 3. Lab 1详解: Unix Utilities

### 3.1 sleep

**目标**: 实现sleep命令

```c
// user/sleep.c
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(2, "Usage: sleep ticks\n");
        exit(1);
    }
    sleep(atoi(argv[1]));
    exit(0);
}
```

### 3.2 pingpong

**目标**: 使用管道在父子进程间通信

```c
// user/pingpong.c
#include "kernel/types.h"
#include "user/user.h"

int main() {
    int p1[2], p2[2];
    pipe(p1); pipe(p2);

    if (fork() == 0) {
        // 子进程
        char buf[1];
        read(p1[0], buf, 1);
        printf("%d: received ping\n", getpid());
        write(p2[1], buf, 1);
        exit(0);
    } else {
        // 父进程
        char buf[1] = {'x'};
        write(p1[1], buf, 1);
        read(p2[0], buf, 1);
        printf("%d: received pong\n", getpid());
        wait(0);
        exit(0);
    }
}
```

---

## 4. Lab 2详解: System Calls

### 4.1 trace系统调用

```c
// kernel/sysproc.c
uint64 sys_trace(void) {
    int mask;
    if (argint(0, &mask) < 0)
        return -1;
    myproc()->trace_mask = mask;
    return 0;
}

// kernel/syscall.c
void syscall(void) {
    int num = p->trapframe->a7;
    p->trapframe->a0 = syscalls[num]();

    // 新增: 如果设置了trace，打印系统调用
    if ((p->trace_mask >> num) & 1) {
        printf("%d: syscall %s -> %d\n",
               p->pid, syscall_names[num], p->trapframe->a0);
    }
}
```

---

## 5. Lab 3详解: Page Tables

### 5.1 打印页表

```c
// kernel/vm.c
void vmprint(pagetable_t pagetable) {
    printf("page table %p\n", pagetable);
    vmprint_helper(pagetable, 0);
}

void vmprint_helper(pagetable_t pagetable, int level) {
    for (int i = 0; i < 512; i++) {
        pte_t pte = pagetable[i];
        if (pte & PTE_V) {
            printf(".. ..%d: pte %p pa %p\n", i, pte, PTE2PA(pte));
            if ((pte & (PTE_R|PTE_W|PTE_X)) == 0) {
                vmprint_helper((pagetable_t)PTE2PA(pte), level + 1);
            }
        }
    }
}
```

---

## 6. 学习建议

### 6.1 推荐顺序

```
1. 阅读xv6教材相关章节
2. 查看Lab handout
3. 从简单函数开始实现
4. 使用gdb调试内核
5. 运行测试用例验证
```

### 6.2 调试技巧

```bash
make qemu-gdb

# 另一个终端
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
gdb-multiarch kernel/kernel
(gdb) target remote localhost:26000
(gdb) b main
(gdb) c
```

---

**文档版本**: 1.0
**创建日期**: 2026-03-19
**维护者**: C_Lang Knowledge Base Team


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
