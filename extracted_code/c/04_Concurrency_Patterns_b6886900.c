/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\04_Concurrency_Patterns.md
 * Line: 487
 * Language: c
 * Block ID: b6886900
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    void (*function)(void *);
    void *argument;
} Task;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    Task *queue;
    int queue_size;
    int head, tail, count;
    pthread_t *threads;
    int num_threads;
    int shutdown;
} ThreadPool;

void *worker(void *arg) {
    ThreadPool *pool = arg;
    while (1) {
        pthread_mutex_lock(&pool->mutex);
        while (pool->count == 0 && !pool->shutdown) {
            pthread_cond_wait(&pool->cond, &pool->mutex);
        }
        if (pool->shutdown) {
            pthread_mutex_unlock(&pool->mutex);
            break;
        }
        Task task = pool->queue[pool->head];
        pool->head = (pool->head + 1) % pool->queue_size;
        pool->count--;
        pthread_mutex_unlock(&pool->mutex);

        task.function(task.argument);  // 执行任务
    }
    return NULL;
}
