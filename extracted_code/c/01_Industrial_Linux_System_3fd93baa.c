/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Industrial_Linux\01_Industrial_Linux_System.md
 * Line: 134
 * Language: c
 * Block ID: 3fd93baa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file preempt_rt_task.c
 * @brief PREEMPT_RT实时任务示例
 * @details 演示如何在PREEMPT_RT内核上创建和管理实时任务
 *
 * 编译: gcc -o preempt_rt_task preempt_rt_task.c -lpthread -lrt
 * 运行: sudo chrt -f 99 ./preempt_rt_task
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <sys/mman.h>

/* 版本信息 */
#define VERSION_MAJOR   1
#define VERSION_MINOR   0
#define VERSION_PATCH   0

/* 实时任务配置 */
#define RT_PRIORITY         80          /* 实时优先级 (1-99) */
#define RT_INTERVAL_US      1000        /* 任务周期: 1ms */
#define MAX_ITERATIONS      10000       /* 最大迭代次数 */
#define WARMUP_ITERATIONS   100         /* 预热迭代次数 */

/* 统计信息结构 */
typedef struct {
    long long min_latency;      /* 最小延迟 (ns) */
    long long max_latency;      /* 最大延迟 (ns) */
    long long avg_latency;      /* 平均延迟 (ns) */
    long long jitter;           /* 抖动 (ns) */
    long long deadline_misses;  /*  deadline错过次数 */
} rt_stats_t;

/* 任务上下文 */
typedef struct {
    int task_id;                /* 任务ID */
    int priority;               /* 任务优先级 */
    int interval_us;            /* 任务周期 (微秒) */
    int running;                /* 运行标志 */
    rt_stats_t stats;           /* 统计信息 */
    pthread_t thread;           /* 线程句柄 */
} rt_task_t;

/* 全局变量 */
static volatile int g_running = 1;
static long long g_latencies[MAX_ITERATIONS];
static int g_latency_count = 0;

/**
 * @brief 获取当前时间 (纳秒)
 * @return 纳秒级时间戳
 */
static inline long long get_time_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

/**
 * @brief 设置线程的CPU亲和性
 * @param thread 线程ID
 * @param cpu_id CPU核心ID
 * @return 成功返回0，失败返回错误码
 */
static int set_thread_affinity(pthread_t thread, int cpu_id)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);

    return pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
}

/**
 * @brief 设置线程为实时调度策略
 * @param priority 实时优先级 (1-99)
 * @return 成功返回0，失败返回错误码
 */
static int set_realtime_priority(int priority)
{
    struct sched_param param;
    param.sched_priority = priority;

    /* 设置SCHED_FIFO实时调度策略 */
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        perror("sched_setscheduler failed");
        return -1;
    }

    return 0;
}

/**
 * @brief 锁定内存防止页面交换
 * @return 成功返回0，失败返回-1
 */
static int lock_memory(void)
{
    /* 锁定当前和未来的内存页面 */
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        perror("mlockall failed");
        return -1;
    }

    printf("[INFO] Memory locked successfully\n");
    return 0;
}

/**
 * @brief 信号处理函数
 * @param sig 信号编号
 */
static void signal_handler(int sig)
{
    (void)sig;
    printf("\n[INFO] Received signal, stopping...\n");
    g_running = 0;
}

/**
 * @brief 计算统计信息
 * @param task 任务上下文
 */
static void calculate_statistics(rt_task_t *task)
{
    long long sum = 0;
    long long min_val = g_latencies[0];
    long long max_val = g_latencies[0];

    for (int i = WARMUP_ITERATIONS; i < g_latency_count; i++) {
        long long val = g_latencies[i];
        sum += val;
        if (val < min_val) min_val = val;
        if (val > max_val) max_val = val;
    }

    int count = g_latency_count - WARMUP_ITERATIONS;
    task->stats.min_latency = min_val;
    task->stats.max_latency = max_val;
    task->stats.avg_latency = count > 0 ? sum / count : 0;
    task->stats.jitter = max_val - min_val;
}

/**
 * @brief 实时任务主函数
 * @param arg 任务参数
 * @return NULL
 */
static void *rt_task_function(void *arg)
{
    rt_task_t *task = (rt_task_t *)arg;
    struct timespec next_time;
    long long interval_ns = task->interval_us * 1000LL;

    printf("[TASK%d] Real-time task started, priority=%d, interval=%d us\n",
           task->task_id, task->priority, task->interval_us);

    /* 设置实时优先级 */
    if (set_realtime_priority(task->priority) != 0) {
        fprintf(stderr, "[TASK%d] Failed to set real-time priority\n", task->task_id);
        return NULL;
    }

    /* 设置CPU亲和性 (绑定到CPU 0) */
    if (set_thread_affinity(pthread_self(), 0) != 0) {
        fprintf(stderr, "[TASK%d] Failed to set CPU affinity\n", task->task_id);
    }

    /* 获取当前时间 */
    clock_gettime(CLOCK_MONOTONIC, &next_time);

    int iteration = 0;
    task->running = 1;

    while (g_running && iteration < MAX_ITERATIONS) {
        /* 记录开始时间 */
        long long start_ns = get_time_ns();

        /* ==================== 实时任务工作区 ==================== */

        /* 模拟控制算法计算 (约100μs工作量) */
        volatile double result = 0.0;
        for (int i = 0; i < 1000; i++) {
            result += i * 0.001;
        }

        /* 模拟I/O操作 */
        static uint32_t counter = 0;
        counter++;

        /* ====================================================== */

        /* 记录结束时间并计算延迟 */
        long long end_ns = get_time_ns();
        long long latency = end_ns - start_ns;

        /* 存储延迟数据 (跳过预热阶段) */
        if (iteration >= WARMUP_ITERATIONS && g_latency_count < MAX_ITERATIONS) {
            g_latencies[g_latency_count++] = latency;
        }

        /* 计算下一个唤醒时间 */
        next_time.tv_nsec += interval_ns;
        while (next_time.tv_nsec >= 1000000000L) {
            next_time.tv_nsec -= 1000000000L;
            next_time.tv_sec++;
        }

        /* 绝对时间睡眠，确保精确周期 */
        int ret = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time, NULL);
        if (ret != 0) {
            if (ret == EINTR) {
                printf("[TASK%d] Sleep interrupted\n", task->task_id);
            }
        }

        iteration++;
    }

    task->running = 0;

    /* 计算统计信息 */
    calculate_statistics(task);

    printf("[TASK%d] Task completed after %d iterations\n", task->task_id, iteration);

    return NULL;
}

/**
 * @brief 打印统计信息
 * @param task 任务上下文
 */
static void print_statistics(const rt_task_t *task)
{
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║           Real-Time Task Statistics Report               ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ Task ID:        %d                                       ║\n", task->task_id);
    printf("║ Priority:       %d                                       ║\n", task->priority);
    printf("║ Interval:       %d us                                    ║\n", task->interval_us);
    printf("║ Sample Count:   %d                                       ║\n", g_latency_count - WARMUP_ITERATIONS);
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ Min Latency:    %8.3f us                              ║\n", task->stats.min_latency / 1000.0);
    printf("║ Max Latency:    %8.3f us                              ║\n", task->stats.max_latency / 1000.0);
    printf("║ Avg Latency:    %8.3f us                              ║\n", task->stats.avg_latency / 1000.0);
    printf("║ Jitter:         %8.3f us                              ║\n", task->stats.jitter / 1000.0);
    printf("║ Deadline Miss:  %lld                                      ║\n", task->stats.deadline_misses);
    printf("╚══════════════════════════════════════════════════════════╝\n");
}

/**
 * @brief 检查系统是否支持PREEMPT_RT
 * @return 支持返回1，否则返回0
 */
static int check_preempt_rt_support(void)
{
    FILE *fp = fopen("/sys/kernel/realtime", "r");
    if (fp != NULL) {
        int realtime = 0;
        fscanf(fp, "%d", &realtime);
        fclose(fp);
        return realtime;
    }

    /* 检查内核版本字符串 */
    fp = fopen("/proc/version", "r");
    if (fp != NULL) {
        char version[256];
        if (fgets(version, sizeof(version), fp) != NULL) {
            fclose(fp);
            return strstr(version, "PREEMPT_RT") != NULL;
        }
        fclose(fp);
    }

    return 0;
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    rt_task_t task = {0};

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║     Industrial Linux Real-Time Task Demo v%d.%d.%d          ║\n",
           VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* 检查PREEMPT_RT支持 */
    if (check_preempt_rt_support()) {
        printf("[INFO] PREEMPT_RT kernel detected ✓\n");
    } else {
        printf("[WARNING] PREEMPT_RT kernel not detected\n");
        printf("[WARNING] Real-time performance may be limited\n");
    }

    /* 注册信号处理 */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* 锁定内存 */
    if (lock_memory() != 0) {
        fprintf(stderr, "[ERROR] Failed to lock memory\n");
        return 1;
    }

    /* 初始化任务参数 */
    task.task_id = 1;
    task.priority = RT_PRIORITY;
    task.interval_us = RT_INTERVAL_US;

    /* 创建实时任务 */
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    /* 设置线程为分离状态 */
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    if (pthread_create(&task.thread, &attr, rt_task_function, &task) != 0) {
        perror("pthread_create failed");
        return 1;
    }

    pthread_attr_destroy(&attr);

    /* 等待任务完成 */
    pthread_join(task.thread, NULL);

    /* 打印统计信息 */
    print_statistics(&task);

    /* 解锁内存 */
    munlockall();

    printf("\n[INFO] Program completed successfully\n");

    return 0;
}
