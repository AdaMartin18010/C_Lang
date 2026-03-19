/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\02_OpenMP_Offload.md
 * Line: 84
 * Language: c
 * Block ID: 799bfda3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// OpenMP Offload基础语法
// 使用target指令将代码卸载到加速器

#include <omp.h>
#include <stdio.h>

// 简单向量加法
void vector_add_offload(float *a, float *b, float *c, int N) {
    // #pragma omp target: 将区域卸载到设备
    // 默认设备通常是GPU
    #pragma omp target
    {
        // 在设备上串行执行
        for (int i = 0; i < N; i++) {
            c[i] = a[i] + b[i];
        }
    }
}

// 带并行化的向量加法
void vector_add_parallel(float *a, float *b, float *c, int N) {
    #pragma omp target parallel for
    for (int i = 0; i < N; i++) {
        c[i] = a[i] + b[i];
    }
}

// 选择特定设备
void vector_add_device_select(float *a, float *b, float *c, int N, int device) {
    // device子句选择目标设备
    #pragma omp target device(device) parallel for
    for (int i = 0; i < N; i++) {
        c[i] = a[i] + b[i];
    }
}

// 检查设备可用性
void check_offload_support(void) {
    // 获取设备数量
    int num_devices = omp_get_num_devices();
    printf("可用设备数: %d\n", num_devices);

    // 获取默认设备
    int default_device = omp_get_default_device();
    printf("默认设备: %d\n", default_device);

    // 获取主机作为设备编号
    int host_device = omp_get_initial_device();
    printf("主机设备编号: %d\n", host_device);

    // 检查设备是否可用
    if (num_devices > 0) {
        // 测试能否在设备上分配内存
        void *test = omp_target_alloc(1024, 0);
        if (test) {
            printf("设备0可用\n");
            omp_target_free(test, 0);
        }
    }
}
