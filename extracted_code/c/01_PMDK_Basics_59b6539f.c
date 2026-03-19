/*
 * Auto-generated from: 03_System_Technology_Domains\12_Persistent_Memory\01_PMDK_Basics.md
 * Line: 86
 * Language: c
 * Block ID: 59b6539f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 系统内存层次
 *
 * ┌─────────────────────────────────────────┐
 * │  CPU寄存器                              │
 * ├─────────────────────────────────────────┤
 * │  CPU缓存 (L1/L2/L3)                     │  <- 易失
 * ├─────────────────────────────────────────┤
 * │  DRAM / 持久内存                        │  <- 持久域边界
 * ├─────────────────────────────────────────┤
 * │  SSD / NVMe                             │
 * └─────────────────────────────────────────┘
 *
 * ADR (Asynchronous DRAM Refresh): 电源故障时将缓存刷入PM
 */

#include <stdint.h>
#include <stdbool.h>
#include <immintrin.h>  /* x86 SIMD指令 */
#include <emmintrin.h>

/* 持久化内存屏障 - 确保前面存储到达持久域 */
static inline void pmem_persist(void) {
    /* SFENCE + CLWB/CLFLUSHOPT */
    _mm_sfence();
}

/* 缓存行刷新指令 (CLFLUSHOPT) */
static inline void pmem_clflushopt(const void *addr) {
    _mm_clflushopt(addr);
}

/* 缓存行回写 (CLWB) - 保留在缓存中 */
static inline void pmem_clwb(const void *addr) {
    /* GCC内建 */
    __builtin_ia32_clwb(addr);
}
