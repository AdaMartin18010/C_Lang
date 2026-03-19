/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\05_Cache_Optimization_Real_World.md
 * Line: 968
 * Language: c
 * Block ID: d69b0e9a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 内存带宽测试
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 顺序读取带宽
double sequential_read_bandwidth(size_t size_mb) {
    size_t size = size_mb * 1024 * 1024;
    char* buffer = aligned_alloc(4096, size);

    // 初始化
    memset(buffer, 1, size);

    volatile char sink;
    clock_t start = clock();

    // 顺序读取
    for (size_t i = 0; i < size; i += 64) {
        sink += buffer[i];  // 每64字节读一次
    }

    double elapsed = (double)(clock() - start) / CLOCKS_PER_SEC;
    double bandwidth = (double)size / elapsed / 1024 / 1024 / 1024;  // GB/s

    free(buffer);
    return bandwidth;
}

// 随机读取带宽
double random_read_bandwidth(size_t size_mb) {
    size_t size = size_mb * 1024 * 1024;
    char* buffer = aligned_alloc(4096, size);
    size_t* indices = malloc((size / 64) * sizeof(size_t));

    // 生成随机访问模式
    for (size_t i = 0; i < size / 64; i++) {
        indices[i] = (rand() % (size / 64)) * 64;
    }

    volatile char sink;
    clock_t start = clock();

    // 随机读取
    for (size_t i = 0; i < size / 64; i++) {
        sink += buffer[indices[i]];
    }

    double elapsed = (double)(clock() - start) / CLOCKS_PER_SEC;
    double bandwidth = (double)size / elapsed / 1024 / 1024 / 1024;

    free(buffer);
    free(indices);
    return bandwidth;
}

int main() {
    printf("Memory Bandwidth Test\n");
    printf("=====================\n\n");

    printf("Sequential Read: %.2f GB/s\n", sequential_read_bandwidth(100));
    printf("Random Read:     %.2f GB/s\n", random_read_bandwidth(100));

    return 0;
}

/* 典型服务器结果:
 * Sequential Read: 45 GB/s
 * Random Read:     2.1 GB/s  (严重受限于延迟)
 */
