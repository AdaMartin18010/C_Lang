/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\02_C11_Memory_Model.md
 * Line: 425
 * Language: c
 * Block ID: d3c28db3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 自旋锁的C11实现
#include <stdatomic.h>
#include <stdbool.h>

typedef atomic_flag spinlock_t;

#define SPINLOCK_INIT ATOMIC_FLAG_INIT

static inline void spinlock_init(spinlock_t *lock) {
    atomic_flag_clear_explicit(lock, memory_order_relaxed);
}

static inline void spinlock_lock(spinlock_t *lock) {
    // 自旋直到获取锁
    while (atomic_flag_test_and_set_explicit(lock, memory_order_acquire)) {
        // 可选：让出CPU或暂停
        // __asm__ volatile ("pause" ::: "memory");
    }
}

static inline void spinlock_unlock(spinlock_t *lock) {
    atomic_flag_clear_explicit(lock, memory_order_release);
}

// 使用示例
spinlock_t my_lock = SPINLOCK_INIT;
int shared_counter = 0;

void increment_counter(void) {
    spinlock_lock(&my_lock);
    shared_counter++;
    spinlock_unlock(&my_lock);
}
