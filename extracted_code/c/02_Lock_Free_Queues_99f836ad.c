/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\02_Lock_Free_Queues.md
 * Line: 61
 * Language: c
 * Block ID: 99f836ad
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// lockfree_common.h
#ifndef LOCKFREE_COMMON_H
#define LOCKFREE_COMMON_H

#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// 缓存行大小（通常为64字节）
#define CACHE_LINE_SIZE 64

// 填充到缓存行大小，避免伪共享
#define CACHE_LINE_PAD(size) \
    char _pad[CACHE_LINE_SIZE - ((size) % CACHE_LINE_SIZE)]

// 内存屏障
#define LFENCE() atomic_thread_fence(memory_order_seq_cst)
#define SFENCE() atomic_thread_fence(memory_order_seq_cst)
#define MFENCE() atomic_thread_fence(memory_order_seq_cst)

// 编译屏障
#define COMPILER_BARRIER() __asm__ volatile ("" ::: "memory")

// 性能计数
static inline uint64_t rdtsc(void) {
#if defined(__x86_64__)
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
#elif defined(__aarch64__)
    uint64_t val;
    __asm__ __volatile__ ("mrs %0, cntvct_el0" : "=r" (val));
    return val;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
#endif
}

// 计算时间差（纳秒）
static inline double ns_diff(uint64_t start, uint64_t end) {
    return (double)(end - start);
}

#endif
