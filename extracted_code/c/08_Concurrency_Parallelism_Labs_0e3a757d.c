/*
 * Auto-generated from: 12_Practice_Exercises\08_Concurrency_Parallelism_Labs.md
 * Line: 133
 * Language: c
 * Block ID: 0e3a757d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    _Atomic(Node*) top;
    atomic_size_t size;
} LockFreeStack;

void lfstack_push(LockFreeStack *s, void *data);
bool lfstack_pop(LockFreeStack *s, void **data);
