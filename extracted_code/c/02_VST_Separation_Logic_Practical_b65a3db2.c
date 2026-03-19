/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\02_VST_Separation_Logic_Practical.md
 * Line: 266
 * Language: c
 * Block ID: b65a3db2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 链表节点
typedef struct Node {
    int value;
    struct Node *next;
} Node;

// 在头部插入
Node* list_prepend(Node *head, int value) {
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL) return NULL;
    new_node->value = value;
    new_node->next = head;
    return new_node;
}
