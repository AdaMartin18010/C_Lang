# MIT 6.S081 Lab 6: Multithreading

## 1. 实验概述

### 1.1 目标

- 理解用户级线程的概念和实现
- 实现线程上下文切换
- 理解线程调度机制
- 实现基本的同步原语
- 使用线程解决并行计算问题

### 1.2 线程 vs 进程

```
进程 (Process):
┌─────────────────────────────────┐
│  独立的地址空间                 │
│  独立的页表                     │
│  独立的打开文件                 │
│  独立的PID                      │
│  父子关系明确                   │
└─────────────────────────────────┘
           切换开销大

线程 (Thread):
┌─────────────────────────────────┐
│  共享地址空间                   │
│  共享页表                       │
│  共享打开文件                   │
│  独立的执行上下文               │
│  独立的栈                       │
└─────────────────────────────────┘
           切换开销小

关键区别:
- 线程共享代码段、数据段、堆
- 线程拥有独立的栈和寄存器
- 线程切换不需要切换页表
```

### 1.3 用户级线程模型

```
用户级线程 (User-Level Threads):

用户空间                       内核空间
┌──────────────┐              ┌──────────────┐
│ Thread 1     │              │              │
│  - 寄存器    │              │              │
│  - 栈        │              │   不知道     │
├──────────────┤  线程库     │   线程的     │
│ Thread 2     │  管理       │   存在       │
│  - 寄存器    │◄───────────►│              │
│  - 栈        │              │   只看到     │
├──────────────┤              │   一个       │
│ Thread 3     │              │   进程       │
│  - 寄存器    │              │              │
│  - 栈        │              │              │
└──────────────┘              └──────────────┘

优势:
- 切换不需要陷入内核 (快)
- 调度策略由应用控制
- 可以创建大量线程

劣势:
- 一个线程阻塞会导致所有线程阻塞
- 无法利用多核 (xv6场景)
```

---

## 2. 线程上下文切换

### 2.1 上下文定义

```c
// user/uthread.c

// 线程上下文 - 保存所有callee-saved寄存器
// 加上ra和sp
struct context {
    uint64 ra;
    uint64 sp;

    // callee-saved寄存器
    uint64 s0;
    uint64 s1;
    uint64 s2;
    uint64 s3;
    uint64 s4;
    uint64 s5;
    uint64 s6;
    uint64 s7;
    uint64 s8;
    uint64 s9;
    uint64 s10;
    uint64 s11;
};

// 线程结构
struct thread {
    char       stack[STACK_SIZE];  // 线程栈
    struct context context;         // 保存的上下文
    int        state;               // 线程状态
};

enum { FREE, RUNNING, RUNNABLE };

struct thread threads[MAX_THREAD];
struct thread *current_thread;
```

### 2.2 上下文切换汇编代码

```asm
# user/uthread_switch.S

# void thread_switch(struct context *old, struct context *new);
# 保存old上下文，恢复new上下文

.globl thread_switch
thread_switch:
    # 保存old寄存器
    sd ra, 0(a0)
    sd sp, 8(a0)
    sd s0, 16(a0)
    sd s1, 24(a0)
    sd s2, 32(a0)
    sd s3, 40(a0)
    sd s4, 48(a0)
    sd s5, 56(a0)
    sd s6, 64(a0)
    sd s7, 72(a0)
    sd s8, 80(a0)
    sd s9, 88(a0)
    sd s10, 96(a0)
    sd s11, 104(a0)

    # 恢复new寄存器
    ld ra, 0(a1)
    ld sp, 8(a1)
    ld s0, 16(a1)
    ld s1, 24(a1)
    ld s2, 32(a1)
    ld s3, 40(a1)
    ld s4, 48(a1)
    ld s5, 56(a1)
    ld s6, 64(a1)
    ld s7, 72(a1)
    ld s8, 80(a1)
    ld s9, 88(a1)
    ld s10, 96(a1)
    ld s11, 104(a1)

    ret
```

### 2.3 线程调度器

```c
// user/uthread.c

void
thread_schedule(void)
{
    struct thread *t, *next_thread;

    // 找到下一个可运行的线程
    next_thread = 0;
    t = current_thread + 1;

    for (int i = 0; i < MAX_THREAD; i++) {
        if (t >= threads + MAX_THREAD)
            t = threads;
        if (t->state == RUNNABLE && t != current_thread) {
            next_thread = t;
            break;
        }
        t++;
    }

    if (next_thread == 0) {
        printf("thread_schedule: no runnable threads\n");
        exit(-1);
    }

    if (current_thread != next_thread) {
        struct thread *prev = current_thread;
        next_thread->state = RUNNING;
        current_thread = next_thread;

        // 上下文切换
        thread_switch(&prev->context, &next_thread->context);
    }
}
```

### 2.4 线程创建

```c
// user/uthread.c

void
thread_create(void (*func)())
{
    struct thread *t;

    // 寻找空闲线程槽
    for (t = threads; t < threads + MAX_THREAD; t++) {
        if (t->state == FREE) break;
    }

    if (t >= threads + MAX_THREAD) {
        printf("thread_create: no free slots\n");
        exit(-1);
    }

    // 初始化上下文
    memset(&t->context, 0, sizeof(t->context));

    // 设置栈指针 (栈从高地址向低地址增长)
    t->context.sp = (uint64)(t->stack + STACK_SIZE);

    // 设置返回地址为线程函数
    t->context.ra = (uint64)func;

    t->state = RUNNABLE;
}

void
thread_yield(void)
{
    current_thread->state = RUNNABLE;
    thread_schedule();
}
```

---

## 3. 使用线程解决实际问题

### 3.1 生产者-消费者问题

```c
// user/prodcon.c
#include "kernel/types.h"
#include "user/user.h"

#define N 10          // 缓冲区大小
#define NUM_ITEMS 100 // 生产/消费数量

int buffer[N];
int count = 0;
int in = 0, out = 0;

// 简单的锁 (使用原子操作)
int lock = 0;

void acquire(int *lk) {
    while (__sync_lock_test_and_set(lk, 1) != 0)
        ;
}

void release(int *lk) {
    __sync_lock_release(lk);
}

void producer() {
    for (int i = 0; i < NUM_ITEMS; i++) {
        acquire(&lock);

        // 等待有空位 (简化版，实际应使用条件变量)
        while (count == N) {
            release(&lock);
            thread_yield();
            acquire(&lock);
        }

        buffer[in] = i;
        in = (in + 1) % N;
        count++;

        release(&lock);
    }
}

void consumer() {
    for (int i = 0; i < NUM_ITEMS; i++) {
        acquire(&lock);

        // 等待有数据
        while (count == 0) {
            release(&lock);
            thread_yield();
            acquire(&lock);
        }

        int item = buffer[out];
        out = (out + 1) % N;
        count--;

        release(&lock);

        printf("consumed: %d\n", item);
    }
}

int main() {
    thread_create(producer);
    thread_create(consumer);

    // 主线程也参与调度
    while (count < NUM_ITEMS) {
        thread_yield();
    }

    exit(0);
}
```

### 3.2 并行计算 - 素数筛选

```c
// user/primes_thread.c
// 使用多线程的素数筛选

#include "kernel/types.h"
#include "user/user.h"

#define MAX_PRIMES 100

struct channel {
    int data[MAX_PRIMES];
    int head;
    int tail;
    int lock;
};

void channel_put(struct channel *c, int n) {
    while (__sync_lock_test_and_set(&c->lock, 1) != 0)
        ;
    c->data[c->tail++] = n;
    __sync_lock_release(&c->lock);
}

int channel_get(struct channel *c) {
    int n;
    while (1) {
        while (__sync_lock_test_and_set(&c->lock, 1) != 0)
            ;
        if (c->head < c->tail) {
            n = c->data[c->head++];
            __sync_lock_release(&c->lock);
            return n;
        }
        __sync_lock_release(&c->lock);
        thread_yield();
    }
}

void sieve(struct channel *in, int p) {
    printf("prime %d\n", p);

    struct channel out = { .head = 0, .tail = 0, .lock = 0 };
    int n;

    // 过滤倍数
    while ((n = channel_get(in)) != -1) {
        if (n % p != 0) {
            channel_put(&out, n);
        }
    }

    // 如果还有数，创建下一个筛子
    if (out.head < out.tail) {
        int next_p = channel_get(&out);
        channel_put(&out, -1);  // 结束标记

        // 创建新线程处理下一个素数
        // ... 需要线程传递参数的支持
    }
}
```

---

## 4. 线程barrier实现

### 4.1 Barrier概念

```
Barrier同步:

Thread 1    Thread 2    Thread 3
    │           │           │
    ▼           ▼           ▼
  计算1       计算2       计算3
    │           │           │
    └───────────┼───────────┘
                ▼
            Barrier等待
                │
    ┌───────────┼───────────┐
    ▼           ▼           ▼
  汇总1       汇总2       汇总3
    │           │           │
    ▼           ▼           ▼
  继续执行...

所有线程必须到达barrier后才能继续
```

### 4.2 Barrier实现

```c
// user/barrier.c

struct barrier {
    int count;          // 当前等待的线程数
    int max;            // 需要等待的线程总数
    int lock;           // 保护barrier的锁
    int passed;         // 当前轮次 (防止过早通过)
};

void barrier_init(struct barrier *b, int n) {
    b->count = 0;
    b->max = n;
    b->lock = 0;
    b->passed = 0;
}

void barrier_wait(struct barrier *b) {
    int passed = b->passed;

    // 增加计数
    while (__sync_lock_test_and_set(&b->lock, 1) != 0)
        ;
    b->count++;
    __sync_lock_release(&b->lock);

    if (b->count == b->max) {
        // 最后一个到达的线程
        b->count = 0;
        b->passed++;  // 增加轮次，唤醒其他线程
    } else {
        // 等待其他线程
        while (b->passed == passed) {
            thread_yield();
        }
    }
}
```

---

## 5. 线程安全与同步

### 5.1 竞态条件

```c
// 非线程安全的计数器
int counter = 0;

void unsafe_increment() {
    counter++;  // 这不是原子操作！
}

// 实际执行的指令:
// 1. ld  t0, counter   (加载)
// 2. addi t0, t0, 1    (加1)
// 3. sd  t0, counter   (存储)

// 两个线程并发执行:
// T1: load(0)    T2: load(0)
// T1: add(1)     T2: add(1)
// T1: store(1)   T2: store(1)
// 结果: counter = 1 (期望是2)
```

### 5.2 原子操作

```c
// 使用RISC-V原子指令

// 原子加
static inline int
atomic_add(int *ptr, int val)
{
    int result;
    __asm__ volatile(
        "amoadd.w %0, %2, (%1)"
        : "=r" (result)
        : "r" (ptr), "r" (val)
        : "memory"
    );
    return result;
}

// 测试并设置 (用于自旋锁)
static inline int
test_and_set(int *ptr, int val)
{
    int result;
    __asm__ volatile(
        "amoswap.w %0, %2, (%1)"
        : "=r" (result)
        : "r" (ptr), "r" (val)
        : "memory"
    );
    return result;
}
```

### 5.3 自旋锁实现

```c
// user/spinlock.h

typedef struct {
    int locked;
} spinlock_t;

void spinlock_init(spinlock_t *lk) {
    lk->locked = 0;
}

void spinlock_acquire(spinlock_t *lk) {
    while (__sync_lock_test_and_set(&lk->locked, 1) != 0)
        ;
    // 内存屏障，防止编译器重排序
    __sync_synchronize();
}

void spinlock_release(spinlock_t *lk) {
    __sync_synchronize();
    __sync_lock_release(&lk->locked);
}
```

---

## 6. 调试多线程程序

### 6.1 常见bug

```
Bug 1: 忘记初始化线程上下文
症状: 程序崩溃或跳转到随机地址
解决: 确保ra和sp正确设置

Bug 2: 栈溢出
症状: 数据损坏，随机崩溃
解决: 增加STACK_SIZE，检查递归深度

Bug 3: 竞态条件
症状: 结果不稳定，偶尔出错
解决: 使用适当的同步机制

Bug 4: 死锁
症状: 程序挂起
解决: 确保锁的获取顺序一致
```

### 6.2 调试技巧

```c
// 添加线程调试信息
#define DEBUG_THREAD 1

#if DEBUG_THREAD
#define thread_debug(fmt, ...) printf("[T%d] " fmt "\n", \
    current_thread - threads, ##__VA_ARGS__)
#else
#define thread_debug(fmt, ...)
#endif

// 使用示例
void thread_function() {
    thread_debug("starting");
    // ...
    thread_debug("yielding");
    thread_yield();
    thread_debug("resumed");
}
```

---

## 7. 性能评估

### 7.1 线程切换开销

```
测量方法:
1. 创建两个线程
2. 让它们来回切换N次
3. 测量总时间

预期结果:
- 用户级线程切换: ~10-100 ns
- 内核级线程切换: ~1-10 us
- 进程切换: ~10-100 us
```

### 7.2 加速比测试

```c
// 计算加速比
void speedup_test() {
    int start, end;

    // 单线程版本
    start = uptime();
    compute_single_thread();
    end = uptime();
    int single_time = end - start;

    // 多线程版本
    start = uptime();
    compute_multi_thread(NUM_THREADS);
    end = uptime();
    int multi_time = end - start;

    printf("Single thread: %d ticks\n", single_time);
    printf("Multi thread: %d ticks\n", multi_time);
    printf("Speedup: %.2fx\n", (float)single_time / multi_time);
}
```

---

## 8. 总结

### 8.1 核心概念

| 概念 | 说明 |
|------|------|
| **上下文** | 线程执行状态，包括寄存器和栈 |
| **上下文切换** | 保存当前线程状态，恢复另一线程状态 |
| **调度** | 决定哪个线程运行的策略 |
| **竞态条件** | 并发访问共享数据导致的不确定性 |
| **同步** | 协调多线程执行顺序的机制 |

### 8.2 RISC-V调用约定

```
Caller-saved寄存器: ra, t0-t6, a0-a7, (调用者保存)
Callee-saved寄存器: s0-s11, (被调用者保存)

线程切换需要保存:
- ra (返回地址)
- sp (栈指针)
- s0-s11 (callee-saved)
```

---

**难度**: ⭐⭐⭐
**预计时间**: 6-8小时
**依赖**: Lab 4 (陷入，理解上下文)
