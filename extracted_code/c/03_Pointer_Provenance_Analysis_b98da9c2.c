/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
 * Line: 269
 * Language: c
 * Block ID: b98da9c2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 指针来源与原子操作
#include <stdatomic.h>
#include <stdint.h>

_Atomic(void*) shared_ptr;

void c11_provenance_atomic(void) {
    int* new_obj = malloc(sizeof(int));
    *new_obj = 42;

    // 原子存储指针，来源一同传递
    atomic_store_explicit(&shared_ptr, new_obj, memory_order_release);
}

void* c11_provenance_consume(void) {
    // consume 内存序：依赖携带来源
    void* ptr = atomic_load_explicit(&shared_ptr, memory_order_consume);

    // 基于 ptr 的依赖操作看到同步效果
    int* int_ptr = (int*)ptr;
    if (int_ptr) {
        int value = *int_ptr;  // 保证看到 42
        (void)value;
    }
    return ptr;
}
