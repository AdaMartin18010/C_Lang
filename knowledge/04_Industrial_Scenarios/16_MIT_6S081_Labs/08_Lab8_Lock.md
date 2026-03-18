# MIT 6.S081 Lab 8: Lock

## 1. 实验概述

### 1.1 目标

- 理解锁竞争对性能的影响
- 实现细粒度锁优化
- 掌握并发数据结构设计

### 1.2 锁竞争问题

```text
内存分配器中的锁竞争:

单个大锁:
┌─────────────────┐
│   kalloc.lock   │  ← 所有CPU竞争同一把锁
└─────────────────┘
         │
    ┌────┼────┐
    ▼    ▼    ▼
  CPU0  CPU1  CPU2

问题:
- 高CPU利用率但低吞吐量
- 锁等待导致CPU空转
- 缓存一致性流量增加

解决方案:
- 每个CPU独立空闲列表
- 减少锁竞争
```

---

## 2. 内存分配器优化

### 2.1 原始实现分析

```c
// kernel/kalloc.c (原始)
struct {
    struct spinlock lock;
    struct run *freelist;  // 全局空闲列表
} kmem;

void* kalloc(void) {
    acquire(&kmem.lock);    // 所有CPU竞争这把锁

    struct run *r = kmem.freelist;
    if (r)
        kmem.freelist = r->next;

    release(&kmem.lock);
    return (void*)r;
}

void kfree(void *pa) {
    acquire(&kmem.lock);    // 再次竞争

    struct run *r = (struct run*)pa;
    r->next = kmem.freelist;
    kmem.freelist = r;

    release(&kmem.lock);
}
```

### 2.2 每CPU空闲列表实现

```c
// kernel/kalloc.c (优化后)

#define NCPU 8

struct {
    struct spinlock lock;
    struct run *freelist;
    int nfree;              // 统计空闲页数
} kmems[NCPU];              // 每个CPU一个空闲列表

// 获取当前CPU的kmem索引
#define CPUID() (cpuid() % NCPU)

void* kalloc(void) {
    int id = CPUID();

    acquire(&kmems[id].lock);

    struct run *r = kmems[id].freelist;
    if (r) {
        kmems[id].freelist = r->next;
        kmems[id].nfree--;
    }

    release(&kmems[id].lock);

    // 如果本地没有，尝试从其他CPU偷取
    if (!r) {
        for (int i = 0; i < NCPU; i++) {
            if (i == id) continue;

            acquire(&kmems[i].lock);
            r = kmems[i].freelist;
            if (r) {
                kmems[i].freelist = r->next;
                kmems[i].nfree--;
            }
            release(&kmems[i].lock);

            if (r) break;
        }
    }

    return (void*)r;
}

void kfree(void *pa) {
    int id = CPUID();

    acquire(&kmems[id].lock);

    struct run *r = (struct run*)pa;
    r->next = kmems[id].freelist;
    kmems[id].freelist = r;
    kmems[id].nfree++;

    release(&kmems[id].lock);
}
```

---

## 3. 缓冲区缓存优化

### 3.1 原始设计

```c
// kernel/bio.c
// 原始：全局缓冲区列表，单把锁

struct {
    struct spinlock lock;
    struct buf buf[NBUF];
    struct buf head;  // LRU列表头
} bcache;

// 问题：所有缓冲区操作竞争同一把锁
```

### 3.2 哈希桶优化

```c
// kernel/bio.c (优化后)

#define NBUCKETS 13  // 哈希桶数量

struct bucket {
    struct spinlock lock;
    struct buf head;  // 该桶的缓冲区列表
};

struct {
    struct bucket buckets[NBUCKETS];
    struct buf buf[NBUF];  // 所有缓冲区
} bcache;

// 哈希函数
#define HASH(blockno) ((blockno) % NBUCKETS)

static struct buf* bget(uint dev, uint blockno) {
    int bidx = HASH(blockno);

    acquire(&bcache.buckets[bidx].lock);

    // 在该桶中查找
    struct buf *b;
    for (b = bcache.buckets[bidx].head.next;
         b != &bcache.buckets[bidx].head;
         b = b->next) {
        if (b->dev == dev && b->blockno == blockno) {
            b->refcnt++;
            release(&bcache.buckets[bidx].lock);
            acquiresleep(&b->lock);
            return b;
        }
    }

    // 未找到，分配新的...

    release(&bcache.buckets[bidx].lock);
    return b;
}
```

---

## 4. 性能测试

### 4.1 kalloctest

```c
// user/kalloctest.c
#include "kernel/types.h"
#include "user/user.h"

void test_alloc_free(int n) {
    void *ptrs[100];

    for (int i = 0; i < n; i++) {
        // 分配
        for (int j = 0; j < 100; j++) {
            ptrs[j] = malloc(4096);
        }

        // 释放
        for (int j = 0; j < 100; j++) {
            free(ptrs[j]);
        }
    }
}

int main() {
    int pid;

    // 创建多个进程同时分配内存
    for (int i = 0; i < 4; i++) {
        pid = fork();
        if (pid == 0) {
            test_alloc_free(1000);
            exit(0);
        }
    }

    for (int i = 0; i < 4; i++) {
        wait(0);
    }

    printf("kalloctest passed\n");
    exit(0);
}
```

### 4.2 bcachetest

```c
// user/bcachetest.c
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main() {
    int fd;
    char buf[512];
    int pid;

    fd = open("testfile", O_CREATE | O_RDWR);
    if (fd < 0) {
        printf("cannot create testfile\n");
        exit(1);
    }

    // 创建多个进程同时读写
    for (int i = 0; i < 4; i++) {
        pid = fork();
        if (pid == 0) {
            // 子进程：大量读写不同块
            for (int j = 0; j < 10000; j++) {
                int block = (i * 10000 + j) % 1000;
                lseek(fd, block * 512, 0);
                write(fd, buf, sizeof(buf));
            }
            exit(0);
        }
    }

    for (int i = 0; i < 4; i++) {
        wait(0);
    }

    close(fd);
    printf("bcachetest passed\n");
    exit(0);
}
```

---

## 5. 关键概念

### 5.1 锁粒度

```text
锁粒度选择:

粗粒度 (大锁):
优点: 实现简单，锁开销小
缺点: 并发度低，竞争激烈
适用: 低竞争场景

细粒度 (多个小锁):
优点: 并发度高，竞争分散
缺点: 实现复杂，可能有死锁风险
适用: 高并发场景

无锁 (Lock-free):
优点: 最高并发度
缺点: 实现极复杂
适用: 极致性能要求
```

### 5.2 锁竞争度量

```bash
# 使用xv6的统计功能查看锁竞争
$ statsc

输出示例:
lock: kmem          #acquire 1000000  #contention 500000 (50%)
lock: bcache        #acquire 2000000  #contention 800000 (40%)

优化目标: 降低contention比例
```

---

**难度**: ⭐⭐⭐⭐
**预计时间**: 6-8小时
