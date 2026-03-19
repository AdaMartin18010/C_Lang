/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 2342
 * Language: c
 * Block ID: 8a2ad235
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 演示: 内存碎片如何导致分配失败
 */
#include <stdio.h>
#include <stdlib.h>

#define NUM_BLOCKS 1000
#define BLOCK_SIZE 1024

// ❌ 问题: 交替分配释放产生碎片
void fragmentation_demo(void) {
    char *blocks[NUM_BLOCKS];

    // 分配所有块
    for (int i = 0; i < NUM_BLOCKS; i++) {
        blocks[i] = malloc(BLOCK_SIZE);
    }

    // 释放偶数索引块（产生碎片）
    for (int i = 0; i < NUM_BLOCKS; i += 2) {
        free(blocks[i]);
        blocks[i] = NULL;
    }

    // 现在有约500KB空闲内存，但都是小块
    // 尝试分配大块可能失败
    char *large = malloc(NUM_BLOCKS * BLOCK_SIZE / 2);  // 约500KB
    if (large == NULL) {
        printf("分配失败: 外部碎片\n");
        // 尽管总空闲内存足够，但没有连续大块
    }

    // 清理
    for (int i = 0; i < NUM_BLOCKS; i++) {
        free(blocks[i]);
    }
    free(large);
}

// ✅ 解决方案1: 使用内存池
// ✅ 解决方案2: 按大小分类分配
// ✅ 解决方案3: 定期内存压缩（如果可能）
// ✅ 解决方案4: 预分配大块，手动管理

typedef struct {
    char *base;
    size_t size;
    size_t offset;
} SimpleArena;

// 使用arena避免碎片
void arena_avoid_fragmentation(void) {
    // 预分配大块
    SimpleArena arena = {
        .base = malloc(1024 * 1024),  // 1MB
        .size = 1024 * 1024,
        .offset = 0
    };

    if (!arena.base) return;

    // 在这个arena中线性分配，无碎片问题
    void *p1 = arena.base + arena.offset;
    arena.offset += 256;

    void *p2 = arena.base + arena.offset;
    arena.offset += 512;

    // ... 使用 p1, p2 ...

    // 一次性释放全部
    free(arena.base);
}
