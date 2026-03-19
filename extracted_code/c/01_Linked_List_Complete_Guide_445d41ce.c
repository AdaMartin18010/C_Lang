/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 453
 * Language: c
 * Block ID: 445d41ce
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 查找数据在链表中的位置
 * @param list 链表指针
 * @param data 要查找的数据
 * @return 找到返回索引，未找到返回 -1
 *
 * 时间复杂度: O(n)
 */
int list_find(const LinkedList* list, int data) {
    Node* current = list->head;
    int index = 0;

    while (current != nullptr) {
        if (current->data == data) {
            return index;
        }
        current = current->next;
        index++;
    }
    return -1;  // 未找到
}

/**
 * @brief 获取指定位置的节点数据
 * @param list 链表指针
 * @param index 位置索引
 * @return 成功返回数据，失败触发断言(或返回默认值)
 */
int list_get(const LinkedList* list, size_t index) {
    if (index >= list->size) {
        fprintf(stderr, "Error: Index out of bounds\n");
        exit(EXIT_FAILURE);
    }

    Node* current = list->head;
    for (size_t i = 0; i < index; i++) {
        current = current->next;
    }
    return current->data;
}

/**
 * @brief 打印链表内容
 * @param list 链表指针
 */
void list_print(const LinkedList* list) {
    printf("LinkedList [size=%zu]: ", list->size);
    printf("head -> ");

    Node* current = list->head;
    while (current != nullptr) {
        printf("%d", current->data);
        if (current->next != nullptr) {
            printf(" -> ");
        }
        current = current->next;
    }
    printf(" -> NULL\n");
}
