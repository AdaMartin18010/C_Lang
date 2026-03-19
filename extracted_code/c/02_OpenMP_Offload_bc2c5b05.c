/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\02_OpenMP_Offload.md
 * Line: 524
 * Language: c
 * Block ID: bc2c5b05
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化数据映射，减少传输

void optimized_data_reuse(float *input, float *output,
                          int N, int iterations) {
    // 创建持久数据环境
    #pragma omp target data map(to: input[0:N]) map(alloc: output[0:N])
    {
        float *temp1 = (float *)omp_target_alloc(N * sizeof(float),
                                                  omp_get_default_device());
        float *temp2 = (float *)omp_target_alloc(N * sizeof(float),
                                                  omp_get_default_device());

        // 多次迭代，数据保持在设备上
        for (int iter = 0; iter < iterations; iter++) {
            // 交替使用temp1和temp2
            float *src = (iter % 2 == 0) ? input : temp2;
            float *dst = (iter % 2 == 0) ? temp1 : temp2;

            #pragma omp target parallel for
            for (int i = 0; i < N; i++) {
                dst[i] = compute(src[i]);
            }
        }

        // 最后一次迭代结果复制到output
        float *final_src = (iterations % 2 == 0) ? input : temp2;
        #pragma omp target parallel for
        for (int i = 0; i < N; i++) {
            output[i] = final_src[i];
        }

        omp_target_free(temp1, omp_get_default_device());
        omp_target_free(temp2, omp_get_default_device());
    }
}

// 使用declare target声明设备函数和变量
#pragma omp declare target

// 设备上可用的全局变量
float device_constant_table[256];

// 设备函数
float device_function(float x) {
    return x * x + 1.0f;
}

#pragma omp end declare target

void use_declare_target(float *data, int N) {
    // 初始化设备常量表
    #pragma omp target
    {
        for (int i = 0; i < 256; i++) {
            device_constant_table[i] = i * 0.1f;
        }
    }

    // 内核使用设备函数和常量
    #pragma omp target parallel for
    for (int i = 0; i < N; i++) {
        int idx = (int)(data[i]) % 256;
        data[i] = device_function(data[i]) + device_constant_table[idx];
    }
}
