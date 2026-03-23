# WebAssembly 模块和内存管理指南

## 目录

- [WebAssembly 模块和内存管理指南](#webassembly-模块和内存管理指南)
  - [目录](#目录)
  - [1. WebAssembly 内存模型](#1-webassembly-内存模型)
    - [1.1 线性内存架构](#11-线性内存架构)
    - [1.2 内存页（Page）概念](#12-内存页page概念)
    - [1.3 内存布局详解](#13-内存布局详解)
    - [1.4 内存对齐](#14-内存对齐)
  - [2. C 内存分配在 WASM 中的工作方式](#2-c-内存分配在-wasm-中的工作方式)
    - [2.1 dlmalloc（Doug Lea 的内存分配器）](#21-dlmallocdoug-lea-的内存分配器)
    - [2.2 内存分配器选项](#22-内存分配器选项)
    - [2.3 内存统计和监控](#23-内存统计和监控)
  - [3. malloc/free 在 WASM 中的使用](#3-mallocfree-在-wasm-中的使用)
    - [3.1 基本使用模式](#31-基本使用模式)
    - [3.2 常见错误和陷阱](#32-常见错误和陷阱)
    - [3.3 内存分配模式](#33-内存分配模式)
  - [4. JavaScript 访问 WASM 内存](#4-javascript-访问-wasm-内存)
    - [4.1 基础内存访问](#41-基础内存访问)
    - [4.2 字符串处理](#42-字符串处理)
    - [4.3 复杂数据结构](#43-复杂数据结构)
  - [5. 内存增长和限制](#5-内存增长和限制)
    - [5.1 内存增长机制](#51-内存增长机制)
    - [5.2 内存限制和错误处理](#52-内存限制和错误处理)
  - [6. 内存泄漏检测](#6-内存泄漏检测)
    - [6.1 泄漏检测工具](#61-泄漏检测工具)
    - [6.2 浏览器内存分析](#62-浏览器内存分析)
  - [7. 最佳实践](#7-最佳实践)
    - [7.1 内存管理准则](#71-内存管理准则)
    - [7.2 性能优化清单](#72-性能优化清单)
    - [7.3 错误处理模式](#73-错误处理模式)
  - [8. 高级内存管理技术](#8-高级内存管理技术)
    - [8.1 共享内存多线程](#81-共享内存多线程)
    - [8.2 内存映射文件](#82-内存映射文件)
  - [9. 内存性能优化](#9-内存性能优化)
    - [9.1 缓存友好性](#91-缓存友好性)
    - [9.2 SIMD 内存访问](#92-simd-内存访问)
  - [10. 实战案例分析](#10-实战案例分析)
    - [10.1 游戏引擎内存管理](#101-游戏引擎内存管理)
    - [10.2 图像处理内存优化](#102-图像处理内存优化)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 1. WebAssembly 内存模型

### 1.1 线性内存架构

```
┌─────────────────────────────────────────────────────────────────────┐
│                     WebAssembly 线性内存模型                          │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  地址空间: 0 ─────────────────────────────────────────── 4GB (理论)   │
│                                                                     │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │  0x00000000 - 0x00000FFF    │  保留区域（不可访问）            │   │
│  │  (第一页，4KB)               │  用于空指针检测                  │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │  0x00001000                  │  静态数据区（只读/读写）         │   │
│  │  (.data, .bss)               │  全局变量、常量、静态变量         │   │
│  │                              │  由编译器预先分配                │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │                              │  堆区（Heap）                   │   │
│  │                              │  动态分配（malloc/free）         │   │
│  │                              │  向上增长 ←                     │   │
│  │                              │                                 │   │
│  │                              │                                 │   │
│  │                              │                                 │   │
│  │                              │  ← brk 指针（堆顶）              │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │                              │  栈区（Stack）                  │   │
│  │                              │  局部变量、函数调用              │   │
│  │                              │  向下增长 →                     │   │
│  │                              │  __stack_pointer 寄存器         │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │  INITIAL_MEMORY 边界         │  初始内存边界                    │   │
│  │  (例如: 16MB, 64MB, 256MB)   │  由 -sINITIAL_MEMORY 设置        │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │  到 MAXIMUM_MEMORY           │  可增长内存区域（可选）          │   │
│  │  (例如: 2GB)                 │  需要 -sALLOW_MEMORY_GROWTH=1    │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │  0xFFFFFFFF                  │  4GB 边界（理论限制）            │   │
│  └──────────────────────────────────────────────────────────────┘   │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 1.2 内存页（Page）概念

```c
// WebAssembly 内存以页为单位管理
// 1 页 = 64 KB = 65,536 字节

#define WASM_PAGE_SIZE 65536

// 内存配置计算
void print_memory_info() {
    // 初始内存页数
    int initial_pages = 256;  // 256 * 64KB = 16MB

    // 最大内存页数
    int max_pages = 32768;    // 32768 * 64KB = 2GB

    printf("WASM Page Size: %d bytes (64 KB)\n", WASM_PAGE_SIZE);
    printf("Initial memory: %d pages = %d MB\n", initial_pages, initial_pages * 64 / 1024);
    printf("Maximum memory: %d pages = %d MB\n", max_pages, max_pages * 64 / 1024);
}

// 编译选项对应关系
/*
-sINITIAL_MEMORY=16MB    → 256 pages
-sINITIAL_MEMORY=64MB    → 1024 pages
-sINITIAL_MEMORY=256MB   → 4096 pages
-sINITIAL_MEMORY=1GB     → 16384 pages
-sMAXIMUM_MEMORY=2GB     → 32768 pages
*/
```

### 1.3 内存布局详解

```c
// memory_layout.c
#include <stdio.h>
#include <stdlib.h>
#include <emscripten/emscripten.h>

// 全局变量 - 位于静态数据区
int global_initialized = 42;           // .data 段
int global_uninitialized;              // .bss 段
static int static_var = 100;           // .data 段
const int const_var = 200;             // .rodata 段

// 字符串常量 - 位于只读数据区
const char* hello = "Hello, WASM!";

EMSCRIPTEN_KEEPALIVE
void print_memory_layout() {
    // 局部变量 - 位于栈
    int local_var = 50;
    int local_array[100];

    // 动态分配 - 位于堆
    int* heap_var = (int*)malloc(sizeof(int));
    int* heap_array = (int*)malloc(1000 * sizeof(int));

    printf("=== 内存布局分析 ===\n\n");

    // 静态数据区
    printf("静态数据区:\n");
    printf("  全局初始化变量:     %p (global_initialized)\n", (void*)&global_initialized);
    printf("  全局未初始化变量:   %p (global_uninitialized)\n", (void*)&global_uninitialized);
    printf("  静态变量:           %p (static_var)\n", (void*)&static_var);
    printf("  常量变量:           %p (const_var)\n", (void*)&const_var);
    printf("  字符串常量:         %p (hello)\n", (void*)hello);

    // 栈区
    printf("\n栈区:\n");
    printf("  局部变量:           %p (&local_var)\n", (void*)&local_var);
    printf("  局部数组:           %p (local_array)\n", (void*)local_array);

    // 堆区
    printf("\n堆区:\n");
    printf("  堆变量:             %p (heap_var)\n", (void*)heap_var);
    printf("  堆数组:             %p (heap_array)\n", (void*)heap_array);

    // 内存区间
    printf("\n内存区间分析:\n");
    printf("  静态数据区大小估算: %lu bytes\n",
           (unsigned long)&local_var - (unsigned long)&global_initialized);
    printf("  栈当前位置:         %p\n", (void*)&local_var);
    printf("  堆当前位置:         %p\n", (void*)heap_array);

    // 清理
    free(heap_var);
    free(heap_array);
}

// 栈大小检查
EMSCRIPTEN_KEEPALIVE
void check_stack_usage() {
    char large_array[1024 * 1024];  // 1MB 栈数组
    printf("Large stack array at: %p\n", (void*)large_array);

    // 注意：大数组可能导致栈溢出
    // 默认栈大小通常为 5MB
}
```

### 1.4 内存对齐

```c
// memory_alignment.c
#include <stdint.h>
#include <stdio.h>
#include <emscripten/emscripten.h>

// 对齐要求
/*
类型        对齐要求    大小
char        1           1
short       2           2
int         4           4
long        4           4 (32位) / 8 (64位)
long long   8           8
float       4           4
double      8           8
指针        4           4 (32位 WASM)
*/

// 未对齐的结构体（有填充）
struct Misaligned {
    char a;      // 1 byte + 3 bytes padding
    int b;       // 4 bytes
    char c;      // 1 byte + 3 bytes padding
};  // 总大小: 12 bytes (不是 6!)

// 对齐的结构体（紧凑）
struct Aligned {
    int b;       // 4 bytes
    char a;      // 1 byte
    char c;      // 1 byte
    // 2 bytes padding at end
};  // 总大小: 8 bytes

// 使用 packed 属性（谨慎使用，可能影响性能）
struct Packed {
    char a;
    int b;
    char c;
} __attribute__((packed));  // 总大小: 6 bytes

EMSCRIPTEN_KEEPALIVE
void print_alignment_info() {
    printf("=== 内存对齐信息 ===\n\n");

    printf("基本类型对齐:\n");
    printf("  alignof(char):      %zu\n", alignof(char));
    printf("  alignof(short):     %zu\n", alignof(short));
    printf("  alignof(int):       %zu\n", alignof(int));
    printf("  alignof(long):      %zu\n", alignof(long));
    printf("  alignof(long long): %zu\n", alignof(long long));
    printf("  alignof(float):     %zu\n", alignof(float));
    printf("  alignof(double):    %zu\n", alignof(double));
    printf("  alignof(void*):     %zu\n", alignof(void*));

    printf("\n结构体大小:\n");
    printf("  sizeof(Misaligned): %zu\n", sizeof(struct Misaligned));
    printf("  sizeof(Aligned):    %zu\n", sizeof(struct Aligned));
    printf("  sizeof(Packed):     %zu\n", sizeof(struct Packed));

    printf("\n结构体布局 (Misaligned):\n");
    struct Misaligned m;
    printf("  &m.a: %p (offset: %zu)\n", (void*)&m.a, offsetof(struct Misaligned, a));
    printf("  &m.b: %p (offset: %zu)\n", (void*)&m.b, offsetof(struct Misaligned, b));
    printf("  &m.c: %p (offset: %zu)\n", (void*)&m.c, offsetof(struct Misaligned, c));
}

// 手动对齐分配
EMSCRIPTEN_KEEPALIVE
void* aligned_malloc(size_t size, size_t alignment) {
    // 分配额外空间用于对齐和存储原始指针
    void* raw = malloc(size + alignment + sizeof(void*));
    if (!raw) return NULL;

    // 计算对齐地址
    uintptr_t aligned = ((uintptr_t)raw + sizeof(void*) + alignment - 1)
                        & ~(alignment - 1);

    // 存储原始指针（在对齐地址前）
    ((void**)aligned)[-1] = raw;

    return (void*)aligned;
}

EMSCRIPTEN_KEEPALIVE
void aligned_free(void* ptr) {
    if (ptr) {
        // 获取原始指针并释放
        free(((void**)ptr)[-1]);
    }
}
```

---

## 2. C 内存分配在 WASM 中的工作方式

### 2.1 dlmalloc（Doug Lea 的内存分配器）

```c
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
```

### 2.2 内存分配器选项

```bash
# ========== Emscripten 内存分配器选择 ==========

# 1. dlmalloc（默认）
# 特点：通用性强，中等性能，支持多线程
emcc app.c -o app.js
# 或显式指定
emcc app.c -sMALLOC=dlmalloc -o app.js

# 2. emmalloc
# 特点：体积更小（约 1KB vs 20KB），单线程性能更好
emcc app.c -sMALLOC=emmalloc -o app.js

# 3. mimalloc (Microsoft 的分配器)
# 特点：高性能，现代分配器
emcc app.c -sMALLOC=mimalloc -o app.js

# 4. 无分配器（自己实现）
emcc app.c -sMALLOC=none -o app.js

# ========== 内存分配器对比 ==========
```

| 分配器 | 代码大小 | 性能 | 多线程 | 适用场景 |
|--------|----------|------|--------|----------|
| dlmalloc | ~20KB | ★★★☆☆ | 支持 | 通用，默认选择 |
| emmalloc | ~1KB | ★★★★☆ | 支持 | 体积敏感 |
| mimalloc | ~30KB | ★★★★★ | 支持 | 高性能需求 |
| none | 0KB | - | - | 自定义分配器 |

```c
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
```

### 2.3 内存统计和监控

```c
// memory_stats.c
#include <emscripten/emscripten.h>
#include <malloc.h>
#include <stdio.h>

// 获取内存统计信息
EMSCRIPTEN_KEEPALIVE
void print_memory_stats() {
    printf("\n=== 内存统计 ===\n\n");

    // mallinfo (传统方式，可能不完全准确)
    struct mallinfo info = mallinfo();
    printf("mallinfo 统计:\n");
    printf("  arena:     %d bytes (非 mmap 空间)\n", info.arena);
    printf("  ordblks:   %d (普通空闲块数)\n", info.ordblks);
    printf("  smblks:    %d (小块空闲块数)\n", info.smblks);
    printf("  hblks:     %d (mmap 区域数)\n", info.hblks);
    printf("  hblkhd:    %d bytes (mmap 空间)\n", info.hblkhd);
    printf("  usmblks:   %d (最大可用块)\n", info.usmblks);
    printf("  fsmblks:   %d bytes (小块总空间)\n", info.fsmblks);
    printf("  uordblks:  %d bytes (已分配)\n", info.uordblks);
    printf("  fordblks:  %d bytes (空闲)\n", info.fordblks);
    printf("  keepcost:  %d bytes (顶部可释放空间)\n", info.keepcost);

    // Emscripten 特定统计
    printf("\nEmscripten 特定统计:\n");
    printf("  HEAP8 length:  %zu bytes\n",
           (size_t)EM_ASM_INT({ return Module.HEAP8.length; }));
    printf("  HEAPU8 length: %zu bytes\n",
           (size_t)EM_ASM_INT({ return Module.HEAPU8.length; }));
}

// 内存使用追踪器
typedef struct {
    size_t total_allocated;
    size_t total_freed;
    size_t current_used;
    size_t peak_used;
    int allocation_count;
    int free_count;
} MemoryTracker;

static MemoryTracker tracker = {0};

// 包装函数
EMSCRIPTEN_KEEPALIVE
void* tracked_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr) {
        tracker.total_allocated += size;
        tracker.current_used += size;
        tracker.allocation_count++;

        if (tracker.current_used > tracker.peak_used) {
            tracker.peak_used = tracker.current_used;
        }

        // 存储大小信息（在指针前）
        size_t* size_ptr = (size_t*)ptr - 1;
        *size_ptr = size;
    }
    return ptr;
}

EMSCRIPTEN_KEEPALIVE
void tracked_free(void* ptr) {
    if (ptr) {
        size_t* size_ptr = (size_t*)ptr - 1;
        size_t size = *size_ptr;

        tracker.total_freed += size;
        tracker.current_used -= size;
        tracker.free_count++;
    }
    free(ptr);
}

EMSCRIPTEN_KEEPALIVE
void print_tracking_stats() {
    printf("\n=== 内存追踪统计 ===\n");
    printf("  总分配:     %zu bytes\n", tracker.total_allocated);
    printf("  总释放:     %zu bytes\n", tracker.total_freed);
    printf("  当前使用:   %zu bytes\n", tracker.current_used);
    printf("  峰值使用:   %zu bytes\n", tracker.peak_used);
    printf("  分配次数:   %d\n", tracker.allocation_count);
    printf("  释放次数:   %d\n", tracker.free_count);
    printf("  潜在泄漏:   %zu bytes (%d 次分配未释放)\n",
           tracker.total_allocated - tracker.total_freed,
           tracker.allocation_count - tracker.free_count);
}
```

---

## 3. malloc/free 在 WASM 中的使用

### 3.1 基本使用模式

```c
// malloc_usage.c
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <emscripten/emscripten.h>

// ========== 基础分配模式 ==========

// 1. 基本分配
EMSCRIPTEN_KEEPALIVE
void* basic_allocation(int size) {
    void* ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "malloc failed for size %d\n", size);
        return NULL;
    }
    return ptr;
}

// 2. 零初始化分配
EMSCRIPTEN_KEEPALIVE
void* zero_allocation(int count, int size) {
    void* ptr = calloc(count, size);
    if (!ptr) {
        fprintf(stderr, "calloc failed\n");
        return NULL;
    }
    // calloc 已清零，无需 memset
    return ptr;
}

// 3. 重新分配
EMSCRIPTEN_KEEPALIVE
void* resize_allocation(void* ptr, int new_size) {
    void* new_ptr = realloc(ptr, new_size);
    if (!new_ptr && new_size > 0) {
        fprintf(stderr, "realloc failed\n");
        return NULL;
    }
    return new_ptr;
}

// 4. 分配并复制字符串
EMSCRIPTEN_KEEPALIVE
char* duplicate_string(const char* source) {
    if (!source) return NULL;

    char* copy = strdup(source);
    // 或: char* copy = malloc(strlen(source) + 1);
    //     strcpy(copy, source);

    return copy;
}

// ========== 常见数据结构分配 ==========

// 动态数组
typedef struct {
    int* data;
    int size;
    int capacity;
} DynamicArray;

EMSCRIPTEN_KEEPALIVE
DynamicArray* array_create(int initial_capacity) {
    DynamicArray* arr = (DynamicArray*)malloc(sizeof(DynamicArray));
    if (!arr) return NULL;

    arr->data = (int*)malloc(initial_capacity * sizeof(int));
    if (!arr->data) {
        free(arr);
        return NULL;
    }

    arr->size = 0;
    arr->capacity = initial_capacity;
    return arr;
}

EMSCRIPTEN_KEEPALIVE
void array_destroy(DynamicArray* arr) {
    if (arr) {
        free(arr->data);
        free(arr);
    }
}

EMSCRIPTEN_KEEPALIVE
int array_push(DynamicArray* arr, int value) {
    if (arr->size >= arr->capacity) {
        // 扩容
        int new_capacity = arr->capacity * 2;
        int* new_data = (int*)realloc(arr->data, new_capacity * sizeof(int));
        if (!new_data) return -1;

        arr->data = new_data;
        arr->capacity = new_capacity;
    }

    arr->data[arr->size++] = value;
    return 0;
}

// 链表
typedef struct Node {
    int value;
    struct Node* next;
} Node;

EMSCRIPTEN_KEEPALIVE
Node* list_create(int value) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node) {
        node->value = value;
        node->next = NULL;
    }
    return node;
}

EMSCRIPTEN_KEEPALIVE
void list_append(Node** head, int value) {
    Node* new_node = list_create(value);
    if (!new_node) return;

    if (*head == NULL) {
        *head = new_node;
        return;
    }

    Node* current = *head;
    while (current->next) {
        current = current->next;
    }
    current->next = new_node;
}

EMSCRIPTEN_KEEPALIVE
void list_destroy(Node* head) {
    while (head) {
        Node* temp = head;
        head = head->next;
        free(temp);
    }
}

// 树结构
typedef struct TreeNode {
    int value;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

EMSCRIPTEN_KEEPALIVE
TreeNode* tree_create(int value) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (node) {
        node->value = value;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

EMSCRIPTEN_KEEPALIVE
void tree_destroy(TreeNode* root) {
    if (root) {
        tree_destroy(root->left);
        tree_destroy(root->right);
        free(root);
    }
}
```

### 3.2 常见错误和陷阱

```c
// malloc_pitfalls.c
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <emscripten/emscripten.h>

// ========== 常见错误示例 ==========

// 错误1: 内存泄漏
void memory_leak_example() {
    int* data = (int*)malloc(100 * sizeof(int));
    // 使用 data...
    // 忘记 free(data)!
}

// 正确做法
void correct_allocation() {
    int* data = (int*)malloc(100 * sizeof(int));
    if (data) {
        // 使用 data...
        free(data);  // 记得释放！
    }
}

// 错误2: 使用已释放的内存（Use After Free）
void use_after_free() {
    int* ptr = (int*)malloc(sizeof(int));
    *ptr = 42;
    free(ptr);
    // 错误！ptr 已经释放
    // printf("%d\n", *ptr);  // 未定义行为！
}

// 错误3: 重复释放（Double Free）
void double_free() {
    int* ptr = (int*)malloc(sizeof(int));
    free(ptr);
    // free(ptr);  // 错误！重复释放
}

// 错误4: 不匹配 new/delete 和 malloc/free
// C++ 代码中:
// int* p = new int[10];
// free(p);  // 错误！应该用 delete[]

// 错误5: 越界写入
void buffer_overflow() {
    int* arr = (int*)malloc(10 * sizeof(int));
    // arr[10] = 0;  // 错误！越界访问
    free(arr);
}

// 错误6: 未检查 malloc 返回值
void unchecked_malloc() {
    // int* ptr = malloc(1000000000000);  // 大分配可能失败
    // *ptr = 1;  // 如果 ptr 为 NULL，崩溃！
}

// 错误7: 错误的 realloc 使用
void wrong_realloc() {
    int* ptr = (int*)malloc(100);
    // 错误：如果 realloc 失败，原指针丢失
    // ptr = realloc(ptr, 200);  // 不安全！

    // 正确做法
    int* new_ptr = (int*)realloc(ptr, 200);
    if (new_ptr) {
        ptr = new_ptr;
    } else {
        // realloc 失败，ptr 仍然有效
        free(ptr);
    }
}

// 错误8: sizeof 误用
void sizeof_misuse() {
    int* arr = (int*)malloc(10);
    // 错误！应该 malloc(10 * sizeof(int))
    // memset(arr, 0, 10);  // 只清零了 10 字节，不是 10 个 int
    free(arr);
}

// ========== 安全封装 ==========

// 安全的分配宏
#define SAFE_MALLOC(type, count) \
    ((type*)safe_malloc_impl((count) * sizeof(type), __FILE__, __LINE__))

#define SAFE_FREE(ptr) \
    do { safe_free_impl(&(ptr)); } while(0)

static void* safe_malloc_impl(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    if (!ptr && size > 0) {
        fprintf(stderr, "Memory allocation failed at %s:%d\n", file, line);
    }
    return ptr;
}

static void safe_free_impl(void** ptr) {
    if (ptr && *ptr) {
        free(*ptr);
        *ptr = NULL;  // 避免悬挂指针
    }
}

// 智能指针模拟
typedef struct {
    void* ptr;
    void (*destructor)(void*);
} SmartPtr;

EMSCRIPTEN_KEEPALIVE
SmartPtr* smart_ptr_create(void* ptr, void (*destructor)(void*)) {
    SmartPtr* sp = (SmartPtr*)malloc(sizeof(SmartPtr));
    if (sp) {
        sp->ptr = ptr;
        sp->destructor = destructor;
    }
    return sp;
}

EMSCRIPTEN_KEEPALIVE
void smart_ptr_destroy(SmartPtr* sp) {
    if (sp) {
        if (sp->ptr && sp->destructor) {
            sp->destructor(sp->ptr);
        }
        free(sp);
    }
}
```

### 3.3 内存分配模式

```c
// allocation_patterns.c
#include <stdlib.h>
#include <string.h>
#include <emscripten/emscripten.h>

// ========== 模式1: arena/bump allocator ==========

typedef struct {
    char* buffer;
    size_t size;
    size_t offset;
} Arena;

EMSCRIPTEN_KEEPALIVE
Arena* arena_create(size_t size) {
    Arena* arena = (Arena*)malloc(sizeof(Arena));
    if (!arena) return NULL;

    arena->buffer = (char*)malloc(size);
    if (!arena->buffer) {
        free(arena);
        return NULL;
    }

    arena->size = size;
    arena->offset = 0;
    return arena;
}

EMSCRIPTEN_KEEPALIVE
void* arena_alloc(Arena* arena, size_t size) {
    size = (size + 7) & ~7;  // 8 字节对齐

    if (arena->offset + size > arena->size) {
        return NULL;  // 内存不足
    }

    void* ptr = arena->buffer + arena->offset;
    arena->offset += size;
    return ptr;
}

EMSCRIPTEN_KEEPALIVE
void arena_reset(Arena* arena) {
    arena->offset = 0;
}

EMSCRIPTEN_KEEPALIVE
void arena_destroy(Arena* arena) {
    if (arena) {
        free(arena->buffer);
        free(arena);
    }
}

// ========== 模式2: 池分配器 ==========

typedef struct PoolBlock {
    struct PoolBlock* next;
} PoolBlock;

typedef struct {
    size_t block_size;
    size_t blocks_per_chunk;
    PoolBlock* free_list;
    char** chunks;
    int num_chunks;
    int max_chunks;
} Pool;

EMSCRIPTEN_KEEPALIVE
Pool* pool_create(size_t block_size, size_t blocks_per_chunk) {
    Pool* pool = (Pool*)malloc(sizeof(Pool));
    if (!pool) return NULL;

    pool->block_size = (block_size < sizeof(PoolBlock)) ?
                       sizeof(PoolBlock) : block_size;
    pool->blocks_per_chunk = blocks_per_chunk;
    pool->free_list = NULL;
    pool->num_chunks = 0;
    pool->max_chunks = 16;
    pool->chunks = (char**)malloc(pool->max_chunks * sizeof(char*));

    return pool;
}

static int pool_add_chunk(Pool* pool) {
    if (pool->num_chunks >= pool->max_chunks) return -1;

    size_t chunk_size = pool->block_size * pool->blocks_per_chunk;
    char* chunk = (char*)malloc(chunk_size);
    if (!chunk) return -1;

    // 初始化空闲链表
    for (size_t i = 0; i < pool->blocks_per_chunk; i++) {
        PoolBlock* block = (PoolBlock*)(chunk + i * pool->block_size);
        block->next = pool->free_list;
        pool->free_list = block;
    }

    pool->chunks[pool->num_chunks++] = chunk;
    return 0;
}

EMSCRIPTEN_KEEPALIVE
void* pool_alloc(Pool* pool) {
    if (!pool->free_list) {
        if (pool_add_chunk(pool) < 0) return NULL;
    }

    PoolBlock* block = pool->free_list;
    pool->free_list = block->next;
    return block;
}

EMSCRIPTEN_KEEPALIVE
void pool_free(Pool* pool, void* ptr) {
    PoolBlock* block = (PoolBlock*)ptr;
    block->next = pool->free_list;
    pool->free_list = block;
}

EMSCRIPTEN_KEEPALIVE
void pool_destroy(Pool* pool) {
    if (pool) {
        for (int i = 0; i < pool->num_chunks; i++) {
            free(pool->chunks[i]);
        }
        free(pool->chunks);
        free(pool);
    }
}

// ========== 模式3: 栈分配器 ==========

typedef struct {
    char* buffer;
    size_t size;
    size_t offset;
    size_t* offsets;  // 保存的偏移栈
    int stack_top;
    int stack_capacity;
} StackAllocator;

EMSCRIPTEN_KEEPALIVE
StackAllocator* stack_create(size_t size) {
    StackAllocator* sa = (StackAllocator*)malloc(sizeof(StackAllocator));
    if (!sa) return NULL;

    sa->buffer = (char*)malloc(size);
    sa->size = size;
    sa->offset = 0;
    sa->offsets = (size_t*)malloc(64 * sizeof(size_t));
    sa->stack_top = 0;
    sa->stack_capacity = 64;

    return sa;
}

EMSCRIPTEN_KEEPALIVE
void* stack_alloc(StackAllocator* sa, size_t size) {
    size = (size + 7) & ~7;

    if (sa->offset + size > sa->size) return NULL;

    void* ptr = sa->buffer + sa->offset;
    sa->offset += size;
    return ptr;
}

EMSCRIPTEN_KEEPALIVE
void stack_push_frame(StackAllocator* sa) {
    if (sa->stack_top < sa->stack_capacity) {
        sa->offsets[sa->stack_top++] = sa->offset;
    }
}

EMSCRIPTEN_KEEPALIVE
void stack_pop_frame(StackAllocator* sa) {
    if (sa->stack_top > 0) {
        sa->offset = sa->offsets[--sa->stack_top];
    }
}

EMSCRIPTEN_KEEPALIVE
void stack_destroy(StackAllocator* sa) {
    if (sa) {
        free(sa->buffer);
        free(sa->offsets);
        free(sa);
    }
}
```

---

## 4. JavaScript 访问 WASM 内存

### 4.1 基础内存访问

```javascript
// ========== WASM 内存视图 ==========

// 假设 Module 是已加载的 WASM 模块实例

// 获取内存缓冲区
const buffer = Module.buffer;  // ArrayBuffer

// 创建不同的类型化数组视图
const heap8   = new Int8Array(buffer);
const heapU8  = new Uint8Array(buffer);
const heap16  = new Int16Array(buffer);
const heapU16 = new Uint16Array(buffer);
const heap32  = new Int32Array(buffer);
const heapU32 = new Uint32Array(buffer);
const heapF32 = new Float32Array(buffer);
const heapF64 = new Float64Array(buffer);

// 或使用 Module 内置的视图
// Module.HEAP8, Module.HEAPU8, Module.HEAP16 等

// ========== 基本读写操作 ==========

// 读取单值
function readInt32(ptr) {
    return Module.HEAP32[ptr >> 2];  // 除以 4，因为每个 int 占 4 字节
}

function readFloat32(ptr) {
    return Module.HEAPF32[ptr >> 2];
}

function readFloat64(ptr) {
    return Module.HEAPF64[ptr >> 3];  // 除以 8
}

// 写入单值
function writeInt32(ptr, value) {
    Module.HEAP32[ptr >> 2] = value;
}

function writeFloat32(ptr, value) {
    Module.HEAPF32[ptr >> 2] = value;
}

// 使用 Module 提供的辅助函数
const value = Module.getValue(ptr, 'i32');
Module.setValue(ptr, 42, 'i32');

// 支持的类型: 'i8', 'i16', 'i32', 'i64', 'float', 'double', '*'

// ========== 数组操作 ==========

// 写入数组到 WASM 内存
function writeArray(ptr, array, type = 'i32') {
    switch(type) {
        case 'i8':
            Module.HEAP8.set(array, ptr);
            break;
        case 'i32':
            Module.HEAP32.set(array, ptr >> 2);
            break;
        case 'f32':
            Module.HEAPF32.set(array, ptr >> 2);
            break;
        case 'f64':
            Module.HEAPF64.set(array, ptr >> 3);
            break;
    }
}

// 从 WASM 内存读取数组
function readArray(ptr, length, type = 'i32') {
    switch(type) {
        case 'i8':
            return Module.HEAP8.slice(ptr, ptr + length);
        case 'i32':
            return Module.HEAP32.slice(ptr >> 2, (ptr >> 2) + length);
        case 'f32':
            return Module.HEAPF32.slice(ptr >> 2, (ptr >> 2) + length);
        case 'f64':
            return Module.HEAPF64.slice(ptr >> 3, (ptr >> 3) + length);
    }
}

// 创建视图（不复制数据）
function createView(ptr, length, type = 'i32') {
    switch(type) {
        case 'i8':
            return new Int8Array(Module.buffer, ptr, length);
        case 'u8':
            return new Uint8Array(Module.buffer, ptr, length);
        case 'i32':
            return new Int32Array(Module.buffer, ptr, length * 4);
        case 'f32':
            return new Float32Array(Module.buffer, ptr, length * 4);
        case 'f64':
            return new Float64Array(Module.buffer, ptr, length * 8);
    }
}
```

### 4.2 字符串处理

```javascript
// ========== 字符串编码和解码 ==========

// 写入 UTF-8 字符串到 WASM 内存
function stringToUTF8(str, ptr, maxBytes) {
    // 使用 Module 内置函数
    return Module.stringToUTF8(str, ptr, maxBytes);
}

// 从 WASM 内存读取 UTF-8 字符串
function UTF8ToString(ptr, maxBytes) {
    return Module.UTF8ToString(ptr, maxBytes);
}

// 分配新内存并写入字符串
function allocateString(str) {
    const len = Module.lengthBytesUTF8(str) + 1;
    const ptr = Module._malloc(len);
    Module.stringToUTF8(str, ptr, len);
    return ptr;
}

// 便捷函数
class StringManager {
    constructor(module) {
        this.module = module;
        this.allocated = new Set();
    }

    allocate(str) {
        const len = this.module.lengthBytesUTF8(str) + 1;
        const ptr = this.module._malloc(len);
        this.module.stringToUTF8(str, ptr, len);
        this.allocated.add(ptr);
        return ptr;
    }

    free(ptr) {
        if (this.allocated.has(ptr)) {
            this.module._free(ptr);
            this.allocated.delete(ptr);
        }
    }

    read(ptr, maxLen = 1024) {
        return this.module.UTF8ToString(ptr, maxLen);
    }

    freeAll() {
        for (const ptr of this.allocated) {
            this.module._free(ptr);
        }
        this.allocated.clear();
    }
}

// 其他编码支持
// UTF-16
function stringToUTF16(str, ptr, maxBytes) {
    const buf = new Uint16Array(Module.buffer, ptr, maxBytes / 2);
    for (let i = 0; i < str.length; i++) {
        buf[i] = str.charCodeAt(i);
    }
    buf[str.length] = 0;
    return str.length * 2;
}

function UTF16ToString(ptr, maxChars) {
    const buf = new Uint16Array(Module.buffer, ptr, maxChars);
    let str = '';
    for (let i = 0; i < maxChars && buf[i]; i++) {
        str += String.fromCharCode(buf[i]);
    }
    return str;
}

// ASCII/Latin-1
function writeAscii(str, ptr) {
    for (let i = 0; i < str.length; i++) {
        Module.HEAPU8[ptr + i] = str.charCodeAt(i);
    }
    Module.HEAPU8[ptr + str.length] = 0;
}

function readAscii(ptr, maxLen) {
    let str = '';
    for (let i = 0; i < maxLen && Module.HEAPU8[ptr + i]; i++) {
        str += String.fromCharCode(Module.HEAPU8[ptr + i]);
    }
    return str;
}
```

### 4.3 复杂数据结构

```javascript
// ========== 结构体访问 ==========

// C 结构体定义参考:
// typedef struct {
//     int x, y;
//     float rotation;
//     char name[32];
// } Transform;

class TransformAccessor {
    constructor(module, ptr) {
        this.module = module;
        this.ptr = ptr;
        this.view = new DataView(module.buffer);
    }

    // x: int at offset 0
    get x() {
        return this.view.getInt32(this.ptr, true);
    }
    set x(value) {
        this.view.setInt32(this.ptr, value, true);
    }

    // y: int at offset 4
    get y() {
        return this.view.getInt32(this.ptr + 4, true);
    }
    set y(value) {
        this.view.setInt32(this.ptr + 4, value, true);
    }

    // rotation: float at offset 8
    get rotation() {
        return this.view.getFloat32(this.ptr + 8, true);
    }
    set rotation(value) {
        this.view.setFloat32(this.ptr + 8, value, true);
    }

    // name: char[32] at offset 12
    get name() {
        let str = '';
        for (let i = 0; i < 32; i++) {
            const char = this.module.HEAPU8[this.ptr + 12 + i];
            if (char === 0) break;
            str += String.fromCharCode(char);
        }
        return str;
    }
    set name(value) {
        for (let i = 0; i < 32 && i < value.length; i++) {
            this.module.HEAPU8[this.ptr + 12 + i] = value.charCodeAt(i);
        }
        if (value.length < 32) {
            this.module.HEAPU8[this.ptr + 12 + value.length] = 0;
        }
    }
}

// 更通用的结构体定义
class StructDef {
    constructor(fields) {
        this.fields = {};
        this.size = 0;

        for (const [name, type] of Object.entries(fields)) {
            const alignment = this.getAlignment(type);
            this.size = (this.size + alignment - 1) & ~(alignment - 1);

            this.fields[name] = {
                type,
                offset: this.size
            };

            this.size += this.getSize(type);
        }

        // 最终对齐到最大对齐要求
        const maxAlign = Math.max(...Object.values(fields).map(t => this.getAlignment(t)));
        this.size = (this.size + maxAlign - 1) & ~(maxAlign - 1);
    }

    getSize(type) {
        const sizes = {
            'i8': 1, 'u8': 1,
            'i16': 2, 'u16': 2,
            'i32': 4, 'u32': 4,
            'f32': 4,
            'f64': 8,
            'pointer': 4
        };
        return sizes[type] || 4;
    }

    getAlignment(type) {
        return this.getSize(type);
    }

    createAccessor(module, ptr) {
        return new StructAccessor(module, ptr, this);
    }
}

class StructAccessor {
    constructor(module, ptr, def) {
        this.module = module;
        this.ptr = ptr;
        this.def = def;
        this.view = new DataView(module.buffer);
    }

    get(field) {
        const info = this.def.fields[field];
        if (!info) return undefined;

        const addr = this.ptr + info.offset;
        switch(info.type) {
            case 'i8': return this.view.getInt8(addr);
            case 'u8': return this.view.getUint8(addr);
            case 'i16': return this.view.getInt16(addr, true);
            case 'u16': return this.view.getUint16(addr, true);
            case 'i32': return this.view.getInt32(addr, true);
            case 'u32': return this.view.getUint32(addr, true);
            case 'f32': return this.view.getFloat32(addr, true);
            case 'f64': return this.view.getFloat64(addr, true);
        }
    }

    set(field, value) {
        const info = this.def.fields[field];
        if (!info) return;

        const addr = this.ptr + info.offset;
        switch(info.type) {
            case 'i8': this.view.setInt8(addr, value); break;
            case 'u8': this.view.setUint8(addr, value); break;
            case 'i16': this.view.setInt16(addr, value, true); break;
            case 'u16': this.view.setUint16(addr, value, true); break;
            case 'i32': this.view.setInt32(addr, value, true); break;
            case 'u32': this.view.setUint32(addr, value, true); break;
            case 'f32': this.view.setFloat32(addr, value, true); break;
            case 'f64': this.view.setFloat64(addr, value, true); break;
        }
    }
}

// 使用示例
const TransformDef = new StructDef({
    x: 'i32',
    y: 'i32',
    rotation: 'f32',
    scale: 'f32'
});

const transform = TransformDef.createAccessor(Module, ptr);
console.log(transform.get('x'));
transform.set('rotation', 3.14159);
```

---

## 5. 内存增长和限制

### 5.1 内存增长机制

```c
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
```

```bash
# 编译选项

# 固定内存（无增长）
emcc app.c -sINITIAL_MEMORY=64MB -sALLOW_MEMORY_GROWTH=0 -o app.js

# 允许内存增长（推荐）
emcc app.c -sINITIAL_MEMORY=64MB -sMAXIMUM_MEMORY=1GB -sALLOW_MEMORY_GROWTH=1 -o app.js

# 指定增长步长
emcc app.c -sINITIAL_MEMORY=64MB -sMAXIMUM_MEMORY=1GB -sALLOW_MEMORY_GROWTH=1 -sMEMORY_GROWTH_STEP=16MB -o app.js

# 异步内存增长（非阻塞）
emcc app.c -sALLOW_MEMORY_GROWTH=1 -sABORTING_MALLOC=0 -o app.js
```

### 5.2 内存限制和错误处理

```c
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
```

---

## 6. 内存泄漏检测

### 6.1 泄漏检测工具

```c
// leak_detector.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <emscripten/emscripten.h>

#define MAX_TRACKED_ALLOCATIONS 10000

typedef struct {
    void* ptr;
    size_t size;
    const char* file;
    int line;
    int active;
} AllocationRecord;

static AllocationRecord allocations[MAX_TRACKED_ALLOCATIONS];
static int num_allocations = 0;
static int tracking_enabled = 0;

// 启用/禁用追踪
EMSCRIPTEN_KEEPALIVE
void leak_detector_enable(int enable) {
    tracking_enabled = enable;
}

// 记录分配
static void record_allocation(void* ptr, size_t size, const char* file, int line) {
    if (!tracking_enabled || !ptr) return;

    for (int i = 0; i < MAX_TRACKED_ALLOCATIONS; i++) {
        if (!allocations[i].active) {
            allocations[i].ptr = ptr;
            allocations[i].size = size;
            allocations[i].file = file;
            allocations[i].line = line;
            allocations[i].active = 1;
            num_allocations++;
            return;
        }
    }
    fprintf(stderr, "Warning: Allocation tracking table full!\n");
}

// 记录释放
static void record_free(void* ptr) {
    if (!tracking_enabled || !ptr) return;

    for (int i = 0; i < MAX_TRACKED_ALLOCATIONS; i++) {
        if (allocations[i].active && allocations[i].ptr == ptr) {
            allocations[i].active = 0;
            num_allocations--;
            return;
        }
    }
}

// 追踪版本的分配函数
#define TRACKED_MALLOC(size) tracked_malloc_impl(size, __FILE__, __LINE__)
#define TRACKED_FREE(ptr) tracked_free_impl(ptr)

void* tracked_malloc_impl(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    record_allocation(ptr, size, file, line);
    return ptr;
}

void tracked_free_impl(void* ptr) {
    record_free(ptr);
    free(ptr);
}

void* tracked_realloc_impl(void* ptr, size_t size, const char* file, int line) {
    record_free(ptr);
    void* new_ptr = realloc(ptr, size);
    record_allocation(new_ptr, size, file, line);
    return new_ptr;
}

// 报告泄漏
EMSCRIPTEN_KEEPALIVE
void leak_detector_report() {
    if (!tracking_enabled) {
        printf("Leak detection not enabled\n");
        return;
    }

    int leak_count = 0;
    size_t total_leaked = 0;

    printf("\n=== Memory Leak Report ===\n\n");

    for (int i = 0; i < MAX_TRACKED_ALLOCATIONS; i++) {
        if (allocations[i].active) {
            printf("Leak #%d:\n", ++leak_count);
            printf("  Address: %p\n", allocations[i].ptr);
            printf("  Size: %zu bytes\n", allocations[i].size);
            printf("  Location: %s:%d\n", allocations[i].file, allocations[i].line);
            printf("\n");
            total_leaked += allocations[i].size;
        }
    }

    if (leak_count == 0) {
        printf("No memory leaks detected!\n");
    } else {
        printf("Total: %d leaks, %zu bytes\n", leak_count, total_leaked);
    }
}

// 重置追踪器
EMSCRIPTEN_KEEPALIVE
void leak_detector_reset() {
    memset(allocations, 0, sizeof(allocations));
    num_allocations = 0;
}

// ========== 使用示例 ==========

void example_with_leak() {
    int* data = TRACKED_MALLOC(100 * sizeof(int));
    // 忘记 free!
}

void example_without_leak() {
    int* data = TRACKED_MALLOC(100 * sizeof(int));
    // 使用 data...
    TRACKED_FREE(data);
}

EMSCRIPTEN_KEEPALIVE
void run_leak_demo() {
    leak_detector_enable(1);
    leak_detector_reset();

    // 分配一些内存
    void* p1 = TRACKED_MALLOC(100);
    void* p2 = TRACKED_MALLOC(200);
    void* p3 = TRACKED_MALLOC(300);

    // 只释放一部分
    TRACKED_FREE(p1);
    // p2 和 p3 泄漏!

    leak_detector_report();
    leak_detector_enable(0);
}
```

### 6.2 浏览器内存分析

```javascript
// ========== JavaScript 内存泄漏检测 ==========

class WasmMemoryMonitor {
    constructor(module) {
        this.module = module;
        this.allocations = new Map();
        this.enabled = false;
    }

    enable() {
        this.enabled = true;
        // 包装 malloc/free
        const originalMalloc = this.module._malloc;
        const originalFree = this.module._free;

        this.module._malloc = (size) => {
            const ptr = originalMalloc.call(this.module, size);
            if (this.enabled && ptr) {
                this.allocations.set(ptr, {
                    size,
                    stack: new Error().stack,
                    timestamp: Date.now()
                });
            }
            return ptr;
        };

        this.module._free = (ptr) => {
            if (this.enabled) {
                this.allocations.delete(ptr);
            }
            return originalFree.call(this.module, ptr);
        };
    }

    report() {
        console.log(`=== Memory Leak Report ===`);
        console.log(`Active allocations: ${this.allocations.size}`);

        let totalBytes = 0;
        for (const [ptr, info] of this.allocations) {
            console.log(`\nAllocation at 0x${ptr.toString(16)}:`);
            console.log(`  Size: ${info.size} bytes`);
            console.log(`  Age: ${(Date.now() - info.timestamp) / 1000}s`);
            console.log(`  Stack: ${info.stack}`);
            totalBytes += info.size;
        }

        console.log(`\nTotal leaked: ${totalBytes} bytes`);

        return {
            count: this.allocations.size,
            totalBytes
        };
    }

    // 快照比较
    takeSnapshot() {
        return new Map(this.allocations);
    }

    compareSnapshots(oldSnapshot, newSnapshot) {
        const added = [];
        const removed = [];

        for (const [ptr, info] of newSnapshot) {
            if (!oldSnapshot.has(ptr)) {
                added.push({ ptr, ...info });
            }
        }

        for (const [ptr, info] of oldSnapshot) {
            if (!newSnapshot.has(ptr)) {
                removed.push({ ptr, ...info });
            }
        }

        return { added, removed };
    }
}

// Chrome DevTools 内存快照分析
// 1. 打开 DevTools > Memory
// 2. 选择 "Heap snapshot"
// 3. 拍摄快照
// 4. 执行可疑代码
// 5. 拍摄另一个快照
// 6. 比较两个快照

// 使用 Performance.memory API
function logMemoryUsage() {
    if (performance.memory) {
        console.log('Memory usage:');
        console.log('  Used JS heap:',
            (performance.memory.usedJSHeapSize / 1048576).toFixed(2), 'MB');
        console.log('  Total JS heap:',
            (performance.memory.totalJSHeapSize / 1048576).toFixed(2), 'MB');
        console.log('  JS heap limit:',
            (performance.memory.jsHeapSizeLimit / 1048576).toFixed(2), 'MB');
    }
}
```

---

## 7. 最佳实践

### 7.1 内存管理准则

```c
// ========== 最佳实践准则 ==========

/*
1. 谁分配，谁释放
   - 在模块边界处明确内存所有权
   - 文档化内存管理责任

2. 使用 RAII 模式
   - 构造函数分配资源
   - 析构函数释放资源

3. 避免裸指针
   - 使用智能指针（在 C++ 中）
   - 或实现引用计数

4. 验证分配结果
   - 总是检查 malloc 返回值
   - 有优雅的错误处理

5. 限制分配频率
   - 批量分配而非多次小分配
   - 使用内存池重用内存

6. 避免内存碎片
   - 分配相似大小的块
   - 按相反顺序释放

7. 监控内存使用
   - 实现内存统计
   - 定期检查泄漏
*/

// 良好的内存管理示例

typedef struct {
    float* data;
    int size;
    int capacity;
    int ref_count;
} Buffer;

Buffer* buffer_create(int capacity) {
    Buffer* buf = (Buffer*)malloc(sizeof(Buffer));
    if (!buf) return NULL;

    buf->data = (float*)malloc(capacity * sizeof(float));
    if (!buf->data) {
        free(buf);
        return NULL;
    }

    buf->size = 0;
    buf->capacity = capacity;
    buf->ref_count = 1;
    return buf;
}

void buffer_retain(Buffer* buf) {
    if (buf) buf->ref_count++;
}

void buffer_release(Buffer* buf) {
    if (buf && --buf->ref_count == 0) {
        free(buf->data);
        free(buf);
    }
}
```

### 7.2 性能优化清单

| 优化项 | 实施方法 | 预期收益 |
|--------|----------|----------|
| 预分配内存 | 一次性大分配 | 减少 50%+ 分配开销 |
| 对象池 | 复用内存块 | 减少 80%+ 分配次数 |
| 批量处理 | 一次处理多个元素 | 减少 JS/WASM 边界穿越 |
| 直接内存访问 | 使用 TypedArray | 零拷贝数据传输 |
| 内存对齐 | 8 字节对齐结构体 | 提高 SIMD 性能 |
| 固定内存 | -sALLOW_MEMORY_GROWTH=0 | 减少边界检查 |

### 7.3 错误处理模式

```c
// 错误处理最佳实践

typedef enum {
    ERR_OK = 0,
    ERR_OUT_OF_MEMORY = -1,
    ERR_INVALID_ARGUMENT = -2,
    ERR_BUFFER_TOO_SMALL = -3,
    // ...
} ErrorCode;

typedef struct {
    ErrorCode code;
    const char* message;
} Result;

// 返回结果的分配函数
Result allocate_buffer(Buffer** out_buf, int size) {
    if (!out_buf) {
        return (Result){ ERR_INVALID_ARGUMENT, "Output pointer is NULL" };
    }

    if (size <= 0) {
        return (Result){ ERR_INVALID_ARGUMENT, "Invalid size" };
    }

    Buffer* buf = buffer_create(size);
    if (!buf) {
        return (Result){ ERR_OUT_OF_MEMORY, "Failed to allocate buffer" };
    }

    *out_buf = buf;
    return (Result){ ERR_OK, "Success" };
}

// 使用示例
EMSCRIPTEN_KEEPALIVE
int safe_process_data(int size) {
    Buffer* buf = NULL;
    Result result = allocate_buffer(&buf, size);

    if (result.code != ERR_OK) {
        fprintf(stderr, "Error: %s\n", result.message);
        return result.code;
    }

    // 使用 buf...

    buffer_release(buf);
    return ERR_OK;
}
```

---

## 8. 高级内存管理技术

### 8.1 共享内存多线程

```c
// shared_memory.c
#include <emscripten/emscripten.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

// 共享内存中的数据结构
#define QUEUE_SIZE 1024

typedef struct {
    volatile int head;
    volatile int tail;
    volatile int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
    float data[QUEUE_SIZE];
} SharedQueue;

// 在共享内存中分配
SharedQueue* queue;

void init_shared_queue() {
    queue = (SharedQueue*)malloc(sizeof(SharedQueue));
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->not_full, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
}

void enqueue(float value) {
    pthread_mutex_lock(&queue->mutex);

    while (queue->count >= QUEUE_SIZE) {
        pthread_cond_wait(&queue->not_full, &queue->mutex);
    }

    queue->data[queue->tail] = value;
    queue->tail = (queue->tail + 1) % QUEUE_SIZE;
    queue->count++;

    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
}

float dequeue() {
    pthread_mutex_lock(&queue->mutex);

    while (queue->count == 0) {
        pthread_cond_wait(&queue->not_empty, &queue->mutex);
    }

    float value = queue->data[queue->head];
    queue->head = (queue->head + 1) % QUEUE_SIZE;
    queue->count--;

    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex);

    return value;
}
```

### 8.2 内存映射文件

```c
// memory_mapped_io.c
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <stdlib.h>
#include <string.h>

// 使用 Emscripten 的文件系统 API 实现内存映射

typedef struct {
    void* data;
    size_t size;
    int fd;
    int flags;
} MemoryMap;

// 模拟内存映射
EMSCRIPTEN_KEEPALIVE
MemoryMap* mmap_file(const char* path, size_t size, int writable) {
    MemoryMap* map = (MemoryMap*)malloc(sizeof(MemoryMap));
    if (!map) return NULL;

    // 分配内存
    map->data = malloc(size);
    if (!map->data) {
        free(map);
        return NULL;
    }

    // 如果是已存在文件，读取内容
    if (EM_ASM_INT({
        try {
            var stat = FS.stat(UTF8ToString($0));
            return stat.size;
        } catch(e) {
            return -1;
        }
    }, path) >= 0) {
        // 读取文件到内存
        EM_ASM({
            var path = UTF8ToString($0);
            var data = FS.readFile(path);
            HEAPU8.set(data, $1);
        }, path, map->data);
    } else {
        // 新文件，清零
        memset(map->data, 0, size);
    }

    map->size = size;
    map->flags = writable ? 1 : 0;

    return map;
}

EMSCRIPTEN_KEEPALIVE
int munmap_file(MemoryMap* map) {
    if (!map) return -1;

    // 如果可写，同步到文件
    if (map->flags & 1) {
        // 写回文件系统
        // 实际实现...
    }

    free(map->data);
    free(map);
    return 0;
}

// 强制同步
EMSCRIPTEN_KEEPALIVE
int msync_map(MemoryMap* map) {
    if (!map || !(map->flags & 1)) return -1;

    // 同步到持久存储
    EM_ASM({
        FS.syncfs(function(err) {
            if (err) console.error('Sync error:', err);
        });
    });

    return 0;
}
```

---

## 9. 内存性能优化

### 9.1 缓存友好性

```c
// cache_optimization.c
#include <emscripten/emscripten.h>
#include <stdlib.h>

// 缓存行大小通常为 64 字节
#define CACHE_LINE_SIZE 64

// 结构体布局优化（SoA vs AoS）

// AoS (Array of Structs) - 缓存不友好，用于随机访问
typedef struct {
    float x, y, z;
    float vx, vy, vz;
    int id;
} Particle_AoS;

// SoA (Struct of Arrays) - 缓存友好，用于批量处理
typedef struct {
    float* x, * y, * z;
    float* vx, * vy, * vz;
    int* id;
    int count;
} Particle_SoA;

// SoA 操作 - 更好的缓存局部性
EMSCRIPTEN_KEEPALIVE
void update_particles_soa(Particle_SoA* particles, int count, float dt) {
    // 顺序访问，缓存友好
    for (int i = 0; i < count; i++) {
        particles->x[i] += particles->vx[i] * dt;
        particles->y[i] += particles->vy[i] * dt;
        particles->z[i] += particles->vz[i] * dt;
    }
}

// AoS 操作 - 缓存不友好（访问不连续）
EMSCRIPTEN_KEEPALIVE
void update_particles_aos(Particle_AoS* particles, int count, float dt) {
    for (int i = 0; i < count; i++) {
        particles[i].x += particles[i].vx * dt;
        particles[i].y += particles[i].vy * dt;
        particles[i].z += particles[i].vz * dt;
    }
}

// 预取提示（如果编译器支持）
#ifdef __EMSCRIPTEN__
#define PREFETCH(addr) __builtin_prefetch(addr, 1, 3)
#else
#define PREFETCH(addr)
#endif

EMSCRIPTEN_KEEPALIVE
void prefetch_example(float* data, int n) {
    const int PREFETCH_DISTANCE = 16;

    for (int i = 0; i < n; i++) {
        // 预取未来的数据
        if (i + PREFETCH_DISTANCE < n) {
            PREFETCH(&data[i + PREFETCH_DISTANCE]);
        }

        // 处理当前数据
        data[i] *= 2.0f;
    }
}
```

### 9.2 SIMD 内存访问

```c
// simd_memory.c
#include <emscripten/emscripten.h>
#include <wasm_simd128.h>

// SIMD 友好的内存布局
EMSCRIPTEN_KEEPALIVE
void simd_add_arrays(float* result, const float* a, const float* b, int n) {
    int i = 0;

    // 每次处理 4 个 float（128 位）
    for (; i + 4 <= n; i += 4) {
        v128_t va = wasm_v128_load(&a[i]);
        v128_t vb = wasm_v128_load(&b[i]);
        v128_t vr = wasm_f32x4_add(va, vb);
        wasm_v128_store(&result[i], vr);
    }

    // 处理剩余元素
    for (; i < n; i++) {
        result[i] = a[i] + b[i];
    }
}

// 对齐的 SIMD 加载
EMSCRIPTEN_KEEPALIVE
void simd_aligned_add(float* result, const float* a, const float* b, int n) {
    // 确保 16 字节对齐
    // 编译时添加 -msimd128

    int i = 0;

    // 处理未对齐的开头
    while (i < n && ((uintptr_t)&a[i] & 15)) {
        result[i] = a[i] + b[i];
        i++;
    }

    // SIMD 处理对齐部分
    int simd_end = n - (n - i) % 4;
    for (; i < simd_end; i += 4) {
        v128_t va = wasm_v128_load(&a[i]);
        v128_t vb = wasm_v128_load(&b[i]);
        v128_t vr = wasm_f32x4_add(va, vb);
        wasm_v128_store(&result[i], vr);
    }

    // 处理剩余元素
    for (; i < n; i++) {
        result[i] = a[i] + b[i];
    }
}
```

---

## 10. 实战案例分析

### 10.1 游戏引擎内存管理

```c
// game_memory_system.c
#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <string.h>

// 游戏对象内存系统
#define MAX_GAME_OBJECTS 10000
#define GAME_OBJECT_SIZE 256

// 内存区域定义
typedef enum {
    MEM_PERMAMENT,    // 永久存储（关卡数据）
    MEM_LEVEL,        // 关卡生命周期
    MEM_TEMPORARY,    // 临时分配（每帧清理）
    MEM_COUNT
} MemoryZone;

// 内存堆管理器
typedef struct {
    char* base;
    size_t size;
    size_t used;
    int mark;
} MemoryHeap;

static MemoryHeap heaps[MEM_COUNT];

// 初始化内存系统
EMSCRIPTEN_KEEPALIVE
void memory_system_init() {
    // 永久存储: 64MB
    heaps[MEM_PERMAMENT].size = 64 * 1024 * 1024;
    heaps[MEM_PERMAMENT].base = (char*)malloc(heaps[MEM_PERMAMENT].size);
    heaps[MEM_PERMAMENT].used = 0;

    // 关卡存储: 128MB
    heaps[MEM_LEVEL].size = 128 * 1024 * 1024;
    heaps[MEM_LEVEL].base = (char*)malloc(heaps[MEM_LEVEL].size);
    heaps[MEM_LEVEL].used = 0;

    // 临时存储: 32MB
    heaps[MEM_TEMPORARY].size = 32 * 1024 * 1024;
    heaps[MEM_TEMPORARY].base = (char*)malloc(heaps[MEM_TEMPORARY].size);
    heaps[MEM_TEMPORARY].used = 0;
}

// 区域分配
EMSCRIPTEN_KEEPALIVE
void* zone_alloc(MemoryZone zone, size_t size, size_t alignment) {
    if (zone >= MEM_COUNT) return NULL;

    MemoryHeap* heap = &heaps[zone];

    // 对齐
    size_t aligned_used = (heap->used + alignment - 1) & ~(alignment - 1);

    if (aligned_used + size > heap->size) {
        return NULL;  // 内存不足
    }

    void* ptr = heap->base + aligned_used;
    heap->used = aligned_used + size;

    return ptr;
}

// 重置区域
EMSCRIPTEN_KEEPALIVE
void zone_reset(MemoryZone zone) {
    if (zone < MEM_COUNT) {
        heaps[zone].used = 0;
    }
}

// 每帧清理临时内存
EMSCRIPTEN_KEEPALIVE
void frame_end() {
    zone_reset(MEM_TEMPORARY);
}

// 游戏对象池
typedef struct GameObject {
    int id;
    float x, y, z;
    float rotation;
    int active;
    // ... 更多字段
} GameObject;

static GameObject* object_pool = NULL;
static int* free_list = NULL;
static int free_count = 0;
static int next_id = 1;

EMSCRIPTEN_KEEPALIVE
void object_pool_init() {
    object_pool = (GameObject*)zone_alloc(MEM_PERMAMENT,
        sizeof(GameObject) * MAX_GAME_OBJECTS, 64);
    free_list = (int*)zone_alloc(MEM_PERMAMENT,
        sizeof(int) * MAX_GAME_OBJECTS, 64);

    // 初始化空闲列表
    for (int i = 0; i < MAX_GAME_OBJECTS; i++) {
        free_list[i] = MAX_GAME_OBJECTS - 1 - i;
    }
    free_count = MAX_GAME_OBJECTS;
}

EMSCRIPTEN_KEEPALIVE
GameObject* object_create() {
    if (free_count == 0) return NULL;

    int index = free_list[--free_count];
    GameObject* obj = &object_pool[index];

    memset(obj, 0, sizeof(GameObject));
    obj->id = next_id++;
    obj->active = 1;

    return obj;
}

EMSCRIPTEN_KEEPALIVE
void object_destroy(GameObject* obj) {
    if (!obj || !obj->active) return;

    int index = obj - object_pool;
    if (index < 0 || index >= MAX_GAME_OBJECTS) return;

    obj->active = 0;
    free_list[free_count++] = index;
}
```

### 10.2 图像处理内存优化

```c
// image_memory.c
#include <emscripten/emscripten.h>
#include <stdlib.h>

// 图像缓冲区管理
#define MAX_IMAGE_CACHE 10

typedef struct {
    unsigned char* data;
    int width;
    int height;
    int channels;
    int ref_count;
    unsigned int last_used;
} ImageBuffer;

static ImageBuffer image_cache[MAX_IMAGE_CACHE];
static unsigned int frame_counter = 0;

// 智能图像分配
EMSCRIPTEN_KEEPALIVE
ImageBuffer* image_allocate(int width, int height, int channels) {
    size_t size = width * height * channels;

    // 查找可重用的缓冲区
    for (int i = 0; i < MAX_IMAGE_CACHE; i++) {
        if (image_cache[i].ref_count == 0 &&
            image_cache[i].width * image_cache[i].height * image_cache[i].channels >= size) {
            // 重用此缓冲区
            image_cache[i].ref_count = 1;
            image_cache[i].width = width;
            image_cache[i].height = height;
            image_cache[i].channels = channels;
            image_cache[i].last_used = frame_counter;
            return &image_cache[i];
        }
    }

    // 分配新缓冲区
    for (int i = 0; i < MAX_IMAGE_CACHE; i++) {
        if (image_cache[i].data == NULL) {
            image_cache[i].data = (unsigned char*)malloc(size);
            if (!image_cache[i].data) return NULL;

            image_cache[i].width = width;
            image_cache[i].height = height;
            image_cache[i].channels = channels;
            image_cache[i].ref_count = 1;
            image_cache[i].last_used = frame_counter;
            return &image_cache[i];
        }
    }

    // 缓存满，清理最旧的
    int oldest = 0;
    for (int i = 1; i < MAX_IMAGE_CACHE; i++) {
        if (image_cache[i].ref_count == 0 &&
            image_cache[i].last_used < image_cache[oldest].last_used) {
            oldest = i;
        }
    }

    if (image_cache[oldest].ref_count == 0) {
        free(image_cache[oldest].data);
        image_cache[oldest].data = (unsigned char*)malloc(size);
        if (!image_cache[oldest].data) return NULL;

        image_cache[oldest].width = width;
        image_cache[oldest].height = height;
        image_cache[oldest].channels = channels;
        image_cache[oldest].ref_count = 1;
        image_cache[oldest].last_used = frame_counter;
        return &image_cache[oldest];
    }

    return NULL;
}

EMSCRIPTEN_KEEPALIVE
void image_retain(ImageBuffer* img) {
    if (img) img->ref_count++;
}

EMSCRIPTEN_KEEPALIVE
void image_release(ImageBuffer* img) {
    if (img && --img->ref_count == 0) {
        // 不立即释放，保留在缓存中
        img->last_used = frame_counter;
    }
}

EMSCRIPTEN_KEEPALIVE
void image_cache_update() {
    frame_counter++;

    // 可选：清理长期未使用的缓冲区
    for (int i = 0; i < MAX_IMAGE_CACHE; i++) {
        if (image_cache[i].ref_count == 0 &&
            image_cache[i].data &&
            frame_counter - image_cache[i].last_used > 60) {
            free(image_cache[i].data);
            image_cache[i].data = NULL;
        }
    }
}
```

---

*文档版本: 1.0*
*最后更新: 2024年*
*作者: C_Lang 项目团队*


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
