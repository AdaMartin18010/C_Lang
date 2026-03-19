/*
 * Auto-generated from: 12_Practice_Exercises\10_Debugging_Puzzles.md
 * Line: 487
 * Language: c
 * Block ID: 4b3f98a2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 方案1: 互斥锁
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* increment(void *arg) {
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

// 方案2: 原子操作（C11）
#include <stdatomic.h>
static _Atomic int counter = 0;
// counter++ 现在是原子的
