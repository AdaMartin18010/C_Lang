/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\02_Hard_RealTime.md
 * Line: 771
 * Language: c
 * Block ID: eccfc1a8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 数据访问模式导致频繁缓存未命中
for (int i = 0; i < 1000; i++) {
    process(data[i * 64]);  // 跳跃访问，每次都在不同缓存行
}

// ✅ 顺序访问，充分利用缓存
for (int i = 0; i < 1000; i++) {
    process(data[i]);  // 连续访问
}

// ✅ 关键代码和数据锁定在缓存
void lock_critical_code_in_cache(void) {
    // 将关键函数预取到缓存
    prefetch_code(safety_critical_function, function_size);
    // 锁定缓存行
    lock_cache_lines(cache_set);
}
