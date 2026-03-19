/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 517
 * Language: c
 * Block ID: 4839c275
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 反转链表（迭代法）
 * @param list 链表指针
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 *
 * 算法思想:
 * 1. 使用三个指针: prev, current, next
 * 2. 逐个反转节点的指向
 * 3. 更新头尾指针
 */
void list_reverse(LinkedList* list) {
    if (list->head == nullptr || list->head->next == nullptr) {
        return;  // 空链表或单节点
    }

    Node *prev = nullptr;
    Node *current = list->head;
    Node *next = nullptr;

    list->tail = list->head;  // 原来的头变成尾

    while (current != nullptr) {
        next = current->next;      // 保存下一个节点
        current->next = prev;      // 反转指针
        prev = current;            // 移动prev
        current = next;            // 移动current
    }

    list->head = prev;  // 新的头节点
}

/**
 * @brief 反转链表（递归法）
 * @param head 当前头节点
 * @return 新的头节点
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n) (递归栈空间)
 */
Node* list_reverse_recursive_helper(Node* head) {
    // 基准情况: 空链表或单节点
    if (head == nullptr || head->next == nullptr) {
        return head;
    }

    // 递归反转后续链表
    Node* new_head = list_reverse_recursive_helper(head->next);

    // 反转当前节点与下一个节点的连接
    head->next->next = head;
    head->next = nullptr;

    return new_head;
}

void list_reverse_recursive(LinkedList* list) {
    if (list->head == nullptr) return;
    list->tail = list->head;
    list->head = list_reverse_recursive_helper(list->head);
}
