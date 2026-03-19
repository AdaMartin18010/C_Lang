/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\02_Lock_Free_Queues.md
 * Line: 281
 * Language: c
 * Block ID: 03ecdb5e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
