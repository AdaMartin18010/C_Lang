/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\02_C11_Memory_Model.md
 * Line: 266
 * Language: c
 * Block ID: bd525da5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 无锁MPSC队列 (多生产者单消费者)
#include <stdatomic.h>
#include <stdalign.h>
#include <stdlib.h>

#define QUEUE_SIZE 1024
#define CACHE_LINE_SIZE 64

// 队列节点
struct node {
    _Alignas(CACHE_LINE_SIZE) atomic_uintptr_t next;
    void *data;
};

// MPSC队列
struct mpsc_queue {
    _Alignas(CACHE_LINE_SIZE) atomic_uintptr_t head;  // 生产者写入
    _Alignas(CACHE_LINE_SIZE) uintptr_t tail;          // 消费者读取
    struct node *buffer;
    size_t mask;
};

// 初始化队列
void mpsc_queue_init(struct mpsc_queue *q, size_t size) {
    size = 1;
    while (size < QUEUE_SIZE) size <<= 1;  // 向上取2的幂

    q->buffer = calloc(size, sizeof(struct node));
    q->mask = size - 1;

    atomic_init(&q->head, 0);
    q->tail = 0;
}

// 生产者入队 (多线程安全)
bool mpsc_enqueue(struct mpsc_queue *q, void *data) {
    // 获取当前位置
    uintptr_t pos = atomic_fetch_add_explicit(
        &q->head, 1, memory_order_relaxed);

    struct node *n = &q->buffer[pos & q->mask];

    // 等待消费者处理完这个位置
    // 注意：简化实现，实际需要考虑ABA问题
    while (atomic_load_explicit(&n->next, memory_order_acquire) != 0) {
        // 自旋或让出
    }

    n->data = data;
    atomic_store_explicit(&n->next, (uintptr_t)1, memory_order_release);

    return true;
}

// 消费者出队 (单线程)
void *mpsc_dequeue(struct mpsc_queue *q) {
    struct node *n = &q->buffer[q->tail & q->mask];

    // 检查是否有数据
    if (atomic_load_explicit(&n->next, memory_order_acquire) == 0) {
        return NULL;  // 空队列
    }

    void *data = n->data;
    atomic_store_explicit(&n->next, 0, memory_order_release);
    q->tail++;

    return data;
}
