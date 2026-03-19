/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 399
 * Language: c
 * Block ID: 4da85d7c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例：数据竞争
int shared_counter = 0;

void *increment(void *arg) {
    for (int i = 0; i < 100000; i++) {
        shared_counter++;  // 非原子操作，数据竞争！
    }
    return NULL;
}

// ✅ 正确示例：使用互斥锁
#include <pthread.h>

int shared_counter_safe = 0;
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void *increment_safe(void *arg) {
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&counter_mutex);
        shared_counter_safe++;
        pthread_mutex_unlock(&counter_mutex);
    }
    return NULL;
}

// ✅ 更好示例：使用原子操作
#include <stdatomic.h>

_Atomic int atomic_counter = 0;

void *increment_atomic(void *arg) {
    for (int i = 0; i < 100000; i++) {
        atomic_fetch_add(&atomic_counter, 1);
    }
    return NULL;
}
