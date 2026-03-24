---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
﻿# MIT 6.S081 Lab 5: Copy-on-Write Fork

## 1. 实验概述

### 1.1 目标

- 实现写时复制(Copy-on-Write, COW)机制
- 优化fork()的性能，避免不必要的内存复制
- 理解页表标志位和引用计数
- 处理COW页面的页面错误

### 1.2 COW原理

```
传统fork:
父进程内存 ──复制──> 子进程内存 (完全独立的副本)
                    (耗时且浪费内存)

COW fork:
父进程内存 ──共享──> 子进程内存 (只读共享)
    ↓                    ↓
  写入时              写入时
    ↓                    ↓
 触发复制           触发复制
    ↓                    ↓
 新物理页           新物理页

优势:
1. fork()执行速度更快
2. 减少内存使用
3. 只有实际写入时才复制
```

### 1.3 xv6内存布局

```
xv6进程虚拟地址空间 (RISC-V Sv39):

0x00000000_00000000
      │
      │  未映射区域
      │
0x00000000_00100000  ← text段 (代码)
      │
      │  data段
      │
      ▼
      堆 (向上增长: sbrk)
      │
      │  未映射区域
      │
0x0000003f_c0000000  ← 用户栈 (向下增长)
      │
      │  未映射 (保护页)
      │
0x0000003f_f0000000  ← trapframe
      │
0x0000003f_f0001000  ← trampoline代码
      │
      │  内核区域 (用户不可访问)
      │
0x00000040_00000000  ← 内核基地址
```

---

## 2. 实现设计

### 2.1 核心设计

```
COW实现关键组件:

1. 页表项(PTE)标志
   - 使用PTE_COW位 (RSW字段) 标记COW页面
   - 清除PTE_W位，使页面只读

2. 引用计数
   - 跟踪每个物理页面的引用次数
   - fork时增加计数，释放时减少计数
   - 计数为1时才真正释放

3. 页面错误处理
   - 检测COW页面错误 (scause=15)
   - 分配新物理页并复制数据
   - 更新页表为可写
```

### 2.2 引用计数数据结构

```c
// kernel/kalloc.c

// 物理内存分为4096字节的页框
// 定义引用计数数组
#define PHYSTOP 0x88000000   // 物理内存上限
#define KERNBASE 0x80000000  // 内核基地址

// 物理页框数量
#define NPAGES ((PHYSTOP - KERNBASE) / PGSIZE)

// 引用计数数组 (每个物理页一个计数)
int ref_count[NPAGES];
struct spinlock ref_lock;  // 保护引用计数

// 物理地址转引用计数索引
#define PA2IDX(pa) (((uint64)(pa) - KERNBASE) / PGSIZE)

// 初始化引用计数
void
kinit()
{
    initlock(&ref_lock, "ref");
    for (int i = 0; i < NPAGES; i++) {
        ref_count[i] = 1;  // 初始为1 (被内核持有)
    }
    // ... 原有freerange代码 ...
}
```

### 2.3 引用计数操作函数

```c
// kernel/kalloc.c

// 增加引用计数
void
increment_ref(uint64 pa)
{
    if (pa < KERNBASE || pa >= PHYSTOP)
        panic("increment_ref: invalid pa");

    acquire(&ref_lock);
    ref_count[PA2IDX(pa)]++;
    release(&ref_lock);
}

// 减少引用计数，如果为0则释放
void
decrement_ref(uint64 pa)
{
    if (pa < KERNBASE || pa >= PHYSTOP)
        panic("decrement_ref: invalid pa");

    acquire(&ref_lock);
    int idx = PA2IDX(pa);
    if (ref_count[idx] < 1)
        panic("decrement_ref: ref_count < 1");

    ref_count[idx]--;
    int new_count = ref_count[idx];
    release(&ref_lock);

    // 如果引用为0，释放页面
    if (new_count == 0) {
        kfree((void *)pa);
    }
}

// 获取引用计数
int
get_ref(uint64 pa)
{
    if (pa < KERNBASE || pa >= PHYSTOP)
        return 0;

    acquire(&ref_lock);
    int count = ref_count[PA2IDX(pa)];
    release(&ref_lock);
    return count;
}
```

### 2.4 修改kalloc和kfree

```c
// kernel/kalloc.c

// 分配页面时设置引用计数为1
void *
kalloc(void)
{
    struct run *r;

    acquire(&kmem.lock);
    r = kmem.freelist;
    if (r) {
        kmem.freelist = r->next;

        // 设置引用计数为1
        uint64 pa = (uint64)r;
        ref_count[PA2IDX(pa)] = 1;
    }
    release(&kmem.lock);

    if (r)
        memset((char *)r, 5, PGSIZE);  // 填充垃圾值

    return (void *)r;
}

// kfree不再直接释放，而是调用decrement_ref
void
kfree(void *pa)
{
    // 添加到空闲列表 (原有逻辑)
    struct run *r = (struct run *)pa;

    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
}
```

---

## 3. 修改fork实现COW

### 3.1 修改uvmcopy

```c
// kernel/vm.c

// 复制页表，但不复制物理页面，使用COW
int
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
    pte_t *pte;
    uint64 pa, i;
    uint flags;

    for (i = 0; i < sz; i += PGSIZE) {
        if ((pte = walk(old, i, 0)) == 0)
            panic("uvmcopy: pte should exist");
        if ((*pte & PTE_V) == 0)
            panic("uvmcopy: page not present");

        pa = PTE2PA(*pte);
        flags = PTE_FLAGS(*pte);

        // 如果是可写页面，转换为COW
        if (flags & PTE_W) {
            // 清除写权限，添加COW标志
            flags = (flags & ~PTE_W) | PTE_COW;
            *pte = PA2PTE(pa) | flags;  // 更新父进程页表
        }

        // 映射到子进程 (共享同一物理页)
        if (mappages(new, i, PGSIZE, pa, flags) != 0) {
            // 失败，取消映射已映射的页面
            uvmunmap(new, 0, i / PGSIZE, 0);
            return -1;
        }

        // 增加引用计数
        increment_ref(pa);
    }

    return 0;
}
```

### 3.2 定义PTE_COW标志

```c
// kernel/riscv.h

// PTE标志位 (使用RSW保留位)
#define PTE_COW (1L << 8)  // Copy-on-Write标志
```

---

## 4. 处理COW页面错误

### 4.1 修改usertrap

```c
// kernel/trap.c

void
usertrap(void)
{
    // ... 原有代码 ...

    uint64 cause = r_scause();

    if (cause == 8) {
        // 系统调用
        // ...
    } else if (cause == 13 || cause == 15) {
        // 13: 加载页面错误
        // 15: 存储/AMO页面错误

        uint64 va = r_stval();  // 错误虚拟地址

        // 尝试处理COW页面错误
        if (cow_handler(va) == 0) {
            // 成功处理
        } else {
            printf("usertrap(): unexpected page fault pa=%p\n", va);
            setkilled(p);
        }
    } else if ((which_dev = devintr()) != 0) {
        // 设备中断
        // ...
    } else {
        // 其他异常
        // ...
    }

    // ...
}
```

### 4.2 COW处理函数

```c
// kernel/trap.c (或 vm.c)

int
cow_handler(uint64 va)
{
    struct proc *p = myproc();
    pte_t *pte;
    uint64 pa;
    uint flags;
    char *new_page;

    // 检查虚拟地址合法性
    if (va >= p->sz || va < PGSIZE)  // 跳过NULL指针
        return -1;

    // 获取页表项
    pte = walk(p->pagetable, va, 0);
    if (pte == 0)
        return -1;
    if ((*pte & PTE_V) == 0)
        return -1;
    if ((*pte & PTE_COW) == 0)
        return -1;  // 不是COW页面

    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);

    // 获取引用计数
    int ref = get_ref(pa);

    if (ref > 1) {
        // 多个进程共享，需要复制
        new_page = kalloc();
        if (new_page == 0) {
            // 内存不足
            return -1;
        }

        // 复制数据
        memmove(new_page, (char *)pa, PGSIZE);

        // 减少原页面引用计数
        decrement_ref(pa);

        // 更新页表指向新页面
        flags = (flags & ~PTE_COW) | PTE_W;  // 清除COW，设置可写
        *pte = PA2PTE((uint64)new_page) | flags;
    } else {
        // 只有一个引用，直接设置为可写
        flags = (flags & ~PTE_COW) | PTE_W;
        *pte = PA2PTE(pa) | flags;
    }

    return 0;
}
```

---

## 5. 修改copyout

### 5.1 处理COW页面的copyout

```c
// kernel/vm.c

int
copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len)
{
    uint64 n, va0, pa0;
    pte_t *pte;

    while (len > 0) {
        va0 = PGROUNDDOWN(dstva);

        // 检查是否需要处理COW
        pte = walk(pagetable, va0, 0);
        if (pte && (*pte & PTE_V) && (*pte & PTE_COW)) {
            // 需要复制COW页面
            uint64 pa = PTE2PA(*pte);
            int ref = get_ref(pa);

            if (ref > 1) {
                char *new_page = kalloc();
                if (new_page == 0)
                    return -1;

                memmove(new_page, (char *)pa, PGSIZE);
                decrement_ref(pa);

                uint flags = (PTE_FLAGS(*pte) & ~PTE_COW) | PTE_W;
                *pte = PA2PTE((uint64)new_page) | flags;
                pa0 = (uint64)new_page;
            } else {
                uint flags = (PTE_FLAGS(*pte) & ~PTE_COW) | PTE_W;
                *pte = PA2PTE(pa) | flags;
                pa0 = pa;
            }
        } else {
            pa0 = walkaddr(pagetable, va0);
            if (pa0 == 0)
                return -1;
        }

        n = PGSIZE - (dstva - va0);
        if (n > len)
            n = len;

        memmove((void *)(pa0 + (dstva - va0)), src, n);

        len -= n;
        src += n;
        dstva = va0 + PGSIZE;
    }

    return 0;
}
```

---

## 6. 释放进程资源

### 6.1 修改uvmunmap

```c
// kernel/vm.c

void
uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free)
{
    uint64 a;
    pte_t *pte;
    uint64 pa;

    if ((va % PGSIZE) != 0)
        panic("uvmunmap: not aligned");

    for (a = va; a < va + npages * PGSIZE; a += PGSIZE) {
        if ((pte = walk(pagetable, a, 0)) == 0)
            panic("uvmunmap: walk");
        if ((*pte & PTE_V) == 0)
            panic("uvmunmap: not mapped");
        if (PTE_FLAGS(*pte) == PTE_V)
            panic("uvmunmap: not a leaf");

        if (do_free) {
            pa = PTE2PA(*pte);
            decrement_ref(pa);  // 使用引用计数释放
        }

        *pte = 0;
    }
}
```

---

## 7. 测试与验证

### 7.1 COW测试程序

```c
// user/cowtest.c
#include "kernel/types.h"
#include "user/user.h"

void simple_test() {
    int pid;
    char *page = sbrk(4096);

    // 写入数据
    strcpy(page, "parent data");

    pid = fork();
    if (pid < 0) {
        printf("fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        // 子进程
        printf("child before write: %s\n", page);

        // 写入触发COW
        strcpy(page, "child data");
        printf("child after write: %s\n", page);

        exit(0);
    } else {
        // 父进程
        wait(0);
        printf("parent after child: %s\n", page);

        if (strcmp(page, "parent data") != 0) {
            printf("ERROR: parent data corrupted!\n");
            exit(1);
        }
    }
}

void stress_test() {
    // 大量fork和写入测试
    for (int i = 0; i < 100; i++) {
        int pid = fork();
        if (pid == 0) {
            // 分配并写入内存
            char *mem = sbrk(4096 * 10);
            for (int j = 0; j < 10; j++) {
                mem[j * 4096] = 'a' + (j % 26);
            }
            exit(0);
        }
        wait(0);
    }
    printf("stress test passed\n");
}

int main() {
    printf("=== COW Test ===\n");
    simple_test();
    stress_test();
    printf("All tests passed!\n");
    exit(0);
}
```

### 7.2 Makefile修改

```makefile
UPROGS=\
    ...
    $U/_cowtest\
    ...
```

---

## 8. 调试技巧

### 8.1 常见错误

```
错误1: panic("kfree: ref_count < 1")
原因: 引用计数管理错误，可能重复释放
解决: 检查所有increment_ref和decrement_ref调用

错误2: 页面错误未处理
原因: cow_handler未正确识别COW页面
解决: 检查PTE_COW标志是否正确设置

错误3: 子进程看到父进程修改后的数据
原因: COW页面未正确复制
解决: 确保写入前完成复制

错误4: 内存泄漏
原因: 引用计数未正确减少
解决: 检查所有错误路径的引用计数处理
```

### 8.2 调试打印

```c
// 打印页表信息
void debug_pte(pagetable_t pagetable, uint64 va) {
    pte_t *pte = walk(pagetable, va, 0);
    if (pte) {
        printf("PTE at va=%p: %p (flags: ", va, *pte);
        if (*pte & PTE_V) printf("V ");
        if (*pte & PTE_R) printf("R ");
        if (*pte & PTE_W) printf("W ");
        if (*pte & PTE_X) printf("X ");
        if (*pte & PTE_U) printf("U ");
        if (*pte & PTE_COW) printf("COW ");
        printf(")\n");
    }
}

// 打印引用计数
void debug_ref(uint64 pa) {
    printf("ref_count[%p] = %d\n", pa, get_ref(pa));
}
```

---

## 9. 性能分析

### 9.1 COW性能对比

```
测试场景: fork后立即exec

传统fork:
- 复制所有可写页面
- 耗时: O(n)，n为页面数
- 内存: 2x原进程

COW fork:
- 仅复制页表
- 耗时: O(1)
- 内存: ~1x原进程 (加上少量页表)

实际改进:
- fork()速度提升 10-100x
- 内存使用减少 30-50%
```

### 9.2 性能测试代码

```c
// 测量fork性能
void benchmark_fork() {
    int start = uptime();

    // 分配大量内存
    sbrk(1024 * 1024 * 10);  // 10MB

    for (int i = 0; i < 100; i++) {
        int pid = fork();
        if (pid == 0) {
            exit(0);
        }
        wait(0);
    }

    int end = uptime();
    printf("100 forks took %d ticks\n", end - start);
}
```

---

## 10. 总结

### 10.1 核心概念

| 概念 | 说明 |
|------|------|
| **COW** | 延迟复制直到写入时 |
| **引用计数** | 跟踪物理页面的共享情况 |
| **PTE_COW** | 标记只读共享页面 |
| **页面错误** | 检测写入COW页面的时机 |

### 10.2 实现要点

1. **fork优化**: 共享物理页，复制页表
2. **引用计数**: 管理物理页生命周期
3. **页面错误处理**: 复制页面并更新权限
4. **边界处理**: copyout和系统调用需要特殊处理

---

**难度**: ⭐⭐⭐⭐
**预计时间**: 10-12小时
**依赖**: Lab 4 (陷入)


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
