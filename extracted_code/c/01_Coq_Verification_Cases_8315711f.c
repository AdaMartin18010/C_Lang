/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_Coq_Examples\01_Coq_Verification_Cases.md
 * Line: 1876
 * Language: c
 * Block ID: 8315711f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* File: c_impl/stack_queue.c */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

/* ============================================================ */
/* 1. 栈实现 */
/* ============================================================ */

typedef struct StackNode {
    int data;
    struct StackNode* next;
} StackNode;

typedef struct {
    StackNode* top;
    int size;
} Stack;

Stack* stack_create() {
    Stack* s = (Stack*)malloc(sizeof(Stack));
    s->top = NULL;
    s->size = 0;
    return s;
}

void stack_push(Stack* s, int x) {
    StackNode* node = (StackNode*)malloc(sizeof(StackNode));
    node->data = x;
    node->next = s->top;
    s->top = node;
    s->size++;
}

int stack_pop(Stack* s, int* out) {
    if (s->top == NULL) return 0;
    StackNode* temp = s->top;
    *out = temp->data;
    s->top = temp->next;
    free(temp);
    s->size--;
    return 1;
}

int stack_peek(Stack* s, int* out) {
    if (s->top == NULL) return 0;
    *out = s->top->data;
    return 1;
}

bool stack_is_empty(Stack* s) {
    return s->top == NULL;
}

void stack_destroy(Stack* s) {
    int dummy;
    while (!stack_is_empty(s)) {
        stack_pop(s, &dummy);
    }
    free(s);
}

/* ============================================================ */
/* 2. 队列实现（双列表法） */
/* ============================================================ */

typedef struct {
    int* front;
    int front_size;
    int front_capacity;
    int* back;
    int back_size;
    int back_capacity;
} Queue;

Queue* queue_create() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = NULL;
    q->front_size = 0;
    q->front_capacity = 0;
    q->back = NULL;
    q->back_size = 0;
    q->back_capacity = 0;
    return q;
}

void queue_enqueue(Queue* q, int x) {
    if (q->back_size >= q->back_capacity) {
        q->back_capacity = q->back_capacity == 0 ? 4 : q->back_capacity * 2;
        q->back = (int*)realloc(q->back, q->back_capacity * sizeof(int));
    }
    q->back[q->back_size++] = x;
}

static void queue_normalize(Queue* q) {
    if (q->front_size == 0 && q->back_size > 0) {
        /* 将back反转到front */
        q->front_capacity = q->back_size;
        q->front = (int*)realloc(q->front, q->front_capacity * sizeof(int));
        for (int i = 0; i < q->back_size; i++) {
            q->front[i] = q->back[q->back_size - 1 - i];
        }
        q->front_size = q->back_size;
        q->back_size = 0;
    }
}

int queue_dequeue(Queue* q, int* out) {
    queue_normalize(q);
    if (q->front_size == 0) return 0;
    *out = q->front[--q->front_size];
    return 1;
}

int queue_front(Queue* q, int* out) {
    queue_normalize(q);
    if (q->front_size == 0) return 0;
    *out = q->front[q->front_size - 1];
    return 1;
}

bool queue_is_empty(Queue* q) {
    queue_normalize(q);
    return q->front_size == 0;
}

void queue_destroy(Queue* q) {
    free(q->front);
    free(q->back);
    free(q);
}

/* ============================================================ */
/* 3. 测试函数 */
/* ============================================================ */

void test_stack() {
    Stack* s = stack_create();
    assert(stack_is_empty(s));

    stack_push(s, 10);
    stack_push(s, 20);
    stack_push(s, 30);

    int val;
    assert(stack_pop(s, &val) && val == 30);
    assert(stack_pop(s, &val) && val == 20);
    assert(stack_peek(s, &val) && val == 10);
    assert(stack_pop(s, &val) && val == 10);
    assert(!stack_pop(s, &val));

    stack_destroy(s);
    printf("Stack tests passed!\n");
}

void test_queue() {
    Queue* q = queue_create();
    assert(queue_is_empty(q));

    queue_enqueue(q, 10);
    queue_enqueue(q, 20);
    queue_enqueue(q, 30);

    int val;
    assert(queue_dequeue(q, &val) && val == 10);
    assert(queue_dequeue(q, &val) && val == 20);
    assert(queue_front(q, &val) && val == 30);
    assert(queue_dequeue(q, &val) && val == 30);
    assert(!queue_dequeue(q, &val));

    queue_destroy(q);
    printf("Queue tests passed!\n");
}

/* FIFO性质测试 */
void test_fifo_property() {
    Queue* q = queue_create();
    int inputs[] = {1, 2, 3, 4, 5};
    int n = 5;
    int outputs[5];

    for (int i = 0; i < n; i++) {
        queue_enqueue(q, inputs[i]);
    }

    for (int i = 0; i < n; i++) {
        assert(queue_dequeue(q, &outputs[i]));
    }

    /* 验证FIFO顺序 */
    for (int i = 0; i < n; i++) {
        assert(inputs[i] == outputs[i]);
    }

    queue_destroy(q);
    printf("FIFO property test passed!\n");
}
