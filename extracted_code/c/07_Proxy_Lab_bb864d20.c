/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\07_Proxy_Lab.md
 * Line: 727
 * Language: c
 * Block ID: bb864d20
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 查找缓存
int cache_find(char *url, char *data) {
    int found = 0;

    sem_wait(&cache.mutex);
    cache.readcnt++;
    if (cache.readcnt == 1)
        sem_wait(&cache.w);
    sem_post(&cache.mutex);

    // 读者临界区
    for (int i = 0; i < MAX_CACHE_BLOCKS; i++) {
        if (cache.blocks[i].valid && !strcmp(cache.blocks[i].url, url)) {
            memcpy(data, cache.blocks[i].data, cache.blocks[i].size);
            found = cache.blocks[i].size;
            // 更新时间戳
            cache.blocks[i].timestamp = ++cache.timestamp_counter;
            break;
        }
    }

    sem_wait(&cache.mutex);
    cache.readcnt--;
    if (cache.readcnt == 0)
        sem_post(&cache.w);
    sem_post(&cache.mutex);

    return found;
}

// 添加缓存（LRU替换）
void cache_insert(char *url, char *data, int size) {
    if (size > MAX_OBJECT_SIZE) return;

    sem_wait(&cache.w);

    // 查找空块或最旧的块
    int idx = -1;
    int min_timestamp = cache.timestamp_counter + 1;

    for (int i = 0; i < MAX_CACHE_BLOCKS; i++) {
        if (!cache.blocks[i].valid) {
            idx = i;
            break;
        }
        if (cache.blocks[i].timestamp < min_timestamp) {
            min_timestamp = cache.blocks[i].timestamp;
            idx = i;
        }
    }

    // 插入新数据
    strcpy(cache.blocks[idx].url, url);
    memcpy(cache.blocks[idx].data, data, size);
    cache.blocks[idx].size = size;
    cache.blocks[idx].valid = 1;
    cache.blocks[idx].timestamp = ++cache.timestamp_counter;

    sem_post(&cache.w);
}
