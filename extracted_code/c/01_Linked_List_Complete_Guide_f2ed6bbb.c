/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 1272
 * Language: c
 * Block ID: f2ed6bbb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 算法步骤:
 * 1. 找到中间节点
 * 2. 反转后半部分
 * 3. 交错合并
 */
void reorder_list(Node* head) {
    if (head == nullptr || head->next == nullptr) return;

    // 1. 找中点
    Node *slow = head, *fast = head;
    while (fast->next != nullptr && fast->next->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 2. 反转后半部分
    Node *prev = nullptr;
    Node *current = slow->next;
    slow->next = nullptr;  // 断开

    while (current != nullptr) {
        Node* next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    // 3. 交错合并
    Node* first = head;
    Node* second = prev;

    while (second != nullptr) {
        Node* temp1 = first->next;
        Node* temp2 = second->next;

        first->next = second;
        second->next = temp1;

        first = temp1;
        second = temp2;
    }
}
