/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\03_Concurrency_Safety_Layers.md
 * Line: 240
 * Language: c
 * Block ID: 336ce468
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 自旋锁 - 适合极短临界区
typedef atomic_flag SpinLock;

void spin_lock(SpinLock *lock) {
    while (atomic_flag_test_and_set(lock)) {
        // 忙等 - 消耗CPU
        // 可添加退避策略
        for (int i = 0; i < 100; i++) __asm__("pause");
    }
}

// 自适应锁 - 先自旋，后阻塞
typedef struct {
    atomic_int state;
    mtx_t mutex;
} AdaptiveLock;

void adaptive_lock(AdaptiveLock *lock) {
    // 先尝试自旋一段时间
    for (int spin = 0; spin < 1000; spin++) {
        int expected = 0;
        if (atomic_compare_exchange_weak(&lock->state, &expected, 1)) {
            return;  // 获得锁
        }
        __asm__("pause");
    }
    // 自旋失败，进入内核阻塞
    mtx_lock(&lock->mutex);
}
