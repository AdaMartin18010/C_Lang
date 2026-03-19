/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\02_Cache_Coherence.md
 * Line: 384
 * Language: c
 * Block ID: cf6a1c44
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 伪共享(False Sharing)检测与优化实例
// ============================================================================

/*
 * 场景: 多线程高并发计数器
 */

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_THREADS     8
#define ITERATIONS      10000000

// ❌ 错误实现: 伪共享严重
struct bad_counter {
    uint64_t count;  // 8字节，同一缓存行
};

struct bad_counter bad_counters[NUM_THREADS];

void *bad_worker(void *arg) {
    int id = *(int*)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        bad_counters[id].count++;  // 所有线程竞争同一缓存行!
    }
    return NULL;
}

// ✅ 正确实现1: 手动填充
struct good_counter {
    uint64_t count;
    char padding[56];  // 填充到64字节
};

struct good_counter good_counters[NUM_THREADS];

void *good_worker(void *arg) {
    int id = *(int*)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        good_counters[id].count++;  // 各线程独占缓存行
    }
    return NULL;
}

// ✅ 正确实现2: 使用__attribute__((aligned(64)))
struct aligned_counter {
    uint64_t count;
} __attribute__((aligned(64)));

struct aligned_counter aligned_counters[NUM_THREADS];

void *aligned_worker(void *arg) {
    int id = *(int*)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        aligned_counters[id].count++;
    }
    return NULL;
}

// 性能测试
int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];
    struct timespec start, end;

    // 测试bad实现
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, bad_worker, &ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double bad_time = (end.tv_sec - start.tv_sec) +
                      (end.tv_nsec - start.tv_nsec) / 1e9;

    // 测试good实现
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, good_worker, &ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double good_time = (end.tv_sec - start.tv_sec) +
                       (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Bad implementation:  %.3f seconds\n", bad_time);
    printf("Good implementation: %.3f seconds\n", good_time);
    printf("Speedup: %.1fx\n", bad_time / good_time);

    return 0;
}
/* 典型输出:
 * Bad implementation:  2.5 seconds
 * Good implementation: 0.3 seconds
 * Speedup: 8.3x
 */
