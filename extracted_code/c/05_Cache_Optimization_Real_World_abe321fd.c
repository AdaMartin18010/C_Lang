/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\05_Cache_Optimization_Real_World.md
 * Line: 496
 * Language: c
 * Block ID: abe321fd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 更好的方案：使用线程私有数据
_Thread_local int local_counter;  // 每个线程独立副本

void* increment_optimized(void* arg) {
    for (int i = 0; i < 100000000; i++) {
        local_counter++;  // 无共享，无竞争
    }

    // 最后汇总到全局
    global_counters[thread_id] = local_counter;
    return NULL;
}

/* 性能:
 * 线程私有: 0.05 s (4线程)
 * 几乎达到线性加速
 */
