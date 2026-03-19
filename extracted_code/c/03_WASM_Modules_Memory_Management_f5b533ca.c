/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 1474
 * Language: c
 * Block ID: f5b533ca
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// memory_growth.c
#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <stdio.h>

// 检查当前内存大小
EMSCRIPTEN_KEEPALIVE
size_t get_heap_size() {
    return (size_t)EM_ASM_INT({
        return Module.HEAP8.length;
    });
}

// 尝试分配大内存块
EMSCRIPTEN_KEEPALIVE
void test_memory_growth() {
    size_t initial_size = get_heap_size();
    printf("Initial heap size: %zu bytes (%.2f MB)\n",
           initial_size, initial_size / (1024.0 * 1024.0));

    // 尝试分配大量内存
    const int num_allocations = 100;
    const size_t alloc_size = 10 * 1024 * 1024;  // 10MB each
    void* ptrs[num_allocations];

    for (int i = 0; i < num_allocations; i++) {
        ptrs[i] = malloc(alloc_size);
        if (!ptrs[i]) {
            printf("Allocation %d failed\n", i);
            break;
        }

        size_t current_size = get_heap_size();
        if (current_size != initial_size) {
            printf("Memory grew at allocation %d: %zu MB\n",
                   i, current_size / (1024 * 1024));
            initial_size = current_size;
        }
    }

    // 清理
    for (int i = 0; i < num_allocations; i++) {
        if (ptrs[i]) free(ptrs[i]);
    }
}

// 内存增长回调
EMSCRIPTEN_KEEPALIVE
void on_memory_grow() {
    printf("Memory has grown!\n");
}
