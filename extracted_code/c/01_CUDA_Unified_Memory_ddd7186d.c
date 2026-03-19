/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\01_CUDA_Unified_Memory.md
 * Line: 365
 * Language: c
 * Block ID: ddd7186d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 统一内存提供自动一致性
// 通过设备同步点保证

void consistency_example(void) {
    int N = 1024;
    float *data;
    cudaMallocManaged(&data, N * sizeof(float));

    // CPU写入
    for (int i = 0; i < N; i++) {
        data[i] = 1.0f;
    }

    // 内核读取和修改
    increment_kernel<<<N/256, 256>>>(data);

    // 同步点：确保GPU修改可见
    cudaDeviceSynchronize();

    // CPU读取结果（自动一致）
    for (int i = 0; i < N; i++) {
        assert(data[i] == 2.0f);
    }

    cudaFree(data);
}

// 流排序一致性
void stream_consistency(void) {
    cudaStream_t stream1, stream2;
    cudaStreamCreate(&stream1);
    cudaStreamCreate(&stream2);

    float *data;
    cudaMallocManaged(&data, 1024 * sizeof(float));

    // 流1写入
    write_kernel<<<blocks, threads, 0, stream1>>>(data);

    // 流2读取（流排序保证可见性）
    read_kernel<<<blocks, threads, 0, stream2>>>(data);

    // 但跨流需要显式同步
    cudaStreamSynchronize(stream1);
    // 现在stream2的读操作能看到stream1的写操作

    cudaStreamDestroy(stream1);
    cudaStreamDestroy(stream2);
    cudaFree(data);
}
