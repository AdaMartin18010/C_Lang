/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\README.md
 * Line: 378
 * Language: c
 * Block ID: 9f5fefe7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 无锁队列节点 */
typedef struct node {
    atomic_int value;
    _Atomic(struct node *)next;
} node_t;

typedef struct {
    _Atomic(node_t *)head;
    _Atomic(node_t *)tail;
} lockfree_queue_t;

void lfqueue_init(lockfree_queue_t *q) {
    node_t *dummy = malloc(sizeof(node_t));
    atomic_store(&dummy->next, NULL);
    atomic_store(&q->head, dummy);
    atomic_store(&q->tail, dummy);
}

void lfqueue_enqueue(lockfree_queue_t *q, int value) {
    node_t *new_node = malloc(sizeof(node_t));
    atomic_store(&new_node->value, value);
    atomic_store(&new_node->next, NULL);

    node_t *tail;
    while (1) {
        tail = atomic_load(&q->tail);
        node_t *next = atomic_load(&tail->next);

        if (tail == atomic_load(&q->tail)) {
            if (next == NULL) {
                if (atomic_compare_exchange_weak(&tail->next, &next, new_node)) {
                    break;
                }
            } else {
                atomic_compare_exchange_weak(&q->tail, &tail, next);
            }
        }
    }
    atomic_compare_exchange_weak(&q->tail, &tail, new_node);
}
