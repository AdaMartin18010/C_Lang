/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\04_Cache_Lab.md
 * Line: 99
 * Language: c
 * Block ID: 37b24828
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// csim.c - 缓存模拟器
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// 缓存行结构
typedef struct {
    bool valid;      // 有效位
    uint64_t tag;    // 标记
    int lru_counter; // LRU计数器
} cache_line_t;

// 缓存组结构
typedef struct {
    cache_line_t *lines;  // 行数组
} cache_set_t;

// 缓存结构
typedef struct {
    cache_set_t *sets;    // 组数组
    int s, E, b;          // 参数
    int S, B;             // 计算值
    int hit_count;
    int miss_count;
    int eviction_count;
    int global_counter;   // 全局LRU计数器
} cache_t;
