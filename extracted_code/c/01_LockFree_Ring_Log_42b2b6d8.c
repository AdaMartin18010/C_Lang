/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\01_LockFree_Ring_Log.md
 * Line: 144
 * Language: c
 * Block ID: 42b2b6d8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 内存序示例：生产者-消费者同步 */

/* 生产者代码 */
void producer_write(RingBuffer *rb, void *data) {
    uint64_t seq = atomic_fetch_add_u64(&rb->write_seq, 1);
    uint32_t idx = seq & rb->mask;

    /* RELEASE语义：确保数据写入在seq更新前完成 */
    rb->buffer[idx] = data;  /* 数据写入 */
    atomic_thread_fence(memory_order_release);
    atomic_store_explicit(&rb->entry_seq[idx].value, seq, memory_order_release);
}

/* 消费者代码 */
void *consumer_read(RingBuffer *rb) {
    uint64_t seq = atomic_load_u64(&rb->read_seq);
    uint32_t idx = seq & rb->mask;

    /* ACQUIRE语义：确保seq读取后才读数据 */
    uint64_t entry_seq = atomic_load_explicit(
        &rb->entry_seq[idx].value, memory_order_acquire);

    if (entry_seq < seq + 1) {
        return NULL;  /* 数据未就绪 */
    }

    atomic_thread_fence(memory_order_acquire);
    void *data = rb->buffer[idx];  /* 数据读取 */
    atomic_fetch_add_u64(&rb->read_seq, 1);

    return data;
}
