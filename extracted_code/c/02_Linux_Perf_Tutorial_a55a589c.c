/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\02_Linux_Perf_Tutorial.md
 * Line: 1523
 * Language: c
 * Block ID: a55a589c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* perf_integration.c - 在C程序中集成perf */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <asm/unistd.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>

/* 简单的perf封装库 */

typedef struct {
    int fd;
    const char *name;
    uint64_t start_value;
} perf_counter_t;

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

int perf_counter_init(perf_counter_t *counter, uint32_t type, uint64_t config,
                      const char *name) {
    struct perf_event_attr attr;

    memset(&attr, 0, sizeof(attr));
    attr.type = type;
    attr.size = sizeof(attr);
    attr.config = config;
    attr.disabled = 1;
    attr.exclude_kernel = 1;
    attr.exclude_hv = 1;
    attr.read_format = PERF_FORMAT_TOTAL_TIME_ENABLED | PERF_FORMAT_TOTAL_TIME_RUNNING;

    counter->fd = perf_event_open(&attr, 0, -1, -1, 0);
    if (counter->fd == -1) {
        fprintf(stderr, "Failed to open perf event %s: %s\n", name, strerror(errno));
        return -1;
    }

    counter->name = name;
    return 0;
}

void perf_counter_start(perf_counter_t *counter) {
    ioctl(counter->fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(counter->fd, PERF_EVENT_IOC_ENABLE, 0);
}

void perf_counter_stop(perf_counter_t *counter) {
    ioctl(counter->fd, PERF_EVENT_IOC_DISABLE, 0);
}

uint64_t perf_counter_read(perf_counter_t *counter) {
    uint64_t values[3];
    if (read(counter->fd, values, sizeof(values)) == sizeof(values)) {
        return values[0];
    }
    return 0;
}

void perf_counter_close(perf_counter_t *counter) {
    close(counter->fd);
}

/* 性能测量宏 */
#define PERF_MEASURE_START(name) \
    perf_counter_t _perf_##name; \
    perf_counter_init(&_perf_##name, PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES, #name); \
    perf_counter_start(&_perf_##name); \
    printf("[PERF] Starting measurement: %s\n", #name)

#define PERF_MEASURE_END(name) \
    perf_counter_stop(&_perf_##name); \
    uint64_t _perf_##name##_cycles = perf_counter_read(&_perf_##name); \
    printf("[PERF] %s: %" PRIu64 " cycles\n", #name, _perf_##name##_cycles); \
    perf_counter_close(&_perf_##name)

/* 测试函数 */
void matrix_multiply(int n) {
    double *a = aligned_alloc(64, n * n * sizeof(double));
    double *b = aligned_alloc(64, n * n * sizeof(double));
    double *c = aligned_alloc(64, n * n * sizeof(double));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++) {
                sum += a[i * n + k] * b[k * n + j];
            }
            c[i * n + j] = sum;
        }
    }

    free(a); free(b); free(c);
}

void memory_intensive(int n) {
    int *buffer = malloc(n * sizeof(int));
    volatile int sum = 0;

    for (int i = 0; i < n; i++) {
        buffer[i] = i;
    }

    for (int i = 0; i < n; i++) {
        sum += buffer[i];
    }

    free(buffer);
    (void)sum;
}

int main() {
    printf("Perf Integration Demo\n");
    printf("=====================\n\n");

    /* 测量矩阵乘法 */
    {
        perf_counter_t cycles, instr, cache_miss;

        perf_counter_init(&cycles, PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES, "cycles");
        perf_counter_init(&instr, PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS, "instructions");
        perf_counter_init(&cache_miss, PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES, "cache-misses");

        printf("Testing matrix multiplication...\n");

        perf_counter_start(&cycles);
        perf_counter_start(&instr);
        perf_counter_start(&cache_miss);

        matrix_multiply(512);

        perf_counter_stop(&cycles);
        perf_counter_stop(&instr);
        perf_counter_stop(&cache_miss);

        uint64_t c = perf_counter_read(&cycles);
        uint64_t i = perf_counter_read(&instr);
        uint64_t m = perf_counter_read(&cache_miss);

        printf("Results:\n");
        printf("  Cycles:        %" PRIu64 "\n", c);
        printf("  Instructions:  %" PRIu64 "\n", i);
        printf("  Cache misses:  %" PRIu64 "\n", m);
        printf("  CPI:           %.3f\n", (double)c / i);

        perf_counter_close(&cycles);
        perf_counter_close(&instr);
        perf_counter_close(&cache_miss);
    }

    printf("\n");

    /* 使用宏测量 */
    PERF_MEASURE_START(memory_test);
    memory_intensive(10000000);
    PERF_MEASURE_END(memory_test);

    printf("\nDone!\n");
    return 0;
}
