/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 304
 * Language: c
 * Block ID: db9ac9b1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// dlmalloc 是 Emscripten 默认的内存分配器
// 提供标准的 malloc/free/realloc/calloc

#include <stdlib.h>
#include <stdio.h>
#include <emscripten/emscripten.h>

// dlmalloc 内部结构
/*
┌─────────────────────────────────────────────────────────────┐
│                    dlmalloc 内存块结构                       │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  已分配内存块                                           │  │
│  │  ┌─────────┬──────────────────────┬─────────┐        │  │
│  │  │ Header  │     User Data        │ (padding)│        │  │
│  │  │ (8字节) │     (用户请求大小)    │         │        │  │
│  │  └─────────┴──────────────────────┴─────────┘        │  │
│  │                                                      │  │
│  │  Header 包含：                                        │  │
│  │  - 块大小                                             │  │
│  │  - 前一个块大小                                       │  │
│  │  - 标志位（是否在使用中）                              │  │
│  │  - 校验和（调试模式）                                  │  │
│  │                                                      │  │
│  └───────────────────────────────────────────────────────┘  │
│                                                             │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  空闲内存块                                             │  │
│  │  ┌─────────┬──────────┬──────────┬─────────────────┐  │  │
│  │  │ Header  │ fd/bk    │  (空)    │      Footer     │  │  │
│  │  │         │ 指针     │          │                 │  │  │
│  │  └─────────┴──────────┴──────────┴─────────────────┘  │  │
│  │                                                      │  │
│  │  fd = forward pointer (下一个空闲块)                   │  │
│  │  bk = backward pointer (上一个空闲块)                  │  │
│  └───────────────────────────────────────────────────────┘  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
*/

EMSCRIPTEN_KEEPALIVE
void demonstrate_allocation() {
    printf("=== 内存分配演示 ===\n\n");

    // 基础分配
    int* p1 = (int*)malloc(sizeof(int) * 10);
    printf("分配 int[10]: %p, 大小: %zu bytes\n", (void*)p1, sizeof(int) * 10);

    // calloc - 清零分配
    int* p2 = (int*)calloc(10, sizeof(int));
    printf("calloc int[10]: %p, 初始化为 0\n", (void*)p2);

    // realloc - 重新分配
    p1 = (int*)realloc(p1, sizeof(int) * 100);
    printf("realloc 到 int[100]: %p\n", (void*)p1);

    // 分配不同大小，观察对齐
    void* sizes[] = {
        malloc(1),
        malloc(7),
        malloc(8),
        malloc(9),
        malloc(15),
        malloc(16),
        malloc(17),
        malloc(24),
        malloc(25),
        malloc(1000)
    };

    printf("\n不同大小的分配:\n");
    size_t request_sizes[] = {1, 7, 8, 9, 15, 16, 17, 24, 25, 1000};
    for (int i = 0; i < 10; i++) {
        printf("  请求 %4zu bytes → 地址 %p\n",
               request_sizes[i], sizes[i]);
        free(sizes[i]);
    }

    free(p1);
    free(p2);
}

// 内存碎片演示
EMSCRIPTEN_KEEPALIVE
void demonstrate_fragmentation() {
    printf("\n=== 内存碎片演示 ===\n\n");

    // 分配多个小块
    void* blocks[100];
    printf("分配 100 个 64-byte 块:\n");
    for (int i = 0; i < 100; i++) {
        blocks[i] = malloc(64);
    }

    // 释放偶数索引的块，产生碎片
    printf("释放偶数索引的块（产生碎片）:\n");
    for (int i = 0; i < 100; i += 2) {
        free(blocks[i]);
        blocks[i] = NULL;
    }

    // 尝试分配大块（可能失败）
    printf("尝试分配大块 (4096 bytes):\n");
    void* large = malloc(4096);
    if (large) {
        printf("  成功: %p\n", large);
        free(large);
    } else {
        printf("  失败（碎片导致）\n");
    }

    // 清理
    for (int i = 0; i < 100; i++) {
        if (blocks[i]) free(blocks[i]);
    }
}
