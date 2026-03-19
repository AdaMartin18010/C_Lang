/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\05_Thread_Pool_Patterns.md
 * Line: 451
 * Language: c
 * Block ID: d611207d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    task_t* batch[64];
    int count;
    mtx_t lock;
} batch_queue_t;

void submit_batch(thread_pool_t* pool, batch_queue_t* bq,
                  void (*fn)(void*), void* arg) {
    task_t* task = create_task(fn, arg);

    mtx_lock(&bq->lock);
    bq->batch[bq->count++] = task;

    if (bq->count >= 64) {
        /* 批量提交 */
        mtx_lock(&pool->queue_lock);
        for (int i = 0; i < bq->count; i++) {
            enqueue(&pool->queue, bq->batch[i]);
        }
        mtx_unlock(&pool->queue_lock);
        cnd_broadcast(&pool->task_available);
        bq->count = 0;
    }
    mtx_unlock(&bq->lock);
}
