/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\07_Proxy_Lab.md
 * Line: 682
 * Language: c
 * Block ID: 5de0aeaa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define MAX_CACHE_SIZE 1049000    // 1MB
#define MAX_OBJECT_SIZE 102400    // 100KB
#define MAX_CACHE_BLOCKS 10

// 缓存块
typedef struct cache_block {
    char url[MAXLINE];
    char data[MAX_OBJECT_SIZE];
    int size;
    int valid;
    int timestamp;
    struct cache_block *prev;
    struct cache_block *next;
} cache_block_t;

typedef struct {
    cache_block_t blocks[MAX_CACHE_BLOCKS];
    int total_size;
    int readcnt;
    sem_t mutex;
    sem_t w;
    int timestamp_counter;
} cache_t;

cache_t cache;

// 初始化缓存
void cache_init() {
    cache.total_size = 0;
    cache.readcnt = 0;
    cache.timestamp_counter = 0;
    sem_init(&cache.mutex, 0, 1);
    sem_init(&cache.w, 0, 1);

    for (int i = 0; i < MAX_CACHE_BLOCKS; i++) {
        cache.blocks[i].valid = 0;
        cache.blocks[i].prev = (i > 0) ? &cache.blocks[i-1] : NULL;
        cache.blocks[i].next = (i < MAX_CACHE_BLOCKS-1) ? &cache.blocks[i+1] : NULL;
    }
}
