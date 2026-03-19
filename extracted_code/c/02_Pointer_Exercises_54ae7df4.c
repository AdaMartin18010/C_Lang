/*
 * Auto-generated from: 12_Practice_Exercises\02_Pointer_Exercises.md
 * Line: 179
 * Language: c
 * Block ID: 54ae7df4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct MemoryPool MemoryPool;

// 创建指定块大小和数量的内存池
MemoryPool* pool_create(size_t block_size, size_t block_count);

// 从内存池分配
void* pool_alloc(MemoryPool *pool);

// 释放回内存池
void pool_free(MemoryPool *pool, void *ptr);

// 销毁内存池
void pool_destroy(MemoryPool *pool);

// 获取统计信息
size_t pool_available(MemoryPool *pool);
size_t pool_used(MemoryPool *pool);
