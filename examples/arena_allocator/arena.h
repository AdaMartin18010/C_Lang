/*
 * Arena Allocator - 工业级内存分配器示例
 * 头文件
 */
#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdint.h>

/* 不透明类型 */
typedef struct Arena Arena;

/* 生命周期管理 */
Arena *arena_create(size_t block_size);
void arena_destroy(Arena *a);
void arena_reset(Arena *a);

/* 分配接口（对齐到 8 字节） */
void *arena_alloc(Arena *a, size_t size);
void *arena_alloc_aligned(Arena *a, size_t size, size_t align);

/* 状态查询 */
size_t arena_total_allocated(const Arena *a);
size_t arena_block_count(const Arena *a);

/* 便捷宏 */
#define arena_new(a, T)       ((T *)arena_alloc((a), sizeof(T)))
#define arena_new_n(a, T, n)  ((T *)arena_alloc((a), sizeof(T) * (n)))

#endif /* ARENA_H */
