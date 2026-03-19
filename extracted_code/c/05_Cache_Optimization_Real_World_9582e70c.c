/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\05_Cache_Optimization_Real_World.md
 * Line: 928
 * Language: c
 * Block ID: 9582e70c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用PAPI库 (Portable Hardware Performance Counters)
#include <papi.h>

void measure_cache_performance(void) {
    int events[3] = {
        PAPI_L1_DCM,   // L1数据缓存未命中
        PAPI_L2_DCM,   // L2数据缓存未命中
        PAPI_L3_TCM    // L3总缓存未命中
    };
    long long values[3];
    int EventSet = PAPI_NULL;

    // 初始化PAPI
    PAPI_library_init(PAPI_VER_CURRENT);
    PAPI_create_eventset(&EventSet);
    PAPI_add_events(EventSet, events, 3);

    // 开始计数
    PAPI_start(EventSet);

    // 执行代码
    run_algorithm();

    // 停止计数
    PAPI_stop(EventSet, values);

    printf("L1 Cache Misses:  %lld\n", values[0]);
    printf("L2 Cache Misses:  %lld\n", values[1]);
    printf("L3 Cache Misses:  %lld\n", values[2]);

    // 计算缓存命中率
    long long l1_accesses = get_l1_accesses();  // 需要另外计数
    double l1_hit_rate = 100.0 * (l1_accesses - values[0]) / l1_accesses;
    printf("L1 Hit Rate: %.2f%%\n", l1_hit_rate);
}
