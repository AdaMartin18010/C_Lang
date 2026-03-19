/*
 * Auto-generated from: 01_Core_Knowledge_System\03_Construction_Layer\03_Modularization_Linking.md
 * Line: 130
 * Language: c
 * Block ID: 0d058d44
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// === 接口：list.h ===
#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdbool.h>

typedef struct List List;
typedef struct ListNode ListNode;

// 构造函数
List *list_new(void);
void list_free(List *list);

// 操作
bool list_append(List *list, void *data);
bool list_prepend(List *list, void *data);
size_t list_length(const List *list);

// 迭代器
typedef struct {
    ListNode *current;
} ListIter;

ListIter list_iter_begin(const List *list);
void *list_iter_next(ListIter *iter);
bool list_iter_end(const ListIter *iter);

#endif

// === 实现：list.c ===
#include "list.h"
#include <stdlib.h>

struct ListNode {
    void *data;
    ListNode *next;
};

struct List {
    ListNode *head;
    ListNode *tail;
    size_t length;
};

List *list_new(void) {
    return calloc(1, sizeof(List));
}

void list_free(List *list) {
    if (!list) return;
    ListNode *current = list->head;
    while (current) {
        ListNode *next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

bool list_append(List *list, void *data) {
    if (!list) return false;
    ListNode *node = malloc(sizeof(ListNode));
    if (!node) return false;
    node->data = data;
    node->next = NULL;

    if (list->tail) {
        list->tail->next = node;
    } else {
        list->head = node;
    }
    list->tail = node;
    list->length++;
    return true;
}

// ... 其他实现
