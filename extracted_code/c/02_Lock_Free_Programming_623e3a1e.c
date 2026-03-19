/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\02_Lock_Free_Programming.md
 * Line: 256
 * Language: c
 * Block ID: 623e3a1e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* FAA 操作 - 比 CAS 更高效，无ABA问题 */

typedef struct {
    _Atomic(int) head;
    _Atomic(int) tail;
    int capacity;
    void** buffer;
} faa_ring_buffer_t;

bool faa_enqueue(faa_ring_buffer_t* q, void* item) {
    int tail = atomic_fetch_add_explicit(&q->tail, 1,
                                          memory_order_relaxed);
    int head = atomic_load_explicit(&q->head, memory_order_acquire);

    if (tail - head >= q->capacity) {
        /* 队列满，回退 */
        atomic_fetch_sub_explicit(&q->tail, 1, memory_order_relaxed);
        return false;
    }

    q->buffer[tail % q->capacity] = item;
    return true;
}

/*
 * FAA 优势:
 * 1. 硬件直接支持，单指令完成
 * 2. 无ABA问题 (递增而非交换)
 * 3. 更少重试
 */
