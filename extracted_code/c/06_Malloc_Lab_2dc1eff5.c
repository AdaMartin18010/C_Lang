/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\06_Malloc_Lab.md
 * Line: 469
 * Language: c
 * Block ID: 2dc1eff5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 按大小类分离的空闲链表
// 大小类: 16-31, 32-63, 64-127, ..., 4096+

#define SIZE_CLASS_COUNT 20

static char *size_class_heads[SIZE_CLASS_COUNT];

// 获取大小类索引
static int get_size_class(size_t size) {
    int index = 0;
    size = (size - 1) >> 4;  // 除以16
    while (size && index < SIZE_CLASS_COUNT - 1) {
        size >>= 1;
        index++;
    }
    return index;
}
