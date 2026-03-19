/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\06_Lab6_Multithreading.md
 * Line: 325
 * Language: c
 * Block ID: 828dcb5c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/primes_thread.c
// 使用多线程的素数筛选

#include "kernel/types.h"
#include "user/user.h"

#define MAX_PRIMES 100

struct channel {
    int data[MAX_PRIMES];
    int head;
    int tail;
    int lock;
};

void channel_put(struct channel *c, int n) {
    while (__sync_lock_test_and_set(&c->lock, 1) != 0)
        ;
    c->data[c->tail++] = n;
    __sync_lock_release(&c->lock);
}

int channel_get(struct channel *c) {
    int n;
    while (1) {
        while (__sync_lock_test_and_set(&c->lock, 1) != 0)
            ;
        if (c->head < c->tail) {
            n = c->data[c->head++];
            __sync_lock_release(&c->lock);
            return n;
        }
        __sync_lock_release(&c->lock);
        thread_yield();
    }
}

void sieve(struct channel *in, int p) {
    printf("prime %d\n", p);

    struct channel out = { .head = 0, .tail = 0, .lock = 0 };
    int n;

    // 过滤倍数
    while ((n = channel_get(in)) != -1) {
        if (n % p != 0) {
            channel_put(&out, n);
        }
    }

    // 如果还有数，创建下一个筛子
    if (out.head < out.tail) {
        int next_p = channel_get(&out);
        channel_put(&out, -1);  // 结束标记

        // 创建新线程处理下一个素数
        // ... 需要线程传递参数的支持
    }
}
