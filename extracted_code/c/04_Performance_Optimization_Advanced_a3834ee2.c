/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 4846
 * Language: c
 * Block ID: a3834ee2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file hft_checklist.c
 * @brief 高频交易系统优化检查清单
 *
 * 编译优化标志：
 * gcc -O3 -march=native -mtune=native -flto -funroll-loops \
 *     -fno-stack-protector -fomit-frame-pointer \
 *     -DNDEBUG hft_optimized.c -o hft_optimized
 */

#ifndef HFT_CHECKLIST_H
#define HFT_CHECKLIST_H

/* 1. 编译器优化 */
#define COMPILE_FLAGS "-O3 -march=native -mtune=native -flto"

/* 2. 内存优化 */
#define USE_HUGEPAGES 1
#define USE_NUMA_LOCAL 1
#define CACHE_LINE_ALIGN __attribute__((aligned(64)))

/* 3. 数据结构设计 */
#define AVOID_FALSE_SHARING 1
#define PREFETCH_HOT_DATA 1
#define USE_SOA_LAYOUT 1

/* 4. 并发优化 */
#define USE_LOCK_FREE 1
#define USE_THREAD_AFFINITY 1
#define AVOID_SHARED_STATE 1

/* 5. I/O优化 */
#define USE_BUSY_POLLING 1
#define DISABLE_NAGLE 1
#define KERNEL_BYPASS 0  /* DPDK/RDMA需要专门硬件 */

/* 6. 算法优化 */
#define PRECOMPUTE_LOOKUP 1
#define USE_SIMD 1
#define BRANCHLESS_CODE 1

/* 7. 测量与监控 */
#define MEASURE_LATENCY 1
#define TRACK_PERCENTILES 1
#define PMC_PROFILING 0  /* 仅在调试时启用 */

#endif /* HFT_CHECKLIST_H */
