/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\01_PMU_Architecture.md
 * Line: 673
 * Language: c
 * Block ID: 58cfe18a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 计数器位宽与溢出处理 */

/* Intel 48位计数器溢出时间估算 (3GHz处理器) */
/* 最大计数值: 2^48 = 281,474,976,710,656 */
/* 溢出时间: 281,474,976,710,656 / 3,000,000,000 ≈ 93,825秒 ≈ 26小时 */

/* ARM 64位计数器溢出时间 */
/* 最大计数值: 2^64 = 18,446,744,073,709,551,616 */
/* 溢出时间: 理论上约194年，实际无需处理 */

/* 溢出处理代码示例 */
#include <stdint.h>
#include <signal.h>

static volatile uint64_t overflow_count[8] = {0};
static volatile uint64_t last_value[8] = {0};

/* Intel PMU溢出中断处理 */
void pmu_overflow_handler(int sig, siginfo_t *info, void *context) {
    uint64_t status = rdmsr(IA32_PERF_GLOBAL_STATUS);

    for (int i = 0; i < num_counters; i++) {
        if (status & (1ULL << i)) {
            overflow_count[i]++;
            /* 重置溢出标志 */
            wrmsr(IA32_PERF_GLOBAL_OVF_CTRL, (1ULL << i));
            /* 重新初始化计数器 */
            wrmsr(IA32_PMC0 + i, 0);
        }
    }
}

/* 读取扩展计数器值 */
uint64_t read_extended_counter(int counter_id) {
    uint64_t current = rdpmc(counter_id);
    uint64_t counter_mask = (1ULL << 48) - 1;  /* 48位掩码 */

    /* 检测溢出 */
    if (current < last_value[counter_id]) {
        overflow_count[counter_id]++;
    }
    last_value[counter_id] = current;

    /* 计算扩展值: overflow * 2^48 + current */
    return (overflow_count[counter_id] << 48) | current;
}
