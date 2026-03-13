# 并发安全层次依赖图

> **文档定位**: 并发编程概念的层次结构
> **表征方式**: 层次图、依赖关系、演进路径

---

## 一、并发安全层次金字塔

```text
                    ┌─────────────┐
                    │   Level 5   │
                    │  无锁算法    │
                    │ (Lock-free) │
                    └──────┬──────┘
                           │ 依赖
                    ┌──────▼──────┐
                    │   Level 4   │
                    │  原子操作   │
                    │  (_Atomic)  │
                    └──────┬──────┘
                           │ 依赖
              ┌────────────┼────────────┐
              │            │            │
       ┌──────▼──────┐ ┌───▼────┐ ┌────▼─────┐
       │   Level 3   │ │ Level 3│ │ Level 3  │
       │   读写锁     │ │ 互斥锁 │ │ 信号量    │
       │  (RWLock)   │ │ (Mutex)│ │(Semaphore)│
       └──────┬──────┘ └───┬────┘ └────┬─────┘
              │            │           │
              └────────────┼───────────┘
                           │ 依赖
                    ┌──────▼──────┐
                    │   Level 2   │
                    │  内存屏障    │
                    │ (Barrier)   │
                    └──────┬──────┘
                           │ 依赖
                    ┌──────▼──────┐
                    │   Level 1   │
                    │  硬件支持    │
                    │ (原子指令)   │
                    └─────────────┘
```

---

## 二、并发原语 × 适用场景 选择矩阵

| 原语 | 读多写少 | 写多读少 | 读写均衡 | 临界区短 | 临界区长 | 需要条件变量 |
|:-----|:--------:|:--------:|:--------:|:--------:|:--------:|:------------:|
| **互斥锁** | ✅ | ✅ | ✅ | ✅ | ✅ | 配合cond |
| **读写锁** | ⭐⭐⭐ | ❌ | ⚠️ | ✅ | ✅ | 配合cond |
| **自旋锁** | ❌ | ❌ | ❌ | ⭐⭐⭐ | ❌ | ❌ |
| **信号量** | ✅ | ✅ | ✅ | ✅ | ✅ | 内置 |
| **原子变量** | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | ❌ | 忙等 |
| **无锁队列** | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | 无 |
| **RCU** | ⭐⭐⭐ | ⚠️ | ⚠️ | N/A | ⭐⭐⭐ | 无 |

---

## 三、内存序 × 使用场景 匹配矩阵

| 内存序 | 同步强度 | 适用场景 | 性能 |
|:-------|:---------|:---------|:----:|
| **relaxed** | 无 | 纯计数器、标志位 | 最高 |
| **consume** | 数据依赖 | 读操作，依赖链传播 | 高 |
| **acquire** | 获取 | 读锁、消费者 | 高 |
| **release** | 释放 | 写锁、生产者 | 高 |
| **acq_rel** | 获取-释放 | 读-改-写操作 | 中 |
| **seq_cst** | 顺序一致 | 默认、最强同步 | 最低 |

---

## 四、并发问题 → 解决方案 映射

```
数据竞争 (Data Race)
    │
    ├── 读-读竞争
    │   └── 解决：无锁读/原子读
    │
    ├── 读-写竞争
    │   └── 解决：互斥锁/RWLock/原子操作
    │
    └── 写-写竞争
        └── 解决：互斥锁/原子操作/无锁算法

死锁 (Deadlock)
    │
    ├── 循环等待
    │   └── 解决：全局锁顺序/try_lock
    │
    ├── 资源抢占
    │   └── 解决：超时机制/死锁检测
    │
    └── 信号丢失
        └── 解决：条件变量正确使用

活锁/饥饿 (Livelock/Starvation)
    │
    └── 解决：公平锁/退避策略/优先级调整
```

---

## 五、C标准 × 并发支持 演进矩阵

| 特性 | C89 | C99 | C11 | C17 | C23 |
|:-----|:---:|:---:|:---:|:---:|:---:|
| **线程支持** | ❌ | ❌ | ✅ | ✅ | ✅ |
| **原子操作** | ❌ | ❌ | ✅ | ✅ | ✅ |
| **thread_local** | ❌ | ❌ | ✅ | ✅ | ✅ |
| **原子标志** | ❌ | ❌ | ✅ | ✅ | ✅ |
| **内存模型** | ❌ | ❌ | ✅ | ✅ | ✅ |
| **futex-like** | ❌ | ❌ | ❌ | ❌ | ⏳ |

---

## 六、并发安全决策树

```
需要共享数据？
├── 只读数据
│   └── 无需同步 ✅
├── 单生产者单消费者
│   ├── 有界队列
│   │   └── 无锁环形队列 ✅
│   └── 无界队列
│       └── 原子指针+内存序
├── 多生产者单消费者
│   └── 原子操作+内存序
├── 多消费者多生产者
│   ├── 高竞争
│   │   └── 互斥锁+条件变量
│   └── 低竞争
│       └── 自旋锁+退避
└── 复杂数据结构
    ├── 读多写少
    │   └── 读写锁/RCU
    ├── 写多读少
    │   └── 互斥锁
    └── 读写均衡
        └── 互斥锁/无锁结构
```

---

## 七、锁机制深度解析

### 7.1 互斥锁实现原理

```c
#include <threads.h>

// C11 标准互斥锁
typedef struct {
    mtx_t mutex;
    int data;
} ProtectedData;

void increment(ProtectedData *pd) {
    mtx_lock(&pd->mutex);
    pd->data++;  // 临界区
    mtx_unlock(&pd->mutex);
}

// 底层实现示意 (Linux futex)
typedef struct {
    atomic_int state;  // 0=unlocked, 1=locked, 2=contended
} Mutex;

void mutex_lock(Mutex *m) {
    int expected = 0;
    if (!atomic_compare_exchange_strong(&m->state, &expected, 1)) {
        // 竞争发生，进入内核等待
        futex_wait(&m->state, 1);
    }
}
```

### 7.2 读写锁机制

```c
// 读者优先 vs 写者优先
typedef struct {
    atomic_int readers;
    atomic_int writers_waiting;
    mtx_t write_lock;
} RWLock;

void rwlock_read_lock(RWLock *rw) {
    while (atomic_load(&rw->writers_waiting) > 0) {
        // 写者优先：等待写者完成
        thrd_yield();
    }
    atomic_fetch_add(&rw->readers, 1);
}

void rwlock_write_lock(RWLock *rw) {
    atomic_fetch_add(&rw->writers_waiting, 1);
    mtx_lock(&rw->write_lock);
    while (atomic_load(&rw->readers) > 0) {
        // 等待所有读者退出
    }
    atomic_fetch_sub(&rw->writers_waiting, 1);
}
```

### 7.3 自旋锁与互斥锁选择

```c
// 自旋锁 - 适合极短临界区
typedef atomic_flag SpinLock;

void spin_lock(SpinLock *lock) {
    while (atomic_flag_test_and_set(lock)) {
        // 忙等 - 消耗CPU
        // 可添加退避策略
        for (int i = 0; i < 100; i++) __asm__("pause");
    }
}

// 自适应锁 - 先自旋，后阻塞
typedef struct {
    atomic_int state;
    mtx_t mutex;
} AdaptiveLock;

void adaptive_lock(AdaptiveLock *lock) {
    // 先尝试自旋一段时间
    for (int spin = 0; spin < 1000; spin++) {
        int expected = 0;
        if (atomic_compare_exchange_weak(&lock->state, &expected, 1)) {
            return;  // 获得锁
        }
        __asm__("pause");
    }
    // 自旋失败，进入内核阻塞
    mtx_lock(&lock->mutex);
}
```

---

## 八、原子操作与内存序

### 8.1 C11 原子操作示例

```c
#include <stdatomic.h>

// 原子计数器
atomic_int counter = ATOMIC_VAR_INIT(0);

void increment(void) {
    atomic_fetch_add(&counter, 1);  // 原子+=1
}

// 原子标志 - 最简单同步
atomic_flag flag = ATOMIC_FLAG_INIT;

void signal(void) {
    atomic_flag_clear(&flag);
}

void wait(void) {
    while (atomic_flag_test_and_set(&flag)) {
        // 等待信号
    }
}

// 显式内存序
atomic_int data = 0;
atomic_int ready = 0;

void producer(void) {
    atomic_store_explicit(&data, 42, memory_order_relaxed);
    atomic_store_explicit(&ready, 1, memory_order_release);  // 发布
}

void consumer(void) {
    while (atomic_load_explicit(&ready, memory_order_acquire) != 1) {
        // 等待
    }
    int value = atomic_load_explicit(&data, memory_order_relaxed);  // 获取
    // value 保证是42
}
```

### 8.2 内存序可视化

```
Release-Acquire 同步示例:

线程A (生产者)              线程B (消费者)
─────────────               ─────────────
store X, 1                  load ready → 1 (acquire)
store ready, 1 (release)    ↓
                            load X → 必须看到1

同步点: ready的release-acquire对
```

---

## 九、无锁数据结构

### 9.1 无锁栈 (Treiber Stack)

```c
typedef struct Node {
    void *data;
    _Atomic(struct Node *) next;
} Node;

typedef struct {
    _Atomic(Node *) head;
} LockFreeStack;

void push(LockFreeStack *stack, Node *node) {
    Node *old_head;
    do {
        old_head = atomic_load(&stack->head);
        atomic_store(&node->next, old_head);
    } while (!atomic_compare_exchange_weak(
        &stack->head, &old_head, node));
}

Node* pop(LockFreeStack *stack) {
    Node *old_head;
    do {
        old_head = atomic_load(&stack->head);
        if (old_head == NULL) return NULL;
        Node *new_head = atomic_load(&old_head->next);
    } while (!atomic_compare_exchange_weak(
        &stack->head, &old_head, new_head));
    return old_head;
}
```

### 9.2 无锁队列 (MPSC)

```c
typedef struct {
    _Atomic(void *) buffer[QUEUE_SIZE];
    atomic_int head;
    atomic_int tail;
} MPSCQueue;

// 多生产者入队
bool enqueue(MPSCQueue *q, void *item) {
    int tail = atomic_fetch_add(&q->tail, 1);
    if (tail - atomic_load(&q->head) >= QUEUE_SIZE) {
        // 队列满
        atomic_fetch_sub(&q->tail, 1);
        return false;
    }
    atomic_store_explicit(&q->buffer[tail % QUEUE_SIZE],
                         item, memory_order_release);
    return true;
}

// 单消费者出队
void* dequeue(MPSCQueue *q) {
    int head = atomic_load(&q->head);
    if (head == atomic_load(&q->tail)) {
        return NULL;  // 空队列
    }
    void *item = atomic_load_explicit(
        &q->buffer[head % QUEUE_SIZE], memory_order_acquire);
    atomic_fetch_add(&q->head, 1);
    return item;
}
```

---

## 十、死锁预防策略

| 策略 | 描述 | 实现方法 |
|:-----|:-----|:---------|
| **锁顺序** | 全局定义获取顺序 | 按内存地址排序 |
| **超时** | 获取锁设置时限 | try_lock_for(timeout) |
| **死锁检测** | 运行时检测循环等待 | 等待图算法 |
| **无锁设计** | 避免使用锁 | 原子操作 |
| **资源分级** | 按优先级分配资源 | 银行家算法 |

---

> **使用建议**: 在选择并发同步机制时，从底层向上检查需求，选择最低满足需求的层次以获得最佳性能。

---

> **更新记录**
>
> - 2025-03-09: 初版创建
> - 2026-03-13: 扩展锁机制、原子操作、无锁数据结构
