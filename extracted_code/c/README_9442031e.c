/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\README.md
 * Line: 935
 * Language: c
 * Block ID: 9442031e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 双端队列用于工作窃取 */
typedef struct {
    atomic_size_t top;
    atomic_size_t bottom;
    size_t mask;
    task_t **buffer;
    pthread_mutex_t lock;
} deque_t;

task_t *deque_pop_bottom(deque_t *deque) {
    size_t bottom = atomic_load_explicit(&deque->bottom, memory_order_relaxed) - 1;
    atomic_thread_fence(memory_order_seq_cst);

    size_t top = atomic_load_explicit(&deque->top, memory_order_relaxed);

    if (top <= bottom) {
        task_t *task = deque->buffer[bottom & deque->mask];
        if (top == bottom) {
            /* 最后一个元素 */
            if (!atomic_compare_exchange_strong_explicit(
                    &deque->top, &top, top + 1,
                    memory_order_seq_cst, memory_order_relaxed)) {
                atomic_store_explicit(&deque->bottom, bottom + 1, memory_order_relaxed);
                return NULL;
            }
            atomic_store_explicit(&deque->top, top + 1, memory_order_relaxed);
        }
        atomic_store_explicit(&deque->bottom, bottom, memory_order_relaxed);
        return task;
    }

    atomic_store_explicit(&deque->bottom, bottom + 1, memory_order_relaxed);
    return NULL;
}

task_t *deque_steal(deque_t *deque) {
    size_t top = atomic_load_explicit(&deque->top, memory_order_acquire);
    atomic_thread_fence(memory_order_seq_cst);
    size_t bottom = atomic_load_explicit(&deque->bottom, memory_order_acquire);

    if (top < bottom) {
        task_t *task = deque->buffer[top & deque->mask];
        if (atomic_compare_exchange_strong_explicit(
                &deque->top, &top, top + 1,
                memory_order_seq_cst, memory_order_relaxed)) {
            return task;
        }
    }
    return NULL;
}
