/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 2153
 * Language: c
 * Block ID: b76ef668
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 固定大小内存池
typedef struct {
    char* buffer;
    size_t block_size;
    size_t block_count;
    size_t free_list;
    uint32_t* next;  // 空闲链表
} FixedPool;

FixedPool* fixed_pool_create(size_t block_size, size_t block_count) {
    FixedPool* pool = malloc(sizeof(FixedPool));
    pool->block_size = (block_size + 7) & ~7;  // 8字节对齐
    pool->block_count = block_count;
    pool->buffer = aligned_alloc(64, pool->block_size * block_count);
    pool->next = malloc(sizeof(uint32_t) * block_count);

    // 初始化空闲链表
    for (uint32_t i = 0; i < block_count - 1; i++) {
        pool->next[i] = i + 1;
    }
    pool->next[block_count - 1] = UINT32_MAX;  // 结束标记
    pool->free_list = 0;

    return pool;
}

void* fixed_pool_alloc(FixedPool* pool) {
    if (pool->free_list == UINT32_MAX) {
        return NULL;  // 池已满
    }

    uint32_t index = pool->free_list;
    pool->free_list = pool->next[index];

    return pool->buffer + (size_t)index * pool->block_size;
}

void fixed_pool_free(FixedPool* pool, void* ptr) {
    if (!ptr) return;

    size_t offset = (char*)ptr - pool->buffer;
    uint32_t index = (uint32_t)(offset / pool->block_size);

    pool->next[index] = pool->free_list;
    pool->free_list = index;
}

// 多层内存池
typedef struct {
    FixedPool* small;   // 64B
    FixedPool* medium;  // 256B
    FixedPool* large;   // 1KB
    Allocator* fallback;
} TieredPool;

void* tiered_alloc(TieredPool* pool, size_t size) {
    if (size <= 64) return fixed_pool_alloc(pool->small);
    if (size <= 256) return fixed_pool_alloc(pool->medium);
    if (size <= 1024) return fixed_pool_alloc(pool->large);
    return pool->fallback->alloc(size, 64, pool->fallback->userdata);
}
