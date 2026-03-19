/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\README.md
 * Line: 698
 * Language: c
 * Block ID: 20b048d7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 矩阵转置优化 - 避免跨 NUMA 节点访问
void numa_aware_transpose(double *dst, double *src, int n, int numa_node) {
    // 绑定到特定 NUMA 节点
    bind_thread_to_node(numa_node);

    // 分配本地内存
    double *local_dst = alloc_on_node(n * n * sizeof(double), numa_node);
    double *local_src = alloc_on_node(n * n * sizeof(double), numa_node);

    // 复制输入数据到本地
    memcpy(local_src, src, n * n * sizeof(double));

    // 执行转置（使用块优化缓存）
    const int BLOCK = 64;
    for (int ii = 0; ii < n; ii += BLOCK) {
        for (int jj = 0; jj < n; jj += BLOCK) {
            for (int i = ii; i < min(ii + BLOCK, n); i++) {
                for (int j = jj; j < min(jj + BLOCK, n); j++) {
                    local_dst[j * n + i] = local_src[i * n + j];
                }
            }
        }
    }

    // 复制结果
    memcpy(dst, local_dst, n * n * sizeof(double));

    numa_free(local_src, n * n * sizeof(double));
    numa_free(local_dst, n * n * sizeof(double));
}

// First-Touch 策略优化
void first_touch_optimization(float *data, size_t n, int num_threads) {
    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();
        int numa_node = tid % numa_num_configured_nodes();

        // 绑定线程到 NUMA 节点
        bind_thread_to_node(numa_node);

        // 首次访问（分配）由创建线程执行
        size_t chunk = n / num_threads;
        size_t start = tid * chunk;
        size_t end = (tid == num_threads - 1) ? n : start + chunk;

        for (size_t i = start; i < end; i++) {
            data[i] = 0.0f;  // First-touch 分配
        }
    }
}
