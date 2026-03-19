/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\06_Lab6_Multithreading.md
 * Line: 417
 * Language: c
 * Block ID: 9386bcb5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/barrier.c

struct barrier {
    int count;          // 当前等待的线程数
    int max;            // 需要等待的线程总数
    int lock;           // 保护barrier的锁
    int passed;         // 当前轮次 (防止过早通过)
};

void barrier_init(struct barrier *b, int n) {
    b->count = 0;
    b->max = n;
    b->lock = 0;
    b->passed = 0;
}

void barrier_wait(struct barrier *b) {
    int passed = b->passed;

    // 增加计数
    while (__sync_lock_test_and_set(&b->lock, 1) != 0)
        ;
    b->count++;
    __sync_lock_release(&b->lock);

    if (b->count == b->max) {
        // 最后一个到达的线程
        b->count = 0;
        b->passed++;  // 增加轮次，唤醒其他线程
    } else {
        // 等待其他线程
        while (b->passed == passed) {
            thread_yield();
        }
    }
}
