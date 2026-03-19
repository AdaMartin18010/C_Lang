/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 2594
 * Language: c
 * Block ID: 2e8b8e21
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file memory_pool.c
 * @brief 高性能内存池实现
 *
 * 内存池优势：
 * - 减少malloc/free系统调用开销
 * - 减少内存碎片
 * - 提高缓存局部性
 * - 避免锁竞争（线程本地池）
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdalign.h>
#include <time.h>
#include <pthread.h>

#define CACHE_LINE_SIZE 64
#define POOL_BLOCK_SIZE 64              /* 每个块的大小 */
#define POOL_BLOCKS_PER_CHUNK 1024      /* 每批分配的块数 */

/**
 * @brief 简单内存池
 */
typedef struct PoolBlock {
    struct PoolBlock *next;
} PoolBlock;

typedef struct {
    PoolBlock *free_list;               /* 空闲块链表 */
    char *chunk_base;                   /* 当前chunk基址 */
    size_t chunk_offset;                /* 当前chunk偏移 */
    size_t chunk_size;                  /* chunk大小 */
    pthread_mutex_t lock;               /* 线程安全锁 */
} MemoryPool;

/* 初始化内存池 */
void pool_init(MemoryPool *pool) {
    pool->free_list = NULL;
    pool->chunk_base = NULL;
    pool->chunk_offset = 0;
    pool->chunk_size = POOL_BLOCK_SIZE * POOL_BLOCKS_PER_CHUNK;
    pthread_mutex_init(&pool->lock, NULL);
}

/* 分配新chunk */
static int pool_alloc_chunk(MemoryPool *pool) {
    pool->chunk_base = aligned_alloc(CACHE_LINE_SIZE, pool->chunk_size);
    if (!pool->chunk_base) return -1;

    pool->chunk_offset = 0;

    /* 将新chunk分割成块，加入空闲链表 */
    for (int i = 0; i < POOL_BLOCKS_PER_CHUNK; i++) {
        PoolBlock *block = (PoolBlock*)(pool->chunk_base + i * POOL_BLOCK_SIZE);
        block->next = pool->free_list;
        pool->free_list = block;
    }

    return 0;
}

/* 从池分配 */
void* pool_alloc(MemoryPool *pool) {
    pthread_mutex_lock(&pool->lock);

    if (!pool->free_list) {
        if (pool_alloc_chunk(pool) < 0) {
            pthread_mutex_unlock(&pool->lock);
            return NULL;
        }
    }

    PoolBlock *block = pool->free_list;
    pool->free_list = block->next;

    pthread_mutex_unlock(&pool->lock);

    /* 清空块（调试用） */
    memset(block, 0, POOL_BLOCK_SIZE);
    return block;
}

/* 归还到池 */
void pool_free(MemoryPool *pool, void *ptr) {
    if (!ptr) return;

    PoolBlock *block = (PoolBlock*)ptr;

    pthread_mutex_lock(&pool->lock);
    block->next = pool->free_list;
    pool->free_list = block;
    pthread_mutex_unlock(&pool->lock);
}

/* 销毁池 */
void pool_destroy(MemoryPool *pool) {
    /* 注意：这里简化处理，实际需要跟踪所有chunk */
    pthread_mutex_destroy(&pool->lock);
}

/**
 * @brief 线程本地内存池（无锁）
 */
__thread MemoryPool *tls_pool = NULL;

void* tls_pool_alloc(void) {
    if (!tls_pool) {
        tls_pool = malloc(sizeof(MemoryPool));
        pool_init(tls_pool);
    }
    return pool_alloc(tls_pool);
}

void tls_pool_free(void *ptr) {
    if (tls_pool) {
        pool_free(tls_pool, ptr);
    }
}

/**
 * @brief 性能基准测试
 */
#define NUM_ALLOCATIONS 10000000
#define NUM_THREADS 4

void* malloc_bench(void *arg) {
    void **ptrs = malloc(NUM_ALLOCATIONS * sizeof(void*));

    clock_t start = clock();
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        ptrs[i] = malloc(POOL_BLOCK_SIZE);
    }
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        free(ptrs[i]);
    }
    clock_t end = clock();

    free(ptrs);
    return (void*)(size_t)(end - start);
}

void* pool_bench(void *arg) {
    MemoryPool pool;
    pool_init(&pool);

    void **ptrs = malloc(NUM_ALLOCATIONS * sizeof(void*));

    clock_t start = clock();
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        ptrs[i] = pool_alloc(&pool);
    }
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        pool_free(&pool, ptrs[i]);
    }
    clock_t end = clock();

    free(ptrs);
    pool_destroy(&pool);
    return (void*)(size_t)(end - start);
}

void benchmark_memory_pool(void) {
    printf("=== Memory Pool Benchmark ===\n\n");

    /* 单线程测试 */
    printf("Single-threaded (%d allocations of %d bytes):\n",
           NUM_ALLOCATIONS, POOL_BLOCK_SIZE);

    clock_t malloc_time = (clock_t)(size_t)malloc_bench(NULL);
    printf("  malloc/free: %ld ms\n", malloc_time * 1000 / CLOCKS_PER_SEC);

    clock_t pool_time = (clock_t)(size_t)pool_bench(NULL);
    printf("  Memory pool: %ld ms (%.2fx speedup)\n",
           pool_time * 1000 / CLOCKS_PER_SEC,
           (double)malloc_time / pool_time);

    /* 多线程测试 */
    printf("\nMulti-threaded (%d threads):\n", NUM_THREADS);
    pthread_t threads[NUM_THREADS];

    /* malloc多线程 */
    clock_t start = clock();
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, malloc_bench, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_t mt_malloc_time = clock() - start;
    printf("  malloc/free: %ld ms\n", mt_malloc_time * 1000 / CLOCKS_PER_SEC);

    /* 每个线程自己的pool（模拟TLS池） */
    start = clock();
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, pool_bench, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_t mt_pool_time = clock() - start;
    printf("  Memory pool: %ld ms (%.2fx speedup)\n",
           mt_pool_time * 1000 / CLOCKS_PER_SEC,
           (double)mt_malloc_time / mt_pool_time);
}

int main(void) {
    benchmark_memory_pool();
    return 0;
}
