/*
 * Auto-generated from: 12_Practice_Exercises\06_Performance_Optimization.md
 * Line: 170
 * Language: c
 * Block ID: 1f674385
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void prefetch_example(float *data, int n) {
    for (int i = 0; i < n; i++) {
        // 预取未来数据
        if (i + 16 < n) {
            __builtin_prefetch(&data[i + 16], 0, 3);
        }
        process(data[i]);
    }
}
