/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Industrial_Linux\01_Industrial_Linux_System.md
 * Line: 3972
 * Language: c
 * Block ID: 04db98f1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file ring_buffer.c
 * @brief 无锁环形缓冲区实现
 * @details 用于高性能数据采集的多生产者单消费者无锁队列
 *
 * 编译: gcc -o ring_buffer ring_buffer.c -lpthread
 * 运行: ./ring_buffer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>

/* 缓存行大小 */
#define CACHE_LINE_SIZE     64

/* 数据点结构 */
typedef struct {
    uint64_t timestamp_ns;      /* 纳秒时间戳 */
    uint32_t tag_id;            /* 测点ID */
    double value;               /* 数值 */
    uint8_t quality;            /* 质量码 (0=Good, 1=Bad) */
    uint8_t reserved[7];        /* 对齐到32字节 */
} __attribute__((packed, aligned(32))) data_point_t;

/* 环形缓冲区结构 */
typedef struct {
    /* 生产者位置 - 独立缓存行 */
    alignas(CACHE_LINE_SIZE)
    atomic_size_t head;

    /* 消费者位置 - 独立缓存行 */
    alignas(CACHE_LINE_SIZE)
    atomic_size_t tail;

    /* 缓冲区大小 - 2的幂次方 */
    size_t capacity;
    size_t mask;

    /* 溢出计数 */
    atomic_size_t overflow_count;

    /* 数据缓冲区 */
    data_point_t *buffer;
} ring_buffer_t;

/* 统计信息 */
typedef struct {
    uint64_t produced;
    uint64_t consumed;
    uint64_t dropped;
    double avg_latency_ns;
    double max_latency_ns;
} buffer_stats_t;

/**
 * @brief 创建环形缓冲区
 * @param capacity 容量 (必须是2的幂次方)
 * @return 缓冲区指针
 */
ring_buffer_t *ring_buffer_create(size_t capacity)
{
    /* 确保容量是2的幂次方 */
    if ((capacity & (capacity - 1)) != 0) {
        fprintf(stderr, "[ERROR] Capacity must be power of 2\n");
        return NULL;
    }

    ring_buffer_t *rb = aligned_alloc(CACHE_LINE_SIZE, sizeof(ring_buffer_t));
    if (!rb) {
        return NULL;
    }

    memset(rb, 0, sizeof(ring_buffer_t));
    rb->capacity = capacity;
    rb->mask = capacity - 1;

    /* 分配缓冲区内存 */
    rb->buffer = aligned_alloc(64, capacity * sizeof(data_point_t));
    if (!rb->buffer) {
        free(rb);
        return NULL;
    }

    memset(rb->buffer, 0, capacity * sizeof(data_point_t));

    atomic_init(&rb->head, 0);
    atomic_init(&rb->tail, 0);
    atomic_init(&rb->overflow_count, 0);

    return rb;
}

/**
 * @brief 销毁环形缓冲区
 */
void ring_buffer_destroy(ring_buffer_t *rb)
{
    if (rb) {
        free(rb->buffer);
        free(rb);
    }
}

/**
 * @brief 检查缓冲区是否为空
 */
static inline bool ring_buffer_is_empty(ring_buffer_t *rb)
{
    return atomic_load_explicit(&rb->head, memory_order_acquire) ==
           atomic_load_explicit(&rb->tail, memory_order_acquire);
}

/**
 * @brief 检查缓冲区是否已满
 */
static inline bool ring_buffer_is_full(ring_buffer_t *rb)
{
    size_t head = atomic_load_explicit(&rb->head, memory_order_acquire);
    size_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire);
    return ((head - tail) & rb->mask) == rb->mask;
}

/**
 * @brief 获取已用空间
 */
static inline size_t ring_buffer_used(ring_buffer_t *rb)
{
    size_t head = atomic_load_explicit(&rb->head, memory_order_acquire);
    size_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire);
    return (head - tail) & rb->mask;
}

/**
 * @brief 获取可用空间
 */
static inline size_t ring_buffer_available(ring_buffer_t *rb)
{
    return rb->capacity - ring_buffer_used(rb) - 1;
}

/**
 * @brief 写入单个数据点
 * @return 成功返回true，失败返回false
 */
bool ring_buffer_push(ring_buffer_t *rb, const data_point_t *point)
{
    size_t head = atomic_load_explicit(&rb->head, memory_order_relaxed);
    size_t next_head = (head + 1) & rb->mask;
    size_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire);

    /* 检查是否已满 */
    if (next_head == tail) {
        atomic_fetch_add_explicit(&rb->overflow_count, 1, memory_order_relaxed);
        return false;
    }

    /* 写入数据 */
    rb->buffer[head] = *point;

    /* 更新head (发布语义) */
    atomic_store_explicit(&rb->head, next_head, memory_order_release);

    return true;
}

/**
 * @brief 批量写入数据点
 * @return 实际写入数量
 */
size_t ring_buffer_push_batch(ring_buffer_t *rb, const data_point_t *points,
                               size_t count)
{
    size_t written = 0;

    for (size_t i = 0; i < count; i++) {
        if (!ring_buffer_push(rb, &points[i])) {
            break;
        }
        written++;
    }

    return written;
}

/**
 * @brief 读取单个数据点
 * @return 成功返回true，失败返回false
 */
bool ring_buffer_pop(ring_buffer_t *rb, data_point_t *point)
{
    size_t tail = atomic_load_explicit(&rb->tail, memory_order_relaxed);
    size_t head = atomic_load_explicit(&rb->head, memory_order_acquire);

    /* 检查是否为空 */
    if (head == tail) {
        return false;
    }

    /* 读取数据 */
    *point = rb->buffer[tail];

    /* 更新tail (发布语义) */
    size_t next_tail = (tail + 1) & rb->mask;
    atomic_store_explicit(&rb->tail, next_tail, memory_order_release);

    return true;
}

/**
 * @brief 批量读取数据点
 * @return 实际读取数量
 */
size_t ring_buffer_pop_batch(ring_buffer_t *rb, data_point_t *points,
                              size_t max_count)
{
    size_t read = 0;

    for (size_t i = 0; i < max_count; i++) {
        if (!ring_buffer_pop(rb, &points[i])) {
            break;
        }
        read++;
    }

    return read;
}

/* 测试配置 */
#define BUFFER_SIZE         (1024 * 1024)   /* 1M数据点 */
#define PRODUCER_COUNT      4
#define TEST_DURATION_SEC   10
#define POINTS_PER_BATCH    32

static ring_buffer_t *g_rb = NULL;
static atomic_size_t g_produced_count = 0;
static atomic_size_t g_consumed_count = 0;
static atomic_size_t g_dropped_count = 0;

/**
 * @brief 获取当前时间 (纳秒)
 */
static inline uint64_t get_time_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

/**
 * @brief 生产者线程
 */
static void *producer_thread(void *arg)
{
    int producer_id = (int)(intptr_t)arg;
    data_point_t batch[POINTS_PER_BATCH];
    uint64_t tag_base = producer_id * 1000;

    printf("[PRODUCER%d] Started\n", producer_id);

    uint64_t start_time = get_time_ns();

    while (true) {
        /* 准备数据 */
        for (int i = 0; i < POINTS_PER_BATCH; i++) {
            batch[i].timestamp_ns = get_time_ns();
            batch[i].tag_id = tag_base + (i % 100);
            batch[i].value = (double)(rand() % 10000) / 100.0;
            batch[i].quality = 0;
        }

        /* 批量写入 */
        size_t written = ring_buffer_push_batch(g_rb, batch, POINTS_PER_BATCH);

        if (written == POINTS_PER_BATCH) {
            atomic_fetch_add(&g_produced_count, written);
        } else {
            atomic_fetch_add(&g_produced_count, written);
            atomic_fetch_add(&g_dropped_count, POINTS_PER_BATCH - written);
        }

        /* 检查超时 */
        if (get_time_ns() - start_time > TEST_DURATION_SEC * 1000000000ULL) {
            break;
        }

        /* 模拟工作负载 */
        // usleep(1);
    }

    printf("[PRODUCER%d] Stopped\n", producer_id);
    return NULL;
}

/**
 * @brief 消费者线程
 */
static void *consumer_thread(void *arg)
{
    (void)arg;
    data_point_t batch[POINTS_PER_BATCH * 4];

    printf("[CONSUMER] Started\n");

    uint64_t start_time = get_time_ns();

    while (true) {
        /* 批量读取 */
        size_t read = ring_buffer_pop_batch(g_rb, batch, POINTS_PER_BATCH * 4);

        if (read > 0) {
            atomic_fetch_add(&g_consumed_count, read);

            /* 模拟处理 */
            for (size_t i = 0; i < read; i++) {
                volatile double processed = batch[i].value * 1.5;
                (void)processed;
            }
        } else {
            /* 缓冲区空，短暂等待 */
            usleep(10);
        }

        /* 检查超时 */
        if (get_time_ns() - start_time > TEST_DURATION_SEC * 1000000000ULL + 1000000000ULL) {
            break;
        }
    }

    /* 消费剩余数据 */
    while (true) {
        size_t read = ring_buffer_pop_batch(g_rb, batch, POINTS_PER_BATCH * 4);
        if (read == 0) break;
        atomic_fetch_add(&g_consumed_count, read);
    }

    printf("[CONSUMER] Stopped\n");
    return NULL;
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║       Lock-Free Ring Buffer Performance Test             ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* 创建环形缓冲区 */
    g_rb = ring_buffer_create(BUFFER_SIZE);
    if (!g_rb) {
        fprintf(stderr, "[ERROR] Failed to create ring buffer\n");
        return 1;
    }

    printf("[INFO] Buffer size: %zu data points (%zu MB)\n",
           BUFFER_SIZE, BUFFER_SIZE * sizeof(data_point_t) / (1024 * 1024));
    printf("[INFO] Producers: %d\n", PRODUCER_COUNT);
    printf("[INFO] Test duration: %d seconds\n\n", TEST_DURATION_SEC);

    /* 锁定内存 */
    mlockall(MCL_CURRENT | MCL_FUTURE);

    /* 创建线程 */
    pthread_t producers[PRODUCER_COUNT];
    pthread_t consumer;

    uint64_t test_start = get_time_ns();

    /* 启动生产者 */
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_create(&producers[i], NULL, producer_thread,
                       (void *)(intptr_t)i);
    }

    /* 启动消费者 */
    pthread_create(&consumer, NULL, consumer_thread, NULL);

    /* 等待测试完成 */
    sleep(TEST_DURATION_SEC + 1);

    /* 等待线程结束 */
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_join(producers[i], NULL);
    }
    pthread_join(consumer, NULL);

    uint64_t test_end = get_time_ns();
    double test_duration = (test_end - test_start) / 1e9;

    /* 获取统计 */
    size_t produced = atomic_load(&g_produced_count);
    size_t consumed = atomic_load(&g_consumed_count);
    size_t dropped = atomic_load(&g_dropped_count);
    size_t overflow = atomic_load(&g_rb->overflow_count);

    /* 打印结果 */
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║                  Test Results                            ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ Test Duration:     %.2f seconds                          ║\n", test_duration);
    printf("║ Points Produced:   %zu                                   ║\n", produced);
    printf("║ Points Consumed:   %zu                                   ║\n", consumed);
    printf("║ Points Dropped:    %zu                                   ║\n", dropped);
    printf("║ Overflow Count:    %zu                                   ║\n", overflow);
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ Throughput:         %.2f M points/second                ║\n",
           produced / test_duration / 1e6);
    printf("║ Throughput:         %.2f MB/second                      ║\n",
           (produced * sizeof(data_point_t)) / test_duration / (1024 * 1024));
    printf("║ Drop Rate:          %.4f%%                              ║\n",
           dropped * 100.0 / produced);
    printf("╚══════════════════════════════════════════════════════════╝\n");

    /* 清理 */
    ring_buffer_destroy(g_rb);

    return 0;
}
