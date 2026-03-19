/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\09_Performance_Optimization.md
 * Line: 749
 * Language: c
 * Block ID: 6540b8a2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 总是内联
static inline int max(int a, int b) {
    return a > b ? a : b;
}

// GCC强制内联
#define FORCE_INLINE __attribute__((always_inline))

static FORCE_INLINE int fast_max(int a, int b) {
    return a > b ? a : b;
}

// 禁止内联 (大函数或递归)
#define NO_INLINE __attribute__((noinline))

NO_INLINE void big_function(void);

// 编译器会权衡，通常不用手动干预
// 除非 profiling 显示需要
