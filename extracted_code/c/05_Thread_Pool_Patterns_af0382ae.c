/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\05_Thread_Pool_Patterns.md
 * Line: 176
 * Language: c
 * Block ID: af0382ae
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define QUEUE_SIZE 256
#define MAX_WORKERS 16

typedef struct {
    void* data;
    void (*fn)(void*);
} work_item_t;

typedef struct {
    work_item_t items[QUEUE_SIZE];
    _Atomic(int) head;  /* 窃取端 */
    _Atomic(int) tail;  /* 本地端 */
    char padding[64 - 2 * sizeof(_Atomic(int))];  /* 填充到缓存行 */
} work_queue_t;

typedef struct {
    int id;
    thrd_t thread;
    work_queue_t local_queue;
    struct thread_pool_ws* pool;
    atomic_bool running;
} worker_ws_t;

typedef struct thread_pool_ws {
    worker_ws_t workers[MAX_WORKERS];
    int worker_count;
    atomic_int next_worker;
} thread_pool_ws_t;

/* 本地入队 */
bool local_push(work_queue_t* q, work_item_t* item) {
    int tail = atomic_load_explicit(&q->tail, memory_order_relaxed);
    int next = (tail + 1) % QUEUE_SIZE;

    if (next == atomic_load_explicit(&q->head, memory_order_acquire)) {
        return false;  /* 满 */
    }

    q->items[tail] = *item;
    atomic_store_explicit(&q->tail, next, memory_order_release);
    return true;
}

/* 本地出队 */
bool local_pop(work_queue_t* q, work_item_t* item) {
    int tail = atomic_load_explicit(&q->tail, memory_order_relaxed) - 1;
    atomic_store_explicit(&q->tail, tail, memory_order_relaxed);
    atomic_thread_fence(memory_order_seq_cst);

    int head = atomic_load_explicit(&q->head, memory_order_relaxed);

    if (head <= tail) {
        *item = q->items[tail % QUEUE_SIZE];

        if (head != tail) {
            return true;
        }

        /* 最后一个元素 */
        if (!atomic_compare_exchange_strong_explicit(
            &q->head, &head, head + 1,
            memory_order_seq_cst, memory_order_relaxed)) {
            atomic_store_explicit(&q->tail, tail + 1, memory_order_relaxed);
            return false;
        }

        atomic_store_explicit(&q->tail, tail + 1, memory_order_relaxed);
        return true;
    }

    atomic_store_explicit(&q->tail, tail + 1, memory_order_relaxed);
    return false;
}

/* 窃取 */
bool steal(work_queue_t* victim, work_item_t* item) {
    int head = atomic_load_explicit(&victim->head, memory_order_acquire);
    atomic_thread_fence(memory_order_seq_cst);
    int tail = atomic_load_explicit(&victim->tail, memory_order_acquire);

    if (head < tail) {
        *item = victim->items[head % QUEUE_SIZE];

        if (atomic_compare_exchange_strong_explicit(
            &victim->head, &head, head + 1,
            memory_order_seq_cst, memory_order_relaxed)) {
            return true;
        }
    }
    return false;
}
