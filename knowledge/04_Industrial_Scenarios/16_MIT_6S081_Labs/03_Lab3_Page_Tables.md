# MIT 6.S081 Lab 3: Page Tables

## 1. 实验概述

### 1.1 目标

- 理解RISC-V页表机制
- 掌握虚拟地址到物理地址的转换
- 实现页表相关的内核功能

### 1.2 RISC-V页表基础

```
RISC-V Sv39分页 (39位虚拟地址)

虚拟地址格式:
| VPN[2] (9位) | VPN[1] (9位) | VPN[0] (9位) | 页内偏移 (12位) |
|--------------|--------------|--------------|-----------------|
|  L2索引      |   L1索引     |   L0索引     |   Offset        |
|     38-30    |    29-21     |    20-12     |     11-0        |

页表项 (PTE) 格式:
| PPN[2] | PPN[1] | PPN[0] | RSW | D | A | G | U | X | W | R | V |
| 53-28  | 27-19  | 18-10  | 9-8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |

标志位:
- V: 有效 (Valid)
- R: 可读 (Read)
- W: 可写 (Write)
- X: 可执行 (eXecute)
- U: 用户态可访问 (User)
- G: 全局映射 (Global)
- A: 已访问 (Accessed)
- D: 脏页 (Dirty)
```

---

## 2. 打印页表

### 2.1 任务

实现`vmprint()`函数，递归打印页表内容。

### 2.2 代码实现

```c
// kernel/vm.c

// 递归辅助函数
void vmprint_helper(pagetable_t pagetable, int level) {
    // 遍历512个PTE
    for (int i = 0; i < 512; i++) {
        pte_t pte = pagetable[i];

        // 只打印有效的PTE
        if (pte & PTE_V) {
            // 打印缩进
            for (int j = 0; j < level; j++) {
                printf(" ..");
            }

            // 打印PTE信息
            uint64 child = PTE2PA(pte);
            printf("%d: pte %p pa %p\n", i, pte, child);

            // 如果不是叶子节点(RWX=0)，递归打印
            if ((pte & (PTE_R | PTE_W | PTE_X)) == 0) {
                // 这是一个指向下一级页表的PTE
                vmprint_helper((pagetable_t)child, level + 1);
            }
        }
    }
}

// 主函数
void vmprint(pagetable_t pagetable) {
    printf("page table %p\n", pagetable);
    vmprint_helper(pagetable, 0);
}
```

### 2.3 在exec()中添加调用

```c
// kernel/exec.c
int exec(char *path, char **argv) {
    // ... 原有代码 ...

    // 添加在返回前
    if (p->pid == 1) {
        vmprint(p->pagetable);
    }

    return argc;
}
```

### 2.4 输出示例

```
page table 0x0000000087f6e000
..0: pte 0x0000000021fda801 pa 0x0000000087f6a000
....0: pte 0x0000000021fda401 pa 0x0000000087f69000
......0: pte 0x0000000021fdac1f pa 0x0000000087f6b000
......1: pte 0x0000000021fda00f pa 0x0000000087f68000
......2: pte 0x0000000021fd9c1b pa 0x0000000087f67000
..255: pte 0x0000000021fdb401 pa 0x0000000087f6d000
....511: pte 0x0000000021fdb001 pa 0x0000000087f6c000
......510: pte 0x0000000021fdd807 pa 0x0000000087f76000
......511: pte 0x0000000020001c0b pa 0x0000000080007000
```

---

## 3. 内核页表检测

### 3.1 任务

为每个进程添加一个内核页表的只读副本，用于检测内核栈溢出。

### 3.2 核心概念

```
传统xv6:
用户页表 ──切换──> 内核页表 (所有进程共享)

修改后:
用户页表 + 内核页表副本 (每个进程独立)
         ↑
    内核可以访问，用户态只读
```

### 3.3 实现

```c
// kernel/proc.h
struct proc {
    // ... 原有字段 ...
    pagetable_t kernel_pagetable;  // 进程的内核页表副本
};

// kernel/vm.c
// 复制内核页表到进程
pagetable_t proc_kpt_init() {
    pagetable_t kpt = uvmcreate();
    if (kpt == 0) return 0;

    // 复制内核映射 (uart, virtio, PLIC等)
    // UART
    mappages(kpt, UART0, PGSIZE, UART0, PTE_R | PTE_W);

    // Virtio MMIO
    mappages(kpt, VIRTIO0, PGSIZE, VIRTIO0, PTE_R | PTE_W);

    // PLIC
    mappages(kpt, PLIC, 0x400000, PLIC, PTE_R | PTE_W);

    // 内核代码和数据
    // ... 复制其他必要映射

    return kpt;
}

// kernel/proc.c
// 在allocproc()中初始化
static struct proc* allocproc(void) {
    // ...

    // 初始化内核页表副本
    p->kernel_pagetable = proc_kpt_init();
    if (p->kernel_pagetable == 0) {
        freeproc(p);
        release(&p->lock);
        return 0;
    }

    // ...
}
```

---

## 4. 加速系统调用

### 4.1 任务

使用USYSCALL页，让用户态可以直接读取某些系统调用信息（如getpid()），避免陷入内核。

### 4.2 设计

```
传统getpid():
用户态 → ecall → 内核态 → 读取pid → 返回

优化后:
用户态直接读取共享页中的pid (无需陷入内核)
```

### 4.3 实现

```c
// kernel/proc.h
struct usyscall {
    int pid;  // 进程ID (用户态只读)
};

struct proc {
    // ...
    struct usyscall *usyscall;  // USYSCALL页
};

// kernel/proc.c
// 在allocproc()中创建USYSCALL页
static struct proc* allocproc(void) {
    // ...

    // 分配USYSCALL页
    if ((p->usyscall = (struct usyscall *)kalloc()) == 0) {
        freeproc(p);
        release(&p->lock);
        return 0;
    }

    // 初始化pid
    p->usyscall->pid = p->pid;

    // 映射到用户态地址空间 (USYSCALL = 0x40000000)
    if (mappages(p->pagetable, USYSCALL, PGSIZE,
                 (uint64)(p->usyscall), PTE_R | PTE_U) < 0) {
        freeproc(p);
        release(&p->lock);
        return 0;
    }

    // ...
}

// 用户库 (user/usys.pl)
// 修改getpid()实现
// 如果USYSCALL页可用，直接读取，否则使用系统调用
```

### 4.4 用户态使用

```c
// user/user.h
struct usyscall {
    int pid;
};

// USYSCALL页地址 (与内核一致)
#define USYSCALL 0x40000000

static inline int getpid_fast(void) {
    struct usyscall *u = (struct usyscall *)USYSCALL;
    return u->pid;
}
```

---

## 5. 关键数据结构

### 5.1 页表操作函数

```c
// kernel/vm.c

// 创建空页表
pagetable_t uvmcreate(void);

// 映射页面
int mappages(pagetable_t pagetable, uint64 va, uint64 size,
             uint64 pa, int perm);

// 取消映射
void uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free);

// 复制页表
int uvmcopy(pagetable_t old, pagetable_t new, uint64 sz);

//  walk页表，返回PTE地址
pte_t *walk(pagetable_t pagetable, uint64 va, int alloc);

// 用户地址复制
int copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len);
int copyin(pagetable_t pagetable, char *dst, uint64 srcva, uint64 len);
int copyinstr(pagetable_t pagetable, char *dst, uint64 srcva, uint64 max);
```

### 5.2 地址转换流程

```
虚拟地址 (VA)
    |
    | 1. 提取VPN[2], VPN[1], VPN[0], Offset
    v
查找satp寄存器获取根页表
    |
    | 2. 使用VPN[2]索引根页表
    v
获得L2 PTE
    |
    | 3. 检查PTE_V, 提取PPN作为L1页表地址
    v
获得L1 PTE
    |
    | 4. 检查PTE_V, 提取PPN作为L0页表地址
    v
获得L0 PTE
    |
    | 5. 检查PTE_V和权限位
    v
提取PPN，拼接Offset
    |
    v
物理地址 (PA)
```

---

## 6. 调试技巧

### 6.1 常见错误

```
错误1: Page fault
原因: 访问了未映射的虚拟地址
解决: 检查mappages()调用

错误2: 权限错误
原因: 用户态访问内核页
解决: 检查PTE_U标志

错误3: 内存泄漏
原因: 未正确释放页表
解决: 检查uvmunmap()调用
```

### 6.2 调试命令

```bash
# 查看页表寄存器
(gdb) p/x $satp

# 查看特定虚拟地址
(gdb) x/10x 0x40000000

# 单步跟踪
(gdb) stepi
```

---

## 7. 总结

本实验核心概念：

- **三级页表**: RISC-V Sv39分页机制
- **PTE标志位**: V/R/W/X/U等权限控制
- **地址转换**: VA → PA的硬件/软件协作
- **性能优化**: USYSCALL页减少系统调用开销

---

**难度**: ⭐⭐⭐
**预计时间**: 6-8小时


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
