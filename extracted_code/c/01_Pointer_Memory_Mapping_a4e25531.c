/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\01_Pointer_Memory_Mapping.md
 * Line: 362
 * Language: c
 * Block ID: a4e25531
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct Arena {
    char *base;
    size_t used;
    size_t capacity;
} Arena;

void* arena_alloc(Arena *a, size_t size) {
    void *ptr = a->base + a->used;
    a->used += size;
    return ptr;
}

// 批量释放所有分配
void arena_reset(Arena *a) {
    a->used = 0;  // O(1) 释放所有内存
}
