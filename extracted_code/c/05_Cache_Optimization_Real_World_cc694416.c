/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\05_Cache_Optimization_Real_World.md
 * Line: 272
 * Language: c
 * Block ID: cc694416
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 链表节点
struct Node {
    int data;
    struct Node* next;
};

// 数组元素
struct Element {
    int data;
    int next_idx;  // 模拟链表，但使用数组索引
};

// 链表遍历
int list_sum_linked(struct Node* head) {
    int sum = 0;
    struct Node* curr = head;
    while (curr != NULL) {
        sum += curr->data;  // 缓存未命中！
        curr = curr->next;  // 随机地址
    }
    return sum;
}

// 数组遍历 (连续内存)
int list_sum_array(struct Element* arr, int head_idx) {
    int sum = 0;
    int idx = head_idx;
    while (idx != -1) {
        sum += arr[idx].data;   // 顺序访问，缓存友好
        idx = arr[idx].next_idx;
    }
    return sum;
}
