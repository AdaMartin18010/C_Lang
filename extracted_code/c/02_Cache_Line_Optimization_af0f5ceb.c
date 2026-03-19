/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\02_Cache_Line_Optimization.md
 * Line: 439
 * Language: c
 * Block ID: af0f5ceb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误: 预取太提前或太多
for (int i = 0; i < n; i++) {
    _mm_prefetch(&data[i + 10000], _MM_HINT_T0);  // 太远了！
    _mm_prefetch(&data[i + 1], _MM_HINT_T0);       // 太近了！
    sum += data[i];
}

// 正确: 适度预取距离
#define PREFETCH_DISTANCE 64
for (int i = 0; i < n; i++) {
    if (i + PREFETCH_DISTANCE < n) {
        _mm_prefetch(&data[i + PREFETCH_DISTANCE], _MM_HINT_T0);
    }
    sum += data[i];
}
