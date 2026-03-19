/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 4474
 * Language: c
 * Block ID: 6a4a9811
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file pebs_analysis.c
 * @brief PEBS精确事件采样分析
 *
 * PEBS在事件发生时自动保存处理器状态
 * 提供精确的指令指针和延迟信息
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

#define PEBS_SAMPLE_SIZE 4096

/**
 * @brief PEBS数据采集
 *
 * 使用PERF_SAMPLE_IP捕获精确指令指针
 * 使用PERF_SAMPLE_ADDR捕获数据地址（用于缓存分析）
 */
void setup_pebs_sampling(void) {
    printf("=== PEBS Sampling Setup ===\n\n");

    struct perf_event_attr pe = {
        .type = PERF_TYPE_HARDWARE,
        .size = sizeof(struct perf_event_attr),
        .config = PERF_COUNT_HW_CACHE_MISSES,
        .sample_period = 10000,        /* 每10000个事件采样一次 */
        .sample_type = PERF_SAMPLE_IP | PERF_SAMPLE_ADDR | PERF_SAMPLE_WEIGHT,
        .disabled = 1,
        .exclude_kernel = 1,
        .precise_ip = 3,               /* 精确IP级别3（PEBS） */
        .wakeup_events = 1
    };

    printf("PEBS configuration:\n");
    printf("  Event: Cache misses\n");
    printf("  Sample period: %d\n", (int)pe.sample_period);
    printf("  Sample types: IP, ADDR, WEIGHT\n");
    printf("  Precise IP: %d (highest precision)\n", pe.precise_ip);
}

/**
 * @brief 使用perf进行PEBS分析
 *
 * 命令行示例：
 * perf record -e cache-misses:pp ./program
 * perf report --sort=symbol
 * perf annotate
 */
void perf_pebs_commands(void) {
    printf("\n=== PEBS perf Commands ===\n\n");

    printf("1. Record cache misses with PEBS:\n");
    printf("   perf record -e cache-misses:pp -c 10000 ./program\n");
    printf("   (:pp = precise, -c = count between samples)\n\n");

    printf("2. Analyze results:\n");
    printf("   perf report --sort=symbol,dso\n\n");

    printf("3. Annotate source:\n");
    printf("   perf annotate --stdio\n\n");

    printf("4. Profile specific function:\n");
    printf("   perf record -e cycles:pp -g -- ./program\n");
    printf("   perf report --call-graph=fractal\n\n");

    printf("5. Memory access profiling:\n");
    printf("   perf mem record ./program\n");
    printf("   perf mem report\n");
}

int main(void) {
    setup_pebs_sampling();
    perf_pebs_commands();
    return 0;
}
