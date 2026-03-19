/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\10_Threads_C11.md
 * Line: 1214
 * Language: c
 * Block ID: 3b7f1c56
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define _GNU_SOURCE
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <threads.h>
#include <string.h>

/* 无锁队列节点 */
typedef struct Node {
    _Atomic(void*) data;
    _Atomic(struct Node*) next;
} Node;

/* 无锁队列结构 */
typedef struct {
    _Atomic(Node*) head;
    _Atomic(Node*) tail;
    atomic_size_t size;
} LockFreeQueue;

/* 初始化队列 */
void lfqueue_init(LockFreeQueue* q) {
    Node* dummy = malloc(sizeof(Node));
    atomic_init(&dummy->data, NULL);
    atomic_init(&dummy->next, NULL);

    atomic_init(&q->head, dummy);
    atomic_init(&q->tail, dummy);
    atomic_init(&q->size, 0);
}

/* 入队 - 无锁实现 */
bool lfqueue_push(LockFreeQueue* q, void* data) {
    Node* new_node = malloc(sizeof(Node));
    if (!new_node) return false;

    atomic_init(&new_node->data, data);
    atomic_init(&new_node->next, NULL);

    Node* tail;
    Node* next;

    while (true) {
        tail = atomic_load_explicit(&q->tail, memory_order_acquire);
        next = atomic_load_explicit(&tail->next, memory_order_acquire);

        /* 再次检查tail是否仍然是最新的 */
        if (tail == atomic_load_explicit(&q->tail, memory_order_acquire)) {
            if (next == NULL) {
                /* 尝试链接新节点 */
                if (atomic_compare_exchange_weak_explicit(
                        &tail->next, &next, new_node,
                        memory_order_release, memory_order_relaxed)) {
                    break; /* 成功 */
                }
            } else {
                /* 帮助推进tail指针 */
                atomic_compare_exchange_weak_explicit(
                    &q->tail, &tail, next,
                    memory_order_release, memory_order_relaxed);
            }
        }
    }

    /* 尝试更新tail指针 */
    atomic_compare_exchange_weak_explicit(
        &q->tail, &tail, new_node,
        memory_order_release, memory_order_relaxed);

    atomic_fetch_add_explicit(&q->size, 1, memory_order_relaxed);
    return true;
}

/* 出队 - 无锁实现 */
bool lfqueue_pop(LockFreeQueue* q, void** data) {
    Node* head;
    Node* tail;
    Node* next;

    while (true) {
        head = atomic_load_explicit(&q->head, memory_order_acquire);
        tail = atomic_load_explicit(&q->tail, memory_order_acquire);
        next = atomic_load_explicit(&head->next, memory_order_acquire);

        if (head == atomic_load_explicit(&q->head, memory_order_acquire)) {
            if (head == tail) {
                if (next == NULL) {
                    return false; /* 队列为空 */
                }
                /* 帮助推进tail */
                atomic_compare_exchange_weak_explicit(
                    &q->tail, &tail, next,
                    memory_order_release, memory_order_relaxed);
            } else {
                *data = atomic_load_explicit(&next->data, memory_order_relaxed);

                /* 尝试更新head */
                if (atomic_compare_exchange_weak_explicit(
                        &q->head, &head, next,
                        memory_order_release, memory_order_relaxed)) {
                    break; /* 成功 */
                }
            }
        }
    }

    /* 安全释放旧head节点 */
    free(head);
    atomic_fetch_sub_explicit(&q->size, 1, memory_order_relaxed);
    return true;
}

/* 获取队列大小 */
size_t lfqueue_size(LockFreeQueue* q) {
    return atomic_load_explicit(&q->size, memory_order_relaxed);
}

/* 测试代码 */
typedef struct {
    int value;
} TestData;

static LockFreeQueue g_queue;
static atomic_int g_push_count = 0;
static atomic_int g_pop_count = 0;

int producer_lf(void *arg) {
    int count = *(int*)arg;
    for (int i = 0; i < count; i++) {
        TestData* data = malloc(sizeof(TestData));
        data->value = i;
        while (!lfqueue_push(&g_queue, data)) {
            thrd_yield();
        }
        atomic_fetch_add(&g_push_count, 1);
    }
    return 0;
}

int consumer_lf(void *arg) {
    int count = *(int*)arg;
    int consumed = 0;
    while (consumed < count) {
        void* data;
        if (lfqueue_pop(&g_queue, &data)) {
            TestData* td = (TestData*)data;
            free(td);
            consumed++;
            atomic_fetch_add(&g_pop_count, 1);
        } else {
            thrd_yield();
        }
    }
    return 0;
}

int main(void) {
    printf("=== Lock-Free Queue Demo ===\n");

    lfqueue_init(&g_queue);

    const int ITEMS = 10000;
    const int PRODUCERS = 2;
    const int CONSUMERS = 2;

    thrd_t producers[PRODUCERS];
    thrd_t consumers[CONSUMERS];

    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    for (int i = 0; i < CONSUMERS; i++) {
        thrd_create(&consumers[i], consumer_lf, (void*)&ITEMS);
    }
    for (int i = 0; i < PRODUCERS; i++) {
        thrd_create(&producers[i], producer_lf, (void*)&ITEMS);
    }

    for (int i = 0; i < PRODUCERS; i++) {
        thrd_join(producers[i], NULL);
    }
    for (int i = 0; i < CONSUMERS; i++) {
        thrd_join(consumers[i], NULL);
    }

    timespec_get(&end, TIME_UTC);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Pushed: %d, Popped: %d\n",
           atomic_load(&g_push_count),
           atomic_load(&g_pop_count));
    printf("Time: %.3f seconds\n", elapsed);
    printf("Ops/sec: %.0f\n", (2.0 * ITEMS * PRODUCERS) / elapsed);

    return 0;
}
