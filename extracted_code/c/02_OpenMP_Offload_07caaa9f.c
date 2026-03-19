/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\02_OpenMP_Offload.md
 * Line: 397
 * Language: c
 * Block ID: 07caaa9f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Single Program Multiple Data (SPMD) 模式
// 每个设备线程执行相同代码，处理不同数据

void spmd_example(float *data, int N) {
    #pragma omp target parallel for
    for (int i = 0; i < N; i++) {
        // 每个线程处理一个元素
        // omp_get_thread_num() 获取线程ID
        // omp_get_num_threads() 获取总线程数
        data[i] = process(data[i], omp_get_thread_num());
    }
}

// 使用teams分发到多个线程组
void teams_example(float *data, int N) {
    // teams: 创建多个线程组（类似CUDA blocks）
    // distribute: 在组间分发迭代
    // parallel for: 组内并行
    #pragma omp target teams distribute parallel for
    for (int i = 0; i < N; i++) {
        int team_id = omp_get_team_num();
        int thread_id = omp_get_thread_num();
        data[i] = team_id * 1000 + thread_id;
    }
}

// 细粒度控制teams和threads
void fine_grained_control(float *data, int N) {
    #pragma omp target teams num_teams(128) thread_limit(256) \
        map(tofrom: data[0:N])
    {
        // 在team内共享的变量
        __attribute__((aligned(64))) float shared_mem[256];

        #pragma omp distribute parallel for
        for (int i = 0; i < N; i++) {
            int tid = omp_get_thread_num();

            // 加载到共享内存
            shared_mem[tid] = data[i];

            // 同步（如果支持）
            #pragma omp barrier

            // 处理...

            data[i] = shared_mem[tid];
        }
    }
}
