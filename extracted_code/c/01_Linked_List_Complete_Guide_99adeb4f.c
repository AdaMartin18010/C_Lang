/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 1414
 * Language: c
 * Block ID: 99adeb4f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用归并排序 (已在上文实现)
// 也可以使用快速排序的链表版本

/**
 * 链表快速排序
 * 选择基准，分区，递归排序
 */
Node* quick_sort_list(Node* head) {
    if (head == nullptr || head->next == nullptr) {
        return head;
    }

    // 选择基准 (这里选第一个)
    int pivot = head->data;

    // 分区
    Node dummy_less = {0, nullptr};
    Node dummy_equal = {0, nullptr};
    Node dummy_greater = {0, nullptr};

    Node *less_tail = &dummy_less;
    Node *equal_tail = &dummy_equal;
    Node *greater_tail = &dummy_greater;

    Node* current = head;
    while (current != nullptr) {
        Node* next = current->next;
        if (current->data < pivot) {
            less_tail->next = current;
            less_tail = current;
        } else if (current->data == pivot) {
            equal_tail->next = current;
            equal_tail = current;
        } else {
            greater_tail->next = current;
            greater_tail = current;
        }
        current = next;
    }

    less_tail->next = nullptr;
    equal_tail->next = nullptr;
    greater_tail->next = nullptr;

    // 递归排序
    Node* less_sorted = quick_sort_list(dummy_less.next);
    Node* greater_sorted = quick_sort_list(dummy_greater.next);

    // 合并
    Node dummy_result = {0, nullptr};
    Node* tail = &dummy_result;

    if (less_sorted != nullptr) {
        tail->next = less_sorted;
        while (tail->next != nullptr) tail = tail->next;
    }

    tail->next = dummy_equal.next;
    while (tail->next != nullptr) tail = tail->next;

    tail->next = greater_sorted;

    return dummy_result.next;
}
