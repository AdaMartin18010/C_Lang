/*
 * Auto-generated from: 10_WebAssembly_C\04_WASM_Performance_Optimization.md
 * Line: 1586
 * Language: c
 * Block ID: 0efa1c7f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// custom_allocator.c
#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <stdint.h>

// 固定大小对象的内存池
typedef struct PoolChunk {
    struct PoolChunk* next;
} PoolChunk;

typedef struct {
    size_t obj_size;
    size_t chunk_size;
    PoolChunk* free_list;
    char** chunks;
    int num_chunks;
    int max_chunks;
} ObjectPool;

EMSCRIPTEN_KEEPALIVE
ObjectPool* pool_create(size_t obj_size, size_t objs_per_chunk) {
    ObjectPool* pool = (ObjectPool*)malloc(sizeof(ObjectPool));
    if (!pool) return NULL;

    pool->obj_size = obj_size < sizeof(PoolChunk) ? sizeof(PoolChunk) : obj_size;
    pool->chunk_size = objs_per_chunk;
    pool->free_list = NULL;
    pool->num_chunks = 0;
    pool->max_chunks = 64;
    pool->chunks = (char**)malloc(pool->max_chunks * sizeof(char*));

    return pool;
}

static int pool_grow(ObjectPool* pool) {
    if (pool->num_chunks >= pool->max_chunks) return -1;

    size_t alloc_size = pool->obj_size * pool->chunk_size;
    char* chunk = (char*)malloc(alloc_size);
    if (!chunk) return -1;

    // 初始化空闲链表
    for (size_t i = 0; i < pool->chunk_size; i++) {
        PoolChunk* obj = (PoolChunk*)(chunk + i * pool->obj_size);
        obj->next = pool->free_list;
        pool->free_list = obj;
    }

    pool->chunks[pool->num_chunks++] = chunk;
    return 0;
}

EMSCRIPTEN_KEEPALIVE
void* pool_alloc(ObjectPool* pool) {
    if (!pool->free_list) {
        if (pool_grow(pool) < 0) return NULL;
    }

    PoolChunk* obj = pool->free_list;
    pool->free_list = obj->next;
    return obj;
}

EMSCRIPTEN_KEEPALIVE
void pool_free(ObjectPool* pool, void* ptr) {
    PoolChunk* obj = (PoolChunk*)ptr;
    obj->next = pool->free_list;
    pool->free_list = obj;
}

EMSCRIPTEN_KEEPALIVE
void pool_destroy(ObjectPool* pool) {
    for (int i = 0; i < pool->num_chunks; i++) {
        free(pool->chunks[i]);
    }
    free(pool->chunks);
    free(pool);
}
