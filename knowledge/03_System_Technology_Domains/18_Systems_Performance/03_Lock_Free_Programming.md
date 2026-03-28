# 无锁编程（Lock-Free Programming）深度指南

> **层级定位**: 03_System_Technology_Domains / 18_Systems_Performance
> **难度级别**: L5 专家级
> **对应标准**: C11 Atomics, C++11 Memory Model
> **最后更新**: 2026-03-28

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | 原子操作、内存序、无锁数据结构、ABA问题、 hazard指针 |
| **前置知识** | [内存模型](../../02_Formal_Semantics_and_Physics/01_Game_Semantics/02_C11_Memory_Model.md)、[并发编程](../../14_Concurrency_Parallelism/README.md)、[原子操作](../../01_Core_Knowledge_System/12_Advanced_Topics/02_Atomic_Operations_Deep.md) |
| **后续延伸** | [高性能日志](../../09_High_Performance_Log/)、[内存数据库](../../11_In_Memory_Database/) |
| **横向关联** | [缓存一致性](../../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/03_Cache_Coherency_Protocols.md)、[CPU微架构](../../02_Formal_Semantics_and_Physics/07_Microarchitecture/readme.md) |
| **权威来源** | C11标准7.17节、Herb Sutter's articles、D. Vyukov's algorithms |

---

## 📑 目录

- [无锁编程（Lock-Free Programming）深度指南](#无锁编程lock-free-programming深度指南)
  - [📋 本节概要](#-本节概要)
  - [📑 目录](#-目录)
  - [🎯 无锁编程概述](#-无锁编程概述)
    - [并发编程范式对比](#并发编程范式对比)
    - [无锁 vs 无等待](#无锁-vs-无等待)
  - [🔧 C11原子操作基础](#-c11原子操作基础)
    - [原子类型](#原子类型)
    - [内存序](#内存序)
  - [⚡ 无锁数据结构](#-无锁数据结构)
    - [无锁队列](#无锁队列)
    - [无锁栈](#无锁栈)
  - [🛡️ ABA问题与解决方案](#️-aba问题与解决方案)
    - [什么是ABA问题？](#什么是aba问题)
    - [解决方案1：Hazard Pointers](#解决方案1hazard-pointers)
    - [解决方案2：Tagged Pointers](#解决方案2tagged-pointers)
  - [🔬 深入理解](#-深入理解)
    - [缓存一致性协议](#缓存一致性协议)
    - [内存屏障实现](#内存屏障实现)
  - [✅ 质量验收清单](#-质量验收清单)
  - [📚 相关资源](#-相关资源)
    - [官方文档](#官方文档)
    - [推荐资源](#推荐资源)
    - [书籍](#书籍)

---

## 🎯 无锁编程概述

### 并发编程范式对比

```
并发控制策略演进：

阻塞式同步（Blocking）
    │
    ├── 互斥锁（Mutex）        ← 最简单，但上下文切换开销大
    ├── 读写锁（RWLock）       ← 读多写少场景
    └── 信号量（Semaphore）    ← 控制并发数量
    │
    缺点：线程阻塞，调度开销，优先级反转
    │
    ▼
非阻塞式同步（Non-blocking）
    │
    ├── 乐观锁（Optimistic Locking）
    │   └── 读取-修改-比较-交换（CAS）
    │
    └── 无锁算法（Lock-Free）
        │
        ├── Lock-Free      ← 保证系统整体进度（至少一个线程前进）
        ├── Wait-Free      ← 每个线程都能在规定步数内完成
        └── Obstruction-Free ← 无竞争时保证进度

性能对比：
阻塞式：适合低竞争、长时间持有
无锁：适合高竞争、短时间操作、实时性要求
```

### 无锁 vs 无等待

| 特性 | Lock-Free | Wait-Free | Obstruction-Free |
|:-----|:----------|:----------|:-----------------|
| **进度保证** | 至少一个线程前进 | 所有线程都前进 | 无竞争时前进 |
| **实现复杂度** | 中等 | 极高 | 较低 |
| **典型应用** | 队列、栈 | 计数器、简单数据结构 | 较少使用 |
| **ABA安全** | 需要处理 | 需要处理 | 需要处理 |
| **starvation-free** | 否 | 是 | 否 |

---

## 🔧 C11原子操作基础

### 原子类型

```c
#include <stdatomic.h>
#include <stdint.h>

// C11标准原子类型
atomic_int      counter;    // 等同于 atomic_int
tomic_uint      ucounter;
atomic_long     lcounter;
atomic_ulong    ulcounter;
atomic_intptr_t ptr_counter;
atomic_size_t   size_counter;

// 自定义原子类型（通过_Atomic关键字）
_Atomic struct Node* atomic_head;

// 原子操作API
void atomic_example() {
    atomic_int val = ATOMIC_VAR_INIT(0);

    // 原子加载/存储
    int v = atomic_load(&val);
    atomic_store(&val, 42);

    // 原子加/减
    atomic_fetch_add(&val, 1);   // 返回旧值
    atomic_fetch_sub(&val, 1);

    // 原子比较交换（CAS）- 无锁编程核心
    int expected = 0;
    bool success = atomic_compare_exchange_strong(&val, &expected, 1);
    // 如果val==expected，则val=1，返回true
    // 否则expected=val，返回false

    // 弱CAS（允许spurious failure，在循环中更高效）
    do {
        expected = atomic_load(&val);
    } while (!atomic_compare_exchange_weak(&val, &expected, expected + 1));
}
```

### 内存序

```c
#include <stdatomic.h>

// 内存序（Memory Order）控制操作可见性

void memory_order_example() {
    atomic_int flag = ATOMIC_VAR_INIT(0);
    int data = 0;

    // 线程1
    data = 42;
    atomic_store_explicit(&flag, 1, memory_order_release);
    // release保证：在此之前的写操作不会重排到后面

    // 线程2
    while (atomic_load_explicit(&flag, memory_order_acquire) != 1) {
        // 自旋等待
    }
    // acquire保证：在此之后的读操作不会重排到前面
    // 现在可以安全读取data
    assert(data == 42);
}
```

**内存序类型详解**：

| 内存序 | 说明 | 使用场景 |
|:-------|:-----|:---------|
| `memory_order_relaxed` | 无同步保证 | 计数器、不需要顺序保证 |
| `memory_order_consume` | 依赖消费（很少使用） | 与acquire类似但范围更小 |
| `memory_order_acquire` | 获取语义 | 读取锁、检查标志 |
| `memory_order_release` | 释放语义 | 写入锁、设置标志 |
| `memory_order_acq_rel` | 获取+释放 | 读-修改-写操作 |
| `memory_order_seq_cst` | 顺序一致性（默认） | 最安全但最慢 |

---

## ⚡ 无锁数据结构

### 无锁队列

```c
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

// Michael-Scott无锁队列（经典算法）
typedef struct Node {
    void* data;
    _Atomic(struct Node*) next;
} Node;

typedef struct {
    _Atomic(Node*) head;
    _Atomic(Node*) tail;
    // 哨兵节点简化边界条件处理
} LockFreeQueue;

void queue_init(LockFreeQueue* q) {
    Node* sentinel = (Node*)malloc(sizeof(Node));
    sentinel->data = NULL;
    atomic_store(&sentinel->next, NULL);

    atomic_store(&q->head, sentinel);
    atomic_store(&q->tail, sentinel);
}

void queue_enqueue(LockFreeQueue* q, void* data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = data;
    atomic_store(&new_node->next, NULL);

    Node* tail;
    Node* next;

    while (true) {
        tail = atomic_load(&q->tail);
        next = atomic_load(&tail->next);

        // 检查tail是否仍然是tail
        if (tail == atomic_load(&q->tail)) {
            if (next == NULL) {
                // 尝试链接新节点
                if (atomic_compare_exchange_strong(&tail->next, &next, new_node)) {
                    // 成功，尝试更新tail
                    atomic_compare_exchange_strong(&q->tail, &tail, new_node);
                    return;
                }
            } else {
                // tail落后，帮助更新
                atomic_compare_exchange_strong(&q->tail, &tail, next);
            }
        }
    }
}

bool queue_dequeue(LockFreeQueue* q, void** data) {
    Node* head;
    Node* tail;
    Node* next;

    while (true) {
        head = atomic_load(&q->head);
        tail = atomic_load(&q->tail);
        next = atomic_load(&head->next);

        if (head == atomic_load(&q->head)) {
            if (head == tail) {
                // 队列为空或tail落后
                if (next == NULL) {
                    return false;  // 空队列
                }
                // 帮助更新tail
                atomic_compare_exchange_strong(&q->tail, &tail, next);
            } else {
                // 读取数据并尝试更新head
                *data = next->data;
                if (atomic_compare_exchange_strong(&q->head, &head, next)) {
                    // 成功出队，释放旧head
                    free(head);
                    return true;
                }
            }
        }
    }
}
```

### 无锁栈

```c
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

// Treiber栈（最简单的无锁数据结构）
typedef struct StackNode {
    void* data;
    struct StackNode* next;
} StackNode;

typedef struct {
    _Atomic(StackNode*) top;
} LockFreeStack;

void stack_init(LockFreeStack* s) {
    atomic_store(&s->top, NULL);
}

void stack_push(LockFreeStack* s, void* data) {
    StackNode* new_node = (StackNode*)malloc(sizeof(StackNode));
    new_node->data = data;

    StackNode* old_top;
    do {
        old_top = atomic_load(&s->top);
        new_node->next = old_top;
    } while (!atomic_compare_exchange_weak(&s->top, &old_top, new_node));
}

bool stack_pop(LockFreeStack* s, void** data) {
    StackNode* old_top;

    do {
        old_top = atomic_load(&s->top);
        if (old_top == NULL) {
            return false;  // 空栈
        }
    } while (!atomic_compare_exchange_weak(&s->top, &old_top, old_top->next));

    *data = old_top->data;
    free(old_top);
    return true;
}
```

---

## 🛡️ ABA问题与解决方案

### 什么是ABA问题？

```
ABA问题场景：

时间线：
T1: 线程A读取ptr = X
    [X] -> [Y] -> [Z]
     ↑
    head

T2: 线程B弹出X，释放X，压入新X'（地址相同）
    [X'] -> [Y] -> [Z]
     ↑
    head

T3: 线程A的CAS操作：比较head==X（地址相同），成功！
    但X已经被释放并重新分配，数据可能不同

后果：
- 使用已释放的内存
- 数据不一致
- 程序崩溃
```

### 解决方案1：Hazard Pointers

```c
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_HAZARD_POINTERS 4
#define MAX_THREADS 64

// Hazard Pointer结构
typedef struct HazardPointer {
    _Atomic(void*) pointer;
    pthread_t thread_id;
} HazardPointer;

HazardPointer g_hazard_pointers[MAX_THREADS * MAX_HAZARD_POINTERS];

// 线程局部存储当前使用的hazard pointer索引
_Thread_local int g_hp_index = -1;

void hp_init() {
    for (int i = 0; i < MAX_THREADS * MAX_HAZARD_POINTERS; i++) {
        atomic_store(&g_hazard_pointers[i].pointer, NULL);
        g_hazard_pointers[i].thread_id = 0;
    }
}

int hp_acquire() {
    pthread_t self = pthread_self();
    for (int i = 0; i < MAX_THREADS * MAX_HAZARD_POINTERS; i++) {
        pthread_t expected = 0;
        if (atomic_compare_exchange_strong(&g_hazard_pointers[i].thread_id,
                                            &expected, self)) {
            g_hp_index = i;
            return i;
        }
    }
    return -1;  // 没有可用slot
}

void hp_release(int index) {
    atomic_store(&g_hazard_pointers[index].pointer, NULL);
}

void hp_set(void* ptr) {
    if (g_hp_index >= 0) {
        atomic_store(&g_hazard_pointers[g_hp_index].pointer, ptr);
    }
}

// 安全释放：检查是否被其他线程的hazard pointer引用
void hp_safe_free(void* ptr) {
    // 检查所有hazard pointers
    for (int i = 0; i < MAX_THREADS * MAX_HAZARD_POINTERS; i++) {
        if (atomic_load(&g_hazard_pointers[i].pointer) == ptr) {
            // 还在使用，加入延迟释放列表
            // ... 实现延迟释放逻辑
            return;
        }
    }
    // 安全释放
    free(ptr);
}
```

### 解决方案2：Tagged Pointers

```c
#include <stdint.h>
#include <stdatomic.h>

// 使用指针低位存储计数器（假设16字节对齐）
#define TAG_BITS 4
#define PTR_MASK (~((1ULL << TAG_BITS) - 1))
#define TAG_MASK ((1ULL << TAG_BITS) - 1)

typedef uintptr_t TaggedPtr;

typedef struct {
    void* ptr;
    uintptr_t tag;
} TaggedPointer;

TaggedPtr make_tagged(void* ptr, uintptr_t tag) {
    return ((uintptr_t)ptr & PTR_MASK) | (tag & TAG_MASK);
}

TaggedPointer unpack(TaggedPtr tp) {
    return (TaggedPointer){
        .ptr = (void*)(tp & PTR_MASK),
        .tag = tp & TAG_MASK
    };
}

// 使用示例
_Atomic(TaggedPtr) atomic_ptr;

void tagged_cas_example() {
    TaggedPtr old = atomic_load(&atomic_ptr);
    TaggedPointer unpacked = unpack(old);

    void* ptr = unpacked.ptr;
    uintptr_t tag = unpacked.tag;

    // 即使ptr地址相同，tag也会不同
    TaggedPtr new_val = make_tagged(ptr, tag + 1);

    atomic_compare_exchange_strong(&atomic_ptr, &old, new_val);
}
```

---

## 🔬 深入理解

### 缓存一致性协议

```
MESI协议状态转换：

E (Exclusive)    ← 独占，可写
    ↓ 其他CPU读取
S (Shared)       ← 共享，只读
    ↓ 本CPU写入
M (Modified)     ← 已修改，与内存不一致
    ↓ 写回内存
I (Invalid)      ← 无效

无锁操作对缓存的影响：
- 每次CAS操作都会触发缓存一致性流量
- 高竞争时成为性能瓶颈
- 解决：退避策略（exponential backoff）
```

### 内存屏障实现

```c
// 编译器屏障
#define compiler_barrier() asm volatile("" ::: "memory")

// CPU内存屏障（x86）
#define mfence() asm volatile("mfence" ::: "memory")
#define sfence() asm volatile("sfence" ::: "memory")
#define lfence() asm volatile("lfence" ::: "memory")

// 使用场景
void barrier_example() {
    volatile int flag = 0;
    int data = 0;

    // 生产者
    data = 42;
    sfence();  // 确保data写入在flag之前可见
    flag = 1;

    // 消费者
    while (!flag) {
        // 自旋
    }
    lfence();  // 确保flag读取在data之前完成
    assert(data == 42);
}
```

---

## ✅ 质量验收清单

- [x] 无锁编程概述与范式对比
- [x] C11原子操作基础
- [x] 内存序详解
- [x] 无锁队列（Michael-Scott算法）
- [x] 无锁栈（Treiber栈）
- [x] ABA问题详解
- [x] Hazard Pointers实现
- [x] Tagged Pointers方案
- [x] 深入理解（缓存一致性、内存屏障）

---

## 📚 相关资源

### 官方文档

- **C11标准 7.17节** - Atomics
- **Intel 64 Architecture Memory Ordering** - 内存序详细说明

### 推荐资源

- **Herb Sutter's articles** - "Lock-Free Code: A False Sense of Security"
- **Dmitry Vyukov's algorithms** - 高性能并发算法
- **1024cores.net** - 并发编程技术详解

### 书籍

- 《C++ Concurrency in Action》 - Anthony Williams
- 《The Art of Multiprocessor Programming》 - Maurice Herlihy

---

> **最后更新**: 2026-03-28
> **版本**: 1.0 - 完整版
> **维护者**: C_Lang Knowledge Base Team
