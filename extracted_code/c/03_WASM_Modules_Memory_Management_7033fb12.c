/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 1546
 * Language: c
 * Block ID: 7033fb12
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// memory_limits.c
#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>

// 内存不足处理
typedef void (*OOMHandler)(void);
static OOMHandler oom_handler = NULL;

void set_oom_handler(OOMHandler handler) {
    oom_handler = handler;
}

// 安全的内存分配
EMSCRIPTEN_KEEPALIVE
void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr && size > 0) {
        if (oom_handler) {
            oom_handler();
        }
    }
    return ptr;
}

// 检查可用内存
EMSCRIPTEN_KEEPALIVE
size_t get_free_memory_estimate() {
    // 简化的可用内存估算
    size_t heap_size = (size_t)EM_ASM_INT({ return Module.HEAP8.length; });

    // 尝试分配以找到实际可用空间
    size_t test_size = heap_size / 2;
    while (test_size > 1024) {
        void* test = malloc(test_size);
        if (test) {
            free(test);
            return test_size;
        }
        test_size /= 2;
    }
    return 0;
}

// 内存压力测试
EMSCRIPTEN_KEEPALIVE
void memory_pressure_test() {
    const size_t block_size = 1024 * 1024;  // 1MB
    int allocated = 0;

    printf("Starting memory pressure test...\n");

    while (1) {
        void* ptr = malloc(block_size);
        if (!ptr) {
            printf("Failed to allocate after %d MB\n", allocated);
            break;
        }

        // 写入数据确保内存被实际分配
        memset(ptr, 0xAA, block_size);

        allocated++;
        if (allocated % 10 == 0) {
            size_t heap = get_heap_size();
            printf("Allocated: %d MB, Heap size: %zu MB\n",
                   allocated, heap / (1024 * 1024));
        }
    }
}

// 内存碎片整理建议
EMSCRIPTEN_KEEPALIVE
void analyze_memory_fragmentation() {
    // 分配许多小块然后释放一半
    const int num_blocks = 1000;
    const size_t block_size = 1024;
    void* blocks[num_blocks];

    // 分配
    for (int i = 0; i < num_blocks; i++) {
        blocks[i] = malloc(block_size);
    }

    // 释放偶数块，产生碎片
    for (int i = 0; i < num_blocks; i += 2) {
        free(blocks[i]);
        blocks[i] = NULL;
    }

    // 尝试分配大块
    size_t large_sizes[] = {
        512 * 1024,   // 512KB
        1024 * 1024,  // 1MB
        2 * 1024 * 1024,  // 2MB
    };

    for (size_t size : large_sizes) {
        void* large = malloc(size);
        if (large) {
            printf("Successfully allocated %zu KB despite fragmentation\n",
                   size / 1024);
            free(large);
        } else {
            printf("Failed to allocate %zu KB due to fragmentation\n",
                   size / 1024);
        }
    }

    // 清理
    for (int i = 0; i < num_blocks; i++) {
        if (blocks[i]) free(blocks[i]);
    }
}
