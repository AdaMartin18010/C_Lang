/*
 * Auto-generated from: 12_Practice_Exercises\10_Debugging_Puzzles.md
 * Line: 550
 * Language: c
 * Block ID: 7f12b55f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 方案1: 统一加锁顺序
void* thread2(void *arg) {
    pthread_mutex_lock(&m1);  // 先m1
    pthread_mutex_lock(&m2);  // 再m2
    // ...
}

// 方案2: 使用trylock
void* thread2(void *arg) {
    pthread_mutex_lock(&m2);
    if (pthread_mutex_trylock(&m1) != 0) {
        pthread_mutex_unlock(&m2);
        // 重试或返回错误
    }
    // ...
}

// 方案3: 使用层级锁或单个锁
