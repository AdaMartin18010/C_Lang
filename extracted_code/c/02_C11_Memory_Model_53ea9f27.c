/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\02_C11_Memory_Model.md
 * Line: 463
 * Language: c
 * Block ID: 53ea9f27
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 基于C11原子操作的读写锁
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    atomic_int state;
} rwlock_t;

#define RWLOCK_UNLOCKED 0
#define RWLOCK_WRITER (-1)

void rwlock_init(rwlock_t *lock) {
    atomic_init(&lock->state, RWLOCK_UNLOCKED);
}

void rwlock_read_lock(rwlock_t *lock) {
    int state;
    do {
        // 等待写者释放
        while ((state = atomic_load_explicit(&lock->state, memory_order_relaxed)) < 0) {
            // 自旋
        }
        // 尝试增加读者计数
    } while (!atomic_compare_exchange_weak_explicit(
        &lock->state, &state, state + 1,
        memory_order_acquire, memory_order_relaxed));
}

void rwlock_read_unlock(rwlock_t *lock) {
    atomic_fetch_sub_explicit(&lock->state, 1, memory_order_release);
}

void rwlock_write_lock(rwlock_t *lock) {
    int expected = RWLOCK_UNLOCKED;
    while (!atomic_compare_exchange_weak_explicit(
        &lock->state, &expected, RWLOCK_WRITER,
        memory_order_acquire, memory_order_relaxed)) {
        expected = RWLOCK_UNLOCKED;
        // 自旋
    }
}

void rwlock_write_unlock(rwlock_t *lock) {
    atomic_store_explicit(&lock->state, RWLOCK_UNLOCKED, memory_order_release);
}
