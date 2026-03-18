# MIT 6.S081 Lab 5: Copy-on-Write Fork

## 1. 实验概述

### 1.1 目标

- 理解写时复制（Copy-on-Write, COW）机制
- 实现COW fork以优化进程创建性能
- 掌握引用计数和页表标记

### 1.2 传统fork vs COW fork

```
传统fork():
父进程                    子进程
┌─────────┐              ┌─────────┐
│ 代码段  │  ──复制──>   │ 代码段  │  (共享，只读)
├─────────┤              ├─────────┤
│ 数据段  │  ──复制──>   │ 数据段  │  (完整复制)
├─────────┤              ├─────────┤
│ 堆      │  ──复制──>   │ 堆      │  (完整复制)
└─────────┘              └─────────┘

问题: fork后立即exec()，复制 wasted

COW fork():
父进程                    子进程
┌─────────┐              ┌─────────┐
│ 代码段  │  <─共享─>    │ 代码段  │  (共享，只读)
├─────────┤              ├─────────┤
│ 数据段  │  <─共享─>    │ 数据段  │  (共享，COW标记)
├─────────┤              ├─────────┤
│ 堆      │  <─共享─>    │ 堆      │  (共享，COW标记)
└─────────┘              └─────────┘

直到一方写入时才复制被修改的页
```

---

## 2. COW核心机制

### 2.1 页表标记

```c
// 使用RISC-V PTE的RSW位 (Reserved for Software)
// 位8-9为软件保留，可用于COW标记

#define PTE_COW (1L << 8)  // COW标记位

// 页表项标志组合
// 只读 + COW: 表示共享页面，需要复制才能写入
// 可写: 表示私有页面，可直接写入
```

### 2.2 引用计数

```c
// kernel/kalloc.c

// 每个物理页需要一个引用计数
// 使用数组来存储引用计数 (假设最多PHYSTOP页)
#define PGREFNUM(pa) (((pa) - KERNBASE) / PGSIZE)

struct {
    struct spinlock lock;
    int count[PHYSTOP / PGSIZE];  // 引用计数数组
} pgref;

void pgref_init(void) {
    initlock(&pgref.lock, "pgref");
    // 初始化为1 (kernel使用的页)
    for (int i = 0; i < PHYSTOP / PGSIZE; i++)
        pgref.count[i] = 1;
}

// 增加引用
void pgref_inc(uint64 pa) {
    acquire(&pgref.lock);
    int idx = PGREFNUM(pa);
    if (idx < 0 || idx >= PHYSTOP / PGSIZE)
        panic("pgref_inc");
    pgref.count[idx]++;
    release(&pgref.lock);
}

// 减少引用，返回是否为0
int pgref_dec(uint64 pa) {
    acquire(&pgref.lock);
    int idx = PGREFNUM(pa);
    if (idx < 0 || idx >= PHYSTOP / PGSIZE)
        panic("pgref_dec");
    if (pgref.count[idx] < 1)
        panic("pgref_dec: count < 1");
    pgref.count[idx]--;
    int count = pgref.count[idx];
    release(&pgref.lock);
    return count == 0;
}

// 获取引用计数
int pgref_get(uint64 pa) {
    acquire(&pgref.lock);
    int idx = PGREFNUM(pa);
    if (idx < 0 || idx >= PHYSTOP / PGSIZE)
        panic("pgref_get");
    int count = pgref.count[idx];
    release(&pgref.lock);
    return count;
}
```

---

## 3. 修改uvmcopy实现COW

### 3.1 原始uvmcopy

```c
// 原始版本：复制所有页面
int uvmcopy(pagetable_t old, pagetable_t new, uint64 sz) {
    pte_t *pte;
    uint64 pa, i;
    uint flags;
    char *mem;

    for (i = 0; i < sz; i += PGSIZE) {
        if ((pte = walk(old, i, 0)) == 0)
            panic("uvmcopy: pte should exist");
        if ((*pte & PTE_V) == 0)
            panic("uvmcopy: page not present");

        pa = PTE2PA(*pte);
        flags = PTE_FLAGS(*pte);

        // 分配新页并复制
        if ((mem = kalloc()) == 0)
            goto err;
        memmove(mem, (char*)pa, PGSIZE);

        // 映射到新页表
        if (mappages(new, i, PGSIZE, (uint64)mem, flags) != 0) {
            kfree(mem);
            goto err;
        }
    }
    return 0;

err:
    uvmunmap(new, 0, i / PGSIZE, 1);
    return -1;
}
```

### 3.2 COW版本uvmcopy

```c
// COW版本：共享页面，标记COW
int uvmcopy(pagetable_t old, pagetable_t new, uint64 sz) {
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

        // 如果是可写页，改为只读并标记COW
        if (flags & PTE_W) {
            flags = (flags | PTE_COW) & ~PTE_W;
            *pte = PA2PTE(pa) | flags;  // 修改父进程页表
        }

        // 增加引用计数
        pgref_inc(pa);

        // 映射到新页表 (共享同一物理页)
        if (mappages(new, i, PGSIZE, pa, flags) != 0) {
            // 注意：这里不能释放pa，因为是共享的
            goto err;
        }
    }
    return 0;

err:
    // 减少引用计数
    for (uint64 j = 0; j < i; j += PGSIZE) {
        pte = walk(new, j, 0);
        if (pte && (*pte & PTE_V)) {
            pa = PTE2PA(*pte);
            if (pgref_dec(pa))
                kfree((void*)pa);
        }
    }
    return -1;
}
```

---

## 4. 处理COW页错误

### 4.1 修改usertrap

```c
// kernel/trap.c

void usertrap(void) {
    // ... 原有代码 ...

    uint64 scause = r_scause();

    if (scause == 8) {
        // 系统调用处理
        // ...
    } else if (scause == 13 || scause == 15) {
        // 13: 加载页错误 (Load page fault)
        // 15: 存储/AMO页错误 (Store/AMO page fault)

        uint64 va = r_stval();  // 出错的虚拟地址

        if (va >= p->sz) {
            // 访问超出进程内存
            printf("usertrap(): page fault va=%p pid=%d\n", va, p->pid);
            setkilled(p);
        } else if (cowhandle(p->pagetable, va) < 0) {
            // COW处理失败
            printf("usertrap(): cowhandle failed\n");
            setkilled(p);
        }
        // 成功处理，返回继续执行

    } else if ((which_dev = devintr()) != 0) {
        // 中断处理
        // ...
    } else {
        // 其他异常
        printf("usertrap(): unexpected scause %p pid=%d\n", scause, p->pid);
        setkilled(p);
    }

    // ...
}
```

### 4.2 COW处理函数

```c
// kernel/vm.c

// 处理COW页错误
// 返回值: 0成功，-1失败
int cowhandle(pagetable_t pagetable, uint64 va) {
    pte_t *pte;
    uint64 pa, newpa;
    uint flags;
    char *mem;

    // 对齐到页边界
    va = PGROUNDDOWN(va);

    // 查找PTE
    if (va >= MAXVA)
        return -1;

    pte = walk(pagetable, va, 0);
    if (pte == 0)
        return -1;
    if ((*pte & PTE_V) == 0)
        return -1;
    if ((*pte & PTE_COW) == 0)
        return -1;  // 不是COW页

    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);

    // 检查引用计数
    if (pgref_get(pa) == 1) {
        // 只有一个引用，直接改为可写
        *pte = (*pte | PTE_W) & ~PTE_COW;
    } else {
        // 多个引用，需要复制
        if ((mem = kalloc()) == 0)
            return -1;  // 内存不足

        // 复制页面内容
        memmove(mem, (char*)pa, PGSIZE);

        // 减少原页引用
        pgref_dec(pa);

        // 更新PTE指向新页
        newpa = (uint64)mem;
        flags = (flags | PTE_W) & ~PTE_COW;
        *pte = PA2PTE(newpa) | flags;
    }

    return 0;
}
```

---

## 5. 修改copyout支持COW

### 5.1 问题

`copyout`用于从内核复制数据到用户空间。如果目标页是COW页，需要处理。

### 5.2 解决方案

```c
// kernel/vm.c

int copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len) {
    uint64 n, va0, pa0;
    pte_t *pte;

    while (len > 0) {
        va0 = PGROUNDDOWN(dstva);

        // 处理COW页
        pte = walk(pagetable, va0, 0);
        if (pte && (*pte & PTE_V) && (*pte & PTE_COW)) {
            if (cowhandle(pagetable, va0) < 0)
                return -1;
        }

        pa0 = walkaddr(pagetable, va0);
        if (pa0 == 0)
            return -1;

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

## 6. 修改uvmunmap释放内存

```c
// kernel/vm.c

void uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free) {
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
            // 使用引用计数决定是否释放
            if (pgref_dec(pa))
                kfree((void*)pa);
        }

        *pte = 0;
    }
}
```

---

## 7. COW流程图

```
父进程调用fork()
    |
    v
调用uvmcopy() [COW版本]
    |
    v
对于每个用户页:
    ├── 如果可写:
    │   ├── 标记为只读+PTE_COW
    │   ├── 增加引用计数
    │   └── 子进程共享该页
    └── 如果只读:
        └── 直接共享
    |
    v
父子进程继续执行
    |
    v
子/父进程尝试写入COW页
    |
    v
触发页错误 (Store page fault)
    |
    v
trap处理调用cowhandle()
    |
    v
检查引用计数:
    ├── 计数==1:
    │   └── 直接改为可写
    └── 计数>1:
        ├── 分配新页
        ├── 复制内容
        ├── 减少原页引用
        └── 映射新页(可写)
    |
    v
返回用户态继续执行
```

---

## 8. 测试与验证

### 8.1 cowtest

```c
// user/cowtest.c
#include "kernel/types.h"
#include "user/user.h"

void simpletest() {
    int pid;

    pid = fork();
    if (pid < 0) {
        printf("fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        // 子进程修改数据
        int x = 100;
        x = 200;  // 触发COW
        printf("child: x = %d\n", x);
        exit(0);
    } else {
        wait(0);
        printf("parent: child exited\n");
    }
}

int main() {
    simpletest();
    printf("COW tests passed\n");
    exit(0);
}
```

### 8.2 性能测试

```c
// 测试fork性能
void forktest() {
    int n = 1000;

    for (int i = 0; i < n; i++) {
        int pid = fork();
        if (pid == 0) {
            exit(0);  // 立即退出
        } else {
            wait(0);
        }
    }

    printf("forked %d times\n", n);
}
```

---

**难度**: ⭐⭐⭐⭐
**预计时间**: 8-10小时
