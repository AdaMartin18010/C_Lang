/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\01_CUDA_Unified_Memory.md
 * Line: 554
 * Language: c
 * Block ID: 6c7d413d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 统一内存允许分配超过GPU物理内存的数据
// 系统会自动在CPU和GPU之间换页

void oversubscription_example(void) {
    size_t gpu_memory;
    cudaMemGetInfo(NULL, &gpu_memory);

    // 分配2倍GPU内存
    size_t alloc_size = gpu_memory * 2;
    float *large_data;
    cudaMallocManaged(&large_data, alloc_size);

    printf("GPU内存: %zu MB\n", gpu_memory / (1024*1024));
    printf("分配: %zu MB\n", alloc_size / (1024*1024));

    // 初始化所有数据（在CPU上）
    for (size_t i = 0; i < alloc_size / sizeof(float); i++) {
        large_data[i] = (float)i;
    }

    // 分批处理
    size_t batch_size = gpu_memory / 2;
    int num_batches = (alloc_size + batch_size - 1) / batch_size;

    for (int b = 0; b < num_batches; b++) {
        float *batch = (float *)((char *)large_data + b * batch_size);
        size_t current_batch = (b == num_batches - 1) ?
            (alloc_size - b * batch_size) : batch_size;

        // 预取当前批次
        cudaMemPrefetchAsync(batch, current_batch, 0, 0);

        // 处理
        kernel<<<current_batch / 256 / sizeof(float), 256>>>(
            batch, current_batch / sizeof(float));

        // 写回（可选）
        cudaMemPrefetchAsync(batch, current_batch, cudaCpuDeviceId, 0);
    }

    cudaDeviceSynchronize();
    cudaFree(large_data);
}
