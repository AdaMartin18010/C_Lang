/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 496
 * Language: c
 * Block ID: f253a537
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
