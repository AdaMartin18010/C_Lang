/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 848
 * Language: c
 * Block ID: 0c921d2a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 引用计数
typedef struct {
    _Atomic int count;
    void* data;
} RefCounted;

void retain(RefCounted* rc) {
    atomic_fetch_add_explicit(&rc->count, 1, memory_order_relaxed);
}

bool release(RefCounted* rc) {
    if (atomic_fetch_sub_explicit(&rc->count, 1, memory_order_release) == 1) {
        atomic_thread_fence(memory_order_acquire);
        return true;  // 需要销毁
    }
    return false;
}
