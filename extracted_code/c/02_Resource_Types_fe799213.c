/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\02_Resource_Types.md
 * Line: 417
 * Language: c
 * Block ID: fe799213
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 对象池实现
typedef struct {
    void *objects;
    bool *available;
    size_t obj_size;
    size_t count;
} ObjectPool;

ObjectPool *pool_create(size_t obj_size, size_t count) {
    ObjectPool *pool = malloc(sizeof(ObjectPool));
    pool->obj_size = obj_size;
    pool->count = count;
    pool->objects = calloc(count, obj_size);
    pool->available = calloc(count, sizeof(bool));
    for (size_t i = 0; i < count; i++) {
        pool->available[i] = true;
    }
    return pool;
}

void *pool_acquire(ObjectPool *pool) {
    for (size_t i = 0; i < pool->count; i++) {
        if (pool->available[i]) {
            pool->available[i] = false;
            return (char *)pool->objects + i * pool->obj_size;
        }
    }
    return NULL;  // 池耗尽
}

void pool_release(ObjectPool *pool, void *obj) {
    size_t idx = ((char *)obj - (char *)pool->objects) / pool->obj_size;
    if (idx < pool->count) {
        pool->available[idx] = true;
    }
}
