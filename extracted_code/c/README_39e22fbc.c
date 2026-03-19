/*
 * Auto-generated from: README.md
 * Line: 847
 * Language: c
 * Block ID: 39e22fbc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Lock-free SPSC ring buffer - 用于高频交易
#include <stdatomic.h>
#include <stdalign.h>

typedef struct {
    alignas(64) atomic_size_t head;  // 生产者索引
    alignas(64) atomic_size_t tail;  // 消费者索引
    size_t capacity;
    uint8_t buffer[];  // 柔性数组
} spsc_ring_t;

bool spsc_enqueue(spsc_ring_t* ring, const void* data, size_t len) {
    size_t head = atomic_load_explicit(&ring->head, memory_order_relaxed);
    size_t tail = atomic_load_explicit(&ring->tail, memory_order_acquire);

    size_t available = ring->capacity - (head - tail);
    if (available < len) return false;  // 队列满

    // 写入数据...

    atomic_store_explicit(&ring->head, head + len, memory_order_release);
    return true;
}
