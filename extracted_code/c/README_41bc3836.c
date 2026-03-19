/*
 * Auto-generated from: 07_Modern_Toolchain\13_Comprehensive_Examples\README.md
 * Line: 405
 * Language: c
 * Block ID: 41bc3836
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* mempool.h - 内存池系统 */
#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct mempool mempool_t;

/* 分配策略 */
typedef enum {
    ALLOC_STRATEGY_LIFO,        /* 栈式分配 - 快 */
    ALLOC_STRATEGY_BEST_FIT,    /* 最佳匹配 - 省空间 */
    ALLOC_STRATEGY_SEQUENTIAL   /* 顺序分配 - 简单 */
} alloc_strategy_t;

/* 内存统计 */
typedef struct {
    size_t total_allocated;
    size_t total_used;
    size_t total_free;
    size_t block_count;
    size_t peak_usage;
    uint32_t allocation_count;
    uint32_t deallocation_count;
} mempool_stats_t;

/* API */
mempool_t *mempool_create(size_t block_size,
                           size_t block_count,
                           alloc_strategy_t strategy);
void mempool_destroy(mempool_t *pool);

void *mempool_alloc(mempool_t *pool);
void mempool_free(mempool_t *pool, void *ptr);

/* 统计 */
void mempool_get_stats(mempool_t *pool, mempool_stats_t *stats);
void mempool_print_stats(mempool_t *pool);

/* 调试 */
bool mempool_contains(mempool_t *pool, void *ptr);
void mempool_validate(mempool_t *pool);

#endif /* MEMPOOL_H */
