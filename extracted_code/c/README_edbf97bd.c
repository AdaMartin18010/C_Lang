/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\README.md
 * Line: 117
 * Language: c
 * Block ID: edbf97bd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ecs/component.h */
#ifndef COMPONENT_H
#define COMPONENT_H

#include <stdint.h>
#include <stdbool.h>

/* 组件类型 ID */
typedef uint32_t component_type_t;

/* 实体 ID - 使用 32 位整数，高 16 位为版本号 */
typedef uint32_t entity_id_t;
#define ENTITY_INDEX(e) ((e) & 0xFFFF)
#define ENTITY_VERSION(e) ((e) >> 16)
#define MAKE_ENTITY(index, version) (((version) << 16) | (index))

/* 变换组件 - 连续内存布局，缓存友好 */
typedef struct {
    float position[3];
    float rotation[4];  /* 四元数 */
    float scale[3];
} transform_component_t;

/* 刚体组件 */
typedef struct {
    float velocity[3];
    float acceleration[3];
    float mass;
    bool is_kinematic;
} rigidbody_component_t;

/* 渲染组件 */
typedef struct {
    uint32_t mesh_handle;
    uint32_t material_handle;
    uint32_t layer;
    bool visible;
} render_component_t;

#endif
