# 内核同步机制

> **层级**: L5 (原理层)
> **目标**: 深入理解 Linux 内核同步原语、实现机制与性能优化
> **更新日期**: 2026-03-28
> **适用内核**: Linux 5.15+ / 6.x

---

## 目录

- [内核同步机制](#内核同步机制)
  - [目录](#目录)
  - [本节概要](#本节概要)
  - [并发与同步基础](#并发与同步基础)
    - [并发源分析](#并发源分析)
    - [同步原语对比](#同步原语对比)
  - [原子操作](#原子操作)
    - [原子整数操作](#原子整数操作)
    - [原子位操作](#原子位操作)
  - [内存屏障](#内存屏障)
    - [内存排序模型](#内存排序模型)
    - [volatile与内存屏障](#volatile与内存屏障)
  - [自旋锁](#自旋锁)
    - [自旋锁基础](#自旋锁基础)
    - [自旋锁变体](#自旋锁变体)
  - [信号量与互斥锁](#信号量与互斥锁)
    - [信号量](#信号量)
    - [互斥锁](#互斥锁)
    - [读写信号量](#读写信号量)
  - [RCU机制](#rcu机制)
    - [RCU核心概念](#rcu核心概念)
    - [RCU链表操作](#rcu链表操作)
    - [宽限期同步](#宽限期同步)
  - [顺序锁与完成量](#顺序锁与完成量)
    - [顺序锁](#顺序锁)
    - [完成量](#完成量)
  - [per-CPU变量](#per-cpu变量)
  - [锁粒度与性能优化](#锁粒度与性能优化)
    - [锁粒度设计](#锁粒度设计)
    - [锁消除技术](#锁消除技术)
  - [死锁检测与调试](#死锁检测与调试)
    - [Lockdep工具](#lockdep工具)
    - [常见死锁模式](#常见死锁模式)
    - [调试技术](#调试技术)
  - [实践指南](#实践指南)
    - [内核模块同步示例](#内核模块同步示例)
  - [层次关联与映射分析](#层次关联与映射分析)
  - [决策矩阵](#决策矩阵)
    - [同步原语选择决策树](#同步原语选择决策树)
    - [性能优化决策表](#性能优化决策表)
  - [相关资源](#相关资源)
    - [官方文档](#官方文档)
    - [书籍](#书籍)
    - [论文与文章](#论文与文章)
    - [工具](#工具)
  - [质量验收清单](#质量验收清单)

---

## 本节概要

| 属性 | 内容 |
|------|------|
| **核心主题** | Linux内核并发控制机制与同步原语 |
| **关键概念** | 原子操作、内存屏障、自旋锁、互斥锁、RCU、读写锁 |
| **适用场景** | 多核SMP系统、中断处理、内核模块开发、驱动程序 |
| **性能目标** | 低延迟、高吞吐、可扩展性、避免伪共享 |
| **调试工具** | lockdep、perf、ftrace、KASAN、KCSAN |
| **内核版本** | Linux 5.15+ (推荐6.x) |

---

## 并发与同步基础

### 并发源分析

Linux内核面临多种并发源，需要不同的同步策略：

```
┌─────────────────────────────────────────────────────────────────┐
│                    Linux内核并发源                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  1. 多处理器(SMP)        N个CPU核心同时执行内核代码            │
│     → 自旋锁、互斥锁、RCU                                      │
│                                                                 │
│  2. 抢占式内核           高优先级任务抢占低优先级任务          │
│     → 抢占禁用、自旋锁                                         │
│                                                                 │
│  3. 中断处理             硬件中断随时打断当前执行流            │
│     → 中断禁用、local_irq_save()                               │
│                                                                 │
│  4. 软中断/Tasklet       中断下半部异步执行                    │
│     → 软中断禁用、local_bh_disable()                           │
│                                                                 │
│  5. 内核线程             多个内核线程访问共享数据              │
│     → 完整的锁机制                                             │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 同步原语对比

| 原语 | 睡眠能力 | 中断上下文 | 适用场景 | 性能特点 |
|------|----------|------------|----------|----------|
| **原子变量** | 否 | 是 | 计数器、标志位 | 最优，无锁 |
| **自旋锁** | 否 | 是(特定类型) | 短临界区、中断 | 忙等待，低延迟 |
| **信号量** | 是 | 否 | 长临界区 | 可能睡眠 |
| **互斥锁** | 是 | 否 | 保护资源访问 | 优先级继承 |
| **RCU** | 否 | 是 | 读多写少 | 读侧无锁 |
| **顺序锁** | 否 | 是 | 写多读少 | 写优先 |

---

## 原子操作

### 原子整数操作

```c
#include <linux/atomic.h>

/**
 * 原子整数操作详解
 *
 * 原子操作保证操作的不可分割性，是构建更复杂同步机制的基础。
 * 在x86上通常使用LOCK前缀或XCHG指令实现。
 */

// 定义和初始化
atomic_t counter = ATOMIC_INIT(0);

// 基本操作
void atomic_operations_demo(void) {
    int v;

    // 读取 - 保证读取的完整性
    v = atomic_read(&counter);

    // 设置
    atomic_set(&counter, 100);

    // 增加/减少 - 返回新值 (非void版本可用)
    atomic_inc(&counter);           // ++counter
    atomic_dec(&counter);           // --counter

    // 带返回值的操作
    v = atomic_inc_return(&counter);
    v = atomic_dec_return(&counter);

    // 加/减指定值
    atomic_add(10, &counter);
    atomic_sub(5, &counter);
    v = atomic_add_return(10, &counter);

    // 测试并操作
    v = atomic_sub_and_test(1, &counter);  // 递减后为0返回true
    v = atomic_dec_and_test(&counter);
    v = atomic_inc_and_test(&counter);

    // 交换操作
    v = atomic_xchg(&counter, 200);  // 设置新值，返回旧值

    // 比较并交换 (CAS) - 实现无锁算法的基础
    int old = 100, new = 200;
    v = atomic_cmpxchg(&counter, old, new);  // 如果counter==old，则设为new，返回实际值
}

// 64位原子操作 (x86_64, arm64)
atomic64_t big_counter = ATOMIC64_INIT(0);

void atomic64_operations(void) {
    long long v;

    atomic64_set(&big_counter, 10000000000LL);
    v = atomic64_read(&big_counter);
    atomic64_inc(&big_counter);
    v = atomic64_inc_return(&big_counter);
}
```

### 原子位操作

```c
#include <linux/bitops.h>

/**
 * 原子位操作
 *
 * 用于操作位图、标志位等。比读-改-写操作更高效、安全。
 */

// 非原子位操作 (需要外部锁保护)
void set_bit(unsigned int nr, volatile unsigned long *addr);
void clear_bit(unsigned int nr, volatile unsigned long *addr);
void change_bit(unsigned int nr, volatile unsigned long *addr);
int test_and_set_bit(unsigned int nr, volatile unsigned long *addr);
int test_and_clear_bit(unsigned int nr, volatile unsigned long *addr);

// 原子位操作示例
void bitmap_operations(void) {
    unsigned long flags = 0;

    // 设置第5位
    set_bit(5, &flags);

    // 测试并设置 (原子操作，常用于实现锁)
    if (!test_and_set_bit(0, &flags)) {
        // 成功获取锁
        // ... 临界区 ...
        clear_bit(0, &flags);  // 释放锁
    }

    // 查找第一个置位/清零位
    int first_set = __ffs(flags);        // 从最低位开始找
    int first_zero = ffz(flags);         // 找第一个0

    // 统计置位数量
    int count = hweight_long(flags);
}

// 典型用法: 设备状态标志
#define DEVICE_OPEN     0
#define DEVICE_BUSY     1
#define DEVICE_ERROR    2

struct my_device {
    unsigned long state;
};

void device_open(struct my_device *dev) {
    if (test_and_set_bit(DEVICE_OPEN, &dev->state)) {
        // 设备已打开
        return;
    }
    // 执行打开操作
}
```

---

## 内存屏障

### 内存排序模型

```c
#include <linux/barrier.h>

/**
 * 内存屏障详解
 *
 * 现代CPU和编译器会进行指令重排序以优化性能，但在多核环境下
 * 这可能导致内存访问顺序与代码顺序不一致。
 *
 * 内存屏障确保特定的内存访问顺序。
 */

// 编译器屏障 - 防止编译器重排序
#define barrier() __asm__ __volatile__("" ::: "memory")

// CPU内存屏障类型
void memory_barrier_examples(void) {
    int a = 1, b = 2, c = 3;

    // mb() - 全内存屏障
    // 确保屏障前的所有内存访问完成后，才执行屏障后的访问
    a = 10;
    mb();  // 全屏障
    b = 20;
    // 保证: a=10对其他CPU可见后，b=20才会被执行

    // rmb() - 读内存屏障
    // 确保屏障前的读操作完成后，才执行屏障后的读操作
    int x = global_var1;
    rmb();
    int y = global_var2;

    // wmb() - 写内存屏障
    // 确保屏障前的写操作完成后，才执行屏障后的写操作
    data_buffer = new_data;
    wmb();
    data_ready_flag = 1;

    // smp_mb() / smp_rmb() / smp_wmb() - SMP优化版本
    // 在单处理器编译时为空操作，SMP编译时与上述等价
}

// 典型用法: 生产者-消费者模式
struct channel {
    int data;
    int ready;
};

void producer(struct channel *ch, int value) {
    ch->data = value;
    smp_wmb();           // 确保data写入在ready之前
    ch->ready = 1;
}

int consumer(struct channel *ch) {
    while (!ch->ready)   // 忙等待 (实际应用使用锁或等待队列)
        cpu_relax();
    smp_rmb();           // 确保读取ready后，才读取data
    return ch->data;
}
```

### volatile与内存屏障

```c
/**
 * volatile vs 内存屏障
 *
 * volatile: 告诉编译器不要优化内存访问，每次都从内存读取
 * 内存屏障: 告诉CPU和编译器保持内存访问的顺序
 *
 * 注意: volatile不能替代内存屏障!
 */

// 错误的用法
volatile int flag = 0;
volatile int data = 0;

void wrong_producer(void) {
    data = 42;          // 可能被重排序到flag之后!
    flag = 1;
}

// 正确的用法
void correct_producer(void) {
    data = 42;
    smp_wmb();          // 显式屏障确保顺序
    flag = 1;
}
```

---

## 自旋锁

### 自旋锁基础

```c
#include <linux/spinlock.h>

/**
 * 自旋锁详解
 *
 * 自旋锁是一种忙等待锁，获取锁失败时CPU会不断轮询(spin)直到获取成功。
 * 适用于短临界区，避免上下文切换开销。
 */

// 定义和初始化
spinlock_t my_lock;

void spinlock_init(void) {
    spin_lock_init(&my_lock);
}

// 静态初始化
static DEFINE_SPINLOCK(static_lock);

// 基本使用模式
void spinlock_basic_usage(void) {
    unsigned long flags;

    // 方式1: 保存中断状态并禁用本地中断 (推荐用于中断上下文)
    spin_lock_irqsave(&my_lock, flags);
    // 临界区 - 中断被禁用，防止死锁
    spin_unlock_irqrestore(&my_lock, flags);

    // 方式2: 仅禁用软中断 (进程上下文，不涉及硬件中断)
    spin_lock_bh(&my_lock);
    // 临界区
    spin_unlock_bh(&my_lock);

    // 方式3: 纯自旋锁 (已知不运行在中断上下文)
    spin_lock(&my_lock);
    // 临界区
    spin_unlock(&my_lock);
}

// 尝试获取锁
void spinlock_try(void) {
    // 立即返回，不等待
    if (spin_trylock(&my_lock)) {
        // 获取成功
        // 临界区
        spin_unlock(&my_lock);
    } else {
        // 获取失败，锁正被占用
        // 可以做其他工作或忙等待
    }
}
```

### 自旋锁变体

```c
/**
 * 自旋锁变体
 */

// 读写自旋锁 - 允许多读者或单写者
rwlock_t my_rwlock;
DEFINE_RWLOCK(my_rwlock);

void rwlock_usage(void) {
    // 读锁 - 共享访问
    read_lock(&my_rwlock);
    // 读临界区 - 允许多个读者同时进入
    read_unlock(&my_rwlock);

    // 写锁 - 独占访问
    write_lock(&my_rwlock);
    // 写临界区 - 独占访问
    write_unlock(&my_rwlock);

    // 带中断保存的版本
    unsigned long flags;
    read_lock_irqsave(&my_rwlock, flags);
    read_unlock_irqrestore(&my_rwlock, flags);
}

// 顺序锁 (seqlock) - 写者优先，读者无锁
seqlock_t my_seqlock;

void seqlock_usage(void) {
    // 写操作
    write_seqlock(&my_seqlock);
    // 写临界区
    write_sequnlock(&my_seqlock);

    // 读操作 - 可能重试
    unsigned seq;
    do {
        seq = read_seqbegin(&my_seqlock);
        // 复制数据到本地
        int local_copy = global_data;
    } while (read_seqretry(&my_seqlock, seq));
}

// 原始自旋锁 (raw_spinlock) - 不可睡眠版本
raw_spinlock_t my_raw_lock;

void raw_spinlock_usage(void) {
    raw_spin_lock(&my_raw_lock);
    // 绝对不可睡眠的临界区
    raw_spin_unlock(&my_raw_lock);
}
```

---

## 信号量与互斥锁

### 信号量

```c
#include <linux/semaphore.h>

/**
 * 信号量详解
 *
 * 信号量允许指定数量的持有者同时访问资源。
 * 当无法获取信号量时，进程会睡眠等待。
 */

// 二值信号量 (类似互斥锁)
struct semaphore sem;

void semaphore_init(void) {
    sema_init(&sem, 1);  // 初始计数为1
}

// 计数信号量
struct semaphore resource_sem;

void counting_semaphore(void) {
    sema_init(&resource_sem, 5);  // 最多5个并发访问

    // 获取信号量 (可能睡眠)
    if (down_interruptible(&resource_sem) != 0) {
        // 被信号中断
        return;
    }
    // 使用资源
    up(&resource_sem);  // 释放

    // 不可中断版本
    down(&resource_sem);
    up(&resource_sem);

    // 尝试获取 (不等待)
    if (down_trylock(&resource_sem) == 0) {
        // 获取成功
        up(&resource_sem);
    }
}
```

### 互斥锁

```c
#include <linux/mutex.h>

/**
 * 互斥锁详解
 *
 * 互斥锁是二值信号量的特化版本，专为互斥访问设计。
 * 特点:
 * - 同一任务可以重复加锁 (递归锁)
 * - 支持优先级继承，防止优先级反转
 * - 解锁只能由加锁者执行
 * - 不能在中断上下文使用
 */

// 定义和初始化
struct mutex my_mutex;

void mutex_init(void) {
    mutex_init(&my_mutex);
}

// 静态初始化
static DEFINE_MUTEX(static_mutex);

void mutex_usage(void) {
    // 获取互斥锁 (可中断)
    if (mutex_lock_interruptible(&my_mutex) != 0) {
        // 被信号中断
        return;
    }
    // 临界区
    mutex_unlock(&my_mutex);

    // 不可中断版本
    mutex_lock(&my_mutex);
    mutex_unlock(&my_mutex);

    // 尝试获取
    if (mutex_trylock(&my_mutex)) {
        // 获取成功
        mutex_unlock(&my_mutex);
    }

    // 带超时
    if (mutex_lock_killable(&my_mutex) == 0) {
        mutex_unlock(&my_mutex);
    }
}

// 典型使用场景: 保护数据结构
struct my_data {
    struct mutex lock;
    int value;
    struct list_head items;
};

void update_data(struct my_data *data, int new_value) {
    mutex_lock(&data->lock);
    data->value = new_value;
    mutex_unlock(&data->lock);
}
```

### 读写信号量

```c
#include <linux/rwsem.h>

/**
 * 读写信号量
 *
 * 允许多个读者或单个写者，但实现上是睡眠锁。
 */
struct rw_semaphore my_rwsem;

void rwsem_usage(void) {
    init_rwsem(&my_rwsem);

    // 读锁
    down_read(&my_rwsem);
    // 读临界区
    up_read(&my_rwsem);

    // 写锁
    down_write(&my_rwsem);
    // 写临界区
    up_write(&my_rwsem);

    // 尝试获取
    if (down_read_trylock(&my_rwsem)) {
        up_read(&my_rwsem);
    }
}
```

---

## RCU机制

### RCU核心概念

```c
#include <linux/rcupdate.h>
#include <linux/rculist.h>

/**
 * RCU (Read-Copy-Update) 详解
 *
 * RCU是一种高性能的读侧无锁同步机制。
 *
 * 核心思想:
 * 1. 读侧: 无锁访问，通过rcu_read_lock()标记读侧临界区
 * 2. 写侧: 复制修改，原子指针替换
 * 3. 回收: 等待所有读者完成后再释放旧数据
 *
 * 适用场景: 读多写少的数据结构
 */

// RCU保护的数据结构
struct my_rcu_data {
    int value;
    struct list_head list;
    struct rcu_head rcu;  // 用于延迟释放
};

// 全局指针 (RCU保护)
struct my_rcu_data __rcu *g_data;

// 读侧临界区
void rcu_reader(void) {
    // 进入RCU读侧临界区 - 禁止抢占，但不禁止中断
    rcu_read_lock();

    // 安全访问RCU保护的数据
    struct my_rcu_data *data = rcu_dereference(g_data);
    if (data) {
        int v = data->value;  // 安全读取
        (void)v;
    }

    // 退出读侧临界区
    rcu_read_unlock();
}

// 写侧更新
void rcu_updater(struct my_rcu_data *new_data) {
    struct my_rcu_data *old_data;

    // 创建新数据副本 (Copy)
    // 修改新数据 ...

    // 原子替换指针 (Update)
    old_data = rcu_replace_pointer(g_data, new_data, true);

    // 安排旧数据延迟释放
    // 等待宽限期结束后调用回调函数
    kfree_rcu(old_data, rcu);
}

// 自定义回调释放
static void my_rcu_free(struct rcu_head *rcu) {
    struct my_rcu_data *data = container_of(rcu, struct my_rcu_data, rcu);
    kfree(data);
}

void rcu_updater_custom(struct my_rcu_data *new_data) {
    struct my_rcu_data *old_data = rcu_replace_pointer(g_data, new_data, true);
    call_rcu(&old_data->rcu, my_rcu_free);
}
```

### RCU链表操作

```c
/**
 * RCU保护的链表操作
 */

struct my_list_entry {
    int data;
    struct list_head list;
    struct rcu_head rcu;
};

LIST_HEAD(my_rcu_list);

// 遍历RCU链表 (读侧)
void rcu_list_traverse(void) {
    struct my_list_entry *entry;

    rcu_read_lock();

    // list_for_each_entry_rcu 是RCU安全的遍历
    list_for_each_entry_rcu(entry, &my_rcu_list, list) {
        // 访问 entry->data
        printk("Data: %d\n", entry->data);
    }

    rcu_read_unlock();
}

// 添加元素 (写侧)
void rcu_list_add(struct my_list_entry *new_entry) {
    // 初始化新元素
    INIT_LIST_HEAD(&new_entry->list);

    // 添加元素 - 使用_rcu后缀的函数
    list_add_rcu(&new_entry->list, &my_rcu_list);
}

// 删除元素 (写侧)
void rcu_list_del(struct my_list_entry *entry) {
    // 从链表移除
    list_del_rcu(&entry->list);

    // 等待宽限期后释放
    kfree_rcu(entry, rcu);
}

// 替换元素
void rcu_list_replace(struct my_list_entry *old, struct my_list_entry *new) {
    list_replace_rcu(&old->list, &new->list);
    kfree_rcu(old, rcu);
}
```

### 宽限期同步

```c
/**
 * RCU宽限期
 *
 * 宽限期(Grace Period)是RCU的核心概念：
 * 确保所有在读侧临界区开始的读者都已完成。
 */

void rcu_synchronize_example(void) {
    // 同步等待宽限期结束 (可能睡眠)
    synchronize_rcu();

    // 现在可以安全释放旧数据
    // 所有之前的读者都已完成

    // 异步等待 - 不睡眠，使用回调
    call_rcu(&some_data->rcu, free_callback);

    //  expedited 版本 - 更快但开销更大
    synchronize_rcu_expedited();
}

// RCU屏障 - 确保所有挂起的RCU回调已完成
void rcu_barrier_example(void) {
    rcu_barrier();  // 等待所有RCU回调执行完毕
}
```

---

## 顺序锁与完成量

### 顺序锁

```c
#include <linux/seqlock.h>

/**
 * 顺序锁 (seqlock) 详解
 *
 * 特点:
 * - 写者优先，读者可能重试
 * - 读者无锁，性能极高
 * - 适用于写少读多且数据一致性要求高的场景
 *
 * 典型应用: jiffies变量、系统时间
 */

seqlock_t jiffies_lock;
u64 jiffies_64;

void seqlock_writer(u64 new_jiffies) {
    write_seqlock(&jiffies_lock);
    jiffies_64 = new_jiffies;
    write_sequnlock(&jiffies_lock);
}

u64 seqlock_reader(void) {
    u64 j;
    unsigned seq;

    do {
        seq = read_seqbegin(&jiffies_lock);
        j = jiffies_64;  // 读取64位值
        // 在32位系统上，这可能需要两次内存访问
    } while (read_seqretry(&jiffies_lock, seq));

    return j;
}

// 带中断保存的版本
void seqlock_irqsave_example(void) {
    unsigned long flags;

    write_seqlock_irqsave(&jiffies_lock, flags);
    // 临界区
    write_sequnlock_irqrestore(&jiffies_lock, flags);
}
```

### 完成量

```c
#include <linux/completion.h>

/**
 * 完成量 (completion) 详解
 *
 * 用于一个线程通知另一个线程某个事件已完成。
 * 类似于信号量，但语义更清晰。
 */

struct completion my_completion;

void completion_init(void) {
    init_completion(&my_completion);
    // 或静态初始化
    // DECLARE_COMPLETION(my_completion);
}

// 等待完成
void completion_waiter(void) {
    // 等待完成 (不可中断)
    wait_for_completion(&my_completion);

    // 可中断等待
    if (wait_for_completion_interruptible(&my_completion) != 0) {
        // 被信号中断
    }

    // 带超时等待
    unsigned long timeout = msecs_to_jiffies(5000);
    if (!wait_for_completion_timeout(&my_completion, timeout)) {
        // 超时
    }
}

// 发出完成信号
void completion_signaler(void) {
    // 唤醒一个等待者
    complete(&my_completion);

    // 唤醒所有等待者
    complete_all(&my_completion);

    // 重新初始化 (用于complete_all后复用)
    reinit_completion(&my_completion);
}

// 典型用法: 线程同步
struct task_struct *worker_thread;
struct completion work_done;

int worker_function(void *data) {
    while (!kthread_should_stop()) {
        // 执行工作
        do_work();

        // 通知主线程
        complete(&work_done);
    }
    return 0;
}

void start_worker(void) {
    init_completion(&work_done);
    worker_thread = kthread_run(worker_function, NULL, "worker");

    // 等待工作完成
    wait_for_completion(&work_done);
}
```

---

## per-CPU变量

```c
#include <linux/percpu.h>

/**
 * per-CPU变量详解
 *
 * 每个CPU拥有独立副本，避免缓存行乒乓(Cache Line Bouncing)。
 * 是高可扩展性的关键优化。
 */

// 定义per-CPU变量
DEFINE_PER_CPU(int, cpu_counter);
DEFINE_PER_CPU(struct statistics, cpu_stats);

void percpu_usage(void) {
    // 获取当前CPU的变量地址
    int __percpu *counter_ptr = this_cpu_ptr(&cpu_counter);

    // 直接操作 (关闭抢占)
    preempt_disable();
    __this_cpu_inc(cpu_counter);
    preempt_enable();

    // 原子操作 (无需显式禁用抢占)
    this_cpu_inc(cpu_counter);
    this_cpu_add(cpu_counter, 10);
}

// 遍历所有CPU
void percpu_aggregate(void) {
    int total = 0;
    int cpu;

    for_each_possible_cpu(cpu) {
        total += per_cpu(cpu_counter, cpu);
    }

    printk("Total counter: %d\n", total);
}

// 动态分配per-CPU变量
void percpu_dynamic(void) {
    struct my_data __percpu *data;

    // 分配
    data = alloc_percpu(struct my_data);
    if (!data)
        return;

    // 使用
    struct my_data *ptr = this_cpu_ptr(data);
    ptr->value = 100;

    // 释放
    free_percpu(data);
}
```

---

## 锁粒度与性能优化

### 锁粒度设计

```c
/**
 * 锁粒度优化策略
 */

// 糟糕: 全局大锁
static DEFINE_MUTEX(global_lock);
struct list_head global_list;

// 优化: 细粒度锁 - 哈希桶锁
#define HASH_SIZE 256
struct hash_bucket {
    struct mutex lock;
    struct hlist_head head;
};
static struct hash_bucket hash_table[HASH_SIZE];

void fine_grained_init(void) {
    for (int i = 0; i < HASH_SIZE; i++) {
        mutex_init(&hash_table[i].lock);
        INIT_HLIST_HEAD(&hash_table[i].head);
    }
}

// 根据key选择锁
static inline struct hash_bucket *get_bucket(unsigned int key) {
    return &hash_table[key % HASH_SIZE];
}

void fine_grained_insert(struct my_node *node, unsigned int key) {
    struct hash_bucket *bucket = get_bucket(key);

    mutex_lock(&bucket->lock);
    hlist_add_head(&node->list, &bucket->head);
    mutex_unlock(&bucket->lock);
}
```

### 锁消除技术

```c
/**
 * 锁消除技术
 */

// 1. RCU替代读写锁
// 从 rwlock_t 改为 seqlock_t 或 RCU

// 2. 原子操作替代锁
// 使用原子变量进行计数
atomic_t ref_count;

void ref_get(void) {
    atomic_inc(&ref_count);
}

int ref_put(void) {
    if (atomic_dec_and_test(&ref_count)) {
        // 最后一个引用，可以释放
        return 1;
    }
    return 0;
}

// 3. per-CPU计数器
DEFINE_PER_CPU(long, event_count);

void count_event(void) {
    this_cpu_inc(event_count);
}

// 定期聚合 (或读取时聚合)
long get_total_events(void) {
    long total = 0;
    int cpu;

    for_each_online_cpu(cpu) {
        total += per_cpu(event_count, cpu);
    }
    return total;
}

// 4. 无锁队列 - LIFO栈
struct lfs_stack {
    void __rcu *top;
};

void lfs_push(struct lfs_stack *stack, struct node *node) {
    node->next = rcu_dereference_protected(stack->top, 1);
    rcu_assign_pointer(stack->top, node);
}

struct node *lfs_pop(struct lfs_stack *stack) {
    struct node *node;

    do {
        node = rcu_dereference(stack->top);
        if (!node)
            return NULL;
    } while (cmpxchg(&stack->top, node, node->next) != node);

    return node;
}
```

---

## 死锁检测与调试

### Lockdep工具

```
Lockdep是内核的锁依赖关系验证器，用于检测潜在死锁。

启用方式:
CONFIG_LOCKDEP=y
CONFIG_LOCK_STAT=y
CONFIG_DEBUG_LOCK_ALLOC=y

使用:
1. 启动后检查 dmesg 中的 lockdep 警告
2. /proc/lockdep 查看依赖关系
3. /proc/lock_stat 查看锁统计
```

### 常见死锁模式

```c
/**
 * 常见死锁模式及避免方法
 */

// 1. 自死锁 - 递归获取同一锁
void self_deadlock(void) {
    spin_lock(&lock);
    // ... 代码路径又获取 lock ...
    spin_lock(&lock);  // DEADLOCK!
}

// 解决: 使用递归锁或重新设计
// mutex支持递归，spinlock不支持

// 2. AB-BA死锁
spinlock_t lock_a, lock_b;

void thread1(void) {
    spin_lock(&lock_a);
    spin_lock(&lock_b);  // 等待thread2释放lock_b
    // ...
    spin_unlock(&lock_b);
    spin_unlock(&lock_a);
}

void thread2(void) {
    spin_lock(&lock_b);
    spin_lock(&lock_a);  // 等待thread1释放lock_a -> DEADLOCK!
    // ...
    spin_unlock(&lock_a);
    spin_unlock(&lock_b);
}

// 解决: 全局锁排序
// 总是先获取lock_a，再获取lock_b

// 3. 中断死锁
void irq_deadlock(void) {
    spin_lock(&lock);  // 未禁用中断
    // 中断发生，中断处理程序也尝试获取lock -> DEADLOCK!
}

// 解决: 使用spin_lock_irqsave()
```

### 调试技术

```c
/**
 * 调试技术
 */

// 1. 断言检查
#define assert_spin_locked(lock)    BUG_ON(!spin_is_locked(lock))
#define assert_mutex_locked(mutex)  lockdep_assert_held(mutex)

// 2. 锁信息打印
void debug_lock_info(void) {
    printk("Lock held: %d\n", spin_is_locked(&my_lock));
}

// 3. KCSAN - 内核并发消毒剂
// 检测数据竞争
// CONFIG_KCSAN=y

// 4. KASAN - 内核地址消毒剂
// 检测内存错误
// CONFIG_KASAN=y
```

---

## 实践指南

### 内核模块同步示例

```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/rcupdate.h>

/**
 * 安全的字符设备驱动示例
 * 使用多种同步机制保护共享数据
 */

#define DEVICE_NAME "syncdev"
#define MAX_DATA_SIZE 1024

struct device_data {
    struct mutex lock;
    char buffer[MAX_DATA_SIZE];
    size_t len;
    atomic_t open_count;
    struct rcu_head rcu;
};

static struct device_data __rcu *g_dev_data;
static dev_t dev_num;
static struct cdev my_cdev;

// 设备打开
static int device_open(struct inode *inode, struct file *filp) {
    struct device_data *data;

    rcu_read_lock();
    data = rcu_dereference(g_dev_data);
    atomic_inc(&data->open_count);
    rcu_read_unlock();

    filp->private_data = data;
    return 0;
}

// 设备读取
static ssize_t device_read(struct file *filp, char __user *buf,
                           size_t count, loff_t *offset) {
    struct device_data *data = filp->private_data;
    size_t to_copy;

    // 使用mutex保护数据访问
    if (mutex_lock_interruptible(&data->lock))
        return -ERESTARTSYS;

    to_copy = min(count, data->len - (size_t)*offset);
    if (to_copy == 0) {
        mutex_unlock(&data->lock);
        return 0;
    }

    if (copy_to_user(buf, data->buffer + *offset, to_copy)) {
        mutex_unlock(&data->lock);
        return -EFAULT;
    }

    *offset += to_copy;
    mutex_unlock(&data->lock);

    return to_copy;
}

// 设备写入
static ssize_t device_write(struct file *filp, const char __user *buf,
                            size_t count, loff_t *offset) {
    struct device_data *data = filp->private_data;
    size_t to_copy;

    if (count > MAX_DATA_SIZE)
        return -EINVAL;

    if (mutex_lock_interruptible(&data->lock))
        return -ERESTARTSYS;

    to_copy = min(count, (size_t)MAX_DATA_SIZE);
    if (copy_from_user(data->buffer, buf, to_copy)) {
        mutex_unlock(&data->lock);
        return -EFAULT;
    }

    data->len = to_copy;
    mutex_unlock(&data->lock);

    return to_copy;
}

// 设备关闭
static int device_release(struct inode *inode, struct file *filp) {
    struct device_data *data = filp->private_data;
    atomic_dec(&data->open_count);
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .read = device_read,
    .write = device_write,
    .release = device_release,
};

// 模块初始化
static int __init syncdev_init(void) {
    struct device_data *data;

    data = kzalloc(sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    mutex_init(&data->lock);
    atomic_set(&data->open_count, 0);
    data->len = 0;

    rcu_assign_pointer(g_dev_data, data);

    if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME) < 0) {
        kfree(data);
        return -1;
    }

    cdev_init(&my_cdev, &fops);
    if (cdev_add(&my_cdev, dev_num, 1) < 0) {
        unregister_chrdev_region(dev_num, 1);
        kfree(data);
        return -1;
    }

    printk(KERN_INFO "syncdev: initialized\n");
    return 0;
}

// 模块清理
static void __exit syncdev_exit(void) {
    struct device_data *data;

    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);

    data = rcu_dereference_protected(g_dev_data, 1);
    synchronize_rcu();
    kfree(data);

    printk(KERN_INFO "syncdev: exited\n");
}

module_init(syncdev_init);
module_exit(syncdev_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Synchronization Example Device");
```

---

## 层次关联与映射分析

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        内核同步机制层次映射                             │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  L7 应用层         用户态同步: pthread_mutex, futex, 信号量            │
│       ↑                    ↓                                            │
│  L6 系统调用层     futex系统调用, 进程间信号量                          │
│       ↑                    ↓                                            │
│  L5 内核API层      mutex, rw_semaphore, completion, workqueue          │
│       ↑                    ↓                                            │
│  L4 核心原语层     spinlock, RCU, 原子操作, 顺序锁                      │
│       ↑                    ↓                                            │
│  L3 硬件抽象层     内存屏障, CPU原子指令, 缓存一致性协议                │
│       ↑                    ↓                                            │
│  L2 硬件层         LOCK前缀, LL/SC (ARM), CAS (x86), 缓存锁             │
│       ↑                    ↓                                            │
│  L1 物理层         总线仲裁, MESI协议, 内存序保证                       │
│                                                                         │
├─────────────────────────────────────────────────────────────────────────┤
│  跨层关注点: 调度/抢占控制 | 中断管理 | NUMA感知 | 能耗优化              │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 决策矩阵

### 同步原语选择决策树

```
需要保护共享数据?
├── 是 → 是否需要睡眠?
│       ├── 是 → 是否在中断上下文?
│       │       ├── 是 → 使用完成量或工作队列延迟处理
│       │       └── 否 → 需要递归锁?
│       │               ├── 是 → mutex
│       │               └── 否 → mutex或信号量
│       └── 否 → 访问模式?
│               ├── 读多写少 → RCU (最佳) 或 rwlock
│               ├── 写多读少 → seqlock 或 rwlock
│               ├── 读少写少 → spinlock
│               └── 仅计数器 → 原子操作
├── 否 → 需要线程同步?
│       ├── 是 → 完成量(completion) 或 等待队列
│       └── 否 → 无需同步
```

### 性能优化决策表

| 问题场景 | 优化策略 | 适用原语 |
|----------|----------|----------|
| 锁竞争激烈 | 细粒度锁 | 哈希桶锁、per-bucket mutex |
| 缓存行乒乓 | 数据对齐 + per-CPU | per_cpu变量, ____cacheline_aligned |
| 读性能瓶颈 | 读侧无锁 | RCU, seqlock |
| 高频率计数 | 消除锁 | 原子操作, per-CPU计数器 |
| 优先级反转 | 优先级继承 | RT-mutex |
| NUMA扩展性差 | NUMA感知设计 | per-node锁, NUMA-aware allocator |

---

## 相关资源

### 官方文档

- [Linux Kernel Documentation - Locking](https://www.kernel.org/doc/Documentation/locking/)
- [RCU Documentation](https://www.kernel.org/doc/Documentation/RCU/)
- [Memory Barriers](https://www.kernel.org/doc/Documentation/memory-barriers.txt)

### 书籍

- 《Linux内核设计与实现》(Robert Love)
- 《深入理解Linux内核》(Daniel P. Bovet)
- 《Linux设备驱动程序》(Jonathan Corbet)

### 论文与文章

- Paul E. McKenney的RCU论文系列
- "Is Parallel Programming Hard, And, If So, What Can You Do About It?"

### 工具

```bash
# lockdep统计
cat /proc/lock_stat

# 锁依赖关系
cat /proc/lockdep

# perf锁分析
perf lock record ./program
perf lock report

# ftrace跟踪
trace-cmd start -e lock:lock_acquire
trace-cmd show
```

---

## 质量验收清单

- [ ] **原子操作**: 正确使用原子API，避免读-改-写竞争
- [ ] **内存屏障**: 在多核访问共享数据时使用适当屏障
- [ ] **自旋锁**: 临界区短，不睡眠，不调用可能阻塞的函数
- [ ] **互斥锁**: 仅在进程上下文使用，考虑优先级继承
- [ ] **RCU**: 读侧无锁，写侧正确处理宽限期
- [ ] **中断安全**: 中断上下文使用适当的锁变体(irqsave)
- [ ] **锁排序**: 定义并遵守全局锁获取顺序
- [ ] **细粒度**: 避免全局大锁，使用分片/哈希锁
- [ ] **per-CPU**: 频繁更新的计数器使用per-CPU变量
- [ ] **调试**: 开发时启用lockdep、KASAN、KCSAN
- [ ] **文档**: 每个锁保护的数据结构有明确注释
- [ ] **性能测试**: 在高并发场景下验证扩展性

---

*文档结束*
