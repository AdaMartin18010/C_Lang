/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 302
 * Language: c
 * Block ID: bb0ecc72
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 原子交换
_Atomic bool lock = ATOMIC_VAR_INIT(false);

bool try_lock(void) {
    // 如果之前是 false（未锁定），设为 true，返回 false（成功）
    return atomic_exchange_explicit(&lock, true, memory_order_acquire);
}

void unlock(void) {
    atomic_store_explicit(&lock, false, memory_order_release);
}
