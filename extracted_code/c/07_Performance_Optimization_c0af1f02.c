/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\07_Performance_Optimization.md
 * Line: 82
 * Language: c
 * Block ID: c0af1f02
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单链表哈希表 - 低性能

typedef struct Node {
    uint64_t key;
    void *value;
    struct Node *next;
} Node;

typedef struct {
    Node **buckets;
    int size;
} HashTable;

void* ht_get(HashTable *ht, uint64_t key) {
    int idx = hash(key) % ht->size;
    Node *n = ht->buckets[idx];
    while (n) {
        if (n->key == key) return n->value;
        n = n->next;  // 链表遍历 - 缓存不友好
    }
    return NULL;
}
