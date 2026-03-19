/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 1670
 * Language: c
 * Block ID: 5dd6a6dc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：频繁单独分配
Entity* create_entity_dynamic() {
    Entity* e = malloc(sizeof(Entity));        // 分配1
    e->components = malloc(sizeof(Component) * 10); // 分配2
    for (int i = 0; i < 10; i++) {
        e->components[i].data = malloc(64);    // 分配3-12
    }
    // 内存碎片严重！
    return e;
}

// ❌ 错误：std::vector频繁重分配
std::vector<Component> components;
for (int i = 0; i < 10000; i++) {
    components.push_back(create_component());  // 可能多次重分配
}

// ✅ 正确：预分配+内存池
typedef struct {
    char* buffer;
    size_t capacity;
    size_t used;
    size_t block_size;
} MemoryPool;

void* pool_alloc(MemoryPool* pool, size_t size) {
    size = (size + 7) & ~7;  // 8字节对齐
    if (pool->used + size > pool->capacity) {
        // 分配新块
        return NULL;  // 或扩展池
    }
    void* ptr = pool->buffer + pool->used;
    pool->used += size;
    return ptr;
}

// ✅ 正确：reserve预分配
std::vector<Component> components;
components.reserve(10000);  // 预分配
for (int i = 0; i < 10000; i++) {
    components.push_back(create_component());  // 无重分配
}
