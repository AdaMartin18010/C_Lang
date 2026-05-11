/*
 * Arena Allocator - 工业级内存分配器实现
 * 编译: gcc -Wall -Wextra -O2 -std=c11 -c arena.c
 */
#include "arena.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define DEFAULT_BLOCK_SIZE (64 * 1024)  /* 64KB 默认块 */
#define ALIGNMENT 8

/* 内存块头 */
typedef struct Block {
    struct Block *next;
    size_t capacity;
    size_t used;
    /* 数据紧跟在头后面: uint8_t data[]; */
} Block;

struct Arena {
    Block *head;       /* 当前活跃块 */
    Block *blocks;     /* 所有块链表（用于销毁） */
    size_t total;
    size_t block_size;
};

/* 向上对齐 */
static inline size_t align_up(size_t n, size_t align) {
    return (n + align - 1) & ~(align - 1);
}

Arena *arena_create(size_t block_size) {
    Arena *a = calloc(1, sizeof(Arena));
    if (!a) return NULL;
    a->block_size = block_size > 0 ? block_size : DEFAULT_BLOCK_SIZE;
    return a;
}

static Block *new_block(size_t size) {
    Block *b = malloc(sizeof(Block) + size);
    if (!b) return NULL;
    b->next = NULL;
    b->capacity = size;
    b->used = 0;
    return b;
}

void *arena_alloc_aligned(Arena *a, size_t size, size_t align) {
    if (!a || size == 0) return NULL;
    
    size = align_up(size, ALIGNMENT);
    align = align > ALIGNMENT ? align_up(align, ALIGNMENT) : ALIGNMENT;
    
    /* 尝试从当前块分配 */
    if (a->head) {
        size_t offset = align_up((uintptr_t)((uint8_t *)a->head + sizeof(Block) + a->head->used), align);
        offset -= (uintptr_t)((uint8_t *)a->head + sizeof(Block));
        
        if (offset + size <= a->head->capacity) {
            a->head->used = offset + size;
            a->total += size;
            return (uint8_t *)a->head + sizeof(Block) + offset;
        }
    }
    
    /* 需要新块 */
    size_t block_size = size > a->block_size ? size : a->block_size;
    Block *b = new_block(block_size);
    if (!b) return NULL;
    
    /* 加入块链表 */
    b->next = a->blocks;
    a->blocks = b;
    a->head = b;
    
    /* 对齐分配 */
    size_t offset = align_up((uintptr_t)((uint8_t *)b + sizeof(Block)), align);
    offset -= (uintptr_t)((uint8_t *)b + sizeof(Block));
    b->used = offset + size;
    a->total += size;
    
    return (uint8_t *)b + sizeof(Block) + offset;
}

void *arena_alloc(Arena *a, size_t size) {
    return arena_alloc_aligned(a, size, ALIGNMENT);
}

void arena_reset(Arena *a) {
    if (!a) return;
    for (Block *b = a->blocks; b; b = b->next) {
        b->used = 0;
    }
    a->head = a->blocks;
    a->total = 0;
}

void arena_destroy(Arena *a) {
    if (!a) return;
    Block *b = a->blocks;
    while (b) {
        Block *next = b->next;
        free(b);
        b = next;
    }
    free(a);
}

size_t arena_total_allocated(const Arena *a) {
    return a ? a->total : 0;
}

size_t arena_block_count(const Arena *a) {
    if (!a) return 0;
    size_t count = 0;
    for (Block *b = a->blocks; b; b = b->next) count++;
    return count;
}
