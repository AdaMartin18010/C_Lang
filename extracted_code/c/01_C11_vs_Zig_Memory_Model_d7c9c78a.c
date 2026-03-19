/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
 * Line: 1554
 * Language: c
 * Block ID: d7c9c78a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 生产者-消费者 (有界缓冲区)
#include <stdatomic.h>
#include <stdbool.h>
#include <string.h>

#define BUFFER_SIZE 1024

typedef struct {
    char buffer[BUFFER_SIZE];
    _Atomic size_t write_idx;
    _Atomic size_t read_idx;
} RingBuffer;

void ringbuffer_init(RingBuffer* rb) {
    atomic_init(&rb->write_idx, 0);
    atomic_init(&rb->read_idx, 0);
}

// 返回可写入的字节数
size_t ringbuffer_write_avail(const RingBuffer* rb) {
    size_t write_idx = atomic_load_explicit(&rb->write_idx, memory_order_relaxed);
    size_t read_idx = atomic_load_explicit(&rb->read_idx, memory_order_acquire);
    return (BUFFER_SIZE - 1 + read_idx - write_idx) % BUFFER_SIZE;
}

// 返回可读取的字节数
size_t ringbuffer_read_avail(const RingBuffer* rb) {
    size_t write_idx = atomic_load_explicit(&rb->write_idx, memory_order_acquire);
    size_t read_idx = atomic_load_explicit(&rb->read_idx, memory_order_relaxed);
    return (BUFFER_SIZE + write_idx - read_idx) % BUFFER_SIZE;
}

bool ringbuffer_write(RingBuffer* rb, const char* data, size_t len) {
    if (ringbuffer_write_avail(rb) < len) {
        return false;  // 空间不足
    }

    size_t write_idx = atomic_load_explicit(&rb->write_idx, memory_order_relaxed);

    for (size_t i = 0; i < len; i++) {
        rb->buffer[(write_idx + i) % BUFFER_SIZE] = data[i];
    }

    // Release: 确保数据写入在索引更新前可见
    atomic_store_explicit(&rb->write_idx,
                         (write_idx + len) % BUFFER_SIZE,
                         memory_order_release);
    return true;
}

bool ringbuffer_read(RingBuffer* rb, char* data, size_t len) {
    if (ringbuffer_read_avail(rb) < len) {
        return false;  // 数据不足
    }

    size_t read_idx = atomic_load_explicit(&rb->read_idx, memory_order_relaxed);

    for (size_t i = 0; i < len; i++) {
        data[i] = rb->buffer[(read_idx + i) % BUFFER_SIZE];
    }

    // Release: 确保读取完成在索引更新前可见
    atomic_store_explicit(&rb->read_idx,
                         (read_idx + len) % BUFFER_SIZE,
                         memory_order_release);
    return true;
}
