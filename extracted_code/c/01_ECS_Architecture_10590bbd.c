/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 1873
 * Language: c
 * Block ID: 10590bbd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：未释放组件数据
void destroy_entity_bad(World* world, Entity e) {
    // 只释放实体ID，忘记释放组件
    free_entity_id(world, e);
}

// ❌ 错误：循环引用
typedef struct {
    Entity parent;
    Entity* children;  // 动态分配
    int child_count;
} Hierarchy;

void destroy_with_hierarchy_bad(World* world, Entity e) {
    Hierarchy* h = get_component(world, e, COMP_HIERARCHY);
    for (int i = 0; i < h->child_count; i++) {
        destroy_with_hierarchy_bad(world, h->children[i]);  // 递归，可能栈溢出
    }
    free(h->children);  // 可能重复释放！
    destroy_entity(world, e);
}

// ✅ 正确：集中清理
void world_destroy_entity_safe(World* world, Entity e) {
    // 1. 通知所有系统（允许清理外部资源）
    event_emit(EVENT_ENTITY_DESTROYING, e);

    // 2. 移除所有组件（组件存储负责释放）
    for (int i = 0; i < MAX_COMPONENT_TYPES; i++) {
        if (world_has_component(world, e, i)) {
            world_remove_component(world, e, i);
        }
    }

    // 3. 释放实体ID
    entity_manager_recycle(world->entity_manager, e);

    // 4. 通知已销毁
    event_emit(EVENT_ENTITY_DESTROYED, e);
}

// ✅ 正确：使用自定义分配器跟踪
typedef struct {
    Allocator base;
    size_t allocated;
    size_t freed;
    void** allocations;  // 调试用
} TrackingAllocator;

void check_leaks(TrackingAllocator* alloc) {
    assert(alloc->allocated == alloc->freed);
}
