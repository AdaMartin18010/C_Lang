# MIT 6.S081 Lab 10: mmap

## 1. 实验概述

### 1.1 目标

- 实现mmap系统调用，支持内存映射文件
- 实现munmap系统调用，解除内存映射
- 理解惰性分配(lazy allocation)机制
- 处理页面错误，加载文件内容到内存
- 支持私有映射和共享映射

### 1.2 mmap概念

```
内存映射文件 (mmap):

传统文件访问:
┌──────────┐    ┌──────────┐    ┌──────────┐
│ 用户程序 │───►│  read()  │───►│ 内核缓冲 │
└──────────┘    └──────────┘    └──────────┘
                                    │
                                    ▼
                                ┌──────────┐
                                │ 磁盘文件 │
                                └──────────┘

mmap文件访问:
┌──────────┐         ┌──────────┐
│ 用户程序 │◄───────►│ 内存页面 │
└──────────┘         └──────────┘
                           │
                     ┌─────┴─────┐
                     ▼           ▼
                内存访问    页面错误
                     │           │
                     └─────┬─────┘
                           ▼
                      ┌──────────┐
                      │ 磁盘文件 │
                      └──────────┘

优势:
- 避免用户态/内核态数据拷贝
- 惰性加载 (只读实际访问的页)
- 支持进程间共享内存
- 简化文件访问代码
```

### 1.3 映射类型

```
映射类型组合:

1. 私有映射 (MAP_PRIVATE)
   - 写操作触发COW (Copy-on-Write)
   - 不影响原文件
   - 用于加载可执行文件

2. 共享映射 (MAP_SHARED)
   - 写操作直接修改文件
   - 修改对其他进程可见
   - 用于进程间通信

3. 匿名映射 (MAP_ANONYMOUS)
   - 不关联文件
   - 分配零填充的内存
   - 用于大规模内存分配

4. 固定映射 (MAP_FIXED)
   - 强制使用指定的虚拟地址
   - 可能替换已有映射
```

---

## 2. 数据结构设计

### 2.1 VMA (Virtual Memory Area)

```c
// kernel/proc.h

// 虚拟内存区域结构
struct vma {
    int valid;              // 是否有效
    uint64 addr;            // 起始虚拟地址
    int length;             // 映射长度
    int prot;               // 保护标志 (R/W/X)
    int flags;              // 映射标志 (PRIVATE/SHARED)
    struct file *file;      // 映射的文件 (可为NULL)
    int offset;             // 文件偏移
};

#define NVMA 16  // 每个进程最大VMA数量

// 在proc结构中添加
struct proc {
    // ... 原有字段 ...
    struct vma vmas[NVMA];   // VMA数组
};

// 保护标志
#define PROT_READ  0x1
#define PROT_WRITE 0x2
#define PROT_EXEC  0x4

// 映射标志
#define MAP_SHARED  0x01
#define MAP_PRIVATE 0x02
#define MAP_FIXED   0x10
#define MAP_ANONYMOUS 0x20
```

### 2.2 查找空闲VMA

```c
// kernel/proc.c

// 查找可用的VMA槽
struct vma*
find_free_vma(struct proc *p)
{
    for (int i = 0; i < NVMA; i++) {
        if (!p->vmas[i].valid)
            return &p->vmas[i];
    }
    return 0;
}

// 根据地址查找VMA
struct vma*
find_vma(struct proc *p, uint64 addr)
{
    for (int i = 0; i < NVMA; i++) {
        if (p->vmas[i].valid &&
            addr >= p->vmas[i].addr &&
            addr < p->vmas[i].addr + p->vmas[i].length) {
            return &p->vmas[i];
        }
    }
    return 0;
}
```

---

## 3. mmap系统调用实现

### 3.1 系统调用入口

```c
// kernel/sysfile.c

uint64
sys_mmap(void)
{
    uint64 addr;
    int length, prot, flags, fd, offset;
    struct file *file = 0;
    struct vma *vma;
    struct proc *p = myproc();

    // 获取参数
    if (argaddr(0, &addr) < 0 ||
        argint(1, &length) < 0 ||
        argint(2, &prot) < 0 ||
        argint(3, &flags) < 0 ||
        argint(4, &fd) < 0 ||
        argint(5, &offset) < 0)
        return -1;

    // 参数检查
    if (length <= 0 || length % PGSIZE != 0)
        return -1;
    if (offset < 0 || offset % PGSIZE != 0)
        return -1;

    // 如果不是匿名映射，需要文件
    if (!(flags & MAP_ANONYMOUS)) {
        if (fd < 0 || fd >= NOFILE || (file = p->ofile[fd]) == 0)
            return -1;

        // 检查文件权限与映射权限是否匹配
        if ((prot & PROT_READ) && !file->readable)
            return -1;
        if ((prot & PROT_WRITE) && !file->writable && (flags & MAP_SHARED))
            return -1;

        // 增加文件引用计数
        filedup(file);
    }

    // 查找空闲VMA
    vma = find_free_vma(p);
    if (!vma) {
        if (file)
            fileclose(file);
        return -1;
    }

    // 分配地址空间
    // 从进程堆顶开始查找
    addr = PGROUNDUP(p->sz);

    // 检查地址空间是否足够
    if (addr + length > MAXVA) {
        if (file)
            fileclose(file);
        return -1;
    }

    // 填充VMA
    vma->valid = 1;
    vma->addr = addr;
    vma->length = length;
    vma->prot = prot;
    vma->flags = flags;
    vma->file = file;
    vma->offset = offset;

    // 增加进程大小
    p->sz = addr + length;

    return addr;
}
```

### 3.2 映射地址空间但不分配物理页

```c
// mmap不立即分配物理页
// 而是记录映射信息到VMA
// 实际分配在页面错误时进行 (惰性分配)

// 用户空间看到的页表:
// 虚拟地址已分配，但PTE_V=0
// 首次访问触发页面错误
// 页面错误处理程序:
//   1. 查找对应VMA
//   2. 分配物理页
//   3. 如果是文件映射，读取文件内容
//   4. 建立映射
```

---

## 4. 页面错误处理

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
        // 页面错误
        uint64 va = r_stval();

        // 先尝试COW处理 (Lab 5)
        if (cow_handler(va) == 0) {
            // COW处理成功
        } else if (mmap_handler(va) == 0) {
            // mmap处理成功
        } else {
            printf("page fault: va=%p\n", va);
            setkilled(p);
        }
    }

    // ...
}
```

### 4.2 mmap页面错误处理

```c
// kernel/trap.c (或 vm.c)

int
mmap_handler(uint64 va)
{
    struct proc *p = myproc();
    struct vma *vma;
    char *pa;

    // 页面对齐
    va = PGROUNDDOWN(va);

    // 查找包含该地址的VMA
    vma = find_vma(p, va);
    if (!vma)
        return -1;  // 不是mmap区域

    // 检查权限
    int cause = r_scause();
    if (cause == 15) {  // Store/AMO page fault
        if (!(vma->prot & PROT_WRITE))
            return -1;  // 写保护错误
    } else if (cause == 13) {  // Load page fault
        if (!(vma->prot & PROT_READ))
            return -1;  // 读保护错误
    }

    // 分配物理页
    pa = kalloc();
    if (!pa)
        return -1;

    memset(pa, 0, PGSIZE);

    // 如果是文件映射，读取文件内容
    if (vma->file) {
        // 计算在文件中的偏移
        int offset = vma->offset + (va - vma->addr);
        int len = PGSIZE;
        if (offset + len > vma->file->ip->size)
            len = vma->file->ip->size - offset;

        if (len > 0) {
            ilock(vma->file->ip);
            readi(vma->file->ip, 0, (uint64)pa, offset, len);
            iunlock(vma->file->ip);
        }
    }

    // 设置页表权限
    int perm = PTE_U;  // 用户可访问
    if (vma->prot & PROT_READ)
        perm |= PTE_R;
    if (vma->prot & PROT_WRITE)
        perm |= PTE_W;
    if (vma->prot & PROT_EXEC)
        perm |= PTE_X;

    // 私有映射使用COW
    if ((vma->flags & MAP_PRIVATE) && (perm & PTE_W)) {
        perm = (perm & ~PTE_W) | PTE_COW;
    }

    // 建立映射
    if (mappages(p->pagetable, va, PGSIZE, (uint64)pa, perm) < 0) {
        kfree(pa);
        return -1;
    }

    // 如果是COW页面，增加引用计数
    if (perm & PTE_COW) {
        increment_ref((uint64)pa);
    }

    return 0;
}
```

---

## 5. munmap系统调用实现

### 5.1 munmap实现

```c
// kernel/sysfile.c

uint64
sys_munmap(void)
{
    uint64 addr;
    int length;
    struct proc *p = myproc();
    struct vma *vma;

    if (argaddr(0, &addr) < 0 ||
        argint(1, &length) < 0)
        return -1;

    // 参数检查
    if (addr % PGSIZE != 0 || length <= 0)
        return -1;

    // 查找包含该地址的VMA
    vma = find_vma(p, addr);
    if (!vma)
        return -1;

    // 检查范围
    if (addr < vma->addr || addr + length > vma->addr + vma->length)
        return -1;

    // 如果是共享映射且文件可写，写回修改
    if ((vma->flags & MAP_SHARED) && vma->file && vma->file->writable) {
        int offset = vma->offset + (addr - vma->addr);
        filewrite(vma->file, addr, length, offset);
    }

    // 解除页表映射
    uvmunmap(p->pagetable, addr, length / PGSIZE, 1);

    // 更新或删除VMA
    if (addr == vma->addr && length == vma->length) {
        // 完全匹配，删除整个VMA
        vma->valid = 0;
        if (vma->file) {
            fileclose(vma->file);
            vma->file = 0;
        }
    } else if (addr == vma->addr) {
        // 从头部解除映射
        vma->addr += length;
        vma->offset += length;
        vma->length -= length;
    } else if (addr + length == vma->addr + vma->length) {
        // 从尾部解除映射
        vma->length -= length;
    } else {
        // 从中间解除映射 - 需要分裂VMA
        // 简化处理：不支持
        panic("munmap: splitting VMA not supported");
    }

    return 0;
}
```

---

## 6. fork和exit处理

### 6.1 fork时复制VMA

```c
// kernel/proc.c

static struct proc*
allocproc(void)
{
    // ... 原有代码 ...
}

// 在fork中复制VMA
int
fork(void)
{
    // ... 原有代码 ...

    // 复制父进程的VMA到子进程
    for (int i = 0; i < NVMA; i++) {
        if (p->vmas[i].valid) {
            np->vmas[i] = p->vmas[i];
            if (np->vmas[i].file)
                filedup(np->vmas[i].file);
        }
    }

    // ... 复制页表等 ...

    return pid;
}
```

### 6.2 exit时清理VMA

```c
// kernel/proc.c

void
exit(int status)
{
    struct proc *p = myproc();

    // ... 原有代码 ...

    // 清理所有VMA
    for (int i = 0; i < NVMA; i++) {
        if (p->vmas[i].valid) {
            // 共享映射写回
            if ((p->vmas[i].flags & MAP_SHARED) &&
                p->vmas[i].file && p->vmas[i].file->writable) {
                filewrite(p->vmas[i].file,
                         p->vmas[i].addr,
                         p->vmas[i].length,
                         p->vmas[i].offset);
            }

            // 解除映射
            uvmunmap(p->pagetable,
                    p->vmas[i].addr,
                    p->vmas[i].length / PGSIZE, 1);

            // 关闭文件
            if (p->vmas[i].file)
                fileclose(p->vmas[i].file);

            p->vmas[i].valid = 0;
        }
    }

    // ... 原有代码 ...
}
```

---

## 7. 测试程序

### 7.1 基本mmap测试

```c
// user/mmaptest.c
#include "kernel/types.h"
#include "user/user.h"

void test_basic_mmap() {
    int fd;
    char *addr;

    // 创建测试文件
    fd = open("testfile", O_CREATE | O_RDWR);
    write(fd, "hello mmap!\n", 12);
    close(fd);

    // 映射文件
    fd = open("testfile", O_RDONLY);
    addr = mmap(0, 4096, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    if (addr == (char *)-1) {
        printf("mmap failed\n");
        exit(1);
    }

    // 读取映射内存
    printf("mapped content: %s", addr);

    // 解除映射
    munmap(addr, 4096);

    // 清理
    unlink("testfile");

    printf("basic mmap test passed\n");
}

void test_private_write() {
    int fd;
    char *addr;

    // 创建测试文件
    fd = open("testfile", O_CREATE | O_RDWR);
    write(fd, "original\n", 9);
    close(fd);

    // 映射文件
    fd = open("testfile", O_RDWR);
    addr = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd);

    // 修改映射内存 (触发COW)
    strcpy(addr, "modified\n");

    // 读取验证
    if (strcmp(addr, "modified\n") != 0) {
        printf("write to private mapping failed\n");
        exit(1);
    }

    munmap(addr, 4096);

    // 验证原文件未被修改
    char buf[20];
    fd = open("testfile", O_RDONLY);
    read(fd, buf, sizeof(buf));
    close(fd);

    if (strcmp(buf, "original\n") != 0) {
        printf("private mapping modified file!\n");
        exit(1);
    }

    unlink("testfile");
    printf("private write test passed\n");
}

void test_anonymous() {
    char *addr;

    // 匿名映射
    addr = mmap(0, 4096, PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (addr == (char *)-1) {
        printf("anonymous mmap failed\n");
        exit(1);
    }

    // 应该初始化为0
    for (int i = 0; i < 4096; i++) {
        if (addr[i] != 0) {
            printf("anonymous mapping not zeroed\n");
            exit(1);
        }
    }

    // 写入测试
    strcpy(addr, "test");

    munmap(addr, 4096);
    printf("anonymous mmap test passed\n");
}

int main() {
    test_basic_mmap();
    test_private_write();
    test_anonymous();
    printf("All mmap tests passed!\n");
    exit(0);
}
```

### 7.2 压力测试

```c
// 大规模映射测试
void stress_test() {
    char *addrs[100];

    // 创建多个映射
    for (int i = 0; i < 100; i++) {
        addrs[i] = mmap(0, 4096, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (addrs[i] == (char *)-1) {
            printf("mmap %d failed\n", i);
            exit(1);
        }

        // 写入触发实际分配
        addrs[i][0] = i;
    }

    // 验证
    for (int i = 0; i < 100; i++) {
        if (addrs[i][0] != i) {
            printf("verification failed at %d\n", i);
            exit(1);
        }
    }

    // 解除映射
    for (int i = 0; i < 100; i++) {
        munmap(addrs[i], 4096);
    }

    printf("stress test passed\n");
}
```

---

## 8. 调试技巧

### 8.1 常见问题

```
问题1: mmap返回失败
原因: VMA数量不足或地址空间不足
解决: 检查NVMA和MAXVA设置

问题2: 页面错误未处理
原因: mmap_handler未注册或VMA查找失败
解决: 检查usertrap中的调用和find_vma实现

问题3: 文件修改未写回
原因: munmap或exit时未正确处理共享映射
解决: 检查写回逻辑

问题4: 内存泄漏
原因: munmap时未正确释放物理页
解决: 检查uvmunmap调用
```

### 8.2 调试工具

```c
// 打印VMA信息
void debug_vma(struct proc *p) {
    printf("VMAs for pid %d:\n", p->pid);
    for (int i = 0; i < NVMA; i++) {
        if (p->vmas[i].valid) {
            printf("  [%d] addr=%p len=%d prot=%x flags=%x\n",
                   i, p->vmas[i].addr, p->vmas[i].length,
                   p->vmas[i].prot, p->vmas[i].flags);
            if (p->vmas[i].file)
                printf("       file=%s offset=%d\n",
                       p->vmas[i].file->ip-> ...);
        }
    }
}

// 在页面错误时打印信息
int mmap_handler(uint64 va) {
    printf("mmap_handler: va=%p\n", va);

    struct vma *vma = find_vma(myproc(), va);
    if (vma) {
        printf("  found vma: addr=%p len=%d\n", vma->addr, vma->length);
    } else {
        printf("  no vma found!\n");
    }

    // ... 处理逻辑 ...
}
```

---

## 9. 总结

### 9.1 核心概念

| 概念 | 说明 |
|------|------|
| **VMA** | 虚拟内存区域，描述一段映射 |
| **惰性分配** | 首次访问时才分配物理页 |
| **私有映射** | 写时复制，不影响原文件 |
| **共享映射** | 直接修改文件，进程间共享 |
| **匿名映射** | 不关联文件，分配零页 |

### 9.2 实现要点

1. **VMA管理**: 使用数组存储进程的所有映射
2. **地址分配**: 从进程地址空间顶部开始分配
3. **页面错误**: 统一处理COW和mmap页面错误
4. **权限检查**: 根据prot和flags设置正确的PTE权限
5. **资源清理**: fork复制VMA，exit时清理并写回

---

**难度**: ⭐⭐⭐⭐
**预计时间**: 12-15小时
**依赖**: Lab 5 (COW), Lab 9 (文件系统)


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
