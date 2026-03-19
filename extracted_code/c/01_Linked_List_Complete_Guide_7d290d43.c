/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 1320
 * Language: c
 * Block ID: 7d290d43
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct RNode {
    int data;
    struct RNode* next;
    struct RNode* random;  // 随机指向任意节点或nullptr
} RNode;

/**
 * 方法1: 哈希表法 O(n) 空间
 * 方法2: 原地复制法 O(1) 空间
 */
RNode* copy_random_list(RNode* head) {
    if (head == nullptr) return nullptr;

    // 第一步: 在每个原节点后复制新节点
    RNode* current = head;
    while (current != nullptr) {
        RNode* copy = (RNode*)malloc(sizeof(RNode));
        copy->data = current->data;
        copy->next = current->next;
        copy->random = nullptr;
        current->next = copy;
        current = copy->next;
    }

    // 第二步: 设置新节点的random指针
    current = head;
    while (current != nullptr) {
        if (current->random != nullptr) {
            current->next->random = current->random->next;
        }
        current = current->next->next;
    }

    // 第三步: 分离两个链表
    RNode* new_head = head->next;
    current = head;
    while (current != nullptr) {
        RNode* copy = current->next;
        current->next = copy->next;
        if (copy->next != nullptr) {
            copy->next = copy->next->next;
        }
        current = current->next;
    }

    return new_head;
}
