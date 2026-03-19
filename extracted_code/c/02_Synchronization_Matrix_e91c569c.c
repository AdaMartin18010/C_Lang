/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\02_Synchronization_Matrix.md
 * Line: 89
 * Language: c
 * Block ID: e91c569c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// mutex_demo.c
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 4
#define INCREMENTS 1000000

typedef struct {
    pthread_mutex_t mutex;
    long counter;
    long read_counter;
} shared_data_t;

shared_data_t data = {
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .counter = 0,
    .read_counter = 0
};

// 普通互斥锁
void *mutex_worker(void *arg) {
    (void)arg;
    for (int i = 0; i < INCREMENTS; i++) {
        pthread_mutex_lock(&data.mutex);
        data.counter++;
        pthread_mutex_unlock(&data.mutex);
    }
    return NULL;
}

// 尝试加锁 (非阻塞)
void *trylock_worker(void *arg) {
    (void)arg;
    int locked = 0;
    for (int i = 0; i < INCREMENTS; i++) {
        // 尝试获取锁，不阻塞
        if (pthread_mutex_trylock(&data.mutex) == 0) {
            data.counter++;
            pthread_mutex_unlock(&data.mutex);
            locked++;
        } else {
            // 获取失败，做其他工作
            __asm__ volatile ("nop");  // 忙等待替代工作
        }
    }
    printf("Thread locked %d times\n", locked);
    return NULL;
}

// 定时加锁
void *timedlock_worker(void *arg) {
    (void)arg;
    struct timespec timeout;
    int success = 0, timeout_count = 0;

    for (int i = 0; i < 100; i++) {
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_nsec += 1000000;  // 1ms

        if (pthread_mutex_timedlock(&data.mutex, &timeout) == 0) {
            data.counter++;
            pthread_mutex_unlock(&data.mutex);
            success++;
        } else {
            timeout_count++;
        }
    }
    printf("Success: %d, Timeouts: %d\n", success, timeout_count);
    return NULL;
}

// 递归互斥锁示例
void recursive_function(shared_data_t *d, int depth);

pthread_mutex_t recursive_mutex;
pthread_mutexattr_t recursive_attr;

void *recursive_worker(void *arg) {
    (void)arg;

    // 初始化递归互斥锁
    pthread_mutexattr_init(&recursive_attr);
    pthread_mutexattr_settype(&recursive_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&recursive_mutex, &recursive_attr);

    recursive_function(&data, 5);

    pthread_mutex_destroy(&recursive_mutex);
    pthread_mutexattr_destroy(&recursive_attr);
    return NULL;
}

void recursive_function(shared_data_t *d, int depth) {
    if (depth <= 0) return;

    pthread_mutex_lock(&recursive_mutex);  // 第一次加锁
    d->read_counter++;

    recursive_function(d, depth - 1);  // 递归，再次加锁

    pthread_mutex_unlock(&recursive_mutex);
}

int main(void) {
    pthread_t threads[NUM_THREADS];

    printf("=== 互斥锁演示 ===\n");

    // 普通互斥锁测试
    printf("\n1. 普通互斥锁测试\n");
    data.counter = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, mutex_worker, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Final counter: %ld (expected: %d)\n",
           data.counter, NUM_THREADS * INCREMENTS);

    // 尝试加锁测试
    printf("\n2. 尝试加锁测试\n");
    data.counter = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, trylock_worker, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Final counter: %ld\n", data.counter);

    // 递归互斥锁测试
    printf("\n3. 递归互斥锁测试\n");
    data.read_counter = 0;
    pthread_t recursive_thread;
    pthread_create(&recursive_thread, NULL, recursive_worker, NULL);
    pthread_join(recursive_thread, NULL);
    printf("Recursive counter: %ld\n", data.read_counter);

    pthread_mutex_destroy(&data.mutex);
    return 0;
}
