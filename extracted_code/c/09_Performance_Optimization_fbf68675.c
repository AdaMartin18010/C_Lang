/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\09_Performance_Optimization.md
 * Line: 548
 * Language: c
 * Block ID: fbf68675
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// GCC内置预取
#include <xmmintrin.h>

void process_with_prefetch(float *arr, int n) {
    for (int i = 0; i < n; i++) {
        // 预取未来要访问的数据
        if (i + 16 < n) {
            _mm_prefetch((char *)&arr[i + 16], _MM_HINT_T0);
        }

        // 处理当前数据
        process(arr[i]);
    }
}

// 或者使用GCC内置
void prefetch(void *addr) {
    __builtin_prefetch(addr, 0, 3);  // 读, 高局部性
}
