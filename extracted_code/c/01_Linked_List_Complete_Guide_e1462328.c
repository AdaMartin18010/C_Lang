/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 1020
 * Language: c
 * Block ID: e1462328
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 使用虚拟头节点简化边界处理
 * 优点:
 * 1. 无需特殊处理空链表
 * 2. 无需特殊处理头节点删除
 * 3. 代码更简洁
 */
Node* remove_elements(Node* head, int val) {
    Node dummy = {0, head};  // C23 支持混合初始化
    Node* current = &dummy;

    while (current->next != nullptr) {
        if (current->next->data == val) {
            Node* temp = current->next;
            current->next = temp->next;
            free(temp);
        } else {
            current = current->next;
        }
    }

    return dummy.next;
}
