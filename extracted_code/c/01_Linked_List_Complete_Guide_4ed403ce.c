/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 370
 * Language: c
 * Block ID: 4ed403ce
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 删除第一个匹配的节点
 * @param list 链表指针
 * @param data 要删除的数据
 * @return 成功返回 true，未找到返回 false
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(1)
 */
bool list_delete(LinkedList* list, int data) {
    if (list->head == nullptr) return false;

    // 头节点就是要删除的节点
    if (list->head->data == data) {
        Node* temp = list->head;
        list->head = temp->next;
        if (list->tail == temp) {
            list->tail = nullptr;  // 删除了唯一节点
        }
        free(temp);
        list->size--;
        return true;
    }

    // 查找要删除的节点
    Node* current = list->head;
    while (current->next != nullptr && current->next->data != data) {
        current = current->next;
    }

    if (current->next == nullptr) return false;  // 未找到

    Node* temp = current->next;
    current->next = temp->next;
    if (list->tail == temp) {
        list->tail = current;  // 更新了尾指针
    }
    free(temp);
    list->size--;
    return true;
}

/**
 * @brief 删除指定位置的节点
 * @param list 链表指针
 * @param index 删除位置 (0-based)
 * @return 成功返回 true，失败返回 false
 */
bool list_delete_at(LinkedList* list, size_t index) {
    if (index >= list->size || list->head == nullptr) {
        fprintf(stderr, "Error: Index out of bounds\n");
        return false;
    }

    Node* to_delete = nullptr;

    if (index == 0) {
        to_delete = list->head;
        list->head = to_delete->next;
        if (list->tail == to_delete) {
            list->tail = nullptr;
        }
    } else {
        Node* current = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            current = current->next;
        }
        to_delete = current->next;
        current->next = to_delete->next;
        if (list->tail == to_delete) {
            list->tail = current;
        }
    }

    free(to_delete);
    list->size--;
    return true;
}
