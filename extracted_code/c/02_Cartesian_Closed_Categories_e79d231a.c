/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\02_Algebraic_Topology\02_Cartesian_Closed_Categories.md
 * Line: 537
 * Language: c
 * Block ID: e79d231a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 函数式编程风格的C实现
 * 利用函数指针模拟高阶函数
 */

#include <assert.h>

// 通用列表
typedef struct list {
    void *head;
    struct list *tail;
} list_t;

// 空列表
#define LIST_EMPTY NULL

// 构造列表
list_t *list_cons(void *head, list_t *tail)
{
    list_t *list = malloc(sizeof(list_t));
    list->head = head;
    list->tail = tail;
    return list;
}

// map : (a → b) → [a] → [b]
list_t *list_map(void *(*f)(void *), list_t *list)
{
    if (list == NULL) return NULL;
    return list_cons(f(list->head), list_map(f, list->tail));
}

// filter : (a → bool) → [a] → [a]
list_t *list_filter(bool (*p)(void *), list_t *list)
{
    if (list == NULL) return NULL;
    if (p(list->head)) {
        return list_cons(list->head, list_filter(p, list->tail));
    }
    return list_filter(p, list->tail);
}

// foldl : (b → a → b) → b → [a] → b
void *list_foldl(void *(*f)(void *, void *), void *acc, list_t *list)
{
    if (list == NULL) return acc;
    return list_foldl(f, f(acc, list->head), list->tail);
}

// foldr : (a → b → b) → b → [a] → b
void *list_foldr(void *(*f)(void *, void *), void *acc, list_t *list)
{
    if (list == NULL) return acc;
    return f(list->head, list_foldr(f, acc, list->tail));
}
