/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 213
 * Language: c
 * Block ID: 953a26fb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 堆类型枚举
 */
typedef enum {
    MAX_HEAP,   // 最大堆
    MIN_HEAP    // 最小堆
} HeapType;

/**
 * 堆结构
 */
typedef struct {
    int* data;          // 存储数组
    size_t size;        // 当前元素数量
    size_t capacity;    // 数组容量
    HeapType type;      // 堆类型
} Heap;

#define HEAP_DEFAULT_CAPACITY 16
