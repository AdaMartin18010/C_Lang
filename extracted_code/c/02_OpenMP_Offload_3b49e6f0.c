/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\02_OpenMP_Offload.md
 * Line: 677
 * Language: c
 * Block ID: 3b49e6f0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：指针数据未映射
void wrong_pointer_mapping(void) {
    float *data = (float *)malloc(N * sizeof(float));
    // 初始化...

    #pragma omp target parallel for  // ❌ data指向的内存未映射！
    for (int i = 0; i < N; i++) {
        data[i] = data[i] * 2;  // 可能段错误
    }
}

// 正确：显式映射数据
void correct_pointer_mapping(void) {
    float *data = (float *)malloc(N * sizeof(float));
    // 初始化...

    #pragma omp target parallel for map(tofrom: data[0:N])
    for (int i = 0; i < N; i++) {
        data[i] = data[i] * 2;
    }
}

// 或使用统一共享内存
void usm_solution(void) {
    float *data = (float *)omp_target_alloc_shared(N * sizeof(float),
                                                    omp_get_default_device());
    // 无需map
    #pragma omp target parallel for
    for (int i = 0; i < N; i++) {
        data[i] = data[i] * 2;
    }
}
