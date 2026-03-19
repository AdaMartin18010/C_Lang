/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 976
 * Language: c
 * Block ID: bda98067
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 二项堆 - 支持高效合并的堆结构
 * 由一组二项树组成
 */

typedef struct BinomialNode {
    int key;
    int degree;              // 子节点数
    struct BinomialNode* parent;
    struct BinomialNode* child;
    struct BinomialNode* sibling;
} BinomialNode;

typedef struct {
    BinomialNode* head;      // 指向根链表
} BinomialHeap;

// 合并两个相同阶的二项树
BinomialNode* binomial_link(BinomialNode* y, BinomialNode* z) {
    if (y->key > z->key) {
        BinomialNode* temp = y;
        y = z;
        z = temp;
    }
    z->parent = y;
    z->sibling = y->child;
    y->child = z;
    y->degree++;
    return y;
}
