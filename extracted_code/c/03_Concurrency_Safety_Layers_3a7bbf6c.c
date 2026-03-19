/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\03_Concurrency_Safety_Layers.md
 * Line: 339
 * Language: c
 * Block ID: 3a7bbf6c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct Node {
    void *data;
    _Atomic(struct Node *) next;
} Node;

typedef struct {
    _Atomic(Node *) head;
} LockFreeStack;

void push(LockFreeStack *stack, Node *node) {
    Node *old_head;
    do {
        old_head = atomic_load(&stack->head);
        atomic_store(&node->next, old_head);
    } while (!atomic_compare_exchange_weak(
        &stack->head, &old_head, node));
}

Node* pop(LockFreeStack *stack) {
    Node *old_head;
    do {
        old_head = atomic_load(&stack->head);
        if (old_head == NULL) return NULL;
        Node *new_head = atomic_load(&old_head->next);
    } while (!atomic_compare_exchange_weak(
        &stack->head, &old_head, new_head));
    return old_head;
}
