/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 1004
 * Language: c
 * Block ID: c8e3412d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// stack.h - 只暴露接口，隐藏实现
typedef struct Stack Stack;

Stack* stack_create(int capacity);
void stack_destroy(Stack* stack);
int stack_push(Stack* stack, int value);
int stack_pop(Stack* stack, int* value);
int stack_peek(const Stack* stack, int* value);
int stack_size(const Stack* stack);
int stack_is_empty(const Stack* stack);

// stack.c - 实现细节对其他文件不可见
#include "stack.h"
#include <stdlib.h>

struct Stack {
    int* data;
    int top;
    int capacity;
};

Stack* stack_create(int capacity) {
    Stack* s = malloc(sizeof(Stack));
    s->data = malloc(capacity * sizeof(int));
    s->top = -1;
    s->capacity = capacity;
    return s;
}
// ... 其他实现
