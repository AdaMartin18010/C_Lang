/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RaspberryPi\01_RaspberryPi_Embedded_Linux.md
 * Line: 1504
 * Language: c
 * Block ID: 3efe27a3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// rt_latency_test.c - 实时性测试
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sched.h>
#include <pthread.h>
#include <sys/mman.h>

#define NSEC_PER_SEC    1000000000ULL
#define INTERVAL_NS     1000000ULL  /* 1ms周期 */

static volatile int running = 1;

static inline unsigned long long timespec_to_ns(struct timespec *ts)
{
    return (unsigned long long)ts->tv_sec * NSEC_PER_SEC + ts->tv_nsec;
}

static void signal_handler(int sig)
{
    (void)sig;
    running = 0;
}

int setup_realtime(int priority)
{
    struct sched_param param;
    cpu_set_t cpuset;

    /* 锁定内存 */
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        perror("mlockall failed");
        return -1;
    }

    /* 绑定到CPU0 */
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    if (sched_setaffinity(0, sizeof(cpuset), &cpuset) == -1) {
        perror("sched_setaffinity failed");
        return -1;
    }

    /* 设置SCHED_FIFO调度 */
    param.sched_priority = priority;
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        perror("sched_setscheduler failed (try running as root)");
        return -1;
    }

    return 0;
}

void latency_test(void)
{
    struct timespec next, now, diff;
    unsigned long long min_latency = -1ULL, max_latency = 0;
    unsigned long long sum_latency = 0;
    unsigned long long latency;
    int iterations = 0;
    int max_iterations = 10000;

    printf("=== Real-time Latency Test ===\n");
    printf("Interval: %llu ns (%.2f kHz)\n", INTERVAL_NS,
           NSEC_PER_SEC / (double)INTERVAL_NS / 1000);
    printf("Running %d iterations...\n\n", max_iterations);

    clock_gettime(CLOCK_MONOTONIC, &next);

    while (running && iterations < max_iterations) {
        /* 计算下一个唤醒时间 */
        next.tv_nsec += INTERVAL_NS;
        if (next.tv_nsec >= NSEC_PER_SEC) {
            next.tv_sec++;
            next.tv_nsec -= NSEC_PER_SEC;
        }

        /* 等待 */
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);

        /* 测量延迟 */
        clock_gettime(CLOCK_MONOTONIC, &now);

        if (now.tv_sec > next.tv_sec ||
            (now.tv_sec == next.tv_sec && now.tv_nsec > next.tv_nsec)) {
            diff.tv_sec = now.tv_sec - next.tv_sec;
            diff.tv_nsec = now.tv_nsec - next.tv_nsec;
        } else {
            diff.tv_sec = 0;
            diff.tv_nsec = 0;
        }

        latency = timespec_to_ns(&diff);

        if (latency < min_latency) min_latency = latency;
        if (latency > max_latency) max_latency = latency;
        sum_latency += latency;
        iterations++;
    }

    printf("Results:\n");
    printf("  Iterations: %d\n", iterations);
    printf("  Min latency: %llu ns (%.2f us)\n", min_latency, min_latency / 1000.0);
    printf("  Max latency: %llu ns (%.2f us)\n", max_latency, max_latency / 1000.0);
    printf("  Avg latency: %llu ns (%.2f us)\n",
           sum_latency / iterations, (sum_latency / iterations) / 1000.0);
    printf("  Jitter: %llu ns (%.2f us)\n",
           max_latency - min_latency, (max_latency - min_latency) / 1000.0);
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    printf("Setting up real-time scheduling...\n");
    if (setup_realtime(80) < 0) {
        printf("Continuing without real-time scheduling...\n");
    }

    latency_test();

    return 0;
}
