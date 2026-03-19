/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\03_Advanced_Learning_Path.md
 * Line: 181
 * Language: c
 * Block ID: 35c0fee4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef LOCK_FREE_QUEUE_H
#define LOCK_FREE_QUEUE_H

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>

// Michael-Scott队列实现
typedef struct LFQueueNode {
    _Atomic(void *) data;
    _Atomic(struct LFQueueNode *) next;
} LFQueueNode;

typedef struct {
    _Atomic(LFQueueNode *) head;
    _Atomic(LFQueueNode *) tail;
    _Atomic(size_t) size;
    char padding[64 - 3 * sizeof(atomic_uintptr_t) - sizeof(atomic_size_t)];
} LFQueue;

LFQueue* lfq_create(void);
void lfq_destroy(LFQueue *queue);
bool lfq_enqueue(LFQueue *queue, void *data);
bool lfq_dequeue(LFQueue *queue, void **data);
size_t lfq_size(LFQueue *queue);
bool lfq_is_empty(LFQueue *queue);

// 多生产者多消费者队列（MPMC）
typedef struct {
    // 环形缓冲区实现
    _Atomic(size_t) head;
    _Atomic(size_t) tail;
    size_t capacity;
    _Atomic(void *) *buffer;
} MPMCQueue;

MPMCQueue* mpmcq_create(size_t capacity);
void mpmcq_destroy(MPMCQueue *queue);
bool mpmcq_enqueue(MPMCQueue *queue, void *data);
bool mpmcq_dequeue(MPMCQueue *queue, void **data);

#endif
