/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\02_Synchronization_Matrix.md
 * Line: 737
 * Language: c
 * Block ID: 9ffa49ff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// condvar_demo.c
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 10
#define NUM_ITEMS 100

// 带条件变量的有界缓冲区
typedef struct {
    int buffer[BUFFER_SIZE];
    int count;
    int in;
    int out;

    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} cond_buffer_t;

cond_buffer_t cb = {
    .count = 0,
    .in = 0,
    .out = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .not_full = PTHREAD_COND_INITIALIZER,
    .not_empty = PTHREAD_COND_INITIALIZER
};

void cond_buffer_put(cond_buffer_t *b, int item) {
    pthread_mutex_lock(&b->mutex);

    // 等待直到有空间
    while (b->count == BUFFER_SIZE) {
        pthread_cond_wait(&b->not_full, &b->mutex);
    }

    b->buffer[b->in] = item;
    b->in = (b->in + 1) % BUFFER_SIZE;
    b->count++;

    printf("Produced: %d, count=%d\n", item, b->count);

    // 通知消费者
    pthread_cond_signal(&b->not_empty);
    pthread_mutex_unlock(&b->mutex);
}

int cond_buffer_get(cond_buffer_t *b) {
    pthread_mutex_lock(&b->mutex);

    // 等待直到有数据
    while (b->count == 0) {
        pthread_cond_wait(&b->not_empty, &b->mutex);
    }

    int item = b->buffer[b->out];
    b->out = (b->out + 1) % BUFFER_SIZE;
    b->count--;

    printf("Consumed: %d, count=%d\n", item, b->count);

    // 通知生产者
    pthread_cond_signal(&b->not_full);
    pthread_mutex_unlock(&b->mutex);

    return item;
}

// 广播示例 - 多消费者唤醒
typedef struct {
    int ready;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} barrier_t;

barrier_t barrier = {
    .ready = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .cond = PTHREAD_COND_INITIALIZER
};

void wait_for_ready(barrier_t *b) {
    pthread_mutex_lock(&b->mutex);
    while (!b->ready) {
        pthread_cond_wait(&b->cond, &b->mutex);
    }
    pthread_mutex_unlock(&b->mutex);
}

void set_ready(barrier_t *b) {
    pthread_mutex_lock(&b->mutex);
    b->ready = 1;
    pthread_cond_broadcast(&b->cond);  // 唤醒所有等待线程
    pthread_mutex_unlock(&b->mutex);
}

// 定时等待示例
int timed_wait_for_ready(barrier_t *b, int timeout_ms) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;

    pthread_mutex_lock(&b->mutex);
    int result = 0;
    while (!b->ready && result == 0) {
        result = pthread_cond_timedwait(&b->cond, &b->mutex, &ts);
    }
    int was_ready = b->ready;
    pthread_mutex_unlock(&b->mutex);

    if (result == ETIMEDOUT) {
        printf("Timed out waiting\n");
        return -1;
    }
    return was_ready ? 0 : -1;
}

// 线程函数
void *producer_thread(void *arg) {
    (void)arg;
    for (int i = 0; i < NUM_ITEMS; i++) {
        cond_buffer_put(&cb, i);
        usleep(rand() % 10000);
    }
    return NULL;
}

void *consumer_thread(void *arg) {
    int id = *(int*)arg;
    for (int i = 0; i < NUM_ITEMS / 2; i++) {
        int item = cond_buffer_get(&cb);
        (void)item;
        usleep(rand() % 15000);
    }
    printf("Consumer %d finished\n", id);
    return NULL;
}

int main(void) {
    pthread_t producer;
    pthread_t consumers[2];
    int consumer_ids[2] = {1, 2};

    printf("=== 条件变量演示 ===\n");

    pthread_create(&producer, NULL, producer_thread, NULL);
    pthread_create(&consumers[0], NULL, consumer_thread, &consumer_ids[0]);
    pthread_create(&consumers[1], NULL, consumer_thread, &consumer_ids[1]);

    pthread_join(producer, NULL);
    pthread_join(consumers[0], NULL);
    pthread_join(consumers[1], NULL);

    printf("\n=== 广播测试 ===\n");
    // 创建等待线程
    // ... 省略

    pthread_mutex_destroy(&cb.mutex);
    pthread_cond_destroy(&cb.not_full);
    pthread_cond_destroy(&cb.not_empty);

    return 0;
}
