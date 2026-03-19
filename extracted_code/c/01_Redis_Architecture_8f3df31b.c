/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\01_Redis_Architecture.md
 * Line: 368
 * Language: c
 * Block ID: 8f3df31b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* zmalloc.c - 实现细节 */

#include "zmalloc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// 已使用内存统计（线程安全版本使用原子操作）
static size_t used_memory = 0;

// OOM 处理函数
static void (*zmalloc_oom_handler)(size_t) = NULL;

// 更新内存统计
#define update_zmalloc_stat_alloc(__n) do { \
    size_t _n = (__n); \
    if (_n&(sizeof(long)-1)) _n += sizeof(long)-(_n&(sizeof(long)-1)); \
    if (zmalloc_thread_safe) { \
        atomicIncr(used_memory,__n); \
    } else { \
        used_memory += _n; \
    } \
} while(0)

// 分配内存并记录统计
void *zmalloc(size_t size) {
    void *ptr = malloc(size+PREFIX_SIZE);

    if (!ptr) {
        zmalloc_oom_handler(size);
        return NULL;
    }

    // 在内存块头部记录大小
    *((size_t*)ptr) = size;
    update_zmalloc_stat_alloc(size+PREFIX_SIZE);

    return (char*)ptr+PREFIX_SIZE;
}

// 获取分配大小
size_t zmalloc_size(void *ptr) {
    void *realptr = (char*)ptr-PREFIX_SIZE;
    size_t size = *((size_t*)realptr);
    return size;
}

// 释放内存
void zfree(void *ptr) {
    if (ptr == NULL) return;

    void *realptr = (char*)ptr-PREFIX_SIZE;
    size_t oldsize = *((size_t*)realptr);
    update_zmalloc_stat_free(oldsize+PREFIX_SIZE);
    free(realptr);
}

// 获取已使用内存
size_t zmalloc_used_memory(void) {
    size_t um;
    if (zmalloc_thread_safe) {
        atomicGet(used_memory,um);
    } else {
        um = used_memory;
    }
    return um;
}
