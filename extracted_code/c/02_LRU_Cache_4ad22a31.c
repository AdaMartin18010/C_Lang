/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\02_LRU_Cache.md
 * Line: 432
 * Language: c
 * Block ID: 4ad22a31
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 删除指定键 */
bool lru_cache_delete(LRUCache *cache, const void *key) {
    uint32_t hash = cache->hash_fn(key);
    LRUNode *node = find_node(cache, key, hash);

    if (!node) {
        return false;
    }

    /* 移除 */
    list_remove(cache, node);
    bucket_remove(cache, node);
    free_node(cache, node);
    cache->size--;

    return true;
}

/* 清空缓存 */
void lru_cache_clear(LRUCache *cache) {
    /* 释放所有节点 */
    for (uint32_t i = 0; i < cache->bucket_count; i++) {
        LRUNode *node = cache->buckets[i].nodes;
        while (node) {
            LRUNode *next = node->next;
            free_node(cache, node);
            node = next;
        }
        cache->buckets[i].nodes = NULL;
    }

    /* 重置链表 */
    cache->head->next = cache->tail;
    cache->tail->prev = cache->head;
    cache->size = 0;
    cache->hits = 0;
    cache->misses = 0;
}
