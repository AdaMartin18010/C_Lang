/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\03_Memory_Management.md
 * Line: 586
 * Language: c
 * Block ID: 72b456aa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct PoolBlock {
    struct PoolBlock *next;
} PoolBlock;

typedef struct {
    size_t block_size;
    size_t blocks_per_chunk;
    PoolBlock *free_list;
    void *chunks;  // 大块内存列表
} MemoryPool;

// 初始化内存池
void pool_init(MemoryPool *pool, size_t block_size, size_t blocks) {
    pool->block_size = block_size < sizeof(PoolBlock)
                       ? sizeof(PoolBlock) : block_size;
    pool->blocks_per_chunk = blocks;
    pool->free_list = NULL;
    pool->chunks = NULL;
}
