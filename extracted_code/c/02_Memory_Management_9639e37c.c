/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 1176
 * Language: c
 * Block ID: 9639e37c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 可扩展的自定义分配器框架
 * 支持: 堆分配、内存池、 arenas、栈分配器
 */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

// 分配器接口
typedef struct Allocator Allocator;

struct Allocator {
    void *(*alloc)(Allocator *self, size_t size);
    void *(*realloc)(Allocator *self, void *ptr, size_t old_size, size_t new_size);
    void (*free)(Allocator *self, void *ptr);
    void (*destroy)(Allocator *self);
    void *ctx;  // 分配器特定上下文
};

/*
 * 1. 系统堆分配器（默认malloc）
 */
static void *heap_alloc(Allocator *self, size_t size) {
    (void)self;
    return malloc(size);
}

static void *heap_realloc(Allocator *self, void *ptr, size_t old_size, size_t new_size) {
    (void)self; (void)old_size;
    return realloc(ptr, new_size);
}

static void heap_free(Allocator *self, void *ptr) {
    (void)self;
    free(ptr);
}

static void heap_destroy(Allocator *self) {
    (void)self;
    // 无需清理
}

static Allocator heap_allocator = {
    .alloc = heap_alloc,
    .realloc = heap_realloc,
    .free = heap_free,
    .destroy = heap_destroy,
    .ctx = NULL
};

Allocator *get_heap_allocator(void) {
    return &heap_allocator;
}

/*
 * 2. Arena分配器（线性分配，整体释放）
 */
typedef struct ArenaBlock {
    struct ArenaBlock *next;
    size_t size;
    size_t used;
    char data[];  // 柔性数组
} ArenaBlock;

typedef struct {
    ArenaBlock *blocks;
    size_t block_size;
    size_t total_allocated;
} Arena;

#define ARENA_DEFAULT_BLOCK_SIZE (64 * 1024)  // 64KB

static Arena *arena_create(size_t block_size) {
    Arena *arena = malloc(sizeof(Arena));
    if (!arena) return NULL;

    arena->blocks = NULL;
    arena->block_size = block_size > 0 ? block_size : ARENA_DEFAULT_BLOCK_SIZE;
    arena->total_allocated = 0;
    return arena;
}

static void *arena_alloc_internal(Arena *arena, size_t size, size_t align) {
    // 对齐要求
    size_t mask = align - 1;

    // 尝试从当前block分配
    ArenaBlock *block = arena->blocks;
    if (block != NULL) {
        size_t aligned_used = (block->used + mask) & ~mask;
        if (aligned_used + size <= block->size) {
            void *ptr = block->data + aligned_used;
            block->used = aligned_used + size;
            arena->total_allocated += size;
            return ptr;
        }
    }

    // 需要新block
    size_t block_size = arena->block_size;
    if (size > block_size - align) {
        block_size = size + align;  // 大块特殊处理
    }

    ArenaBlock *new_block = malloc(sizeof(ArenaBlock) + block_size);
    if (!new_block) return NULL;

    new_block->next = arena->blocks;
    new_block->size = block_size;
    new_block->used = size;
    arena->blocks = new_block;
    arena->total_allocated += size;

    return new_block->data;
}

static void *arena_alloc(Allocator *self, size_t size) {
    Arena *arena = (Arena*)self->ctx;
    return arena_alloc_internal(arena, size, 16);  // 默认16字节对齐
}

static void *arena_realloc(Allocator *self, void *ptr, size_t old_size, size_t new_size) {
    // Arena不支持真正realloc，只能分配新内存并拷贝
    void *new_ptr = arena_alloc(self, new_size);
    if (new_ptr && ptr) {
        memcpy(new_ptr, ptr, old_size < new_size ? old_size : new_size);
    }
    return new_ptr;
}

static void arena_free(Allocator *self, void *ptr) {
    (void)self; (void)ptr;
    // Arena不支持单独释放，只能整体reset
}

static void arena_destroy(Allocator *self) {
    Arena *arena = (Arena*)self->ctx;
    ArenaBlock *block = arena->blocks;
    while (block) {
        ArenaBlock *next = block->next;
        free(block);
        block = next;
    }
    free(arena);
    free(self);
}

// 重置arena（释放所有分配但不释放arena本身）
static void arena_reset(Arena *arena) {
    ArenaBlock *block = arena->blocks;
    while (block) {
        block->used = 0;
        block = block->next;
    }
    arena->total_allocated = 0;
}

Allocator *create_arena_allocator(size_t block_size) {
    Allocator *alloc = malloc(sizeof(Allocator));
    Arena *arena = arena_create(block_size);
    if (!alloc || !arena) {
        free(alloc);
        free(arena);
        return NULL;
    }

    alloc->alloc = arena_alloc;
    alloc->realloc = arena_realloc;
    alloc->free = arena_free;
    alloc->destroy = arena_destroy;
    alloc->ctx = arena;
    return alloc;
}

/*
 * 3. 栈分配器（作用域内快速分配）
 */
typedef struct {
    char *buffer;
    size_t size;
    size_t offset;
    void *saved_state;  // 用于回滚
} StackAllocator;

#define STACK_BUFFER_SIZE (16 * 1024)  // 16KB栈缓冲区

static __thread char stack_buffer[STACK_BUFFER_SIZE];
static __thread StackAllocator stack_alloc = {
    .buffer = stack_buffer,
    .size = STACK_BUFFER_SIZE,
    .offset = 0,
    .saved_state = NULL
};

void *stack_alloc_push(size_t size, size_t align) {
    size_t mask = align - 1;
    size_t aligned_offset = (stack_alloc.offset + mask) & ~mask;

    if (aligned_offset + size > stack_alloc.size) {
        return NULL;  // 栈溢出，回退到堆
    }

    void *ptr = stack_alloc.buffer + aligned_offset;
    stack_alloc.offset = aligned_offset + size;
    return ptr;
}

void stack_alloc_pop(void *marker) {
    stack_alloc.offset = (char*)marker - stack_alloc.buffer;
}

void *stack_alloc_save(void) {
    return (void*)(stack_alloc.buffer + stack_alloc.offset);
}

/*
 * 使用示例: 复合分配器
 */
void *allocator_alloc(Allocator *alloc, size_t size) {
    return alloc->alloc(alloc, size);
}

void allocator_free(Allocator *alloc, void *ptr) {
    alloc->free(alloc, ptr);
}

void *allocator_realloc(Allocator *alloc, void *ptr, size_t old_size, size_t new_size) {
    return alloc->realloc(alloc, ptr, old_size, new_size);
}

void allocator_destroy(Allocator *alloc) {
    alloc->destroy(alloc);
}
