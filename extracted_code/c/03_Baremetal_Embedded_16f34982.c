/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\03_Baremetal_Embedded.md
 * Line: 751
 * Language: c
 * Block ID: 16f34982
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// memory.c - 简单内存管理器

#include "hal.h"

// 内存块头部
typedef struct block_header {
    size_t size;
    int free;
    struct block_header *next;
} block_header_t;

#define BLOCK_SIZE sizeof(block_header_t)

// 堆起始和结束
extern char _heap_start[];
extern char _heap_end[];

static block_header_t *heap_start = NULL;
static int heap_initialized = 0;

// 初始化堆
void heap_init(void) {
    if (heap_initialized) return;

    heap_start = (block_header_t *)_heap_start;
    heap_start->size = _heap_end - _heap_start - BLOCK_SIZE;
    heap_start->free = 1;
    heap_start->next = NULL;

    heap_initialized = 1;
}

// 分配内存
void* malloc(size_t size) {
    if (!heap_initialized) heap_init();

    // 对齐到8字节
    size = (size + 7) & ~7;

    block_header_t *current = heap_start;
    block_header_t *prev = NULL;

    // 查找合适的块
    while (current != NULL) {
        if (current->free && current->size >= size) {
            // 分裂块（如果剩余空间足够大）
            if (current->size >= size + BLOCK_SIZE + 8) {
                block_header_t *new_block = (block_header_t *)((char *)current + BLOCK_SIZE + size);
                new_block->size = current->size - size - BLOCK_SIZE;
                new_block->free = 1;
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;
            }

            current->free = 0;
            return (char *)current + BLOCK_SIZE;
        }

        prev = current;
        current = current->next;
    }

    // 内存不足
    return NULL;
}

// 释放内存
void free(void *ptr) {
    if (!ptr) return;

    block_header_t *block = (block_header_t *)((char *)ptr - BLOCK_SIZE);
    block->free = 1;

    // 合并相邻的空闲块
    block_header_t *current = heap_start;
    while (current != NULL && current->next != NULL) {
        if (current->free && current->next->free) {
            current->size += BLOCK_SIZE + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

// 重新分配
void* realloc(void *ptr, size_t size) {
    if (!ptr) return malloc(size);
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    block_header_t *block = (block_header_t *)((char *)ptr - BLOCK_SIZE);

    // 如果当前块足够大，直接返回
    if (block->size >= size) {
        return ptr;
    }

    // 分配新块并复制数据
    void *new_ptr = malloc(size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, block->size < size ? block->size : size);
        free(ptr);
    }

    return new_ptr;
}

// 获取内存统计
void heap_stats(size_t *total, size_t *used, size_t *free_size) {
    *total = _heap_end - _heap_start;
    *used = 0;
    *free_size = 0;

    block_header_t *current = heap_start;
    while (current != NULL) {
        if (current->free) {
            *free_size += current->size;
        } else {
            *used += current->size;
        }
        current = current->next;
    }
}
