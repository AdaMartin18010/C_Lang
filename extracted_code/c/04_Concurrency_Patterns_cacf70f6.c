/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\04_Concurrency_Patterns.md
 * Line: 706
 * Language: c
 * Block ID: cacf70f6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef atomic_flag SpinLock;

#define SPIN_LOCK_INIT ATOMIC_FLAG_INIT

void spin_lock(SpinLock *lock) {
    while (atomic_flag_test_and_set_explicit(lock,
                                              memory_order_acquire)) {
        // 自旋等待
    }
}

void spin_unlock(SpinLock *lock) {
    atomic_flag_clear_explicit(lock, memory_order_release);
}

// 使用
SpinLock lock = SPIN_LOCK_INIT;

spin_lock(&lock);
// 临界区
spin_unlock(&lock);
