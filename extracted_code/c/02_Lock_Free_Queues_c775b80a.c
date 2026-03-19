/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\02_Lock_Free_Queues.md
 * Line: 175
 * Language: c
 * Block ID: c775b80a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
