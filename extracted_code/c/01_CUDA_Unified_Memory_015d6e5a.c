/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\01_CUDA_Unified_Memory.md
 * Line: 198
 * Language: c
 * Block ID: 015d6e5a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// cudaMemPrefetchAsync: 异步预取到指定设备
// 减少按需迁移的开销

void prefetch_example(void) {
    const int N = 1 << 20;  // 1M元素
    float *data;

    cudaMallocManaged(&data, N * sizeof(float));

    // 初始化数据（在CPU上）
    for (int i = 0; i < N; i++) {
        data[i] = (float)i;
    }

    // 预取到GPU（异步）
    // 参数: 指针, 大小, 目标设备, 流
    cudaMemPrefetchAsync(data, N * sizeof(float), 0, 0);

    // 可以同时做其他CPU工作...

    // 确保预取完成
    cudaStreamSynchronize(0);

    // 现在内核执行时不会触发页错误
    kernel<<<N/256, 256>>>(data);
    cudaDeviceSynchronize();

    // 预取回CPU（可选）
    cudaMemPrefetchAsync(data, N * sizeof(float), cudaCpuDeviceId, 0);
    cudaStreamSynchronize(0);

    // 现在可以在CPU上高效访问
    float sum = 0;
    for (int i = 0; i < N; i++) {
        sum += data[i];
    }

    cudaFree(data);
}

// 多GPU预取
void multi_gpu_prefetch(int num_gpus) {
    int N = 1 << 24;
    float *data;

    cudaMallocManaged(&data, N * sizeof(float));

    // 数据分区预取到不同GPU
    int chunk_size = N / num_gpus;

    for (int i = 0; i < num_gpus; i++) {
        cudaSetDevice(i);
        float *chunk = data + i * chunk_size;
        cudaMemPrefetchAsync(chunk, chunk_size * sizeof(float), i, 0);
    }

    // 并行执行内核
    for (int i = 0; i < num_gpus; i++) {
        cudaSetDevice(i);
        float *chunk = data + i * chunk_size;
        kernel<<<chunk_size/256, 256>>>(chunk);
    }

    // 同步所有设备
    for (int i = 0; i < num_gpus; i++) {
        cudaSetDevice(i);
        cudaDeviceSynchronize();
    }

    cudaFree(data);
}
