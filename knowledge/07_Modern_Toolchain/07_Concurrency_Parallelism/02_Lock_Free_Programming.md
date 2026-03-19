# 无锁编程 (Lock-Free Programming)

## 目录

- [无锁编程 (Lock-Free Programming)](#无锁编程-lock-free-programming)
  - [目录](#目录)
  - [概述](#概述)
    - [为什么需要无锁编程](#为什么需要无锁编程)
    - [进度保证级别](#进度保证级别)
  - [无锁 vs 无等待](#无锁-vs-无等待)
    - [无锁栈示例 (Lock-Free)](#无锁栈示例-lock-free)
    - [无等待计数器示例 (Wait-Free)](#无等待计数器示例-wait-free)
  - [原子操作原语](#原子操作原语)
    - [比较交换 (CAS) 模式](#比较交换-cas-模式)
    - [获取-累加 (Fetch-and-Add)](#获取-累加-fetch-and-add)
    - [读-修改-写 (RMW) 操作](#读-修改-写-rmw-操作)
  - [无锁队列](#无锁队列)
    - [Michael-Scott 队列 (MPMC)](#michael-scott-队列-mpmc)
    - [无锁工作窃取队列 (Work-Stealing)](#无锁工作窃取队列-work-stealing)
  - [无锁哈希表](#无锁哈希表)
    - [分片哈希表 (Sharded Hash Table)](#分片哈希表-sharded-hash-table)
  - [内存回收 (Hazard Pointers)](#内存回收-hazard-pointers)
    - [基本 Hazard Pointer 实现](#基本-hazard-pointer-实现)
    - [使用 Hazard Pointer 的栈](#使用-hazard-pointer-的栈)
  - [读-复制-更新 (RCU)](#读-复制-更新-rcu)
    - [简化 RCU 实现](#简化-rcu-实现)
  - [序列锁 (Seqlock)](#序列锁-seqlock)
    - [序列锁实现](#序列锁实现)
  - [性能优化](#性能优化)
    - [缓存行优化](#缓存行优化)
    - [指数退避](#指数退避)
  - [验证与调试](#验证与调试)
    - [线程清理](#线程清理)
    - [检测数据竞争](#检测数据竞争)
    - [断言检查](#断言检查)
  - [总结](#总结)
  - [参考资源](#参考资源)

---

## 概述

无锁编程是一种并发编程技术，不依赖互斥锁来保护共享数据，而是使用原子操作来确保线程安全。

### 为什么需要无锁编程

```
传统锁的问题:
1. 死锁 - 获取锁的顺序不一致
2. 优先级反转 - 低优先级线程持有锁，高优先级线程等待
3. 护航 (Convoying) - 线程频繁阻塞唤醒
4. 难以组合 - 多个锁的协调困难

无锁的优势:
1. 确定性的延迟 - 操作总有进度
2. 免疫死锁 - 不需要获取锁
3. 更好的可扩展性 - 减少线程间阻塞
4. 信号安全 - 可以在信号处理程序中调用

代价:
1. 复杂性高 - 难以正确实现
2. 内存开销 - ABA防护、内存回收
3. CPU消耗 - CAS重试可能浪费周期
```

### 进度保证级别

```c
/*
 * 1. 阻塞 (Blocking)
 *    - 使用锁，一个线程可能永远阻塞其他线程
 *
 * 2. 无锁 (Lock-Free)
 *    - 至少一个线程在有限步骤内完成操作
 *    - 系统整体总有进度
 *
 * 3. 无等待 (Wait-Free)
 *    - 每个线程在有限步骤内完成操作
 *    - 最强的进度保证
 *
 * 4. 无干扰 (Obstruction-Free)
 *    - 线程单独执行时能在有限步骤完成
 *    - 最弱的无锁保证
 */
```

---

## 无锁 vs 无等待

### 无锁栈示例 (Lock-Free)

```c
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct node {
    void* data;
    struct node* next;
} node_t;

typedef struct {
    _Atomic(node_t*) top;
} lock_free_stack_t;

void stack_init(lock_free_stack_t* stack) {
    atomic_init(&stack->top, NULL);
}

void stack_push(lock_free_stack_t* stack, void* data) {
    node_t* node = malloc(sizeof(node_t));
    node->data = data;

    node_t* old_top;
    do {
        old_top = atomic_load_explicit(&stack->top, memory_order_relaxed);
        node->next = old_top;
    } while (!atomic_compare_exchange_weak_explicit(
        &stack->top, &old_top, node,
        memory_order_release, memory_order_relaxed));
}

/* 无锁但不保证无等待 - 可能无限重试 */
bool stack_pop(lock_free_stack_t* stack, void** data) {
    node_t* old_top;
    do {
        old_top = atomic_load_explicit(&stack->top, memory_order_acquire);
        if (old_top == NULL) {
            return false;
        }
        /* 其他线程可能在这里插入，导致CAS失败 */
    } while (!atomic_compare_exchange_weak_explicit(
        &stack->top, &old_top, old_top->next,
        memory_order_release, memory_order_relaxed));

    *data = old_top->data;
    /* 内存回收问题! */
    return true;
}
```

### 无等待计数器示例 (Wait-Free)

```c
#include <stdatomic.h>
#include <stdint.h>

/* 无等待计数器 - 每个线程有本地计数器 */
#define MAX_THREADS 64

typedef struct {
    _Atomic(int64_t) counters[MAX_THREADS];
} wait_free_counter_t;

void wf_counter_init(wait_free_counter_t* counter) {
    for (int i = 0; i < MAX_THREADS; i++) {
        atomic_init(&counter->counters[i], 0);
    }
}

/* 无等待增加 - 固定步骤完成 */
void wf_counter_increment(wait_free_counter_t* counter, int thread_id) {
    atomic_fetch_add_explicit(&counter->counters[thread_id], 1,
                               memory_order_relaxed);
}

/* 无等待读取 - 汇总所有计数器 */
int64_t wf_counter_get(wait_free_counter_t* counter) {
    int64_t sum = 0;
    for (int i = 0; i < MAX_THREADS; i++) {
        sum += atomic_load_explicit(&counter->counters[i],
                                     memory_order_relaxed);
    }
    return sum;
}

/*
 * 特点:
 * - 每个线程写自己的缓存行，无竞争
 * - 操作步骤固定，无重试
 * - 真正的无等待 (Wait-Free)
 * - 代价: 读取时需要扫描所有计数器
 */
```

---

## 原子操作原语

### 比较交换 (CAS) 模式

```c
#include <stdatomic.h>
#include <stdbool.h>

/* CAS 重试模式 */
typedef struct {
    _Atomic(int) value;
} atomic_counter_t;

/* 模式1: 简单重试 */
void cas_simple_retry(atomic_counter_t* counter, int new_val) {
    int expected;
    do {
        expected = atomic_load_explicit(&counter->value,
                                        memory_order_relaxed);
    } while (!atomic_compare_exchange_weak_explicit(
        &counter->value, &expected, new_val,
        memory_order_seq_cst, memory_order_relaxed));
}

/* 模式2: 带条件检查 */
bool cas_conditional(atomic_counter_t* counter, int threshold, int increment) {
    int expected, desired;
    do {
        expected = atomic_load_explicit(&counter->value,
                                        memory_order_relaxed);
        if (expected >= threshold) {
            return false;  /* 条件不满足 */
        }
        desired = expected + increment;
    } while (!atomic_compare_exchange_weak_explicit(
        &counter->value, &expected, desired,
        memory_order_seq_cst, memory_order_relaxed));
    return true;
}

/* 模式3: 指数退避 (减少竞争) */
#include <threads.h>

void cas_with_backoff(atomic_counter_t* counter, int new_val) {
    int expected;
    int backoff = 1;
    const int max_backoff = 1024;

    do {
        expected = atomic_load_explicit(&counter->value,
                                        memory_order_relaxed);
        if (!atomic_compare_exchange_weak_explicit(
            &counter->value, &expected, new_val,
            memory_order_seq_cst, memory_order_relaxed)) {

            /* 失败时退避 */
            for (int i = 0; i < backoff; i++) {
                thrd_yield();  /* 或: __asm__ volatile("pause") */
            }
            backoff = (backoff < max_backoff) ? backoff * 2 : max_backoff;
        }
    } while (expected != new_val);
}
```

### 获取-累加 (Fetch-and-Add)

```c
/* FAA 操作 - 比 CAS 更高效，无ABA问题 */

typedef struct {
    _Atomic(int) head;
    _Atomic(int) tail;
    int capacity;
    void** buffer;
} faa_ring_buffer_t;

bool faa_enqueue(faa_ring_buffer_t* q, void* item) {
    int tail = atomic_fetch_add_explicit(&q->tail, 1,
                                          memory_order_relaxed);
    int head = atomic_load_explicit(&q->head, memory_order_acquire);

    if (tail - head >= q->capacity) {
        /* 队列满，回退 */
        atomic_fetch_sub_explicit(&q->tail, 1, memory_order_relaxed);
        return false;
    }

    q->buffer[tail % q->capacity] = item;
    return true;
}

/*
 * FAA 优势:
 * 1. 硬件直接支持，单指令完成
 * 2. 无ABA问题 (递增而非交换)
 * 3. 更少重试
 */
```

### 读-修改-写 (RMW) 操作

```c
/* 位操作 - 原子标志位 */
typedef struct {
    _Atomic(uint32_t) flags;
} atomic_flags_t;

#define FLAG_A (1u << 0)
#define FLAG_B (1u << 1)
#define FLAG_C (1u << 2)

/* 设置标志 */
void set_flag(atomic_flags_t* f, uint32_t flag) {
    atomic_fetch_or_explicit(&f->flags, flag, memory_order_release);
}

/* 清除标志 */
void clear_flag(atomic_flags_t* f, uint32_t flag) {
    atomic_fetch_and_explicit(&f->flags, ~flag, memory_order_release);
}

/* 翻转标志 */
void toggle_flag(atomic_flags_t* f, uint32_t flag) {
    atomic_fetch_xor_explicit(&f->flags, flag, memory_order_release);
}

/* 检查并设置 */
bool test_and_set_flag(atomic_flags_t* f, uint32_t flag) {
    uint32_t old = atomic_fetch_or_explicit(&f->flags, flag,
                                             memory_order_acq_rel);
    return (old & flag) != 0;
}
```

---

## 无锁队列

### Michael-Scott 队列 (MPMC)

```c
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct node {
    void* data;
    _Atomic(struct node*) next;
} node_t;

typedef struct {
    _Atomic(node_t*) head;
    _Atomic(node_t*) tail;
} ms_queue_t;

void ms_queue_init(ms_queue_t* q) {
    node_t* dummy = malloc(sizeof(node_t));
    dummy->data = NULL;
    atomic_init(&dummy->next, NULL);

    atomic_init(&q->head, dummy);
    atomic_init(&q->tail, dummy);
}

void ms_queue_enqueue(ms_queue_t* q, void* data) {
    node_t* node = malloc(sizeof(node_t));
    node->data = data;
    atomic_init(&node->next, NULL);

    node_t* tail;
    node_t* next;

    while (1) {
        tail = atomic_load_explicit(&q->tail, memory_order_acquire);
        next = atomic_load_explicit(&tail->next, memory_order_acquire);

        /* 检查 tail 是否仍然是 tail */
        if (tail == atomic_load_explicit(&q->tail, memory_order_relaxed)) {
            if (next == NULL) {
                /* 尝试链接新节点 */
                if (atomic_compare_exchange_weak_explicit(
                    &tail->next, &next, node,
                    memory_order_release, memory_order_relaxed)) {
                    break;  /* 入队成功 */
                }
            } else {
                /* 协助移动 tail */
                atomic_compare_exchange_weak_explicit(
                    &q->tail, &tail, next,
                    memory_order_release, memory_order_relaxed);
            }
        }
    }

    /* 尝试移动 tail 到新节点 */
    atomic_compare_exchange_weak_explicit(
        &q->tail, &tail, node,
        memory_order_release, memory_order_relaxed);
}

bool ms_queue_dequeue(ms_queue_t* q, void** data) {
    node_t* head;
    node_t* tail;
    node_t* next;

    while (1) {
        head = atomic_load_explicit(&q->head, memory_order_acquire);
        tail = atomic_load_explicit(&q->tail, memory_order_acquire);
        next = atomic_load_explicit(&head->next, memory_order_acquire);

        if (head == atomic_load_explicit(&q->head, memory_order_relaxed)) {
            if (head == tail) {
                if (next == NULL) {
                    return false;  /* 空队列 */
                }
                /* 协助移动 tail */
                atomic_compare_exchange_weak_explicit(
                    &q->tail, &tail, next,
                    memory_order_release, memory_order_relaxed);
            } else {
                *data = next->data;
                /* 尝试移动 head */
                if (atomic_compare_exchange_weak_explicit(
                    &q->head, &head, next,
                    memory_order_release, memory_order_relaxed)) {
                    break;  /* 出队成功 */
                }
            }
        }
    }

    /* 延迟释放 head (Hazard Pointer needed!) */
    /* free(head); */
    return true;
}
```

### 无锁工作窃取队列 (Work-Stealing)

```c
#include <stdatomic.h>
#include <string.h>

#define WS_QUEUE_SIZE 1024

typedef struct {
    void* items[WS_QUEUE_SIZE];
    _Atomic(int) top;      /* 窃取端 */
    _Atomic(int) bottom;   /* 本地端 */
} ws_queue_t;

void ws_queue_init(ws_queue_t* q) {
    atomic_init(&q->top, 0);
    atomic_init(&q->bottom, 0);
}

/* 本地线程: push (bottom) */
bool ws_push(ws_queue_t* q, void* item) {
    int b = atomic_load_explicit(&q->bottom, memory_order_relaxed);
    int t = atomic_load_explicit(&q->top, memory_order_acquire);

    if (b - t >= WS_QUEUE_SIZE - 1) {
        return false;  /* 满 */
    }

    q->items[b % WS_QUEUE_SIZE] = item;
    atomic_thread_fence(memory_order_release);
    atomic_store_explicit(&q->bottom, b + 1, memory_order_relaxed);
    return true;
}

/* 本地线程: pop (bottom) */
void* ws_pop(ws_queue_t* q) {
    int b = atomic_load_explicit(&q->bottom, memory_order_relaxed) - 1;
    atomic_store_explicit(&q->bottom, b, memory_order_relaxed);
    atomic_thread_fence(memory_order_seq_cst);

    int t = atomic_load_explicit(&q->top, memory_order_relaxed);

    if (t <= b) {
        void* item = q->items[b % WS_QUEUE_SIZE];

        if (t != b) {
            return item;  /* 多个元素，直接返回 */
        }

        /* 最后一个元素，与窃取者竞争 */
        if (!atomic_compare_exchange_strong_explicit(
            &q->top, &t, t + 1,
            memory_order_seq_cst, memory_order_relaxed)) {
            /* 竞争失败 */
            atomic_store_explicit(&q->bottom, b + 1, memory_order_relaxed);
            return NULL;
        }

        atomic_store_explicit(&q->bottom, b + 1, memory_order_relaxed);
        return item;
    } else {
        /* 空队列 */
        atomic_store_explicit(&q->bottom, b + 1, memory_order_relaxed);
        return NULL;
    }
}

/* 其他线程: steal (top) */
void* ws_steal(ws_queue_t* q) {
    int t = atomic_load_explicit(&q->top, memory_order_acquire);
    atomic_thread_fence(memory_order_seq_cst);
    int b = atomic_load_explicit(&q->bottom, memory_order_acquire);

    if (t < b) {
        void* item = q->items[t % WS_QUEUE_SIZE];

        if (atomic_compare_exchange_strong_explicit(
            &q->top, &t, t + 1,
            memory_order_seq_cst, memory_order_relaxed)) {
            return item;
        }
        /* 竞争失败，重试或放弃 */
        return NULL;  /* 或重试 */
    }
    return NULL;  /* 空 */
}
```

---

## 无锁哈希表

### 分片哈希表 (Sharded Hash Table)

```c
#include <stdatomic.h>
#include <string.h>

#define NUM_SHARDS 64
#define BUCKET_SIZE 16

typedef struct entry {
    _Atomic(uint64_t) key;    /* 0 表示空 */
    void* value;
} entry_t;

typedef struct shard {
    entry_t buckets[BUCKET_SIZE];
    char padding[64 - sizeof(entry_t) * BUCKET_SIZE % 64];  /* 缓存行填充 */
} shard_t;

typedef struct {
    shard_t shards[NUM_SHARDS];
} concurrent_hash_t;

void chash_init(concurrent_hash_t* ht) {
    for (int i = 0; i < NUM_SHARDS; i++) {
        for (int j = 0; j < BUCKET_SIZE; j++) {
            atomic_init(&ht->shards[i].buckets[j].key, 0);
        }
    }
}

static inline int get_shard(uint64_t key) {
    return (key >> 56) % NUM_SHARDS;  /* 高8位决定分片 */
}

static inline int get_bucket(uint64_t key) {
    return (key & 0xFFFFFFFF) % BUCKET_SIZE;  /* 低32位决定桶 */
}

/* 插入 */
bool chash_insert(concurrent_hash_t* ht, uint64_t key, void* value) {
    if (key == 0) return false;  /* 0 保留为哨兵 */

    int s = get_shard(key);
    entry_t* entries = ht->shards[s].buckets;

    int bucket = get_bucket(key);

    for (int i = 0; i < BUCKET_SIZE; i++) {
        int idx = (bucket + i) % BUCKET_SIZE;  /* 线性探测 */
        uint64_t expected = 0;

        if (atomic_compare_exchange_strong_explicit(
            &entries[idx].key, &expected, key,
            memory_order_release, memory_order_relaxed)) {
            /* 成功获取空槽 */
            entries[idx].value = value;
            return true;
        }

        if (entries[idx].key == key) {
            return false;  /* 已存在 */
        }
    }
    return false;  /* 满 */
}

/* 查找 */
void* chash_get(concurrent_hash_t* ht, uint64_t key) {
    int s = get_shard(key);
    entry_t* entries = ht->shards[s].buckets;

    int bucket = get_bucket(key);

    for (int i = 0; i < BUCKET_SIZE; i++) {
        int idx = (bucket + i) % BUCKET_SIZE;
        uint64_t k = atomic_load_explicit(&entries[idx].key,
                                           memory_order_acquire);
        if (k == key) {
            return entries[idx].value;
        }
        if (k == 0) {
            break;  /* 未找到 */
        }
    }
    return NULL;
}

/*
 * 删除更复杂，需要标记删除或使用 Hazard Pointers
 * 这里简化处理
 */
```

---

## 内存回收 (Hazard Pointers)

### 基本 Hazard Pointer 实现

```c
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_THREADS 64
#define MAX_HP_PER_THREAD 4
#define RETIRED_THRESHOLD 20

typedef struct hp_record {
    _Atomic(void*) hazard_ptrs[MAX_HP_PER_THREAD];
    void* retired_list[RETIRED_THRESHOLD];
    int retired_count;
    struct hp_record* next;
} hp_record_t;

static _Atomic(hp_record_t*) hp_head = NULL;
static __thread hp_record_t* my_hp_record = NULL;

/* 初始化线程的 hazard pointer 记录 */
void hp_thread_init(void) {
    if (my_hp_record != NULL) return;

    my_hp_record = calloc(1, sizeof(hp_record_t));

    hp_record_t* old_head;
    do {
        old_head = atomic_load_explicit(&hp_head, memory_order_relaxed);
        my_hp_record->next = old_head;
    } while (!atomic_compare_exchange_weak_explicit(
        &hp_head, &old_head, my_hp_record,
        memory_order_release, memory_order_relaxed));
}

/* 设置 hazard pointer */
void hp_set(int index, void* ptr) {
    atomic_store_explicit(&my_hp_record->hazard_ptrs[index], ptr,
                          memory_order_release);
}

/* 清除 hazard pointer */
void hp_clear(int index) {
    atomic_store_explicit(&my_hp_record->hazard_ptrs[index], NULL,
                          memory_order_release);
}

/* 扫描所有 hazard pointers，检查是否安全 */
static bool is_safe_to_free(void* ptr) {
    hp_record_t* record = atomic_load_explicit(&hp_head, memory_order_acquire);

    while (record != NULL) {
        for (int i = 0; i < MAX_HP_PER_THREAD; i++) {
            void* hp = atomic_load_explicit(&record->hazard_ptrs[i],
                                            memory_order_acquire);
            if (hp == ptr) {
                return false;  /* 仍被保护 */
            }
        }
        record = record->next;
    }
    return true;
}

/* 尝试释放退休节点 */
static void scan_retired(void) {
    void* new_retired[RETIRED_THRESHOLD];
    int new_count = 0;

    for (int i = 0; i < my_hp_record->retired_count; i++) {
        void* ptr = my_hp_record->retired_list[i];
        if (is_safe_to_free(ptr)) {
            free(ptr);  /* 安全释放 */
        } else {
            new_retired[new_count++] = ptr;  /* 保留 */
        }
    }

    memcpy(my_hp_record->retired_list, new_retired,
           new_count * sizeof(void*));
    my_hp_record->retired_count = new_count;
}

/* 退休节点 */
void hp_retire(void* ptr) {
    my_hp_record->retired_list[my_hp_record->retired_count++] = ptr;

    if (my_hp_record->retired_count >= RETIRED_THRESHOLD) {
        scan_retired();
    }
}

/* 清理 */
void hp_thread_cleanup(void) {
    /* 释放所有退休节点 */
    for (int i = 0; i < my_hp_record->retired_count; i++) {
        /* 这里应该等待所有其他线程 */
        free(my_hp_record->retired_list[i]);
    }

    /* 从链表中移除 */
    /* 简化处理，实际需要CAS操作 */
    free(my_hp_record);
    my_hp_record = NULL;
}
```

### 使用 Hazard Pointer 的栈

```c
typedef struct {
    _Atomic(node_t*) top;
} safe_stack_t;

void safe_push(safe_stack_t* stack, void* data) {
    node_t* node = malloc(sizeof(node_t));
    node->data = data;

    node_t* old_top;
    do {
        old_top = atomic_load_explicit(&stack->top, memory_order_relaxed);
        node->next = old_top;
    } while (!atomic_compare_exchange_weak_explicit(
        &stack->top, &old_top, node,
        memory_order_release, memory_order_relaxed));
}

bool safe_pop(safe_stack_t* stack, void** data) {
    hp_thread_init();

    node_t* old_top;
    do {
        old_top = atomic_load_explicit(&stack->top, memory_order_relaxed);
        if (old_top == NULL) {
            return false;
        }

        /* 设置 hazard pointer */
        hp_set(0, old_top);

        /* 再次检查 */
        if (old_top != atomic_load_explicit(&stack->top,
                                            memory_order_acquire)) {
            continue;  /* 已变化，重试 */
        }
    } while (!atomic_compare_exchange_weak_explicit(
        &stack->top, &old_top, old_top->next,
        memory_order_release, memory_order_relaxed));

    /* 清除 hazard pointer */
    hp_clear(0);

    *data = old_top->data;

    /* 退休节点，延迟释放 */
    hp_retire(old_top);

    return true;
}
```

---

## 读-复制-更新 (RCU)

### 简化 RCU 实现

```c
#include <stdatomic.h>
#include <stdlib.h>

/* RCU: 读多写少场景的零开销读 */

typedef struct {
    _Atomic(int) count;
} rcu_gp_t;

static rcu_gp_t rcu_gp = { 0 };
static __thread int rcu_nest = 0;

/* 读端进入 */
void rcu_read_lock(void) {
    if (rcu_nest++ == 0) {
        /* 第一次进入，记录全局计数 */
        int gp = atomic_load_explicit(&rcu_gp.count, memory_order_acquire);
        /* 存储到线程本地 */
        __asm__ volatile ("" ::: "memory");  /* 编译器屏障 */
    }
}

/* 读端退出 */
void rcu_read_unlock(void) {
    if (--rcu_nest == 0) {
        __asm__ volatile ("" ::: "memory");
    }
}

/* 写端: 等待所有读端完成 */
void synchronize_rcu(void) {
    int old_gp = atomic_fetch_add_explicit(&rcu_gp.count, 1,
                                            memory_order_acq_rel);

    /* 等待所有在之前 grace period 开始的读端完成 */
    /* 实际实现需要遍历所有线程 */
    /* 简化版本 */
}

/* RCU 使用示例 */
typedef struct {
    _Atomic(void*) ptr;
} rcu_pointer_t;

void* rcu_dereference(rcu_pointer_t* rp) {
    return atomic_load_explicit(&rp->ptr, memory_order_consume);
}

void rcu_assign_pointer(rcu_pointer_t* rp, void* new_ptr) {
    atomic_store_explicit(&rp->ptr, new_ptr, memory_order_release);
}

/* 更新示例 */
void update_config(rcu_pointer_t* config_ptr, config_t* new_config) {
    config_t* old_config = atomic_load_explicit(&config_ptr->ptr,
                                                 memory_order_relaxed);

    /* 发布新版本 */
    rcu_assign_pointer(config_ptr, new_config);

    /* 等待 grace period */
    synchronize_rcu();

    /* 安全释放旧版本 */
    free(old_config);
}

/* 读取示例 */
void read_config(rcu_pointer_t* config_ptr) {
    rcu_read_lock();

    config_t* config = rcu_dereference(config_ptr);
    /* 使用 config... */

    rcu_read_unlock();
}
```

---

## 序列锁 (Seqlock)

### 序列锁实现

```c
#include <stdatomic.h>
#include <stdbool.h>

typedef struct {
    _Atomic(unsigned) sequence;  /* 偶数=无写者，奇数=正在写 */
    /* 数据在这里 */
} seqlock_t;

void seqlock_init(seqlock_t* sl) {
    atomic_init(&sl->sequence, 0);
}

/* 写端 */
void seqlock_write_begin(seqlock_t* sl) {
    unsigned seq = atomic_load_explicit(&sl->sequence, memory_order_relaxed);
    atomic_store_explicit(&sl->sequence, seq + 1, memory_order_release);
}

void seqlock_write_end(seqlock_t* sl) {
    unsigned seq = atomic_load_explicit(&sl->sequence, memory_order_relaxed);
    atomic_store_explicit(&sl->sequence, seq + 1, memory_order_release);
}

/* 读端 */
unsigned seqlock_read_begin(seqlock_t* sl) {
    unsigned seq;
    do {
        seq = atomic_load_explicit(&sl->sequence, memory_order_acquire);
    } while (seq & 1);  /* 等待写者完成 */
    return seq;
}

bool seqlock_read_retry(seqlock_t* sl, unsigned start_seq) {
    unsigned end_seq = atomic_load_explicit(&sl->sequence,
                                            memory_order_acquire);
    return end_seq != start_seq;
}

/* 使用示例 */
typedef struct {
    seqlock_t lock;
    int x, y, z;
} data_t;

void update_data(data_t* d, int x, int y, int z) {
    seqlock_write_begin(&d->lock);
    d->x = x;
    d->y = y;
    d->z = z;
    seqlock_write_end(&d->lock);
}

bool read_data(data_t* d, int* x, int* y, int* z) {
    unsigned seq;
    do {
        seq = seqlock_read_begin(&d->lock);
        *x = d->x;
        *y = d->y;
        *z = d->z;
    } while (seqlock_read_retry(&d->lock, seq));

    return true;
}

/*
 * 序列锁特点:
 * - 读者无锁，开销极低
 * - 写者需要重试读者
 * - 适合读多写少
 * - 数据不能包含指针!
 */
```

---

## 性能优化

### 缓存行优化

```c
#include <stdatomic.h>

#define CACHE_LINE_SIZE 64

/* 错误: 伪共享 */
typedef struct {
    _Atomic(int) counter1;  /* 线程1写入 */
    _Atomic(int) counter2;  /* 线程2写入 */
} false_sharing_t;

/* 正确: 缓存行填充 */
typedef struct {
    _Atomic(int) counter;
    char padding[CACHE_LINE_SIZE - sizeof(_Atomic(int))];
} padded_counter_t;

typedef struct {
    padded_counter_t c1;
    padded_counter_t c2;
} no_sharing_t;

/* 批量处理减少竞争 */
#define BATCH_SIZE 64

typedef struct {
    _Atomic(int) count;
    int local_batch[BATCH_SIZE];
    int batch_index;
} batched_counter_t;

void batched_increment(batched_counter_t* c) {
    int idx = c->batch_index++;
    if (idx < BATCH_SIZE) {
        c->local_batch[idx] = 1;  /* 批量累积 */
    } else {
        /* 提交批量 */
        atomic_fetch_add_explicit(&c->count, BATCH_SIZE,
                                   memory_order_relaxed);
        c->batch_index = 0;
    }
}
```

### 指数退避

```c
#include <threads.h>

void exp_backoff(int* count) {
    int limit = 1 << (*count > 10 ? 10 : *count);
    for (int i = 0; i < limit; i++) {
        __asm__ volatile ("pause");  /* x86 */
        /* 或其他架构的yield指令 */
    }
    (*count)++;
}

/* 使用 */
int backoff = 0;
while (!try_operation()) {
    exp_backoff(&backoff);
}
```

---

## 验证与调试

### 线程清理

```c
#include <threads.h>

/* 注册的清理函数 */
void cleanup_handler(void* arg) {
    hp_thread_cleanup();  /* Hazard Pointer清理 */
    /* 其他清理 */
}

int thread_wrapper(void* arg) {
    hp_thread_init();

    /* 设置清理 */
    tss_t cleanup_key;
    tss_create(&cleanup_key, cleanup_handler);
    tss_set(cleanup_key, (void*)1);

    /* 实际工作 */
    worker(arg);

    /* 显式清理 */
    cleanup_handler(NULL);
    return 0;
}
```

### 检测数据竞争

```sh
# 使用 ThreadSanitizer
gcc -fsanitize=thread -g -o program program.c
./program

# 使用 Helgrind (Valgrind)
valgrind --tool=helgrind ./program

# 使用 Intel Inspector
inspxe-cl -collect ti3 ./program
```

### 断言检查

```c
#include <assert.h>

#ifdef DEBUG_LOCKFREE
    #define LF_ASSERT(cond, msg) \
        do { if (!(cond)) { fprintf(stderr, "LF_ASSERT: %s\n", msg); abort(); } } while(0)
#else
    #define LF_ASSERT(cond, msg) ((void)0)
#endif

/* 使用 */
LF_ASSERT(atomic_load(&q->head) != NULL, "Head cannot be null");
```

---

## 总结

无锁编程关键要点：

1. **选择合适的同步机制**
   - 读多写少: RCU、序列锁
   - 高竞争: 分段数据结构
   - 需要等待: 条件变量可能更简单

2. **内存回收是必须的**
   - Hazard Pointers: 通用
   - RCU: 读多写少
   - 垃圾回收: 语言支持

3. **测试和验证**
   - 使用 ThreadSanitizer
   - 压力测试
   - 形式化验证

4. **权衡**
   - 只有当锁成为瓶颈时才使用无锁
   - 优先考虑代码正确性和可维护性

---

## 参考资源

- [The Art of Multiprocessor Programming](https://www.amazon.com/Art-Multiprocessor-Programming-Maurice-Herlihy/dp/0124159508)
- [Lock-Free Algorithms](https://www.cs.cmu.edu/~410-s05/lectures/L31_LockFree.pdf)
- [Hazard Pointers](http://web.cecs.pdx.edu/~walpole/class/cs510/papers/05.pdf)
- [Linux Kernel RCU](https://www.kernel.org/doc/html/latest/RCU/whatisRCU.html)


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
