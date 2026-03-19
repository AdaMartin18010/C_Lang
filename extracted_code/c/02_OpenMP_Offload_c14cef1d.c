/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\02_OpenMP_Offload.md
 * Line: 452
 * Language: c
 * Block ID: c14cef1d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// OpenMP Offload中的任务并行

void task_parallel_offload(float *data, int N) {
    #pragma omp target map(tofrom: data[0:N])
    {
        // 创建任务在设备上异步执行
        #pragma omp task
        {
            for (int i = 0; i < N/2; i++) {
                data[i] = compute_a(data[i]);
            }
        }

        #pragma omp task
        {
            for (int i = N/2; i < N; i++) {
                data[i] = compute_b(data[i]);
            }
        }

        // 等待所有任务完成
        #pragma omp taskwait
    }
}

// 异步目标区域
void async_target(float *a, float *b, float *c, int N) {
    // nowait: 异步执行，不等待完成
    #pragma omp target nowait map(to: a[0:N], b[0:N]) map(from: c[0:N])
    {
        #pragma omp parallel for
        for (int i = 0; i < N; i++) {
            c[i] = a[i] + b[i];
        }
    }

    // 主机可以在这里做其他工作...
    do_cpu_work();

    // 显式等待目标区域完成
    #pragma omp taskwait
    // 或使用 barrier
    // #pragma omp barrier
}

// 多个异步目标区域
void multiple_async_targets(float *data, int num_sections) {
    int section_size = N / num_sections;

    for (int s = 0; s < num_sections; s++) {
        float *section = data + s * section_size;

        #pragma omp target nowait \
            map(tofrom: section[0:section_size])
        {
            #pragma omp parallel for
            for (int i = 0; i < section_size; i++) {
                section[i] = process(section[i]);
            }
        }
    }

    // 等待所有section完成
    #pragma omp taskwait
}
