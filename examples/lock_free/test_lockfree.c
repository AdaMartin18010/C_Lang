/*
 * 无锁数据结构测试
 * 编译: gcc -Wall -Wextra -O2 -std=c11 -o test_lockfree spsc_ring.c test_lockfree.c
 */
#include "spsc_ring.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

#define RING_SIZE 1024
#define MSG_COUNT 100000

/* 测试基础 push/pop */
void test_basic(void) {
    printf("=== SPSC Ring: Basic Push/Pop ===\n");
    SpscRing *ring = spsc_ring_create(RING_SIZE);
    assert(ring);

    const char *msg = "Hello, Lock-Free!";
    size_t len = strlen(msg) + 1;

    assert(spsc_ring_push(ring, msg, len));
    assert(spsc_ring_pop_avail(ring) == len);

    char buf[64];
    size_t n = spsc_ring_pop(ring, buf, sizeof(buf));
    assert(n == len);
    assert(strcmp(buf, msg) == 0);

    spsc_ring_destroy(ring);
    printf("PASS\n\n");
}

/* 测试环绕写入 */
void test_wraparound(void) {
    printf("=== SPSC Ring: Wrap-Around ===\n");
    SpscRing *ring = spsc_ring_create(16);
    assert(ring);

    /* 填满大部分 */
    uint8_t data[10] = {1,2,3,4,5,6,7,8,9,10};
    assert(spsc_ring_push(ring, data, 10));

    /* 消费 8 字节 */
    uint8_t buf[16];
    assert(spsc_ring_pop(ring, buf, 8) == 8);

    /* 此时 head=10, tail=8, 再 push 6 字节应环绕 */
    uint8_t data2[6] = {11,12,13,14,15,16};
    assert(spsc_ring_push(ring, data2, 6));

    /* 消费所有 */
    size_t n = spsc_ring_pop(ring, buf, sizeof(buf));
    assert(n == 8); /* 剩余 2 + 新 6 = 8 */
    assert(buf[0] == 9 && buf[1] == 10); /* 之前剩余 */
    assert(buf[2] == 11 && buf[7] == 16); /* 新数据 */

    spsc_ring_destroy(ring);
    printf("PASS\n\n");
}

/* 生产者和消费者线程参数 */
typedef struct {
    SpscRing *ring;
    int id;
} ThreadArg;

/* 生产者线程 */
void *producer_thread(void *arg) {
    ThreadArg *a = arg;
    SpscRing *ring = a->ring;
    for (int i = 0; i < MSG_COUNT; i++) {
        uint32_t msg = (uint32_t)i;
        while (!spsc_ring_push(ring, &msg, sizeof(msg))) {
            sched_yield();
        }
    }
    return NULL;
}

/* 消费者线程 */
void *consumer_thread(void *arg) {
    ThreadArg *a = arg;
    SpscRing *ring = a->ring;
    int received = 0;
    uint32_t last = 0;
    while (received < MSG_COUNT) {
        uint32_t msg;
        size_t n = spsc_ring_pop(ring, &msg, sizeof(msg));
        if (n == sizeof(msg)) {
            assert(msg == (uint32_t)received);
            last = msg;
            received++;
        } else {
            sched_yield();
        }
    }
    (void)a; (void)last;
    return NULL;
}

/* 多线程 SPSC 测试 */
void test_spsc_threads(void) {
    printf("=== SPSC Ring: Multi-Threaded ===\n");
    SpscRing *ring = spsc_ring_create(4096);
    assert(ring);

    ThreadArg arg = {ring, 0};
    pthread_t producer, consumer;

    pthread_create(&consumer, NULL, consumer_thread, &arg);
    pthread_create(&producer, NULL, producer_thread, &arg);

    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);

    printf("  Transferred %d messages between threads\n", MSG_COUNT);
    spsc_ring_destroy(ring);
    printf("PASS\n\n");
}

/* 原子计数器基准 */
void benchmark_atomic_vs_mutex(void) {
    printf("=== Atomic Counter Benchmark ===\n");
    _Atomic uint64_t atomic_counter = 0;
    clock_t t0 = clock();
    for (int i = 0; i < 10000000; i++) {
        atomic_fetch_add_explicit(&atomic_counter, 1, memory_order_relaxed);
    }
    clock_t t1 = clock();
    printf("  Atomic increments: %.3f ms\n",
           (double)(t1 - t0) * 1000.0 / CLOCKS_PER_SEC);
    printf("  Final count: %llu\n", (unsigned long long)atomic_counter);
    printf("PASS\n\n");
}

int main(void) {
    test_basic();
    test_wraparound();
    test_spsc_threads();
    benchmark_atomic_vs_mutex();
    printf("All lock-free tests passed!\n");
    return 0;
}
