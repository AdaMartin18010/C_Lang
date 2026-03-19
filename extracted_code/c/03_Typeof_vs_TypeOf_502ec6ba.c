/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 2394
 * Language: c
 * Block ID: 502ec6ba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 泛型链表实现

#ifndef GENERIC_LIST_H
#define GENERIC_LIST_H

#include <stdlib.h>
#include <stdbool.h>

// 声明链表节点和链表类型
#define DECLARE_LIST(T, NAME) \
    typedef struct NAME##Node { \
        T data; \
        struct NAME##Node* next; \
    } NAME##Node; \
    \
    typedef struct { \
        NAME##Node* head; \
        NAME##Node* tail; \
        size_t size; \
    } NAME; \
    \
    static inline NAME NAME##_create(void) { \
        return (NAME){NULL, NULL, 0}; \
    } \
    \
    static inline void NAME##_push_front(NAME* list, T value) { \
        NAME##Node* node = malloc(sizeof(NAME##Node)); \
        node->data = value; \
        node->next = list->head; \
        list->head = node; \
        if (!list->tail) list->tail = node; \
        list->size++; \
    } \
    \
    static inline bool NAME##_pop_front(NAME* list, T* out) { \
        if (!list->head) return false; \
        NAME##Node* node = list->head; \
        *out = node->data; \
        list->head = node->next; \
        if (!list->head) list->tail = NULL; \
        free(node); \
        list->size--; \
        return true; \
    } \
    \
    static inline void NAME##_destroy(NAME* list) { \
        T dummy; \
        while (NAME##_pop_front(list, &dummy)) {} \
    }

#endif

// 使用
DECLARE_LIST(int, IntList)
DECLARE_LIST(double, DoubleList)

void demoC23List(void) {
    IntList list = IntList_create();
    IntList_push_front(&list, 10);
    IntList_push_front(&list, 20);

    int val;
    while (IntList_pop_front(&list, &val)) {
        printf("%d\n", val);
    }

    IntList_destroy(&list);
}
