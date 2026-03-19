/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\README.md
 * Line: 574
 * Language: c
 * Block ID: 30601230
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// my_malloc.c - 追踪内存分配
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>

// 保存原始 malloc 的指针
static void* (*real_malloc)(size_t) = NULL;

// 首次使用时初始化
void* malloc(size_t size) {
    if (!real_malloc) {
        real_malloc = dlsym(RTLD_NEXT, "malloc");
    }

    void* ptr = real_malloc(size);
    printf("[HOOK] malloc(%zu) = %p\n", size, ptr);
    return ptr;
}
