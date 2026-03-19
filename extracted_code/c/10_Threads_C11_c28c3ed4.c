/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\10_Threads_C11.md
 * Line: 2102
 * Language: c
 * Block ID: c28c3ed4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <stdatomic.h>
#include <time.h>
#include <stdbool.h>

/* ========== 配置常量 ========== */
#define BUFFER_SIZE 64
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 3
#define ITEMS_PER_PRODUCER 1000
#define MAX_ITEM_VALUE 10000

/* ========== 数据结构 ========== */

typedef struct {
    int value;
    int producer_id;
    struct timespec produced_at;
} Item;

typedef struct {
    Item buffer[BUFFER_SIZE];
    atomic_int head;  /* 消费者位置 */
    atomic_int tail;  /* 生产者位置 */
    atomic_int count; /* 当前项目数 */

    /* 同步 */
    mtx_t mutex;
    cnd_t not_full;
    cnd_t not_empty;

    /* 统计 */
    atomic_long total_produced;
    atomic_long total_consumed;
    atomic_long sum_produced;
    atomic_long sum_consumed;
} SharedBuffer;

typedef struct {
    int id;
    SharedBuffer* buffer;
} ThreadConfig;

/* ========== 共享缓冲区实现 ========== */

void buffer_init(SharedBuffer* b) {
    atomic_init(&b->head, 0);
    atomic_init(&b->tail, 0);
    atomic_init(&b->count, 0);
    atomic_init(&b->total_produced, 0);
    atomic_init(&b->total_consumed, 0);
    atomic_init(&b->sum_produced, 0);
    atomic_init(&b->sum_consumed, 0);

    mtx_init(&b->mutex, mtx_plain);
    cnd_init(&b->not_full);
    cnd_init(&b->not_empty);
}

void buffer_destroy(SharedBuffer* b) {
    mtx_destroy(&b->mutex);
    cnd_destroy(&b->not_full);
    cnd_destroy(&b->not_empty);
}

bool buffer_push(SharedBuffer* b, const Item* item, int timeout_ms) {
    mtx_lock(&b->mutex);

    while (atomic_load(&b->count) == BUFFER_SIZE) {
        if (timeout_ms == 0) {
            mtx_unlock(&b->mutex);
            return false;
        }

        struct timespec deadline;
        timespec_get(&deadline, TIME_UTC);
        deadline.tv_nsec += timeout_ms * 1000000;

        if (cnd_timedwait(&b->not_full, &b->mutex, &deadline)
            == thrd_timeout) {
            mtx_unlock(&b->mutex);
            return false;
        }
    }

    int tail = atomic_load(&b->tail);
    b->buffer[tail] = *item;
    atomic_store(&b->tail, (tail + 1) % BUFFER_SIZE);
    atomic_fetch_add(&b->count, 1);

    atomic_fetch_add(&b->total_produced, 1);
    atomic_fetch_add(&b->sum_produced, item->value);

    cnd_signal(&b->not_empty);
    mtx_unlock(&b->mutex);
    return true;
}

bool buffer_pop(SharedBuffer* b, Item* item, int timeout_ms) {
    mtx_lock(&b->mutex);

    while (atomic_load(&b->count) == 0) {
        if (timeout_ms == 0) {
            mtx_unlock(&b->mutex);
            return false;
        }

        struct timespec deadline;
        timespec_get(&deadline, TIME_UTC);
        deadline.tv_nsec += timeout_ms * 1000000;

        if (cnd_timedwait(&b->not_empty, &b->mutex, &deadline)
            == thrd_timeout) {
            mtx_unlock(&b->mutex);
            return false;
        }
    }

    int head = atomic_load(&b->head);
    *item = b->buffer[head];
    atomic_store(&b->head, (head + 1) % BUFFER_SIZE);
    atomic_fetch_sub(&b->count, 1);

    atomic_fetch_add(&b->total_consumed, 1);
    atomic_fetch_add(&b->sum_consumed, item->value);

    cnd_signal(&b->not_full);
    mtx_unlock(&b->mutex);
    return true;
}

/* ========== 生产者/消费者线程 ========== */

int producer_thread(void* arg) {
    ThreadConfig* cfg = (ThreadConfig*)arg;
    SharedBuffer* buf = cfg->buffer;

    printf("[Producer %d] Starting, will produce %d items\n",
           cfg->id, ITEMS_PER_PRODUCER);

    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        Item item = {
            .value = rand() % MAX_ITEM_VALUE,
            .producer_id = cfg->id
        };
        timespec_get(&item.produced_at, TIME_UTC);

        while (!buffer_push(buf, &item, 100)) {
            /* 超时重试 */
        }

        /* 偶尔让出CPU */
        if (i % 100 == 0) {
            thrd_yield();
        }
    }

    printf("[Producer %d] Finished\n", cfg->id);
    return 0;
}

int consumer_thread(void* arg) {
    ThreadConfig* cfg = (ThreadConfig*)arg;
    SharedBuffer* buf = cfg->buffer;

    printf("[Consumer %d] Starting\n", cfg->id);

    int consumed = 0;
    Item item;

    while (consumed < (NUM_PRODUCERS * ITEMS_PER_PRODUCER) / NUM_CONSUMERS + 1) {
        if (buffer_pop(buf, &item, 1000)) {
            consumed++;

            /* 模拟处理时间 */
            volatile double sum = 0;
            for (int i = 0; i < item.value % 1000; i++) {
                sum += i;
            }
        } else {
            /* 检查是否还有生产者在工作 */
            if (atomic_load(&buf->total_produced)
                >= NUM_PRODUCERS * ITEMS_PER_PRODUCER) {
                break;
            }
        }
    }

    printf("[Consumer %d] Finished, consumed %d items\n", cfg->id, consumed);
    return consumed;
}

/* ========== 主程序 ========== */

int main(void) {
    printf("========================================\n");
    printf("  Multi-Threaded Producer-Consumer Demo\n");
    printf("========================================\n");
    printf("Producers: %d x %d items\n", NUM_PRODUCERS, ITEMS_PER_PRODUCER);
    printf("Consumers: %d\n", NUM_CONSUMERS);
    printf("Buffer size: %d\n\n", BUFFER_SIZE);

    srand((unsigned)time(NULL));

    SharedBuffer buffer;
    buffer_init(&buffer);

    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    /* 创建线程 */
    thrd_t producers[NUM_PRODUCERS];
    thrd_t consumers[NUM_CONSUMERS];
    ThreadConfig pconfigs[NUM_PRODUCERS];
    ThreadConfig cconfigs[NUM_CONSUMERS];

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        cconfigs[i].id = i + 1;
        cconfigs[i].buffer = &buffer;
        thrd_create(&consumers[i], consumer_thread, &cconfigs[i]);
    }

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pconfigs[i].id = i + 1;
        pconfigs[i].buffer = &buffer;
        thrd_create(&producers[i], producer_thread, &pconfigs[i]);
    }

    /* 等待完成 */
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        thrd_join(producers[i], NULL);
    }

    /* 发送终止信号 - 等待队列清空 */
    thrd_sleep(&(struct timespec){.tv_sec = 1}, NULL);

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        thrd_join(consumers[i], NULL);
    }

    timespec_get(&end, TIME_UTC);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;

    /* 输出统计 */
    printf("\n========================================\n");
    printf("  Results\n");
    printf("========================================\n");
    printf("Time elapsed: %.3f seconds\n", elapsed);
    printf("Total produced: %ld\n", atomic_load(&buffer.total_produced));
    printf("Total consumed: %ld\n", atomic_load(&buffer.total_consumed));
    printf("Sum produced: %ld\n", atomic_load(&buffer.sum_produced));
    printf("Sum consumed: %ld\n", atomic_load(&buffer.sum_consumed));
    printf("Throughput: %.0f items/sec\n",
           atomic_load(&buffer.total_produced) / elapsed);

    if (atomic_load(&buffer.sum_produced) == atomic_load(&buffer.sum_consumed)) {
        printf("\n✓ Data integrity verified!\n");
    } else {
        printf("\n✗ DATA INTEGRITY ERROR!\n");
    }

    buffer_destroy(&buffer);
    return 0;
}
