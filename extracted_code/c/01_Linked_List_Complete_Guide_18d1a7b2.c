/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 299
 * Language: c
 * Block ID: 18d1a7b2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 在链表尾部插入节点
 * @param list 链表指针
 * @param data 插入的数据
 * @return 成功返回 true，失败返回 false
 *
 * 时间复杂度: O(1) (使用尾指针优化)
 * 空间复杂度: O(1)
 */
bool list_append(LinkedList* list, int data) {
    Node* new_node = create_node(data);
    if (new_node == nullptr) return false;

    if (list->tail == nullptr) {
        // 空链表
        list->head = list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail = new_node;
    }
    list->size++;
    return true;
}
