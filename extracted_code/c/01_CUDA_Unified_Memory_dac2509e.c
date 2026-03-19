/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\01_CUDA_Unified_Memory.md
 * Line: 274
 * Language: c
 * Block ID: dac2509e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// cudaMemAdvise: 给运行时系统提供内存使用提示

void mem_advise_example(void) {
    const int N = 1 << 20;
    float *data;

    cudaMallocManaged(&data, N * sizeof(float));

    // 内存建议类型

    // 1. cudaMemAdviseSetReadMostly
    // 数据主要被读取，很少写入
    // 允许多个GPU同时缓存只读副本
    cudaMemAdvise(data, N * sizeof(float),
                  cudaMemAdviseSetReadMostly, 0);

    // 多GPU只读访问场景
    for (int i = 0; i < num_gpus; i++) {
        cudaSetDevice(i);
        read_only_kernel<<<blocks, threads>>>(data);
    }

    // 清除只读建议
    cudaMemAdvise(data, N * sizeof(float),
                  cudaMemAdviseUnsetReadMostly, 0);

    // 2. cudaMemAdviseSetPreferredLocation
    // 设置数据的首选位置
    cudaMemAdvise(data, N * sizeof(float),
                  cudaMemAdviseSetPreferredLocation, 0);  // GPU 0

    // 或设置在CPU
    // cudaMemAdvise(data, size, cudaMemAdviseSetPreferredLocation,
    //               cudaCpuDeviceId);

    // 3. cudaMemAdviseSetAccessedBy
    // 预建立GPU页表映射，避免首次访问时的页错误
    cudaMemAdvise(data, N * sizeof(float),
                  cudaMemAdviseSetAccessedBy, 0);  // GPU 0

    // 多GPU场景：为所有GPU建立映射
    for (int i = 0; i < num_gpus; i++) {
        cudaMemAdvise(data, N * sizeof(float),
                      cudaMemAdviseSetAccessedBy, i);
    }
}

// 优化案例分析：图像处理流水线
void image_processing_pipeline(void) {
    int width = 1920, height = 1080;
    size_t image_size = width * height * 3 * sizeof(uint8_t);

    uint8_t *input, *output, *intermediate;
    cudaMallocManaged(&input, image_size);
    cudaMallocManaged(&output, image_size);
    cudaMallocManaged(&intermediate, image_size);

    // 读取输入图像（CPU）
    load_image(input);

    // 建议：输入数据主要在GPU读取，输出在GPU写入
    cudaMemAdvise(input, image_size, cudaMemAdviseSetReadMostly, 0);
    cudaMemAdvise(input, image_size, cudaMemAdviseSetPreferredLocation, 0);

    // 预取到GPU
    cudaMemPrefetchAsync(input, image_size, 0, 0);

    // 阶段1: 去噪
    denoise_kernel<<<grid, block>>>(input, intermediate, width, height);

    // 阶段2: 锐化
    sharpen_kernel<<<grid, block>>>(intermediate, output, width, height);

    // 建议：输出数据回到CPU
    cudaMemPrefetchAsync(output, image_size, cudaCpuDeviceId, 0);
    cudaStreamSynchronize(0);

    // 保存结果
    save_image(output);

    cudaFree(input);
    cudaFree(output);
    cudaFree(intermediate);
}
