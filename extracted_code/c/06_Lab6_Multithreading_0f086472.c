/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\06_Lab6_Multithreading.md
 * Line: 246
 * Language: c
 * Block ID: 0f086472
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/prodcon.c
#include "kernel/types.h"
#include "user/user.h"

#define N 10          // 缓冲区大小
#define NUM_ITEMS 100 // 生产/消费数量

int buffer[N];
int count = 0;
int in = 0, out = 0;

// 简单的锁 (使用原子操作)
int lock = 0;

void acquire(int *lk) {
    while (__sync_lock_test_and_set(lk, 1) != 0)
        ;
}

void release(int *lk) {
    __sync_lock_release(lk);
}

void producer() {
    for (int i = 0; i < NUM_ITEMS; i++) {
        acquire(&lock);

        // 等待有空位 (简化版，实际应使用条件变量)
        while (count == N) {
            release(&lock);
            thread_yield();
            acquire(&lock);
        }

        buffer[in] = i;
        in = (in + 1) % N;
        count++;

        release(&lock);
    }
}

void consumer() {
    for (int i = 0; i < NUM_ITEMS; i++) {
        acquire(&lock);

        // 等待有数据
        while (count == 0) {
            release(&lock);
            thread_yield();
            acquire(&lock);
        }

        int item = buffer[out];
        out = (out + 1) % N;
        count--;

        release(&lock);

        printf("consumed: %d\n", item);
    }
}

int main() {
    thread_create(producer);
    thread_create(consumer);

    // 主线程也参与调度
    while (count < NUM_ITEMS) {
        thread_yield();
    }

    exit(0);
}
