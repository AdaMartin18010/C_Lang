/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\README.md
 * Line: 775
 * Language: c
 * Block ID: 99e9f3f8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 高精度时间测量
 */

#include <time.h>
#include <x86intrin.h>

/* 使用RDTSC指令 - 纳秒级精度 */
static inline uint64_t rdtsc(void)
{
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

/* 使用RDTSCP - 带序列化和处理器ID */
static inline uint64_t rdtscp(uint32_t *aux)
{
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtscp" : "=a" (lo), "=d" (hi), "=c" (*aux));
    return ((uint64_t)hi << 32) | lo;
}

/* TSC到纳秒转换 */
struct tsc_calibrator {
    uint64_t tsc_hz;        /* TSC频率 */
    double ns_per_tsc;      /* 每个TSC周期对应的纳秒数 */
};

void calibrate_tsc(struct tsc_calibrator *cal)
{
    struct timespec ts_start, ts_end;
    uint64_t tsc_start, tsc_end;

    /* 使用1秒间隔校准 */
    clock_gettime(CLOCK_MONOTONIC, &ts_start);
    tsc_start = rdtsc();

    do {
        clock_gettime(CLOCK_MONOTONIC, &ts_end);
    } while ((ts_end.tv_sec - ts_start.tv_sec) * 1000000000UL +
             (ts_end.tv_nsec - ts_start.tv_nsec) < 1000000000UL);

    tsc_end = rdtsc();

    cal->tsc_hz = tsc_end - tsc_start;
    cal->ns_per_tsc = 1000000000.0 / cal->tsc_hz;
}

static inline uint64_t tsc_to_ns(uint64_t tsc, struct tsc_calibrator *cal)
{
    return (uint64_t)(tsc * cal->ns_per_tsc);
}

/* 延迟直方图 */
struct latency_histogram {
    atomic_uint_fast64_t buckets[32];  /* 指数分桶 */
    atomic_uint_fast64_t total;
    atomic_uint_fast64_t count;
};

void record_latency(struct latency_histogram *hist, uint64_t ns)
{
    int bucket = 0;
    uint64_t threshold = 100;  /* 100ns起始 */

    while (bucket < 31 && ns > threshold) {
        threshold <<= 1;
        bucket++;
    }

    atomic_fetch_add(&hist->buckets[bucket], 1);
    atomic_fetch_add(&hist->total, ns);
    atomic_fetch_add(&hist->count, 1);
}
