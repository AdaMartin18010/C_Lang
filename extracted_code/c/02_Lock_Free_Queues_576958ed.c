/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\02_Lock_Free_Queues.md
 * Line: 479
 * Language: c
 * Block ID: 576958ed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
