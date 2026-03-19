/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\README.md
 * Line: 313
 * Language: c
 * Block ID: f13ff8ae
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct mempool {
    char* buffer;
    size_t size;
    size_t used;
    struct mempool* next;  // 链表支持多个块
} mempool_t;

mempool_t* mempool_create(size_t initial_size) {
    mempool_t* pool = malloc(sizeof(mempool_t));
    pool->buffer = malloc(initial_size);
    pool->size = initial_size;
    pool->used = 0;
    pool->next = NULL;
    return pool;
}

void* mempool_alloc(mempool_t* pool, size_t size) {
    // 对齐
    size = (size + 7) & ~7;

    if (pool->used + size > pool->size) {
        // 分配新块
        if (!pool->next) {
            pool->next = mempool_create(pool->size * 2);
        }
        return mempool_alloc(pool->next, size);
    }

    void* ptr = pool->buffer + pool->used;
    pool->used += size;
    return ptr;
}

void mempool_destroy(mempool_t* pool) {
    if (pool->next) mempool_destroy(pool->next);
    free(pool->buffer);
    free(pool);
}
