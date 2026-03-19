/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\02_Lock_Free_Queues.md
 * Line: 558
 * Language: c
 * Block ID: 34cf8b9e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
