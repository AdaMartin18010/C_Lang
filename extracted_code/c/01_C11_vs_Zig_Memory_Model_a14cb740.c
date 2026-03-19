/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
 * Line: 1340
 * Language: c
 * Block ID: a14cb740
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 无锁队列 (Michael-Scott 算法简化版)
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct Node {
    void* data;
    _Atomic(struct Node*) next;
} Node;

typedef struct {
    _Atomic(Node*) head;
    _Atomic(Node*) tail;
} LockFreeQueue;

void queue_init(LockFreeQueue* q) {
    Node* dummy = malloc(sizeof(Node));
    dummy->data = NULL;
    atomic_init(&dummy->next, NULL);
    atomic_init(&q->head, dummy);
    atomic_init(&q->tail, dummy);
}

void queue_enqueue(LockFreeQueue* q, void* data) {
    Node* node = malloc(sizeof(Node));
    node->data = data;
    atomic_init(&node->next, NULL);

    Node* tail;
    Node* next;

    for (;;) {
        tail = atomic_load_explicit(&q->tail, memory_order_acquire);
        next = atomic_load_explicit(&tail->next, memory_order_acquire);

        // 检查 tail 是否仍然有效
        if (tail == atomic_load_explicit(&q->tail, memory_order_acquire)) {
            if (next == NULL) {
                // 尝试链接新节点
                if (atomic_compare_exchange_weak_explicit(
                    &tail->next,
                    &next,
                    node,
                    memory_order_release,
                    memory_order_relaxed)) {
                    break;  // 成功
                }
            } else {
                // 帮助移动 tail
                atomic_compare_exchange_weak_explicit(
                    &q->tail,
                    &tail,
                    next,
                    memory_order_release,
                    memory_order_relaxed
                );
            }
        }
    }

    // 尝试移动 tail
    atomic_compare_exchange_weak_explicit(
        &q->tail,
        &tail,
        node,
        memory_order_release,
        memory_order_relaxed
    );
}

bool queue_dequeue(LockFreeQueue* q, void** data) {
    Node* head;
    Node* tail;
    Node* next;

    for (;;) {
        head = atomic_load_explicit(&q->head, memory_order_acquire);
        tail = atomic_load_explicit(&q->tail, memory_order_acquire);
        next = atomic_load_explicit(&head->next, memory_order_acquire);

        if (head == atomic_load_explicit(&q->head, memory_order_acquire)) {
            if (head == tail) {
                if (next == NULL) {
                    return false;  // 队列为空
                }
                // 帮助移动 tail
                atomic_compare_exchange_weak_explicit(
                    &q->tail,
                    &tail,
                    next,
                    memory_order_release,
                    memory_order_relaxed
                );
            } else {
                *data = next->data;
                if (atomic_compare_exchange_weak_explicit(
                    &q->head,
                    &head,
                    next,
                    memory_order_release,
                    memory_order_relaxed)) {
                    break;  // 成功
                }
            }
        }
    }

    free(head);  // 释放旧 dummy 节点
    return true;
}
