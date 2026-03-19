/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
 * Line: 565
 * Language: c
 * Block ID: dd3eb1d8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 显式内存序完整示例
#include <stdatomic.h>
#include <stdbool.h>

typedef struct {
    _Atomic int ref_count;
    void* data;
} SharedObject;

SharedObject* create_object(void) {
    SharedObject* obj = malloc(sizeof(SharedObject));
    atomic_init(&obj->ref_count, 1);  // 初始化不需要序
    obj->data = NULL;
    return obj;
}

void retain(SharedObject* obj) {
    // Relaxed: 仅计数，不需要同步
    atomic_fetch_add_explicit(&obj->ref_count, 1, memory_order_relaxed);
}

void release_object(SharedObject* obj) {
    // Release: 确保之前的写入可见
    if (atomic_fetch_sub_explicit(&obj->ref_count, 1,
                                   memory_order_release) == 1) {
        // Acquire: 看到 count==0 后，确保看到所有 retain 的操作
        atomic_thread_fence(memory_order_acquire);
        free(obj->data);
        free(obj);
    }
}

// 更现代的 C11 方式 (C17)
bool compare_and_set(_Atomic int* ptr, int expected, int desired) {
    return atomic_compare_exchange_strong_explicit(
        ptr, &expected, desired,
        memory_order_acq_rel,   // 成功时的内存序
        memory_order_relaxed    // 失败时的内存序
    );
}
