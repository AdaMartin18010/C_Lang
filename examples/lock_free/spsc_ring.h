/*
 * 无锁单生产者单消费者环形缓冲区（Lock-Free SPSC Ring Buffer）
 * 头文件
 */
#ifndef SPSC_RING_H
#define SPSC_RING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

typedef struct {
    _Atomic size_t head;  /* 生产者写入位置 */
    _Atomic size_t tail;  /* 消费者读取位置 */
    size_t capacity;
    uint8_t *buffer;
} SpscRing;

/* 生命周期 */
SpscRing *spsc_ring_create(size_t capacity);
void spsc_ring_destroy(SpscRing *ring);

/* 生产者接口（单线程） */
bool spsc_ring_push(SpscRing *ring, const void *data, size_t len);
size_t spsc_ring_push_avail(const SpscRing *ring);

/* 消费者接口（单线程） */
size_t spsc_ring_pop(SpscRing *ring, void *buf, size_t buf_size);
size_t spsc_ring_pop_avail(const SpscRing *ring);

#endif /* SPSC_RING_H */
