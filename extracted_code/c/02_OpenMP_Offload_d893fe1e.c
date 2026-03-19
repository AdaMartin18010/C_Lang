/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\02_OpenMP_Offload.md
 * Line: 751
 * Language: c
 * Block ID: d893fe1e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：异步target和主机访问数据竞争
void async_race_condition(void) {
    float data[1000];

    #pragma omp target nowait map(tofrom: data[0:1000])
    {
        // 修改data...
        for (int i = 0; i < 1000; i++) {
            data[i] = compute(i);
        }
    }

    // ❌ 危险：target可能还在执行
    float sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += data[i];
    }
}

// 正确：等待target完成
void correct_async(void) {
    float data[1000];

    #pragma omp target nowait map(tofrom: data[0:1000])
    {
        for (int i = 0; i < 1000; i++) {
            data[i] = compute(i);
        }
    }

    // 显式等待
    #pragma omp taskwait

    // 现在安全访问
    float sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += data[i];
    }
}
