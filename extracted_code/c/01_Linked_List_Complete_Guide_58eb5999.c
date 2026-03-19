/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 1372
 * Language: c
 * Block ID: 58eb5999
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 每k个节点一组进行翻转
 * 输入: 1->2->3->4->5, k=2
 * 输出: 2->1->4->3->5
 */
Node* reverse_k_group(Node* head, int k) {
    Node dummy = {0, head};
    Node* group_prev = &dummy;

    while (true) {
        // 检查是否还有k个节点
        Node* kth = group_prev;
        for (int i = 0; i < k && kth != nullptr; i++) {
            kth = kth->next;
        }
        if (kth == nullptr) break;  // 不足k个

        Node* group_next = kth->next;
        Node* prev = kth->next;
        Node* current = group_prev->next;

        // 翻转这一组
        while (current != group_next) {
            Node* next = current->next;
            current->next = prev;
            prev = current;
            current = next;
        }

        // 更新连接
        Node* new_group_prev = group_prev->next;
        group_prev->next = kth;
        group_prev = new_group_prev;
    }

    return dummy.next;
}
