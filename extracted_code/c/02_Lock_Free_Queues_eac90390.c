/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\02_Lock_Free_Queues.md
 * Line: 329
 * Language: c
 * Block ID: eac90390
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
