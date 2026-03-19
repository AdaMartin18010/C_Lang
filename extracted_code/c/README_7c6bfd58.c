/*
 * Auto-generated from: 07_Modern_Toolchain\13_Comprehensive_Examples\README.md
 * Line: 454
 * Language: c
 * Block ID: 7c6bfd58
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* mempool.c */
#include "mempool.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

/* 内存块头部 */
typedef struct block_header {
    struct block_header *next;
    bool in_use;
    uint32_t magic;         /* 魔数用于检测越界 */
} block_header_t;

#define BLOCK_MAGIC 0xDEADBEEF

struct mempool {
    size_t block_size;
    size_t block_count;
    size_t actual_block_size;  /* 包含头部的实际大小 */
    alloc_strategy_t strategy;

    /* 内存区域 */
    uint8_t *memory;
    size_t memory_size;

    /* 空闲链表 */
    block_header_t *free_list;

    /* 统计 */
    mempool_stats_t stats;

    /* 线程安全 */
    pthread_mutex_t lock;

    /* 调试 */
    #ifdef MEMPOOL_DEBUG
    block_header_t **allocated_blocks;
    size_t allocated_count;
    #endif
};

mempool_t *mempool_create(size_t block_size,
                           size_t block_count,
                           alloc_strategy_t strategy) {
    mempool_t *pool = calloc(1, sizeof(mempool_t));
    if (!pool) return NULL;

    pool->block_size = block_size;
    pool->block_count = block_count;
    pool->strategy = strategy;
    pool->actual_block_size = sizeof(block_header_t) + block_size;

    /* 对齐到8字节边界 */
    pool->actual_block_size = (pool->actual_block_size + 7) & ~7;

    /* 分配内存 */
    pool->memory_size = pool->actual_block_size * block_count;
    pool->memory = aligned_alloc(64, pool->memory_size);
    if (!pool->memory) {
        free(pool);
        return NULL;
    }

    /* 初始化所有块到空闲链表 */
    pool->free_list = NULL;
    for (size_t i = 0; i < block_count; i++) {
        block_header_t *header = (block_header_t *)(pool->memory +
                                                     i * pool->actual_block_size);
        header->in_use = false;
        header->magic = BLOCK_MAGIC;
        header->next = pool->free_list;
        pool->free_list = header;
    }

    pool->stats.total_allocated = pool->memory_size;
    pool->stats.total_free = block_count;
    pool->stats.block_count = block_count;

    pthread_mutex_init(&pool->lock, NULL);

    return pool;
}

void *mempool_alloc(mempool_t *pool) {
    if (!pool) return NULL;

    pthread_mutex_lock(&pool->lock);

    block_header_t *block = pool->free_list;
    if (!block) {
        pthread_mutex_unlock(&pool->lock);
        return NULL;  /* 池耗尽 */
    }

    /* 从空闲链表移除 */
    pool->free_list = block->next;
    block->in_use = true;
    block->next = NULL;

    /* 更新统计 */
    pool->stats.total_free--;
    pool->stats.total_used++;
    pool->stats.allocation_count++;

    size_t current_usage = pool->stats.total_used * pool->block_size;
    if (current_usage > pool->stats.peak_usage) {
        pool->stats.peak_usage = current_usage;
    }

    pthread_mutex_unlock(&pool->lock);

    /* 返回数据区域 */
    return (void *)(block + 1);
}

void mempool_free(mempool_t *pool, void *ptr) {
    if (!pool || !ptr) return;

    block_header_t *block = (block_header_t *)ptr - 1;

    /* 验证魔数 */
    assert(block->magic == BLOCK_MAGIC);
    assert(block->in_use);

    pthread_mutex_lock(&pool->lock);

    /* 放回空闲链表 (LIFO策略) */
    block->in_use = false;
    block->next = pool->free_list;
    pool->free_list = block;

    /* 更新统计 */
    pool->stats.total_free++;
    pool->stats.total_used--;
    pool->stats.deallocation_count++;

    pthread_mutex_unlock(&pool->lock);
}

void mempool_destroy(mempool_t *pool) {
    if (!pool) return;

    /* 检查内存泄漏 */
    if (pool->stats.total_used > 0) {
        fprintf(stderr, "Warning: %zu blocks still in use!\n",
                pool->stats.total_used);
    }

    pthread_mutex_destroy(&pool->lock);
    free(pool->memory);
    free(pool);
}

void mempool_print_stats(mempool_t *pool) {
    if (!pool) return;

    printf("\n=== Memory Pool Statistics ===\n");
    printf("Block Size:      %zu bytes\n", pool->block_size);
    printf("Block Count:     %zu\n", pool->block_count);
    printf("Total Memory:    %zu bytes\n", pool->stats.total_allocated);
    printf("Used Blocks:     %zu\n", pool->stats.total_used);
    printf("Free Blocks:     %zu\n", pool->stats.total_free);
    printf("Peak Usage:      %zu bytes\n", pool->stats.peak_usage);
    printf("Allocations:     %u\n", pool->stats.allocation_count);
    printf("Deallocations:   %u\n", pool->stats.deallocation_count);
    printf("Utilization:     %.1f%%\n",
           100.0 * pool->stats.total_used / pool->stats.block_count);
    printf("==============================\n");
}
