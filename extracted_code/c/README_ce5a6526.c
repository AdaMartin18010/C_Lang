/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\README.md
 * Line: 295
 * Language: c
 * Block ID: ce5a6526
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// fixed_pool.h - 固定块内存池
#ifndef FIXED_POOL_H
#define FIXED_POOL_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t *pool;
    uint8_t *used_map;
    uint16_t block_size;
    uint16_t num_blocks;
    uint16_t used_count;
} fixed_pool_t;

bool fixed_pool_init(fixed_pool_t *pool, void *buffer,
                     uint16_t buffer_size, uint16_t block_size);
void *fixed_pool_alloc(fixed_pool_t *pool);
void fixed_pool_free(fixed_pool_t *pool, void *ptr);

#endif
