/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\README.md
 * Line: 217
 * Language: c
 * Block ID: 651a4a70
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ecs/systems/movement_system.c */
#include "ecs/systems/movement_system.h"
#include "ecs/archetype.h"
#include "core/time.h"

/* 查询所有包含 Transform 和 Rigidbody 的 Archetypes */
void movement_system_update(world_t* world, float delta_time) {
    component_type_t query[] = {
        COMPONENT_TRANSFORM,
        COMPONENT_RIGIDBODY
    };

    archetype_list_t* archetypes = world_query(world, query, 2);

    for (uint32_t a = 0; a < archetypes->count; a++) {
        archetype_t* arch = archetypes->items[a];

        ARCHETYPE_FOR_EACH(arch, chunk, i) {
            /* SOA 布局允许 SIMD 优化 */
            transform_component_t* transforms = archetype_get_component(
                arch, chunk, i, COMPONENT_TRANSFORM);
            rigidbody_component_t* rigidbodies = archetype_get_component(
                arch, chunk, i, COMPONENT_RIGIDBODY);

            /* 批量更新 - 缓存友好的线性访问 */
            for (uint32_t j = 0; j < chunk->count; j++) {
                if (rigidbodies[j].is_kinematic) continue;

                /* velocity = velocity + acceleration * dt */
                rigidbodies[j].velocity[0] += rigidbodies[j].acceleration[0] * delta_time;
                rigidbodies[j].velocity[1] += rigidbodies[j].acceleration[1] * delta_time;
                rigidbodies[j].velocity[2] += rigidbodies[j].acceleration[2] * delta_time;

                /* position = position + velocity * dt */
                transforms[j].position[0] += rigidbodies[j].velocity[0] * delta_time;
                transforms[j].position[1] += rigidbodies[j].velocity[1] * delta_time;
                transforms[j].position[2] += rigidbodies[j].velocity[2] * delta_time;
            }
        }
    }
}
