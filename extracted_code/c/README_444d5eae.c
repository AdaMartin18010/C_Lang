/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\README.md
 * Line: 256
 * Language: c
 * Block ID: 444d5eae
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 问题：两个独立变量在同一缓存行
typedef struct {
    int counter1;  // 被线程A频繁修改
    int counter2;  // 被线程B频繁修改
} SharedData;  // 可能在同一64字节缓存行

// 解决方案：缓存行对齐
#define CACHE_LINE_SIZE 64

typedef struct {
    alignas(CACHE_LINE_SIZE) int counter1;
    char padding[CACHE_LINE_SIZE - sizeof(int)];
} AlignedCounter;

typedef struct {
    AlignedCounter c1;
    AlignedCounter c2;
} PaddedData;  // 保证在不同缓存行
