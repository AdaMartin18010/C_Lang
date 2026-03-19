/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\01_Creational_Patterns.md
 * Line: 408
 * Language: c
 * Block ID: 35c8d71f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    void* (*init)(void);    /* 初始化函数 */
    void (*reset)(void*);   /* 重置函数 */
} pool_ops_t;

typedef struct {
    void* objects;
    int capacity;
    int item_size;
    pool_ops_t ops;

    mtx_t lock;
    void** available;
    int available_count;
} init_pool_t;

init_pool_t* pool_create_init(int cap, int size, pool_ops_t* ops) {
    init_pool_t* pool = malloc(sizeof(init_pool_t));
    pool->capacity = cap;
    pool->item_size = size;
    pool->ops = *ops;

    pool->objects = malloc(cap * size);
    pool->available = malloc(sizeof(void*) * cap);
    pool->available_count = cap;

    /* 初始化所有对象 */
    for (int i = 0; i < cap; i++) {
        void* obj = (char*)pool->objects + i * size;
        if (ops->init) {
            ops->init(obj);
        }
        pool->available[i] = obj;
    }

    mtx_init(&pool->lock, mtx_plain);
    return pool;
}

void* pool_get(init_pool_t* pool) {
    mtx_lock(&pool->lock);

    if (pool->available_count == 0) {
        mtx_unlock(&pool->lock);
        return NULL;
    }

    void* obj = pool->available[--pool->available_count];
    mtx_unlock(&pool->lock);
    return obj;
}

void pool_put(init_pool_t* pool, void* obj) {
    if (pool->ops.reset) {
        pool->ops.reset(obj);
    }

    mtx_lock(&pool->lock);
    pool->available[pool->available_count++] = obj;
    mtx_unlock(&pool->lock);
}
