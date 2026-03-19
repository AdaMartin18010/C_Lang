/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\05_Cache_Optimization_Real_World.md
 * Line: 741
 * Language: c
 * Block ID: f3f918d6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// first-touch策略：数据初始化时分配到访问它的节点

void first_touch_allocation(float** data, size_t n, int num_threads) {
    // 1. 使用interleave分配，避免全部在一个节点
    *data = numa_alloc_interleaved(n * sizeof(float));

    // 2. 每个线程初始化自己的部分 (触发first-touch)
    #pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < n; i++) {
        (*data)[i] = 0.0f;  // 这会分配数据到当前线程的本地节点
    }
}

// 对比: 非NUMA感知分配
void bad_allocation(float** data, size_t n) {
    // 所有数据都在调用线程的节点
    *data = malloc(n * sizeof(float));

    // 初始化
    for (int i = 0; i < n; i++) {
        (*data)[i] = 0.0f;
    }

    // 其他节点访问时都是远程访问！
}

/* 性能对比 (双路服务器, 64GB数据):
 * 非NUMA感知:  2.5 s
 * NUMA优化:    1.3 s (1.9x加速)
 */
