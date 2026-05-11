/*
 * 链表算法（反转、环检测、合并）示例
 * 编译: gcc -Wall -Wextra -o list_algorithms list_algorithms.c
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node *node_create(int data) {
    Node *n = malloc(sizeof(Node));
    n->data = data;
    n->next = NULL;
    return n;
}

void list_print(const Node *head) {
    while (head) {
        printf("%d -> ", head->data);
        head = head->next;
    }
    printf("NULL\n");
}

/* 反转链表 */
Node *list_reverse(Node *head) {
    Node *prev = NULL, *curr = head, *next = NULL;
    while (curr) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}

/* Floyd判环 */
int has_cycle(Node *head) {
    Node *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return 1;
    }
    return 0;
}

/* 找中点 */
Node *list_middle(Node *head) {
    Node *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

/* 合并两个有序链表 */
Node *list_merge(Node *a, Node *b) {
    Node dummy = {0, NULL};
    Node *tail = &dummy;
    
    while (a && b) {
        if (a->data <= b->data) {
            tail->next = a; a = a->next;
        } else {
            tail->next = b; b = b->next;
        }
        tail = tail->next;
    }
    tail->next = a ? a : b;
    return dummy.next;
}

int main(void) {
    /* 创建链表 1->2->3->4->5 */
    Node *head = node_create(1);
    head->next = node_create(2);
    head->next->next = node_create(3);
    head->next->next->next = node_create(4);
    head->next->next->next->next = node_create(5);
    
    printf("Original: "); list_print(head);
    printf("Middle: %d\n", list_middle(head)->data);
    
    head = list_reverse(head);
    printf("Reversed: "); list_print(head);
    
    printf("Has cycle: %d\n", has_cycle(head));
    
    /* 测试合并 */
    Node *a = node_create(1); a->next = node_create(3); a->next->next = node_create(5);
    Node *b = node_create(2); b->next = node_create(4); b->next->next = node_create(6);
    Node *merged = list_merge(a, b);
    printf("Merged: "); list_print(merged);
    
    return 0;
}
