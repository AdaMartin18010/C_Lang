/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\09_Performance_Optimization.md
 * Line: 506
 * Language: c
 * Block ID: 5d96cf70
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 结构体内存布局优化
// 不好 (24字节)
struct Bad {
    char a;      // 1 + 3 padding
    int b;       // 4
    char c;      // 1 + 3 padding
    int d;       // 4
    char e;      // 1 + 3 padding
};

// 好 (12字节)
struct Good {
    int b;       // 4
    int d;       // 4
    char a;      // 1
    char c;      // 1
    char e;      // 1 + 1 padding
};

// 显式对齐
#include <stdalign.h>

alignas(64) float buffer[1024];  // 64字节对齐 (缓存行)

// 动态对齐
float *ptr = aligned_alloc(64, size);

// 结构体对齐
struct Aligned {
    char data;
    alignas(64) char cache_line[64];  // 单独对齐
};
