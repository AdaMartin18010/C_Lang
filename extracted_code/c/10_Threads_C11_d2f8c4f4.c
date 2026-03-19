/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\10_Threads_C11.md
 * Line: 1779
 * Language: c
 * Block ID: d2f8c4f4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define _GNU_SOURCE
#include <stdatomic.h>
#include <stdio.h>
#include <threads.h>
#include <stdbool.h>

/* 内存序说明表
 *
 * memory_order_relaxed - 无同步保证，仅原子性
 * memory_order_consume - 数据依赖同步 ( rarely used )
 * memory_order_acquire - 读操作，后续操作不会重排到前面
 * memory_order_release - 写操作，前面操作不会重排到后面
 * memory_order_acq_rel - 读+写操作，同时有acquire和release语义
 * memory_order_seq_cst - 顺序一致性，最强的保证
 */

/* 基本原子操作示例 */
void basic_atomic_operations(void) {
    atomic_int counter = ATOMIC_VAR_INIT(0);

    /* 原子加 */
    atomic_fetch_add(&counter, 1);

    /* 显式内存序 */
    atomic_fetch_add_explicit(&counter, 1, memory_order_relaxed);

    /* 原子交换 */
    int old = atomic_exchange(&counter, 100);
    printf("Old value: %d\n", old);

    /* 比较交换 (CAS) */
    int expected = 100;
    bool success = atomic_compare_exchange_strong(
        &counter, &expected, 200);
    printf("CAS %s, counter=%d\n",
           success ? "succeeded" : "failed",
           atomic_load(&counter));
}

/* 自旋锁实现 */
typedef struct {
    atomic_flag locked;
} SpinLock;

void spinlock_init(SpinLock* lock) {
    atomic_flag_clear(&lock->locked);
}

void spinlock_lock(SpinLock* lock) {
    /* 测试并设置循环 */
    while (atomic_flag_test_and_set(&lock->locked)) {
        /* 自旋等待 */
        thrd_yield();
    }
}

void spinlock_unlock(SpinLock* lock) {
    atomic_flag_clear(&lock->locked);
}

/* 引用计数实现 */
typedef struct {
    void* data;
    atomic_int ref_count;
} RefCounted;

RefCounted* ref_create(void* data) {
    RefCounted* rc = malloc(sizeof(RefCounted));
    rc->data = data;
    atomic_init(&rc->ref_count, 1);
    return rc;
}

void ref_acquire(RefCounted* rc) {
    atomic_fetch_add(&rc->ref_count, 1);
}

void ref_release(RefCounted* rc) {
    if (atomic_fetch_sub(&rc->ref_count, 1) == 1) {
        /* 最后一个引用，释放 */
        free(rc->data);
        free(rc);
    }
}

/* 无锁栈实现 */
typedef struct StackNode {
    void* data;
    struct StackNode* next;
} StackNode;

typedef struct {
    _Atomic(StackNode*) top;
} LockFreeStack;

void lfstack_init(LockFreeStack* stack) {
    atomic_init(&stack->top, NULL);
}

void lfstack_push(LockFreeStack* stack, void* data) {
    StackNode* new_node = malloc(sizeof(StackNode));
    new_node->data = data;

    do {
        new_node->next = atomic_load(&stack->top);
    } while (!atomic_compare_exchange_weak(
        &stack->top, &new_node->next, new_node));
}

bool lfstack_pop(LockFreeStack* stack, void** data) {
    StackNode* top;
    do {
        top = atomic_load(&stack->top);
        if (top == NULL) return false;
    } while (!atomic_compare_exchange_weak(
        &stack->top, &top, top->next));

    *data = top->data;
    free(top);
    return true;
}

/* 单例模式 - 双重检查锁定 */
typedef struct {
    int value;
} Singleton;

static _Atomic(Singleton*) g_instance = NULL;
static SpinLock g_instance_lock;

Singleton* get_instance(void) {
    Singleton* instance = atomic_load_explicit(&g_instance,
                                                memory_order_acquire);
    if (instance == NULL) {
        spinlock_lock(&g_instance_lock);
        instance = atomic_load_explicit(&g_instance, memory_order_relaxed);
        if (instance == NULL) {
            instance = malloc(sizeof(Singleton));
            instance->value = 42;
            atomic_store_explicit(&g_instance, instance,
                                  memory_order_release);
        }
        spinlock_unlock(&g_instance_lock);
    }
    return instance;
}

int main(void) {
    printf("=== Atomic Operations Demo ===\n");
    basic_atomic_operations();
    return 0;
}
