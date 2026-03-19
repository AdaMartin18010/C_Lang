/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\04_Cache_Lab.md
 * Line: 133
 * Language: c
 * Block ID: 160399bb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 初始化缓存
cache_t* init_cache(int s, int E, int b) {
    cache_t *cache = malloc(sizeof(cache_t));
    cache->s = s;
    cache->E = E;
    cache->b = b;
    cache->S = 1 << s;    // 2^s
    cache->B = 1 << b;    // 2^b
    cache->hit_count = 0;
    cache->miss_count = 0;
    cache->eviction_count = 0;
    cache->global_counter = 0;

    // 分配组
    cache->sets = malloc(cache->S * sizeof(cache_set_t));
    for (int i = 0; i < cache->S; i++) {
        cache->sets[i].lines = malloc(E * sizeof(cache_line_t));
        for (int j = 0; j < E; j++) {
            cache->sets[i].lines[j].valid = false;
            cache->sets[i].lines[j].tag = 0;
            cache->sets[i].lines[j].lru_counter = 0;
        }
    }
    return cache;
}
