/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\02_DMA_Offload.md
 * Line: 854
 * Language: c
 * Block ID: 62a405a3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 描述符池管理器 - 高效分配/释放DMA描述符
 * ============================================================================ */

#include <string.h>

#define DESC_POOL_DEFAULT_SIZE  1024
#define DESC_ALIGNMENT          64

/* 描述符池结构 */
typedef struct desc_pool {
    dma_descriptor_t *pool;             /* 描述符数组 */
    uint32_t size;                      /* 池大小 */
    uint8_t *bitmap;                    /* 分配位图 */
    uint32_t free_count;                /* 空闲数量 */
    atomic_flag lock;                   /* 锁 */
} desc_pool_t;

/* 初始化描述符池 */
int desc_pool_init(desc_pool_t *pool, uint32_t size)
{
    if (!pool || size == 0) {
        return -1;
    }

    /* 分配对齐的描述符内存 */
    pool->pool = aligned_alloc(DESC_ALIGNMENT,
                               size * sizeof(dma_descriptor_t));
    if (!pool->pool) {
        return -1;
    }

    /* 分配位图 (每8个描述符1字节) */
    uint32_t bitmap_size = (size + 7) / 8;
    pool->bitmap = calloc(1, bitmap_size);
    if (!pool->bitmap) {
        free(pool->pool);
        return -1;
    }

    pool->size = size;
    pool->free_count = size;
    atomic_flag_clear(&pool->lock);

    /* 清零描述符 */
    memset(pool->pool, 0, size * sizeof(dma_descriptor_t));

    return 0;
}

/* 分配描述符 */
dma_descriptor_t *desc_alloc(desc_pool_t *pool)
{
    if (!pool) return NULL;

    while (atomic_flag_test_and_set(&pool->lock)) {
        /* 自旋等待 */
    }

    dma_descriptor_t *desc = NULL;

    if (pool->free_count > 0) {
        for (uint32_t i = 0; i < pool->size; i++) {
            uint32_t byte_idx = i / 8;
            uint32_t bit_idx = i % 8;

            if (!(pool->bitmap[byte_idx] & (1U << bit_idx))) {
                /* 找到空闲描述符 */
                pool->bitmap[byte_idx] |= (1U << bit_idx);
                pool->free_count--;
                desc = &pool->pool[i];
                break;
            }
        }
    }

    atomic_flag_clear(&pool->lock);
    return desc;
}

/* 批量分配描述符 */
uint32_t desc_alloc_batch(desc_pool_t *pool, dma_descriptor_t **descs,
                           uint32_t count)
{
    if (!pool || !descs || count == 0) {
        return 0;
    }

    while (atomic_flag_test_and_set(&pool->lock)) {
        /* 自旋等待 */
    }

    uint32_t allocated = 0;

    for (uint32_t i = 0; i < pool->size && allocated < count; i++) {
        uint32_t byte_idx = i / 8;
        uint32_t bit_idx = i % 8;

        if (!(pool->bitmap[byte_idx] & (1U << bit_idx))) {
            pool->bitmap[byte_idx] |= (1U << bit_idx);
            descs[allocated++] = &pool->pool[i];
        }
    }

    pool->free_count -= allocated;
    atomic_flag_clear(&pool->lock);

    return allocated;
}

/* 释放描述符 */
void desc_free(desc_pool_t *pool, dma_descriptor_t *desc)
{
    if (!pool || !desc) return;

    /* 计算索引 */
    ptrdiff_t idx = desc - pool->pool;
    if (idx < 0 || idx >= (ptrdiff_t)pool->size) {
        return;  /* 无效描述符 */
    }

    while (atomic_flag_test_and_set(&pool->lock)) {
        /* 自旋等待 */
    }

    uint32_t byte_idx = (uint32_t)idx / 8;
    uint32_t bit_idx = (uint32_t)idx % 8;

    if (pool->bitmap[byte_idx] & (1U << bit_idx)) {
        pool->bitmap[byte_idx] &= ~(1U << bit_idx);
        pool->free_count++;

        /* 清零描述符 */
        memset(desc, 0, sizeof(dma_descriptor_t));
    }

    atomic_flag_clear(&pool->lock);
}

/* 获取池统计 */
desc_pool_stats_t desc_pool_get_stats(desc_pool_t *pool)
{
    desc_pool_stats_t stats = {0};
    if (!pool) return stats;

    stats.total_desc = pool->size;
    stats.free_desc = pool->free_count;
    stats.pending_desc = pool->size - pool->free_count;

    return stats;
}
