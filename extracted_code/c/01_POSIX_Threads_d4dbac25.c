/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 1935
 * Language: c
 * Block ID: d4dbac25
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 陷阱9：信号发送时无等待者 */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int ready = 0;

void *slow_consumer(void *arg) {
    sleep(2);  // 延迟启动

    pthread_mutex_lock(&mutex);
    while (!ready) {
        pthread_cond_wait(&cond, &mutex);
    }
    printf("Consumer: Got signal\n");
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *fast_producer(void *arg) {
    pthread_mutex_lock(&mutex);
    ready = 1;
    pthread_cond_signal(&cond);  // 此时consumer还未wait！
    pthread_mutex_unlock(&mutex);
    printf("Producer: Signal sent\n");
    return NULL;
}

/* 结果：consumer永远等待 */

/* 修复方案1：确保状态持久化 */
void *producer_fixed(void *arg) {
    pthread_mutex_lock(&mutex);
    ready = 1;  // 状态先设置
    pthread_cond_broadcast(&cond);  // 然后通知
    pthread_mutex_unlock(&mutex);
    return NULL;
}

/* 修复方案2：使用信号量（信号不会丢失） */
#include <semaphore.h>
sem_t data_ready;

void consumer_sem(void) {
    sem_wait(&data_ready);  // 即使生产者先post也能收到
    printf("Consumer: Got signal\n");
}

void producer_sem(void) {
    sem_post(&data_ready);  // 信号量保持计数
    printf("Producer: Signal sent\n");
}
