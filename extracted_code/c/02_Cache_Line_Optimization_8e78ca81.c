/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\02_Cache_Line_Optimization.md
 * Line: 368
 * Language: c
 * Block ID: 8e78ca81
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define _GNU_SOURCE
#include <numa.h>
#include <sched.h>
#include <stdio.h>

void numa_optimization() {
    if (numa_available() < 0) {
        printf("NUMA not available\n");
        return;
    }

    int num_nodes = numa_num_configured_nodes();
    printf("NUMA nodes: %d\n", num_nodes);

    // 分配NUMA本地内存
    int size = 1024 * 1024 * 100;  // 100MB
    int node = 0;  // 目标NUMA节点

    void* local_mem = numa_alloc_onnode(size, node);

    // 绑定线程到特定CPU
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);  // 绑定到CPU 0
    sched_setaffinity(0, sizeof(cpuset), &cpuset);

    // 使用内存...

    numa_free(local_mem, size);
}

// 第一触摸策略 (First Touch)
void first_touch_policy() {
    int n = 1000000;
    double* data = malloc(n * sizeof(double));

    // 重要: 在将使用数据的线程中初始化
    // 这样内存会分配在该线程的本地NUMA节点
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        data[i] = 0.0;  // 第一触摸决定NUMA位置
    }
}
