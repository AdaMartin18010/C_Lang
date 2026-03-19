/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\02_LRU_Cache.md
 * Line: 314
 * Language: c
 * Block ID: 3c0de5e2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 从哈希桶移除节点 */
static void bucket_remove(LRUCache *cache, LRUNode *node) {
    uint32_t idx = node->hash & cache->mask;
    LRUNode **pp = &cache->buckets[idx].nodes;

    while (*pp) {
        if (*pp == node) {
            *pp = node->next;  /* 从冲突链表移除 */
            return;
        }
        pp = &(*pp)->next;
    }
}

/* 将节点加入哈希桶 */
static void bucket_add(LRUCache *cache, LRUNode *node) {
    uint32_t idx = node->hash & cache->mask;
    node->next = cache->buckets[idx].nodes;
    cache->buckets[idx].nodes = node;
}

/* 分配新节点 */
static LRUNode* alloc_node(LRUCache *cache) {
    if (!cache->free_list) {
        return NULL;  /* 节点池耗尽 */
    }

    LRUNode *node = cache->free_list;
    cache->free_list = node->next;
    memset(node, 0, sizeof(LRUNode));
    return node;
}

/* 回收节点 */
static void free_node(LRUCache *cache, LRUNode *node) {
    /* 清理 */
    if (cache->free_fn) {
        cache->free_fn(node->key, node->value);
    }

    /* 归还空闲链表 */
    node->key = NULL;
    node->value = NULL;
    node->next = cache->free_list;
    cache->free_list = node;
}

/* 淘汰最久未使用的条目 */
static bool evict_lru(LRUCache *cache) {
    LRUNode *victim = list_get_tail(cache);

    while (victim && (victim->flags & NODE_FLAG_PINNED)) {
        /* 跳过被固定的节点 */
        victim = victim->prev;
    }

    if (!victim) {
        return false;  /* 无法淘汰 */
    }

    /* 从链表和哈希表移除 */
    list_remove(cache, victim);
    bucket_remove(cache, victim);

    /* 释放 */
    free_node(cache, victim);
    cache->size--;

    return true;
}

/* 插入或更新缓存 */
bool lru_cache_put(LRUCache *cache, void *key, void *value) {
    uint32_t hash = cache->hash_fn(key);
    LRUNode *node = find_node(cache, key, hash);

    if (node) {
        /* 更新现有值 */
        if (cache->free_fn) {
            cache->free_fn(node->key, node->value);
        }
        node->key = key;
        node->value = value;
        list_move_to_head(cache, node);
        return true;
    }

    /* 检查容量 */
    if (cache->size >= cache->capacity) {
        if (!evict_lru(cache)) {
            return false;  /* 无法插入 */
        }
    }

    /* 分配新节点 */
    node = alloc_node(cache);
    if (!node) {
        return false;
    }

    /* 初始化 */
    node->key = key;
    node->value = value;
    node->hash = hash;
    node->flags = 0;

    /* 加入数据结构 */
    bucket_add(cache, node);
    list_add_to_head(cache, node);

    cache->size++;
    return true;
}
