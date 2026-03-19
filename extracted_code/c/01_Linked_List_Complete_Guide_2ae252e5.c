/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 272
 * Language: c
 * Block ID: 2ae252e5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 在链表头部插入节点
 * @param list 链表指针
 * @param data 插入的数据
 * @return 成功返回 true，失败返回 false
 *
 * 时间复杂度: O(1)
 * 空间复杂度: O(1)
 */
bool list_prepend(LinkedList* list, int data) {
    Node* new_node = create_node(data);
    if (new_node == nullptr) return false;

    new_node->next = list->head;
    list->head = new_node;

    if (list->tail == nullptr) {
        list->tail = new_node;  // 第一个节点
    }
    list->size++;
    return true;
}
