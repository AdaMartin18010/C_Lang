/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 1092
 * Language: c
 * Block ID: a90bce7d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file alignment_optimization.c
 * @brief 数据对齐优化技术
 *
 * 对齐原则：
 * - 基本类型按其自然边界对齐
 * - 结构体按最大成员对齐
 * - 缓存行对齐（64字节）用于并发访问
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <immintrin.h>
#include <time.h>

#define CACHE_LINE 64

/* 未对齐的结构体 - 可能有内部填充 */
typedef struct {
    char a;       /* 1字节 */
    int b;        /* 4字节，可能需要3字节填充 */
    char c;       /* 1字节 */
    double d;     /* 8字节，可能需要7字节填充 */
} BadAlignment;

/* 重新排列的结构体 - 最小化填充 */
typedef struct {
    double d;     /* 8字节 */
    int b;        /* 4字节 */
    char a;       /* 1字节 */
    char c;       /* 1字节 */
    /* 2字节填充到8字节对齐 */
} GoodAlignment;

/* 显式对齐的结构体 */
typedef struct __attribute__((aligned(CACHE_LINE))) {
    char data[56];  /* 填充到64字节 */
} CacheLineAligned;

void print_structure_info(void) {
    printf("=== Structure Alignment Analysis ===\n\n");

    printf("BadAlignment:\n");
    printf("  Size: %zu bytes\n", sizeof(BadAlignment));
    printf("  offsetof(a): %zu\n", __builtin_offsetof(BadAlignment, a));
    printf("  offsetof(b): %zu\n", __builtin_offsetof(BadAlignment, b));
    printf("  offsetof(c): %zu\n", __builtin_offsetof(BadAlignment, c));
    printf("  offsetof(d): %zu\n", __builtin_offsetof(BadAlignment, d));
    printf("  Wasted space: ~%zu bytes\n\n",
           sizeof(BadAlignment) - (1 + 4 + 1 + 8));

    printf("GoodAlignment:\n");
    printf("  Size: %zu bytes\n", sizeof(GoodAlignment));
    printf("  offsetof(d): %zu\n", __builtin_offsetof(GoodAlignment, d));
    printf("  offsetof(b): %zu\n", __builtin_offsetof(GoodAlignment, b));
    printf("  offsetof(a): %zu\n", __builtin_offsetof(GoodAlignment, a));
    printf("  offsetof(c): %zu\n", __builtin_offsetof(GoodAlignment, c));
    printf("  Wasted space: %zu bytes\n\n",
           sizeof(GoodAlignment) - (8 + 4 + 1 + 1));
}

/* 未对齐内存访问的性能影响 */
void misaligned_access_demo(void) {
    printf("\n=== Misaligned Access Performance ===\n\n");

    const int N = 100000000;
    char *raw = malloc(N * sizeof(int) + 64);

    /* 对齐地址 */
    int *aligned = (int*)(((uintptr_t)raw + 63) & ~63);
    /* 非对齐地址（偏移1字节） */
    int *misaligned = (int*)((char*)aligned + 1);

    /* 初始化 */
    for (int i = 0; i < N; i++) {
        aligned[i] = i;
    }
    memcpy(misaligned, aligned, N * sizeof(int));

    /* 对齐访问 */
    volatile long long sum1 = 0;
    clock_t start = clock();
    for (int i = 0; i < N; i++) {
        sum1 += aligned[i];
    }
    clock_t aligned_time = clock() - start;

    /* 非对齐访问 */
    volatile long long sum2 = 0;
    start = clock();
    for (int i = 0; i < N; i++) {
        sum2 += misaligned[i];  /* 跨缓存行边界 */
    }
    clock_t misaligned_time = clock() - start;

    printf("Aligned access:   %ld ms\n", aligned_time * 1000 / CLOCKS_PER_SEC);
    printf("Misaligned access: %ld ms (%.2fx slower)\n",
           misaligned_time * 1000 / CLOCKS_PER_SEC,
           (double)misaligned_time / aligned_time);

    free(raw);
}

/* SIMD对齐要求演示 */
void simd_alignment_demo(void) {
    printf("\n=== SIMD Alignment Requirements ===\n\n");

    const int N = 10000000;
    float *unaligned = malloc(N * sizeof(float) + 64);
    float *aligned = (float*)(((uintptr_t)unaligned + 31) & ~31);

    for (int i = 0; i < N; i++) {
        aligned[i] = (float)i;
    }

    /* 未对齐的SIMD加载 */
    __m256 sum_vec = _mm256_setzero_ps();
    clock_t start = clock();

    for (int i = 0; i < N; i += 8) {
        /* _mm256_loadu_ps: 非对齐加载，可能跨缓存行 */
        __m256 vec = _mm256_loadu_ps(&aligned[i]);
        sum_vec = _mm256_add_ps(sum_vec, vec);
    }

    clock_t unaligned_simd_time = clock() - start;

    /* 对齐的SIMD加载 */
    sum_vec = _mm256_setzero_ps();
    start = clock();

    for (int i = 0; i < N; i += 8) {
        /* _mm256_load_ps: 对齐加载，要求32字节对齐 */
        __m256 vec = _mm256_load_ps(&aligned[i]);
        sum_vec = _mm256_add_ps(sum_vec, vec);
    }

    clock_t aligned_simd_time = clock() - start;

    printf("Unaligned SIMD (_mm256_loadu_ps): %ld ms\n",
           unaligned_simd_time * 1000 / CLOCKS_PER_SEC);
    printf("Aligned SIMD (_mm256_load_ps):    %ld ms (%.2fx faster)\n",
           aligned_simd_time * 1000 / CLOCKS_PER_SEC,
           (double)unaligned_simd_time / aligned_simd_time);

    /* 水平求和 */
    float result[8];
    _mm256_storeu_ps(result, sum_vec);
    volatile float total = 0;
    for (int i = 0; i < 8; i++) total += result[i];
    (void)total;

    free(unaligned);
}

/* 自定义对齐分配器 */
void* aligned_malloc(size_t size, size_t alignment) {
    void *ptr = NULL;
    #ifdef _WIN32
        ptr = _aligned_malloc(size, alignment);
    #else
        posix_memalign(&ptr, alignment, size);
    #endif
    return ptr;
}

void aligned_free(void *ptr) {
    #ifdef _WIN32
        _aligned_free(ptr);
    #else
        free(ptr);
    #endif
}

int main(void) {
    print_structure_info();
    misaligned_access_demo();
    simd_alignment_demo();
    return 0;
}
