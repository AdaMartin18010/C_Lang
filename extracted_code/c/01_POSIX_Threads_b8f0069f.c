/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 2798
 * Language: c
 * Block ID: b8f0069f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>

/*========================================
 * 绑定线程到特定CPU核心
 * 减少缓存迁移，提高性能
 *========================================*/

int set_thread_affinity(int cpu_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);

    pthread_t current = pthread_self();
    return pthread_setaffinity_np(current, sizeof(cpu_set_t), &cpuset);
}

void *affinity_worker(void *arg) {
    int cpu_id = *(int*)arg;

    // 设置CPU亲和性
    if (set_thread_affinity(cpu_id) != 0) {
        fprintf(stderr, "Failed to set affinity to CPU %d\n", cpu_id);
    }

    // 验证
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

    printf("Thread running on CPU: ");
    for (int i = 0; i < CPU_SETSIZE; i++) {
        if (CPU_ISSET(i, &cpuset)) {
            printf("%d ", i);
        }
    }
    printf("\n");

    // 执行工作...
    return NULL;
}

/*========================================
 * NUMA感知的内存分配
 *========================================*/
#include <numa.h>

void *numa_alloc_on_node(size_t size, int node) {
    #ifdef HAVE_NUMA
    return numa_alloc_onnode(size, node);
    #else
    return malloc(size);
    #endif
}

void numa_optimal_worker(int node_id) {
    // 在NUMA节点上分配内存
    void *local_memory = numa_alloc_onnode(1024 * 1024, node_id);

    // 绑定到对应节点的CPU
    int cpu_start = node_id * (sysconf(_SC_NPROCESSORS_ONLN) / numa_num_configured_nodes());
    set_thread_affinity(cpu_start);

    // 访问本地内存（低延迟）
    // ...

    #ifdef HAVE_NUMA
    numa_free(local_memory, 1024 * 1024);
    #else
    free(local_memory);
    #endif
}
