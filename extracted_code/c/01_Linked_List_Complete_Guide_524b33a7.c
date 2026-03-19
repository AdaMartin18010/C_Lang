/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 232
 * Language: c
 * Block ID: 524b33a7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 标准下的链表节点定义
typedef struct Node {
    int data;               // 数据域
    struct Node* next;      // 指针域
} Node;

// 带头节点的链表类型定义
typedef struct {
    Node* head;             // 头指针
    Node* tail;             // 尾指针（可选，优化尾部操作）
    size_t size;            // 节点数量
} LinkedList;
