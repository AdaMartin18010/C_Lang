/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\05_Interview_Preparation_Path.md
 * Line: 278
 * Language: c
 * Block ID: c165258a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct MemoryPool {
    // 固定大小的内存块
    size_t block_size;
    size_t block_count;

    // 空闲链表
    _Atomic(void *) free_list;

    // 大块内存（用于分配）
    void *memory;
    size_t used;
    pthread_mutex_t lock;
} MemoryPool;

// 关键设计点：
// 1. 预分配大块内存，减少系统调用
// 2. 使用原子操作实现无锁free list（或细粒度锁）
// 3. 固定大小分配，避免碎片
// 4. 对齐到缓存行，避免伪共享

void *pool_alloc(MemoryPool *pool) {
    // 尝试从无锁链表获取
    void *block = atomic_load(&pool->free_list);
    while (block != NULL) {
        void *next = *(void **)block;
        if (atomic_compare_exchange_weak(&pool->free_list,
                                          &block, next)) {
            return block;
        }
    }

    // 链表为空，从大块分配
    pthread_mutex_lock(&pool->lock);
    if (pool->used + pool->block_size <= pool->block_count * pool->block_size) {
        void *ptr = (char *)pool->memory + pool->used;
        pool->used += pool->block_size;
        pthread_mutex_unlock(&pool->lock);
        return ptr;
    }
    pthread_mutex_unlock(&pool->lock);
    return NULL;  // 内存池耗尽
}

void pool_free(MemoryPool *pool, void *ptr) {
    // 压入无锁链表
    void *old_head = atomic_load(&pool->free_list);
    do {
        *(void **)ptr = old_head;
    } while (!atomic_compare_exchange_weak(&pool->free_list,
                                            &old_head, ptr));
}
