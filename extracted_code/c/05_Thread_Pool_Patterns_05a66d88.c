/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\05_Thread_Pool_Patterns.md
 * Line: 358
 * Language: c
 * Block ID: 05a66d88
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef enum {
    PRIORITY_LOW = 0,
    PRIORITY_NORMAL = 1,
    PRIORITY_HIGH = 2,
    PRIORITY_URGENT = 3,
    NUM_PRIORITIES
} priority_t;

typedef struct {
    task_t* queues[NUM_PRIORITIES];
    mtx_t locks[NUM_PRIORITIES];
    cnd_t conds[NUM_PRIORITIES];
} priority_pool_t;

/* 提交带优先级的任务 */
bool submit_priority(priority_pool_t* pool, priority_t prio,
                     void (*fn)(void*), void* arg) {
    task_t* task = create_task(fn, arg);

    mtx_lock(&pool->locks[prio]);
    enqueue(&pool->queues[prio], task);
    cnd_signal(&pool->conds[prio]);
    mtx_unlock(&pool->locks[prio]);
    return true;
}

/* 工作线程: 按优先级获取任务 */
task_t* get_next_task(priority_pool_t* pool) {
    /* 高优先级优先 */
    for (int prio = PRIORITY_URGENT; prio >= PRIORITY_LOW; prio--) {
        mtx_lock(&pool->locks[prio]);
        task_t* task = dequeue(&pool->queues[prio]);
        mtx_unlock(&pool->locks[prio]);

        if (task) return task;
    }
    return NULL;
}
