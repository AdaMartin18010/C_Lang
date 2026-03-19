/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\02_Lock_Free_Queues.md
 * Line: 116
 * Language: c
 * Block ID: ef61e127
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
