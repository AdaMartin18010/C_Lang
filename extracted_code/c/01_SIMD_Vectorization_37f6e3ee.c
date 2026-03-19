/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 2445
 * Language: c
 * Block ID: 37f6e3ee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * PAPI (Performance API) 使用示例
 * 编译: gcc -O3 -o perf_test perf_test.c -lpapi
 */

#include <papi.h>
#include <stdio.h>

void measure_with_papi() {
    int EventSet = PAPI_NULL;
    long long values[4];

    // 初始化PAPI
    PAPI_library_init(PAPI_VER_CURRENT);
    PAPI_create_eventset(&EventSet);

    // 添加事件
    PAPI_add_event(EventSet, PAPI_TOT_CYC);   // 总周期
    PAPI_add_event(EventSet, PAPI_TOT_INS);   // 总指令
    PAPI_add_event(EventSet, PAPI_FP_OPS);    // 浮点操作
    PAPI_add_event(EventSet, PAPI_L1_DCM);    // L1数据缓存未命中

    // 开始计数
    PAPI_start(EventSet);

    // ===== 被测代码 =====
    // compute_kernel();
    // ===================

    // 停止计数
    PAPI_stop(EventSet, values);

    printf("总周期:     %lld\n", values[0]);
    printf("总指令:     %lld\n", values[1]);
    printf("浮点操作:   %lld\n", values[2]);
    printf("L1未命中:   %lld\n", values[3]);
    printf("IPC:        %.2f\n", (double)values[1] / values[0]);

    PAPI_cleanup_eventset(EventSet);
    PAPI_destroy_eventset(&EventSet);
}
