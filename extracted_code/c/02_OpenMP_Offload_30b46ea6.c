/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\02_OpenMP_Offload.md
 * Line: 594
 * Language: c
 * Block ID: 30b46ea6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// OpenMP 5.0+ 多设备支持

void multi_device_distribute(float *data, int N, int num_devices) {
    int chunk_size = (N + num_devices - 1) / num_devices;

    // 为每个设备创建任务
    for (int d = 0; d < num_devices; d++) {
        int start = d * chunk_size;
        int end = (d + 1) * chunk_size;
        if (end > N) end = N;
        int actual_chunk = end - start;

        #pragma omp task
        {
            // 设置目标设备
            omp_set_default_device(d);

            float *chunk = data + start;

            #pragma omp target map(tofrom: chunk[0:actual_chunk])
            {
                #pragma omp parallel for
                for (int i = 0; i < actual_chunk; i++) {
                    chunk[i] = process(chunk[i]);
                }
            }
        }
    }

    // 等待所有设备完成
    #pragma omp taskwait
}

// 设备间的数据传递
void device_to_device_transfer(float *data, int N, int src_dev, int dst_dev) {
    // 在源设备上分配和计算
    float *src_ptr = (float *)omp_target_alloc_device(N * sizeof(float), src_dev);

    #pragma omp target device(src_dev)
    {
        #pragma omp parallel for
        for (int i = 0; i < N; i++) {
            src_ptr[i] = compute_source(i);
        }
    }

    // 设备间直接复制（如果支持）
    float *dst_ptr = (float *)omp_target_alloc_device(N * sizeof(float), dst_dev);

    // 使用memmove进行设备间传输
    omp_target_memcpy(dst_ptr, src_ptr, N * sizeof(float),
                      0, 0,  // dst和src偏移
                      dst_dev, src_dev);

    // 在目标设备上继续处理
    #pragma omp target device(dst_dev)
    {
        #pragma omp parallel for
        for (int i = 0; i < N; i++) {
            dst_ptr[i] = process_destination(dst_ptr[i]);
        }
    }

    // 复制回主机
    float *host_result = (float *)malloc(N * sizeof(float));
    omp_target_memcpy(host_result, dst_ptr, N * sizeof(float),
                      0, 0,
                      omp_get_initial_device(), dst_dev);

    // 清理
    omp_target_free(src_ptr, src_dev);
    omp_target_free(dst_ptr, dst_dev);
    free(host_result);
}
