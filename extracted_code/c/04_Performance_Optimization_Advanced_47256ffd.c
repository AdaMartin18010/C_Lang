/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 4565
 * Language: c
 * Block ID: 47256ffd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file hft_optimized.c
 * @brief 高频交易系统性能优化完整案例
 *
 * 高频交易（HFT）性能要求：
 * - 延迟：微秒级（<10μs）
 * - 吞吐量：每秒百万订单
 * - 抖动：极低（99.9% < 5μs）
 *
 * 优化策略：
 * 1. CPU亲和性绑定
 * 2. 禁用超线程/SMT
 * 3. 禁用CPU频率调节
 * 4. 大页内存
 * 5. 内核旁路网络（DPDK/RDMA）
 * 6. 无锁数据结构
 * 7. 预计算/查表
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <x86intrin.h>

/* 高精度计时器（RDTSC） */
static inline uint64_t rdtsc(void) {
    return __rdtsc();
}

static inline uint64_t rdtscp(uint32_t *aux) {
    return __rdtscp(aux);
}

/* CPU频率校准 */
double calibrate_tsc_frequency(void) {
    uint64_t tsc_start = rdtsc();
    clock_t clock_start = clock();

    /* 休眠100ms */
    struct timespec ts = {0, 100000000};
    nanosleep(&ts, NULL);

    uint64_t tsc_end = rdtsc();
    clock_t clock_end = clock();

    double clock_seconds = (clock_end - clock_start) / (double)CLOCKS_PER_SEC;
    uint64_t tsc_elapsed = tsc_end - tsc_start;

    return tsc_elapsed / clock_seconds;  /* TSC频率（cycles/second） */
}

/* 微秒级延迟测量 */
double tsc_to_us(uint64_t tsc_delta, double tsc_freq) {
    return (tsc_delta / tsc_freq) * 1000000.0;
}

/**
 * @brief 订单簿数据结构（高度优化）
 */
#define MAX_PRICE_LEVELS 1000
#define MAX_ORDERS_PER_LEVEL 1000

typedef struct Order {
    uint64_t order_id;
    uint32_t quantity;
    uint32_t price;
    uint8_t side;  /* 0=bid, 1=ask */
} __attribute__((packed)) Order;

typedef struct PriceLevel {
    uint32_t price;
    uint32_t total_quantity;
    uint32_t order_count;
    Order orders[MAX_ORDERS_PER_LEVEL];
    pthread_spinlock_t lock;  /* 每个价格级别独立锁 */
} __attribute__((aligned(64))) PriceLevel;  /* 缓存行对齐 */

typedef struct {
    PriceLevel bids[MAX_PRICE_LEVELS];  /* 买单 */
    PriceLevel asks[MAX_PRICE_LEVELS];  /* 卖单 */
    uint32_t best_bid;
    uint32_t best_ask;
    _Atomic(uint64_t) sequence;  /* 序列号 */
} OrderBook;

/* 初始化订单簿 */
void orderbook_init(OrderBook *book) {
    memset(book, 0, sizeof(OrderBook));
    for (int i = 0; i < MAX_PRICE_LEVELS; i++) {
        pthread_spin_init(&book->bids[i].lock, PTHREAD_PROCESS_PRIVATE);
        pthread_spin_init(&book->asks[i].lock, PTHREAD_PROCESS_PRIVATE);
    }
    book->best_bid = 0;
    book->best_ask = UINT32_MAX;
}

/* 添加订单（优化版本） */
static inline uint64_t add_order_fast(OrderBook *book, uint32_t price,
                                       uint32_t qty, uint8_t side) {
    uint64_t seq = atomic_fetch_add(&book->sequence, 1);

    PriceLevel *level = (side == 0) ? &book->bids[price] : &book->asks[price];

    /* 快速路径：无竞争时无锁添加 */
    if (level->order_count < MAX_ORDERS_PER_LEVEL - 1) {
        uint32_t idx = level->order_count;
        level->orders[idx].order_id = seq;
        level->orders[idx].quantity = qty;
        level->orders[idx].price = price;
        level->orders[idx].side = side;

        /* 内存屏障确保数据可见 */
        __atomic_thread_fence(__ATOMIC_RELEASE);
        level->order_count++;
        level->total_quantity += qty;
    }

    /* 更新最优价格 */
    if (side == 0 && price > book->best_bid) {
        book->best_bid = price;
    } else if (side == 1 && price < book->best_ask) {
        book->best_ask = price;
    }

    return seq;
}

/**
 * @brief 网络优化 - 低延迟socket设置
 */
int create_low_latency_socket(void) {
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) return -1;

    /* TCP_NODELAY - 禁用Nagle算法 */
    int nodelay = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

    /* TCP_QUICKACK - 立即发送ACK */
    int quickack = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_QUICKACK, &quickack, sizeof(quickack));

    /* SO_BUSY_POLL - 忙轮询接收（减少中断延迟） */
    int busy_poll = 50;  /* 50μs忙轮询 */
    setsockopt(sock, SOL_SOCKET, SO_BUSY_POLL, &busy_poll, sizeof(busy_poll));

    /* 设置高优先级 */
    int priority = 6;  /* DSCP EF (Expedited Forwarding) */
    setsockopt(sock, IPPROTO_IP, IP_TOS, &priority, sizeof(priority));

    return sock;
}

/**
 * @brief 内存预取优化
 */
void prefetch_orderbook(OrderBook *book) {
    /* 预取常用价格级别到L1 */
    for (int i = 0; i < 10; i++) {
        _mm_prefetch((const char*)&book->bids[i], _MM_HINT_T0);
        _mm_prefetch((const char*)&book->asks[i], _MM_HINT_T0);
    }
}

/**
 * @brief 延迟测试
 */
void latency_benchmark(void) {
    printf("=== HFT System Latency Benchmark ===\n\n");

    /* 校准TSC */
    double tsc_freq = calibrate_tsc_frequency();
    printf("TSC Frequency: %.2f GHz\n\n", tsc_freq / 1e9);

    OrderBook book;
    orderbook_init(&book);

    /* 预热 */
    for (int i = 0; i < 1000; i++) {
        add_order_fast(&book, 100 + (i % 100), 100, i % 2);
    }

    /* 延迟测试 */
    const int N = 1000000;
    uint64_t *latencies = malloc(N * sizeof(uint64_t));

    printf("Measuring order insertion latency...\n");

    for (int i = 0; i < N; i++) {
        uint64_t start = rdtsc();
        add_order_fast(&book, 100 + (i % 100), 100, i % 2);
        uint64_t end = rdtsc();
        latencies[i] = end - start;
    }

    /* 统计分析 */
    uint64_t min_lat = latencies[0];
    uint64_t max_lat = latencies[0];
    uint64_t sum_lat = 0;

    for (int i = 0; i < N; i++) {
        if (latencies[i] < min_lat) min_lat = latencies[i];
        if (latencies[i] > max_lat) max_lat = latencies[i];
        sum_lat += latencies[i];
    }

    /* 计算百分位数 */
    qsort(latencies, N, sizeof(uint64_t),
          (int(*)(const void*,const void*))memcmp);

    uint64_t p50 = latencies[N / 2];
    uint64_t p99 = latencies[(int)(N * 0.99)];
    uint64_t p999 = latencies[(int)(N * 0.999)];

    printf("\nLatency Statistics:\n");
    printf("  Min:     %.2f ns\n", tsc_to_us(min_lat, tsc_freq) * 1000);
    printf("  Max:     %.2f ns\n", tsc_to_us(max_lat, tsc_freq) * 1000);
    printf("  Mean:    %.2f ns\n", tsc_to_us(sum_lat / N, tsc_freq) * 1000);
    printf("  P50:     %.2f ns\n", tsc_to_us(p50, tsc_freq) * 1000);
    printf("  P99:     %.2f ns\n", tsc_to_us(p99, tsc_freq) * 1000);
    printf("  P99.9:   %.2f ns\n", tsc_to_us(p999, tsc_freq) * 1000);

    printf("\nPerformance:\n");
    printf("  Throughput: %.2f M orders/sec\n", N / (sum_lat / tsc_freq) / 1e6);

    free(latencies);
}

/**
 * @brief 系统调优建议
 */
void system_tuning_guide(void) {
    printf("\n=== HFT System Tuning Guide ===\n\n");

    printf("1. CPU隔离:\n");
    printf("   isolcpus=2,3 nohz_full=2,3 rcu_nocbs=2,3\n");
    printf("   (隔离CPU 2,3，禁用tick，RCU offload)\n\n");

    printf("2. 禁用超线程:\n");
    printf("   /sys/devices/system/cpu/smt/control = off\n\n");

    printf("3. CPU频率锁定:\n");
    printf("   cpupower frequency-set -g performance\n\n");

    printf("4. 禁用交换:\n");
    printf("   swapoff -a\n\n");
    printf("   vm.swappiness = 0\n\n");

    printf("5. 网络调优:\n");
    printf("   net.core.busy_poll = 50\n");
    printf("   net.core.busy_budget = 200\n");
    printf("   net.ipv4.tcp_low_latency = 1\n\n");

    printf("6. 中断亲和性:\n");
    printf("   /proc/irq/<num>/smp_affinity = <isolated_cpu_mask>\n\n");

    printf("7. 大页内存:\n");
    printf("   vm.nr_hugepages = 1024\n\n");
}

int main(void) {
    latency_benchmark();
    system_tuning_guide();
    return 0;
}
