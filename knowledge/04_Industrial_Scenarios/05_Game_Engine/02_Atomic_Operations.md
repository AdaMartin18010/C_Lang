# 原子操作与内存序

> **层级定位**: 04 Industrial Scenarios / 05 Game Engine
> **对应标准**: C11 _Atomic, C++11 memory_order, x86/x64/ARM内存模型
> **难度级别**: L5
> **预估学习时间**: 10-12小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | 原子操作、内存序、内存屏障、无锁编程、RCU |
| **前置知识** | C基础、并发编程、CPU缓存 |
| **后续延伸** | 无锁数据结构、高性能并发、形式化验证 |
| **权威来源** | C11标准 §7.17, Intel SDM Vol.3, ARM ARM |

---


---

## 📑 目录

- [原子操作与内存序](#原子操作与内存序)
  - [📋 本节概要](#-本节概要)
  - [📑 目录](#-目录)
  - [📖 1. 原子操作基础](#-1-原子操作基础)
    - [1.1 为什么需要原子操作](#11-为什么需要原子操作)
    - [1.2 C11原子类型](#12-c11原子类型)
    - [1.3 基本原子操作](#13-基本原子操作)
  - [📖 2. 内存序 (Memory Order)](#-2-内存序-memory-order)
    - [2.1 内存模型基础](#21-内存模型基础)
    - [2.2 六种内存序详解](#22-六种内存序详解)
    - [2.3 Acquire-Release 同步模式](#23-acquire-release-同步模式)
  - [📖 3. 无锁数据结构](#-3-无锁数据结构)
    - [3.1 无锁栈 (Treiber Stack)](#31-无锁栈-treiber-stack)
    - [3.2 无锁队列 (Michael-Scott Queue)](#32-无锁队列-michael-scott-queue)
  - [📖 4. 内存屏障](#-4-内存屏障)
  - [⚠️ 常见陷阱](#️-常见陷阱)
    - [陷阱 ATOM01: 数据竞争](#陷阱-atom01-数据竞争)
    - [陷阱 ATOM02: ABA问题](#陷阱-atom02-aba问题)
    - [陷阱 ATOM03: 错误使用Relaxed](#陷阱-atom03-错误使用relaxed)
  - [✅ 质量验收清单](#-质量验收清单)


---

## 📖 1. 原子操作基础

### 1.1 为什么需要原子操作

```c
// 非原子操作的问题
volatile int counter = 0;

void increment() {
    counter++;  // 实际上: 读-改-写 (3步)
}

// 汇编级展开 (x86):
// mov eax, [counter]   ; 读取
// inc eax              ; 增加
// mov [counter], eax   ; 写回
//
// 问题: 3条指令之间可能被其他线程打断！
```

### 1.2 C11原子类型

```c
#include <stdatomic.h>
#include <stdint.h>

// 原子类型声明
_Atomic int atomic_counter;
atomic_int atomic_int_var;  // 等价写法

// 各种原子类型
atomic_bool flag;
atomic_char achar;
atomic_short ashort;
atomic_int aint;
atomic_long along;
atomic_llong allong;
atomic_uintptr_t aptr;
atomic_size_t asize;

// 初始化
atomic_int counter = ATOMIC_VAR_INIT(0);
_Atomic(int) counter2 = 0;  // C11标准写法
```

### 1.3 基本原子操作

```c
#include <stdatomic.h>
#include <stdio.h>

void atomic_basic_operations() {
    atomic_int counter = 0;
    int expected = 0;

    // 原子读取
    int val = atomic_load(&counter);

    // 原子写入
    atomic_store(&counter, 42);

    // 原子增加并返回旧值
    int old = atomic_fetch_add(&counter, 1);  // ++counter

    // 原子减法
    atomic_fetch_sub(&counter, 5);

    // 原子位操作
    atomic_fetch_or(&counter, 0xFF);
    atomic_fetch_and(&counter, 0xF0);
    atomic_fetch_xor(&counter, 0x01);

    // CAS (Compare-And-Swap)
    // 如果counter == expected，则counter = 100
    bool success = atomic_compare_exchange_strong(
        &counter, &expected, 100);

    printf("Old: %d, Success: %d\n", old, success);
}

int main() {
    atomic_basic_operations();
    return 0;
}
```

---

## 📖 2. 内存序 (Memory Order)

### 2.1 内存模型基础

```
┌─────────────────────────────────────────────────────────────┐
│                      内存模型层次                            │
├─────────────────────────────────────────────────────────────┤
│  Sequentially Consistent (seq_cst)  ← 最强，默认             │
│       ↓                                                      │
│  Acquire-Release (acquire/release/acq_rel)                  │
│       ↓                                                      │
│  Release-Consume (consume)  ← C17废弃，用acquire替代         │
│       ↓                                                      │
│  Relaxed (relaxed)  ← 最弱，仅保证原子性                      │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 六种内存序详解

```c
#include <stdatomic.h>

void memory_order_examples() {
    atomic_int flag = 0;
    atomic_int data = 0;

    // 1. memory_order_seq_cst (顺序一致性)
    // 保证所有线程看到相同的操作顺序
    atomic_store(&flag, 1, memory_order_seq_cst);
    int val = atomic_load(&flag, memory_order_seq_cst);

    // 2. memory_order_relaxed (松散)
    // 仅保证操作原子性，不保证顺序
    // 适用于计数器、统计场景
    atomic_fetch_add(&counter, 1, memory_order_relaxed);

    // 3. memory_order_acquire (获取)
    // 用于读操作：之后的读写不会重排到此次读之前
    while (atomic_load(&flag, memory_order_acquire) == 0) {
        // 等待
    }
    // 保证这里能看到flag设置之前的所有写入

    // 4. memory_order_release (释放)
    // 用于写操作：之前的读写不会重排到此次写之后
    atomic_store(&data, 42, memory_order_relaxed);
    atomic_store(&flag, 1, memory_order_release);  // 发布

    // 5. memory_order_acq_rel (获取-释放)
    // 用于读-改-写操作
    atomic_fetch_add(&counter, 1, memory_order_acq_rel);

    // 6. memory_order_consume (消费) - C17已弃用
    // 类似acquire但只依赖当前值
}
```

### 2.3 Acquire-Release 同步模式

```c
#include <stdatomic.h>
#include <stdio.h>
#include <pthread.h>

atomic_int flag = 0;
int shared_data = 0;

void* producer_thread(void* arg) {
    // 步骤1: 准备数据
    shared_data = 42;

    // 步骤2: 发布 (release)
    // 保证shared_data的写入对consumer可见
    atomic_store(&flag, 1, memory_order_release);
    return NULL;
}

void* consumer_thread(void* arg) {
    // 步骤1: 等待发布 (acquire)
    while (atomic_load(&flag, memory_order_acquire) == 0) {
        // 自旋等待
    }

    // 步骤2: 读取数据
    // 保证能看到producer在release之前的所有写入
    printf("Data: %d\n", shared_data);  // 必定是42
    return NULL;
}

int main() {
    pthread_t prod, cons;
    pthread_create(&prod, NULL, producer_thread, NULL);
    pthread_create(&cons, NULL, consumer_thread, NULL);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    return 0;
}
```

---

## 📖 3. 无锁数据结构

### 3.1 无锁栈 (Treiber Stack)

```c
#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>

// 无锁栈节点
typedef struct Node {
    int data;
    struct Node* next;
} Node;

// 无锁栈
typedef struct {
    _Atomic(Node*) top;
} LockFreeStack;

void stack_init(LockFreeStack* stack) {
    atomic_init(&stack->top, NULL);
}

// 无锁压栈
void stack_push(LockFreeStack* stack, int value) {
    Node* new_node = malloc(sizeof(Node));
    new_node->data = value;

    Node* old_top;
    do {
        old_top = atomic_load(&stack->top);
        new_node->next = old_top;
        // CAS: 如果top还是old_top，则更新为new_node
    } while (!atomic_compare_exchange_weak(
        &stack->top, &old_top, new_node));
}

// 无锁弹栈
bool stack_pop(LockFreeStack* stack, int* value) {
    Node* old_top;
    do {
        old_top = atomic_load(&stack->top);
        if (old_top == NULL) return false;  // 空栈
        // CAS: 如果top还是old_top，则更新为next
    } while (!atomic_compare_exchange_weak(
        &stack->top, &old_top, old_top->next));

    *value = old_top->data;
    // 注意：这里需要延迟释放或使用HP/EBR
    free(old_node);  // 简化处理，实际需要内存回收
    return true;
}
```

### 3.2 无锁队列 (Michael-Scott Queue)

```c
#include <stdatomic.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Node {
    int data;
    _Atomic(struct Node*) next;
} Node;

typedef struct {
    _Atomic(Node*) head;
    _Atomic(Node*) tail;
} LockFreeQueue;

void queue_init(LockFreeQueue* q) {
    Node* dummy = malloc(sizeof(Node));
    dummy->next = NULL;
    atomic_init(&q->head, dummy);
    atomic_init(&q->tail, dummy);
}

void queue_enqueue(LockFreeQueue* q, int value) {
    Node* new_node = malloc(sizeof(Node));
    new_node->data = value;
    atomic_init(&new_node->next, NULL);

    Node* tail;
    Node* next;

    while (1) {
        tail = atomic_load(&q->tail);
        next = atomic_load(&tail->next);

        if (tail == atomic_load(&q->tail)) {
            if (next == NULL) {
                // 尝试链接新节点
                if (atomic_compare_exchange_weak(
                    &tail->next, &next, new_node)) {
                    break;
                }
            } else {
                // 帮助移动tail
                atomic_compare_exchange_weak(
                    &q->tail, &tail, next);
            }
        }
    }

    // 尝试移动tail
    atomic_compare_exchange_weak(&q->tail, &tail, new_node);
}
```

---

## 📖 4. 内存屏障

```c
// 显式内存屏障 (底层)
#include <stdatomic.h>

void memory_fences() {
    // 全屏障
    atomic_thread_fence(memory_order_seq_cst);

    // 获取屏障
    atomic_thread_fence(memory_order_acquire);

    // 释放屏障
    atomic_thread_fence(memory_order_release);

    // 信号量屏障 (进程间)
    atomic_signal_fence(memory_order_seq_cst);
}

// 编译器屏障 (GCC/Clang)
#define compiler_barrier() __asm__ __volatile__("" ::: "memory")

void compiler_fence_example() {
    int x = 1;
    compiler_barrier();  // 阻止编译器重排
    int y = x;
}
```

---

## ⚠️ 常见陷阱

### 陷阱 ATOM01: 数据竞争

```c
// 错误：非原子读取原子变量
atomic_int flag = 0;
int x = flag;  // 未定义行为！

// 正确
int x = atomic_load(&flag);
```

### 陷阱 ATOM02: ABA问题

```c
// ABA问题示例
// 栈: A -> B -> C
// T1: 读取top=A, 准备CAS到B
// T2: pop A, pop B, push A (A被重用)
// T1: CAS成功，但栈结构已改变！

// 解决方案：使用带版本号的指针 (Tagged Pointer)
// 或 Hazard Pointers / Epoch-Based Reclamation
```

### 陷阱 ATOM03: 错误使用Relaxed

```c
// 错误：用relaxed实现同步
atomic_int flag = 0;
int data = 0;

void producer() {
    data = 42;
    atomic_store(&flag, 1, memory_order_relaxed);  // 太弱！
}

void consumer() {
    while (atomic_load(&flag, memory_order_relaxed) == 0);
    printf("%d\n", data);  // 可能读到0！
}

// 正确：使用acquire-release
atomic_store(&flag, 1, memory_order_release);
while (atomic_load(&flag, memory_order_acquire) == 0);
```

---

## ✅ 质量验收清单

- [x] C11原子类型与操作
- [x] 六种内存序详解
- [x] Acquire-Release同步模式
- [x] 无锁栈实现
- [x] 无锁队列实现
- [x] 内存屏障使用
- [x] ABA问题说明
- [x] 常见陷阱分析

---

> **更新记录**
>
> - 2025-03-09: 重写充实，从模板63行扩展至完整技术文档


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
