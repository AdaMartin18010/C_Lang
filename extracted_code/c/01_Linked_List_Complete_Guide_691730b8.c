/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 665
 * Language: c
 * Block ID: 691730b8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 双链表节点定义
 * 每个节点包含: 数据、前驱指针、后继指针
 */
typedef struct DNode {
    int data;
    struct DNode* prev;   // 前驱指针
    struct DNode* next;   // 后继指针
} DNode;

typedef struct {
    DNode* head;
    DNode* tail;
    size_t size;
} DoublyLinkedList;
