/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 611
 * Language: c
 * Block ID: 703c9d54
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 原子指针
_Atomic(void*) atomic_ptr = ATOMIC_VAR_INIT(NULL);
_Atomic(int*) atomic_int_ptr = ATOMIC_VAR_INIT(NULL);

void ptr_example(void) {
    int value = 42;

    // 存储
    atomic_store_explicit(&atomic_int_ptr, &value, memory_order_release);

    // 加载
    int* ptr = atomic_load_explicit(&atomic_int_ptr, memory_order_acquire);

    // 交换
    int value2 = 100;
    int* old = atomic_exchange_explicit(&atomic_int_ptr, &value2, memory_order_acq_rel);
}
