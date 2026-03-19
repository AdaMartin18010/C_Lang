/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\01_CUDA_Unified_Memory.md
 * Line: 420
 * Language: c
 * Block ID: 08fbc106
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Pascal及更新架构支持GPU和CPU并发访问统一内存

void concurrent_access_example(void) {
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);

    if (!prop.concurrentManagedAccess) {
        printf("设备不支持并发管理访问\n");
        return;
    }

    const int N = 1 << 20;
    float *data;
    cudaMallocManaged(&data, N * sizeof(float));

    // 允许GPU和CPU同时访问
    // 注意：需要程序员保证没有数据竞争

    // 将数据分区
    int gpu_portion = N * 0.8;  // 80%给GPU
    int cpu_portion = N - gpu_portion;  // 20%给CPU

    // 预取GPU部分
    cudaMemPrefetchAsync(data, gpu_portion * sizeof(float), 0, 0);

    // GPU处理其部分
    gpu_kernel<<<gpu_portion/256, 256>>>(data, gpu_portion);

    // CPU同时处理其部分（需要同步机制）
    // CPU部分无需预取，保持在CPU内存
    cpu_process(data + gpu_portion, cpu_portion);

    cudaDeviceSynchronize();

    cudaFree(data);
}

// 使用内存围栏的细粒度同步
void fine_grained_sync(void) {
    volatile int *flag;
    cudaMallocManaged(&flag, sizeof(int));
    *flag = 0;

    // 内核设置标志
    __global__ void signal_kernel(volatile int *flag) {
        // 完成工作...
        __threadfence_system();  // 系统范围内存围栏
        *flag = 1;
    }

    signal_kernel<<<1, 1>>>(flag);

    // CPU自旋等待（谨慎使用）
    while (*flag == 0) {
        // 忙等待
    }

    cudaDeviceSynchronize();
    cudaFree((void*)flag);
}
