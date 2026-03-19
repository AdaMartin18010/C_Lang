/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 1489
 * Language: c
 * Block ID: dbb4f35c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 陷阱1.1：简单的非原子操作竞态 */
int counter = 0;

void *increment(void *arg) {
    for (int i = 0; i < 100000; i++) {
        counter++;  // 分解为: LOAD → ADD → STORE，非原子！
    }
    return NULL;
}

/* 问题分析：
 * 线程T1          线程T2
 * LOAD counter=0
 *                 LOAD counter=0
 * ADD → 1
 *                 ADD → 1
 * STORE counter=1
 *                 STORE counter=1
 *
 * 期望：2，实际：1（丢失更新）
 */

/* 修复方案 */
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void *increment_fixed(void *arg) {
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&counter_mutex);
        counter++;
        pthread_mutex_unlock(&counter_mutex);
    }
    return NULL;
}

/* 或者使用原子操作（C11） */
#include <stdatomic.h>
atomic_int atomic_counter = 0;

void *increment_atomic(void *arg) {
    for (int i = 0; i < 100000; i++) {
        atomic_fetch_add(&atomic_counter, 1);
    }
    return NULL;
}
