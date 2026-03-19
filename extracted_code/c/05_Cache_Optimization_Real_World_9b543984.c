/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\05_Cache_Optimization_Real_World.md
 * Line: 421
 * Language: c
 * Block ID: 9b543984
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 假共享示例
#include <pthread.h>

struct Counter {
    int count;
};

struct Counter counters[4];  // 4个计数器，可能在同一缓存行

void* increment(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 100000000; i++) {
        counters[id].count++;  // 修改触发缓存行失效
    }
    return NULL;
}

/* 问题分析:
 *
 * 假设 counters[0-3] 位于同一64字节缓存行:
 *
 * CPU 0 写入 counters[0]:
 *   -> 缓存行变为 Modified (M)
 *
 * CPU 1 写入 counters[1]:
 *   -> CPU 0的缓存行失效
 *   -> CPU 1获取缓存行，变为M
 *   -> CPU 0等待
 *
 * CPU 0 再次写入 counters[0]:
 *   -> CPU 1的缓存行失效
 *   -> CPU 0获取缓存行
 *   -> CPU 1等待
 *
 * 结果: 每次递增都导致缓存行在CPU间 ping-pong
 * 性能: 比单线程慢10-100倍！
 */
