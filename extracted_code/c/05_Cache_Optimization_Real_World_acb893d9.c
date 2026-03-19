/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\05_Cache_Optimization_Real_World.md
 * Line: 463
 * Language: c
 * Block ID: acb893d9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 解决方案：确保每个计数器在独立缓存行

#include <stdalign.h>

// 方法1: 使用对齐属性
struct alignas(64) PaddedCounter {
    int count;
    char padding[64 - sizeof(int)];  // 填充到64字节
};

struct PaddedCounter counters[4];  // 每个在独立缓存行

// 方法2: 使用编译器扩展
#ifdef __GNUC__
#define CACHE_LINE_SIZE 64
#define CACHE_ALIGNED __attribute__((aligned(CACHE_LINE_SIZE)))
#endif

struct CACHE_ALIGNED PaddedCounter2 {
    int count;
};

// 性能对比
/*
 * 未填充:  1.2 s (4线程)
 * 填充:    0.08 s (4线程)
 * 加速比:  15x
 */
