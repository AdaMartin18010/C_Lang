/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\02_Cache_Line_Optimization.md
 * Line: 325
 * Language: c
 * Block ID: f4746a2e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void gcc_prefetch(float* data, int n) {
    float sum = 0;

    for (int i = 0; i < n; i++) {
        // GCC内置函数
        __builtin_prefetch(&data[i + 64], 0, 3);
        // 参数: 地址, 读写(0=读,1=写), 局部性(0-3)

        sum += data[i];
    }
}
