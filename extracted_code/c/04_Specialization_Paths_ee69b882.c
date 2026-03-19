/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\04_Specialization_Paths.md
 * Line: 377
 * Language: c
 * Block ID: ee69b882
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 目标：实现接近Intel MKL性能的GEMM

// 优化步骤：
// 1. 朴素三重循环 (5 GFLOPS)
// 2. 缓存分块 (20 GFLOPS)
// 3. SSE向量化 (40 GFLOPS)
// 4. AVX2向量化 (80 GFLOPS)
// 5. AVX-512 (120 GFLOPS)
// 6. 汇编优化 (150 GFLOPS)

// 参考：BLIS、OpenBLAS源码
