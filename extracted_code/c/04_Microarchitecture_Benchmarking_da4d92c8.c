/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\04_Microarchitecture_Benchmarking.md
 * Line: 1228
 * Language: c
 * Block ID: da4d92c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* memory_bandwidth_test.c - 内存带宽测试 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <emmintrin.h>  /* SSE2 */
#include <immintrin.h>  /* AVX */

/* RDTSC */
static inline uint64_t rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

static inline uint64_t rdtscp(uint32_t *aux) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtscp" : "=a" (lo), "=d" (hi), "=c" (*aux));
    return ((uint64_t)hi << 32) | lo;
}

#define TEST_SIZE (1024 * 1024 * 256)  /* 256MB */
#define ITERATIONS 10

/* 标量读取带宽测试 */
double test_scalar_read(char *src, size_t size) {
    volatile uint64_t sum = 0;
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int iter = 0; iter < ITERATIONS; iter++) {
        uint64_t local_sum = 0;
        for (size_t i = 0; i < size; i += 8) {
            local_sum += *(uint64_t*)(src + i);
        }
        sum += local_sum;
    }
    uint64_t end = rdtscp(&aux);
    (void)sum;

    double seconds = (end - start) / 3.0e9;
    double gb_per_sec = (double)(size * ITERATIONS) / (1024 * 1024 * 1024) / seconds;
    return gb_per_sec;
}

/* 标量写入带宽测试 */
double test_scalar_write(char *dst, size_t size) {
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (size_t i = 0; i < size; i += 8) {
            *(uint64_t*)(dst + i) = iter;
        }
    }
    uint64_t end = rdtscp(&aux);

    double seconds = (end - start) / 3.0e9;
    double gb_per_sec = (double)(size * ITERATIONS) / (1024 * 1024 * 1024) / seconds;
    return gb_per_sec;
}

/* 标量复制带宽测试 */
double test_scalar_copy(char *dst, char *src, size_t size) {
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int iter = 0; iter < ITERATIONS; iter++) {
        memcpy(dst, src, size);
    }
    uint64_t end = rdtscp(&aux);

    double seconds = (end - start) / 3.0e9;
    /* 复制涉及读取+写入，所以数据量要×2 */
    double gb_per_sec = (double)(size * 2 * ITERATIONS) / (1024 * 1024 * 1024) / seconds;
    return gb_per_sec;
}

/* SSE读取带宽测试 */
double test_sse_read(char *src, size_t size) {
    volatile __m128i sum = _mm_setzero_si128();
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (size_t i = 0; i < size; i += 64) {
            __m128i r0 = _mm_load_si128((__m128i*)(src + i));
            __m128i r1 = _mm_load_si128((__m128i*)(src + i + 16));
            __m128i r2 = _mm_load_si128((__m128i*)(src + i + 32));
            __m128i r3 = _mm_load_si128((__m128i*)(src + i + 48));
            sum = _mm_add_epi64(sum, r0);
            sum = _mm_add_epi64(sum, r1);
            sum = _mm_add_epi64(sum, r2);
            sum = _mm_add_epi64(sum, r3);
        }
    }
    uint64_t end = rdtscp(&aux);
    (void)sum;

    double seconds = (end - start) / 3.0e9;
    double gb_per_sec = (double)(size * ITERATIONS) / (1024 * 1024 * 1024) / seconds;
    return gb_per_sec;
}

/* AVX读取带宽测试 */
double test_avx_read(char *src, size_t size) {
    volatile __m256i sum = _mm256_setzero_si256();
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (size_t i = 0; i < size; i += 64) {
            __m256i r0 = _mm256_load_si256((__m256i*)(src + i));
            __m256i r1 = _mm256_load_si256((__m256i*)(src + i + 32));
            sum = _mm256_add_epi64(sum, r0);
            sum = _mm256_add_epi64(sum, r1);
        }
    }
    uint64_t end = rdtscp(&aux);
    (void)sum;

    double seconds = (end - start) / 3.0e9;
    double gb_per_sec = (double)(size * ITERATIONS) / (1024 * 1024 * 1024) / seconds;
    return gb_per_sec;
}

/* 内存填充测试 (memset) */
double test_memset(char *dst, size_t size) {
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int iter = 0; iter < ITERATIONS; iter++) {
        memset(dst, iter & 0xFF, size);
    }
    uint64_t end = rdtscp(&aux);

    double seconds = (end - start) / 3.0e9;
    double gb_per_sec = (double)(size * ITERATIONS) / (1024 * 1024 * 1024) / seconds;
    return gb_per_sec;
}

/* 随机访问带宽测试 */
double test_random_read(char *src, size_t *indices, size_t num_indices) {
    volatile uint64_t sum = 0;
    uint32_t aux;

    uint64_t start = rdtscp(&aux);
    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (size_t i = 0; i < num_indices; i++) {
            sum += *(uint64_t*)(src + indices[i]);
        }
    }
    uint64_t end = rdtscp(&aux);
    (void)sum;

    double seconds = (end - start) / 3.0e9;
    double total_bytes = num_indices * sizeof(uint64_t) * ITERATIONS;
    double gb_per_sec = total_bytes / (1024 * 1024 * 1024) / seconds;
    return gb_per_sec;
}

int main() {
    printf("========================================\n");
    printf("      内存带宽测试                       \n");
    printf("========================================\n\n");
    printf("测试数据大小: %d MB\n\n", TEST_SIZE / (1024 * 1024));

    /* 分配对齐内存 */
    char *src = aligned_alloc(64, TEST_SIZE);
    char *dst = aligned_alloc(64, TEST_SIZE);

    /* 初始化 */
    memset(src, 0xAA, TEST_SIZE);

    /* 准备随机访问索引 */
    size_t num_indices = TEST_SIZE / 64;
    size_t *indices = malloc(num_indices * sizeof(size_t));
    for (size_t i = 0; i < num_indices; i++) {
        indices[i] = (rand() % num_indices) * 64;
    }

    printf("%-30s %15s\n", "测试项目", "带宽 (GB/s)");
    printf("--------------------------------------------------------------------------------\n");

    /* 顺序访问测试 */
    printf("\n【顺序访问测试】\n");
    printf("%-30s %15.2f\n", "标量读取", test_scalar_read(src, TEST_SIZE));
    printf("%-30s %15.2f\n", "标量写入", test_scalar_write(dst, TEST_SIZE));
    printf("%-30s %15.2f\n", "标量复制 (memcpy)", test_scalar_copy(dst, src, TEST_SIZE));
    printf("%-30s %15.2f\n", "SSE 128-bit读取", test_sse_read(src, TEST_SIZE));
    printf("%-30s %15.2f\n", "AVX 256-bit读取", test_avx_read(src, TEST_SIZE));
    printf("%-30s %15.2f\n", "memset", test_memset(dst, TEST_SIZE));

    /* 随机访问测试 */
    printf("\n【随机访问测试 (缓存行级别)】\n");
    printf("%-30s %15.2f\n", "随机读取 (64B步幅)", test_random_read(src, indices, num_indices));

    printf("\n========================================\n");
    printf("参考值:\n");
    printf("  DDR4-3200双通道理论带宽: ~50 GB/s\n");
    printf("  实际顺序读取带宽: 40-45 GB/s (80-90%%效率)\n");
    printf("  随机读取带宽: 通常 <5 GB/s (受延迟限制)\n");
    printf("========================================\n");

    free(src);
    free(dst);
    free(indices);

    return 0;
}
