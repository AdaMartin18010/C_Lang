/*
 * Auto-generated from: 07_Modern_Toolchain\13_Comprehensive_Examples\README.md
 * Line: 630
 * Language: c
 * Block ID: 6dc31163
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* benchmark.c - 性能测试 */
#include "mempool.h"
#include <stdio.h>
#include <time.h>

#define BLOCK_SIZE 64
#define BLOCK_COUNT 100000
#define ITERATIONS 1000000

static double get_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main(void) {
    void **ptrs = calloc(BLOCK_COUNT, sizeof(void *));

    printf("=== Memory Pool vs malloc Benchmark ===\n");
    printf("Block Size: %d, Count: %d, Iterations: %d\n\n",
           BLOCK_SIZE, BLOCK_COUNT, ITERATIONS);

    /* 测试malloc/free */
    double start = get_time();
    for (int iter = 0; iter < ITERATIONS / BLOCK_COUNT; iter++) {
        for (int i = 0; i < BLOCK_COUNT; i++) {
            ptrs[i] = malloc(BLOCK_SIZE);
        }
        for (int i = 0; i < BLOCK_COUNT; i++) {
            free(ptrs[i]);
        }
    }
    double malloc_time = get_time() - start;
    printf("malloc/free:      %.3f seconds\n", malloc_time);

    /* 测试内存池 */
    mempool_t *pool = mempool_create(BLOCK_SIZE, BLOCK_COUNT,
                                      ALLOC_STRATEGY_LIFO);

    start = get_time();
    for (int iter = 0; iter < ITERATIONS / BLOCK_COUNT; iter++) {
        for (int i = 0; i < BLOCK_COUNT; i++) {
            ptrs[i] = mempool_alloc(pool);
        }
        for (int i = 0; i < BLOCK_COUNT; i++) {
            mempool_free(pool, ptrs[i]);
        }
    }
    double pool_time = get_time() - start;
    printf("mempool:          %.3f seconds\n", pool_time);

    printf("\nSpeedup: %.1fx\n", malloc_time / pool_time);

    mempool_print_stats(pool);
    mempool_destroy(pool);
    free(ptrs);

    return 0;
}
