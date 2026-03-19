/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\02_Synchronization_Matrix.md
 * Line: 912
 * Language: c
 * Block ID: d8feab81
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// c11_atomics.c
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#define NUM_THREADS 4
#define ITERATIONS 10000000

// 原子计数器
atomic_int atomic_counter = 0;
atomic_long atomic_sum = 0;

// 内存序说明:
// memory_order_relaxed - 无同步或顺序约束
// memory_order_consume - 消费操作，数据依赖同步
// memory_order_acquire - 获取操作，用于加载
// memory_order_release - 释放操作，用于存储
// memory_order_acq_rel - 获取-释放，用于读-修改-写
// memory_order_seq_cst - 顺序一致，最强约束(默认)

// 基本原子操作
int atomic_worker(void *arg) {
    (void)arg;

    for (int i = 0; i < ITERATIONS; i++) {
        // 原子自增 (seq_cst)
        atomic_fetch_add(&atomic_counter, 1);

        // 原子自增 (relaxed)
        atomic_fetch_add_explicit(&atomic_sum, i, memory_order_relaxed);
    }

    return 0;
}

// 原子标志 (类似自旋锁)
atomic_flag spinlock = ATOMIC_FLAG_INIT;
int protected_data = 0;

void atomic_lock(void) {
    // 测试并设置，忙等待
    while (atomic_flag_test_and_set(&spinlock)) {
        // 可选: 让出CPU
        thrd_yield();
    }
}

void atomic_unlock(void) {
    atomic_flag_clear(&spinlock);
}

int spinlock_worker(void *arg) {
    (void)arg;

    for (int i = 0; i < ITERATIONS; i++) {
        atomic_lock();
        protected_data++;
        atomic_unlock();
    }

    return 0;
}

// 原子指针操作
_Atomic(void*) atomic_ptr = NULL;

void pointer_example(void) {
    int data = 42;

    // 原子存储指针
    atomic_store(&atomic_ptr, &data);

    // 原子加载指针
    int *p = atomic_load(&atomic_ptr);

    // 比较并交换
    int new_data = 100;
    int *expected = &data;
    atomic_compare_exchange_strong(&atomic_ptr, &expected, &new_data);
}

// 原子布尔
atomic_bool ready = ATOMIC_VAR_INIT(false);
atomic_int value = ATOMIC_VAR_INIT(0);

// 发布-订阅模式
void producer_thread(void) {
    value = 42;
    // 确保value写入在ready之前对其他线程可见
    atomic_store_explicit(&ready, true, memory_order_release);
}

void consumer_thread(void) {
    // 确保读取ready后再读取value
    while (!atomic_load_explicit(&ready, memory_order_acquire)) {
        thrd_yield();
    }
    printf("Value: %d\n", value);
}

// 原子浮点 (使用union实现)
typedef union {
    _Atomic(uint64_t) i;
    double d;
} atomic_double_t;

void atomic_double_store(atomic_double_t *ad, double val) {
    union { double d; uint64_t i; } u = {.d = val};
    atomic_store(&ad->i, u.i);
}

double atomic_double_load(atomic_double_t *ad) {
    uint64_t i = atomic_load(&ad->i);
    union { uint64_t i; double d; } u = {.i = i};
    return u.d;
}

int main(void) {
    thrd_t threads[NUM_THREADS];

    printf("=== C11 原子操作演示 ===\n");

    // 基本原子操作测试
    printf("\n1. 基本原子自增\n");
    atomic_counter = 0;
    atomic_sum = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        thrd_create(&threads[i], atomic_worker, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        thrd_join(threads[i], NULL);
    }

    printf("Atomic counter: %d (expected: %d)\n",
           atomic_load(&atomic_counter), NUM_THREADS * ITERATIONS);

    // 原子标志锁测试
    printf("\n2. 原子自旋锁\n");
    protected_data = 0;
    atomic_flag_clear(&spinlock);

    for (int i = 0; i < NUM_THREADS; i++) {
        thrd_create(&threads[i], spinlock_worker, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        thrd_join(threads[i], NULL);
    }

    printf("Protected data: %d (expected: %d)\n",
           protected_data, NUM_THREADS * ITERATIONS);

    printf("\nC11原子操作优势:\n");
    printf("- 标准可移植\n");
    printf("- 细粒度内存序控制\n");
    printf("- 无锁编程基础\n");
    printf("- 编译器可优化\n");

    return 0;
}
