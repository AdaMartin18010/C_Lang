/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\07_Performance_Optimization.md
 * Line: 327
 * Language: c
 * Block ID: 834bd595
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 软件预取提示
#include <immintrin.h>

void process_array(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        // 预取未来数据
        if (i + 16 < n) {
            _mm_prefetch((const char*)&arr[i + 16], _MM_HINT_T0);
        }
        process(arr[i]);
    }
}
