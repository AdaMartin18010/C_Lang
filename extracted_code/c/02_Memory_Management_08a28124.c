/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 1040
 * Language: c
 * Block ID: 08a28124
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 固定大小内存池实现 - 适用于频繁分配/释放相同大小对象的场景
 */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

// 内存块大小（不包含元数据）
#define POOL_BLOCK_SIZE 64
#define POOL_BLOCKS_PER_CHUNK 1024

typedef struct PoolBlock {
    union {
        char data[POOL_BLOCK_SIZE];           // 用户数据区域
        struct PoolBlock *next_free;          // 空闲时下个空闲块指针
    };
} PoolBlock;

typedef struct PoolChunk {
    struct PoolChunk *next;                   // 下一个chunk
    PoolBlock blocks[POOL_BLOCKS_PER_CHUNK];  // 实际内存块数组
} PoolChunk;

typedef struct {
    PoolChunk *chunks;        // chunk链表头
    PoolBlock *free_list;     // 空闲块链表
    size_t num_chunks;        // chunk数量
    size_t num_free;          // 当前空闲块数
    size_t num_allocated;     // 当前已分配块数
} MemoryPool;

// 初始化内存池
int mempool_init(MemoryPool *pool) {
    pool->chunks = NULL;
    pool->free_list = NULL;
    pool->num_chunks = 0;
    pool->num_free = 0;
    pool->num_allocated = 0;
    return 0;
}

// 销毁内存池
void mempool_destroy(MemoryPool *pool) {
    PoolChunk *chunk = pool->chunks;
    while (chunk != NULL) {
        PoolChunk *next = chunk->next;
        free(chunk);
        chunk = next;
    }
    pool->chunks = NULL;
    pool->free_list = NULL;
}

// 内部: 分配新chunk
static int mempool_grow(MemoryPool *pool) {
    PoolChunk *new_chunk = malloc(sizeof(PoolChunk));
    if (new_chunk == NULL) return -1;

    new_chunk->next = pool->chunks;
    pool->chunks = new_chunk;
    pool->num_chunks++;

    // 将新chunk的所有块加入空闲链表
    for (int i = POOL_BLOCKS_PER_CHUNK - 1; i >= 0; i--) {
        new_chunk->blocks[i].next_free = pool->free_list;
        pool->free_list = &new_chunk->blocks[i];
    }
    pool->num_free += POOL_BLOCKS_PER_CHUNK;

    return 0;
}

// 从池中分配
void *mempool_alloc(MemoryPool *pool) {
    if (pool->free_list == NULL) {
        if (mempool_grow(pool) != 0) return NULL;
    }

    PoolBlock *block = pool->free_list;
    pool->free_list = block->next_free;
    pool->num_free--;
    pool->num_allocated++;

    return block->data;
}

// 释放回池中（极其快速）
void mempool_free(MemoryPool *pool, void *ptr) {
    if (ptr == NULL) return;

    PoolBlock *block = (PoolBlock*)ptr;
    block->next_free = pool->free_list;
    pool->free_list = block;
    pool->num_free++;
    pool->num_allocated--;
}

// 获取统计信息
void mempool_stats(MemoryPool *pool, size_t *chunks, size_t *free, size_t *allocated) {
    if (chunks) *chunks = pool->num_chunks;
    if (free) *free = pool->num_free;
    if (allocated) *allocated = pool->num_allocated;
}

/*
 * 使用示例
 */
typedef struct {
    int x, y;
    char data[56];  // 确保总大小不超过POOL_BLOCK_SIZE
} MyObject;

void example_usage(void) {
    MemoryPool pool;
    mempool_init(&pool);

    // 快速分配
    MyObject *obj1 = mempool_alloc(&pool);
    MyObject *obj2 = mempool_alloc(&pool);

    // 使用...
    obj1->x = 10;
    obj2->y = 20;

    // 快速释放（无需系统调用）
    mempool_free(&pool, obj1);
    mempool_free(&pool, obj2);

    mempool_destroy(&pool);
}
