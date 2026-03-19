/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\02_Intermediate_Learning_Path.md
 * Line: 219
 * Language: c
 * Block ID: dd950050
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

// 节点定义
typedef struct ListNode {
    void *data;
    struct ListNode *next;
    struct ListNode *prev;
} ListNode;

// 链表定义
typedef struct {
    ListNode *head;
    ListNode *tail;
    size_t size;
    size_t data_size;
    void (*free_fn)(void *);  // 自定义释放函数
} LinkedList;

// 生命周期
LinkedList* list_create(size_t data_size, void (*free_fn)(void *));
void list_destroy(LinkedList *list);

// 增删改查
int list_push_front(LinkedList *list, const void *data);
int list_push_back(LinkedList *list, const void *data);
int list_pop_front(LinkedList *list, void *out);
int list_pop_back(LinkedList *list, void *out);
ListNode* list_find(LinkedList *list, const void *data,
                    int (*cmp)(const void *, const void *));
int list_remove(LinkedList *list, ListNode *node);

// 高级操作
void list_sort(LinkedList *list, int (*cmp)(const void *, const void *));
LinkedList* list_reverse(LinkedList *list);
void list_foreach(LinkedList *list, void (*fn)(void *, void *), void *arg);

#endif
