/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_Coq_Examples\01_Coq_Verification_Cases.md
 * Line: 744
 * Language: c
 * Block ID: efa2bf62
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* File: c_impl/linked_list.c */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

/* 创建新节点 */
Node* create_node(int data) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->next = NULL;
    return node;
}

/* 计算链表长度 */
int list_length(Node* head) {
    int len = 0;
    while (head != NULL) {
        len++;
        head = head->next;
    }
    return len;
}

/* 追加元素到链表末尾 */
void list_append(Node** head, int data) {
    Node* new_node = create_node(data);
    if (*head == NULL) {
        *head = new_node;
        return;
    }
    Node* current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
}

/* 链表反转 */
Node* list_reverse(Node* head) {
    Node *prev = NULL, *current = head, *next = NULL;
    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    return prev;
}

/* 获取第n个元素 */
int list_nth(Node* head, int n, int default_val) {
    int i = 0;
    while (head != NULL && i < n) {
        head = head->next;
        i++;
    }
    if (head == NULL) return default_val;
    return head->data;
}

/* 查找元素 */
int list_contains(Node* head, int value) {
    while (head != NULL) {
        if (head->data == value) return 1;
        head = head->next;
    }
    return 0;
}

/* 映射函数 */
Node* list_map(Node* head, int (*f)(int)) {
    if (head == NULL) return NULL;
    Node* new_head = create_node(f(head->data));
    Node* new_current = new_head;
    Node* current = head->next;
    while (current != NULL) {
        new_current->next = create_node(f(current->data));
        new_current = new_current->next;
        current = current->next;
    }
    return new_head;
}

/* 释放链表 */
void list_free(Node* head) {
    while (head != NULL) {
        Node* temp = head;
        head = head->next;
        free(temp);
    }
}

/* 测试函数 */
void test_linked_list() {
    Node* list = NULL;

    /* 测试追加操作 */
    list_append(&list, 1);
    list_append(&list, 2);
    list_append(&list, 3);
    assert(list_length(list) == 3);

    /* 测试反转操作 */
    Node* reversed = list_reverse(list);
    assert(list_nth(reversed, 0, -1) == 3);
    assert(list_nth(reversed, 1, -1) == 2);
    assert(list_nth(reversed, 2, -1) == 1);

    /* 测试包含操作 */
    assert(list_contains(reversed, 2) == 1);
    assert(list_contains(reversed, 5) == 0);

    list_free(reversed);
}
