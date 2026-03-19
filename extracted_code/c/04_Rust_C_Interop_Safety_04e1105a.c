/*
 * Auto-generated from: 08_Zig_C_Connection\04_Rust_C_Interop_Safety.md
 * Line: 3232
 * Language: c
 * Block ID: 04e1105a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C语言并发示例（POSIX线程） */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* 共享数据 - 需要手动同步 */
int shared_counter = 0;
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

/* 线程函数 */
void* increment_counter(void* arg) {
    int thread_id = *(int*)arg;

    for (int i = 0; i < 1000; i++) {
        /* 手动加锁 */
        pthread_mutex_lock(&counter_mutex);

        /* 临界区 */
        shared_counter++;

        /* 手动解锁 */
        pthread_mutex_unlock(&counter_mutex);
    }

    printf("线程 %d 完成\n", thread_id);
    return NULL;
}

/* 无锁（数据竞争示例） */
void* unsafe_increment(void* arg) {
    for (int i = 0; i < 1000; i++) {
        /* 危险！没有同步 */
        shared_counter++;
    }
    return NULL;
}

/* 条件变量示例 */
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;
int ready = 0;

void* producer(void* arg) {
    sleep(1);  /* 模拟工作 */

    pthread_mutex_lock(&cond_mutex);
    ready = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&cond_mutex);

    return NULL;
}

void* consumer(void* arg) {
    pthread_mutex_lock(&cond_mutex);
    while (!ready) {
        pthread_cond_wait(&cond, &cond_mutex);
    }
    pthread_mutex_unlock(&cond_mutex);

    printf("消费者收到信号\n");
    return NULL;
}

int main(void) {
    pthread_t threads[4];
    int thread_ids[4] = {0, 1, 2, 3};

    printf("C语言并发测试\n");
    printf("============\n");

    /* 创建线程 */
    for (int i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, increment_counter, &thread_ids[i]);
    }

    /* 等待线程完成 */
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("最终计数器值: %d（预期: 4000）\n", shared_counter);

    /* 条件变量测试 */
    printf("\n条件变量测试:\n");
    pthread_t prod, cons;
    pthread_create(&cons, NULL, consumer, NULL);
    pthread_create(&prod, NULL, producer, NULL);
    pthread_join(cons, NULL);
    pthread_join(prod, NULL);

    /* 清理 */
    pthread_mutex_destroy(&counter_mutex);
    pthread_mutex_destroy(&cond_mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
