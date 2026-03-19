/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 3586
 * Language: c
 * Block ID: f940dc72
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file rcu_demo.c
 * @brief RCU无锁读优化技术
 *
 * RCU特点：
 * - 读者无锁、无原子操作、无内存屏障
 * - 写者复制修改，原子替换指针
 * - 延迟回收旧数据
 *
 * 适用场景：读多写少的数据结构
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

/* 简化版RCU实现 */

static _Atomic(int) rcu_readers_active = 0;
static pthread_mutex_t rcu_write_lock = PTHREAD_MUTEX_INITIALIZER;

#define rcu_read_lock() \
    atomic_fetch_add_explicit(&rcu_readers_active, 1, memory_order_acquire)

#define rcu_read_unlock() \
    atomic_fetch_sub_explicit(&rcu_readers_active, 1, memory_order_release)

#define rcu_synchronize() \
    do { \
        while (atomic_load_explicit(&rcu_readers_active, memory_order_acquire) > 0) \
            ; \
    } while(0)

/**
 * @brief RCU保护的数据结构
 */
typedef struct DataStructure {
    int value;
    char data[1024];
} DataStructure;

static _Atomic(DataStructure*) g_data = NULL;

/* 读者 */
void* reader_thread(void *arg) {
    int iterations = *(int*)arg;
    long long sum = 0;

    for (int i = 0; i < iterations; i++) {
        rcu_read_lock();

        DataStructure *data = atomic_load_explicit(&g_data, memory_order_relaxed);
        if (data) {
            sum += data->value;
        }

        rcu_read_unlock();
    }

    return (void*)(size_t)sum;
}

/* 写者 */
void* writer_thread(void *arg) {
    int iterations = *(int*)arg;

    for (int i = 0; i < iterations; i++) {
        pthread_mutex_lock(&rcu_write_lock);

        /* 读取旧数据 */
        DataStructure *old_data = atomic_load_explicit(&g_data, memory_order_relaxed);

        /* 复制并修改 */
        DataStructure *new_data = malloc(sizeof(DataStructure));
        if (old_data) {
            memcpy(new_data, old_data, sizeof(DataStructure));
        } else {
            memset(new_data, 0, sizeof(DataStructure));
        }
        new_data->value = i;

        /* 原子替换指针 */
        atomic_store_explicit(&g_data, new_data, memory_order_release);

        pthread_mutex_unlock(&rcu_write_lock);

        /* 等待所有读者完成 */
        rcu_synchronize();

        /* 安全释放旧数据 */
        if (old_data) {
            free(old_data);
        }
    }

    return NULL;
}

/**
 * @brief 线程亲和性设置
 */
void set_thread_affinity(pthread_t thread, int cpu) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
}

void benchmark_rcu(void) {
    printf("=== RCU Read-Mostly Benchmark ===\n\n");

    const int NUM_READERS = 7;
    const int NUM_WRITERS = 1;
    const int READ_ITERATIONS = 10000000;
    const int WRITE_ITERATIONS = 10000;

    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];

    /* 初始化数据 */
    DataStructure *initial = calloc(1, sizeof(DataStructure));
    atomic_store(&g_data, initial);

    clock_t start = clock();

    /* 启动读者 */
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, reader_thread,
                      (void*)&READ_ITERATIONS);
    }

    /* 启动写者 */
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_create(&writers[i], NULL, writer_thread,
                      (void*)&WRITE_ITERATIONS);
    }

    /* 等待完成 */
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    clock_t elapsed = clock() - start;

    printf("Total time: %ld ms\n", elapsed * 1000 / CLOCKS_PER_SEC);
    printf("Readers: %d x %d ops = %d reads\n",
           NUM_READERS, READ_ITERATIONS, NUM_READERS * READ_ITERATIONS);
    printf("Writers: %d x %d ops = %d writes\n",
           NUM_WRITERS, WRITE_ITERATIONS, NUM_WRITERS * WRITE_ITERATIONS);
    printf("Read throughput: %.2f M ops/sec\n",
           (NUM_READERS * READ_ITERATIONS / 1000000.0) /
           (elapsed / (double)CLOCKS_PER_SEC));

    /* 清理 */
    DataStructure *final = atomic_load(&g_data);
    free(final);
}

int main(void) {
    benchmark_rcu();
    return 0;
}
