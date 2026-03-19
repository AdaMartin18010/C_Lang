/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 1218
 * Language: c
 * Block ID: ab49337c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * HLS调试实用技巧
 */

// 技巧1：条件编译调试
void debuggable_design(int in[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE

        int temp = compute(in[i]);

        #ifndef __SYNTHESIS__
        // 仅在C仿真时执行
        printf("Step %d: in=%d, temp=%d\n", i, in[i], temp);
        assert(temp >= 0 && temp < 1000);
        #endif

        out[i] = temp;
    }
}

// 技巧2：波形调试
// 在cosim_design时设置 -trace_level all
// 生成波形文件（.wdb）用于Vivado分析

// 技巧3：断点模拟
void breakpoint_simulation(int data[1000]) {
    for (int i = 0; i < 1000; i++) {
        #pragma HLS PIPELINE

        #ifndef __SYNTHESIS__
        if (i == 500) {
            printf("Breakpoint at i=500\n");
            printf("data[500] = %d\n", data[i]);
            getchar();  // 暂停
        }
        #endif

        data[i] = process(data[i]);
    }
}

// 技巧4：分段验证
void incremental_verification(int in[1000], int out[1000]) {
    // 阶段1：验证直通
    for (int i = 0; i < 1000; i++) {
        #pragma HLS PIPELINE
        out[i] = in[i];
    }
    // 验证通过后再添加处理
}

// 技巧5：性能计数器
void performance_counters(int in[1000], int out[1000]) {
    #ifndef __SYNTHESIS__
    clock_t start = clock();
    #endif

    for (int i = 0; i < 1000; i++) {
        #pragma HLS PIPELINE
        out[i] = heavy_computation(in[i]);
    }

    #ifndef __SYNTHESIS__
    clock_t end = clock();
    printf("Elapsed: %f ms\n",
           (double)(end - start) / CLOCKS_PER_SEC * 1000);
    #endif
}
