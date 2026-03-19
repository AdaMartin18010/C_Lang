/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 1095
 * Language: c
 * Block ID: 2211392d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 链表归并排序
 * 时间复杂度: O(n log n)
 * 空间复杂度: O(log n) (递归栈)
 */
Node* merge_sorted_lists(Node* l1, Node* l2) {
    Node dummy = {0, nullptr};
    Node* tail = &dummy;

    while (l1 != nullptr && l2 != nullptr) {
        if (l1->data <= l2->data) {
            tail->next = l1;
            l1 = l1->next;
        } else {
            tail->next = l2;
            l2 = l2->next;
        }
        tail = tail->next;
    }

    tail->next = (l1 != nullptr) ? l1 : l2;
    return dummy.next;
}

Node* merge_sort_list(Node* head) {
    if (head == nullptr || head->next == nullptr) {
        return head;
    }

    // 找中点，分成两半
    Node *slow = head, *fast = head->next;
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
    }

    Node* mid = slow->next;
    slow->next = nullptr;  // 断开

    // 递归排序
    Node* left = merge_sort_list(head);
    Node* right = merge_sort_list(mid);

    // 合并
    return merge_sorted_lists(left, right);
}
