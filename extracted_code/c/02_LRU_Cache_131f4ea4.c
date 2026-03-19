/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\02_LRU_Cache.md
 * Line: 479
 * Language: c
 * Block ID: 131f4ea4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 获取统计信息 */
typedef struct {
    uint32_t size;
    uint32_t capacity;
    uint64_t hits;
    uint64_t misses;
    double hit_rate;
} LRUStats;

LRUStats lru_cache_stats(LRUCache *cache) {
    LRUStats stats = {
        .size = cache->size,
        .capacity = cache->capacity,
        .hits = cache->hits,
        .misses = cache->misses,
    };

    uint64_t total = stats.hits + stats.misses;
    if (total > 0) {
        stats.hit_rate = (double)stats.hits / total;
    }

    return stats;
}

/* 遍历回调 */
typedef void (*LRUVisitor)(const void *key, void *value, void *ctx);

void lru_cache_foreach(LRUCache *cache, LRUVisitor visitor, void *ctx) {
    /* 从MRU到LRU遍历 */
    LRUNode *node = cache->head->next;
    while (node != cache->tail) {
        visitor(node->key, node->value, ctx);
        node = node->next;
    }
}

/* 调整容量 */
bool lru_cache_resize(LRUCache *cache, uint32_t new_capacity) {
    if (new_capacity < cache->size) {
        /* 淘汰多余条目 */
        while (cache->size > new_capacity) {
            if (!evict_lru(cache)) {
                return false;
            }
        }
    }

    cache->capacity = new_capacity;
    return true;
}
