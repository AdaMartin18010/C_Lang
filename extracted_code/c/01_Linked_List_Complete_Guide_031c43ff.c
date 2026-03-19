/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 327
 * Language: c
 * Block ID: 031c43ff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 在指定位置插入节点
 * @param list 链表指针
 * @param index 插入位置 (0-based)
 * @param data 插入的数据
 * @return 成功返回 true，失败返回 false
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
bool list_insert_at(LinkedList* list, size_t index, int data) {
    if (index > list->size) {
        fprintf(stderr, "Error: Index out of bounds\n");
        return false;
    }

    if (index == 0) {
        return list_prepend(list, data);
    }

    if (index == list->size) {
        return list_append(list, data);
    }

    Node* new_node = create_node(data);
    if (new_node == nullptr) return false;

    // 找到插入位置的前一个节点
    Node* current = list->head;
    for (size_t i = 0; i < index - 1; i++) {
        current = current->next;
    }

    new_node->next = current->next;
    current->next = new_node;
    list->size++;
    return true;
}
