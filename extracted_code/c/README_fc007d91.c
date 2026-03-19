/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\README.md
 * Line: 391
 * Language: c
 * Block ID: fc007d91
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Linux perf_event_open 示例
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <sys/syscall.h>
#include <unistd.h>

long perf_event_open(struct perf_event_attr *hw_event,
                     pid_t pid, int cpu, int group_fd,
                     unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu,
                   group_fd, flags);
}

// 常用性能计数器
// - CPU_CYCLES: 时钟周期
// - INSTRUCTIONS:  retired指令
// - CACHE_REFERENCES/MISSES: 缓存
// - BRANCHES/MISSES: 分支
