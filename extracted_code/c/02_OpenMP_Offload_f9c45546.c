/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\02_OpenMP_Offload.md
 * Line: 311
 * Language: c
 * Block ID: f9c45546
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// OpenMP 5.0+ 支持统一共享内存
// 类似CUDA统一内存，自动页面迁移

// 需要编译器支持：-fopenmp-targets=... -fopenmp-usm

void unified_shared_memory(void) {
    const int N = 1 << 20;

    // 使用标准malloc分配
    // 在USM模式下，内存自动对设备可访问
    float *data = (float *)malloc(N * sizeof(float));

    // 初始化
    for (int i = 0; i < N; i++) {
        data[i] = i * 0.5f;
    }

    // 无需显式map，直接访问
    #pragma omp target parallel for
    for (int i = 0; i < N; i++) {
        data[i] = data[i] * data[i];
    }

    // 主机访问结果（自动同步）
    float sum = 0;
    for (int i = 0; i < N; i++) {
        sum += data[i];
    }

    free(data);
}

// 显式USM内存分配
void explicit_usm_allocation(void) {
    const int N = 1024;
    int device = omp_get_default_device();

    // 分配统一共享内存
    float *shared_mem = (float *)omp_target_alloc_host(N * sizeof(float), device);

    // 或使用设备分配
    float *device_mem = (float *)omp_target_alloc_device(N * sizeof(float), device);

    // 或使用共享分配（自动迁移）
    float *managed_mem = (float *)omp_target_alloc_shared(N * sizeof(float), device);

    // 初始化并使用...

    // 释放
    omp_target_free(shared_mem, device);
    omp_target_free(device_mem, device);
    omp_target_free(managed_mem, device);
}

// USM与显式map的混合使用
void mixed_usm_explicit(void) {
    int N = 1024;

    // USM内存
    float *usm_data = (float *)omp_target_alloc_shared(N * sizeof(float),
                                                        omp_get_default_device());

    // 普通主机内存
    float *host_data = (float *)malloc(N * sizeof(float));

    // 初始化USM数据
    for (int i = 0; i < N; i++) {
        usm_data[i] = i;
    }

    // 混合使用：USM自动迁移，host_data显式映射
    #pragma omp target parallel for map(to: host_data[0:N])
    for (int i = 0; i < N; i++) {
        usm_data[i] = usm_data[i] + host_data[i];
    }

    omp_target_free(usm_data, omp_get_default_device());
    free(host_data);
}
