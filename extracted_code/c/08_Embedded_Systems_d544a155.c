/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 1172
 * Language: c
 * Block ID: d544a155
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// profiling.c

#include "core_cm4.h"

void DWT_Init(void) {
    // 使能DWT
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;           // 清零计数器
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;  // 使能
}

static inline uint32_t DWT_GetCycles(void) {
    return DWT->CYCCNT;
}

// 测量函数执行时间
void measure_function(void) {
    uint32_t start = DWT_GetCycles();

    // 被测代码
    function_to_measure();

    uint32_t end = DWT_GetCycles();
    uint32_t cycles = end - start;

    printf("Function took %u cycles\n", cycles);
}

// 更精确的测量 (避免缓存影响)
#define MEASURE_START() \
    DWT->CYCCNT = 0; \
    __DSB(); \
    uint32_t _start = DWT->CYCCNT

#define MEASURE_END(msg) do { \
    __DSB(); \
    uint32_t _end = DWT->CYCCNT; \
    printf(msg ": %u cycles\n", _end - _start); \
} while(0)
