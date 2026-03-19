/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\03_JIT_Compilation.md
 * Line: 580
 * Language: c
 * Block ID: 70b9c160
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// jit_cache.h
#ifndef JIT_CACHE_H
#define JIT_CACHE_H

#include <stdint.h>
#include "bytecode.h"

#define CACHE_SIZE 256
#define CACHE_HOT_THRESHOLD 100  // 热点代码调用阈值

// 缓存条目
typedef struct {
    uint32_t hash;          // 字节码哈希
    void *machine_code;     // 机器码地址
    size_t code_size;       // 代码大小
    uint64_t call_count;    // 调用计数
    uint64_t exec_time;     // 累计执行时间
    int is_optimized;       // 是否已优化
} CacheEntry;

// 代码缓存
typedef struct {
    CacheEntry entries[CACHE_SIZE];
    int entry_count;
    uint64_t total_compilations;
    uint64_t cache_hits;
    uint64_t cache_misses;
} JITCache;

JITCache* cache_init(void);
void cache_free(JITCache *cache);
CacheEntry* cache_lookup(JITCache *cache, Bytecode *bc);
void cache_insert(JITCache *cache, Bytecode *bc, void *code, size_t size);
int is_hot_code(CacheEntry *entry);

#endif
