/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\02_Synchronization_Matrix.md
 * Line: 404
 * Language: c
 * Block ID: 9d81d53d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// spinlock_demo.c
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>

#define NUM_THREADS 4
#define ITERATIONS 10000000

typedef struct {
    pthread_spinlock_t spinlock;
    long counter;
} spin_data_t;

spin_data_t spin_data;

// 自旋锁线程 - 适用于短临界区
void *spinlock_worker(void *arg) {
    (void)arg;

    for (int i = 0; i < ITERATIONS; i++) {
        pthread_spin_lock(&spin_data.spinlock);

        // 极短的临界区 - 仅自增
        spin_data.counter++;

        pthread_spin_unlock(&spin_data.spinlock);
    }

    return NULL;
}

// 自定义自旋锁 (使用原子操作)
typedef struct {
    volatile int flag;
} custom_spinlock_t;

#define CUSTOM_SPINLOCK_INITIALIZER {0}

static inline void custom_spin_init(custom_spinlock_t *lock) {
    lock->flag = 0;
}

static inline void custom_spin_lock(custom_spinlock_t *lock) {
    // TAS (Test-And-Set) 自旋
    while (__sync_lock_test_and_set(&lock->flag, 1)) {
        // 自旋等待，可选CPU放松
        #ifdef __x86_64__
        __asm__ volatile ("pause" ::: "memory");
        #else
        sched_yield();  // 让出CPU
        #endif
    }
}

static inline void custom_spin_unlock(custom_spinlock_t *lock) {
    __sync_lock_release(&lock->flag);
}

custom_spinlock_t custom_lock = CUSTOM_SPINLOCK_INITIALIZER;
long custom_counter = 0;

void *custom_spin_worker(void *arg) {
    (void)arg;

    for (int i = 0; i < ITERATIONS; i++) {
        custom_spin_lock(&custom_lock);
        custom_counter++;
        custom_spin_unlock(&custom_lock);
    }

    return NULL;
}

// 带退避的自旋锁
#define MAX_BACKOFF 1024

typedef struct {
    volatile int flag;
} backoff_spinlock_t;

void backoff_spin_lock(backoff_spinlock_t *lock) {
    int backoff = 1;

    while (__sync_lock_test_and_set(&lock->flag, 1)) {
        // 指数退避
        for (int i = 0; i < backoff; i++) {
            #ifdef __x86_64__
            __asm__ volatile ("pause");
            #endif
        }
        backoff = (backoff < MAX_BACKOFF) ? backoff * 2 : MAX_BACKOFF;
    }
}

void backoff_spin_unlock(backoff_spinlock_t *lock) {
    __sync_lock_release(&lock->flag);
}

// 比较自旋锁和互斥锁性能
void performance_comparison(void) {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_spinlock_t spinlock;

    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);

    long mutex_counter = 0;
    long spin_counter = 0;

    // 互斥锁测试
    // ... (省略计时代码)

    // 自旋锁测试
    // ... (省略计时代码)

    pthread_mutex_destroy(&mutex);
    pthread_spin_destroy(&spinlock);
}

int main(void) {
    pthread_t threads[NUM_THREADS];

    printf("=== 自旋锁演示 ===\n");

    // 初始化自旋锁
    pthread_spin_init(&spin_data.spinlock, PTHREAD_PROCESS_PRIVATE);
    spin_data.counter = 0;

    printf("Testing POSIX spinlock...\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, spinlock_worker, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Final counter: %ld (expected: %d)\n",
           spin_data.counter, NUM_THREADS * ITERATIONS);

    pthread_spin_destroy(&spin_data.spinlock);

    // 自定义自旋锁测试
    printf("\nTesting custom spinlock...\n");
    custom_spin_init(&custom_lock);
    custom_counter = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, custom_spin_worker, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Custom counter: %ld\n", custom_counter);

    printf("\n自旋锁适用场景:\n");
    printf("- 临界区极短 (几个指令)\n");
    printf("- 多核系统\n");
    printf("- 锁竞争不激烈\n");
    printf("- 避免上下文切换开销\n");

    return 0;
}
