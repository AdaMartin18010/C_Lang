/*
 * Auto-generated from: 15_Memory_Safe_Languages\01_Rust_vs_C\02_Ownership_vs_Pointers.md
 * Line: 395
 * Language: c
 * Block ID: 7d4c6ada
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C链表实现
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    size_t len;
} List;

List* list_new() {
    List* list = malloc(sizeof(List));
    list->head = NULL;
    list->len = 0;
    return list;
}

void list_push(List* list, int value) {
    Node* node = malloc(sizeof(Node));
    node->data = value;
    node->next = list->head;
    list->head = node;
    list->len++;
}

// 容易出错的删除操作
void list_remove(List* list, int value) {
    Node** current = &list->head;
    while (*current) {
        if ((*current)->data == value) {
            Node* to_remove = *current;
            *current = (*current)->next;
            free(to_remove);
            list->len--;
            return;  // 只删除第一个匹配项
        }
        current = &(*current)->next;
    }
}

// 必须手动调用，容易遗漏
void list_free(List* list) {
    Node* current = list->head;
    while (current) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
