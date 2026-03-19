/*
 * Auto-generated from: 03_System_Technology_Domains\07_Atomic_Operations.md
 * Line: 75
 * Language: c
 * Block ID: 44afcb34
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 无锁栈 (Treiber Stack)
typedef struct Node {
    void *data;
    _Atomic(struct Node *) next;
} Node;

typedef struct {
    _Atomic(Node *) head;
} LockFreeStack;

void push(LockFreeStack *stack, void *data) {
    Node *node = malloc(sizeof(Node));
    node->data = data;

    do {
        node->next = atomic_load(&stack->head);
    } while (!atomic_compare_exchange_weak(
        &stack->head, &node->next, node));
}

void *pop(LockFreeStack *stack) {
    Node *head;
    do {
        head = atomic_load(&stack->head);
        if (head == NULL) return NULL;
    } while (!atomic_compare_exchange_weak(
        &stack->head, &head, head->next));

    void *data = head->data;
    free(head);
    return data;
}
