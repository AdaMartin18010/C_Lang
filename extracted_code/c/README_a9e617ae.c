/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\README.md
 * Line: 162
 * Language: c
 * Block ID: a9e617ae
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ecs/archetype.h - 原型存储实现 */
#ifndef ARCHETYPE_H
#define ARCHETYPE_H

#include "component.h"

#define MAX_COMPONENTS_PER_ARCHETYPE 16
#define ARCHETYPE_CHUNK_SIZE 16384  /* 16KB 内存块 */

/* Archetype 标识一种组件组合 */
typedef struct archetype {
    component_type_t components[MAX_COMPONENTS_PER_ARCHETYPE];
    uint32_t component_count;
    uint32_t component_sizes[MAX_COMPONENTS_PER_ARCHETYPE];
    uint32_t entity_size;  /* 单个实体占用的总字节数 */

    /* 内存块链表 */
    struct chunk* chunks;
    uint32_t chunk_count;
    uint32_t entity_count;
} archetype_t;

/* 内存块 - 使用 SOA (Structure of Arrays) 布局 */
typedef struct chunk {
    uint8_t* data;           /* 组件数据 */
    uint32_t capacity;       /* 最大实体数 */
    uint32_t count;          /* 当前实体数 */
    struct chunk* next;
} chunk_t;

/* 创建 Archetype */
archetype_t* archetype_create(const component_type_t* types,
                               const uint32_t* sizes,
                               uint32_t count);

/* 在 Archetype 中创建实体 */
entity_id_t archetype_spawn(archetype_t* archetype);

/* 获取组件指针 - O(1) 访问 */
void* archetype_get_component(archetype_t* archetype,
                               chunk_t* chunk,
                               uint32_t index_in_chunk,
                               component_type_t component_type);

/* 遍历所有实体 */
#define ARCHETYPE_FOR_EACH(archetype, chunk_var, index_var) \
    for (chunk_t* chunk_var = (archetype)->chunks; chunk_var; chunk_var = chunk_var->next) \
        for (uint32_t index_var = 0; index_var < chunk_var->count; index_var++)

#endif
