/*
 * Auto-generated from: 12_Practice_Exercises\08_Concurrency_Parallelism_Labs.md
 * Line: 111
 * Language: c
 * Block ID: 7a69560d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct Node {
    void *data;
    _Atomic(struct Node*) next;
} Node;

typedef struct {
    _Atomic(Node*) head;
    _Atomic(Node*) tail;
    atomic_size_t size;
} LockFreeQueue;

LockFreeQueue* lfqueue_create(void);
void lfqueue_destroy(LockFreeQueue *q);

bool lfqueue_enqueue(LockFreeQueue *q, void *data);
bool lfqueue_dequeue(LockFreeQueue *q, void **data);
size_t lfqueue_size(const LockFreeQueue *q);
