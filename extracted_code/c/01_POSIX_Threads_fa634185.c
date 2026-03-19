/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 1538
 * Language: c
 * Block ID: fa634185
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 陷阱2.1：相互等待死锁 */
pthread_mutex_t mutex_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_b = PTHREAD_MUTEX_INITIALIZER;

void *thread_a(void *arg) {
    pthread_mutex_lock(&mutex_a);
    printf("Thread A: locked A\n");
    usleep(1000);  // 增加上下文切换概率

    pthread_mutex_lock(&mutex_b);  // 等待B
    printf("Thread A: locked B\n");

    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
    return NULL;
}

void *thread_b(void *arg) {
    pthread_mutex_lock(&mutex_b);
    printf("Thread B: locked B\n");
    usleep(1000);

    pthread_mutex_lock(&mutex_a);  // 等待A → 死锁！
    printf("Thread B: locked A\n");

    pthread_mutex_unlock(&mutex_a);
    pthread_mutex_unlock(&mutex_b);
    return NULL;
}

/* 修复方案：全局锁排序 */
#define LOCK_A_FIRST 1
#define LOCK_B_FIRST 2

void lock_ordered(pthread_mutex_t *m1, pthread_mutex_t *m2, int order) {
    pthread_mutex_t *first = (order == LOCK_A_FIRST) ? m1 : m2;
    pthread_mutex_t *second = (order == LOCK_A_FIRST) ? m2 : m1;
    pthread_mutex_lock(first);
    pthread_mutex_lock(second);
}

void *thread_a_fixed(void *arg) {
    lock_ordered(&mutex_a, &mutex_b, LOCK_A_FIRST);
    // ... 临界区
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
    return NULL;
}

void *thread_b_fixed(void *arg) {
    lock_ordered(&mutex_a, &mutex_b, LOCK_A_FIRST);  // 相同顺序！
    // ... 临界区
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
    return NULL;
}
