/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\README.md
 * Line: 405
 * Language: c
 * Block ID: 65d4a2fe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 基于无锁环形缓冲区的异步日志
 *
 * 生产者（业务线程）-> 环形缓冲区 -> 消费者（日志线程）-> 磁盘
 */

#include <stdatomic.h>
#include <pthread.h>
#include <stdint.h>

#define RING_BUFFER_SIZE (64 * 1024 * 1024)  // 64MB 环形缓冲区
#define CACHE_LINE_SIZE 64

// 无锁环形缓冲区
typedef struct __attribute__((aligned(CACHE_LINE_SIZE))) {
    uint8_t *buffer;
    size_t size;

    // 使用缓存行对齐避免伪共享
    alignas(CACHE_LINE_SIZE) _Atomic size_t write_pos;
    alignas(CACHE_LINE_SIZE) _Atomic size_t read_pos;

    pthread_mutex_t notify_mutex;
    pthread_cond_t notify_cond;
} LockFreeRingBuffer;

// 初始化环形缓冲区
LockFreeRingBuffer* ring_buffer_create(size_t size)
{
    LockFreeRingBuffer *rb = aligned_alloc(CACHE_LINE_SIZE,
                                            sizeof(LockFreeRingBuffer));
    rb->buffer = aligned_alloc(CACHE_LINE_SIZE, size);
    rb->size = size;
    atomic_init(&rb->write_pos, 0);
    atomic_init(&rb->read_pos, 0);
    pthread_mutex_init(&rb->notify_mutex, NULL);
    pthread_cond_init(&rb->notify_cond, NULL);
    return rb;
}

// 生产者写入（无锁）
size_t ring_buffer_write(LockFreeRingBuffer *rb, const void *data,
                          size_t len)
{
    size_t write_pos = atomic_load(&rb->write_pos);
    size_t read_pos = atomic_load(&rb->read_pos);

    // 计算可用空间
    size_t available = rb->size - (write_pos - read_pos);
    if (len > available) {
        return 0;  // 缓冲区已满
    }

    // 计算实际写入位置（取模）
    size_t actual_pos = write_pos & (rb->size - 1);  // 要求 size 是 2 的幂

    // 处理环绕
    size_t first_chunk = rb->size - actual_pos;
    if (len <= first_chunk) {
        memcpy(rb->buffer + actual_pos, data, len);
    } else {
        memcpy(rb->buffer + actual_pos, data, first_chunk);
        memcpy(rb->buffer, (uint8_t *)data + first_chunk, len - first_chunk);
    }

    // 内存屏障 + 更新写位置
    __atomic_thread_fence(__ATOMIC_RELEASE);
    atomic_store(&rb->write_pos, write_pos + len);

    // 通知消费者
    pthread_cond_signal(&rb->notify_cond);

    return len;
}

// 消费者读取
size_t ring_buffer_read(LockFreeRingBuffer *rb, void *data, size_t len)
{
    size_t read_pos = atomic_load(&rb->read_pos);
    size_t write_pos = atomic_load(&rb->write_pos);

    size_t available = write_pos - read_pos;
    size_t to_read = (len < available) ? len : available;

    if (to_read == 0) return 0;

    size_t actual_pos = read_pos & (rb->size - 1);
    size_t first_chunk = rb->size - actual_pos;

    if (to_read <= first_chunk) {
        memcpy(data, rb->buffer + actual_pos, to_read);
    } else {
        memcpy(data, rb->buffer + actual_pos, first_chunk);
        memcpy((uint8_t *)data + first_chunk, rb->buffer,
               to_read - first_chunk);
    }

    __atomic_thread_fence(__ATOMIC_ACQUIRE);
    atomic_store(&rb->read_pos, read_pos + to_read);

    return to_read;
}
