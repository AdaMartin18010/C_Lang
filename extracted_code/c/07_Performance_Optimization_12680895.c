/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\07_Performance_Optimization.md
 * Line: 375
 * Language: c
 * Block ID: 12680895
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct Pool {
    char *memory;
    size_t used;
    size_t capacity;
    size_t block_size;
} Pool;

// 快速分配 - O(1)
void* pool_alloc(Pool *pool) {
    if (pool->used + pool->block_size > pool->capacity) {
        return NULL;  // 池耗尽
    }
    void *ptr = pool->memory + pool->used;
    pool->used += pool->block_size;
    return ptr;
}

// 批量释放 - O(1)
void pool_reset(Pool *pool) {
    pool->used = 0;
}
