/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
 * Line: 1192
 * Language: c
 * Block ID: 98fd3f85
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 Spinlock 实现
#include <stdatomic.h>
#include <stdbool.h>
#include <threads.h>

typedef struct {
    _Atomic bool locked;
} Spinlock;

#define SPINLOCK_INIT { ATOMIC_VAR_INIT(false) }

void spinlock_init(Spinlock* lock) {
    atomic_init(&lock->locked, false);
}

void spinlock_lock(Spinlock* lock) {
    // 测试并测试-设置 (test-and-test-and-set)
    for (;;) {
        // 首先宽松检查，避免在锁忙时产生内存屏障
        if (!atomic_load_explicit(&lock->locked, memory_order_relaxed)) {
            // 尝试获取锁
            if (!atomic_exchange_explicit(&lock->locked, true,
                                          memory_order_acquire)) {
                return;  // 成功获取锁
            }
        }
        // 自旋等待
        thrd_yield();
    }
}

bool spinlock_trylock(Spinlock* lock) {
    return !atomic_exchange_explicit(&lock->locked, true,
                                     memory_order_acquire);
}

void spinlock_unlock(Spinlock* lock) {
    // Release: 确保锁内所有写入在解锁后可见
    atomic_store_explicit(&lock->locked, false, memory_order_release);
}

// 使用示例
Spinlock g_lock = SPINLOCK_INIT;
int shared_data = 0;

int worker(void* arg) {
    (void)arg;
    for (int i = 0; i < 1000; i++) {
        spinlock_lock(&g_lock);
        shared_data++;  // 临界区
        spinlock_unlock(&g_lock);
    }
    return 0;
}
