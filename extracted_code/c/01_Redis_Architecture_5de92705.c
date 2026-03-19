/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\01_Redis_Architecture.md
 * Line: 341
 * Language: c
 * Block ID: 5de92705
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* zmalloc.h - Redis 内存分配抽象层 */

#ifndef __ZMALLOC_H
#define __ZMALLOC_H

#include <stddef.h>

// 内存分配统计
#define PREFIX_SIZE (sizeof(size_t))

// 分配函数
void *zmalloc(size_t size);
void *zcalloc(size_t count, size_t size);
void *zrealloc(void *ptr, size_t size);
void zfree(void *ptr);
char *zstrdup(const char *s);
size_t zmalloc_used_memory(void);
void zmalloc_set_oom_handler(void (*oom_handler)(size_t));

// 内存对齐分配
void *zmalloc_no_tcache(size_t size);
void zfree_no_tcache(void *ptr);

#endif
