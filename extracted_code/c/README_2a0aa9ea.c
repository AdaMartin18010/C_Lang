/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\README.md
 * Line: 490
 * Language: c
 * Block ID: 2a0aa9ea
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* core/object_pool.h - 类型安全的对象池 */
#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include "memory_pool.h"

#define DEFINE_OBJECT_POOL(type, pool_name) \
    static memory_pool_t* pool_name = NULL; \
    \
    void pool_name##_init(size_t count) { \
        pool_name = memory_pool_create(sizeof(type), count, _Alignof(type)); \
    } \
    \
    type* pool_name##_acquire(void) { \
        return (type*)memory_pool_alloc(pool_name); \
    } \
    \
    void pool_name##_release(type* obj) { \
        memory_pool_free(pool_name, obj); \
    }

#endif

/* 使用示例 */
/* particle.c */
#include "core/object_pool.h"

typedef struct {
    float position[3];
    float velocity[3];
    float lifetime;
    uint32_t color;
} particle_t;

DEFINE_OBJECT_POOL(particle_t, particle_pool)

void particle_system_init(void) {
    particle_pool_init(10000);  /* 预分配 10000 个粒子 */
}

particle_t* particle_spawn(const float* pos, const float* vel) {
    particle_t* p = particle_pool_acquire();
    if (p) {
        memcpy(p->position, pos, sizeof(float) * 3);
        memcpy(p->velocity, vel, sizeof(float) * 3);
        p->lifetime = 2.0f;
        p->color = 0xFFFFFFFF;
    }
    return p;
}
