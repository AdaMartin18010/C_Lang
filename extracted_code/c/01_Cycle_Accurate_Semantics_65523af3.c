/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\01_Cycle_Accurate_Semantics.md
 * Line: 74
 * Language: c
 * Block ID: 65523af3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 典型缓存行大小：64字节
// 即8个double，或16个int，或64个char

// 结构体对齐到缓存行
typedef struct {
    alignas(64) int data[16];  // C11 alignas
} CacheLine;

// 或传统方式
define CACHE_LINE_SIZE 64
define CACHE_ALIGN __attribute__((aligned(CACHE_LINE_SIZE)))

struct CACHE_ALIGN PaddedStruct {
    int value;
    char padding[CACHE_LINE_SIZE - sizeof(int)];
};
