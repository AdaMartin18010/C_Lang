/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\README.md
 * Line: 460
 * Language: c
 * Block ID: c4374cc6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 无锁单生产者单消费者(SPSC)队列
 * 适用于HFT核心路径
 */

#include <stdatomic.h>
#include <stdint.h>
#include <stdbool.h>

/* 缓存行大小 */
#define CACHE_LINE_SIZE 64

/* SPSC无锁队列 */
typedef struct {
    /* 生产者索引 - 独占缓存行 */
    alignas(CACHE_LINE_SIZE)
    atomic_size_t head;          /* 写入位置 */

    /* 消费者索引 - 独占缓存行 */
    alignas(CACHE_LINE_SIZE)
    atomic_size_t tail;          /* 读取位置 */

    /* 队列数据 - 独占缓存行 */
    alignas(CACHE_LINE_SIZE)
    size_t capacity;             /* 容量（必须是2的幂） */
    size_t mask;                 /* 用于快速取模 */
    void **buffer;               /* 数据缓冲区 */
} spsc_queue_t;

/* 初始化队列 */
int spsc_queue_init(spsc_queue_t *q, size_t capacity)
{
    /* 容量必须是2的幂 */
    if ((capacity & (capacity - 1)) != 0) {
        return -1;
    }

    q->buffer = aligned_alloc(CACHE_LINE_SIZE,
                              capacity * sizeof(void *));
    if (!q->buffer) {
        return -1;
    }

    q->capacity = capacity;
    q->mask = capacity - 1;
    atomic_init(&q->head, 0);
    atomic_init(&q->tail, 0);

    return 0;
}

/* 生产者入队 */
bool spsc_enqueue(spsc_queue_t *q, void *item)
{
    const size_t head = atomic_load_explicit(&q->head,
                                              memory_order_relaxed);
    const size_t next_head = (head + 1) & q->mask;

    /* 检查队列是否满 */
    if (next_head == atomic_load_explicit(&q->tail,
                                           memory_order_acquire)) {
        return false;  /* 队列满 */
    }

    /* 写入数据 */
    q->buffer[head] = item;

    /* 发布：更新head，使用release语义确保消费者看到完整数据 */
    atomic_store_explicit(&q->head, next_head, memory_order_release);

    return true;
}

/* 消费者出队 */
bool spsc_dequeue(spsc_queue_t *q, void **item)
{
    const size_t tail = atomic_load_explicit(&q->tail,
                                              memory_order_relaxed);

    /* 检查队列是否空 */
    if (tail == atomic_load_explicit(&q->head,
                                      memory_order_acquire)) {
        return false;  /* 队列空 */
    }

    /* 读取数据 */
    *item = q->buffer[tail];

    /* 消费：更新tail，使用release语义 */
    atomic_store_explicit(&q->tail, (tail + 1) & q->mask,
                          memory_order_release);

    return true;
}

/* 批量操作 - 减少内存屏障开销 */
size_t spsc_dequeue_bulk(spsc_queue_t *q, void **items, size_t max)
{
    const size_t head = atomic_load_explicit(&q->head,
                                              memory_order_acquire);
    size_t tail = atomic_load_explicit(&q->tail,
                                        memory_order_relaxed);

    size_t available = (head - tail) & q->mask;
    size_t to_read = (available < max) ? available : max;

    for (size_t i = 0; i < to_read; i++) {
        items[i] = q->buffer[(tail + i) & q->mask];
    }

    atomic_store_explicit(&q->tail, (tail + to_read) & q->mask,
                          memory_order_release);

    return to_read;
}
