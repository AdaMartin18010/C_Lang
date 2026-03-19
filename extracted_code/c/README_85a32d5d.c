/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\README.md
 * Line: 319
 * Language: c
 * Block ID: 85a32d5d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// fixed_pool.c
#include "fixed_pool.h"
#include <string.h>

#define BITMAP_SIZE(n) (((n) + 7) / 8)

static inline void bitmap_set(uint8_t *bitmap, uint16_t index) {
    bitmap[index / 8] |= (1 << (index % 8));
}

static inline void bitmap_clear(uint8_t *bitmap, uint16_t index) {
    bitmap[index / 8] &= ~(1 << (index % 8));
}

static inline bool bitmap_test(const uint8_t *bitmap, uint16_t index) {
    return (bitmap[index / 8] & (1 << (index % 8))) != 0;
}

bool fixed_pool_init(fixed_pool_t *pool, void *buffer,
                     uint16_t buffer_size, uint16_t block_size) {
    if (!pool || !buffer || block_size == 0) return false;
    if (block_size < sizeof(void *)) block_size = sizeof(void *);

    uint16_t max_blocks = buffer_size / block_size;
    uint16_t bitmap_bytes = BITMAP_SIZE(max_blocks);

    while ((max_blocks * block_size + bitmap_bytes) > buffer_size && max_blocks > 0) {
        max_blocks--;
        bitmap_bytes = BITMAP_SIZE(max_blocks);
    }

    if (max_blocks == 0) return false;

    pool->used_map = buffer;
    pool->pool = (uint8_t *)buffer + bitmap_bytes;
    pool->block_size = block_size;
    pool->num_blocks = max_blocks;
    pool->used_count = 0;

    memset(pool->used_map, 0, bitmap_bytes);
    return true;
}

void *fixed_pool_alloc(fixed_pool_t *pool) {
    if (!pool) return NULL;

    for (uint16_t i = 0; i < pool->num_blocks; i++) {
        if (!bitmap_test(pool->used_map, i)) {
            bitmap_set(pool->used_map, i);
            pool->used_count++;
            return pool->pool + (i * pool->block_size);
        }
    }
    return NULL;
}

void fixed_pool_free(fixed_pool_t *pool, void *ptr) {
    if (!pool || !ptr) return;

    uint8_t *p = ptr;
    if (p < pool->pool) return;

    uint16_t offset = p - pool->pool;
    if (offset % pool->block_size != 0) return;

    uint16_t index = offset / pool->block_size;
    if (index >= pool->num_blocks) return;

    if (bitmap_test(pool->used_map, index)) {
        bitmap_clear(pool->used_map, index);
        pool->used_count--;
    }
}
