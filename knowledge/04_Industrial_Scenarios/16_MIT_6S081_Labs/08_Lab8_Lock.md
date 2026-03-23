# MIT 6.S081 Lab 8: Lock

## 1. 实验概述

### 1.1 目标

- 理解锁在多核系统中的重要性
- 实现自旋锁和睡眠锁
- 掌握锁粒度优化的方法
- 检测和避免死锁
- 分析锁竞争对性能的影响

### 1.2 为什么需要锁

```
多核系统并发访问问题:

Core 0                    Core 1
  │                         │
  ▼                         ▼
读取counter(10)           读取counter(10)
加1 (11)                  加1 (11)
写回counter               写回counter
  │                         │
  └────────► counter = 11 ◄─┘

期望: counter = 12
实际: counter = 11 (丢失了一个增量)

根本原因:
- 读取-修改-写入不是原子操作
- 两个CPU并发执行时可能交错
```

### 1.3 锁的类型

```
锁的分类:

1. 自旋锁 (Spinlock)
   - 获取锁失败时忙等待
   - 适用于短临界区
   - 不能睡眠

2. 睡眠锁 (Sleeplock)
   - 获取锁失败时阻塞，让出CPU
   - 适用于长临界区
   - 可以在持有锁时睡眠

3. 读写锁 (RWLock)
   - 读共享，写独占
   - 适用于读多写少场景

4. 顺序锁 (Seqlock)
   - 写者优先
   - 读取时检测是否被修改
```

---

## 2. 自旋锁实现

### 2.1 RISC-V原子指令

```asm
# RISC-V原子操作指令

# 原子交换 (用于test-and-set)
amoswap.w rd, rs2, (rs1)
  # temp = *rs1
  # *rs1 = rs2
  # rd = temp

# 原子加 (用于计数器)
amoadd.w rd, rs2, (rs1)
  # temp = *rs1
  # *rs1 = temp + rs2
  # rd = temp

# 比较并交换 (CAS)
amocas.w rd, rs2, (rs1)
  # if (*rs1 == rd) *rs1 = rs2
  # rd = old_value

# 加载保留 / 存储条件 (LL/SC)
lr.w rd, (rs1)      # 加载并设置保留标记
sc.w rd, rs2, (rs1) # 如果保留有效则存储
                    # rd = 0成功，1失败
```

### 2.2 xv6自旋锁实现

```c
// kernel/spinlock.h

struct spinlock {
    uint locked;       // 锁状态: 0=空闲, 1=被持有

    // 调试信息
    char *name;        // 锁名称
    struct cpu *cpu;   // 持有锁的CPU
};
```

```c
// kernel/spinlock.c

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"

void
initlock(struct spinlock *lk, char *name)
{
    lk->name = name;
    lk->locked = 0;
    lk->cpu = 0;
}

// 获取自旋锁
void
acquire(struct spinlock *lk)
{
    // 关中断，防止中断处理程序死锁
    push_off();

    // 检查：不能在持有锁时再次获取
    if (holding(lk))
        panic("acquire");

    // 自旋直到获取锁
    // 使用amoswap实现test-and-set
    while (__sync_lock_test_and_set(&lk->locked, 1) != 0)
        ;

    // 内存屏障：确保锁获取在临界区之前完成
    __sync_synchronize();

    // 记录持有信息
    lk->cpu = mycpu();
}

// 释放自旋锁
void
release(struct spinlock *lk)
{
    // 检查：必须持有锁才能释放
    if (!holding(lk))
        panic("release");

    lk->cpu = 0;

    // 内存屏障：确保临界区在锁释放之前完成
    __sync_synchronize();

    // 释放锁
    __sync_lock_release(&lk->locked);

    // 开中断
    pop_off();
}

// 检查是否持有锁
int
holding(struct spinlock *lk)
{
    int r;
    r = (lk->locked && lk->cpu == mycpu());
    return r;
}
```

### 2.3 中断处理

```c
// kernel/spinlock.c

// 保存中断状态并关中断
void
push_off(void)
{
    int old = intr_get();

    // 保存旧状态
    intr_off();

    // 如果是第一次关中断，保存旧状态
    if (mycpu()->noff == 0)
        mycpu()->intena = old;

    mycpu()->noff += 1;
}

// 恢复中断状态
void
pop_off(void)
{
    struct cpu *c = mycpu();

    // 检查：是否在关中断状态
    if (intr_get())
        panic("pop_off - interruptible");

    // 检查：嵌套计数是否正确
    if (c->noff < 1)
        panic("pop_off");

    c->noff -= 1;

    // 只有当所有嵌套都退出后才开中断
    if (c->noff == 0 && c->intena)
        intr_on();
}
```

---

## 3. 睡眠锁实现

### 3.1 睡眠锁数据结构

```c
// kernel/sleeplock.h

struct sleeplock {
    uint locked;       // 锁状态
    struct spinlock lk; // 保护此结构的自旋锁

    // 调试信息
    char *name;
    int pid;           // 持有锁的进程
};
```

### 3.2 睡眠锁实现

```c
// kernel/sleeplock.c

void
initsleeplock(struct sleeplock *lk, char *name)
{
    initlock(&lk->lk, "sleep lock");
    lk->name = name;
    lk->locked = 0;
    lk->pid = 0;
}

void
acquiresleep(struct sleeplock *lk)
{
    // 先获取保护锁
    acquire(&lk->lk);

    // 等待锁释放
    while (lk->locked) {
        // 睡眠，等待锁释放
        sleep(lk, &lk->lk);
    }

    // 获取锁
    lk->locked = 1;
    lk->pid = myproc()->pid;

    release(&lk->lk);
}

void
releasesleep(struct sleeplock *lk)
{
    acquire(&lk->lk);

    lk->locked = 0;
    lk->pid = 0;

    // 唤醒等待的进程
    wakeup(lk);

    release(&lk->lk);
}

int
holdingsleep(struct sleeplock *lk)
{
    int r;

    acquire(&lk->lk);
    r = lk->locked && (lk->pid == myproc()->pid);
    release(&lk->lk);

    return r;
}
```

---

## 4. 死锁检测与预防

### 4.1 死锁条件

```
死锁四个必要条件:

1. 互斥 (Mutual Exclusion)
   - 资源一次只能被一个进程使用

2. 占有并等待 (Hold and Wait)
   - 进程持有资源同时等待其他资源

3. 不可剥夺 (No Preemption)
   - 资源不能被强制剥夺

4. 循环等待 (Circular Wait)
   - 进程间形成循环等待链

破坏任一条件即可避免死锁
```

### 4.2 锁顺序规则

```c
// kernel/lockorder.h

// 定义全局锁顺序
enum lock_order {
    LOCK_MIN = 0,

    LOCK_BCACHE,      // 缓冲区缓存锁
    LOCK_VNODE,       // 虚拟节点锁
    LOCK_INODE,       // inode锁
    LOCK_DIR,         // 目录锁
    LOCK_FILE,        // 文件锁
    LOCK_PROC,        // 进程锁

    LOCK_MAX
};

// 每个锁关联一个顺序值
struct ordered_lock {
    struct spinlock lk;
    enum lock_order order;
};

// 按顺序获取多个锁
void
acquire_ordered(struct ordered_lock *lk1, struct ordered_lock *lk2)
{
    // 总是先获取顺序值小的锁
    if (lk1->order < lk2->order) {
        acquire(&lk1->lk);
        acquire(&lk2->lk);
    } else if (lk1->order > lk2->order) {
        acquire(&lk2->lk);
        acquire(&lk1->lk);
    } else {
        panic("acquire_ordered: same order");
    }
}
```

### 4.3 死锁检测工具

```c
// kernel/lockdebug.c

#ifdef DEBUG_LOCK

#define MAX_LOCK_DEPTH 10

struct lock_stack {
    struct spinlock *locks[MAX_LOCK_DEPTH];
    int depth;
};

struct lock_stack lock_stacks[NCPU];

void
lockdebug_acquire(struct spinlock *lk)
{
    struct lock_stack *ls = &lock_stacks[cpuid()];

    // 检查：锁是否已经被持有
    for (int i = 0; i < ls->depth; i++) {
        if (ls->locks[i] == lk) {
            panic("lockdebug: recursive acquire of %s", lk->name);
        }
    }

    // 检查：锁顺序
    for (int i = 0; i < ls->depth; i++) {
        if (ls->locks[i] > lk) {
            printf("WARNING: lock order violation\n");
            printf("  acquired %s after %s\n",
                   lk->name, ls->locks[i]->name);
        }
    }

    // 记录锁获取
    if (ls->depth >= MAX_LOCK_DEPTH)
        panic("lockdebug: lock stack overflow");

    ls->locks[ls->depth++] = lk;
}

void
lockdebug_release(struct spinlock *lk)
{
    struct lock_stack *ls = &lock_stacks[cpuid()];

    // 检查：释放的必须是最后获取的锁
    if (ls->depth == 0 || ls->locks[ls->depth - 1] != lk) {
        panic("lockdebug: release order violation");
    }

    ls->depth--;
}

void
lockdebug_dump(void)
{
    struct lock_stack *ls = &lock_stacks[cpuid()];

    printf("Lock stack (depth=%d):\n", ls->depth);
    for (int i = 0; i < ls->depth; i++) {
        printf("  %d: %s\n", i, ls->locks[i]->name);
    }
}

#endif // DEBUG_LOCK
```

---

## 5. 锁粒度优化

### 5.1 粗粒度锁 vs 细粒度锁

```
粗粒度锁 (Big Kernel Lock):
┌─────────────────────────────────┐
│         全局大锁                │
├─────────────────────────────────┤
│  fs │  vm │ net │ proc │  ...  │
└─────────────────────────────────┘
优势: 实现简单，不会死锁
劣势: 并发度低，多核扩展性差

细粒度锁:
┌─────────┬─────────┬─────────┬───┐
│  fs锁   │  vm锁   │ net锁   │...│
├─────────┴─────────┴─────────┴───┤
│  各子系统内部进一步细分          │
│  如: 每个inode一个锁             │
│     每个缓冲区一个锁             │
│     每个socket一个锁             │
└─────────────────────────────────┘
优势: 并发度高，多核扩展性好
劣势: 实现复杂，容易死锁
```

### 5.2 Buffer Cache锁优化

```c
// kernel/bio.c

// 原始设计: 全局bcache锁
struct {
    struct spinlock lock;
    struct buf buf[NBUF];
    struct buf head;  // LRU链表头
} bcache;

// 优化设计: 哈希桶 + 每桶锁
#define NBUCKETS 13

struct bucket {
    struct spinlock lock;
    struct buf *head;
};

struct {
    struct bucket buckets[NBUCKETS];
    struct buf buf[NBUF];
} bcache;

// 哈希函数
#define HASH(dev, blockno) (((dev) + (blockno)) % NBUCKETS)

struct buf *
bget(uint dev, uint blockno)
{
    int bucket = HASH(dev, blockno);

    // 只锁定对应的桶
    acquire(&bcache.buckets[bucket].lock);

    // 在桶中查找
    for (struct buf *b = bcache.buckets[bucket].head;
         b != 0; b = b->next) {
        if (b->dev == dev && b->blockno == blockno) {
            b->refcnt++;
            release(&bcache.buckets[bucket].lock);
            return b;
        }
    }

    // 未找到，分配新缓冲区...

    release(&bcache.buckets[bucket].lock);
    return b;
}
```

### 5.3 读写锁实现

```c
// kernel/rwlock.h

struct rwlock {
    struct spinlock lk;     // 保护此结构
    int readers;            // 读者数量
    int writing;            // 是否有写者
    struct proc *writer;    // 写者进程
    struct spinlock wait_lk; // 等待队列锁
};

void initrwlock(struct rwlock *rw, char *name);
void readlock(struct rwlock *rw);
void readunlock(struct rwlock *rw);
void writelock(struct rwlock *rw);
void writeunlock(struct rwlock *rw);
```

```c
// kernel/rwlock.c

void
readlock(struct rwlock *rw)
{
    acquire(&rw->lk);

    // 等待写者完成
    while (rw->writing) {
        sleep(rw, &rw->lk);
    }

    rw->readers++;

    release(&rw->lk);
}

void
readunlock(struct rwlock *rw)
{
    acquire(&rw->lk);

    rw->readers--;

    // 如果最后一个读者离开，唤醒写者
    if (rw->readers == 0) {
        wakeup(rw);
    }

    release(&rw->lk);
}

void
writelock(struct rwlock *rw)
{
    acquire(&rw->lk);

    // 等待所有读者和写者完成
    while (rw->writing || rw->readers > 0) {
        sleep(rw, &rw->lk);
    }

    rw->writing = 1;
    rw->writer = myproc();

    release(&rw->lk);
}

void
writeunlock(struct rwlock *rw)
{
    acquire(&rw->lk);

    rw->writing = 0;
    rw->writer = 0;

    // 唤醒所有等待的读者和写者
    wakeup(rw);

    release(&rw->lk);
}
```

---

## 6. 锁性能分析

### 6.1 锁竞争度量

```c
// kernel/lockstat.h

struct lock_stat {
    const char *name;
    uint64 acquires;
    uint64 misses;      // 获取失败次数
    uint64 spins;       // 自旋次数
    uint64 sleep;       // 睡眠次数 (对于睡眠锁)
    uint64 max_hold_time;  // 最大持有时间
    uint64 total_hold_time;
};

#define MAX_LOCK_STATS 100
extern struct lock_stat lock_stats[MAX_LOCK_STATS];
extern int lock_stat_count;

void lockstat_init(struct spinlock *lk, const char *name);
void lockstat_acquired(struct spinlock *lk);
void lockstat_released(struct spinlock *lk);
```

### 6.2 性能测试

```c
// 锁竞争测试
void lock_contention_test(void)
{
    struct spinlock test_lock;
    initlock(&test_lock, "test");

    int shared_counter = 0;

    // 在多个CPU上并行运行
    for (int i = 0; i < NCPU; i++) {
        if (fork() == 0) {
            // 绑定到特定CPU
            cpu_bind(i);

            for (int j = 0; j < 1000000; j++) {
                acquire(&test_lock);
                shared_counter++;
                release(&test_lock);
            }

            exit(0);
        }
    }

    // 等待所有子进程
    for (int i = 0; i < NCPU; i++) {
        wait(0);
    }

    printf("Final counter: %d (expected: %d)\n",
           shared_counter, NCPU * 1000000);
}
```

---

## 7. 调试技巧

### 7.1 常见锁问题

```
问题1: 死锁
症状: 系统挂起
诊断: 检查锁获取顺序，使用lockdebug_dump()

问题2: 数据竞争
症状: 随机崩溃，数据不一致
诊断: 检查共享数据是否都被锁保护

问题3: 锁竞争严重
症状: 多核性能差
诊断: 使用lockstat查看spin次数

问题4: 忘记释放锁
症状: 系统挂起或异常行为
诊断: 检查所有代码路径是否都释放锁
```

### 7.2 锁调试宏

```c
#ifdef DEBUG_LOCK

#define LOCK_TRACE(lk, op) \
    printf("[%s] CPU%d %s %s at %s:%d\n", \
           ticks ? "K" : "B", cpuid(), op, (lk)->name, __FILE__, __LINE__)

#define ACQUIRE(lk) do { \
    LOCK_TRACE(lk, "acquiring"); \
    acquire(lk); \
    LOCK_TRACE(lk, "acquired"); \
} while(0)

#define RELEASE(lk) do { \
    LOCK_TRACE(lk, "releasing"); \
    release(lk); \
    LOCK_TRACE(lk, "released"); \
} while(0)

#else
#define ACQUIRE(lk) acquire(lk)
#define RELEASE(lk) release(lk)
#endif
```

---

## 8. 总结

### 8.1 核心概念

| 概念 | 说明 |
|------|------|
| **原子性** | 操作不可中断，要么全做要么不做 |
| **互斥** | 同一时间只有一个进程进入临界区 |
| **自旋** | 忙等待直到获取锁 |
| **阻塞** | 睡眠等待直到锁可用 |
| **死锁** | 循环等待导致系统停滞 |

### 8.2 最佳实践

1. **保持临界区短小**: 减少锁持有时间
2. **统一的锁顺序**: 避免死锁
3. **选择合适的锁类型**: 自旋锁vs睡眠锁
4. **避免在持有锁时睡眠**: 除非使用睡眠锁
5. **考虑锁粒度**: 细粒度提高并发但增加复杂度

---

**难度**: ⭐⭐⭐⭐
**预计时间**: 10-12小时
**依赖**: Lab 6 (理解并发)


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
