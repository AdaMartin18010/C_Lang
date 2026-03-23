# C 语言游戏引擎开发

本目录包含使用 C 语言开发游戏引擎的核心技术与最佳实践，涵盖从底层架构到高级系统设计的完整知识体系。

---

## 目录

- [C 语言游戏引擎开发](#c-语言游戏引擎开发)
  - [目录](#目录)
  - [1. 游戏引擎架构概述](#1-游戏引擎架构概述)
    - [1.1 分层架构设计](#11-分层架构设计)
    - [1.2 模块依赖关系](#12-模块依赖关系)
  - [2. ECS (Entity Component System) 设计](#2-ecs-entity-component-system-设计)
    - [2.1 ECS 核心概念](#21-ecs-核心概念)
    - [2.2 内存布局优化](#22-内存布局优化)
    - [2.3 Archetype 存储系统](#23-archetype-存储系统)
    - [2.4 System 实现](#24-system-实现)
  - [3. 游戏循环与帧率控制](#3-游戏循环与帧率控制)
    - [3.1 固定时间步长更新](#31-固定时间步长更新)
    - [3.2 游戏循环实现](#32-游戏循环实现)
  - [4. 内存池与对象池](#4-内存池与对象池)
    - [4.1 固定大小内存池](#41-固定大小内存池)
    - [4.2 内存池实现](#42-内存池实现)
    - [4.3 对象池模式](#43-对象池模式)
  - [5. 事件系统](#5-事件系统)
    - [5.1 事件总线设计](#51-事件总线设计)
    - [5.2 事件总线实现](#52-事件总线实现)
    - [5.3 事件类型定义](#53-事件类型定义)
  - [6. 资源管理](#6-资源管理)
    - [6.1 资源管理器架构](#61-资源管理器架构)
    - [6.2 资源管理器实现](#62-资源管理器实现)
  - [7. 渲染抽象层](#7-渲染抽象层)
    - [7.1 RHI (Rendering Hardware Interface) 设计](#71-rhi-rendering-hardware-interface-设计)
    - [7.2 渲染命令缓冲区](#72-渲染命令缓冲区)
    - [7.3 渲染循环示例](#73-渲染循环示例)
  - [参考资料](#参考资料)
  - [实践建议](#实践建议)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 1. 游戏引擎架构概述

### 1.1 分层架构设计

现代游戏引擎采用分层架构，实现关注点分离和模块化设计：

```
┌─────────────────────────────────────────────────────────────┐
│                      应用层 (Application)                    │
│                   游戏逻辑、AI、物理交互                      │
├─────────────────────────────────────────────────────────────┤
│                      脚本层 (Scripting)                      │
│              Lua/Python 绑定、热重载系统                     │
├─────────────────────────────────────────────────────────────┤
│                      核心层 (Core Systems)                   │
│    ECS、事件系统、资源管理、场景图、输入处理                  │
├─────────────────────────────────────────────────────────────┤
│                      中间件层 (Middleware)                   │
│      物理引擎 (Bullet)、音频 (FMOD)、网络 (ENet)             │
├─────────────────────────────────────────────────────────────┤
│                      平台抽象层 (Platform)                   │
│    渲染抽象 (RHI)、文件系统、线程、内存管理                   │
├─────────────────────────────────────────────────────────────┤
│                      硬件层 (Hardware)                       │
│         GPU、CPU、内存、存储设备、输入设备                    │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 模块依赖关系

```c
/* engine.h - 引擎主头文件 */
#ifndef ENGINE_H
#define ENGINE_H

#include "core/memory.h"      /* 内存管理必须在最底层 */
#include "core/log.h"         /* 日志系统 */
#include "core/time.h"        /* 时间系统 */
#include "platform/window.h"  /* 平台窗口 */
#include "renderer/rhi.h"     /* 渲染硬件接口 */
#include "ecs/world.h"        /* ECS 世界 */
#include "resources/manager.h" /* 资源管理 */
#include "events/bus.h"       /* 事件总线 */

typedef struct {
    memory_pool_t* global_pool;
    window_t* window;
    rhi_context_t* renderer;
    world_t* world;
    resource_manager_t* resources;
    event_bus_t* events;
    bool running;
} engine_t;

engine_t* engine_create(const engine_config_t* config);
void engine_run(engine_t* engine);
void engine_destroy(engine_t* engine);

#endif
```

---

## 2. ECS (Entity Component System) 设计

### 2.1 ECS 核心概念

ECS 是游戏引擎的架构模式，将数据与逻辑分离，提高缓存友好性和并行处理能力：

- **Entity (实体)**: 唯一的标识符，代表游戏对象
- **Component (组件)**: 纯数据结构，存储实体属性
- **System (系统)**: 处理具有特定组件的实体的逻辑

### 2.2 内存布局优化

```c
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
```

### 2.3 Archetype 存储系统

```c
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
```

### 2.4 System 实现

```c
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
```

---

## 3. 游戏循环与帧率控制

### 3.1 固定时间步长更新

```c
/* core/game_loop.h */
#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    double target_fps;
    double fixed_delta_time;    /* 固定物理更新间隔 */
    double max_delta_time;      /* 防止卡顿导致的时间跳跃 */

    /* 运行时数据 */
    double current_time;
    double accumulator;         /* 物理更新累积器 */
    uint64_t frame_count;
    double fps_update_interval;
    double last_fps_update;
    int current_fps;
} game_loop_t;

game_loop_t* game_loop_create(double target_fps, double fixed_hz);
void game_loop_run(game_loop_t* loop,
                    void (*update)(double),      /* 固定频率更新 */
                    void (*render)(double),      /* 插值渲染 */
                    bool (*should_quit)(void));

#endif
```

### 3.2 游戏循环实现

```c
/* core/game_loop.c */
#include "game_loop.h"
#include "time.h"
#include <stdio.h>

game_loop_t* game_loop_create(double target_fps, double fixed_hz) {
    game_loop_t* loop = malloc(sizeof(game_loop_t));
    loop->target_fps = target_fps;
    loop->fixed_delta_time = 1.0 / fixed_hz;
    loop->max_delta_time = 0.25;  /* 最大 250ms */
    loop->current_time = time_get_seconds();
    loop->accumulator = 0.0;
    loop->frame_count = 0;
    loop->fps_update_interval = 0.5;
    loop->last_fps_update = loop->current_time;
    loop->current_fps = 0;
    return loop;
}

void game_loop_run(game_loop_t* loop,
                    void (*update)(double),
                    void (*render)(double),
                    bool (*should_quit)(void)) {
    while (!should_quit()) {
        double new_time = time_get_seconds();
        double frame_time = new_time - loop->current_time;
        loop->current_time = new_time;

        /* 防止时间跳跃过大 */
        if (frame_time > loop->max_delta_time) {
            frame_time = loop->max_delta_time;
        }

        loop->accumulator += frame_time;

        /* 固定时间步长更新 - 保证物理确定性 */
        while (loop->accumulator >= loop->fixed_delta_time) {
            update(loop->fixed_delta_time);
            loop->accumulator -= loop->fixed_delta_time;
        }

        /* 插值系数 - 用于平滑渲染 */
        double alpha = loop->accumulator / loop->fixed_delta_time;
        render(alpha);

        /* FPS 计算 */
        loop->frame_count++;
        if (loop->current_time - loop->last_fps_update >= loop->fps_update_interval) {
            loop->current_fps = (int)(loop->frame_count /
                (loop->current_time - loop->last_fps_update));
            loop->frame_count = 0;
            loop->last_fps_update = loop->current_time;
            printf("FPS: %d\n", loop->current_fps);
        }

        /* 帧率限制 */
        if (loop->target_fps > 0) {
            double target_frame_time = 1.0 / loop->target_fps;
            double sleep_time = target_frame_time - (time_get_seconds() - new_time);
            if (sleep_time > 0) {
                time_sleep(sleep_time);
            }
        }
    }
}
```

---

## 4. 内存池与对象池

### 4.1 固定大小内存池

```c
/* core/memory_pool.h */
#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <stddef.h>
#include <stdint.h>

typedef struct memory_pool memory_pool_t;

/* 创建固定块大小的内存池 */
memory_pool_t* memory_pool_create(size_t block_size,
                                   size_t block_count,
                                   size_t alignment);
void memory_pool_destroy(memory_pool_t* pool);

/* O(1) 分配和释放 */
void* memory_pool_alloc(memory_pool_t* pool);
void memory_pool_free(memory_pool_t* pool, void* ptr);

/* 内存统计 */
size_t memory_pool_used_blocks(const memory_pool_t* pool);
size_t memory_pool_free_blocks(const memory_pool_t* pool);

#endif
```

### 4.2 内存池实现

```c
/* core/memory_pool.c */
#include "memory_pool.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* 空闲块链表节点 */
typedef struct free_block {
    struct free_block* next;
} free_block_t;

struct memory_pool {
    uint8_t* buffer;          /* 原始内存块 */
    size_t block_size;
    size_t block_count;
    size_t alignment;
    free_block_t* free_list;  /* 空闲链表 */
    size_t used_count;
};

memory_pool_t* memory_pool_create(size_t block_size,
                                   size_t block_count,
                                   size_t alignment) {
    /* 确保块大小至少能存储一个指针 */
    if (block_size < sizeof(void*)) {
        block_size = sizeof(void*);
    }

    /* 对齐块大小 */
    size_t aligned_block_size = (block_size + alignment - 1) & ~(alignment - 1);

    memory_pool_t* pool = malloc(sizeof(memory_pool_t));
    pool->block_size = aligned_block_size;
    pool->block_count = block_count;
    pool->alignment = alignment;
    pool->used_count = 0;

    /* 分配对齐的内存 */
    size_t total_size = aligned_block_size * block_count + alignment;
    pool->buffer = aligned_alloc(alignment, total_size);

    /* 初始化空闲链表 */
    pool->free_list = NULL;
    for (size_t i = 0; i < block_count; i++) {
        free_block_t* block = (free_block_t*)(pool->buffer + i * aligned_block_size);
        block->next = pool->free_list;
        pool->free_list = block;
    }

    return pool;
}

void* memory_pool_alloc(memory_pool_t* pool) {
    if (!pool->free_list) {
        return NULL;  /* 池已满 */
    }

    free_block_t* block = pool->free_list;
    pool->free_list = block->next;
    pool->used_count++;

    /* 清零内存 */
    memset(block, 0, pool->block_size);
    return block;
}

void memory_pool_free(memory_pool_t* pool, void* ptr) {
    if (!ptr) return;

    /* 验证指针属于此池 */
    uint8_t* byte_ptr = ptr;
    if (byte_ptr < pool->buffer ||
        byte_ptr >= pool->buffer + pool->block_size * pool->block_count) {
        assert(0 && "Pointer does not belong to this pool");
        return;
    }

    free_block_t* block = ptr;
    block->next = pool->free_list;
    pool->free_list = block;
    pool->used_count--;
}
```

### 4.3 对象池模式

```c
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
```

---

## 5. 事件系统

### 5.1 事件总线设计

```c
/* events/bus.h */
#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t event_type_t;
typedef void (*event_handler_t)(const void* event_data, void* user_data);

typedef struct event_bus event_bus_t;

event_bus_t* event_bus_create(void);
void event_bus_destroy(event_bus_t* bus);

/* 订阅事件 */
void event_bus_subscribe(event_bus_t* bus,
                         event_type_t type,
                         event_handler_t handler,
                         void* user_data);
void event_bus_unsubscribe(event_bus_t* bus,
                           event_type_t type,
                           event_handler_t handler);

/* 发布事件 */
void event_bus_publish(event_bus_t* bus,
                       event_type_t type,
                       const void* data,
                       size_t data_size);

/* 发布延迟事件（下一帧处理） */
void event_bus_publish_delayed(event_bus_t* bus,
                                event_type_t type,
                                const void* data,
                                size_t data_size,
                                float delay_seconds);

/* 处理所有延迟事件 */
void event_bus_process_delayed(event_bus_t* bus, float delta_time);

#endif
```

### 5.2 事件总线实现

```c
/* events/bus.c */
#include "bus.h"
#include "core/memory_pool.h"
#include <stdlib.h>
#include <string.h>

#define MAX_HANDLERS_PER_EVENT 32
#define MAX_DELAYED_EVENTS 256

typedef struct {
    event_handler_t handler;
    void* user_data;
} subscription_t;

typedef struct {
    event_type_t type;
    void* data;
    size_t data_size;
    float remaining_time;
} delayed_event_t;

struct event_bus {
    subscription_t* subscriptions[256];  /* 按事件类型索引 */
    uint32_t subscription_counts[256];

    delayed_event_t delayed_events[MAX_DELAYED_EVENTS];
    uint32_t delayed_count;
    memory_pool_t* event_data_pool;
};

event_bus_t* event_bus_create(void) {
    event_bus_t* bus = calloc(1, sizeof(event_bus_t));
    bus->event_data_pool = memory_pool_create(256, 1024, 64);
    return bus;
}

void event_bus_subscribe(event_bus_t* bus,
                         event_type_t type,
                         event_handler_t handler,
                         void* user_data) {
    if (bus->subscription_counts[type] >= MAX_HANDLERS_PER_EVENT) {
        return;
    }

    if (!bus->subscriptions[type]) {
        bus->subscriptions[type] = malloc(sizeof(subscription_t) *
                                          MAX_HANDLERS_PER_EVENT);
    }

    subscription_t* sub = &bus->subscriptions[type][bus->subscription_counts[type]++];
    sub->handler = handler;
    sub->user_data = user_data;
}

void event_bus_publish(event_bus_t* bus,
                       event_type_t type,
                       const void* data,
                       size_t data_size) {
    if (!bus->subscriptions[type]) return;

    /* 复制事件数据，避免生命周期问题 */
    void* event_copy = NULL;
    if (data && data_size > 0) {
        event_copy = memory_pool_alloc(bus->event_data_pool);
        if (event_copy) {
            memcpy(event_copy, data, data_size > 256 ? 256 : data_size);
        }
    }

    /* 通知所有订阅者 */
    for (uint32_t i = 0; i < bus->subscription_counts[type]; i++) {
        subscription_t* sub = &bus->subscriptions[type][i];
        sub->handler(event_copy, sub->user_data);
    }

    if (event_copy) {
        memory_pool_free(bus->event_data_pool, event_copy);
    }
}

void event_bus_process_delayed(event_bus_t* bus, float delta_time) {
    uint32_t write_index = 0;

    for (uint32_t i = 0; i < bus->delayed_count; i++) {
        delayed_event_t* evt = &bus->delayed_events[i];
        evt->remaining_time -= delta_time;

        if (evt->remaining_time <= 0) {
            /* 触发事件 */
            event_bus_publish(bus, evt->type, evt->data, evt->data_size);
            free(evt->data);
        } else {
            /* 保留未到期的事件 */
            if (write_index != i) {
                bus->delayed_events[write_index] = *evt;
            }
            write_index++;
        }
    }

    bus->delayed_count = write_index;
}
```

### 5.3 事件类型定义

```c
/* events/types.h */
#ifndef EVENT_TYPES_H
#define EVENT_TYPES_H

/* 事件类型枚举 */
enum {
    EVENT_NONE = 0,

    /* 输入事件 */
    EVENT_KEY_PRESSED,
    EVENT_KEY_RELEASED,
    EVENT_MOUSE_MOVED,
    EVENT_MOUSE_BUTTON,

    /* 游戏逻辑事件 */
    EVENT_ENTITY_SPAWNED,
    EVENT_ENTITY_DESTROYED,
    EVENT_COLLISION,
    EVENT_DAMAGE_DEALT,

    /* 系统事件 */
    EVENT_WINDOW_RESIZE,
    EVENT_RESOURCE_LOADED,
    EVENT_SCENE_CHANGE,

    EVENT_COUNT
};

/* 事件数据结构 */
typedef struct {
    int key_code;
    int modifiers;  /* Shift, Ctrl, Alt */
} key_event_t;

typedef struct {
    uint32_t entity_a;
    uint32_t entity_b;
    float contact_point[3];
    float normal[3];
    float penetration_depth;
} collision_event_t;

typedef struct {
    uint32_t entity_id;
    float damage;
    uint32_t source_entity;
    uint32_t damage_type;
} damage_event_t;

#endif
```

---

## 6. 资源管理

### 6.1 资源管理器架构

```c
/* resources/manager.h */
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t resource_handle_t;
typedef uint32_t resource_type_t;

#define INVALID_HANDLE 0

/* 资源加载器接口 */
typedef struct {
    void* (*load)(const char* path, void* user_data);
    void (*unload)(void* resource, void* user_data);
    size_t (*get_size)(const void* resource);
} resource_loader_t;

typedef struct resource_manager resource_manager_t;

resource_manager_t* resource_manager_create(size_t max_memory);
void resource_manager_destroy(resource_manager_t* manager);

/* 注册资源类型 */
void resource_manager_register_type(resource_manager_t* manager,
                                     resource_type_t type,
                                     const resource_loader_t* loader,
                                     void* user_data);

/* 加载资源 */
resource_handle_t resource_manager_load(resource_manager_t* manager,
                                        resource_type_t type,
                                        const char* path);

/* 获取资源 */
void* resource_manager_get(resource_manager_t* manager,
                           resource_handle_t handle);

/* 释放资源引用 */
void resource_manager_release(resource_manager_t* manager,
                              resource_handle_t handle);

/* 热重载 */
void resource_manager_hot_reload(resource_manager_t* manager);

#endif
```

### 6.2 资源管理器实现

```c
/* resources/manager.c */
#include "manager.h"
#include "core/hash_map.h"
#include <stdlib.h>
#include <string.h>

#define MAX_RESOURCES 4096
#define MAX_RESOURCE_TYPES 32

typedef struct {
    void* data;
    char* path;
    resource_type_t type;
    uint32_t ref_count;
    uint64_t last_modified;  /* 用于热重载 */
    size_t size;
} resource_entry_t;

struct resource_manager {
    resource_entry_t resources[MAX_RESOURCES];
    hash_map_t* path_to_handle;  /* 路径 -> 句柄映射 */

    resource_loader_t loaders[MAX_RESOURCE_TYPES];
    void* loader_user_data[MAX_RESOURCE_TYPES];

    size_t total_memory;
    size_t max_memory;
    size_t used_memory;
};

resource_manager_t* resource_manager_create(size_t max_memory) {
    resource_manager_t* manager = calloc(1, sizeof(resource_manager_t));
    manager->path_to_handle = hash_map_create(MAX_RESOURCES);
    manager->max_memory = max_memory;
    return manager;
}

resource_handle_t resource_manager_load(resource_manager_t* manager,
                                        resource_type_t type,
                                        const char* path) {
    /* 检查是否已加载 */
    resource_handle_t* existing = hash_map_get(manager->path_to_handle, path);
    if (existing) {
        manager->resources[*existing].ref_count++;
        return *existing;
    }

    /* 查找空闲槽位 */
    resource_handle_t handle = INVALID_HANDLE;
    for (uint32_t i = 1; i < MAX_RESOURCES; i++) {
        if (manager->resources[i].ref_count == 0) {
            handle = i;
            break;
        }
    }

    if (handle == INVALID_HANDLE) {
        /* 尝试卸载未使用的资源 */
        resource_manager_evict_unused(manager);
        /* 再次尝试 */
        for (uint32_t i = 1; i < MAX_RESOURCES; i++) {
            if (manager->resources[i].ref_count == 0) {
                handle = i;
                break;
            }
        }
    }

    if (handle == INVALID_HANDLE) {
        return INVALID_HANDLE;  /* 资源槽位已满 */
    }

    /* 加载资源 */
    resource_entry_t* entry = &manager->resources[handle];
    entry->data = manager->loaders[type].load(path,
        manager->loader_user_data[type]);

    if (!entry->data) {
        return INVALID_HANDLE;
    }

    entry->path = strdup(path);
    entry->type = type;
    entry->ref_count = 1;
    entry->size = manager->loaders[type].get_size(entry->data);
    entry->last_modified = file_get_modified_time(path);

    manager->used_memory += entry->size;
    hash_map_put(manager->path_to_handle, path, &handle, sizeof(handle));

    return handle;
}

void* resource_manager_get(resource_manager_t* manager,
                           resource_handle_t handle) {
    if (handle == INVALID_HANDLE || handle >= MAX_RESOURCES) {
        return NULL;
    }
    resource_entry_t* entry = &manager->resources[handle];
    if (entry->ref_count == 0) {
        return NULL;
    }
    return entry->data;
}

void resource_manager_release(resource_manager_t* manager,
                              resource_handle_t handle) {
    if (handle == INVALID_HANDLE || handle >= MAX_RESOURCES) {
        return;
    }

    resource_entry_t* entry = &manager->resources[handle];
    if (entry->ref_count > 0) {
        entry->ref_count--;

        if (entry->ref_count == 0) {
            /* 延迟卸载，保留在内存中以便复用 */
            /* 实际实现可以添加 LRU 缓存 */
        }
    }
}

void resource_manager_hot_reload(resource_manager_t* manager) {
    for (uint32_t i = 1; i < MAX_RESOURCES; i++) {
        resource_entry_t* entry = &manager->resources[i];
        if (entry->ref_count == 0) continue;

        uint64_t current_modified = file_get_modified_time(entry->path);
        if (current_modified > entry->last_modified) {
            printf("Hot reloading: %s\n", entry->path);

            /* 卸载旧资源 */
            manager->loaders[entry->type].unload(entry->data,
                manager->loader_user_data[entry->type]);

            /* 加载新资源 */
            entry->data = manager->loaders[entry->type].load(entry->path,
                manager->loader_user_data[entry->type]);
            entry->last_modified = current_modified;
        }
    }
}
```

---

## 7. 渲染抽象层

### 7.1 RHI (Rendering Hardware Interface) 设计

```c
/* renderer/rhi.h */
#ifndef RHI_H
#define RHI_H

#include <stdint.h>
#include <stdbool.h>

/* 渲染后端枚举 */
typedef enum {
    RHI_BACKEND_OPENGL,
    RHI_BACKEND_VULKAN,
    RHI_BACKEND_D3D11,
    RHI_BACKEND_D3D12,
    RHI_BACKEND_METAL
} rhi_backend_t;

/* 句柄类型 */
typedef uint32_t rhi_buffer_t;
typedef uint32_t rhi_texture_t;
typedef uint32_t rhi_shader_t;
typedef uint32_t rhi_pipeline_t;
typedef uint32_t rhi_framebuffer_t;

typedef struct rhi_context rhi_context_t;

/* 上下文创建 */
rhi_context_t* rhi_create_context(rhi_backend_t backend, void* window_handle);
void rhi_destroy_context(rhi_context_t* ctx);

/* 缓冲区 */
typedef enum {
    RHI_BUFFER_VERTEX,
    RHI_BUFFER_INDEX,
    RHI_BUFFER_UNIFORM,
    RHI_BUFFER_STORAGE
} rhi_buffer_type_t;

rhi_buffer_t rhi_create_buffer(rhi_context_t* ctx,
                                rhi_buffer_type_t type,
                                size_t size,
                                const void* data);
void rhi_destroy_buffer(rhi_context_t* ctx, rhi_buffer_t buffer);
void* rhi_map_buffer(rhi_context_t* ctx, rhi_buffer_t buffer);
void rhi_unmap_buffer(rhi_context_t* ctx, rhi_buffer_t buffer);

/* 纹理 */
typedef enum {
    RHI_TEXTURE_2D,
    RHI_TEXTURE_3D,
    RHI_TEXTURE_CUBE
} rhi_texture_type_t;

typedef enum {
    RHI_FORMAT_R8G8B8A8_UNORM,
    RHI_FORMAT_R32G32B32_FLOAT,
    RHI_FORMAT_R32G32B32A32_FLOAT,
    RHI_FORMAT_D24_UNORM_S8_UINT,
    RHI_FORMAT_BC1_UNORM  /* DXT1 */
} rhi_format_t;

rhi_texture_t rhi_create_texture(rhi_context_t* ctx,
                                  rhi_texture_type_t type,
                                  uint32_t width,
                                  uint32_t height,
                                  uint32_t depth,
                                  rhi_format_t format,
                                  uint32_t mip_levels);
void rhi_destroy_texture(rhi_context_t* ctx, rhi_texture_t texture);
void rhi_update_texture(rhi_context_t* ctx,
                        rhi_texture_t texture,
                        uint32_t mip_level,
                        const void* data,
                        size_t size);

/* 渲染状态 */
typedef enum {
    RHI_CULL_NONE,
    RHI_CULL_FRONT,
    RHI_CULL_BACK
} rhi_cull_mode_t;

typedef enum {
    RHI_COMPARE_LESS,
    RHI_COMPARE_LEQUAL,
    RHI_COMPARE_EQUAL,
    RHI_COMPARE_GREATER,
    RHI_COMPARE_ALWAYS
} rhi_compare_func_t;

typedef struct {
    rhi_cull_mode_t cull_mode;
    bool depth_test;
    rhi_compare_func_t depth_func;
    bool depth_write;
    bool blend_enable;
    uint32_t viewport_width;
    uint32_t viewport_height;
} rhi_pipeline_state_t;

/* 绘制命令 */
void rhi_begin_frame(rhi_context_t* ctx);
void rhi_end_frame(rhi_context_t* ctx);

void rhi_set_pipeline_state(rhi_context_t* ctx, const rhi_pipeline_state_t* state);
void rhi_set_vertex_buffer(rhi_context_t* ctx, rhi_buffer_t buffer, uint32_t stride);
void rhi_set_index_buffer(rhi_context_t* ctx, rhi_buffer_t buffer, rhi_format_t format);
void rhi_set_shader(rhi_context_t* ctx, rhi_shader_t shader);

void rhi_draw(rhi_context_t* ctx, uint32_t vertex_count, uint32_t first_vertex);
void rhi_draw_indexed(rhi_context_t* ctx, uint32_t index_count, uint32_t first_index);
void rhi_draw_instanced(rhi_context_t* ctx, uint32_t vertex_count,
                        uint32_t instance_count, uint32_t first_instance);

/* 统一变量 */
void rhi_set_uniform_float(rhi_context_t* ctx, const char* name, float value);
void rhi_set_uniform_vec3(rhi_context_t* ctx, const char* name, const float* value);
void rhi_set_uniform_mat4(rhi_context_t* ctx, const char* name, const float* value);
void rhi_set_uniform_buffer(rhi_context_t* ctx, uint32_t slot, rhi_buffer_t buffer);
void rhi_set_texture(rhi_context_t* ctx, uint32_t slot, rhi_texture_t texture);

#endif
```

### 7.2 渲染命令缓冲区

```c
/* renderer/command_buffer.h */
#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include "rhi.h"

typedef enum {
    CMD_SET_PIPELINE,
    CMD_SET_VERTEX_BUFFER,
    CMD_SET_INDEX_BUFFER,
    CMD_DRAW,
    CMD_DRAW_INDEXED,
    CMD_SET_VIEWPORT,
    CMD_SET_SCISSOR,
    CMD_BIND_UNIFORM,
    CMD_BIND_TEXTURE
} render_command_type_t;

/* 渲染命令 - 紧凑布局，便于缓存 */
typedef struct {
    render_command_type_t type;
    union {
        struct { uint32_t vertex_count; uint32_t first_vertex; } draw;
        struct { uint32_t index_count; uint32_t first_index; } draw_indexed;
        struct { rhi_buffer_t buffer; uint32_t stride; } vertex_buffer;
        struct { rhi_buffer_t buffer; rhi_format_t format; } index_buffer;
        struct { uint32_t slot; rhi_texture_t texture; } texture;
        rhi_pipeline_state_t pipeline;
    } data;
} render_command_t;

/* 命令缓冲区 */
typedef struct {
    render_command_t* commands;
    uint32_t capacity;
    uint32_t count;

    /* 排序键用于渲染状态排序，减少状态切换 */
    uint64_t* sort_keys;
} command_buffer_t;

command_buffer_t* command_buffer_create(uint32_t capacity);
void command_buffer_destroy(command_buffer_t* cmd_buf);
void command_buffer_clear(command_buffer_t* cmd_buf);

/* 记录命令 */
void cmd_set_pipeline(command_buffer_t* cmd_buf,
                      const rhi_pipeline_state_t* state,
                      uint64_t sort_key);
void cmd_draw(command_buffer_t* cmd_buf,
              uint32_t vertex_count,
              uint32_t first_vertex,
              uint64_t sort_key);

/* 排序和执行 */
void command_buffer_sort(command_buffer_t* cmd_buf);
void command_buffer_execute(command_buffer_t* cmd_buf, rhi_context_t* ctx);

#endif
```

### 7.3 渲染循环示例

```c
/* renderer/renderer.c */
#include "renderer/renderer.h"
#include "renderer/command_buffer.h"
#include "ecs/components.h"

void renderer_render_scene(renderer_t* renderer, world_t* world, camera_t* camera) {
    rhi_context_t* rhi = renderer->rhi;
    command_buffer_t* cmd_buf = renderer->command_buffer;

    command_buffer_clear(cmd_buf);

    /* 更新相机 uniforms */
    float view_matrix[16], proj_matrix[16];
    camera_get_view_matrix(camera, view_matrix);
    camera_get_projection_matrix(camera, proj_matrix);

    rhi_begin_frame(rhi);

    /* 收集渲染命令 */
    component_type_t query[] = { COMPONENT_TRANSFORM, COMPONENT_RENDER };
    archetype_list_t* archetypes = world_query(world, query, 2);

    for (uint32_t a = 0; a < archetypes->count; a++) {
        archetype_t* arch = archetypes->items[a];

        ARCHETYPE_FOR_EACH(arch, chunk, i) {
            transform_component_t* transforms = archetype_get_component(
                arch, chunk, i, COMPONENT_TRANSFORM);
            render_component_t* renders = archetype_get_component(
                arch, chunk, i, COMPONENT_RENDER);

            for (uint32_t j = 0; j < chunk->count; j++) {
                if (!renders[j].visible) continue;

                mesh_t* mesh = resource_manager_get(renderer->resources,
                                                     renders[j].mesh_handle);
                material_t* material = resource_manager_get(renderer->resources,
                                                             renders[j].material_handle);

                /* 计算排序键：材质 -> 网格 -> 深度 */
                uint64_t sort_key = ((uint64_t)renders[j].material_handle << 32) |
                                    ((uint64_t)renders[j].mesh_handle << 16) |
                                    (uint64_t)(transforms[j].position[2] * 1000);

                /* 记录命令 */
                cmd_set_pipeline(cmd_buf, &material->pipeline_state, sort_key);
                cmd_set_vertex_buffer(cmd_buf, mesh->vertex_buffer,
                                      sizeof(vertex_t), sort_key);
                cmd_set_index_buffer(cmd_buf, mesh->index_buffer,
                                     RHI_FORMAT_R16_UINT, sort_key);

                /* 绑定材质参数 */
                float mvp[16];
                mat4_multiply(proj_matrix, view_matrix, mvp);
                mat4_multiply(mvp, transforms[j].world_matrix, mvp);

                /* 实例数据可以批量提交 */
                cmd_draw_indexed(cmd_buf, mesh->index_count, 0, sort_key);
            }
        }
    }

    /* 排序并执行命令，最小化状态切换 */
    command_buffer_sort(cmd_buf);
    command_buffer_execute(cmd_buf, rhi);

    rhi_end_frame(rhi);
}
```

---

## 参考资料

1. **Game Engine Architecture** - Jason Gregory
2. **Data-Oriented Design** - Richard Fabian
3. **Unity ECS Documentation** - Unity Technologies
4. **Vulkan Tutorial** - Alexander Overvoorde
5. **Handmade Hero** - Casey Muratori

---

## 实践建议

1. **性能优先**: 使用性能分析工具 (Profiler) 识别瓶颈
2. **缓存友好**: 采用 SOA 布局，减少缓存未命中
3. **延迟计算**: 只在必要时进行计算，避免不必要的工作
4. **批量处理**: 合并绘制调用，减少 API 开销
5. **内存管理**: 使用内存池，避免运行时分配


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
