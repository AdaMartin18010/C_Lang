/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\10_Threads_C11.md
 * Line: 734
 * Language: c
 * Block ID: 12e5a575
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>
#include <stdbool.h>

/* ========== 生产者-消费者队列 ========== */
#define QUEUE_SIZE 16

typedef struct {
    int items[QUEUE_SIZE];
    int head;
    int tail;
    int count;

    mtx_t mutex;
    cnd_t not_full;   /* 队列不满时通知 */
    cnd_t not_empty;  /* 队列不空时通知 */
} BoundedQueue;

void queue_init(BoundedQueue* q) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    mtx_init(&q->mutex, mtx_plain);
    cnd_init(&q->not_full);
    cnd_init(&q->not_empty);
}

void queue_destroy(BoundedQueue* q) {
    mtx_destroy(&q->mutex);
    cnd_destroy(&q->not_full);
    cnd_destroy(&q->not_empty);
}

/* 生产者入队 */
bool queue_push(BoundedQueue* q, int item, int timeout_ms) {
    mtx_lock(&q->mutex);

    /* 使用while防止虚假唤醒 */
    while (q->count == QUEUE_SIZE) {
        if (timeout_ms < 0) {
            /* 无限等待 */
            cnd_wait(&q->not_full, &q->mutex);
        } else {
            /* 超时等待 */
            struct timespec deadline;
            timespec_get(&deadline, TIME_UTC);
            deadline.tv_nsec += timeout_ms * 1000000;
            if (deadline.tv_nsec >= 1000000000) {
                deadline.tv_sec++;
                deadline.tv_nsec -= 1000000000;
            }

            int rc = cnd_timedwait(&q->not_full, &q->mutex, &deadline);
            if (rc == thrd_timeout) {
                mtx_unlock(&q->mutex);
                return false;
            }
        }
    }

    /* 添加元素 */
    q->items[q->tail] = item;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->count++;

    printf("[Producer] Pushed %d, count=%d\n", item, q->count);

    /* 通知消费者 */
    cnd_signal(&q->not_empty);
    mtx_unlock(&q->mutex);
    return true;
}

/* 消费者出队 */
bool queue_pop(BoundedQueue* q, int* item, int timeout_ms) {
    mtx_lock(&q->mutex);

    /* 使用while防止虚假唤醒 */
    while (q->count == 0) {
        if (timeout_ms < 0) {
            cnd_wait(&q->not_empty, &q->mutex);
        } else {
            struct timespec deadline;
            timespec_get(&deadline, TIME_UTC);
            deadline.tv_nsec += timeout_ms * 1000000;
            if (deadline.tv_nsec >= 1000000000) {
                deadline.tv_sec++;
                deadline.tv_nsec -= 1000000000;
            }

            int rc = cnd_timedwait(&q->not_empty, &q->mutex, &deadline);
            if (rc == thrd_timeout) {
                mtx_unlock(&q->mutex);
                return false;
            }
        }
    }

    /* 取出元素 */
    *item = q->items[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->count--;

    printf("[Consumer] Popped %d, count=%d\n", *item, q->count);

    /* 通知生产者 */
    cnd_signal(&q->not_full);
    mtx_unlock(&q->mutex);
    return true;
}

/* ========== 多生产者多消费者示例 ========== */

typedef struct {
    BoundedQueue* queue;
    int id;
    int items_to_produce;
} ProducerArg;

typedef struct {
    BoundedQueue* queue;
    int id;
    int items_to_consume;
} ConsumerArg;

int producer_thread(void *arg) {
    ProducerArg* p = (ProducerArg*)arg;

    for (int i = 0; i < p->items_to_produce; i++) {
        int item = p->id * 1000 + i;

        /* 生产数据 */
        while (!queue_push(p->queue, item, 1000)) {
            printf("[Producer %d] Push timeout, retrying...\n", p->id);
        }

        /* 模拟生产时间 */
        thrd_sleep(&(struct timespec){.tv_nsec = 50000000}, NULL);
    }

    printf("[Producer %d] Finished producing %d items\n",
           p->id, p->items_to_produce);
    return 0;
}

int consumer_thread(void *arg) {
    ConsumerArg* c = (ConsumerArg*)arg;
    int items_consumed = 0;

    while (items_consumed < c->items_to_consume) {
        int item;

        if (queue_pop(c->queue, &item, 2000)) {
            /* 处理数据 */
            (void)item; /* 使用item */
            items_consumed++;

            /* 模拟处理时间 */
            thrd_sleep(&(struct timespec){.tv_nsec = 80000000}, NULL);
        } else {
            printf("[Consumer %d] Pop timeout, checking...\n", c->id);
        }
    }

    printf("[Consumer %d] Finished consuming %d items\n",
           c->id, items_consumed);
    return 0;
}

/* ========== 读写锁模式（用条件变量实现） ========== */

typedef struct {
    mtx_t mutex;
    cnd_t read_cond;
    cnd_t write_cond;

    int active_readers;
    int waiting_writers;
    bool writer_active;
} RWLock;

void rwlock_init(RWLock* lock) {
    mtx_init(&lock->mutex, mtx_plain);
    cnd_init(&lock->read_cond);
    cnd_init(&lock->write_cond);
    lock->active_readers = 0;
    lock->waiting_writers = 0;
    lock->writer_active = false;
}

void rwlock_destroy(RWLock* lock) {
    mtx_destroy(&lock->mutex);
    cnd_destroy(&lock->read_cond);
    cnd_destroy(&lock->write_cond);
}

void rwlock_read_lock(RWLock* lock) {
    mtx_lock(&lock->mutex);

    /* 等待所有写者完成 */
    while (lock->writer_active || lock->waiting_writers > 0) {
        cnd_wait(&lock->read_cond, &lock->mutex);
    }

    lock->active_readers++;
    mtx_unlock(&lock->mutex);
}

void rwlock_read_unlock(RWLock* lock) {
    mtx_lock(&lock->mutex);

    lock->active_readers--;

    /* 最后一个读者唤醒写者 */
    if (lock->active_readers == 0) {
        cnd_signal(&lock->write_cond);
    }

    mtx_unlock(&lock->mutex);
}

void rwlock_write_lock(RWLock* lock) {
    mtx_lock(&lock->mutex);

    lock->waiting_writers++;

    /* 等待所有读者和其他写者完成 */
    while (lock->writer_active || lock->active_readers > 0) {
        cnd_wait(&lock->write_cond, &lock->mutex);
    }

    lock->waiting_writers--;
    lock->writer_active = true;
    mtx_unlock(&lock->mutex);
}

void rwlock_write_unlock(RWLock* lock) {
    mtx_lock(&lock->mutex);

    lock->writer_active = false;

    /* 优先唤醒写者，如果没有则唤醒所有读者 */
    if (lock->waiting_writers > 0) {
        cnd_signal(&lock->write_cond);
    } else {
        cnd_broadcast(&lock->read_cond);
    }

    mtx_unlock(&lock->mutex);
}

/* ========== 主测试程序 ========== */
int main(void) {
    printf("=== Condition Variable Demo ===\n\n");

    BoundedQueue queue;
    queue_init(&queue);

    /* 配置 */
    const int NUM_PRODUCERS = 2;
    const int NUM_CONSUMERS = 3;
    const int ITEMS_PER_PRODUCER = 10;
    const int TOTAL_ITEMS = NUM_PRODUCERS * ITEMS_PER_PRODUCER;
    const int ITEMS_PER_CONSUMER = (TOTAL_ITEMS + NUM_CONSUMERS - 1) / NUM_CONSUMERS;

    thrd_t producers[NUM_PRODUCERS];
    thrd_t consumers[NUM_CONSUMERS];
    ProducerArg pargs[NUM_PRODUCERS];
    ConsumerArg cargs[NUM_CONSUMERS];

    printf("Configuration:\n");
    printf("  Producers: %d x %d items\n", NUM_PRODUCERS, ITEMS_PER_PRODUCER);
    printf("  Consumers: %d x ~%d items\n", NUM_CONSUMERS, ITEMS_PER_CONSUMER);
    printf("  Queue size: %d\n\n", QUEUE_SIZE);

    /* 启动消费者 */
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        cargs[i].queue = &queue;
        cargs[i].id = i + 1;
        cargs[i].items_to_consume = ITEMS_PER_CONSUMER;
        thrd_create(&consumers[i], consumer_thread, &cargs[i]);
    }

    /* 启动生产者 */
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pargs[i].queue = &queue;
        pargs[i].id = i + 1;
        pargs[i].items_to_produce = ITEMS_PER_PRODUCER;
        thrd_create(&producers[i], producer_thread, &pargs[i]);
    }

    /* 等待所有线程完成 */
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        thrd_join(producers[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        thrd_join(consumers[i], NULL);
    }

    printf("\n=== All threads completed ===\n");

    queue_destroy(&queue);
    return 0;
}
