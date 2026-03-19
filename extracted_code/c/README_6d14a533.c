/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\README.md
 * Line: 403
 * Language: c
 * Block ID: 6d14a533
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* core/memory_pool.c */
#include "memory_pool.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* 空闲块链表节点 */
typedef struct free_block {
    struct free_block* next;
} free_block_t;

struct memory_pool {
    uint8_t* buffer;          /* 原始内存块 */
    size_t block_size;
    size_t block_count;
    size_t alignment;
    free_block_t* free_list;  /* 空闲链表 */
    size_t used_count;
};

memory_pool_t* memory_pool_create(size_t block_size,
                                   size_t block_count,
                                   size_t alignment) {
    /* 确保块大小至少能存储一个指针 */
    if (block_size < sizeof(void*)) {
        block_size = sizeof(void*);
    }

    /* 对齐块大小 */
    size_t aligned_block_size = (block_size + alignment - 1) & ~(alignment - 1);

    memory_pool_t* pool = malloc(sizeof(memory_pool_t));
    pool->block_size = aligned_block_size;
    pool->block_count = block_count;
    pool->alignment = alignment;
    pool->used_count = 0;

    /* 分配对齐的内存 */
    size_t total_size = aligned_block_size * block_count + alignment;
    pool->buffer = aligned_alloc(alignment, total_size);

    /* 初始化空闲链表 */
    pool->free_list = NULL;
    for (size_t i = 0; i < block_count; i++) {
        free_block_t* block = (free_block_t*)(pool->buffer + i * aligned_block_size);
        block->next = pool->free_list;
        pool->free_list = block;
    }

    return pool;
}

void* memory_pool_alloc(memory_pool_t* pool) {
    if (!pool->free_list) {
        return NULL;  /* 池已满 */
    }

    free_block_t* block = pool->free_list;
    pool->free_list = block->next;
    pool->used_count++;

    /* 清零内存 */
    memset(block, 0, pool->block_size);
    return block;
}

void memory_pool_free(memory_pool_t* pool, void* ptr) {
    if (!ptr) return;

    /* 验证指针属于此池 */
    uint8_t* byte_ptr = ptr;
    if (byte_ptr < pool->buffer ||
        byte_ptr >= pool->buffer + pool->block_size * pool->block_count) {
        assert(0 && "Pointer does not belong to this pool");
        return;
    }

    free_block_t* block = ptr;
    block->next = pool->free_list;
    pool->free_list = block;
    pool->used_count--;
}
