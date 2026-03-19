/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\10_Threads_C11.md
 * Line: 444
 * Language: c
 * Block ID: 23343b10
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>
#include <errno.h>

/* ========== 1. 普通互斥锁 ========== */
typedef struct {
    mtx_t mutex;
    int data;
} PlainProtectedData;

int plain_mutex_demo(void) {
    PlainProtectedData shared = {0};

    /* 初始化普通锁 */
    int rc = mtx_init(&shared.mutex, mtx_plain);
    if (rc != thrd_success) {
        fprintf(stderr, "mtx_init failed: %d\n", rc);
        return -1;
    }

    /* 基本加锁/解锁 */
    mtx_lock(&shared.mutex);
    shared.data = 42;
    mtx_unlock(&shared.mutex);

    /* 尝试加锁（非阻塞） */
    if (mtx_trylock(&shared.mutex) == thrd_success) {
        printf("Got lock immediately\n");
        mtx_unlock(&shared.mutex);
    } else {
        printf("Lock is busy\n");
    }

    mtx_destroy(&shared.mutex);
    return 0;
}

/* ========== 2. 递归互斥锁 ========== */
typedef struct {
    mtx_t mutex;
    int depth;
    int value;
} RecursiveProtectedData;

static RecursiveProtectedData g_recursive_data = {0};

/* 递归函数需要使用递归锁 */
void recursive_operation(int depth) {
    if (depth <= 0) return;

    /* 同一线程可重复加锁 */
    mtx_lock(&g_recursive_data.mutex);

    g_recursive_data.depth++;
    printf("  [Depth %d] Processing...\n", g_recursive_data.depth);

    /* 递归调用 - 再次获取同一把锁 */
    recursive_operation(depth - 1);

    printf("  [Depth %d] Done\n", g_recursive_data.depth);
    g_recursive_data.depth--;

    mtx_unlock(&g_recursive_data.mutex);
}

int recursive_mutex_demo(void) {
    /* 初始化递归锁 */
    int rc = mtx_init(&g_recursive_data.mutex, mtx_recursive);
    if (rc != thrd_success) {
        fprintf(stderr, "Failed to init recursive mutex: %d\n", rc);
        return -1;
    }

    printf("=== Recursive Mutex Demo ===\n");
    recursive_operation(3);

    mtx_destroy(&g_recursive_data.mutex);
    return 0;
}

/* ========== 3. 定时互斥锁 ========== */
typedef struct {
    mtx_t mutex;
    int resource;
} TimedProtectedData;

static TimedProtectedData g_timed_data = {0};

/* 持有锁很长时间的线程 */
int slow_holding_thread(void *arg) {
    (void)arg;

    mtx_lock(&g_timed_data.mutex);
    printf("[Slow] Acquired lock, holding for 2 seconds...\n");

    thrd_sleep(&(struct timespec){.tv_sec = 2}, NULL);

    printf("[Slow] Releasing lock\n");
    mtx_unlock(&g_timed_data.mutex);
    return 0;
}

/* 带超时的线程 */
int impatient_thread(void *arg) {
    int id = *(int*)arg;

    /* 计算超时时间点 */
    struct timespec deadline;
    timespec_get(&deadline, TIME_UTC);
    deadline.tv_sec += 1;  /* 最多等待1秒 */

    printf("[Thread %d] Trying to acquire lock with 1s timeout...\n", id);

    int rc = mtx_timedlock(&g_timed_data.mutex, &deadline);
    if (rc == thrd_success) {
        printf("[Thread %d] Got lock! Resource=%d\n", id, g_timed_data.resource);
        mtx_unlock(&g_timed_data.mutex);
    } else if (rc == thrd_timeout) {
        printf("[Thread %d] Timeout! Could not get lock in time\n", id);
    } else {
        printf("[Thread %d] Error: %d\n", id, rc);
    }
    return rc;
}

int timed_mutex_demo(void) {
    /* 初始化定时锁 */
    int rc = mtx_init(&g_timed_data.mutex, mtx_timed | mtx_plain);
    if (rc != thrd_success) {
        fprintf(stderr, "Failed to init timed mutex: %d\n", rc);
        return -1;
    }

    g_timed_data.resource = 999;

    printf("\n=== Timed Mutex Demo ===\n");

    thrd_t slow, fast1, fast2;
    thrd_create(&slow, slow_holding_thread, NULL);

    /* 让慢线程先拿到锁 */
    thrd_sleep(&(struct timespec){.tv_nsec = 100000000}, NULL);

    int id1 = 1, id2 = 2;
    thrd_create(&fast1, impatient_thread, &id1);
    thrd_create(&fast2, impatient_thread, &id2);

    thrd_join(slow, NULL);
    thrd_join(fast1, NULL);
    thrd_join(fast2, NULL);

    mtx_destroy(&g_timed_data.mutex);
    return 0;
}

/* ========== 4. 死锁检测与避免 ========== */

/* 锁的顺序层次表 - 必须按从低到高获取 */
typedef enum {
    LOCK_LEVEL_CACHE = 1,
    LOCK_LEVEL_DATABASE = 2,
    LOCK_LEVEL_FILESYSTEM = 3,
    LOCK_LEVEL_NETWORK = 4
} LockLevel;

typedef struct {
    mtx_t mutex;
    LockLevel level;
    const char* name;
} HierarchicalMutex;

static _Thread_local LockLevel g_current_lock_level = 0;

void hierarchical_lock(HierarchicalMutex* m) {
    if (m->level <= g_current_lock_level) {
        fprintf(stderr, "DEADLOCK VIOLATION: Trying to acquire '%s' (level %d) "
                "while holding lock at level %d\n",
                m->name, m->level, g_current_lock_level);
        abort();
    }

    mtx_lock(&m->mutex);
    g_current_lock_level = m->level;
    printf("[Lock] Acquired '%s' at level %d\n", m->name, m->level);
}

void hierarchical_unlock(HierarchicalMutex* m) {
    mtx_unlock(&m->mutex);
    g_current_lock_level = 0;  /* 简化处理 */
    printf("[Lock] Released '%s'\n", m->name);
}

/* ========== 性能测试 ========== */
typedef struct {
    mtx_t mutex;
    volatile long long counter;
} PerfData;

static PerfData g_perf_data = {0};

int perf_worker(void *arg) {
    int iterations = *(int*)arg;

    for (int i = 0; i < iterations; i++) {
        mtx_lock(&g_perf_data.mutex);
        g_perf_data.counter++;
        mtx_unlock(&g_perf_data.mutex);
    }
    return 0;
}

void performance_comparison(void) {
    printf("\n=== Mutex Performance Test ===\n");

    const int ITERATIONS = 100000;
    const int NUM_THREADS = 4;

    mtx_init(&g_perf_data.mutex, mtx_plain);
    g_perf_data.counter = 0;

    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    thrd_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        thrd_create(&threads[i], perf_worker, (void*)&ITERATIONS);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        thrd_join(threads[i], NULL);
    }

    timespec_get(&end, TIME_UTC);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;

    long long total_ops = (long long)ITERATIONS * NUM_THREADS;
    printf("Total operations: %lld\n", total_ops);
    printf("Final counter: %lld\n", g_perf_data.counter);
    printf("Time elapsed: %.3f seconds\n", elapsed);
    printf("Ops/second: %.0f\n", total_ops / elapsed);

    mtx_destroy(&g_perf_data.mutex);
}

/* 主函数 */
int main(void) {
    plain_mutex_demo();
    recursive_mutex_demo();
    timed_mutex_demo();
    performance_comparison();
    return 0;
}
