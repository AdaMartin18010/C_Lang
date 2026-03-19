/*
 * Auto-generated from: 12_Practice_Exercises\03_Data_Structure_Projects.md
 * Line: 320
 * Language: c
 * Block ID: ba80843c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct LRUCache LRUCache;

LRUCache* lru_create(int capacity);
void lru_destroy(LRUCache *cache);

// 获取值，如果不存在返回NULL，存在则移到最近使用
void* lru_get(LRUCache *cache, int key);

// 插入或更新值
void lru_put(LRUCache *cache, int key, void *value);

// 当前大小
int lru_size(const LRUCache *cache);
