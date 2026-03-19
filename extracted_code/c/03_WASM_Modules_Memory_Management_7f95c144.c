/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 457
 * Language: c
 * Block ID: 7f95c144
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 自定义内存分配器示例
#include <stddef.h>
#include <string.h>

static char heap[1024 * 1024];  // 1MB 静态堆
static size_t heap_offset = 0;

// 简单的 bump allocator
void* bump_alloc(size_t size) {
    size = (size + 7) & ~7;  // 8 字节对齐

    if (heap_offset + size > sizeof(heap)) {
        return NULL;  // 内存不足
    }

    void* ptr = &heap[heap_offset];
    heap_offset += size;
    return ptr;
}

// 重置分配器（不释放单个块）
void bump_reset() {
    heap_offset = 0;
}

// 使用自定义分配器
EMSCRIPTEN_KEEPALIVE
void use_custom_allocator() {
    int* arr = (int*)bump_alloc(100 * sizeof(int));
    // 使用 arr...

    // 批量重置，不单独释放
    bump_reset();
}
