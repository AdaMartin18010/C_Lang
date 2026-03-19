/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 2407
 * Language: c
 * Block ID: a0de91bb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file numa_optimization.c
 * @brief NUMA (Non-Uniform Memory Access) 优化
 *
 * NUMA架构特点：
 * - 每个CPU有本地内存，访问更快
 * - 访问远程内存需要跨互连，延迟更高
 * - 线程应使用本地节点的内存
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <numa.h>
#include <numaif.h>
#include <sched.h>
#include <unistd.h>

/**
 * @brief 检测NUMA拓扑
 */
void detect_numa_topology(void) {
    printf("=== NUMA Topology ===\n\n");

    if (numa_available() < 0) {
        printf("NUMA not available on this system\n");
        return;
    }

    int num_nodes = numa_num_configured_nodes();
    int num_cpus = numa_num_configured_cpus();

    printf("NUMA nodes: %d\n", num_nodes);
    printf("CPUs: %d\n", num_cpus);

    for (int node = 0; node < num_nodes; node++) {
        long long free_mem, total_mem;
        numa_node_size64(node, &free_mem);
        total_mem = numa_node_size(node, NULL);

        printf("\nNode %d:\n", node);
        printf("  Memory: %.2f GB total, %.2f GB free\n",
               total_mem / (1024.0 * 1024 * 1024),
               free_mem / (1024.0 * 1024 * 1024));

        /* CPU掩码 */
        struct bitmask *cpus = numa_node_to_cpus(node);
        printf("  CPUs: ");
        for (int i = 0; i < num_cpus; i++) {
            if (numa_bitmask_isbitset(cpus, i)) {
                printf("%d ", i);
            }
        }
        printf("\n");
        numa_bitmask_free(cpus);
    }
}

/**
 * @brief NUMA本地 vs 远程内存访问性能
 */
typedef struct {
    int node;
    char *memory;
    size_t size;
    volatile long long sum;
} ThreadArgs;

void* memory_worker(void *arg) {
    ThreadArgs *args = (ThreadArgs*)arg;

    /* 绑定到指定节点 */
    struct bitmask *mask = numa_allocate_nodemask();
    numa_bitmask_setbit(mask, args->node);
    numa_bind(mask);
    numa_bitmask_free(mask);

    int *arr = (int*)args->memory;
    int n = args->size / sizeof(int);
    long long sum = 0;

    /* 多次访问 */
    for (int iter = 0; iter < 10; iter++) {
        for (int i = 0; i < n; i++) {
            sum += arr[i];
        }
    }

    args->sum = sum;
    return NULL;
}

void numa_locality_test(void) {
    printf("\n=== NUMA Locality Test ===\n\n");

    if (numa_available() < 0) {
        printf("NUMA not available\n");
        return;
    }

    int num_nodes = numa_max_node() + 1;
    if (num_nodes < 2) {
        printf("Single NUMA node, skipping test\n");
        return;
    }

    const size_t SIZE = 256 * 1024 * 1024;  /* 256MB */

    /* 在节点0分配内存 */
    char *mem_node0 = numa_alloc_onnode(SIZE, 0);
    for (int i = 0; i < SIZE / sizeof(int); i++) {
        ((int*)mem_node0)[i] = i;
    }

    pthread_t thread;
    ThreadArgs args = {0, mem_node0, SIZE, 0};

    /* 测试1: 节点0线程访问节点0内存（本地） */
    printf("Node 0 thread accessing Node 0 memory (LOCAL):\n");
    args.node = 0;
    clock_t start = clock();
    pthread_create(&thread, NULL, memory_worker, &args);
    pthread_join(thread, NULL);
    clock_t local_time = clock() - start;
    printf("  Time: %ld ms (sum=%lld)\n",
           local_time * 1000 / CLOCKS_PER_SEC, args.sum);

    /* 测试2: 节点1线程访问节点0内存（远程） */
    if (num_nodes > 1) {
        printf("Node 1 thread accessing Node 0 memory (REMOTE):\n");
        args.node = 1;
        args.sum = 0;
        start = clock();
        pthread_create(&thread, NULL, memory_worker, &args);
        pthread_join(thread, NULL);
        clock_t remote_time = clock() - start;
        printf("  Time: %ld ms (sum=%lld)\n",
               remote_time * 1000 / CLOCKS_PER_SEC, args.sum);
        printf("  Remote access penalty: %.2fx\n",
               (double)remote_time / local_time);
    }

    numa_free(mem_node0, SIZE);
}

/**
 * @brief NUMA感知的内存分配策略
 */
void numa_allocation_policies(void) {
    printf("\n=== NUMA Allocation Policies ===\n\n");

    if (numa_available() < 0) return;

    const size_t SIZE = 100 * 1024 * 1024;

    /* 默认策略 - 在本地节点分配 */
    printf("1. Default (local preferred):\n");
    char *p1 = numa_alloc(SIZE);
    printf("   Allocated on node: %d\n", numa_node_of_cpu(sched_getcpu()));
    numa_free(p1, SIZE);

    /* 绑定到特定节点 */
    printf("2. Bind to node 0:\n");
    char *p2 = numa_alloc_onnode(SIZE, 0);
    printf("   Explicitly allocated on node 0\n");
    numa_free(p2, SIZE);

    /* 交错分配 - 在所有节点间轮询 */
    printf("3. Interleaved (all nodes):\n");
    char *p3 = numa_alloc_interleaved(SIZE);
    printf("   Pages distributed across all nodes\n");
    numa_free(p3, SIZE);
}

int main(void) {
    detect_numa_topology();
    numa_locality_test();
    numa_allocation_policies();
    return 0;
}
