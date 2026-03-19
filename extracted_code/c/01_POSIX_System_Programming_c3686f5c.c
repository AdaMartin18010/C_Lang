/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 3106
 * Language: c
 * Block ID: c3686f5c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file posix_condition_rwlock.c
 * @brief POSIX条件变量和读写锁
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int ready = 0;

/**
 * @brief 条件变量基本使用
 */
void *condition_wait_thread(void *arg) {
    printf("等待线程: 等待条件...\n");

    pthread_mutex_lock(&mutex);

    /* 等待条件，自动释放互斥锁 */
    while (!ready) {
        pthread_cond_wait(&cond, &mutex);
    }

    printf("等待线程: 条件满足，继续执行\n");

    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *condition_signal_thread(void *arg) {
    sleep(2);

    printf("信号线程: 准备发送信号...\n");

    pthread_mutex_lock(&mutex);
    ready = 1;
    pthread_mutex_unlock(&mutex);

    /* 发送信号唤醒等待线程 */
    pthread_cond_signal(&cond);
    printf("信号线程: 信号已发送\n");

    return NULL;
}

void condition_variable_demo(void) {
    pthread_t wait_thread, signal_thread;

    printf("\n=== 条件变量 ===\n");

    ready = 0;

    pthread_create(&wait_thread, NULL, condition_wait_thread, NULL);
    pthread_create(&signal_thread, NULL, condition_signal_thread, NULL);

    pthread_join(wait_thread, NULL);
    pthread_join(signal_thread, NULL);
}

/**
 * @brief 生产者消费者模型
 */
#define BUFFER_SIZE 10

static int buffer[BUFFER_SIZE];
static int count = 0;
static int produce_index = 0;
static int consume_index = 0;

static pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
static pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 20; i++) {
        pthread_mutex_lock(&buffer_mutex);

        /* 等待缓冲区不满 */
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&not_full, &buffer_mutex);
        }

        /* 生产数据 */
        buffer[produce_index] = id * 100 + i;
        printf("生产者 %d: 生产 %d\n", id, buffer[produce_index]);
        produce_index = (produce_index + 1) % BUFFER_SIZE;
        count++;

        /* 通知消费者 */
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&buffer_mutex);

        usleep(100000);  /* 模拟生产时间 */
    }

    return NULL;
}

void *consumer(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&buffer_mutex);

        /* 等待缓冲区不空 */
        while (count == 0) {
            pthread_cond_wait(&not_empty, &buffer_mutex);
        }

        /* 消费数据 */
        int data = buffer[consume_index];
        printf("  消费者 %d: 消费 %d\n", id, data);
        consume_index = (consume_index + 1) % BUFFER_SIZE;
        count--;

        /* 通知生产者 */
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&buffer_mutex);

        usleep(200000);  /* 模拟消费时间 */
    }

    return NULL;
}

void producer_consumer_demo(void) {
    pthread_t producers[2], consumers[2];
    int p_ids[2] = {1, 2};
    int c_ids[2] = {1, 2};

    printf("\n=== 生产者消费者模型 ===\n");

    /* 创建生产者 */
    for (int i = 0; i < 2; i++) {
        pthread_create(&producers[i], NULL, producer, &p_ids[i]);
    }

    /* 创建消费者 */
    for (int i = 0; i < 2; i++) {
        pthread_create(&consumers[i], NULL, consumer, &c_ids[i]);
    }

    /* 等待所有线程 */
    for (int i = 0; i < 2; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(consumers[i], NULL);
    }
}

/**
 * @brief 读写锁演示
 */
static pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
static int shared_data = 0;

void *reader_thread(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 5; i++) {
        pthread_rwlock_rdlock(&rwlock);  /* 获取读锁 */
        printf("读者 %d: 读取数据 = %d\n", id, shared_data);
        pthread_rwlock_unlock(&rwlock);
        usleep(100000);
    }

    return NULL;
}

void *writer_thread(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 3; i++) {
        pthread_rwlock_wrlock(&rwlock);  /* 获取写锁 */
        shared_data++;
        printf("写者 %d: 写入数据 = %d\n", id, shared_data);
        pthread_rwlock_unlock(&rwlock);
        usleep(300000);
    }

    return NULL;
}

void read_write_lock_demo(void) {
    pthread_t readers[3], writers[2];
    int r_ids[3] = {1, 2, 3};
    int w_ids[2] = {1, 2};

    printf("\n=== 读写锁 ===\n");

    shared_data = 0;

    /* 创建读者线程 */
    for (int i = 0; i < 3; i++) {
        pthread_create(&readers[i], NULL, reader_thread, &r_ids[i]);
    }

    /* 创建写者线程 */
    for (int i = 0; i < 2; i++) {
        pthread_create(&writers[i], NULL, writer_thread, &w_ids[i]);
    }

    /* 等待所有线程 */
    for (int i = 0; i < 3; i++) {
        pthread_join(readers[i], NULL);
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(writers[i], NULL);
    }

    printf("最终数据值: %d\n", shared_data);
}

/**
 * @brief 屏障同步
 */
static pthread_barrier_t barrier;

void *barrier_thread(void *arg) {
    int id = *(int *)arg;

    printf("线程 %d: 到达屏障\n", id);

    /* 等待所有线程到达屏障 */
    int result = pthread_barrier_wait(&barrier);

    if (result == PTHREAD_BARRIER_SERIAL_THREAD) {
        printf("线程 %d: 是序列化线程（最后一个到达）\n", id);
    } else {
        printf("线程 %d: 通过屏障\n", id);
    }

    return NULL;
}

void barrier_demo(void) {
    pthread_t threads[4];
    int ids[4] = {1, 2, 3, 4};

    printf("\n=== 屏障同步 ===\n");

    /* 初始化屏障，计数为4 */
    pthread_barrier_init(&barrier, NULL, 4);

    /* 创建线程 */
    for (int i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, barrier_thread, &ids[i]);
    }

    /* 等待所有线程 */
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);
    printf("所有线程已通过屏障\n");
}

int main(void) {
    printf("=== POSIX条件变量和读写锁演示 ===\n");

    condition_variable_demo();
    producer_consumer_demo();
    read_write_lock_demo();
    barrier_demo();

    printf("\n=== 同步原语演示完成 ===\n");
    return 0;
}
