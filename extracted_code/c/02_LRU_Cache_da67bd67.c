/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\02_LRU_Cache.md
 * Line: 149
 * Language: c
 * Block ID: da67bd67
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 创建LRU缓存 */
LRUCache* lru_cache_create(uint32_t capacity,
                           HashFunc hash_fn,
                           EqualFunc equal_fn,
                           FreeFunc free_fn) {
    if (capacity == 0) capacity = 1024;

    /* 哈希表大小取2的幂 */
    uint32_t bucket_count = 1;
    while (bucket_count < capacity * 2) {
        bucket_count <<= 1;
    }

    LRUCache *cache = calloc(1, sizeof(LRUCache));
    cache->capacity = capacity;
    cache->bucket_count = bucket_count;
    cache->mask = bucket_count - 1;
    cache->hash_fn = hash_fn;
    cache->equal_fn = equal_fn;
    cache->free_fn = free_fn;

    /* 分配哈希表 */
    cache->buckets = calloc(bucket_count, sizeof(HashBucket));

    /* 分配节点池 */
    cache->pool_size = capacity;
    cache->node_pool = calloc(capacity, sizeof(LRUNode));

    /* 初始化空闲链表 */
    for (uint32_t i = 0; i < capacity - 1; i++) {
        cache->node_pool[i].next = &cache->node_pool[i + 1];
    }
    cache->free_list = &cache->node_pool[0];

    /* 初始化双向链表（虚拟头尾节点） */
    cache->head = calloc(1, sizeof(LRUNode));
    cache->tail = calloc(1, sizeof(LRUNode));
    cache->head->next = cache->tail;
    cache->tail->prev = cache->head;

    return cache;
}

/* 销毁缓存 */
void lru_cache_destroy(LRUCache *cache) {
    if (!cache) return;

    /* 释放所有条目 */
    if (cache->free_fn) {
        for (uint32_t i = 0; i < cache->bucket_count; i++) {
            LRUNode *node = cache->buckets[i].nodes;
            while (node) {
                cache->free_fn(node->key, node->value);
                node = node->next;
            }
        }
    }

    free(cache->buckets);
    free(cache->node_pool);
    free(cache->head);
    free(cache->tail);
    free(cache);
}
