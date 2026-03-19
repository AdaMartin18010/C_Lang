/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 531
 * Language: c
 * Block ID: 930c01c1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef ECS_FRAMEWORK_H
#define ECS_FRAMEWORK_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// 类型定义
// ============================================================================

typedef uint32_t Entity;
typedef uint32_t ComponentType;
typedef uint32_t SystemId;

#define NULL_ENTITY 0
#define MAX_COMPONENT_TYPES 64
#define MAX_SYSTEMS 128
#define CACHE_LINE_SIZE 64

// ============================================================================
// 内存分配器
// ============================================================================

typedef struct {
    void* (*alloc)(size_t size, size_t align, void* userdata);
    void  (*free)(void* ptr, void* userdata);
    void* userdata;
} Allocator;

// 默认分配器（使用标准库）
static inline void* default_alloc(size_t size, size_t align, void* userdata) {
    (void)userdata;
    #ifdef _WIN32
    return _aligned_malloc(size, align);
    #else
    void* ptr;
    posix_memalign(&ptr, align, size);
    return ptr;
    #endif
}

static inline void default_free(void* ptr, void* userdata) {
    (void)userdata;
    #ifdef _WIN32
    _aligned_free(ptr);
    #else
    free(ptr);
    #endif
}

static inline Allocator default_allocator(void) {
    return (Allocator){ default_alloc, default_free, NULL };
}

// ============================================================================
// 组件存储（SoA布局）
// ============================================================================

typedef struct {
    ComponentType type;
    size_t element_size;
    size_t capacity;
    size_t count;
    void* data;           // 组件数据数组
    Entity* entities;     // 实体ID数组（与data对应）
    uint32_t* sparse;     // 稀疏数组: entity -> dense index
    size_t sparse_capacity;
} ComponentStorage;

static inline ComponentStorage* component_storage_create(
    Allocator* alloc,
    ComponentType type,
    size_t element_size,
    size_t initial_capacity
) {
    ComponentStorage* storage = alloc->alloc(sizeof(ComponentStorage), alignof(ComponentStorage), alloc->userdata);
    memset(storage, 0, sizeof(ComponentStorage));

    storage->type = type;
    storage->element_size = element_size;
    storage->capacity = initial_capacity;
    storage->count = 0;

    size_t data_size = element_size * initial_capacity;
    storage->data = alloc->alloc(data_size, CACHE_LINE_SIZE, alloc->userdata);
    memset(storage->data, 0, data_size);

    storage->entities = alloc->alloc(sizeof(Entity) * initial_capacity, alignof(Entity), alloc->userdata);

    storage->sparse_capacity = 1024;
    storage->sparse = alloc->alloc(sizeof(uint32_t) * storage->sparse_capacity, alignof(uint32_t), alloc->userdata);
    memset(storage->sparse, 0, sizeof(uint32_t) * storage->sparse_capacity);

    return storage;
}

static inline void component_storage_destroy(ComponentStorage* storage, Allocator* alloc) {
    alloc->free(storage->data, alloc->userdata);
    alloc->free(storage->entities, alloc->userdata);
    alloc->free(storage->sparse, alloc->userdata);
    alloc->free(storage, alloc->userdata);
}

static inline void component_storage_ensure_sparse(ComponentStorage* storage, Allocator* alloc, Entity entity) {
    if (entity >= storage->sparse_capacity) {
        size_t new_capacity = storage->sparse_capacity * 2;
        while (new_capacity <= entity) new_capacity *= 2;

        uint32_t* new_sparse = alloc->alloc(sizeof(uint32_t) * new_capacity, alignof(uint32_t), alloc->userdata);
        memset(new_sparse, 0, sizeof(uint32_t) * new_capacity);
        memcpy(new_sparse, storage->sparse, sizeof(uint32_t) * storage->sparse_capacity);

        alloc->free(storage->sparse, alloc->userdata);
        storage->sparse = new_sparse;
        storage->sparse_capacity = new_capacity;
    }
}

static inline void* component_storage_add(
    ComponentStorage* storage,
    Allocator* alloc,
    Entity entity,
    const void* data
) {
    // 确保稀疏数组足够大
    component_storage_ensure_sparse(storage, alloc, entity);

    // 检查是否已存在
    uint32_t idx = storage->sparse[entity];
    if (idx != 0) {
        // 更新现有组件
        void* dest = (char*)storage->data + (idx - 1) * storage->element_size;
        memcpy(dest, data, storage->element_size);
        return dest;
    }

    // 扩容检查
    if (storage->count >= storage->capacity) {
        size_t new_capacity = storage->capacity * 2;

        void* new_data = alloc->alloc(storage->element_size * new_capacity, CACHE_LINE_SIZE, alloc->userdata);
        memcpy(new_data, storage->data, storage->element_size * storage->capacity);
        alloc->free(storage->data, alloc->userdata);
        storage->data = new_data;

        Entity* new_entities = alloc->alloc(sizeof(Entity) * new_capacity, alignof(Entity), alloc->userdata);
        memcpy(new_entities, storage->entities, sizeof(Entity) * storage->capacity);
        alloc->free(storage->entities, alloc->userdata);
        storage->entities = new_entities;

        storage->capacity = new_capacity;
    }

    // 添加新组件
    uint32_t new_idx = storage->count++;
    void* dest = (char*)storage->data + new_idx * storage->element_size;
    memcpy(dest, data, storage->element_size);
    storage->entities[new_idx] = entity;
    storage->sparse[entity] = new_idx + 1;  // +1避免0歧义

    return dest;
}

static inline void component_storage_remove(ComponentStorage* storage, Entity entity) {
    if (entity >= storage->sparse_capacity) return;

    uint32_t idx = storage->sparse[entity];
    if (idx == 0) return;
    idx--;  // 转回0-based

    // 用最后一个元素填充
    uint32_t last = storage->count - 1;
    if (idx != last) {
        // 移动数据
        memcpy(
            (char*)storage->data + idx * storage->element_size,
            (char*)storage->data + last * storage->element_size,
            storage->element_size
        );
        storage->entities[idx] = storage->entities[last];

        // 更新被移动实体的稀疏索引
        storage->sparse[storage->entities[last]] = idx + 1;
    }

    storage->count--;
    storage->sparse[entity] = 0;
}

static inline void* component_storage_get(ComponentStorage* storage, Entity entity) {
    if (entity >= storage->sparse_capacity) return NULL;

    uint32_t idx = storage->sparse[entity];
    if (idx == 0) return NULL;

    return (char*)storage->data + (idx - 1) * storage->element_size;
}

static inline bool component_storage_has(ComponentStorage* storage, Entity entity) {
    if (entity >= storage->sparse_capacity) return false;
    return storage->sparse[entity] != 0;
}

// ============================================================================
// 实体管理器
// ============================================================================

typedef struct {
    Entity next_id;
    Entity* free_list;
    size_t free_count;
    size_t free_capacity;
    Allocator* alloc;
} EntityManager;

static inline EntityManager* entity_manager_create(Allocator* alloc) {
    EntityManager* em = alloc->alloc(sizeof(EntityManager), alignof(EntityManager), alloc->userdata);
    em->next_id = 1;
    em->free_list = alloc->alloc(sizeof(Entity) * 256, alignof(Entity), alloc->userdata);
    em->free_count = 0;
    em->free_capacity = 256;
    em->alloc = alloc;
    return em;
}

static inline void entity_manager_destroy(EntityManager* em) {
    em->alloc->free(em->free_list, em->alloc->userdata);
    em->alloc->free(em, em->alloc->userdata);
}

static inline Entity entity_manager_create(EntityManager* em) {
    if (em->free_count > 0) {
        return em->free_list[--em->free_count];
    }
    return em->next_id++;
}

static inline void entity_manager_destroy_entity(EntityManager* em, Entity entity) {
    if (em->free_count >= em->free_capacity) {
        size_t new_capacity = em->free_capacity * 2;
        Entity* new_list = em->alloc->alloc(sizeof(Entity) * new_capacity, alignof(Entity), em->alloc->userdata);
        memcpy(new_list, em->free_list, sizeof(Entity) * em->free_capacity);
        em->alloc->free(em->free_list, em->alloc->userdata);
        em->free_list = new_list;
        em->free_capacity = new_capacity;
    }
    em->free_list[em->free_count++] = entity;
}

// ============================================================================
// 查询系统
// ============================================================================

typedef struct {
    ComponentType* with;
    size_t with_count;
    ComponentType* without;
    size_t without_count;
} QueryDesc;

typedef struct {
    Entity* entities;
    size_t count;
    size_t capacity;
} QueryResult;

// ============================================================================
// 事件系统
// ============================================================================

typedef enum {
    EVENT_ENTITY_CREATED,
    EVENT_ENTITY_DESTROYED,
    EVENT_COMPONENT_ADDED,
    EVENT_COMPONENT_REMOVED,
    EVENT_CUSTOM
} EventType;

typedef struct {
    EventType type;
    Entity entity;
    ComponentType component_type;
    void* data;
} Event;

typedef void (*EventHandler)(const Event* event, void* userdata);

typedef struct EventListener {
    EventHandler handler;
    void* userdata;
    struct EventListener* next;
} EventListener;

typedef struct {
    EventListener* listeners[16];  // 每种事件类型的监听器链表
    Allocator* alloc;
} EventManager;

static inline EventManager* event_manager_create(Allocator* alloc) {
    EventManager* em = alloc->alloc(sizeof(EventManager), alignof(EventManager), alloc->userdata);
    memset(em, 0, sizeof(EventManager));
    em->alloc = alloc;
    return em;
}

static inline void event_manager_subscribe(EventManager* em, EventType type, EventHandler handler, void* userdata) {
    EventListener* listener = em->alloc->alloc(sizeof(EventListener), alignof(EventListener), em->alloc->userdata);
    listener->handler = handler;
    listener->userdata = userdata;
    listener->next = em->listeners[type];
    em->listeners[type] = listener;
}

static inline void event_manager_emit(EventManager* em, const Event* event) {
    EventListener* listener = em->listeners[event->type];
    while (listener) {
        listener->handler(event, listener->userdata);
        listener = listener->next;
    }
}

// ============================================================================
// ECS世界
// ============================================================================

typedef struct {
    EntityManager* entity_manager;
    ComponentStorage* storages[MAX_COMPONENT_TYPES];
    size_t storage_count;
    EventManager* event_manager;
    Allocator* alloc;
} World;

static inline World* world_create(Allocator* alloc) {
    World* world = alloc->alloc(sizeof(World), alignof(World), alloc->userdata);
    memset(world, 0, sizeof(World));
    world->alloc = alloc;
    world->entity_manager = entity_manager_create(alloc);
    world->event_manager = event_manager_create(alloc);
    return world;
}

static inline void world_destroy(World* world) {
    for (size_t i = 0; i < MAX_COMPONENT_TYPES; i++) {
        if (world->storages[i]) {
            component_storage_destroy(world->storages[i], world->alloc);
        }
    }
    entity_manager_destroy(world->entity_manager);
    // 清理事件监听器
    for (int i = 0; i < 16; i++) {
        EventListener* listener = world->event_manager->listeners[i];
        while (listener) {
            EventListener* next = listener->next;
            world->alloc->free(listener, world->alloc->userdata);
            listener = next;
        }
    }
    world->alloc->free(world->event_manager, world->alloc->userdata);
    world->alloc->free(world, world->alloc->userdata);
}

static inline Entity world_spawn(World* world) {
    Entity e = entity_manager_create(world->entity_manager);
    Event event = { EVENT_ENTITY_CREATED, e, 0, NULL };
    event_manager_emit(world->event_manager, &event);
    return e;
}

static inline void world_destroy_entity(World* world, Entity entity) {
    // 移除所有组件
    for (size_t i = 0; i < MAX_COMPONENT_TYPES; i++) {
        if (world->storages[i] && component_storage_has(world->storages[i], entity)) {
            component_storage_remove(world->storages[i], entity);
        }
    }

    Event event = { EVENT_ENTITY_DESTROYED, entity, 0, NULL };
    event_manager_emit(world->event_manager, &event);

    entity_manager_destroy_entity(world->entity_manager, entity);
}

static inline void world_register_component(World* world, ComponentType type, size_t size) {
    assert(type < MAX_COMPONENT_TYPES);
    if (world->storages[type] == NULL) {
        world->storages[type] = component_storage_create(world->alloc, type, size, 64);
    }
}

static inline void* world_add_component(World* world, Entity entity, ComponentType type, const void* data) {
    assert(type < MAX_COMPONENT_TYPES);
    assert(world->storages[type] != NULL);

    void* result = component_storage_add(world->storages[type], world->alloc, entity, data);

    Event event = { EVENT_COMPONENT_ADDED, entity, type, result };
    event_manager_emit(world->event_manager, &event);

    return result;
}

static inline void world_remove_component(World* world, Entity entity, ComponentType type) {
    assert(type < MAX_COMPONENT_TYPES);
    if (world->storages[type]) {
        component_storage_remove(world->storages[type], entity);

        Event event = { EVENT_COMPONENT_REMOVED, entity, type, NULL };
        event_manager_emit(world->event_manager, &event);
    }
}

static inline void* world_get_component(World* world, Entity entity, ComponentType type) {
    assert(type < MAX_COMPONENT_TYPES);
    if (world->storages[type] == NULL) return NULL;
    return component_storage_get(world->storages[type], entity);
}

static inline bool world_has_component(World* world, Entity entity, ComponentType type) {
    assert(type < MAX_COMPONENT_TYPES);
    if (world->storages[type] == NULL) return false;
    return component_storage_has(world->storages[type], entity);
}

// ============================================================================
// 查询迭代器
// ============================================================================

typedef struct {
    World* world;
    ComponentType* types;
    size_t type_count;
    ComponentStorage* primary;  // 最小的存储用于迭代
    size_t current_index;
} QueryIter;

static inline QueryIter world_query(World* world, ComponentType* types, size_t count) {
    QueryIter iter = { world, types, count, NULL, 0 };

    // 找到最小的组件存储（交集的估计）
    size_t min_count = SIZE_MAX;
    for (size_t i = 0; i < count; i++) {
        ComponentStorage* storage = world->storages[types[i]];
        if (storage && storage->count < min_count) {
            min_count = storage->count;
            iter.primary = storage;
        }
    }

    return iter;
}

static inline bool query_next(QueryIter* iter, Entity* out_entity) {
    if (!iter->primary) return false;

    while (iter->current_index < iter->primary->count) {
        Entity e = iter->primary->entities[iter->current_index++];

        // 检查实体是否有所有需要的组件
        bool has_all = true;
        for (size_t i = 0; i < iter->type_count; i++) {
            if (iter->primary->type != iter->types[i]) {
                if (!world_has_component(iter->world, e, iter->types[i])) {
                    has_all = false;
                    break;
                }
            }
        }

        if (has_all) {
            *out_entity = e;
            return true;
        }
    }

    return false;
}

// ============================================================================
// 系统调度器
// ============================================================================

typedef void (*SystemFn)(World* world, float delta_time, void* userdata);

typedef struct {
    SystemId id;
    SystemFn fn;
    void* userdata;
    ComponentType* reads;
    size_t read_count;
    ComponentType* writes;
    size_t write_count;
    SystemId* depends_on;
    size_t depend_count;
    const char* name;
} SystemDesc;

typedef struct {
    SystemDesc systems[MAX_SYSTEMS];
    size_t count;
} Scheduler;

static inline Scheduler* scheduler_create(Allocator* alloc) {
    Scheduler* s = alloc->alloc(sizeof(Scheduler), alignof(Scheduler), alloc->userdata);
    s->count = 0;
    return s;
}

static inline SystemId scheduler_register(Scheduler* s, SystemDesc* desc) {
    assert(s->count < MAX_SYSTEMS);
    SystemId id = s->count;
    s->systems[s->count++] = *desc;
    s->systems[id].id = id;
    return id;
}

static inline void scheduler_run(Scheduler* s, World* world, float delta_time) {
    // 简单顺序执行（生产环境需要拓扑排序）
    for (size_t i = 0; i < s->count; i++) {
        s->systems[i].fn(world, delta_time, s->systems[i].userdata);
    }
}

#ifdef __cplusplus
}
#endif

#endif // ECS_FRAMEWORK_H
