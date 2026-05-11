/*
 * 无锁 SPSC Ring Buffer 实现
 * 使用 C11 _Atomic + memory_order_relaxed（单生产者单消费者场景下安全）
 * 编译: gcc -Wall -Wextra -O2 -std=c11 -c spsc_ring.c
 */
#include "spsc_ring.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

SpscRing *spsc_ring_create(size_t capacity) {
    SpscRing *ring = calloc(1, sizeof(SpscRing));
    if (!ring) return NULL;
    ring->capacity = capacity;
    ring->buffer = malloc(capacity);
    if (!ring->buffer) {
        free(ring);
        return NULL;
    }
    atomic_init(&ring->head, 0);
    atomic_init(&ring->tail, 0);
    return ring;
}

void spsc_ring_destroy(SpscRing *ring) {
    if (!ring) return;
    free(ring->buffer);
    free(ring);
}

/* 计算可用空间（生产者视角） */
size_t spsc_ring_push_avail(const SpscRing *ring) {
    size_t head = atomic_load_explicit(&ring->head, memory_order_relaxed);
    size_t tail = atomic_load_explicit(&ring->tail, memory_order_acquire);
    if (head >= tail) {
        return ring->capacity - (head - tail) - 1; /* 留一个空位区分满/空 */
    } else {
        return tail - head - 1;
    }
}

/* 计算可读取数据量（消费者视角） */
size_t spsc_ring_pop_avail(const SpscRing *ring) {
    size_t head = atomic_load_explicit(&ring->head, memory_order_acquire);
    size_t tail = atomic_load_explicit(&ring->tail, memory_order_relaxed);
    if (head >= tail) {
        return head - tail;
    } else {
        return ring->capacity - (tail - head);
    }
}

/* 生产者：写入数据 */
bool spsc_ring_push(SpscRing *ring, const void *data, size_t len) {
    if (len == 0 || len > ring->capacity - 1) return false;
    if (spsc_ring_push_avail(ring) < len) return false;

    size_t head = atomic_load_explicit(&ring->head, memory_order_relaxed);
    size_t write_end = head + len;

    if (write_end <= ring->capacity) {
        /* 连续写入 */
        memcpy(ring->buffer + head, data, len);
    } else {
        /* 环绕写入 */
        size_t first = ring->capacity - head;
        memcpy(ring->buffer + head, data, first);
        memcpy(ring->buffer, (const uint8_t *)data + first, len - first);
    }

    /* 更新 head */
    size_t new_head = (head + len) % ring->capacity;
    atomic_store_explicit(&ring->head, new_head, memory_order_release);
    return true;
}

/* 消费者：读取数据 */
size_t spsc_ring_pop(SpscRing *ring, void *buf, size_t buf_size) {
    size_t avail = spsc_ring_pop_avail(ring);
    if (avail == 0) return 0;
    size_t len = (avail < buf_size) ? avail : buf_size;

    size_t tail = atomic_load_explicit(&ring->tail, memory_order_relaxed);
    size_t read_end = tail + len;

    if (read_end <= ring->capacity) {
        memcpy(buf, ring->buffer + tail, len);
    } else {
        size_t first = ring->capacity - tail;
        memcpy(buf, ring->buffer + tail, first);
        memcpy((uint8_t *)buf + first, ring->buffer, len - first);
    }

    size_t new_tail = (tail + len) % ring->capacity;
    atomic_store_explicit(&ring->tail, new_tail, memory_order_release);
    return len;
}
