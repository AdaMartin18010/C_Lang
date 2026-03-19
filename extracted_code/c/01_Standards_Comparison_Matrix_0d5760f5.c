/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 2017
 * Language: c
 * Block ID: 0d5760f5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 无锁日志系统 - 利用C11原子操作 */
#include <stdatomic.h>
#include <stdbool.h>
#include <string.h>

#define LOG_BUFFER_SIZE (1 << 20)  /* 1MB */
#define LOG_ENTRY_MAX 1024

typedef struct {
    _Atomic(size_t) head;
    _Atomic(size_t) tail;
    char buffer[LOG_BUFFER_SIZE];
} LogRingBuffer;

static LogRingBuffer g_log_buffer;

bool log_write(const char* msg, size_t len) {
    if (len > LOG_ENTRY_MAX) return false;

    size_t head = atomic_load_explicit(&g_log_buffer.head,
                                       memory_order_relaxed);
    size_t tail = atomic_load_explicit(&g_log_buffer.tail,
                                       memory_order_acquire);

    size_t available = LOG_BUFFER_SIZE - (head - tail);
    if (len + sizeof(size_t) > available) return false;

    /* 写入长度前缀 */
    memcpy(&g_log_buffer.buffer[head % LOG_BUFFER_SIZE],
           &len, sizeof(len));
    head += sizeof(len);

    /* 写入消息 */
    size_t write_pos = head % LOG_BUFFER_SIZE;
    if (write_pos + len > LOG_BUFFER_SIZE) {
        /* 处理环绕 */
        size_t first_part = LOG_BUFFER_SIZE - write_pos;
        memcpy(&g_log_buffer.buffer[write_pos], msg, first_part);
        memcpy(g_log_buffer.buffer, msg + first_part, len - first_part);
    } else {
        memcpy(&g_log_buffer.buffer[write_pos], msg, len);
    }

    atomic_store_explicit(&g_log_buffer.head, head + len,
                          memory_order_release);
    return true;
}
