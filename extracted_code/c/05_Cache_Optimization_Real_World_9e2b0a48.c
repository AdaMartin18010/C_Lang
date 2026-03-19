/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\05_Cache_Optimization_Real_World.md
 * Line: 528
 * Language: c
 * Block ID: 9e2b0a48
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 硬件预取友好代码
void process_array_good(float* arr, int n) {
    // 顺序访问 - 硬件预取有效
    for (int i = 0; i < n; i++) {
        arr[i] = arr[i] * 2.0f;
    }
}

void process_array_bad(float* arr, int n) {
    // 随机访问 - 硬件预取无效
    for (int i = 0; i < n; i++) {
        int idx = hash_function(i) % n;
        arr[idx] = arr[idx] * 2.0f;
    }
}
