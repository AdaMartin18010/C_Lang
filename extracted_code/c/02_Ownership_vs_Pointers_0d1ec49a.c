/*
 * Auto-generated from: 15_Memory_Safe_Languages\01_Rust_vs_C\02_Ownership_vs_Pointers.md
 * Line: 271
 * Language: c
 * Block ID: 0d1ec49a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C: 数据竞争示例
#include <pthread.h>

int shared_counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* increment(void* arg) {
    for (int i = 0; i < 100000; i++) {
        // 容易遗漏锁保护
        shared_counter++;  // 非原子操作，数据竞争！

        // 正确但需要手动确保
        pthread_mutex_lock(&mutex);
        shared_counter++;  // 受保护
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
