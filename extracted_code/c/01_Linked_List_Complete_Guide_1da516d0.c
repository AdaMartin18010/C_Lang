/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 251
 * Language: c
 * Block ID: 1da516d0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 创建新节点
 * @param data 节点数据
 * @return 新节点的指针，失败返回 nullptr
 */
[[nodiscard]]
Node* create_node(int data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == nullptr) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return nullptr;
    }
    new_node->data = data;
    new_node->next = nullptr;
    return new_node;
}
