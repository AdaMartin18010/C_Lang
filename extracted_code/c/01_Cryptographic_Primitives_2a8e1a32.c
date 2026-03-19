/*
 * Auto-generated from: 04_Industrial_Scenarios\12_Blockchain_Crypto\01_Cryptographic_Primitives.md
 * Line: 29
 * Language: c
 * Block ID: 2a8e1a32
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// secure_memory.h
#ifndef SECURE_MEMORY_H
#define SECURE_MEMORY_H

#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>

// 安全分配（锁定内存，防止交换到磁盘）
static inline void* secure_alloc(size_t size) {
    void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) return NULL;

    // 锁定物理内存
    if (mlock(ptr, size) != 0) {
        munmap(ptr, size);
        return NULL;
    }
    return ptr;
}

// 安全释放（清零后释放）
static inline void secure_free(void* ptr, size_t size) {
    if (!ptr) return;

    // 显式清零（防止编译器优化）
    volatile unsigned char* p = ptr;
    while (size--) *p++ = 0;

    munlock(ptr, size);
    munmap(ptr, size);
}

// 安全内存结构
typedef struct {
    unsigned char* data;
    size_t size;
} SecureBuffer;

SecureBuffer* secure_buffer_create(size_t size) {
    SecureBuffer* buf = calloc(1, sizeof(SecureBuffer));
    buf->data = secure_alloc(size);
    if (!buf->data) {
        free(buf);
        return NULL;
    }
    buf->size = size;
    return buf;
}

void secure_buffer_destroy(SecureBuffer* buf) {
    if (!buf) return;
    secure_free(buf->data, buf->size);
    free(buf);
}

#endif
