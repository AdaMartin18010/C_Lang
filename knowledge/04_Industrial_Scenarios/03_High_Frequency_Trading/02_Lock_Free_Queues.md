# 无锁队列与LMAX Disruptor

## 1. 无锁数据结构原理

### 1.1 为什么需要无锁队列

在高频交易（HFT）等低延迟场景中，传统基于互斥锁的并发数据结构存在以下问题：

| 问题 | 说明 |
|------|------|
| **上下文切换** | 锁竞争导致线程阻塞，触发上下文切换（~1-10μs） |
| **缓存失效** | 锁变量成为热点，导致CPU缓存频繁失效 |
| **优先级反转** | 高优先级线程可能被低优先级线程阻塞 |
| **死锁风险** | 复杂的锁顺序可能导致死锁 |

无锁（Lock-Free）数据结构使用原子操作代替锁，保证至少有一个线程在有限步骤内完成操作。

### 1.2 内存序（Memory Ordering）

C11标准提供了6种内存序：

```c
// memory_order_relaxed: 最宽松的排序，只保证原子性
// memory_order_consume: 数据依赖顺序（极少使用）
// memory_order_acquire: 获取语义，之后的读写不能重排到前面
// memory_order_release: 释放语义，之前的读写不能重排到后面
// memory_order_acq_rel: 获取+释放（用于读-修改-写操作）
// memory_order_seq_cst: 顺序一致性（默认，最强）

// 典型使用模式
// Load(Load-Consume/Acquire): 读操作
// Store(Store-Release): 写操作
// Read-Modify-Write(Acq_Rel/Seq_Cst): CAS/Exchange等
```

## 2. C11原子操作实现

### 2.1 基础原子类型定义

```c
// lockfree_common.h
#ifndef LOCKFREE_COMMON_H
#define LOCKFREE_COMMON_H

#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// 缓存行大小（通常为64字节）
#define CACHE_LINE_SIZE 64

// 填充到缓存行大小，避免伪共享
#define CACHE_LINE_PAD(size) \
    char _pad[CACHE_LINE_SIZE - ((size) % CACHE_LINE_SIZE)]

// 内存屏障
#define LFENCE() atomic_thread_fence(memory_order_seq_cst)
#define SFENCE() atomic_thread_fence(memory_order_seq_cst)
#define MFENCE() atomic_thread_fence(memory_order_seq_cst)

// 编译屏障
#define COMPILER_BARRIER() __asm__ volatile ("" ::: "memory")

// 性能计数
static inline uint64_t rdtsc(void) {
#if defined(__x86_64__)
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
#elif defined(__aarch64__)
    uint64_t val;
    __asm__ __volatile__ ("mrs %0, cntvct_el0" : "=r" (val));
    return val;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
#endif
}

// 计算时间差（纳秒）
static inline double ns_diff(uint64_t start, uint64_t end) {
    return (double)(end - start);
}

#endif
```

## 3. 单生产者单消费者（SPSC）队列

```c
// spsc_queue.h
#ifndef SPSC_QUEUE_H
#define SPSC_QUEUE_H

#include "lockfree_common.h"

// SPSC无锁队列 - 使用环形缓冲区
typedef struct {
    // 生产者索引（仅生产者线程写入）
    _Alignas(CACHE_LINE_SIZE)
    atomic_size_t head;

    // 消费者索引（仅消费者线程写入）
    _Alignas(CACHE_LINE_SIZE)
    atomic_size_t tail;

    // 缓冲区大小（必须是2的幂）
    size_t capacity;
    size_t mask;

    // 数据缓冲区
    void **buffer;
} SPSCQueue;

// 创建/销毁
SPSCQueue* spsc_queue_create(size_t capacity);
void spsc_queue_destroy(SPSCQueue *q);

// 生产者操作
bool spsc_enqueue(SPSCQueue *q, void *item);
bool spsc_try_enqueue(SPSCQueue *q, void *item);

// 消费者操作
void* spsc_dequeue(SPSCQueue *q);
void* spsc_try_dequeue(SPSCQueue *q);

// 查询状态
static inline size_t spsc_size(const SPSCQueue *q) {
    size_t head = atomic_load_explicit(&q->head, memory_order_acquire);
    size_t tail = atomic_load_explicit(&q->tail, memory_order_acquire);
    return (head - tail);
}

static inline bool spsc_is_empty(const SPSCQueue *q) {
    size_t head = atomic_load_explicit(&q->head, memory_order_acquire);
    size_t tail = atomic_load_explicit(&q->tail, memory_order_acquire);
    return head == tail;
}

static inline bool spsc_is_full(const SPSCQueue *q) {
    size_t head = atomic_load_explicit(&q->head, memory_order_relaxed);
    size_t tail = atomic_load_explicit(&q->tail, memory_order_acquire);
    return (head - tail) >= q->capacity;
}

#endif
```

```c
// spsc_queue.c
#include "spsc_queue.h"

// 找到大于等于n的最小2的幂
static size_t next_power_of_2(size_t n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    if (sizeof(size_t) == 8) {
        n |= n >> 32;
    }
    return n + 1;
}

SPSCQueue* spsc_queue_create(size_t capacity) {
    // 容量必须向上取整到2的幂
    capacity = next_power_of_2(capacity);

    SPSCQueue *q = aligned_alloc(CACHE_LINE_SIZE, sizeof(SPSCQueue));
    if (!q) return NULL;

    q->capacity = capacity;
    q->mask = capacity - 1;
    q->buffer = calloc(capacity, sizeof(void*));

    if (!q->buffer) {
        free(q);
        return NULL;
    }

    atomic_init(&q->head, 0);
    atomic_init(&q->tail, 0);

    return q;
}

void spsc_queue_destroy(SPSCQueue *q) {
    if (q) {
        free(q->buffer);
        free(q);
    }
}

// 生产者入队
bool spsc_enqueue(SPSCQueue *q, void *item) {
    // 获取当前head位置（生产者私有）
    size_t head = atomic_load_explicit(&q->head, memory_order_relaxed);
    size_t next_head = (head + 1) & q->mask;

    // 检查是否满（与tail比较）
    // 注意：消费者可能正在修改tail，使用acquire获取最新值
    size_t tail = atomic_load_explicit(&q->tail, memory_order_acquire);

    if ((head - tail) >= q->capacity - 1) {
        // 队列满（保留一个位置区分满和空）
        return false;
    }

    // 写入数据
    q->buffer[head & q->mask] = item;

    // 更新head（release语义确保数据先写入，再更新索引）
    atomic_store_explicit(&q->head, head + 1, memory_order_release);

    return true;
}

// 消费者出队
void* spsc_dequeue(SPSCQueue *q) {
    // 获取当前tail位置（消费者私有）
    size_t tail = atomic_load_explicit(&q->tail, memory_order_relaxed);

    // 检查是否空（与head比较）
    // 使用acquire确保看到生产者最新写入
    size_t head = atomic_load_explicit(&q->head, memory_order_acquire);

    if (head == tail) {
        return NULL;  // 队列空
    }

    // 读取数据
    void *item = q->buffer[tail & q->mask];

    // 更新tail（release语义）
    atomic_store_explicit(&q->tail, tail + 1, memory_order_release);

    return item;
}

// 非阻塞尝试入队
bool spsc_try_enqueue(SPSCQueue *q, void *item) {
    return spsc_enqueue(q, item);
}

// 非阻塞尝试出队
void* spsc_try_dequeue(SPSCQueue *q) {
    return spsc_dequeue(q);
}
```

## 4. 多生产者多消费者（MPMC）队列

```c
// mpmc_queue.h
#ifndef MPMC_QUEUE_H
#define MPMC_QUEUE_H

#include "lockfree_common.h"

// 队列槽位状态
typedef enum {
    SLOT_EMPTY = 0,      // 空闲
    SLOT_WRITING,        // 正在写入
    SLOT_READY,          // 数据就绪
    SLOT_READING         // 正在读取
} SlotState;

// 队列槽位
typedef struct {
    _Alignas(CACHE_LINE_SIZE)
    atomic_int state;           // 槽位状态
    atomic_size_t turn;         // 轮次（用于同步）
    void *data;                 // 数据指针
} MPMCSlot;

// MPMC无锁队列
typedef struct {
    MPMCSlot *slots;            // 槽位数组
    size_t capacity;            // 容量（2的幂）
    size_t mask;

    _Alignas(CACHE_LINE_SIZE)
    atomic_size_t head;         // 生产者索引

    _Alignas(CACHE_LINE_SIZE)
    atomic_size_t tail;         // 消费者索引
} MPMCQueue;

MPMCQueue* mpmc_queue_create(size_t capacity);
void mpmc_queue_destroy(MPMCQueue *q);

bool mpmc_enqueue(MPMCQueue *q, void *item);
void* mpmc_dequeue(MPMCQueue *q);

bool mpmc_is_empty(const MPMCQueue *q);
bool mpmc_is_full(const MPMCQueue *q);

#endif
```

```c
// mpmc_queue.c
#include "mpmc_queue.h"

static size_t next_power_of_2(size_t n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    if (sizeof(size_t) == 8) n |= n >> 32;
    return n + 1;
}

MPMCQueue* mpmc_queue_create(size_t capacity) {
    capacity = next_power_of_2(capacity);

    MPMCQueue *q = aligned_alloc(CACHE_LINE_SIZE, sizeof(MPMCQueue));
    if (!q) return NULL;

    q->capacity = capacity;
    q->mask = capacity - 1;
    q->slots = aligned_alloc(CACHE_LINE_SIZE,
                             capacity * sizeof(MPMCSlot));

    if (!q->slots) {
        free(q);
        return NULL;
    }

    // 初始化槽位
    for (size_t i = 0; i < capacity; i++) {
        atomic_init(&q->slots[i].state, SLOT_EMPTY);
        atomic_init(&q->slots[i].turn, 0);
        q->slots[i].data = NULL;
    }

    atomic_init(&q->head, 0);
    atomic_init(&q->tail, 0);

    return q;
}

void mpmc_queue_destroy(MPMCQueue *q) {
    if (q) {
        free(q->slots);
        free(q);
    }
}

// MPMC入队（基于Dmitry Vyukov的无锁队列算法）
bool mpmc_enqueue(MPMCQueue *q, void *item) {
    MPMCSlot *slot;
    size_t pos, turn;

    // 获取当前位置
    pos = atomic_fetch_add_explicit(&q->head, 1, memory_order_relaxed);
    slot = &q->slots[pos & q->mask];
    turn = pos / q->capacity;

    // 等待槽位可用
    int expected_state;
    while (1) {
        expected_state = atomic_load_explicit(&slot->state,
                                               memory_order_acquire);

        // 检查轮次是否匹配
        size_t current_turn = atomic_load_explicit(&slot->turn,
                                                    memory_order_relaxed);

        if (current_turn == turn && expected_state == SLOT_EMPTY) {
            // 尝试标记为正在写入
            if (atomic_compare_exchange_weak_explicit(
                    &slot->state,
                    &expected_state,
                    SLOT_WRITING,
                    memory_order_acquire,
                    memory_order_relaxed)) {
                break;
            }
        }
        // 自旋等待
        __asm__ volatile ("pause" ::: "memory");
    }

    // 写入数据
    slot->data = item;
    atomic_store_explicit(&slot->turn, turn + 1, memory_order_release);
    atomic_store_explicit(&slot->state, SLOT_READY, memory_order_release);

    return true;
}

// MPMC出队
void* mpmc_dequeue(MPMCQueue *q) {
    MPMCSlot *slot;
    size_t pos, turn;
    void *item;

    // 获取当前位置
    pos = atomic_fetch_add_explicit(&q->tail, 1, memory_order_relaxed);
    slot = &q->slots[pos & q->mask];
    turn = pos / q->capacity;

    // 等待数据就绪
    int expected_state;
    while (1) {
        expected_state = atomic_load_explicit(&slot->state,
                                               memory_order_acquire);

        size_t current_turn = atomic_load_explicit(&slot->turn,
                                                    memory_order_relaxed);

        if (current_turn == (turn + 1) && expected_state == SLOT_READY) {
            // 尝试标记为正在读取
            if (atomic_compare_exchange_weak_explicit(
                    &slot->state,
                    &expected_state,
                    SLOT_READING,
                    memory_order_acquire,
                    memory_order_relaxed)) {
                break;
            }
        }
        __asm__ volatile ("pause" ::: "memory");
    }

    // 读取数据
    item = slot->data;
    atomic_store_explicit(&slot->state, SLOT_EMPTY, memory_order_release);

    return item;
}

bool mpmc_is_empty(const MPMCQueue *q) {
    size_t head = atomic_load_explicit(&q->head, memory_order_acquire);
    size_t tail = atomic_load_explicit(&q->tail, memory_order_acquire);
    return head == tail;
}

bool mpmc_is_full(const MPMCQueue *q) {
    size_t head = atomic_load_explicit(&q->head, memory_order_relaxed);
    size_t tail = atomic_load_explicit(&q->tail, memory_order_acquire);
    return (head - tail) >= q->capacity;
}
```

## 5. LMAX Disruptor模式实现

```c
// disruptor.h
#ifndef DISRUPTOR_H
#define DISRUPTOR_H

#include "lockfree_common.h"

// Disruptor模式核心组件：
// 1. Ring Buffer - 环形缓冲区
// 2. Sequence - 序列号（类似游标）
// 3. Sequencer - 协调生产者/消费者
// 4. WaitStrategy - 等待策略
// 5. EventProcessor - 事件处理器

// 等待策略类型
typedef enum {
    WAIT_BUSY_SPIN,      // 忙等（最低延迟）
    WAIT_YIELD,          // 线程让步
    WAIT_SLEEP,          // 睡眠等待
    WAIT_BLOCKING        // 阻塞等待（最高吞吐量）
} WaitStrategyType;

// 序列（Sequence）- 使用缓存行填充避免伪共享
typedef struct {
    _Alignas(CACHE_LINE_SIZE)
    atomic_int64_t value;
    char _pad[CACHE_LINE_SIZE - sizeof(atomic_int64_t)];
} Sequence;

// 序列屏障
typedef struct {
    Sequence *cursor;           // 生产者游标
    Sequence **dependencies;    // 依赖的序列（用于DAG）
    size_t dep_count;
} SequenceBarrier;

// Ring Buffer事件
typedef struct {
    int64_t sequence;           // 序列号
    void *data;                 // 事件数据
    bool published;             // 是否已发布
} RingBufferEvent;

// Ring Buffer
typedef struct {
    RingBufferEvent *buffer;    // 事件数组
    size_t capacity;
    size_t mask;

    Sequence *cursor;           // 生产者游标
    Sequence **gating_sequences; // 消费者序列（用于流量控制）
    size_t gating_count;

    WaitStrategyType wait_type;
} RingBuffer;

// Disruptor结构
typedef struct {
    RingBuffer *ring_buffer;
    Sequence **consumer_sequences;
    void (**handlers)(void *event, int64_t sequence, bool end_of_batch);
    size_t consumer_count;
} Disruptor;

// API
RingBuffer* ring_buffer_create(size_t capacity, WaitStrategyType wait_type);
void ring_buffer_destroy(RingBuffer *rb);

int64_t ring_buffer_next(RingBuffer *rb);
void ring_buffer_publish(RingBuffer *rb, int64_t sequence);
RingBufferEvent* ring_buffer_get(RingBuffer *rb, int64_t sequence);

// 等待策略实现
void wait_strategy_wait_for(int64_t sequence, Sequence *cursor,
                            WaitStrategyType type);

#endif
```

```c
// disruptor.c
#include "disruptor.h"
#include <sched.h>
#include <unistd.h>

static size_t next_power_of_2(size_t n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    if (sizeof(size_t) == 8) n |= n >> 32;
    return n + 1;
}

// 创建Ring Buffer
RingBuffer* ring_buffer_create(size_t capacity, WaitStrategyType wait_type) {
    capacity = next_power_of_2(capacity);

    RingBuffer *rb = aligned_alloc(CACHE_LINE_SIZE, sizeof(RingBuffer));
    if (!rb) return NULL;

    rb->buffer = aligned_alloc(CACHE_LINE_SIZE,
                               capacity * sizeof(RingBufferEvent));
    if (!rb->buffer) {
        free(rb);
        return NULL;
    }

    rb->capacity = capacity;
    rb->mask = capacity - 1;
    rb->wait_type = wait_type;

    // 初始化事件
    for (size_t i = 0; i < capacity; i++) {
        rb->buffer[i].sequence = -1;
        rb->buffer[i].published = false;
        rb->buffer[i].data = NULL;
    }

    // 创建生产者游标
    rb->cursor = aligned_alloc(CACHE_LINE_SIZE, sizeof(Sequence));
    atomic_init(&rb->cursor->value, -1);

    rb->gating_sequences = NULL;
    rb->gating_count = 0;

    return rb;
}

void ring_buffer_destroy(RingBuffer *rb) {
    if (rb) {
        free(rb->buffer);
        free(rb->cursor);
        free(rb);
    }
}

// 获取下一个可写位置
int64_t ring_buffer_next(RingBuffer *rb) {
    int64_t next_sequence = atomic_load_explicit(&rb->cursor->value,
                                                   memory_order_relaxed) + 1;

    // 检查是否有足够空间（考虑最慢的消费者）
    if (rb->gating_count > 0) {
        int64_t min_gating = INT64_MAX;
        for (size_t i = 0; i < rb->gating_count; i++) {
            int64_t seq = atomic_load_explicit(&rb->gating_sequences[i]->value,
                                                memory_order_acquire);
            if (seq < min_gating) {
                min_gating = seq;
            }
        }

        // 等待空间释放
        while (next_sequence - min_gating >= (int64_t)rb->capacity) {
            wait_strategy_wait_for(min_gating + 1, rb->cursor, rb->wait_type);

            // 重新获取最小值
            min_gating = INT64_MAX;
            for (size_t i = 0; i < rb->gating_count; i++) {
                int64_t seq = atomic_load_explicit(&rb->gating_sequences[i]->value,
                                                    memory_order_acquire);
                if (seq < min_gating) {
                    min_gating = seq;
                }
            }
        }
    }

    return next_sequence;
}

// 发布事件
void ring_buffer_publish(RingBuffer *rb, int64_t sequence) {
    size_t index = sequence & rb->mask;

    // 写入序列号
    rb->buffer[index].sequence = sequence;
    atomic_thread_fence(memory_order_release);

    // 标记为已发布
    rb->buffer[index].published = true;

    // 更新游标
    atomic_store_explicit(&rb->cursor->value, sequence,
                           memory_order_release);
}

// 获取事件
RingBufferEvent* ring_buffer_get(RingBuffer *rb, int64_t sequence) {
    return &rb->buffer[sequence & rb->mask];
}

// 等待策略实现
void wait_strategy_wait_for(int64_t sequence, Sequence *cursor,
                            WaitStrategyType type) {
    switch (type) {
        case WAIT_BUSY_SPIN:
            // 忙等，使用pause指令减少功耗
            __asm__ volatile ("pause" ::: "memory");
            break;

        case WAIT_YIELD:
            sched_yield();
            break;

        case WAIT_SLEEP:
            usleep(1);
            break;

        case WAIT_BLOCKING:
            // 在实际实现中，这里会使用条件变量
            // 简化版使用yield
            sched_yield();
            break;
    }
}

// 消费者等待特定序列
RingBufferEvent* ring_buffer_wait_for(RingBuffer *rb, int64_t sequence) {
    size_t index = sequence & rb->mask;

    while (!rb->buffer[index].published ||
           rb->buffer[index].sequence != sequence) {
        wait_strategy_wait_for(sequence, rb->cursor, rb->wait_type);
        COMPILER_BARRIER();
    }

    return &rb->buffer[index];
}
```

## 6. 性能测试与对比

```c
// benchmark.c
#include "spsc_queue.h"
#include "mpmc_queue.h"
#include "disruptor.h"
#include <pthread.h>
#include <math.h>

#define ITERATIONS 10000000
#define QUEUE_SIZE 65536

// 测试数据结构
typedef struct {
    int64_t value;
    uint64_t timestamp;
} TestMessage;

// 锁队列（用于对比）
typedef struct {
    pthread_mutex_t lock;
    void **buffer;
    size_t head, tail, capacity;
} LockedQueue;

LockedQueue* locked_queue_create(size_t capacity) {
    LockedQueue *q = malloc(sizeof(LockedQueue));
    pthread_mutex_init(&q->lock, NULL);
    q->buffer = calloc(capacity, sizeof(void*));
    q->capacity = capacity;
    q->head = q->tail = 0;
    return q;
}

bool locked_queue_enqueue(LockedQueue *q, void *item) {
    pthread_mutex_lock(&q->lock);
    size_t next_head = (q->head + 1) % q->capacity;
    if (next_head == q->tail) {
        pthread_mutex_unlock(&q->lock);
        return false;
    }
    q->buffer[q->head] = item;
    q->head = next_head;
    pthread_mutex_unlock(&q->lock);
    return true;
}

void* locked_queue_dequeue(LockedQueue *q) {
    pthread_mutex_lock(&q->lock);
    if (q->head == q->tail) {
        pthread_mutex_unlock(&q->lock);
        return NULL;
    }
    void *item = q->buffer[q->tail];
    q->tail = (q->tail + 1) % q->capacity;
    pthread_mutex_unlock(&q->lock);
    return item;
}

// SPSC测试
typedef struct {
    SPSCQueue *q;
    int64_t count;
    uint64_t *latencies;
} SPSCContext;

void* spsc_producer(void *arg) {
    SPSCContext *ctx = arg;
    for (int64_t i = 0; i < ctx->count; i++) {
        TestMessage *msg = malloc(sizeof(TestMessage));
        msg->value = i;
        msg->timestamp = rdtsc();
        while (!spsc_enqueue(ctx->q, msg)) {
            __asm__ volatile ("pause");
        }
    }
    return NULL;
}

void* spsc_consumer(void *arg) {
    SPSCContext *ctx = arg;
    for (int64_t i = 0; i < ctx->count; i++) {
        TestMessage *msg;
        while (!(msg = spsc_dequeue(ctx->q))) {
            __asm__ volatile ("pause");
        }
        uint64_t latency = rdtsc() - msg->timestamp;
        ctx->latencies[i] = latency;
        free(msg);
    }
    return NULL;
}

// 运行SPSC测试
void benchmark_spsc(void) {
    printf("\n=== SPSC Queue Benchmark ===\n");

    SPSCQueue *q = spsc_queue_create(QUEUE_SIZE);
    SPSCContext ctx = {q, ITERATIONS, calloc(ITERATIONS, sizeof(uint64_t))};

    pthread_t prod, cons;
    uint64_t start = rdtsc();

    pthread_create(&prod, NULL, spsc_producer, &ctx);
    pthread_create(&cons, NULL, spsc_consumer, &ctx);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    uint64_t total_cycles = rdtsc() - start;

    // 计算统计数据
    double avg_latency = 0;
    uint64_t max_latency = 0;
    for (int64_t i = 0; i < ITERATIONS; i++) {
        avg_latency += ctx.latencies[i];
        if (ctx.latencies[i] > max_latency) max_latency = ctx.latencies[i];
    }
    avg_latency /= ITERATIONS;

    printf("Messages: %d\n", ITERATIONS);
    printf("Total time: %.2f ms\n", total_cycles / 2.6e6);
    printf("Throughput: %.2f M msg/s\n",
           ITERATIONS / (total_cycles / 2.6e9));
    printf("Avg latency: %.0f cycles\n", avg_latency);
    printf("Max latency: %lu cycles\n", max_latency);

    free(ctx.latencies);
    spsc_queue_destroy(q);
}

// 锁队列测试
void* locked_producer(void *arg) {
    LockedQueue *q = arg;
    for (int i = 0; i < ITERATIONS; i++) {
        TestMessage *msg = malloc(sizeof(TestMessage));
        msg->value = i;
        while (!locked_queue_enqueue(q, msg)) {
            sched_yield();
        }
    }
    return NULL;
}

void* locked_consumer(void *arg) {
    LockedQueue *q = arg;
    for (int i = 0; i < ITERATIONS; i++) {
        TestMessage *msg;
        while (!(msg = locked_queue_dequeue(q))) {
            sched_yield();
        }
        free(msg);
    }
    return NULL;
}

void benchmark_locked(void) {
    printf("\n=== Locked Queue Benchmark ===\n");

    LockedQueue *q = locked_queue_create(QUEUE_SIZE);
    pthread_t prod, cons;

    uint64_t start = rdtsc();
    pthread_create(&prod, NULL, locked_producer, q);
    pthread_create(&cons, NULL, locked_consumer, q);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    uint64_t total_cycles = rdtsc() - start;

    printf("Messages: %d\n", ITERATIONS);
    printf("Total time: %.2f ms\n", total_cycles / 2.6e6);
    printf("Throughput: %.2f M msg/s\n",
           ITERATIONS / (total_cycles / 2.6e9));
}

int main(void) {
    printf("=== Lock-Free Queue Benchmark ===\n");
    printf("Iterations: %d, Queue size: %d\n", ITERATIONS, QUEUE_SIZE);

    benchmark_spsc();
    benchmark_locked();

    // Disruptor测试
    printf("\n=== Disruptor Benchmark ===\n");
    RingBuffer *rb = ring_buffer_create(QUEUE_SIZE, WAIT_BUSY_SPIN);

    uint64_t start = rdtsc();
    for (int i = 0; i < ITERATIONS; i++) {
        int64_t seq = ring_buffer_next(rb);
        RingBufferEvent *evt = ring_buffer_get(rb, seq);
        evt->data = (void*)(intptr_t)i;
        ring_buffer_publish(rb, seq);
    }
    uint64_t disruptor_cycles = rdtsc() - start;

    printf("Disruptor publish throughput: %.2f M msg/s\n",
           ITERATIONS / (disruptor_cycles / 2.6e9));

    ring_buffer_destroy(rb);

    return 0;
}

// 编译:
// gcc -o benchmark benchmark.c spsc_queue.c mpmc_queue.c disruptor.c \
//     -lpthread -O3 -march=native -std=c17
```

## 7. 性能对比总结

| 队列类型 | 适用场景 | 延迟（典型） | 吞吐量 | 实现复杂度 |
|----------|----------|-------------|--------|-----------|
| **有锁队列** | 通用场景 | 50-500ns | 中等 | 低 |
| **SPSC无锁** | 单生产者单消费者 | 5-20ns | 极高 | 低 |
| **MPMC无锁** | 多生产者多消费者 | 20-50ns | 高 | 中 |
| **Disruptor** | 高频交易/低延迟 | 10-30ns | 极高 | 高 |

在高频交易场景中，Disruptor模式相比传统队列可以获得10-100倍的延迟提升，主要原因是：

1. 消除锁竞争
2. 伪共享避免（缓存行填充）
3. 批处理能力
4. 内存预分配减少GC压力
