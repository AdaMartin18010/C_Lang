/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 2134
 * Language: c
 * Block ID: a8e167a1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 陷阱12：缺少同步导致的数据竞争 */
int shared_data = 0;
int ready_flag = 0;

void *producer_no_sync(void *arg) {
    shared_data = 42;
    ready_flag = 1;  // 没有同步原语，可能重排序！
    return NULL;
}

void *consumer_no_sync(void *arg) {
    while (!ready_flag) {
        // 忙等待
    }
    printf("Data: %d\n", shared_data);  // 可能看到0！
    return NULL;
}

/* 问题分析（可能的重排序）：
 * 编译器/CPU可能重排序：
 * 实际执行顺序可能是：
 *   producer: ready_flag = 1
 *   consumer: 看到ready_flag=1，读取shared_data=0
 *   producer: shared_data = 42
 */

/* 修复方案1：使用互斥锁建立happens-before */
pthread_mutex_t hb_mutex = PTHREAD_MUTEX_INITIALIZER;

void *producer_hb(void *arg) {
    pthread_mutex_lock(&hb_mutex);
    shared_data = 42;
    ready_flag = 1;
    pthread_mutex_unlock(&hb_mutex);
    return NULL;
}

void *consumer_hb(void *arg) {
    pthread_mutex_lock(&hb_mutex);
    while (!ready_flag) {
        pthread_mutex_unlock(&hb_mutex);
        sched_yield();
        pthread_mutex_lock(&hb_mutex);
    }
    printf("Data: %d\n", shared_data);  // 保证看到42
    pthread_mutex_unlock(&hb_mutex);
    return NULL;
}

/* 修复方案2：使用内存屏障 */
#include <stdatomic.h>
atomic_int atomic_ready = 0;

void *producer_barrier(void *arg) {
    shared_data = 42;
    atomic_thread_fence(memory_order_release);  // 发布语义
    atomic_store_explicit(&atomic_ready, 1, memory_order_relaxed);
    return NULL;
}

void *consumer_barrier(void *arg) {
    while (atomic_load_explicit(&atomic_ready, memory_order_relaxed) == 0) {
        // 自旋
    }
    atomic_thread_fence(memory_order_acquire);  // 获取语义
    printf("Data: %d\n", shared_data);  // 保证看到42
    return NULL;
}

/* 修复方案3：使用C11原子变量 */
_Atomic int atomic_data = 0;

void *producer_atomic(void *arg) {
    atomic_store(&atomic_data, 42);
    return NULL;
}

void *consumer_atomic(void *arg) {
    int val = atomic_load(&atomic_data);
    printf("Data: %d\n", val);
    return NULL;
}
