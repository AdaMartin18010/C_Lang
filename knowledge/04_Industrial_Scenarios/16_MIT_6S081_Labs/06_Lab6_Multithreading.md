# MIT 6.S081 Lab 6: Multithreading

## 1. 实验概述

### 1.1 目标

- 理解用户级线程实现
- 掌握上下文切换机制
- 实现线程创建和调度
- 使用线程加速程序

### 1.2 用户级线程 vs 内核级线程

```
用户级线程 (本实验):
├── 线程管理在用户空间
├── 切换不需要陷入内核
├── 阻塞系统调用会阻塞所有线程
└── 实现简单，效率高

内核级线程:
├── 线程管理在内核空间
├── 切换需要陷入内核
├── 阻塞不影响其他线程
└── 实现复杂，更灵活
```

---

## 2. 线程数据结构

### 2.1 线程上下文

```c
// 保存线程执行状态
struct context {
    uint64 ra;    // 返回地址
    uint64 sp;    // 栈指针

    // 被调用者保存的寄存器
    uint64 s0;    // 帧指针
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
```

### 2.2 线程控制块

```c
// user/uthread.c

#define MAX_THREAD  4
#define STACK_SIZE  4096

// 线程状态
enum thread_state { FREE, RUNNING, RUNNABLE };

struct thread {
    char       stack[STACK_SIZE];  // 线程栈
    struct context context;         // 保存的上下文
    enum thread_state state;        // 线程状态
    int        id;                  // 线程ID
};

struct thread all_thread[MAX_THREAD];
struct thread *current_thread;
int thread_count = 0;
```

---

## 3. 上下文切换

### 3.1 汇编实现

```asm
# user/uthread_switch.S
# void thread_switch(struct context *old, struct context *new);

.globl thread_switch
thread_switch:
    # 保存旧线程的寄存器
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

    # 恢复新线程的寄存器
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

    # 返回到新线程
    ret
```

### 3.2 C包装函数

```c
// user/uthread.c

extern void thread_switch(struct context *old, struct context *new);

void thread_yield(void) {
    struct thread *next;

    // 找到下一个可运行的线程
    next = current_thread + 1;
    if (next >= all_thread + MAX_THREAD)
        next = all_thread;

    while (next->state != RUNNABLE && next != current_thread)
        next = (next + 1 >= all_thread + MAX_THREAD) ?
               all_thread : next + 1;

    if (next == current_thread)
        return;  // 没有其他可运行线程

    // 切换
    struct thread *prev = current_thread;
    next->state = RUNNING;
    current_thread = next;

    thread_switch(&prev->context, &next->context);
}
```

---

## 4. 线程管理函数

### 4.1 线程创建

```c
// user/uthread.c

void thread_create(void (*func)()) {
    struct thread *t;

    // 找到空闲线程槽
    for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
        if (t->state == FREE)
            break;
    }

    if (t >= all_thread + MAX_THREAD) {
        printf("thread_create: no free slots\n");
        exit(-1);
    }

    // 初始化线程
    t->state = RUNNABLE;
    t->id = thread_count++;

    // 设置栈顶
    t->context.sp = (uint64)(t->stack + STACK_SIZE);

    // 设置返回地址为线程函数
    t->context.ra = (uint64)func;
}

void thread_init(void) {
    // 初始化主线程
    current_thread = &all_thread[0];
    current_thread->state = RUNNING;
    current_thread->id = 0;
    thread_count = 1;
}
```

### 4.2 线程调度

```c
// user/uthread.c

void thread_schedule(void) {
    struct thread *t;
    int found = 0;

    // 寻找可运行线程
    for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
        if (t->state == RUNNABLE) {
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("thread_schedule: no runnable threads\n");
        exit(-1);
    }

    // 切换到选中的线程
    if (t != current_thread) {
        struct thread *prev = current_thread;
        next->state = RUNNING;
        current_thread = t;

        thread_switch(&prev->context, &t->context);
    }
}
```

---

## 5. 使用示例

### 5.1 简单多线程程序

```c
// user/uthreadtest.c
#include "kernel/types.h"
#include "user/user.h"

int shared_counter = 0;

void thread_func(void) {
    for (int i = 0; i < 100; i++) {
        printf("Thread %d: counter = %d\n",
               current_thread->id, shared_counter++);
        thread_yield();  // 主动让出CPU
    }

    current_thread->state = FREE;
    thread_schedule();  // 切换到其他线程
}

int main() {
    // 初始化线程系统
    thread_init();

    // 创建3个工作线程
    thread_create(thread_func);
    thread_create(thread_func);
    thread_create(thread_func);

    // 主线程也参与工作
    thread_func();

    printf("All threads finished\n");
    exit(0);
}
```

### 5.2 并行计算示例

```c
// 使用多线程并行计算数组和
#define N 1000
#define NTHREAD 4

int array[N];
int partial_sums[NTHREAD];

void worker(void) {
    int tid = current_thread->id;
    int start = tid * (N / NTHREAD);
    int end = start + (N / NTHREAD);

    int sum = 0;
    for (int i = start; i < end; i++) {
        sum += array[i];
    }

    partial_sums[tid] = sum;
    current_thread->state = FREE;
    thread_schedule();
}

int main() {
    // 初始化数组
    for (int i = 0; i < N; i++)
        array[i] = i;

    thread_init();

    // 创建工作线程
    for (int i = 0; i < NTHREAD - 1; i++)
        thread_create(worker);

    // 主线程也工作
    worker();

    // 汇总结果
    int total = 0;
    for (int i = 0; i < NTHREAD; i++)
        total += partial_sums[i];

    printf("Total sum: %d\n", total);
    exit(0);
}
```

---

## 6. 使用多线程加速程序

### 6.1 ph程序 (并行哈希表)

```c
// user/ph.c
// 并行哈希表测试

#define NBUCKET 5
#define NKEYS 10000
#define NTHREADS 4

struct entry {
    int key;
    int value;
    struct entry *next;
};

struct entry *table[NBUCKET];
int keys[NKEYS];
int nthread;

void put(int key, int value) {
    int i = key % NBUCKET;

    struct entry *e = malloc(sizeof(struct entry));
    e->key = key;
    e->value = value;
    e->next = table[i];
    table[i] = e;
}

struct entry* get(int key) {
    int i = key % NBUCKET;

    struct entry *e = table[i];
    while (e) {
        if (e->key == key)
            return e;
        e = e->next;
    }
    return 0;
}

void thread_put(void) {
    int tid = current_thread->id;
    int start = tid * (NKEYS / nthread);
    int end = start + (NKEYS / nthread);

    for (int i = start; i < end; i++) {
        put(keys[i], i);
    }

    current_thread->state = FREE;
    thread_schedule();
}

int main(int argc, char *argv[]) {
    // ... 初始化和启动线程 ...

    // 对比单线程和多线程性能
    // 注意：本实验的线程库没有锁，存在竞态条件

    return 0;
}
```

---

## 7. 线程切换流程

```
线程A运行中
    |
    | 调用thread_yield()
    v
保存线程A上下文 (ra, sp, s0-s11)
    |
    v
选择下一个可运行线程B
    |
    v
恢复线程B上下文
    |
    v
从线程B上次保存的位置继续执行
    |
    v
线程B运行
```

---

## 8. 调试技巧

### 8.1 常见问题

```
问题1: 线程不切换
原因: 忘记调用thread_yield()或thread_schedule()
解决: 在适当位置添加yield

问题2: 栈溢出
原因: 递归太深或局部变量太大
解决: 增加STACK_SIZE或优化代码

问题3: 竞态条件
原因: 无锁访问共享数据
解决: 本实验不处理，实际系统需要锁
```

### 8.2 调试输出

```c
void debug_thread(struct thread *t) {
    printf("Thread %d:\n", t->id);
    printf("  state: %d\n", t->state);
    printf("  sp: %p\n", t->context.sp);
    printf("  ra: %p\n", t->context.ra);
}
```

---

**难度**: ⭐⭐⭐
**预计时间**: 5-7小时
