/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 538
 * Language: c
 * Block ID: 78bf6709
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 atomic_flag
#include <stdatomic.h>

// 静态初始化（保证 lock-free）
atomic_flag lock = ATOMIC_FLAG_INIT;

void flag_example(void) {
    // 测试并设置
    bool was_set = atomic_flag_test_and_set(&lock);

    // 清除
    atomic_flag_clear(&lock);
}

// 基于 atomic_flag 的自旋锁
void spinlock_lock(atomic_flag* lock) {
    while (atomic_flag_test_and_set_explicit(lock, memory_order_acquire)) {
        // 自旋
    }
}

void spinlock_unlock(atomic_flag* lock) {
    atomic_flag_clear_explicit(lock, memory_order_release);
}
