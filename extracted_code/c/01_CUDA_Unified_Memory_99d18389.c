/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\01_CUDA_Unified_Memory.md
 * Line: 658
 * Language: c
 * Block ID: 99d18389
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：假设所有设备都支持统一内存
void wrong_assumption(void) {
    float *data;
    cudaMallocManaged(&data, size);  // 在旧设备上可能失败
}

// 正确：检查设备能力
void check_capability(void) {
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);

    if (!prop.managedMemory) {
        // 回退到传统cudaMalloc/cudaMemcpy
        traditional_approach();
        return;
    }

    // 使用统一内存
    unified_approach();
}
