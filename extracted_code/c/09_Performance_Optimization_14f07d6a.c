/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\09_Performance_Optimization.md
 * Line: 577
 * Language: c
 * Block ID: 14f07d6a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 不好: 可能共享缓存行
struct Counter {
    atomic_int count;
};

struct Counter counters[8];  // 8个线程的计数器

// 好: 填充到缓存行大小
#define CACHE_LINE_SIZE 64

struct PaddedCounter {
    atomic_int count;
    char padding[CACHE_LINE_SIZE - sizeof(atomic_int)];
};

struct PaddedCounter counters[8];
// 确保每个计数器在不同缓存行
