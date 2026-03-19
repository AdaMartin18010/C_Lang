/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\06_Lab6_Multithreading.md
 * Line: 518
 * Language: c
 * Block ID: 4c29496d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/spinlock.h

typedef struct {
    int locked;
} spinlock_t;

void spinlock_init(spinlock_t *lk) {
    lk->locked = 0;
}

void spinlock_acquire(spinlock_t *lk) {
    while (__sync_lock_test_and_set(&lk->locked, 1) != 0)
        ;
    // 内存屏障，防止编译器重排序
    __sync_synchronize();
}

void spinlock_release(spinlock_t *lk) {
    __sync_synchronize();
    __sync_lock_release(&lk->locked);
}
