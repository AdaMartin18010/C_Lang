/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\02_LRU_Cache.md
 * Line: 264
 * Language: c
 * Block ID: c3fbb4c7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 查找节点 */
static LRUNode* find_node(LRUCache *cache, const void *key, uint32_t hash) {
    uint32_t idx = hash & cache->mask;
    LRUNode *node = cache->buckets[idx].nodes;

    while (node) {
        if (node->hash == hash && cache->equal_fn(node->key, key)) {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

/* 获取缓存值 */
void* lru_cache_get(LRUCache *cache, const void *key) {
    uint32_t hash = cache->hash_fn(key);
    LRUNode *node = find_node(cache, key, hash);

    if (node) {
        /* 命中：移到头部 */
        if (!(node->flags & NODE_FLAG_PINNED)) {
            list_move_to_head(cache, node);
        }
        cache->hits++;
        return node->value;
    }

    cache->misses++;
    return NULL;
}

/* 带PIN标记的Get（防止淘汰） */
void* lru_cache_get_pinned(LRUCache *cache, const void *key) {
    void *value = lru_cache_get(cache, key);
    if (value) {
        uint32_t hash = cache->hash_fn(key);
        LRUNode *node = find_node(cache, key, hash);
        if (node) {
            node->flags |= NODE_FLAG_PINNED;
        }
    }
    return value;
}
