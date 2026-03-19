/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\03_Memory_Management.md
 * Line: 643
 * Language: c
 * Block ID: 6eb7a3f2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void *pool_alloc(MemoryPool *pool) {
    // 空闲链表为空，分配新的大块
    if (pool->free_list == NULL) {
        size_t chunk_size = pool->block_size * pool->blocks_per_chunk;
        char *chunk = malloc(chunk_size);
        if (chunk == NULL) return NULL;

        // 将新大块加入chunks列表
        *(void**)chunk = pool->chunks;
        pool->chunks = chunk;

        // 切分并加入空闲链表
        for (size_t i = 0; i < pool->blocks_per_chunk; i++) {
            PoolBlock *block = (PoolBlock*)(chunk + i * pool->block_size);
            block->next = pool->free_list;
            pool->free_list = block;
        }
    }

    // 从空闲链表取一个块
    PoolBlock *block = pool->free_list;
    pool->free_list = block->next;
    return block;
}

void pool_free(MemoryPool *pool, void *ptr) {
    if (ptr == NULL) return;
    PoolBlock *block = ptr;
    block->next = pool->free_list;
    pool->free_list = block;
}
