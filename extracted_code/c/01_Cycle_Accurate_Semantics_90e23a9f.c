/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\01_Cycle_Accurate_Semantics.md
 * Line: 229
 * Language: c
 * Block ID: 90e23a9f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 链表遍历：随机访问，缓存不友好
typedef struct Node {
    int data;
    struct Node *next;
} Node;

int sum_list(Node *head) {
    int sum = 0;
    for (Node *p = head; p; p = p->next) {
        sum += p->data;  // 每次访问不同缓存行
    }
    return sum;
}

// ✅ 解决方案：数组存储索引而非指针
typedef struct {
    int data;
    int next_idx;  // 数组索引
} ArrayNode;

// 或使用连续内存池
