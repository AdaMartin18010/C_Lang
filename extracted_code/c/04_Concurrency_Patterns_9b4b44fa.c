/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\04_Concurrency_Patterns.md
 * Line: 585
 * Language: c
 * Block ID: 9b4b44fa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    void *result;
    int ready;
} Future;

typedef struct {
    void *(*func)(void *);
    void *arg;
    Future *future;
} AsyncTask;

void *async_execute(void *arg) {
    AsyncTask *task = arg;
    void *result = task->func(task->arg);

    pthread_mutex_lock(&task->future->mutex);
    task->future->result = result;
    task->future->ready = 1;
    pthread_cond_broadcast(&task->future->cond);
    pthread_mutex_unlock(&task->future->mutex);

    free(task);
    return NULL;
}

void *future_get(Future *f) {
    pthread_mutex_lock(&f->mutex);
    while (!f->ready) {
        pthread_cond_wait(&f->cond, &f->mutex);
    }
    void *result = f->result;
    pthread_mutex_unlock(&f->mutex);
    return result;
}
