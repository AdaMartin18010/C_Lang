/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 563
 * Language: c
 * Block ID: 24e9799e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 固定块内存池 - 无碎片、确定性的内存分配
 * ============================================================================ */

/* fixed_pool.h */
#ifndef FIXED_POOL_H
#define FIXED_POOL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* 内存池句柄 */
typedef struct {
    uint8_t *pool;            /* 内存池基地址 */
    uint8_t *used_map;        /* 使用位图 */
    uint16_t block_size;      /* 块大小 */
    uint16_t num_blocks;      /* 块数量 */
    uint16_t used_count;      /* 已使用块数 */
} fixed_pool_t;

/* 初始化内存池 */
bool fixed_pool_init(fixed_pool_t *pool, void *buffer, uint16_t buffer_size,
                     uint16_t block_size);

/* 分配内存 */
void *fixed_pool_alloc(fixed_pool_t *pool);

/* 释放内存 */
void fixed_pool_free(fixed_pool_t *pool, void *ptr);

/* 获取空闲块数 */
uint16_t fixed_pool_free_count(const fixed_pool_t *pool);

/* 检查指针是否属于该池 */
bool fixed_pool_contains(const fixed_pool_t *pool, const void *ptr);

#endif /* FIXED_POOL_H */

/* fixed_pool.c */
#include "fixed_pool.h"
#include <string.h>

/* 计算所需的位图大小 */
#define BITMAP_SIZE(n) (((n) + 7) / 8)

/* 设置位图中的位 */
static inline void bitmap_set(uint8_t *bitmap, uint16_t index)
{
    bitmap[index / 8] |= (1 << (index % 8));
}

/* 清除位图中的位 */
static inline void bitmap_clear(uint8_t *bitmap, uint16_t index)
{
    bitmap[index / 8] &= ~(1 << (index % 8));
}

/* 检查位图中的位 */
static inline bool bitmap_test(const uint8_t *bitmap, uint16_t index)
{
    return (bitmap[index / 8] & (1 << (index % 8))) != 0;
}

/* 初始化内存池 */
bool fixed_pool_init(fixed_pool_t *pool, void *buffer, uint16_t buffer_size,
                     uint16_t block_size)
{
    if (!pool || !buffer || block_size == 0) {
        return false;
    }

    /* 确保块大小至少为指针大小 (用于对齐) */
    if (block_size < sizeof(void *)) {
        block_size = sizeof(void *);
    }

    /* 计算可能的块数量 */
    /* 一部分空间用于位图，剩余用于数据 */
    uint16_t max_blocks = buffer_size / block_size;
    uint16_t bitmap_bytes = BITMAP_SIZE(max_blocks);

    /* 调整块数以容纳位图 */
    while ((max_blocks * block_size + bitmap_bytes) > buffer_size
           && max_blocks > 0) {
        max_blocks--;
        bitmap_bytes = BITMAP_SIZE(max_blocks);
    }

    if (max_blocks == 0) {
        return false;
    }

    /* 设置位图位置在池起始处 */
    pool->used_map = buffer;
    pool->pool = (uint8_t *)buffer + bitmap_bytes;
    pool->block_size = block_size;
    pool->num_blocks = max_blocks;
    pool->used_count = 0;

    /* 清零位图 */
    memset(pool->used_map, 0, bitmap_bytes);

    return true;
}

/* 分配内存 - O(n) 但无碎片 */
void *fixed_pool_alloc(fixed_pool_t *pool)
{
    if (!pool) return NULL;

    /* 查找空闲块 */
    for (uint16_t i = 0; i < pool->num_blocks; i++) {
        if (!bitmap_test(pool->used_map, i)) {
            bitmap_set(pool->used_map, i);
            pool->used_count++;
            return pool->pool + (i * pool->block_size);
        }
    }

    return NULL;  /* 无可用块 */
}

/* 释放内存 */
void fixed_pool_free(fixed_pool_t *pool, void *ptr)
{
    if (!pool || !ptr) return;

    /* 检查指针是否在池范围内 */
    uint8_t *p = ptr;
    if (p < pool->pool || p >= pool->pool + (pool->num_blocks * pool->block_size)) {
        return;  /* 非法指针 */
    }

    /* 检查对齐 */
    uint16_t offset = p - pool->pool;
    if (offset % pool->block_size != 0) {
        return;  /* 未对齐 */
    }

    uint16_t index = offset / pool->block_size;
    if (bitmap_test(pool->used_map, index)) {
        bitmap_clear(pool->used_map, index);
        pool->used_count--;
    }
}

/* 获取空闲块数 */
uint16_t fixed_pool_free_count(const fixed_pool_t *pool)
{
    if (!pool) return 0;
    return pool->num_blocks - pool->used_count;
}

/* 检查指针是否属于该池 */
bool fixed_pool_contains(const fixed_pool_t *pool, const void *ptr)
{
    if (!pool || !ptr) return false;

    const uint8_t *p = ptr;
    return (p >= pool->pool &&
            p < pool->pool + (pool->num_blocks * pool->block_size));
}

/* ============================================================================
 * 使用示例
 * ============================================================================ */

#ifdef EXAMPLE_USAGE

#include <stdio.h>

/* 定义池大小 */
#define POOL_BUFFER_SIZE 1024
#define BLOCK_SIZE 32
#define NUM_BLOCKS 28  /* ~1024 / 32，略少以容纳开销 */

/* 池缓冲区 */
static uint8_t pool_buffer[POOL_BUFFER_SIZE];
static fixed_pool_t my_pool;

/* 测试结构 */
typedef struct {
    uint16_t id;
    uint8_t data[24];
    uint8_t flags;
} packet_t;

void example_usage(void)
{
    /* 初始化池 */
    if (!fixed_pool_init(&my_pool, pool_buffer, POOL_BUFFER_SIZE, BLOCK_SIZE)) {
        printf("Pool init failed!\n");
        return;
    }

    printf("Pool: %d blocks of %d bytes\n",
           my_pool.num_blocks, my_pool.block_size);

    /* 分配 */
    packet_t *pkt1 = fixed_pool_alloc(&my_pool);
    packet_t *pkt2 = fixed_pool_alloc(&my_pool);

    if (pkt1 && pkt2) {
        pkt1->id = 1;
        pkt2->id = 2;

        printf("Allocated: pkt1=%p, pkt2=%p\n", (void*)pkt1, (void*)pkt2);
        printf("Free blocks: %d\n", fixed_pool_free_count(&my_pool));
    }

    /* 释放 */
    fixed_pool_free(&my_pool, pkt1);
    printf("After free: %d free blocks\n", fixed_pool_free_count(&my_pool));
}

#endif /* EXAMPLE_USAGE */
