# 原子操作与内存序

> **对应标准**: C11 `_Atomic`, C++11 `std::atomic`, Linux `atomic.h`
> **难度**: L5 | **更新**: 2026-03-15

---

## 概述

原子操作是多线程编程的基石，保证操作不可中断，避免竞态条件。

```
┌─────────────────────────────────────────────────────────────┐
│                    原子操作层次结构                           │
├─────────────────────────────────────────────────────────────┤
│  硬件层: x86 LOCK前缀, ARM LDREX/STREX, RISC-V AMO            │
├─────────────────────────────────────────────────────────────┤
│  内核层: Linux atomic_t, spinlock, RCU                        │
├─────────────────────────────────────────────────────────────┤
│  编译器层: __atomic_*, __sync_*, _Atomic                      │
├─────────────────────────────────────────────────────────────┤
│  标准库: C11 <stdatomic.h>, C++ <atomic>                      │
├─────────────────────────────────────────────────────────────┤
│  应用层: 无锁队列、计数器、标志位                              │
└─────────────────────────────────────────────────────────────┘
```

## C11原子操作API

```c
#include <stdatomic.h>

// 原子类型定义
_Atomic int counter;
atomic_int counter2;  // 等价写法

// 基本操作
atomic_init(&counter, 0);          // 初始化
atomic_store(&counter, 10);        // 原子写
int val = atomic_load(&counter);   // 原子读
int old = atomic_exchange(&counter, 20);  // 交换

// 读-修改-写
atomic_fetch_add(&counter, 1);     // 原子的counter++
atomic_fetch_sub(&counter, 1);     // 原子的counter--
atomic_fetch_and/or/xor(&counter, mask);  // 位操作

// 比较并交换(CAS)
atomic_compare_exchange_strong(&counter, &expected, desired);
atomic_compare_exchange_weak(&counter, &expected, desired);  // 可能伪失败
```

## 内存序(Memory Order)

| 内存序 | 说明 | 用途 |
|:-------|:-----|:-----|
| `memory_order_relaxed` | 无同步保证 | 单纯原子性，如计数器 |
| `memory_order_acquire` | 获取语义 | 读锁、消费者端 |
| `memory_order_release` | 释放语义 | 写锁、生产者端 |
| `memory_order_acq_rel` | 获取+释放 | 读-修改-写操作 |
| `memory_order_seq_cst` | 顺序一致 | 默认，最强约束 |

```
Acquire-Release 同步模式:

Thread A (Producer)          Thread B (Consumer)
─────────────────────        ─────────────────────
data = 42;                   if (atomic_load(&flag, acquire))
atomic_store(&flag, 1,         // 保证看到所有之前的写入
             release);           use(data);  // 一定能看到data=42
```

## 无锁数据结构示例

```c
// 无锁栈 (Treiber Stack)
typedef struct Node {
    void *data;
    _Atomic(struct Node *) next;
} Node;

typedef struct {
    _Atomic(Node *) head;
} LockFreeStack;

void push(LockFreeStack *stack, void *data) {
    Node *node = malloc(sizeof(Node));
    node->data = data;

    do {
        node->next = atomic_load(&stack->head);
    } while (!atomic_compare_exchange_weak(
        &stack->head, &node->next, node));
}

void *pop(LockFreeStack *stack) {
    Node *head;
    do {
        head = atomic_load(&stack->head);
        if (head == NULL) return NULL;
    } while (!atomic_compare_exchange_weak(
        &stack->head, &head, head->next));

    void *data = head->data;
    free(head);
    return data;
}
```

## 硬件实现

| 架构 | 原子指令 | 特点 |
|:-----|:---------|:-----|
| x86_64 | LOCK前缀, XCHG, CMPXCHG | 强内存模型 |
| ARM64 | LDXR/STXR, CAS | 弱内存模型 |
| RISC-V | LR/SC, AMO | 基础指令集 |

## ABA问题与解决方案

```
ABA问题:              解决方案:

Stack: A→B→C         带标签指针(Tagged Pointer)
                      ┌─────────────┐
Thread1准备pop A      │ ptr: 0x1000 │
  (被抢占)            │ tag: 1      │
                      └─────────────┘
Thread2: pop A, pop B,
         push A back
Thread1继续CAS:       Thread1看到tag已变，
  成功，但B已释放！     CAS失败，重新读取
```

## 常见陷阱

| 错误 | 后果 | 正确做法 |
|:-----|:-----|:---------|
| 数据竞争访问非原子变量 | UB | 所有共享数据用原子或锁保护 |
| Relaxed序用于同步 | 可见性问题 | 使用acquire-release配对 |
| 忘记初始化原子变量 | 未定义值 | 使用`atomic_init` |
| ABA问题 | 使用已释放内存 | 使用带标签指针或 hazard pointer |

---

> **状态**: ✅ 已完成
