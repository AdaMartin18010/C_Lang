/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 1671
 * Language: c
 * Block ID: 82e74556
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
