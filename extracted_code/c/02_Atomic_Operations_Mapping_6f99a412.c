/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 796
 * Language: c
 * Block ID: 6f99a412
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 高效自旋锁
#include <stdatomic.h>

typedef struct {
    _Atomic bool locked;
} Spinlock;

void spinlock_lock(Spinlock* lock) {
    // TTAS: Test-And-Test-And-Set
    for (;;) {
        if (!atomic_load_explicit(&lock->locked, memory_order_relaxed)) {
            if (!atomic_exchange_explicit(&lock->locked, true, memory_order_acquire)) {
                return;
            }
        }
    }
}

void spinlock_unlock(Spinlock* lock) {
    atomic_store_explicit(&lock->locked, false, memory_order_release);
}
