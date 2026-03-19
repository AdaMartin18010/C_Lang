/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\01_LockFree_Ring_Log.md
 * Line: 464
 * Language: c
 * Block ID: c2507190
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 日志条目结构 */
typedef struct {
    uint64_t timestamp;
    uint32_t level;
    uint32_t tid;
    char message[256];
} LogEntry;

/* MPSC日志缓冲区 */
typedef struct {
    /* 生产者头 - 多线程竞争 */
    AtomicU64 head CACHE_ALIGNED;

    /* 消费者尾 - 单线程独占 */
    AtomicU64 tail CACHE_ALIGNED;

    /* 预留区域 - 解决ABA问题 */
    AtomicU64 reserve_head CACHE_ALIGNED;

    uint32_t capacity;
    uint32_t mask;

    LogEntry *entries;
} MPSCLogBuffer;

/* 预留写入空间 - 批量 */
uint64_t mpsc_log_reserve(MPSCLogBuffer *buf, uint32_t n) {
    return atomic_fetch_add_explicit(&buf->reserve_head.value, n,
                                    memory_order_relaxed);
}

/* 提交写入 */
void mpsc_log_commit(MPSCLogBuffer *buf, uint64_t position) {
    /* 等待前面所有预留完成 */
    while (atomic_load_explicit(&buf->head.value, memory_order_acquire)
           != position) {
        __asm__ volatile("pause");
    }

    atomic_store_explicit(&buf->head.value, position + 1,
                         memory_order_release);
}

/* 消费 - 批量读取 */
uint32_t mpsc_log_consume_burst(MPSCLogBuffer *buf, LogEntry *out,
                                 uint32_t max) {
    uint64_t head = atomic_load_explicit(&buf->head.value,
                                        memory_order_acquire);
    uint64_t tail = atomic_load_explicit(&buf->tail.value,
                                        memory_order_relaxed);

    uint32_t available = head - tail;
    uint32_t to_read = (available < max) ? available : max;

    for (uint32_t i = 0; i < to_read; i++) {
        uint32_t pos = (tail + i) & buf->mask;
        out[i] = buf->entries[pos];
    }

    atomic_store_explicit(&buf->tail.value, tail + to_read,
                         memory_order_release);

    return to_read;
}
