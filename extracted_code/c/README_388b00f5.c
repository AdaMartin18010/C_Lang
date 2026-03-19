/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\README.md
 * Line: 442
 * Language: c
 * Block ID: 388b00f5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// CPU 特性检测
#include <cpuid.h>

typedef struct {
    int has_sse;
    int has_sse2;
    int has_avx;
    int has_avx2;
    int has_avx512;
} cpu_features_t;

void detect_cpu_features(cpu_features_t* feat) {
    unsigned int eax, ebx, ecx, edx;

    // 检查 CPUID 支持
    if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
        feat->has_sse = (edx >> 25) & 1;
        feat->has_sse2 = (edx >> 26) & 1;
        feat->has_avx = (ecx >> 28) & 1;
    }

    if (__get_cpuid(7, &eax, &ebx, &ecx, &edx)) {
        feat->has_avx2 = (ebx >> 5) & 1;
        feat->has_avx512 = (ebx >> 16) & 1;
    }
}

// 分支预测提示
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

// 使用示例
void optimized_search(int* arr, size_t n, int target) {
    for (size_t i = 0; i < n; i++) {
        if (unlikely(arr[i] == target)) {  // 不太可能匹配
            return;  // 快速返回
        }
    }
}
