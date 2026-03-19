/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\01_PMU_Architecture.md
 * Line: 953
 * Language: c
 * Block ID: 9619093b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* pmu_perf.c - 使用perf_event_open的PMU接口 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <asm/unistd.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>

/* 对于旧版本内核可能没有定义 */
#ifndef PERF_COUNT_HW_REF_CPU_CYCLES
#define PERF_COUNT_HW_REF_CPU_CYCLES 9
#endif

/* perf_event_open系统调用封装 */
static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

/* PMU事件配置结构体 */
typedef struct {
    const char *name;
    uint32_t type;
    uint64_t config;
    const char *description;
} pmu_event_t;

/* 通用硬件事件表 */
static const pmu_event_t hw_events[] = {
    {"cycles",          PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES,       "CPU周期"},
    {"instructions",    PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS,     "退役指令"},
    {"cache-references",PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_REFERENCES, "缓存引用"},
    {"cache-misses",    PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES,     "缓存未命中"},
    {"branches",        PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS, "分支指令"},
    {"branch-misses",   PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_MISSES,    "分支预测失败"},
    {"bus-cycles",      PERF_TYPE_HARDWARE, PERF_COUNT_HW_BUS_CYCLES,       "总线周期"},
    {"ref-cycles",      PERF_TYPE_HARDWARE, PERF_COUNT_HW_REF_CPU_CYCLES,   "参考周期"},
    {NULL, 0, 0, NULL}
};

/* 缓存事件配置表 (L1D) */
static const pmu_event_t cache_events_l1d[] = {
    {"L1-dcache-loads",       PERF_TYPE_HW_CACHE,
     (PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_READ << 8) |
      (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16)), "L1数据缓存加载"},
    {"L1-dcache-load-misses", PERF_TYPE_HW_CACHE,
     (PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_READ << 8) |
      (PERF_COUNT_HW_CACHE_RESULT_MISS << 16)), "L1数据缓存加载未命中"},
    {"L1-dcache-stores",      PERF_TYPE_HW_CACHE,
     (PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_WRITE << 8) |
      (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16)), "L1数据缓存存储"},
    {"L1-dcache-store-misses",PERF_TYPE_HW_CACHE,
     (PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_WRITE << 8) |
      (PERF_COUNT_HW_CACHE_RESULT_MISS << 16)), "L1数据缓存存储未命中"},
    {NULL, 0, 0, NULL}
};

/* PMU会话结构体 */
typedef struct {
    int fd;
    const char *name;
    uint64_t id;
} pmu_counter_t;

typedef struct {
    pmu_counter_t *counters;
    int num_counters;
    int group_leader;
} pmu_session_t;

/* 初始化性能事件属性 */
static void init_event_attr(struct perf_event_attr *attr, uint32_t type,
                            uint64_t config) {
    memset(attr, 0, sizeof(struct perf_event_attr));
    attr->type = type;
    attr->size = sizeof(struct perf_event_attr);
    attr->config = config;

    /* 配置选项 */
    attr->disabled = 1;           /* 初始禁用 */
    attr->exclude_kernel = 0;     /* 包含内核态 */
    attr->exclude_hv = 1;         /* 排除虚拟机监控器 */
    attr->read_format = PERF_FORMAT_TOTAL_TIME_ENABLED |
                        PERF_FORMAT_TOTAL_TIME_RUNNING |
                        PERF_FORMAT_ID;

    /* 启用精确事件 (如果支持) */
    attr->precise_ip = 2;         /* 请求精确IP */
}

/* 打开PMU计数器 */
int pmu_open_counter(pmu_counter_t *counter, uint32_t type, uint64_t config,
                     int group_fd, const char *name) {
    struct perf_event_attr attr;

    init_event_attr(&attr, type, config);

    counter->fd = perf_event_open(&attr, 0, -1, group_fd, 0);
    if (counter->fd == -1) {
        fprintf(stderr, "Error opening %s: %s\n", name, strerror(errno));
        return -1;
    }

    counter->name = name;

    /* 获取事件ID */
    if (ioctl(counter->fd, PERF_EVENT_IOC_ID, &counter->id) == -1) {
        perror("ioctl PERF_EVENT_IOC_ID");
        close(counter->fd);
        return -1;
    }

    return 0;
}

/* 读取计数器值 */
uint64_t pmu_read_value(pmu_counter_t *counter) {
    uint64_t values[3];  /* value, enabled, running */

    if (read(counter->fd, values, sizeof(values)) != sizeof(values)) {
        perror("read counter");
        return 0;
    }

    /* 计算归一化值 (考虑多路复用) */
    if (values[2] > 0) {
        return (uint64_t)((double)values[0] * values[1] / values[2]);
    }
    return values[0];
}

/* 读取原始计数器值 (不归一化) */
void pmu_read_raw(pmu_counter_t *counter, uint64_t *value,
                  uint64_t *enabled, uint64_t *running) {
    uint64_t values[3];

    if (read(counter->fd, values, sizeof(values)) == sizeof(values)) {
        *value = values[0];
        *enabled = values[1];
        *running = values[2];
    }
}

/* 启动计数器 */
void pmu_start(pmu_counter_t *counter) {
    ioctl(counter->fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(counter->fd, PERF_EVENT_IOC_ENABLE, 0);
}

/* 停止计数器 */
void pmu_stop(pmu_counter_t *counter) {
    ioctl(counter->fd, PERF_EVENT_IOC_DISABLE, 0);
}

/* 关闭计数器 */
void pmu_close(pmu_counter_t *counter) {
    close(counter->fd);
}

/* ==================== 测试代码 ==================== */

/* 内存密集型测试函数 */
void memory_intensive_test(void) {
    const int SIZE = 1024 * 1024 * 100;  /* 100MB */
    char *buffer = malloc(SIZE);
    volatile int sum = 0;

    if (!buffer) {
        perror("malloc");
        return;
    }

    /* 写入测试 */
    for (int i = 0; i < SIZE; i += 64) {
        buffer[i] = i & 0xFF;
    }

    /* 读取测试 */
    for (int i = 0; i < SIZE; i += 64) {
        sum += buffer[i];
    }

    free(buffer);
}

/* 分支预测测试函数 */
void branch_test(int pattern) {
    volatile int sum = 0;
    const int ITERATIONS = 100000000;

    for (int i = 0; i < ITERATIONS; i++) {
        if (pattern == 0) {
            /* 完全可预测的分支 */
            if (i >= 0) {
                sum++;
            }
        } else if (pattern == 1) {
            /* 随机分支 */
            if (rand() % 2) {
                sum++;
            }
        } else {
            /* 交替分支 */
            if (i & 1) {
                sum++;
            }
        }
    }
}

/* 计算密集型测试 */
uint64_t cpu_intensive_test(uint64_t n) {
    uint64_t sum = 0;
    for (uint64_t i = 0; i < n; i++) {
        sum += i * i;
        sum ^= (sum >> 17);
    }
    return sum;
}

/* 打印计数器结果 */
void print_results(pmu_counter_t *counters, int num_counters,
                   const char *test_name) {
    printf("\n========== %s ==========\n", test_name);
    printf("%-20s %20s %15s %15s %10s\n",
           "Event", "Value", "Enabled(ns)", "Running(ns)", "Ratio");
    printf("%s\n", "--------------------------------------------------------------------------------");

    uint64_t cycles_value = 0;
    uint64_t instr_value = 0;

    for (int i = 0; i < num_counters; i++) {
        uint64_t value, enabled, running;
        pmu_read_raw(&counters[i], &value, &enabled, &running);

        double ratio = (enabled > 0) ? ((double)running / enabled) : 1.0;

        printf("%-20s %20" PRIu64 " %15" PRIu64 " %15" PRIu64 " %9.2f%%\n",
               counters[i].name, value, enabled, running, ratio * 100);

        if (strcmp(counters[i].name, "cycles") == 0) {
            cycles_value = value;
        } else if (strcmp(counters[i].name, "instructions") == 0) {
            instr_value = value;
        }
    }

    /* 计算CPI */
    if (instr_value > 0) {
        printf("\n性能指标:\n");
        printf("  CPI (Cycles Per Instruction): %.3f\n",
               (double)cycles_value / instr_value);
        printf("  IPC (Instructions Per Cycle): %.3f\n",
               (double)instr_value / cycles_value);
    }
}

int main(int argc, char *argv[]) {
    pmu_counter_t counters[6];
    int num_counters = 0;

    printf("PMU性能监控测试程序\n");
    printf("===================\n\n");

    /* 打开硬件计数器 */
    printf("初始化PMU计数器...\n");

    /* 基础事件 */
    if (pmu_open_counter(&counters[num_counters],
                         hw_events[0].type, hw_events[0].config,
                         -1, hw_events[0].name) == 0) {
        num_counters++;
    }

    if (pmu_open_counter(&counters[num_counters],
                         hw_events[1].type, hw_events[1].config,
                         counters[0].fd, hw_events[1].name) == 0) {
        num_counters++;
    }

    if (pmu_open_counter(&counters[num_counters],
                         hw_events[2].type, hw_events[2].config,
                         counters[0].fd, hw_events[2].name) == 0) {
        num_counters++;
    }

    if (pmu_open_counter(&counters[num_counters],
                         hw_events[3].type, hw_events[3].config,
                         counters[0].fd, hw_events[3].name) == 0) {
        num_counters++;
    }

    if (pmu_open_counter(&counters[num_counters],
                         hw_events[4].type, hw_events[4].config,
                         counters[0].fd, hw_events[4].name) == 0) {
        num_counters++;
    }

    if (pmu_open_counter(&counters[num_counters],
                         hw_events[5].type, hw_events[5].config,
                         counters[0].fd, hw_events[5].name) == 0) {
        num_counters++;
    }

    printf("成功打开 %d 个计数器\n\n", num_counters);

    /* 测试1: 内存密集型工作负载 */
    printf("运行测试1: 内存密集型工作负载\n");
    pmu_start(&counters[0]);
    memory_intensive_test();
    pmu_stop(&counters[0]);
    print_results(counters, num_counters, "内存密集型测试");

    /* 重置计数器 */
    for (int i = 0; i < num_counters; i++) {
        pmu_start(&counters[i]);
    }

    /* 测试2: 分支预测测试 - 可预测 */
    printf("\n运行测试2: 分支预测测试 (可预测模式)\n");
    pmu_start(&counters[0]);
    branch_test(0);
    pmu_stop(&counters[0]);
    print_results(counters, num_counters, "分支预测测试(可预测)");

    /* 测试3: 分支预测测试 - 随机 */
    for (int i = 0; i < num_counters; i++) {
        pmu_start(&counters[i]);
    }
    printf("\n运行测试3: 分支预测测试 (随机模式)\n");
    pmu_start(&counters[0]);
    branch_test(1);
    pmu_stop(&counters[0]);
    print_results(counters, num_counters, "分支预测测试(随机)");

    /* 测试4: CPU密集型 */
    for (int i = 0; i < num_counters; i++) {
        pmu_start(&counters[i]);
    }
    printf("\n运行测试4: CPU密集型工作负载\n");
    pmu_start(&counters[0]);
    volatile uint64_t result = cpu_intensive_test(1000000000ULL);
    (void)result;  /* 抑制未使用警告 */
    pmu_stop(&counters[0]);
    print_results(counters, num_counters, "CPU密集型测试");

    /* 清理 */
    for (int i = 0; i < num_counters; i++) {
        pmu_close(&counters[i]);
    }

    printf("\n测试完成!\n");
    return 0;
}
