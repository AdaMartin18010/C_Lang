/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 385
 * Language: c
 * Block ID: db29cb4c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11标准对齐分配
#include <stdalign.h>
#include <stdlib.h>

// 方法1: 静态对齐
alignas(64) float buffer[1024];  // 64字节对齐

// 方法2: 动态对齐 (C11)
float *ptr;
posix_memalign((void**)&ptr, 64, 1024 * sizeof(float));

// 方法3: 标准C11 aligned_alloc
float *ptr2 = aligned_alloc(64, 1024 * sizeof(float));
