/*
 * Auto-generated from: 16_Embedded_AI_Agents\08_Real_Time_AI\01_Deterministic_Inference.md
 * Line: 240
 * Language: c
 * Block ID: 5c6165f8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用周期计数器测量基本块执行时间
#include <stdint.h>

// ARM DWT周期计数器 (Cortex-M3/M4/M7)
#define DWT_CYCCNT (*((volatile uint32_t*)0xE0001004))
#define DWT_CONTROL (*((volatile uint32_t*)0xE0001000))
#define SCB_DEMCR (*((volatile uint32_t*)0xE000EDFC))

void dwt_init(void) {
    SCB_DEMCR |= 0x01000000;  // Enable DWT
    DWT_CONTROL |= 1;         // Enable cycle counter
}

uint32_t dwt_get_cycles(void) {
    return DWT_CYCCNT;
}

typedef struct {
    uint32_t min_cycles;
    uint32_t max_cycles;
    uint32_t total_cycles;
    uint32_t call_count;
} timing_stats_t;

// 测量函数执行时间
typedef struct {
    uint32_t start;
    timing_stats_t* stats;
} timed_scope_t;

timed_scope_t timed_function_start(timing_stats_t* stats) {
    return (timed_scope_t){dwt_get_cycles(), stats};
}

void timed_function_end(timed_scope_t scope) {
    uint32_t end = dwt_get_cycles();
    uint32_t cycles = end - scope.start;

    timing_stats_t* s = scope.stats;
    if (cycles < s->min_cycles || s->call_count == 0) {
        s->min_cycles = cycles;
    }
    if (cycles > s->max_cycles) {
        s->max_cycles = cycles;
    }
    s->total_cycles += cycles;
    s->call_count++;
}

// 使用示例
static timing_stats_t conv_stats = {0};

void profiled_convolution(...) {
    timed_scope_t scope = timed_function_start(&conv_stats);

    // 执行卷积
    convolution_impl(...);

    timed_function_end(scope);
}
