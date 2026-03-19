/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\04_Concurrency_Patterns.md
 * Line: 766
 * Language: c
 * Block ID: 6084b269
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct Node {
    void *data;
    struct Node *next;
} Node;

typedef struct {
    _Atomic(Node*) top;
} LockFreeStack;

void push(LockFreeStack *stack, void *data) {
    Node *new_node = malloc(sizeof(Node));
    new_node->data = data;

    do {
        new_node->next = atomic_load(&stack->top);
    } while (!atomic_compare_exchange_weak(&stack->top,
                                           &new_node->next,
                                           new_node));
}

void *pop(LockFreeStack *stack) {
    Node *top;
    do {
        top = atomic_load(&stack->top);
        if (top == NULL) return NULL;
    } while (!atomic_compare_exchange_weak(&stack->top,
                                           &top,
                                           top->next));
    void *data = top->data;
    free(top);  // 注意：ABA问题
    return data;
}
