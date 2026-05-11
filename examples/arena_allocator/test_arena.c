/*
 * Arena Allocator 测试与演示
 * 编译: gcc -Wall -Wextra -O2 -std=c11 -o test_arena arena.c test_arena.c
 */
#include "arena.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

typedef struct {
    int x, y;
    const char *name;
} Point;

void test_basic(void) {
    printf("=== Test: Basic Allocation ===\n");
    Arena *a = arena_create(1024);
    assert(a);
    
    int *p1 = arena_new(a, int);
    *p1 = 42;
    
    Point *pt = arena_new(a, Point);
    pt->x = 10; pt->y = 20; pt->name = "origin";
    
    int *arr = arena_new_n(a, int, 100);
    for (int i = 0; i < 100; i++) arr[i] = i;
    
    printf("p1 = %d, pt = (%d,%d), arr[50] = %d\n", *p1, pt->x, pt->y, arr[50]);
    printf("Total allocated: %zu bytes\n", arena_total_allocated(a));
    printf("Block count: %zu\n", arena_block_count(a));
    
    arena_destroy(a);
    printf("PASS\n\n");
}

void test_reset(void) {
    printf("=== Test: Arena Reset ===\n");
    Arena *a = arena_create(256);
    
    for (int i = 0; i < 1000; i++) {
        int *p = arena_new(a, int);
        *p = i;
    }
    printf("Before reset: %zu bytes, %zu blocks\n",
           arena_total_allocated(a), arena_block_count(a));
    
    arena_reset(a);
    
    /* 复用同一块内存 */
    int *p = arena_new(a, int);
    *p = 9999;
    printf("After reset: %zu bytes, %zu blocks\n",
           arena_total_allocated(a), arena_block_count(a));
    assert(*p == 9999);
    
    arena_destroy(a);
    printf("PASS\n\n");
}

void test_large_alloc(void) {
    printf("=== Test: Large Allocation ===\n");
    Arena *a = arena_create(1024);
    
    /* 分配超过默认块大小的对象 */
    uint8_t *big = arena_alloc(a, 5000);
    memset(big, 0xAB, 5000);
    
    printf("Large alloc: %zu bytes, %zu blocks\n",
           arena_total_allocated(a), arena_block_count(a));
    assert(big[4999] == 0xAB);
    
    arena_destroy(a);
    printf("PASS\n\n");
}

void benchmark(void) {
    printf("=== Benchmark: Arena vs malloc ===\n");
    const int N = 100000;
    const int SIZE = 64;
    
    /* Arena */
    Arena *a = arena_create(64 * 1024);
    clock_t t0 = clock();
    for (int i = 0; i < N; i++) {
        void *p = arena_alloc(a, SIZE);
        (void)p;
    }
    clock_t t1 = clock();
    
    /* malloc */
    void **ptrs = malloc(N * sizeof(void *));
    clock_t t2 = clock();
    for (int i = 0; i < N; i++) {
        ptrs[i] = malloc(SIZE);
    }
    clock_t t3 = clock();
    for (int i = 0; i < N; i++) free(ptrs[i]);
    free(ptrs);
    
    double arena_ms = (double)(t1 - t0) * 1000.0 / CLOCKS_PER_SEC;
    double malloc_ms = (double)(t3 - t2) * 1000.0 / CLOCKS_PER_SEC;
    
    printf("Arena:  %.3f ms (%zu allocations, %zu blocks)\n",
           arena_ms, (size_t)N, arena_block_count(a));
    printf("malloc: %.3f ms (%d allocations, %d frees)\n",
           malloc_ms, N, N);
    printf("Speedup: %.1fx\n", malloc_ms / arena_ms);
    
    arena_destroy(a);
}

int main(void) {
    test_basic();
    test_reset();
    test_large_alloc();
    benchmark();
    return 0;
}
