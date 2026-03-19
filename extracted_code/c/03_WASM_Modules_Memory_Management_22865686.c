/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 907
 * Language: c
 * Block ID: 22865686
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// allocation_patterns.c
#include <stdlib.h>
#include <string.h>
#include <emscripten/emscripten.h>

// ========== 模式1: arena/bump allocator ==========

typedef struct {
    char* buffer;
    size_t size;
    size_t offset;
} Arena;

EMSCRIPTEN_KEEPALIVE
Arena* arena_create(size_t size) {
    Arena* arena = (Arena*)malloc(sizeof(Arena));
    if (!arena) return NULL;

    arena->buffer = (char*)malloc(size);
    if (!arena->buffer) {
        free(arena);
        return NULL;
    }

    arena->size = size;
    arena->offset = 0;
    return arena;
}

EMSCRIPTEN_KEEPALIVE
void* arena_alloc(Arena* arena, size_t size) {
    size = (size + 7) & ~7;  // 8 字节对齐

    if (arena->offset + size > arena->size) {
        return NULL;  // 内存不足
    }

    void* ptr = arena->buffer + arena->offset;
    arena->offset += size;
    return ptr;
}

EMSCRIPTEN_KEEPALIVE
void arena_reset(Arena* arena) {
    arena->offset = 0;
}

EMSCRIPTEN_KEEPALIVE
void arena_destroy(Arena* arena) {
    if (arena) {
        free(arena->buffer);
        free(arena);
    }
}

// ========== 模式2: 池分配器 ==========

typedef struct PoolBlock {
    struct PoolBlock* next;
} PoolBlock;

typedef struct {
    size_t block_size;
    size_t blocks_per_chunk;
    PoolBlock* free_list;
    char** chunks;
    int num_chunks;
    int max_chunks;
} Pool;

EMSCRIPTEN_KEEPALIVE
Pool* pool_create(size_t block_size, size_t blocks_per_chunk) {
    Pool* pool = (Pool*)malloc(sizeof(Pool));
    if (!pool) return NULL;

    pool->block_size = (block_size < sizeof(PoolBlock)) ?
                       sizeof(PoolBlock) : block_size;
    pool->blocks_per_chunk = blocks_per_chunk;
    pool->free_list = NULL;
    pool->num_chunks = 0;
    pool->max_chunks = 16;
    pool->chunks = (char**)malloc(pool->max_chunks * sizeof(char*));

    return pool;
}

static int pool_add_chunk(Pool* pool) {
    if (pool->num_chunks >= pool->max_chunks) return -1;

    size_t chunk_size = pool->block_size * pool->blocks_per_chunk;
    char* chunk = (char*)malloc(chunk_size);
    if (!chunk) return -1;

    // 初始化空闲链表
    for (size_t i = 0; i < pool->blocks_per_chunk; i++) {
        PoolBlock* block = (PoolBlock*)(chunk + i * pool->block_size);
        block->next = pool->free_list;
        pool->free_list = block;
    }

    pool->chunks[pool->num_chunks++] = chunk;
    return 0;
}

EMSCRIPTEN_KEEPALIVE
void* pool_alloc(Pool* pool) {
    if (!pool->free_list) {
        if (pool_add_chunk(pool) < 0) return NULL;
    }

    PoolBlock* block = pool->free_list;
    pool->free_list = block->next;
    return block;
}

EMSCRIPTEN_KEEPALIVE
void pool_free(Pool* pool, void* ptr) {
    PoolBlock* block = (PoolBlock*)ptr;
    block->next = pool->free_list;
    pool->free_list = block;
}

EMSCRIPTEN_KEEPALIVE
void pool_destroy(Pool* pool) {
    if (pool) {
        for (int i = 0; i < pool->num_chunks; i++) {
            free(pool->chunks[i]);
        }
        free(pool->chunks);
        free(pool);
    }
}

// ========== 模式3: 栈分配器 ==========

typedef struct {
    char* buffer;
    size_t size;
    size_t offset;
    size_t* offsets;  // 保存的偏移栈
    int stack_top;
    int stack_capacity;
} StackAllocator;

EMSCRIPTEN_KEEPALIVE
StackAllocator* stack_create(size_t size) {
    StackAllocator* sa = (StackAllocator*)malloc(sizeof(StackAllocator));
    if (!sa) return NULL;

    sa->buffer = (char*)malloc(size);
    sa->size = size;
    sa->offset = 0;
    sa->offsets = (size_t*)malloc(64 * sizeof(size_t));
    sa->stack_top = 0;
    sa->stack_capacity = 64;

    return sa;
}

EMSCRIPTEN_KEEPALIVE
void* stack_alloc(StackAllocator* sa, size_t size) {
    size = (size + 7) & ~7;

    if (sa->offset + size > sa->size) return NULL;

    void* ptr = sa->buffer + sa->offset;
    sa->offset += size;
    return ptr;
}

EMSCRIPTEN_KEEPALIVE
void stack_push_frame(StackAllocator* sa) {
    if (sa->stack_top < sa->stack_capacity) {
        sa->offsets[sa->stack_top++] = sa->offset;
    }
}

EMSCRIPTEN_KEEPALIVE
void stack_pop_frame(StackAllocator* sa) {
    if (sa->stack_top > 0) {
        sa->offset = sa->offsets[--sa->stack_top];
    }
}

EMSCRIPTEN_KEEPALIVE
void stack_destroy(StackAllocator* sa) {
    if (sa) {
        free(sa->buffer);
        free(sa->offsets);
        free(sa);
    }
}
