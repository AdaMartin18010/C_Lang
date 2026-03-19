/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 896
 * Language: c
 * Block ID: 1ffe5e40
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 Michael-Scott 无锁队列
typedef struct Node {
    void* data;
    _Atomic(struct Node*) next;
} Node;

typedef struct {
    _Atomic(Node*) head;
    _Atomic(Node*) tail;
} LockFreeQueue;

void queue_enqueue(LockFreeQueue* q, void* data) {
    Node* node = malloc(sizeof(Node));
    node->data = data;
    atomic_init(&node->next, NULL);

    while (true) {
        Node* tail = atomic_load_explicit(&q->tail, memory_order_acquire);
        Node* next = atomic_load_explicit(&tail->next, memory_order_acquire);

        if (tail == atomic_load_explicit(&q->tail, memory_order_acquire)) {
            if (next == NULL) {
                if (atomic_compare_exchange_weak_explicit(
                        &tail->next, &next, node,
                        memory_order_release, memory_order_relaxed)) {
                    atomic_compare_exchange_weak_explicit(
                        &q->tail, &tail, node,
                        memory_order_release, memory_order_relaxed);
                    return;
                }
            }
        }
    }
}
