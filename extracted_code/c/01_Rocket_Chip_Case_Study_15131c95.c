/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\01_Rocket_Chip_Case_Study.md
 * Line: 797
 * Language: c
 * Block ID: 15131c95
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// perf_counters.h - 硬件性能计数器
#ifndef PERF_COUNTERS_H
#define PERF_COUNTERS_H

#include <stdint.h>

// RISC-V标准性能计数器CSR
#define CSR_CYCLE       0xC00
#define CSR_TIME        0x01
#define CSR_INSTRET     0xC02
#define CSR_CYCLEH      0xC80
#define CSR_TIMEH       0xC81
#define CSR_INSTRETH    0xC82

// 事件选择器
#define PERF_EVENT_CYCLES           0
#define PERF_EVENT_INSTRUCTIONS     1
#define PERF_EVENT_CACHE_REFERENCES 2
#define PERF_EVENT_CACHE_MISSES     3
#define PERF_EVENT_BRANCHES         4
#define PERF_EVENT_BRANCH_MISSES    5
#define PERF_EVENT_BUS_CYCLES       6

// 读取64位周期计数器
static inline uint64_t read_cycle(void) {
    uint32_t cycleh, cyclel;
    asm volatile (
        "csrr %0, cycleh\n"
        "csrr %1, cycle\n"
        : "=r"(cycleh), "=r"(cyclel)
    );
    return ((uint64_t)cycleh << 32) | cyclel;
}

static inline uint64_t read_instret(void) {
    uint32_t instreth, instretl;
    asm volatile (
        "csrr %0, instreth\n"
        "csrr %1, instret\n"
        : "=r"(instreth), "=r"(instretl)
    );
    return ((uint64_t)instreth << 32) | instretl;
}

// 性能测量宏
#define PERF_START() \
    uint64_t _perf_start_cycle = read_cycle(); \
    uint64_t _perf_start_inst = read_instret()

#define PERF_END(name) do { \
    uint64_t _perf_end_cycle = read_cycle(); \
    uint64_t _perf_end_inst = read_instret(); \
    printf("%s: cycles=%lu, insts=%lu, CPI=%.2f\n", \
           name, \
           _perf_end_cycle - _perf_start_cycle, \
           _perf_end_inst - _perf_start_inst, \
           (double)(_perf_end_cycle - _perf_start_cycle) / \
           (_perf_end_inst - _perf_start_inst)); \
} while(0)

#endif
