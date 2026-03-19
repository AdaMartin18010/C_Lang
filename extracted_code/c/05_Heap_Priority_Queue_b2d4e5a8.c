/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 1213
 * Language: c
 * Block ID: b2d4e5a8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 对偶堆 - 一种自适应的堆结构
 * 在实际应用中通常比二叉堆和二项堆更高效
 */

typedef struct PairingNode {
    int key;
    struct PairingNode* child;      // 第一个子节点
    struct PairingNode* sibling;    // 兄弟节点
    struct PairingNode* prev;       // 前驱节点 (父节点或左兄弟)
} PairingNode;

typedef struct {
    PairingNode* root;
    size_t size;
} PairingHeap;

// 合并两个对偶堆
PairingNode* pairing_merge(PairingNode* a, PairingNode* b) {
    if (a == nullptr) return b;
    if (b == nullptr) return a;

    // 确保a是根节点较小的
    if (a->key > b->key) {
        PairingNode* temp = a;
        a = b;
        b = temp;
    }

    // 将b作为a的第一个子节点
    b->sibling = a->child;
    if (a->child != nullptr) {
        a->child->prev = b;
    }
    b->prev = a;
    a->child = b;

    return a;
}

// 插入
void pairing_push(PairingHeap* heap, int key) {
    PairingNode* node = (PairingNode*)malloc(sizeof(PairingNode));
    node->key = key;
    node->child = nullptr;
    node->sibling = nullptr;
    node->prev = nullptr;

    heap->root = pairing_merge(heap->root, node);
    heap->size++;
}

// 两两合并
static PairingNode* pairing_two_pass_merge(PairingNode* first) {
    if (first == nullptr || first->sibling == nullptr) {
        return first;
    }

    // 第一遍: 从左到右两两合并
    PairingNode* pairs = nullptr;
    while (first != nullptr && first->sibling != nullptr) {
        PairingNode* a = first;
        PairingNode* b = first->sibling;
        first = b->sibling;

        PairingNode* merged = pairing_merge(a, b);
        merged->sibling = pairs;
        pairs = merged;
    }

    if (first != nullptr) {
        first->sibling = pairs;
        pairs = first;
    }

    // 第二遍: 从右到左合并
    PairingNode* result = nullptr;
    while (pairs != nullptr) {
        PairingNode* next = pairs->sibling;
        result = pairing_merge(result, pairs);
        pairs = next;
    }

    return result;
}

// 删除最小值
int pairing_pop(PairingHeap* heap) {
    if (heap->root == nullptr) {
        fprintf(stderr, "Error: Heap is empty\n");
        return -1;
    }

    int min_key = heap->root->key;
    PairingNode* old_root = heap->root;

    heap->root = pairing_two_pass_merge(heap->root->child);
    free(old_root);
    heap->size--;

    return min_key;
}
