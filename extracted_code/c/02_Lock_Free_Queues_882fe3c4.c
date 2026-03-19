/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\02_Lock_Free_Queues.md
 * Line: 715
 * Language: c
 * Block ID: 882fe3c4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// benchmark.c
#include "spsc_queue.h"
#include "mpmc_queue.h"
#include "disruptor.h"
#include <pthread.h>
#include <math.h>

#define ITERATIONS 10000000
#define QUEUE_SIZE 65536

// 测试数据结构
typedef struct {
    int64_t value;
    uint64_t timestamp;
} TestMessage;

// 锁队列（用于对比）
typedef struct {
    pthread_mutex_t lock;
    void **buffer;
    size_t head, tail, capacity;
} LockedQueue;

LockedQueue* locked_queue_create(size_t capacity) {
    LockedQueue *q = malloc(sizeof(LockedQueue));
    pthread_mutex_init(&q->lock, NULL);
    q->buffer = calloc(capacity, sizeof(void*));
    q->capacity = capacity;
    q->head = q->tail = 0;
    return q;
}

bool locked_queue_enqueue(LockedQueue *q, void *item) {
    pthread_mutex_lock(&q->lock);
    size_t next_head = (q->head + 1) % q->capacity;
    if (next_head == q->tail) {
        pthread_mutex_unlock(&q->lock);
        return false;
    }
    q->buffer[q->head] = item;
    q->head = next_head;
    pthread_mutex_unlock(&q->lock);
    return true;
}

void* locked_queue_dequeue(LockedQueue *q) {
    pthread_mutex_lock(&q->lock);
    if (q->head == q->tail) {
        pthread_mutex_unlock(&q->lock);
        return NULL;
    }
    void *item = q->buffer[q->tail];
    q->tail = (q->tail + 1) % q->capacity;
    pthread_mutex_unlock(&q->lock);
    return item;
}

// SPSC测试
typedef struct {
    SPSCQueue *q;
    int64_t count;
    uint64_t *latencies;
} SPSCContext;

void* spsc_producer(void *arg) {
    SPSCContext *ctx = arg;
    for (int64_t i = 0; i < ctx->count; i++) {
        TestMessage *msg = malloc(sizeof(TestMessage));
        msg->value = i;
        msg->timestamp = rdtsc();
        while (!spsc_enqueue(ctx->q, msg)) {
            __asm__ volatile ("pause");
        }
    }
    return NULL;
}

void* spsc_consumer(void *arg) {
    SPSCContext *ctx = arg;
    for (int64_t i = 0; i < ctx->count; i++) {
        TestMessage *msg;
        while (!(msg = spsc_dequeue(ctx->q))) {
            __asm__ volatile ("pause");
        }
        uint64_t latency = rdtsc() - msg->timestamp;
        ctx->latencies[i] = latency;
        free(msg);
    }
    return NULL;
}

// 运行SPSC测试
void benchmark_spsc(void) {
    printf("\n=== SPSC Queue Benchmark ===\n");

    SPSCQueue *q = spsc_queue_create(QUEUE_SIZE);
    SPSCContext ctx = {q, ITERATIONS, calloc(ITERATIONS, sizeof(uint64_t))};

    pthread_t prod, cons;
    uint64_t start = rdtsc();

    pthread_create(&prod, NULL, spsc_producer, &ctx);
    pthread_create(&cons, NULL, spsc_consumer, &ctx);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    uint64_t total_cycles = rdtsc() - start;

    // 计算统计数据
    double avg_latency = 0;
    uint64_t max_latency = 0;
    for (int64_t i = 0; i < ITERATIONS; i++) {
        avg_latency += ctx.latencies[i];
        if (ctx.latencies[i] > max_latency) max_latency = ctx.latencies[i];
    }
    avg_latency /= ITERATIONS;

    printf("Messages: %d\n", ITERATIONS);
    printf("Total time: %.2f ms\n", total_cycles / 2.6e6);
    printf("Throughput: %.2f M msg/s\n",
           ITERATIONS / (total_cycles / 2.6e9));
    printf("Avg latency: %.0f cycles\n", avg_latency);
    printf("Max latency: %lu cycles\n", max_latency);

    free(ctx.latencies);
    spsc_queue_destroy(q);
}

// 锁队列测试
void* locked_producer(void *arg) {
    LockedQueue *q = arg;
    for (int i = 0; i < ITERATIONS; i++) {
        TestMessage *msg = malloc(sizeof(TestMessage));
        msg->value = i;
        while (!locked_queue_enqueue(q, msg)) {
            sched_yield();
        }
    }
    return NULL;
}

void* locked_consumer(void *arg) {
    LockedQueue *q = arg;
    for (int i = 0; i < ITERATIONS; i++) {
        TestMessage *msg;
        while (!(msg = locked_queue_dequeue(q))) {
            sched_yield();
        }
        free(msg);
    }
    return NULL;
}

void benchmark_locked(void) {
    printf("\n=== Locked Queue Benchmark ===\n");

    LockedQueue *q = locked_queue_create(QUEUE_SIZE);
    pthread_t prod, cons;

    uint64_t start = rdtsc();
    pthread_create(&prod, NULL, locked_producer, q);
    pthread_create(&cons, NULL, locked_consumer, q);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    uint64_t total_cycles = rdtsc() - start;

    printf("Messages: %d\n", ITERATIONS);
    printf("Total time: %.2f ms\n", total_cycles / 2.6e6);
    printf("Throughput: %.2f M msg/s\n",
           ITERATIONS / (total_cycles / 2.6e9));
}

int main(void) {
    printf("=== Lock-Free Queue Benchmark ===\n");
    printf("Iterations: %d, Queue size: %d\n", ITERATIONS, QUEUE_SIZE);

    benchmark_spsc();
    benchmark_locked();

    // Disruptor测试
    printf("\n=== Disruptor Benchmark ===\n");
    RingBuffer *rb = ring_buffer_create(QUEUE_SIZE, WAIT_BUSY_SPIN);

    uint64_t start = rdtsc();
    for (int i = 0; i < ITERATIONS; i++) {
        int64_t seq = ring_buffer_next(rb);
        RingBufferEvent *evt = ring_buffer_get(rb, seq);
        evt->data = (void*)(intptr_t)i;
        ring_buffer_publish(rb, seq);
    }
    uint64_t disruptor_cycles = rdtsc() - start;

    printf("Disruptor publish throughput: %.2f M msg/s\n",
           ITERATIONS / (disruptor_cycles / 2.6e9));

    ring_buffer_destroy(rb);

    return 0;
}

// 编译:
// gcc -o benchmark benchmark.c spsc_queue.c mpmc_queue.c disruptor.c \
//     -lpthread -O3 -march=native -std=c17
