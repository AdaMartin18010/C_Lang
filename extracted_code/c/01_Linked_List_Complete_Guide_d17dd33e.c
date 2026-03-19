/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 1146
 * Language: c
 * Block ID: d17dd33e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * LRU (Least Recently Used) 缓存
 * 结合哈希表和双链表实现 O(1) 操作
 */

typedef struct LRUCache {
    int capacity;
    int size;
    // 哈希表: key -> DNode*
    // 这里简化实现，实际使用更好的哈希表
    DNode** hash_table;
    int* keys;
    DoublyLinkedList list;  // 按访问时间排序，最新在头部
} LRUCache;

// 将节点移到头部 (表示最近使用)
void lru_move_to_head(LRUCache* cache, DNode* node) {
    if (cache->list.head == node) return;

    // 从当前位置移除
    if (node->prev != nullptr) node->prev->next = node->next;
    if (node->next != nullptr) node->next->prev = node->prev;
    if (cache->list.tail == node) cache->list.tail = node->prev;

    // 插入头部
    node->prev = nullptr;
    node->next = cache->list.head;
    if (cache->list.head != nullptr) cache->list.head->prev = node;
    cache->list.head = node;
    if (cache->list.tail == nullptr) cache->list.tail = node;
}

// 访问缓存
int lru_get(LRUCache* cache, int key) {
    if (key < 0 || key >= cache->capacity) return -1;

    DNode* node = cache->hash_table[key];
    if (node == nullptr) return -1;

    // 移到头部
    lru_move_to_head(cache, node);
    return node->data;
}

// 放入缓存
void lru_put(LRUCache* cache, int key, int value) {
    DNode* node = cache->hash_table[key];

    if (node != nullptr) {
        // 更新已存在的节点
        node->data = value;
        lru_move_to_head(cache, node);
    } else {
        // 创建新节点
        DNode* new_node = dnode_create(value);

        if (cache->size >= cache->capacity) {
            // 淘汰尾部 (最久未使用)
            DNode* tail = cache->list.tail;
            cache->hash_table[tail->data] = nullptr;  // 简化: 用data作为key
            dlist_delete_node(&cache->list, tail);
            cache->size--;
        }

        // 插入头部
        new_node->next = cache->list.head;
        if (cache->list.head != nullptr) {
            cache->list.head->prev = new_node;
        }
        cache->list.head = new_node;
        if (cache->list.tail == nullptr) {
            cache->list.tail = new_node;
        }

        cache->hash_table[key] = new_node;
        cache->size++;
    }
}
