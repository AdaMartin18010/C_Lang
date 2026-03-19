/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\README.md
 * Line: 374
 * Language: c
 * Block ID: bfbaf676
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* core/memory_pool.h */
#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <stddef.h>
#include <stdint.h>

typedef struct memory_pool memory_pool_t;

/* 创建固定块大小的内存池 */
memory_pool_t* memory_pool_create(size_t block_size,
                                   size_t block_count,
                                   size_t alignment);
void memory_pool_destroy(memory_pool_t* pool);

/* O(1) 分配和释放 */
void* memory_pool_alloc(memory_pool_t* pool);
void memory_pool_free(memory_pool_t* pool, void* ptr);

/* 内存统计 */
size_t memory_pool_used_blocks(const memory_pool_t* pool);
size_t memory_pool_free_blocks(const memory_pool_t* pool);

#endif
