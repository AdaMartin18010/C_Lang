/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 4249
 * Language: c
 * Block ID: e1092a7b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file pmc_analysis.c
 * @brief 性能监控计数器（PMC）使用指南
 *
 * PMC提供CPU内部事件计数：
 * - 时钟周期、指令 retired
 * - 缓存命中/未命中
 * - 分支预测成功/失败
 * - TLB miss
 * - 等等
 *
 * Linux接口：/proc/sys/kernel/perf_event_paranoid
 * 工具：perf, PAPI库
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <asm/unistd.h>
#include <time.h>

/* 系统调用包装 */
static long perf_event_open(struct perf_event_attr *hw_event,
                            pid_t pid, int cpu, int group_fd,
                            unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

/* 配置PMC事件 */
struct perf_event_attr create_event_config(int type, unsigned long config) {
    struct perf_event_attr pe = {
        .type = type,
        .size = sizeof(struct perf_event_attr),
        .config = config,
        .disabled = 1,
        .exclude_kernel = 1,
        .exclude_hv = 1
    };
    return pe;
}

/**
 * @brief 测量基本性能指标
 */
void measure_basic_metrics(void) {
    printf("=== Basic PMC Metrics ===\n\n");

    /* 配置事件：CPU周期 */
    struct perf_event_attr pe_cycles = create_event_config(
        PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES);

    /* 配置事件：指令数 */
    struct perf_event_attr pe_instructions = create_event_config(
        PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS);

    /* 配置事件：缓存引用 */
    struct perf_event_attr pe_cache_ref = create_event_config(
        PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_REFERENCES);

    /* 配置事件：缓存未命中 */
    struct perf_event_attr pe_cache_miss = create_event_config(
        PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES);

    /* 打开事件 */
    int fd_cycles = perf_event_open(&pe_cycles, 0, -1, -1, 0);
    int fd_instr = perf_event_open(&pe_instructions, 0, -1, -1, 0);
    int fd_cache_ref = perf_event_open(&pe_cache_ref, 0, -1, -1, 0);
    int fd_cache_miss = perf_event_open(&pe_cache_miss, 0, -1, -1, 0);

    if (fd_cycles < 0 || fd_instr < 0) {
        printf("perf_event_open failed (try running as root or adjust perf_event_paranoid)\n");
        return;
    }

    /* 启动测量 */
    ioctl(fd_cycles, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_instr, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_cache_ref, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_cache_miss, PERF_EVENT_IOC_RESET, 0);

    ioctl(fd_cycles, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(fd_instr, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(fd_cache_ref, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(fd_cache_miss, PERF_EVENT_IOC_ENABLE, 0);

    /* 执行被测代码 */
    const int N = 100000000;
    volatile long long sum = 0;
    for (int i = 0; i < N; i++) {
        sum += i;
    }
    (void)sum;

    /* 停止测量 */
    ioctl(fd_cycles, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(fd_instr, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(fd_cache_ref, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(fd_cache_miss, PERF_EVENT_IOC_DISABLE, 0);

    /* 读取结果 */
    long long cycles, instructions, cache_refs, cache_misses;
    read(fd_cycles, &cycles, sizeof(cycles));
    read(fd_instr, &instructions, sizeof(instructions));
    read(fd_cache_ref, &cache_refs, sizeof(cache_refs));
    read(fd_cache_miss, &cache_misses, sizeof(cache_misses));

    /* 计算指标 */
    printf("Execution metrics:\n");
    printf("  CPU cycles:      %lld\n", cycles);
    printf("  Instructions:    %lld\n", instructions);
    printf("  IPC:             %.2f\n", (double)instructions / cycles);
    printf("  CPI:             %.2f\n", (double)cycles / instructions);
    printf("  Cache refs:      %lld\n", cache_refs);
    printf("  Cache misses:    %lld\n", cache_misses);
    printf("  Cache miss rate: %.2f%%\n",
           cache_refs > 0 ? 100.0 * cache_misses / cache_refs : 0);

    close(fd_cycles);
    close(fd_instr);
    close(fd_cache_ref);
    close(fd_cache_miss);
}

/**
 * @brief 测量分支预测
 */
void measure_branch_prediction(void) {
    printf("\n=== Branch Prediction Metrics ===\n\n");

    struct perf_event_attr pe_branches = create_event_config(
        PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS);
    struct perf_event_attr pe_branch_miss = create_event_config(
        PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_MISSES);

    int fd_br = perf_event_open(&pe_branches, 0, -1, -1, 0);
    int fd_miss = perf_event_open(&pe_branch_miss, 0, -1, -1, 0);

    if (fd_br < 0) {
        printf("Branch events not available\n");
        return;
    }

    /* 有序数据（预测成功率高） */
    const int N = 10000000;
    int *ordered = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        ordered[i] = i % 100;
    }

    ioctl(fd_br, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_miss, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_br, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(fd_miss, PERF_EVENT_IOC_ENABLE, 0);

    volatile long long sum = 0;
    for (int i = 0; i < N; i++) {
        if (ordered[i] < 50) {
            sum += ordered[i];
        }
    }

    ioctl(fd_br, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(fd_miss, PERF_EVENT_IOC_DISABLE, 0);

    long long branches, misses;
    read(fd_br, &branches, sizeof(branches));
    read(fd_miss, &misses, sizeof(misses));

    printf("Ordered data:\n");
    printf("  Branches:       %lld\n", branches);
    printf("  Mispredictions: %lld\n", misses);
    printf("  Miss rate:      %.2f%%\n",
           100.0 * misses / branches);

    /* 随机数据（预测失败率高） */
    srand(42);
    int *random = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        random[i] = rand() % 100;
    }

    ioctl(fd_br, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_miss, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_br, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(fd_miss, PERF_EVENT_IOC_ENABLE, 0);

    sum = 0;
    for (int i = 0; i < N; i++) {
        if (random[i] < 50) {
            sum += random[i];
        }
    }

    ioctl(fd_br, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(fd_miss, PERF_EVENT_IOC_DISABLE, 0);

    read(fd_br, &branches, sizeof(branches));
    read(fd_miss, &misses, sizeof(misses));

    printf("\nRandom data:\n");
    printf("  Branches:       %lld\n", branches);
    printf("  Mispredictions: %lld\n", misses);
    printf("  Miss rate:      %.2f%%\n",
           100.0 * misses / branches);

    free(ordered);
    free(random);
    close(fd_br);
    close(fd_miss);
}

int main(void) {
    measure_basic_metrics();
    measure_branch_prediction();
    return 0;
}
