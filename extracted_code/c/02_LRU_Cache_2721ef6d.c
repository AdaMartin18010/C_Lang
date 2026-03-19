/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\02_LRU_Cache.md
 * Line: 222
 * Language: c
 * Block ID: 2721ef6d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 从链表中移除节点 */
static inline void list_remove(LRUCache *cache, LRUNode *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

/* 将节点移到头部（最近使用） */
static inline void list_move_to_head(LRUCache *cache, LRUNode *node) {
    /* 先从原位置移除 */
    list_remove(cache, node);

    /* 插入头部 */
    node->next = cache->head->next;
    node->prev = cache->head;
    cache->head->next->prev = node;
    cache->head->next = node;
}

/* 在头部插入新节点 */
static inline void list_add_to_head(LRUCache *cache, LRUNode *node) {
    node->next = cache->head->next;
    node->prev = cache->head;
    cache->head->next->prev = node;
    cache->head->next = node;
}

/* 获取尾部节点（最久未使用） */
static inline LRUNode* list_get_tail(LRUCache *cache) {
    if (cache->tail->prev == cache->head) {
        return NULL;  /* 空链表 */
    }
    return cache->tail->prev;
}
