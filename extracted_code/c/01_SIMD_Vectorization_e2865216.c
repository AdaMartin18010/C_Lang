/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 1925
 * Language: c
 * Block ID: e2865216
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 数据对齐策略详解
 */

#include <stdlib.h>
#include <stdint.h>

// 方法1: C11静态对齐
#include <stdalign.h>
alignas(64) float static_buffer[1024];  // 64字节对齐

// 方法2: GCC/Clang扩展
float gcc_aligned[1024] __attribute__((aligned(64)));

// 方法3: MSVC
// __declspec(align(64)) float msvc_buffer[1024];

// 方法4: 动态对齐分配
void* aligned_malloc(size_t size, size_t alignment) {
    void *ptr;
    #if defined(_WIN32)
        ptr = _aligned_malloc(size, alignment);
    #else
        posix_memalign(&ptr, alignment, size);
    #endif
    return ptr;
}

// 检查对齐
int is_aligned(const void *ptr, size_t alignment) {
    return ((uintptr_t)ptr & (alignment - 1)) == 0;
}
