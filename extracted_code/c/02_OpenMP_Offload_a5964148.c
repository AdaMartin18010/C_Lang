/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\02_OpenMP_Offload.md
 * Line: 230
 * Language: c
 * Block ID: a5964148
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// target data: 创建持续的数据环境
// 多个target区域可以共享数据

void target_data_example(float *a, float *b, float *c,
                         float *d, int N) {
    // 创建设备数据环境
    #pragma omp target data map(to: a[0:N], b[0:N]) \
                            map(alloc: c[0:N], d[0:N])
    {
        // 第一个内核
        #pragma omp target parallel for nowait
        for (int i = 0; i < N; i++) {
            c[i] = a[i] + b[i];
        }

        // 第二个内核（c已在设备上，无需再次传输）
        #pragma omp target parallel for
        for (int i = 0; i < N; i++) {
            d[i] = c[i] * 2.0f;
        }

        // 退出target data时，c和d是alloc映射，不复制回
    }
}

// 使用update子句显式同步数据
void update_clause_example(float *data, int N) {
    #pragma omp target data map(alloc: data[0:N])
    {
        // 设备初始化
        #pragma omp target parallel for
        for (int i = 0; i < N; i++) {
            data[i] = i * 1.0f;
        }

        // 将设备数据更新回主机
        #pragma omp target update from(data[0:N/2])

        // 主机可以访问前半部分
        printf("Host: data[0] = %f\n", data[0]);

        // 修改主机数据
        for (int i = 0; i < N/2; i++) {
            data[i] *= 2;
        }

        // 将主机数据更新到设备
        #pragma omp target update to(data[0:N/2])

        // 继续设备执行...
        #pragma omp target parallel for
        for (int i = 0; i < N; i++) {
            data[i] += 1.0f;
        }

        // 最终复制回主机
        #pragma omp target update from(data[0:N])
    }
}

// enter data 和 exit data 分离
void enter_exit_data(float *persistent_data, int size) {
    // 进入数据环境（在函数入口处）
    #pragma omp target enter data map(alloc: persistent_data[0:size])

    // 多次内核执行，数据保持在设备上
    for (int iter = 0; iter < 100; iter++) {
        #pragma omp target parallel for
        for (int i = 0; i < size; i++) {
            persistent_data[i] = compute(persistent_data[i]);
        }
    }

    // 退出数据环境（在函数出口处）
    #pragma omp target exit data map(from: persistent_data[0:size])
}
