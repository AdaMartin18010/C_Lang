/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\01_CUDA_Unified_Memory.md
 * Line: 624
 * Language: c
 * Block ID: 68e0a23e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：在GPU内核执行期间访问统一内存
void dangerous_pattern(void) {
    float *data;
    cudaMallocManaged(&data, size);

    kernel<<<grid, block>>>(data);

    // ❌ 危险：内核仍在执行，CPU访问可能竞争
    float sum = 0;
    for (int i = 0; i < N; i++) {
        sum += data[i];
    }

    cudaDeviceSynchronize();
}

// 正确：同步后再访问
void safe_pattern(void) {
    float *data;
    cudaMallocManaged(&data, size);

    kernel<<<grid, block>>>(data);
    cudaDeviceSynchronize();  // ✅ 确保GPU完成

    float sum = 0;
    for (int i = 0; i < N; i++) {
        sum += data[i];
    }
}
