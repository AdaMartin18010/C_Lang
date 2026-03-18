# MIT 6.S081 Lab 10: mmap

## 1. 实验概述

### 1.1 目标

- 实现`mmap`和`munmap`系统调用
- 理解内存映射文件机制
- 掌握懒加载(lazy allocation)技术

### 1.2 mmap概念

```text
mmap: 将文件映射到进程的虚拟地址空间

传统文件I/O:
read/write系统调用 ──► 数据拷贝到内核缓冲区 ──► 再拷贝到用户空间
(两次拷贝)

mmap文件I/O:
虚拟内存 ──► 页错误 ──► 从文件直接加载到物理页
(零拷贝)

优势:
- 简化文件访问(像访问内存一样)
- 支持进程间共享内存
- 懒加载(按需读取)
- 内核自动管理缓存
```

---

## 2. 系统设计

### 2.1 数据结构

```c
// kernel/proc.h

// VMA (Virtual Memory Area) 结构
struct vma {
    int valid;              // 是否有效
    uint64 start;          // 起始地址
    uint64 end;            // 结束地址
    int prot;              // 保护标志 (PROT_READ/PROT_WRITE)
    int flags;             // 标志 (MAP_SHARED/MAP_PRIVATE)
    struct file *file;     // 映射的文件
    uint64 offset;         // 文件偏移
    int fd;                // 文件描述符
};

#define NVMA 16  // 每个进程最多16个VMA

struct proc {
    // ... 原有字段 ...
    struct vma vmas[NVMA];  // VMA数组
};
```

### 2.2 标志定义

```c
// user/user.h

// 保护标志
#define PROT_NONE   0x0
#define PROT_READ   0x1
#define PROT_WRITE  0x2
#define PROT_EXEC   0x4

// 映射标志
#define MAP_SHARED   0x01   // 共享映射，修改写回文件
#define MAP_PRIVATE  0x02   // 私有映射，修改不影响文件
#define MAP_ANONYMOUS 0x20  // 匿名映射(无文件)
#define MAP_FIXED    0x10   // 固定地址映射
```

---

## 3. mmap实现

### 3.1 系统调用

```c
// kernel/sysfile.c

uint64 sys_mmap(void) {
    uint64 addr;
    int length, prot, flags, fd;
    struct file *f;
    int offset;
    struct proc *p = myproc();
    struct vma *v;

    // 获取参数
    if (argaddr(0, &addr) < 0 || argint(1, &length) < 0 ||
        argint(2, &prot) < 0 || argint(3, &flags) < 0 ||
        argint(4, &fd) < 0 || argint(5, &offset) < 0)
        return -1;

    // 对齐长度
    length = PGROUNDUP(length);

    // 获取文件
    if (fd >= 0) {
        if (fd < 0 || fd >= NOFILE || (f = p->ofile[fd]) == 0)
            return -1;

        // 检查权限
        if ((prot & PROT_READ) && !f->readable)
            return -1;
        if ((prot & PROT_WRITE) && !f->writable && (flags & MAP_SHARED))
            return -1;
    } else {
        f = 0;
    }

    // 寻找空闲VMA
    for (v = p->vmas; v < p->vmas + NVMA; v++) {
        if (!v->valid)
            break;
    }

    if (v >= p->vmas + NVMA)
        return -1;  // 没有空闲VMA

    // 计算映射地址
    // 从进程内存顶部向下查找空闲区域
    addr = MMAPAREA_END;
    for (struct vma *pv = p->vmas; pv < p->vmas + NVMA; pv++) {
        if (pv->valid && pv->end > addr - length && pv->start < addr) {
            addr = pv->start;
        }
    }
    addr = PGROUNDDOWN(addr - length);

    if (addr < MMAPAREA_START)
        return -1;  // 地址空间不足

    // 填充VMA
    v->valid = 1;
    v->start = addr;
    v->end = addr + length;
    v->prot = prot;
    v->flags = flags;
    v->file = f;
    v->offset = offset;
    v->fd = fd;

    // 增加文件引用计数
    if (f)
        filedup(f);

    return addr;
}
```

### 3.2 页错误处理

```c
// kernel/trap.c

void usertrap(void) {
    // ... 原有代码 ...

    uint64 va = r_stval();
    struct proc *p = myproc();

    // 检查是否是mmap区域的页错误
    struct vma *v = 0;
    for (int i = 0; i < NVMA; i++) {
        if (p->vmas[i].valid &&
            va >= p->vmas[i].start &&
            va < p->vmas[i].end) {
            v = &p->vmas[i];
            break;
        }
    }

    if (v) {
        // 懒加载mmap页面
        if (mmap_handler(p, v, va) < 0) {
            printf("mmap_handler failed\n");
            setkilled(p);
        }
        // 成功，返回继续执行
    } else {
        // 原有的页错误处理
        // ...
    }

    // ...
}

int mmap_handler(struct proc *p, struct vma *v, uint64 va) {
    va = PGROUNDDOWN(va);
    char *mem = kalloc();

    if (mem == 0)
        return -1;

    memset(mem, 0, PGSIZE);

    // 从文件读取数据(如果有)
    if (v->file) {
        ilock(v->file->ip);
        uint offset = v->offset + (va - v->start);
        int n = readi(v->file->ip, 0, (uint64)mem, offset, PGSIZE);
        iunlock(v->file->ip);

        if (n < 0) {
            kfree(mem);
            return -1;
        }
    }

    // 计算权限
    int perm = PTE_U;
    if (v->prot & PROT_READ)
        perm |= PTE_R;
    if (v->prot & PROT_WRITE)
        perm |= PTE_W;
    if (v->prot & PROT_EXEC)
        perm |= PTE_X;

    // 映射页面
    if (mappages(p->pagetable, va, PGSIZE, (uint64)mem, perm) < 0) {
        kfree(mem);
        return -1;
    }

    return 0;
}
```

---

## 4. munmap实现

```c
// kernel/sysfile.c

uint64 sys_munmap(void) {
    uint64 addr;
    int length;
    struct proc *p = myproc();
    struct vma *v = 0;

    // 获取参数
    if (argaddr(0, &addr) < 0 || argint(1, &length) < 0)
        return -1;

    addr = PGROUNDDOWN(addr);
    length = PGROUNDUP(length);

    // 查找对应的VMA
    for (int i = 0; i < NVMA; i++) {
        if (p->vmas[i].valid &&
            addr >= p->vmas[i].start &&
            addr < p->vmas[i].end) {
            v = &p->vmas[i];
            break;
        }
    }

    if (!v)
        return -1;

    // 写回修改的页面(如果是共享映射)
    if ((v->flags & MAP_SHARED) && v->file) {
        // 遍历映射区域，写回脏页
        for (uint64 a = addr; a < addr + length; a += PGSIZE) {
            pte_t *pte = walk(p->pagetable, a, 0);
            if (pte && (*pte & PTE_V) && (*pte & PTE_D)) {
                // 页面被修改，写回文件
                uint64 pa = PTE2PA(*pte);
                uint offset = v->offset + (a - v->start);

                begin_op();
                ilock(v->file->ip);
                writei(v->file->ip, 0, pa, offset, PGSIZE);
                iunlock(v->file->ip);
                end_op();
            }
        }
    }

    // 取消页面映射
    uvmunmap(p->pagetable, addr, length / PGSIZE, 1);

    // 更新VMA
    if (addr == v->start && length == (v->end - v->start)) {
        // 完全卸载
        v->valid = 0;
        if (v->file) {
            fileclose(v->file);
            v->file = 0;
        }
    } else if (addr == v->start) {
        // 从头部卸载
        v->start += length;
        v->offset += length;
    } else if (addr + length == v->end) {
        // 从尾部卸载
        v->end -= length;
    } else {
        // 中间卸载(简化处理：不支持)
        return -1;
    }

    return 0;
}
```

---

## 5. fork和exit处理

### 5.1 fork时复制VMA

```c
// kernel/proc.c

int fork(void) {
    // ... 原有代码 ...

    // 复制父进程的VMA到子进程
    for (int i = 0; i < NVMA; i++) {
        if (p->vmas[i].valid) {
            np->vmas[i] = p->vmas[i];
            if (np->vmas[i].file)
                filedup(np->vmas[i].file);
        }
    }

    // ...
}
```

### 5.2 exit时清理VMA

```c
// kernel/proc.c

void exit(int status) {
    // ... 原有代码 ...

    // 清理VMA
    for (int i = 0; i < NVMA; i++) {
        if (p->vmas[i].valid) {
            // 类似munmap的处理
            munmap_handler(p, &p->vmas[i]);
            p->vmas[i].valid = 0;
        }
    }

    // ...
}
```

---

## 6. 测试

```c
// user/mmaptest.c
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/memlayout.h"

int main() {
    int fd;
    char *addr;
    int n;

    // 测试1: 文件映射
    fd = open("testfile", O_CREATE | O_RDWR);
    if (fd < 0) {
        printf("cannot create testfile\n");
        exit(1);
    }

    // 写入测试数据
    write(fd, "hello mmap", 10);

    // 映射文件
    addr = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        printf("mmap failed\n");
        exit(1);
    }

    // 通过内存访问修改文件
    addr[0] = 'H';

    // 卸载映射
    munmap(addr, 4096);
    close(fd);

    // 验证修改
    fd = open("testfile", O_RDONLY);
    char buf[10];
    read(fd, buf, 10);
    if (buf[0] != 'H') {
        printf("mmap modification failed\n");
        exit(1);
    }
    close(fd);

    printf("mmap test passed\n");
    exit(0);
}
```

---

**难度**: ⭐⭐⭐⭐
**预计时间**: 8-10小时
