/*
 * 侵入式链表（Linux Kernel风格）示例
 * 编译: gcc -Wall -Wextra -o intrusive_list intrusive_list.c
 */
#include <stdio.h>
#include <stddef.h>

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

typedef struct list_node {
    struct list_node *next;
    struct list_node *prev;
} list_node_t;

#define LIST_HEAD_INIT(name) { &(name), &(name) }

static inline void list_init(list_node_t *head) {
    head->next = head;
    head->prev = head;
}

static inline void list_insert_after(list_node_t *node, list_node_t *new_node) {
    new_node->next = node->next;
    new_node->prev = node;
    node->next->prev = new_node;
    node->next = new_node;
}

static inline void list_add(list_node_t *head, list_node_t *new_node) {
    list_insert_after(head, new_node);
}

static inline void list_remove(list_node_t *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

static inline int list_is_empty(list_node_t *head) {
    return head->next == head;
}

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/* 使用示例 */
typedef struct {
    int id;
    const char *name;
    list_node_t link;
} Task;

int main(void) {
    list_node_t task_list;
    list_init(&task_list);
    
    Task t1 = {1, "Task1", {NULL, NULL}};
    Task t2 = {2, "Task2", {NULL, NULL}};
    Task t3 = {3, "Task3", {NULL, NULL}};
    
    list_add(&task_list, &t1.link);
    list_add(&task_list, &t2.link);
    list_add(&task_list, &t3.link);
    
    list_node_t *pos;
    list_for_each(pos, &task_list) {
        Task *t = container_of(pos, Task, link);
        printf("Task %d: %s\n", t->id, t->name);
    }
    
    return 0;
}
