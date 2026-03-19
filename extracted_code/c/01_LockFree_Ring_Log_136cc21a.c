/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\01_LockFree_Ring_Log.md
 * Line: 245
 * Language: c
 * Block ID: 136cc21a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 生产者写入 - 无锁 */
bool spsc_ring_push(SPSCRingBuffer *rb, void *data) {
    uint64_t write_idx = atomic_load_explicit(
        &rb->producer.write_idx.value, memory_order_relaxed);

    /* 检查空间 - 使用缓存的读位置避免缓存行跳动 */
    uint64_t next_write = write_idx + 1;
    if (next_write - rb->producer.write_cached_read >= rb->capacity) {
        rb->producer.write_cached_read = atomic_load_explicit(
            &rb->consumer.read_idx.value, memory_order_acquire);

        if (next_write - rb->producer.write_cached_read >= rb->capacity) {
            return false;  /* 缓冲区满 */
        }
    }

    /* 写入数据 */
    uint32_t idx = write_idx & rb->mask;
    rb->buffer[idx] = data;

    /* 发布写入 */
    atomic_thread_fence(memory_order_release);
    atomic_store_explicit(&rb->producer.write_idx.value, next_write,
                         memory_order_release);

    return true;
}

/* 消费者读取 - 无锁 */
bool spsc_ring_pop(SPSCRingBuffer *rb, void **data) {
    uint64_t read_idx = atomic_load_explicit(
        &rb->consumer.read_idx.value, memory_order_relaxed);

    /* 检查数据可用性 - 使用缓存的写位置 */
    if (read_idx >= rb->consumer.read_cached_write) {
        rb->consumer.read_cached_write = atomic_load_explicit(
            &rb->producer.write_idx.value, memory_order_acquire);

        if (read_idx >= rb->consumer.read_cached_write) {
            return false;  /* 缓冲区空 */
        }
    }

    /* 读取数据 */
    uint32_t idx = read_idx & rb->mask;
    atomic_thread_fence(memory_order_acquire);
    *data = rb->buffer[idx];

    /* 确认读取 */
    atomic_store_explicit(&rb->consumer.read_idx.value, read_idx + 1,
                         memory_order_release);

    return true;
}

/* 批量写入 - 减少内存屏障开销 */
uint32_t spsc_ring_push_burst(SPSCRingBuffer *rb, void **data, uint32_t n) {
    uint64_t write_idx = atomic_load_explicit(
        &rb->producer.write_idx.value, memory_order_relaxed);
    uint64_t read_idx = atomic_load_explicit(
        &rb->consumer.read_idx.value, memory_order_acquire);

    /* 计算可用空间 */
    uint32_t available = rb->capacity - (write_idx - read_idx);
    uint32_t to_write = (n < available) ? n : available;

    if (to_write == 0) return 0;

    /* 批量写入 */
    for (uint32_t i = 0; i < to_write; i++) {
        uint32_t idx = (write_idx + i) & rb->mask;
        rb->buffer[idx] = data[i];
    }

    /* 单次内存屏障发布所有写入 */
    atomic_thread_fence(memory_order_release);
    atomic_store_explicit(&rb->producer.write_idx.value, write_idx + to_write,
                         memory_order_release);

    return to_write;
}
