/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\04_Cache_Lab.md
 * Line: 163
 * Language: c
 * Block ID: 583d1f06
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 解析地址，提取tag和set index
void parse_address(cache_t *cache, uint64_t addr,
                   uint64_t *tag, int *set_idx) {
    int b = cache->b;
    int s = cache->s;

    *set_idx = (addr >> b) & ((1 << s) - 1);
    *tag = addr >> (b + s);
}

// 访问缓存
void access_cache(cache_t *cache, uint64_t addr, char op) {
    uint64_t tag;
    int set_idx;
    parse_address(cache, addr, &tag, &set_idx);

    cache_set_t *set = &cache->sets[set_idx];
    cache->global_counter++;

    // 检查是否命中
    for (int i = 0; i < cache->E; i++) {
        if (set->lines[i].valid && set->lines[i].tag == tag) {
            // 命中
            cache->hit_count++;
            set->lines[i].lru_counter = cache->global_counter;

            // 如果是M操作，额外一次命中
            if (op == 'M') {
                cache->hit_count++;
            }
            return;
        }
    }

    // 未命中
    cache->miss_count++;

    // 查找空行
    for (int i = 0; i < cache->E; i++) {
        if (!set->lines[i].valid) {
            // 使用空行
            set->lines[i].valid = true;
            set->lines[i].tag = tag;
            set->lines[i].lru_counter = cache->global_counter;

            if (op == 'M') {
                cache->hit_count++;
            }
            return;
        }
    }

    // 需要驱逐
    cache->eviction_count++;

    // 找到LRU行
    int lru_idx = 0;
    int min_counter = set->lines[0].lru_counter;
    for (int i = 1; i < cache->E; i++) {
        if (set->lines[i].lru_counter < min_counter) {
            min_counter = set->lines[i].lru_counter;
            lru_idx = i;
        }
    }

    // 替换LRU行
    set->lines[lru_idx].tag = tag;
    set->lines[lru_idx].lru_counter = cache->global_counter;

    if (op == 'M') {
        cache->hit_count++;
    }
}
