/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 331
 * Language: c
 * Block ID: 351f59ac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 比较并交换
_Atomic int counter = ATOMIC_VAR_INIT(0);

bool cas_example(int expected, int desired) {
    // 强 CAS
    return atomic_compare_exchange_strong(&counter, &expected, desired);
}

bool cas_weak_example(int expected, int desired) {
    // 弱 CAS（允许伪失败，但可能更快）
    return atomic_compare_exchange_weak(&counter, &expected, desired);
}

// 显式内存序
bool cas_with_order(int* ptr, int expected, int desired) {
    return atomic_compare_exchange_strong_explicit(
        ptr, &expected, desired,
        memory_order_acq_rel,   // 成功时内存序
        memory_order_relaxed    // 失败时内存序
    );
}
