# 虚拟内存子系统深度解析

> **层级定位**: 03 System Technology Domains / 16 OS Kernel
> **参考课程**: MIT 6.828, Berkeley CS162, CMU 15-410
> **难度级别**: L5 专家
> **预估学习时间**: 15-20 小时

---

## 目录

- [虚拟内存子系统深度解析](#虚拟内存子系统深度解析)
  - [目录](#目录)
  - [1. 虚拟内存基础](#1-虚拟内存基础)
    - [1.1 为什么需要虚拟内存](#11-为什么需要虚拟内存)
    - [1.2 地址转换基础](#12-地址转换基础)
  - [2. Linux内存管理架构](#2-linux内存管理架构)
    - [2.1 内存区域划分](#21-内存区域划分)
    - [2.2 核心数据结构](#22-核心数据结构)
  - [3. 页面分配与回收](#3-页面分配与回收)
    - [3.1 伙伴系统](#31-伙伴系统)
    - [3.2 Slab分配器](#32-slab分配器)
    - [3.3 页面回收算法](#33-页面回收算法)
  - [4. 缺页异常处理](#4-缺页异常处理)
    - [4.1 缺页类型](#41-缺页类型)
    - [4.2 缺页处理流程](#42-缺页处理流程)
    - [4.3 写时复制(Copy-On-Write)](#43-写时复制copy-on-write)
  - [5. 内存映射(mmap)](#5-内存映射mmap)
    - [5.1 mmap系统调用](#51-mmap系统调用)
    - [5.2 mmap内核实现](#52-mmap内核实现)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. 虚拟内存基础

### 1.1 为什么需要虚拟内存

```
┌─────────────────────────────────────────────────────────────┐
│                     虚拟内存解决的问题                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 隔离性 (Isolation)                                      │
│     ┌─────┐  ┌─────┐  ┌─────┐                              │
│     │进程A│  │进程B│  │进程C│  各自认为自己拥有整个地址空间    │
│     └──┬──┘  └──┬──┘  └──┬──┘                              │
│        │        │        │                                  │
│        └────────┼────────┘                                  │
│                 ▼                                           │
│     ┌─────────────────────┐                                │
│     │    MMU + 页表        │  硬件隔离，一个进程无法访问其他   │
│     └─────────────────────┘  进程的内存                      │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│  2. 透明性 (Transparency)                                   │
│     • 物理内存碎片化 → 虚拟地址连续                         │
│     • 物理内存不足 → 透明交换到磁盘                         │
│     • 多进程共享 → 写时复制(CoW)                           │
├─────────────────────────────────────────────────────────────┤
│  3. 效率 (Efficiency)                                       │
│     • 缓存：TLB加速地址转换                                 │
│     • 按需加载：仅加载需要的页面                            │
│     • 共享内存：多进程共享只读代码                          │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 地址转换基础

```
虚拟地址 → [页表遍历] → 物理地址

页表结构（x86-64，4级页表）：
┌──────────┬──────────┬──────────┬──────────┬────────────┐
│  PML4    │  PDPT    │   PD     │   PT     │  页内偏移  │
│ 9 bits   │ 9 bits   │ 9 bits   │ 9 bits   │  12 bits   │
│ (512项)  │ (512项)  │ (512项)  │ (512项)  │  (4KB页)   │
└──────────┴──────────┴──────────┴──────────┴────────────┘
    │           │         │         │            │
    ▼           ▼         ▼         ▼            ▼
CR3寄存器   物理地址    物理地址   物理地址    物理页+偏移
(基址)      指向PDPT   指向PD    指向PT      最终物理地址

页表项(PTE)格式：
┌────────────────────────────────────────────────────────┐
│ 物理页框号(40 bits) │ 标志位(12 bits):                    │
│                    │ D A G U W P                         │
│                    │ ↓ ↓ ↓ ↓ ↓ ↓                         │
│                    │ 脏 访问 全局 用户 可写 存在           │
└────────────────────────────────────────────────────────┘
```

---

## 2. Linux内存管理架构

### 2.1 内存区域划分

```
┌─────────────────────────────────────────────┐ 0xFFFFFFFFFFFFFFFF
│                内核空间                      │
│  ┌───────────────────────────────────────┐  │
│  │  vmalloc区域（vmalloc）                │  │
│  │  非连续物理页，用于大内存分配            │  │
│  ├───────────────────────────────────────┤  │
│  │  直接映射区（linear mapping）           │  │
│  │  物理地址 = 虚拟地址 - PAGE_OFFSET      │  │
│  │  用于kmalloc，物理连续                   │  │
│  ├───────────────────────────────────────┤  │
│  │  固定映射（fixmap）                     │  │
│  │  编译时确定的特殊用途页                  │  │
│  └───────────────────────────────────────┘  │
├─────────────────────────────────────────────┤ PAGE_OFFSET (0xFFFF888000000000)
│                                             │
│                                             │
│                （空洞）                      │
│                                             │
│                                             │
├─────────────────────────────────────────────┤ 0x00007FFFFFFFFFFF
│                用户空间                      │
│  ┌───────────────────────────────────────┐  │
│  │  栈（Stack）向下增长                   │  │
│  ├───────────────────────────────────────┤  │
│  │  内存映射区（mmap）                    │  │
│  │  共享库、文件映射、匿名映射              │  │
│  ├───────────────────────────────────────┤  │
│  │  堆（Heap）向上增长                    │  │
│  │  brk/mmalloc分配                       │  │
│  ├───────────────────────────────────────┤  │
│  │  BSS段（未初始化全局变量）              │  │
│  ├───────────────────────────────────────┤  │
│  │  数据段（已初始化全局变量）             │  │
│  ├───────────────────────────────────────┤  │
│  │  代码段（Text）                        │  │
│  └───────────────────────────────────────┘  │
└─────────────────────────────────────────────┘ 0x0000000000400000 (典型)
```

### 2.2 核心数据结构

```c
// 内存描述符 - 每个进程一个
struct mm_struct {
    pgd_t *pgd;                    // 页全局目录（CR3）

    atomic_t mm_users;             // 使用计数（线程共享）
    atomic_t mm_count;             // 引用计数

    unsigned long start_code;      // 代码段起始
    unsigned long end_code;        // 代码段结束
    unsigned long start_data;      // 数据段起始
    unsigned long end_data;        // 数据段结束
    unsigned long start_brk;       // 堆起始
    unsigned long brk;             // 堆当前位置
    unsigned long start_stack;     // 栈起始

    struct vm_area_struct *mmap;   // VMA链表
    struct rb_root mm_rb;          // VMA红黑树（加速查找）

    unsigned long total_vm;        // 总映射页数
    unsigned long locked_vm;       // 锁定的页数
    unsigned long rss;             // 实际驻留内存
};

// 虚拟内存区域 (VMA)
struct vm_area_struct {
    unsigned long vm_start;        // 起始地址
    unsigned long vm_end;          // 结束地址

    struct mm_struct *vm_mm;       // 所属mm_struct
    pgprot_t vm_page_prot;         // 页面保护标志
    unsigned long vm_flags;        // 标志（RWX等）

    struct rb_node vm_rb;          // 红黑树节点
    struct list_head vm_list;      // 链表节点

    // 操作函数表
    const struct vm_operations_struct *vm_ops;

    // 文件映射相关
    struct file *vm_file;          // 映射的文件
    unsigned long vm_pgoff;        // 文件内偏移

    void *vm_private_data;         // 私有数据
};

// VMA操作
struct vm_operations_struct {
    void (*open)(struct vm_area_struct *area);
    void (*close)(struct vm_area_struct *area);
    int (*fault)(struct vm_area_struct *vma, struct vm_fault *vmf);
    int (*page_mkwrite)(struct vm_area_struct *vma, struct vm_fault *vmf);
    // ...
};
```

---

## 3. 页面分配与回收

### 3.1 伙伴系统

```
伙伴系统解决外部碎片：

初始：16页空闲块
┌────────────────────────────────────────────────┐
│                   16页块                        │
└────────────────────────────────────────────────┘

分配4页：
┌────────────────┬────────────────┬────────┬──────┐
│     4页        │      4页       │   4页  │  4页 │
│   (已分配)     │    (空闲)      │ (空闲) │ (空闲)│
└────────────────┴────────────────┴────────┴──────┘

分配2页：
┌────────┬────────┬────────────────┬────────┬──────┐
│  2页   │  2页   │      4页       │   4页  │  4页 │
│(已分配)│ (空闲) │    (空闲)      │ (空闲) │(空闲)│
└────────┴────────┴────────────────┴────────┴──────┘

释放4页（与前一个4页合并）：
┌────────┬────────┬──────────────────────────┬──────┐
│  2页   │  2页   │            8页            │  4页 │
│(已分配)│ (空闲) │          (空闲)           │(空闲)│
└────────┴────────┴──────────────────────────┴──────┘
```

```c
// 页面分配API
struct page *alloc_pages(gfp_t gfp_mask, unsigned int order);
#define alloc_page(gfp_mask) alloc_pages(gfp_mask, 0)

void __free_pages(struct page *page, unsigned int order);
#define __free_page(page) __free_pages((page), 0)

// GFP标志详解
GFP_KERNEL      // 标准分配，可睡眠，适用于大多数情况
GFP_ATOMIC      // 原子分配，不睡眠，用于中断上下文
GFP_USER        // 为用户空间分配
GFP_HIGHUSER    // 优先使用高端内存（32位系统）
GFP_DMA         // 适合DMA的内存（<16MB）
GFP_DMA32       // 适合32位DMA的内存（<4GB）
```

### 3.2 Slab分配器

```c
// Slab用于频繁分配的小对象
struct kmem_cache;

// 创建缓存
struct kmem_cache *kmem_cache_create(
    const char *name,
    size_t size,           // 对象大小
    size_t align,          // 对齐要求
    unsigned long flags,   // 标志
    void (*ctor)(void *)   // 构造函数（已废弃）
);

// 分配/释放
void *kmem_cache_alloc(struct kmem_cache *, gfp_t flags);
void kmem_cache_free(struct kmem_cache *, void *);

// Slab着色（Slab Coloring）
// 不同slab从缓存行不同偏移开始，减少缓存冲突
// ┌──────────┬──────────┬──────────┬──────────┐
// │ Slab 0   │ Slab 1   │ Slab 2   │ Slab 3   │
// │ offset 0 │ offset 8 │ offset 16│ offset 24│
// └──────────┴──────────┴──────────┴──────────┘
```

### 3.3 页面回收算法

```
页面回收(Page Reclaim)策略：

1. 页框回收算法(PFRA)选择牺牲页：
   ┌─────────────────────────────────────────────┐
   │  优先级：                                    │
   │  1. 未引用的页（未映射到任何进程）             │
   │  2. 非活动匿名页（交换到swap）                 │
   │  3. 非活动文件页（可能已同步到磁盘）           │
   │  4. 活动页（需要降级为非活动）                 │
   └─────────────────────────────────────────────┘

2. 双时钟算法（Active/Inactive Lists）：

   活动链表        非活动链表        空闲链表
   ┌──────┐       ┌──────┐        ┌──────┐
   │ page │       │ page │   →→→ │      │
   │ page │  ←→→  │ page │  ←→→  │      │
   │ page │       │ page │        │      │
   └──────┘       └──────┘        └──────┘

   • 引用时从非活动移到活动
   • 扫描时未引用从活动降级到非活动
   • 非活动页可回收

3. 交换(Swapping)：
   • 匿名页（堆、栈）→ 交换分区/文件
   • 文件页（映射文件）→ 直接丢弃（可重新读取）
```

---

## 4. 缺页异常处理

### 4.1 缺页类型

```
┌─────────────────────────────────────────────────────────────┐
│                     缺页异常分类                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 主缺页(Major Fault)                                     │
│     • 需要磁盘I/O                                           │
│     • 文件映射首次访问                                       │
│     • 交换页从磁盘读回                                       │
│     • 耗时：毫秒级                                           │
│                                                             │
│  2. 次缺页(Minor Fault)                                     │
│     • 无需磁盘I/O                                           │
│     • 零页分配（COW）                                        │
│     • 已存在页面的映射                                       │
│     • 耗时：微秒级                                           │
│                                                             │
│  3. 错误缺页(Segmentation Fault)                            │
│     • 访问无效地址                                           │
│     • 权限错误（写只读页）                                    │
│     • 栈溢出                                                 │
│     • 结果：SIGSEGV                                          │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 缺页处理流程

```c
// do_page_fault - x86缺页处理入口
do_page_fault(struct pt_regs *regs, unsigned long error_code)
{
    // error_code:
    // bit 0: 0=页不存在, 1=保护错误
    // bit 1: 0=读, 1=写
    // bit 2: 0=内核, 1=用户

    // 1. 定位vma
    vma = find_vma(mm, address);
    if (!vma || address < vma->vm_start)
        goto bad_area;  // 无效访问

    // 2. 检查权限
    if (!(vma->vm_flags & VM_WRITE) && (error_code & PF_WRITE))
        goto bad_area;  // 写只读区域

    // 3. 处理缺页
    ret = handle_mm_fault(vma, address, flags);

    // 4. 更新统计
    if (ret & VM_FAULT_MAJOR)
        current->maj_flt++;
    else
        current->min_flt++;
}

// handle_mm_fault核心逻辑
int handle_mm_fault(struct vm_area_struct *vma,
                    unsigned long address,
                    unsigned int flags)
{
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    // 遍历页表，按需创建中间层
    pgd = pgd_offset(mm, address);
    p4d = p4d_alloc(mm, pgd, address);
    pud = pud_alloc(mm, p4d, address);
    pmd = pmd_alloc(mm, pud, address);

    // 处理pte
    return handle_pte_fault(mm, vma, address, pmd, pte, flags);
}

// 具体缺页处理
static int handle_pte_fault(struct mm_struct *mm, ...)
{
    pte_t entry;

    if (!pte_present(entry)) {
        if (pte_none(entry)) {
            // 首次访问 - 匿名页或文件映射
            if (vma_is_anonymous(vma))
                return do_anonymous_page(mm, vma, address, pte, pmd, flags);
            else
                return do_fault(mm, vma, address, pte, pmd, flags, entry);
        }
        // 交换页
        return do_swap_page(mm, vma, address, pte, pmd, flags, entry);
    }

    // 写时复制(COW)
    if (flags & FAULT_FLAG_WRITE) {
        if (!pte_write(entry))
            return do_wp_page(mm, vma, address, pte, pmd, ptl, entry);
    }

    return 0;
}
```

### 4.3 写时复制(Copy-On-Write)

```
COW机制：

fork()时：
┌─────────────┐              ┌─────────────┐
│   父进程     │              │   子进程     │
│             │              │             │
│ ┌─────────┐ │              │ ┌─────────┐ │
│ │ 物理页0  │ │◄────────────►│ │ 物理页0  │ │
│ │ (只读)   │ │   共享页     │ │ (只读)   │ │
│ └─────────┘ │              │ └─────────┘ │
│ ┌─────────┐ │              │ ┌─────────┐ │
│ │ 物理页1  │ │◄────────────►│ │ 物理页1  │ │
│ │ (只读)   │ │              │ │ (只读)   │ │
│ └─────────┘ │              │ └─────────┘ │
└─────────────┘              └─────────────┘

子进程写页0时：
┌─────────────┐              ┌─────────────┐
│   父进程     │              │   子进程     │
│             │              │             │
│ ┌─────────┐ │              │ ┌─────────┐ │
│ │ 物理页0  │ │              │ │ 物理页2  │ │
│ │ (只读)   │ │              │ │ (可写)   │ │
│ └─────────┘ │              │ └─────────┘ │
│ ┌─────────┐ │              │ ┌─────────┐ │
│ │ 物理页1  │ │◄────────────►│ │ 物理页1  │ │
│ │ (只读)   │ │              │ │ (只读)   │ │
│ └─────────┘ │              │ └─────────┘ │
└─────────────┘              └─────────────┘

优势：
• fork()快速（无需复制内存）
• 内存节省（仅复制修改的页）
• exec()时无需复制（直接替换）
```

---

## 5. 内存映射(mmap)

### 5.1 mmap系统调用

```c
#include <sys/mman.h>

// 创建内存映射
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

// 参数详解
addr    // 建议映射地址（NULL让内核选择）
length  // 映射长度（向上取整到页大小）
prot    // 保护标志：PROT_READ | PROT_WRITE | PROT_EXEC | PROT_NONE
flags   // 映射类型：
        //   MAP_SHARED  - 共享映射，修改写回文件
        //   MAP_PRIVATE - 私有映射，COW
        //   MAP_ANONYMOUS - 匿名映射（无文件）
        //   MAP_FIXED   - 强制使用指定地址
        //   MAP_LOCKED  - 锁定到内存（不交换）
fd      // 文件描述符（匿名映射时为-1）
offset  // 文件偏移（必须是页大小倍数）

// 解除映射
int munmap(void *addr, size_t length);

// 同步文件映射到磁盘
int msync(void *addr, size_t length, int flags);

// 修改保护
int mprotect(void *addr, size_t len, int prot);

// 建议预读/释放
int madvise(void *addr, size_t length, int advice);
//   MADV_SEQUENTIAL - 顺序访问
//   MADV_RANDOM     - 随机访问
//   MADV_WILLNEED   - 即将访问（预读）
//   MADV_DONTNEED   - 不再需要（释放）
//   MADV_HUGEPAGE   - 建议使用大页
```

### 5.2 mmap内核实现

```c
// 匿名映射（堆分配）
void *malloc(size_t size) {
    // 大块使用mmap
    if (size > MMAP_THRESHOLD) {
        void *p = mmap(NULL, size, PROT_READ|PROT_WRITE,
                       MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        return p;
    }
    // 小块使用brk
    return brk_alloc(size);
}

// 文件映射（共享库加载）
// 1. 打开文件
int fd = open("/lib/libc.so.6", O_RDONLY);

// 2. 获取文件信息
struct stat st;
fstat(fd, &st);

// 3. 映射到内存
void *addr = mmap(NULL, st.st_size, PROT_READ|PROT_EXEC,
                  MAP_PRIVATE, fd, 0);

// 4. 关闭文件描述符（映射仍然有效）
close(fd);

// 5. 通过addr访问文件内容
Elf64_Ehdr *elf = addr;
```

---

## 关联导航

### 前置知识

- [内存管理硬件](../../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/05_Memory_Management_Hardware/README.md)
- [Linux内核模块编程](./01_Linux_Kernel_Module_Programming.md)

### 后续延伸

- [内存分配器设计](../01_System_Programming/07_Memory_Allocator_Design.md)
- [NUMA架构与优化](./04_NUMA_Architecture.md)

### 参考

- Understanding the Linux Virtual Memory Manager, Mel Gorman
- Operating Systems: Three Easy Pieces
- Linux Kernel Development, Robert Love
