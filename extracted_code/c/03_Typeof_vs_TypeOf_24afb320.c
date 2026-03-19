/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 1262
 * Language: c
 * Block ID: 24afb320
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 使用 typeof 实现泛型栈

#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>

// 声明栈类型
#define DECLARE_STACK(T, NAME) \
    typedef struct { \
        T* data; \
        size_t size; \
        size_t capacity; \
    } NAME; \
    \
    static inline NAME NAME##_create(void) { \
        return (NAME){NULL, 0, 0}; \
    } \
    \
    static inline void NAME##_push(NAME* s, T value) { \
        if (s->size >= s->capacity) { \
            size_t new_cap = s->capacity == 0 ? 8 : s->capacity * 2; \
            s->data = realloc(s->data, new_cap * sizeof(T)); \
            s->capacity = new_cap; \
        } \
        s->data[s->size++] = value; \
    } \
    \
    static inline T NAME##_pop(NAME* s) { \
        assert(s->size > 0); \
        return s->data[--s->size]; \
    } \
    \
    static inline void NAME##_destroy(NAME* s) { \
        free(s->data); \
        s->data = NULL; \
        s->size = s->capacity = 0; \
    }

#endif

// 使用示例
DECLARE_STACK(int, IntStack)
DECLARE_STACK(double, DoubleStack)

void demo_c23_stack(void) {
    IntStack s = IntStack_create();
    IntStack_push(&s, 10);
    IntStack_push(&s, 20);
    int val = IntStack_pop(&s);
    printf("Popped: %d\n", val);
    IntStack_destroy(&s);
}
