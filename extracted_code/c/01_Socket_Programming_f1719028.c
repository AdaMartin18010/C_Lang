/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 3969
 * Language: c
 * Block ID: f1719028
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * perf_monitor.c - 性能监控工具
 */
#include <time.h>
#include <stdio.h>

// 高精度计时器
typedef struct {
    struct timespec start;
    struct timespec end;
} timer_t;

void timer_start(timer_t *t) {
    clock_gettime(CLOCK_MONOTONIC, &t->start);
}

double timer_elapsed_us(timer_t *t) {
    clock_gettime(CLOCK_MONOTONIC, &t->end);
    return (t->end.tv_sec - t->start.tv_sec) * 1e6 +
           (t->end.tv_nsec - t->start.tv_nsec) / 1e3;
}

// 性能指标统计
typedef struct {
    unsigned long total_requests;
    unsigned long total_bytes_sent;
    unsigned long total_bytes_recv;
    double total_latency_us;
    unsigned long error_count;
    time_t start_time;
} perf_stats_t;

void print_stats(perf_stats_t *stats) {
    time_t elapsed = time(NULL) - stats->start_time;
    if (elapsed == 0) return;

    printf("========== Performance Statistics ==========\n");
    printf("Runtime: %ld seconds\n", elapsed);
    printf("Total Requests: %lu\n", stats->total_requests);
    printf("Requests/sec: %.2f\n", (double)stats->total_requests / elapsed);
    printf("Avg Latency: %.2f us\n",
           stats->total_requests ? stats->total_latency_us / stats->total_requests : 0);
    printf("Error Rate: %.4f%%\n",
           stats->total_requests ? 100.0 * stats->error_count / stats->total_requests : 0);
    printf("Throughput: %.2f MB/s\n",
           (double)(stats->total_bytes_sent + stats->total_bytes_recv) / elapsed / 1024 / 1024);
    printf("============================================\n");
}

// Linux性能监控: /proc/net/sockstat
define PROC_SOCKSTAT "/proc/net/sockstat"

void monitor_sockets() {
    FILE *fp = fopen(PROC_SOCKSTAT, "r");
    if (!fp) return;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
}

// 连接状态统计
void monitor_tcp_states() {
    system("netstat -an | awk '/^tcp/ {++S[$NF]} END {for(a in S) print a, S[a]}'");
}
