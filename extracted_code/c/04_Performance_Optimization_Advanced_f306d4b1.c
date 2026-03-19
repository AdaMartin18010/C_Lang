/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 598
 * Language: c
 * Block ID: f306d4b1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file cache_hierarchy.c
 * @brief CPU缓存层次结构分析与优化
 *
 * 典型x86-64缓存结构：
 * L1 Data:    32KB, 8-way, 64B line, 4 cycle latency
 * L1 Inst:    32KB, 8-way, 64B line
 * L2 Unified: 256KB-512KB, 4-way, 12 cycle latency
 * L3 Unified: 8MB-32MB, 16-way, 40 cycle latency
 * DRAM:       GBs, 200+ cycle latency
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <x86intrin.h>

/* 缓存行大小 */
#define CACHE_LINE_SIZE 64

/* 各缓存层级大小（典型值，实际需通过CPUID检测） */
#define L1D_SIZE (32 * 1024)      /* 32KB L1数据缓存 */
#define L2_SIZE (256 * 1024)      /* 256KB L2缓存 */
#define L3_SIZE (8 * 1024 * 1024) /* 8MB L3缓存 */

/* 驱逐缓存行的辅助函数 */
static inline void flush_cache_line(void *p) {
    _mm_clflush(p);
}

/* 内存屏障 */
static inline void mfence(void) {
    _mm_mfence();
}

/* 获取时间戳 */
static inline uint64_t rdtscp(uint32_t *aux) {
    return __rdtscp(aux);
}

/**
 * @brief 缓存延迟测试
 *
 * 使用指针追踪（pointer chasing）测量各级缓存延迟
 */
void cache_latency_test(void) {
    printf("=== Cache Latency Test ===\n\n");

    const size_t sizes[] = {
        4 * 1024,           /* 4KB - L1 */
        32 * 1024,          /* 32KB - L1边界 */
        64 * 1024,          /* 64KB - L2 */
        256 * 1024,         /* 256KB - L2边界 */
        512 * 1024,         /* 512KB - L2/L3 */
        4 * 1024 * 1024,    /* 4MB - L3 */
        16 * 1024 * 1024,   /* 16MB - L3边界 */
        64 * 1024 * 1024    /* 64MB - 内存 */
    };
    const char *labels[] = {
        "4KB (L1)", "32KB (L1)", "64KB (L2)",
        "256KB (L2)", "512KB (L2/3)", "4MB (L3)",
        "16MB (L3)", "64MB (DRAM)"
    };

    for (size_t t = 0; t < sizeof(sizes)/sizeof(sizes[0]); t++) {
        size_t size = sizes[t];
        int num_elements = size / sizeof(void*);

        /* 分配对齐内存 */
        void **buffer = aligned_alloc(CACHE_LINE_SIZE, size);
        if (!buffer) continue;

        /* 创建随机访问模式（避免预取） */
        srand(42);
        int *indices = malloc(num_elements * sizeof(int));
        for (int i = 0; i < num_elements; i++) {
            indices[i] = i;
        }
        /* Fisher-Yates shuffle */
        for (int i = num_elements - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            int tmp = indices[i];
            indices[i] = indices[j];
            indices[j] = tmp;
        }

        /* 构建指针链 */
        for (int i = 0; i < num_elements; i++) {
            buffer[indices[i]] = &buffer[indices[(i + 1) % num_elements]];
        }

        /* 预热 */
        volatile void **p = buffer;
        for (int i = 0; i < num_elements; i++) {
            p = *(void **)p;
        }

        /* 测量 */
        p = buffer;
        uint32_t aux;
        uint64_t start = rdtscp(&aux);

        for (int i = 0; i < num_elements; i++) {
            p = *(void **)p;  /* 内存加载依赖 */
        }

        uint64_t end = rdtscp(&aux);
        double cycles_per_access = (double)(end - start) / num_elements;

        printf("%s: %.2f cycles/access\n", labels[t], cycles_per_access);

        free(indices);
        free(buffer);
    }
}

/**
 * @brief 缓存带宽测试
 *
 * 顺序访问可最大化带宽，利用预取和宽向量加载
 */
void cache_bandwidth_test(void) {
    printf("\n=== Cache Bandwidth Test ===\n\n");

    const int N = 100 * 1024 * 1024;  /* 100MB */
    char *src = aligned_alloc(CACHE_LINE_SIZE, N);
    char *dst = aligned_alloc(CACHE_LINE_SIZE, N);

    if (!src || !dst) {
        printf("Memory allocation failed\n");
        return;
    }

    memset(src, 'A', N);

    /* 测试1: memcpy（可能使用rep movsb或SIMD） */
    printf("Testing memcpy bandwidth...\n");
    clock_t start = clock();
    memcpy(dst, src, N);
    clock_t end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    double bandwidth_mb = (N / (1024.0 * 1024.0)) / seconds;
    printf("  memcpy: %.2f MB/s\n", bandwidth_mb);

    /* 测试2: 简单字节拷贝 */
    printf("Testing byte-by-byte copy...\n");
    start = clock();
    for (int i = 0; i < N; i++) {
        dst[i] = src[i];
    }
    end = clock();
    seconds = (double)(end - start) / CLOCKS_PER_SEC;
    bandwidth_mb = (N / (1024.0 * 1024.0)) / seconds;
    printf("  Byte copy: %.2f MB/s\n", bandwidth_mb);

    /* 测试3: 8字节拷贝 */
    printf("Testing 8-byte copy...\n");
    uint64_t *src64 = (uint64_t*)src;
    uint64_t *dst64 = (uint64_t*)dst;
    int n64 = N / 8;
    start = clock();
    for (int i = 0; i < n64; i++) {
        dst64[i] = src64[i];
    }
    end = clock();
    seconds = (double)(end - start) / CLOCKS_PER_SEC;
    bandwidth_mb = (N / (1024.0 * 1024.0)) / seconds;
    printf("  8-byte copy: %.2f MB/s\n", bandwidth_mb);

    free(src);
    free(dst);
}

/**
 * @brief 缓存行伪共享（False Sharing）演示
 *
 * 当不同CPU核心修改同一缓存行内的不同变量时，
 * 会导致缓存行在核心间来回传递，严重降低性能
 */

#include <pthread.h>

/* 未填充结构体 - 容易发生伪共享 */
typedef struct {
    volatile long long counter;
} UnpaddedCounter;

/* 填充到缓存行大小 - 避免伪共享 */
typedef struct {
    volatile long long counter;
    char padding[CACHE_LINE_SIZE - sizeof(long long)];
} PaddedCounter;

#define NUM_THREADS 4
#define ITERATIONS 10000000

void *unpadded_worker(void *arg) {
    UnpaddedCounter *counters = (UnpaddedCounter*)arg;
    int id = (int)(size_t)pthread_self() % NUM_THREADS;  /* 简化 */

    for (int i = 0; i < ITERATIONS; i++) {
        counters[id].counter++;
    }
    return NULL;
}

void *padded_worker(void *arg) {
    PaddedCounter *counters = (PaddedCounter*)arg;
    int id = (int)(size_t)pthread_self() % NUM_THREADS;

    for (int i = 0; i < ITERATIONS; i++) {
        counters[id].counter++;
    }
    return NULL;
}

void false_sharing_demo(void) {
    printf("\n=== False Sharing Demo ===\n\n");

    pthread_t threads[NUM_THREADS];

    /* 未填充版本 */
    printf("Unpadded counters:\n");
    UnpaddedCounter *unpadded = calloc(NUM_THREADS, sizeof(UnpaddedCounter));
    clock_t start = clock();

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, unpadded_worker, unpadded);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("  Time: %ld ms\n", (clock() - start) * 1000 / CLOCKS_PER_SEC);
    free(unpadded);

    /* 填充版本 */
    printf("Padded counters:\n");
    PaddedCounter *padded = calloc(NUM_THREADS, sizeof(PaddedCounter));
    start = clock();

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, padded_worker, padded);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("  Time: %ld ms\n", (clock() - start) * 1000 / CLOCKS_PER_SEC);
    free(padded);
}

int main(void) {
    cache_latency_test();
    cache_bandwidth_test();
    false_sharing_demo();
    return 0;
}
