/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\01_CUDA_Unified_Memory.md
 * Line: 84
 * Language: c
 * Block ID: b3d1d44a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 统一内存的核心：单个指针访问CPU和GPU内存
// 无需显式的cudaMemcpy

#include <cuda_runtime.h>
#include <stdio.h>

// 传统CUDA（分离内存）
void traditional_approach(void) {
    int *h_data, *d_data;
    size_t size = 1024 * sizeof(int);

    // 主机分配
    h_data = (int *)malloc(size);

    // 设备分配
    cudaMalloc(&d_data, size);

    // 显式传输
    cudaMemcpy(d_data, h_data, size, cudaMemcpyHostToDevice);

    // 内核执行...

    // 复制回主机
    cudaMemcpy(h_data, d_data, size, cudaMemcpyDeviceToHost);

    free(h_data);
    cudaFree(d_data);
}

// 统一内存方法
void unified_memory_approach(void) {
    int *data;
    size_t size = 1024 * sizeof(int);

    // 统一内存分配
    cudaMallocManaged(&data, size);

    // 直接在主机初始化
    for (int i = 0; i < 1024; i++) {
        data[i] = i;
    }

    // 内核直接使用（自动迁移）
    kernel<<<grid, block>>>(data);
    cudaDeviceSynchronize();

    // 直接在主机访问结果
    for (int i = 0; i < 1024; i++) {
        printf("%d ", data[i]);
    }

    cudaFree(data);
}
