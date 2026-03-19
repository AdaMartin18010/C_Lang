/*
 * Auto-generated from: 12_Practice_Exercises\02_Pointer_Exercises.md
 * Line: 273
 * Language: c
 * Block ID: 47a30f27
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct ListNode {
    void *data;
    struct ListNode *next;
} ListNode;

typedef struct {
    ListNode *head;
    ListNode *tail;
    size_t size;
    void (*free_func)(void *);
} List;

// 基本操作
List* list_create(void (*free_func)(void *));
void list_destroy(List *list);
void list_push_front(List *list, void *data);
void list_push_back(List *list, void *data);
void* list_pop_front(List *list);
void* list_pop_back(List *list);
void* list_get(const List *list, size_t index);

// 高级操作
void list_sort(List *list, int (*compare)(const void *, const void *));
List* list_reverse(const List *list);
List* list_merge(List *a, List *b, int (*compare)(const void *, const void *));
void list_remove_if(List *list, int (*predicate)(const void *));
void list_foreach(List *list, void (*func)(void *));
