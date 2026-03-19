/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 1048
 * Language: c
 * Block ID: 2256c64b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 查找链表的中间节点
 * 快指针走2步，慢指针走1步
 * 当快指针到达末尾，慢指针在中间
 */
Node* find_middle(Node* head) {
    if (head == nullptr) return nullptr;

    Node *slow = head;
    Node *fast = head;

    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;  // 中间节点 (奇数) 或上中点 (偶数)
}

/**
 * 查找倒数第k个节点
 */
Node* find_kth_from_end(Node* head, int k) {
    if (head == nullptr || k <= 0) return nullptr;

    Node *fast = head;
    Node *slow = head;

    // 快指针先走k步
    for (int i = 0; i < k; i++) {
        if (fast == nullptr) return nullptr;  // k超出长度
        fast = fast->next;
    }

    // 同时走，快指针到达末尾时，慢指针在倒数第k个
    while (fast != nullptr) {
        fast = fast->next;
        slow = slow->next;
    }

    return slow;
}
