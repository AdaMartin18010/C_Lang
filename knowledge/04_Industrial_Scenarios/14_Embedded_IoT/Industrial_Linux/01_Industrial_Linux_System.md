# 工业Linux系统开发指南

## 目录

- [工业Linux系统开发指南](#工业linux系统开发指南)
  - [目录](#目录)
  - [1. 工业Linux概述](#1-工业linux概述)
    - [1.1 工业Linux的特点](#11-工业linux的特点)
    - [1.2 主流工业Linux方案](#12-主流工业linux方案)
    - [1.3 系统架构](#13-系统架构)
  - [2. 实时Linux(Xenomai/PREEMPT\_RT)](#2-实时linuxxenomaipreempt_rt)
    - [2.1 PREEMPT\_RT补丁](#21-preempt_rt补丁)
    - [2.2 实时任务编程示例](#22-实时任务编程示例)
    - [2.3 Xenomai实时编程](#23-xenomai实时编程)
  - [3. EtherCAT主站开发](#3-ethercat主站开发)
    - [3.1 EtherCAT协议概述](#31-ethercat协议概述)
    - [3.2 IgH EtherCAT主站示例](#32-igh-ethercat主站示例)
  - [4. PLC运行时(软PLC)](#4-plc运行时软plc)
    - [4.1 软PLC架构](#41-软plc架构)
    - [4.2 迷你软PLC运行时实现](#42-迷你软plc运行时实现)
  - [5. OPC UA服务器实现](#5-opc-ua服务器实现)
    - [5.1 OPC UA概述](#51-opc-ua概述)

---

## 1. 工业Linux概述

### 1.1 工业Linux的特点

工业Linux是专门为工业控制场景优化的Linux发行版或内核配置，具备以下特点：

| 特性 | 说明 | 重要性 |
|------|------|--------|
| 实时性 | 微秒级响应延迟 | ★★★★★ |
| 稳定性 | 7×24小时连续运行 | ★★★★★ |
| 确定性 | 任务执行时间可预测 | ★★★★☆ |
| 安全性 | 功能安全(SIL认证) | ★★★★☆ |
| 互联互通 | 支持多种工业协议 | ★★★★☆ |

### 1.2 主流工业Linux方案

```
┌─────────────────────────────────────────────────────────────────┐
│                    工业Linux方案对比                             │
├─────────────────┬─────────────────┬─────────────────────────────┤
│     方案         │     特点        │         适用场景            │
├─────────────────┼─────────────────┼─────────────────────────────┤
│ PREEMPT_RT      │ 内核抢占补丁     │ 通用实时控制                │
│ Xenomai         │ 双内核架构       │ 硬实时，μs级                │
│ RT-Linux        │ 早期实时方案     │  legacy系统                │
│ Ubuntu Core     │ 容器化、事务更新 │ 物联网边缘设备              │
│ Debian工业版    │ 长期支持、稳定   │ 通用工业PC                  │
│ Buildroot定制   │ 极致精简         │ 嵌入式控制器                │
│ Yocto项目       │ 灵活定制         │ 复杂嵌入式系统              │
└─────────────────┴─────────────────┴─────────────────────────────┘
```

### 1.3 系统架构

```
┌─────────────────────────────────────────────────────────────────┐
│                      应用层 (Applications)                       │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐             │
│  │  SCADA   │ │   MES    │ │   HMI    │ │数据分析  │             │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘             │
├─────────────────────────────────────────────────────────────────┤
│                      中间件层 (Middleware)                       │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐             │
│  │  OPC UA  │ │  MQTT    │ │ DDS/ROS2 │ │ 数据库   │             │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘             │
├─────────────────────────────────────────────────────────────────┤
│                      实时控制层 (Real-time)                      │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐             │
│  │ EtherCAT │ │  CANopen │ │ PLC运行  │ │ 运动控制 │             │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘             │
├─────────────────────────────────────────────────────────────────┤
│                      内核层 (Kernel)                             │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐             │
│  │PREEMPT_RT│ │ Xenomai  │ │ 设备驱动 │ │ 网络协议 │             │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘             │
├─────────────────────────────────────────────────────────────────┤
│                      硬件层 (Hardware)                           │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐             │
│  │ 工业PC   │ │嵌入式板卡│ │ I/O模块  │ │ 通信接口 │             │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘             │
└─────────────────────────────────────────────────────────────────┘
```

---

## 2. 实时Linux(Xenomai/PREEMPT_RT)

### 2.1 PREEMPT_RT补丁

PREEMPT_RT是Linux内核的实时补丁，通过以下机制实现实时性：

```
┌──────────────────────────────────────────────────────────────┐
│                    PREEMPT_RT核心机制                         │
├──────────────────────────────────────────────────────────────┤
│ 1. 线程化中断处理 (Threaded IRQ)                             │
│ 2. 高精度定时器 (High-Resolution Timers)                     │
│ 3. 优先级继承互斥锁 (PI-Futex)                               │
│ 4. 可抢占的RCU (Preemptible RCU)                             │
│ 5. 完全可抢占内核 (Fully Preemptible Kernel)                 │
└──────────────────────────────────────────────────────────────┘
```

### 2.2 实时任务编程示例

```c
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
```

### 2.3 Xenomai实时编程

```c
/**
 * @file xenomai_task.c
 * @brief Xenomai实时任务示例
 * @details 演示如何使用Xenomai创建硬实时任务
 *
 * 编译: gcc -o xenomai_task xenomai_task.c $(xeno-config --cflags --ldflags) -lnative
 * 运行: sudo ./xenomai_task
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <alchemy/sem.h>
#include <rtdm/rtdm.h>

/* Xenomai任务配置 */
#define TASK_PRIORITY   80
#define TASK_PERIOD_NS  1000000     /* 1ms周期 */
#define TASK_MODE       (T_JOINABLE | T_FPU)
#define TASK_STKSZ      0           /* 使用默认栈大小 */

/* 统计信息 */
static RT_TASK rt_task;
static volatile int g_running = 1;
static long long g_min_latency = 1000000000LL;
static long long g_max_latency = 0;
static long long g_total_latency = 0;
static int g_sample_count = 0;

/**
 * @brief 信号处理函数
 */
static void signal_handler(int sig)
{
    (void)sig;
    g_running = 0;
}

/**
 * @brief 获取当前时间 (纳秒)
 */
static inline long long rt_get_time_ns(void)
{
    RTIME now = rt_timer_read();
    return (long long)now;
}

/**
 * @brief Xenomai实时任务主体
 */
static void xenomai_task_body(void *arg)
{
    (void)arg;

    int err;
    RTIME period = rt_timer_ns2ticks(TASK_PERIOD_NS);
    RTIME next_period = rt_timer_read() + period;

    printf("[XENO] Hard real-time task started\n");
    printf("[XENO] Period: %lld ns\n", TASK_PERIOD_NS);

    /* 设置任务为周期性执行 */
    err = rt_task_set_periodic(NULL, TM_NOW, period);
    if (err != 0) {
        fprintf(stderr, "[XENO] rt_task_set_periodic failed: %d\n", err);
        return;
    }

    while (g_running) {
        /* 记录开始时间 */
        RTIME start = rt_timer_read();

        /* ==================== 硬实时工作区 ==================== */

        /* 模拟控制算法 (必须在截止时间前完成) */
        volatile double control_output = 0.0;

        /* PID计算示例 */
        static double integral = 0.0;
        static double last_error = 0.0;
        double setpoint = 100.0;
        double feedback = 95.0 + (rand() % 10);  /* 模拟传感器反馈 */

        double error = setpoint - feedback;
        integral += error * (TASK_PERIOD_NS / 1e9);
        double derivative = (error - last_error) / (TASK_PERIOD_NS / 1e9);

        double Kp = 1.0, Ki = 0.1, Kd = 0.01;
        control_output = Kp * error + Ki * integral + Kd * derivative;
        last_error = error;

        /* ====================================================== */

        /* 记录结束时间并计算延迟 */
        RTIME end = rt_timer_read();
        long long latency = (long long)(end - start);

        /* 更新统计 */
        if (latency < g_min_latency) g_min_latency = latency;
        if (latency > g_max_latency) g_max_latency = latency;
        g_total_latency += latency;
        g_sample_count++;

        /* 等待下一个周期 */
        err = rt_task_wait_period(NULL);
        if (err == -ETIMEDOUT) {
            fprintf(stderr, "[XENO] Deadline missed!\n");
        }
    }

    printf("[XENO] Task terminated\n");
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    int err;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║          Xenomai Hard Real-Time Demo                     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* 检查是否在Xenomai Cobalt域运行 */
    if (!rt_task_shadow(NULL, "main", 50, T_FPU)) {
        printf("[INFO] Running in Xenomai Cobalt domain ✓\n");
    } else {
        printf("[WARNING] Not in Xenomai domain, real-time not guaranteed\n");
    }

    /* 注册信号处理 */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* 创建实时任务 */
    err = rt_task_create(&rt_task, "xeno_task", TASK_STKSZ,
                         TASK_PRIORITY, TASK_MODE);
    if (err != 0) {
        fprintf(stderr, "[ERROR] rt_task_create failed: %d\n", err);
        return 1;
    }

    /* 启动任务 */
    err = rt_task_start(&rt_task, &xenomai_task_body, NULL);
    if (err != 0) {
        fprintf(stderr, "[ERROR] rt_task_start failed: %d\n", err);
        rt_task_delete(&rt_task);
        return 1;
    }

    printf("[INFO] Press Ctrl+C to stop...\n\n");

    /* 等待任务完成 */
    while (g_running) {
        usleep(100000);  /* 100ms */

        /* 显示实时统计 */
        if (g_sample_count > 0) {
            printf("\r[INFO] Samples: %d | Min: %lld ns | Max: %lld ns | Avg: %lld ns",
                   g_sample_count, g_min_latency, g_max_latency,
                   g_total_latency / g_sample_count);
            fflush(stdout);
        }
    }

    printf("\n");

    /* 等待并删除任务 */
    rt_task_join(&rt_task);
    rt_task_delete(&rt_task);

    /* 打印最终统计 */
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║              Xenomai Task Statistics                     ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ Total Samples:  %d                                       ║\n", g_sample_count);
    printf("║ Min Latency:    %lld ns  (%lld μs)                      ║\n",
           g_min_latency, g_min_latency / 1000);
    printf("║ Max Latency:    %lld ns  (%lld μs)                      ║\n",
           g_max_latency, g_max_latency / 1000);
    printf("║ Avg Latency:    %lld ns  (%lld μs)                      ║\n",
           g_total_latency / g_sample_count,
           (g_total_latency / g_sample_count) / 1000);
    printf("╚══════════════════════════════════════════════════════════╝\n");

    return 0;
}
```

---

## 3. EtherCAT主站开发

### 3.1 EtherCAT协议概述

```
┌─────────────────────────────────────────────────────────────────┐
│                    EtherCAT数据帧结构                           │
├─────────────────────────────────────────────────────────────────┤
│  Ethernet Header (14 bytes)                                     │
│  ┌─────────────┬─────────────┬─────────────────────────────┐    │
│  │ Dest MAC    │ Src MAC     │ EtherType (0x88A4)          │    │
│  │ (6 bytes)   │ (6 bytes)   │ (2 bytes)                   │    │
│  └─────────────┴─────────────┴─────────────────────────────┘    │
├─────────────────────────────────────────────────────────────────┤
│  EtherCAT Header (2 bytes)                                      │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │ Length (11 bits) | Reserved (1 bit) | Type (4 bits)     │    │
│  └─────────────────────────────────────────────────────────┘    │
├─────────────────────────────────────────────────────────────────┤
│  EtherCAT Datagrams                                             │
│  ┌─────────┬─────────┬─────────┬─────────┬─────────┐           │
│  │ Command │ Index   │ Address │ Length  │ Data... │           │
│  │ (1byte) │ (1byte) │ (4byte) │ (2byte) │ (nbyte) │           │
│  └─────────┴─────────┴─────────┴─────────┴─────────┘           │
├─────────────────────────────────────────────────────────────────┤
│  Working Counter (2 bytes)                                      │
└─────────────────────────────────────────────────────────────────┘
```

### 3.2 IgH EtherCAT主站示例

```c
/**
 * @file ethercat_master.c
 * @brief IgH EtherCAT主站开发示例
 * @details 演示如何使用IgH EtherCAT Master库开发主站应用
 *
 * 依赖: libethercat-devel
 * 编译: gcc -o ethercat_master ethercat_master.c -lethercat -lpthread
 * 运行: sudo ./ethercat_master
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <sys/mman.h>
#include <ecrt.h>

/* EtherCAT配置 */
#define ECAT_DEVICE         "eth0"          /* EtherCAT网卡 */
#define TASK_FREQUENCY      1000            /* 任务频率: 1kHz */
#define TASK_PERIOD_NS      (1000000000 / TASK_FREQUENCY)

/* 从站配置示例 - Beckhoff EL2008 (8路数字输出) */
#define EL2008_VENDOR_ID    0x00000002
#define EL2008_PRODUCT_ID   0x07D83052

/* 从站配置示例 - Beckhoff EL1018 (8路数字输入) */
#define EL1018_VENDOR_ID    0x00000002
#define EL1018_PRODUCT_ID   0x03FA3052

/* 从站位置 */
#define SLAVE_DO_POSITION   0, 0    /* 总线位置0, 从站位置0 */
#define SLAVE_DI_POSITION   0, 1    /* 总线位置0, 从站位置1 */

/* 全局变量 */
static volatile int g_running = 1;
static ec_master_t *g_master = NULL;
static ec_master_state_t g_master_state = {};

/* 从站句柄 */
static ec_slave_config_t *g_sc_el2008 = NULL;
static ec_slave_config_t *g_sc_el1018 = NULL;
static ec_slave_config_state_t g_sc_el2008_state = {};
static ec_slave_config_state_t g_sc_el1018_state = {};

/* 过程数据域 */
static ec_domain_t *g_domain = NULL;
static ec_domain_state_t g_domain_state = {};
static uint8_t *g_domain_pd = NULL;  /* 过程数据指针 */

/* 过程数据偏移 */
static unsigned int g_off_el2008_out = 0;
static unsigned int g_off_el1018_in = 0;

/* 应用周期计数 */
static uint32_t g_cycle_count = 0;

/* 性能统计 */
static long long g_min_cycle_time = 1000000000LL;
static long long g_max_cycle_time = 0;
static long long g_total_cycle_time = 0;
static int g_stat_count = 0;

/**
 * @brief PDO条目配置
 * @details EL2008数字输出模块的PDO配置
 */
static const ec_pdo_entry_reg_t g_domain_regs[] = {
    /* EL2008: 8路数字输出 */
    {SLAVE_DO_POSITION, EL2008_VENDOR_ID, EL2008_PRODUCT_ID,
     0x7000, 0x01, &g_off_el2008_out},
    /* EL1018: 8路数字输入 */
    {SLAVE_DI_POSITION, EL1018_VENDOR_ID, EL1018_PRODUCT_ID,
     0x6000, 0x01, &g_off_el1018_in},
    {}
};

/* EL2008输出PDO映射 */
static const ec_pdo_entry_info_t g_el2008_entries[] = {
    {0x7000, 0x01, 8}   /* 8位输出 */
};

static const ec_pdo_info_t g_el2008_pdos[] = {
    {0x1600, 1, g_el2008_entries}
};

static const ec_sync_info_t g_el2008_syncs[] = {
    {0, EC_DIR_OUTPUT, 1, g_el2008_pdos, EC_WD_ENABLE},
    {0xff}  /* 结束标记 */
};

/* EL1018输入PDO映射 */
static const ec_pdo_entry_info_t g_el1018_entries[] = {
    {0x6000, 0x01, 8}   /* 8位输入 */
};

static const ec_pdo_info_t g_el1018_pdos[] = {
    {0x1A00, 1, g_el1018_entries}
};

static const ec_sync_info_t g_el1018_syncs[] = {
    {0, EC_DIR_INPUT, 1, g_el1018_pdos, EC_WD_DISABLE},
    {0xff}  /* 结束标记 */
};

/**
 * @brief 获取当前时间 (纳秒)
 */
static inline long long get_time_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

/**
 * @brief 信号处理函数
 */
static void signal_handler(int sig)
{
    (void)sig;
    printf("\n[INFO] Received signal, stopping...\n");
    g_running = 0;
}

/**
 * @brief 检查主站状态
 */
static void check_master_state(void)
{
    ec_master_state_t ms;
    ecrt_master_state(g_master, &ms);

    if (ms.slaves_responding != g_master_state.slaves_responding) {
        printf("[INFO] %u slave(s) responding\n", ms.slaves_responding);
    }

    if (ms.al_states != g_master_state.al_states) {
        printf("[INFO] AL states: 0x%02X\n", ms.al_states);
    }

    if (ms.link_up != g_master_state.link_up) {
        printf("[INFO] Link is %s\n", ms.link_up ? "up" : "down");
    }

    g_master_state = ms;
}

/**
 * @brief 检查从站配置状态
 */
static void check_slave_config_states(void)
{
    ec_slave_config_state_t s;

    /* 检查EL2008状态 */
    ecrt_slave_config_state(g_sc_el2008, &s);
    if (s.al_state != g_sc_el2008_state.al_state) {
        printf("[INFO] EL2008: State 0x%02X\n", s.al_state);
    }
    if (s.online != g_sc_el2008_state.online) {
        printf("[INFO] EL2008: %s\n", s.online ? "online" : "offline");
    }
    if (s.operational != g_sc_el2008_state.operational) {
        printf("[INFO] EL2008: %s\n", s.operational ? "operational" : "not operational");
    }
    g_sc_el2008_state = s;

    /* 检查EL1018状态 */
    ecrt_slave_config_state(g_sc_el1018, &s);
    if (s.al_state != g_sc_el1018_state.al_state) {
        printf("[INFO] EL1018: State 0x%02X\n", s.al_state);
    }
    if (s.online != g_sc_el1018_state.online) {
        printf("[INFO] EL1018: %s\n", s.online ? "online" : "offline");
    }
    if (s.operational != g_sc_el1018_state.operational) {
        printf("[INFO] EL1018: %s\n", s.operational ? "operational" : "not operational");
    }
    g_sc_el1018_state = s;
}

/**
 * @brief 检查域状态
 */
static void check_domain_state(void)
{
    ec_domain_state_t ds;
    ecrt_domain_state(g_domain, &ds);

    if (ds.working_counter != g_domain_state.working_counter) {
        printf("[INFO] Domain: WC %u\n", ds.working_counter);
    }

    if (ds.wc_state != g_domain_state.wc_state) {
        printf("[INFO] Domain: State %u\n", ds.wc_state);
    }

    g_domain_state = ds;
}

/**
 * @brief 初始化EtherCAT主站
 * @return 成功返回0，失败返回-1
 */
static int init_ethercat_master(void)
{
    printf("[INFO] Initializing EtherCAT master...\n");

    /* 请求主站实例 */
    g_master = ecrt_request_master(0);
    if (!g_master) {
        fprintf(stderr, "[ERROR] Failed to request master\n");
        return -1;
    }

    /* 创建过程数据域 */
    g_domain = ecrt_master_create_domain(g_master);
    if (!g_domain) {
        fprintf(stderr, "[ERROR] Failed to create domain\n");
        return -1;
    }

    /* 配置EL2008从站 */
    printf("[INFO] Configuring EL2008 (Digital Output)...\n");
    g_sc_el2008 = ecrt_master_slave_config(g_master,
        SLAVE_DO_POSITION, EL2008_VENDOR_ID, EL2008_PRODUCT_ID);
    if (!g_sc_el2008) {
        fprintf(stderr, "[ERROR] Failed to get EL2008 slave config\n");
        return -1;
    }

    if (ecrt_slave_config_pdos(g_sc_el2008, EC_END, g_el2008_syncs)) {
        fprintf(stderr, "[ERROR] Failed to config EL2008 PDOs\n");
        return -1;
    }

    /* 配置EL1018从站 */
    printf("[INFO] Configuring EL1018 (Digital Input)...\n");
    g_sc_el1018 = ecrt_master_slave_config(g_master,
        SLAVE_DI_POSITION, EL1018_VENDOR_ID, EL1018_PRODUCT_ID);
    if (!g_sc_el1018) {
        fprintf(stderr, "[ERROR] Failed to get EL1018 slave config\n");
        return -1;
    }

    if (ecrt_slave_config_pdos(g_sc_el1018, EC_END, g_el1018_syncs)) {
        fprintf(stderr, "[ERROR] Failed to config EL1018 PDOs\n");
        return -1;
    }

    /* 注册PDO条目 */
    if (ecrt_domain_reg_pdo_entry_list(g_domain, g_domain_regs)) {
        fprintf(stderr, "[ERROR] Failed to register PDO entries\n");
        return -1;
    }

    /* 激活主站配置 */
    printf("[INFO] Activating master configuration...\n");
    if (ecrt_master_activate(g_master)) {
        fprintf(stderr, "[ERROR] Failed to activate master\n");
        return -1;
    }

    /* 获取过程数据指针 */
    g_domain_pd = ecrt_domain_data(g_domain);
    if (!g_domain_pd) {
        fprintf(stderr, "[ERROR] Failed to get domain data pointer\n");
        return -1;
    }

    printf("[INFO] EtherCAT master initialized successfully\n");
    return 0;
}

/**
 * @brief 释放EtherCAT资源
 */
static void release_ethercat_master(void)
{
    printf("[INFO] Releasing EtherCAT master...\n");

    if (g_master) {
        ecrt_master_deactivate(g_master);
        ecrt_release_master(g_master);
        g_master = NULL;
    }
}

/**
 * @brief 运行主循环
 */
static void run_main_loop(void)
{
    struct timespec wakeup_time;
    long long start_time, end_time, cycle_time;

    /* 设置起始时间 */
    clock_gettime(CLOCK_MONOTONIC, &wakeup_time);

    printf("[INFO] Starting cyclic task (Frequency: %d Hz)...\n", TASK_FREQUENCY);
    printf("[INFO] Press Ctrl+C to stop\n\n");

    while (g_running) {
        start_time = get_time_ns();

        /* 接收EtherCAT数据 */
        ecrt_master_receive(g_master);
        ecrt_domain_process(g_domain);

        /* 检查状态 (每100周期) */
        if (g_cycle_count % 100 == 0) {
            check_domain_state();
            check_master_state();
            check_slave_config_states();
        }

        /* ==================== 应用逻辑 ==================== */

        /* 读取输入 (EL1018) */
        uint8_t input_data = EC_READ_U8(g_domain_pd + g_off_el1018_in);

        /* 处理逻辑: 跑马灯效果 */
        uint8_t output_data = 0;
        if (g_sc_el2008_state.operational) {
            /* 简单的跑马灯模式 */
            int led_index = (g_cycle_count / 100) % 8;
            output_data = (1 << led_index);
        }

        /* 写入输出 (EL2008) */
        EC_WRITE_U8(g_domain_pd + g_off_el2008_out, output_data);

        /* 周期性打印信息 */
        if (g_cycle_count % 1000 == 0) {
            printf("[DATA] Input: 0x%02X | Output: 0x%02X | Cycle: %u\n",
                   input_data, output_data, g_cycle_count);
        }

        /* ================================================= */

        /* 发送EtherCAT数据 */
        ecrt_domain_queue(g_domain);
        ecrt_master_send(g_master);

        g_cycle_count++;

        /* 统计周期时间 */
        end_time = get_time_ns();
        cycle_time = end_time - start_time;

        if (cycle_time < g_min_cycle_time) g_min_cycle_time = cycle_time;
        if (cycle_time > g_max_cycle_time) g_max_cycle_time = cycle_time;
        g_total_cycle_time += cycle_time;
        g_stat_count++;

        /* 等待下一个周期 */
        wakeup_time.tv_nsec += TASK_PERIOD_NS;
        while (wakeup_time.tv_nsec >= 1000000000L) {
            wakeup_time.tv_nsec -= 1000000000L;
            wakeup_time.tv_sec++;
        }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &wakeup_time, NULL);
    }
}

/**
 * @brief 打印统计信息
 */
static void print_statistics(void)
{
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║            EtherCAT Performance Statistics               ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ Total Cycles:    %u                                       ║\n", g_cycle_count);
    printf("║ Min Cycle Time:  %lld μs                                 ║\n", g_min_cycle_time / 1000);
    printf("║ Max Cycle Time:  %lld μs                                 ║\n", g_max_cycle_time / 1000);
    printf("║ Avg Cycle Time:  %lld μs                                 ║\n",
           g_stat_count > 0 ? (g_total_cycle_time / g_stat_count) / 1000 : 0);
    printf("╚══════════════════════════════════════════════════════════╝\n");
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║       IgH EtherCAT Master Demo Application               ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* 注册信号处理 */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* 锁定内存 */
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        perror("mlockall failed");
    }

    /* 初始化EtherCAT */
    if (init_ethercat_master() != 0) {
        fprintf(stderr, "[ERROR] Failed to initialize EtherCAT\n");
        return 1;
    }

    /* 运行主循环 */
    run_main_loop();

    /* 打印统计 */
    print_statistics();

    /* 释放资源 */
    release_ethercat_master();

    printf("\n[INFO] Program completed\n");

    return 0;
}
```

---

## 4. PLC运行时(软PLC)

### 4.1 软PLC架构

```
┌─────────────────────────────────────────────────────────────────┐
│                      软PLC运行时架构                            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                   IEC 61131-3 逻辑层                    │   │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐   │   │
│  │  │   LD     │ │   FBD    │ │   ST     │ │   SFC    │   │   │
│  │  │ 梯形图   │ │功能块图  │ │ 结构化文本│ │顺序功能图│   │   │
│  │  └──────────┘ └──────────┘ └──────────┘ └──────────┘   │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                   PLC运行时引擎                        │   │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐   │   │
│  │  │ 扫描周期 │ │ 任务调度 │ │ 变量管理 │ │ 诊断接口 │   │   │
│  │  │ 管理器   │ │ 器       │ │ 器       │ │          │   │   │
│  │  └──────────┘ └──────────┘ └──────────┘ └──────────┘   │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                   I/O处理层                            │   │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐   │   │
│  │  │EtherCAT  │ │ Modbus   │ │  CANopen │ │ 仿真I/O  │   │   │
│  │  │ 主站    │ │ RTU/TCP  │ │          │ │          │   │   │
│  │  └──────────┘ └──────────┘ └──────────┘ └──────────┘   │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                   通信接口层                           │   │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐   │   │
│  │  │  OPC UA  │ │  MQTT    │ │  TCP/IP  │ │ 串口通信 │   │   │
│  │  │ 服务器  │ │ 客户端   │ │  Socket  │ │          │   │   │
│  │  └──────────┘ └──────────┘ └──────────┘ └──────────┘   │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

### 4.2 迷你软PLC运行时实现

```c
/**
 * @file soft_plc_runtime.c
 * @brief 迷你软PLC运行时实现
 * @details 实现一个轻量级的软PLC运行时，支持扫描周期管理、
 *          变量表、I/O映射和简单的梯形图逻辑
 *
 * 编译: gcc -o soft_plc_runtime soft_plc_runtime.c -lpthread
 * 运行: sudo ./soft_plc_runtime
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>

/* 版本信息 */
#define PLC_VERSION_MAJOR   1
#define PLC_VERSION_MINOR   0
#define PLC_VERSION_PATCH   0

/* PLC配置 */
#define PLC_MAX_VARIABLES   256     /* 最大变量数 */
#define PLC_MAX_PROGRAMS    8       /* 最大程序数 */
#define PLC_MAX_IO_POINTS   128     /* 最大I/O点数 */
#define PLC_SCAN_TIME_US    10000   /* 默认扫描周期: 10ms */

/* 变量类型 */
typedef enum {
    VAR_TYPE_BOOL = 0,      /* 布尔量 */
    VAR_TYPE_INT,           /* 整型 */
    VAR_TYPE_REAL,          /* 浮点 */
    VAR_TYPE_WORD,          /* 16位字 */
    VAR_TYPE_DWORD,         /* 32位双字 */
    VAR_TYPE_TIMER,         /* 定时器 */
    VAR_TYPE_COUNTER,       /* 计数器 */
    VAR_TYPE_MAX
} plc_var_type_t;

/* 变量区域 */
typedef enum {
    VAR_AREA_I = 0,         /* 输入区 */
    VAR_AREA_Q,             /* 输出区 */
    VAR_AREA_M,             /* 中间变量区 */
    VAR_AREA_DB,            /* 数据块区 */
    VAR_AREA_T,             /* 定时器区 */
    VAR_AREA_C,             /* 计数器区 */
    VAR_AREA_MAX
} plc_var_area_t;

/* 定时器结构 */
typedef struct {
    bool in;                /* 使能输入 */
    bool q;                 /* 输出 */
    uint32_t pt;            /* 预设时间 (ms) */
    uint32_t et;            /* 已计时间 (ms) */
    uint64_t start_time;    /* 启动时间戳 */
} plc_timer_t;

/* 计数器结构 */
typedef struct {
    bool cu;                /* 加计数输入 */
    bool cd;                /* 减计数输入 */
    bool r;                 /* 复位 */
    bool ld;                /* 装载 */
    int32_t pv;             /* 预设值 */
    int32_t cv;             /* 当前值 */
    bool q;                 /* 输出 */
    bool qu;                /* 加计数到达 */
    bool qd;                /* 减计数到达 */
    bool prev_cu;           /* 上次cu状态 */
    bool prev_cd;           /* 上次cd状态 */
} plc_counter_t;

/* 变量值联合体 */
typedef union {
    bool b;                 /* 布尔值 */
    int32_t i;              /* 整数值 */
    float f;                /* 浮点值 */
    uint16_t w;             /* 字值 */
    uint32_t dw;            /* 双字值 */
} plc_var_value_t;

/* 变量定义 */
typedef struct {
    char name[32];          /* 变量名 */
    plc_var_type_t type;    /* 变量类型 */
    plc_var_area_t area;    /* 变量区域 */
    uint16_t addr;          /* 区域地址 */
    plc_var_value_t value;  /* 当前值 */
    plc_var_value_t prev;   /* 上次值 */
    bool forced;            /* 强制标志 */
    plc_var_value_t force_val; /* 强制值 */
} plc_variable_t;

/* I/O点定义 */
typedef struct {
    plc_var_area_t area;    /* I/O区域 */
    uint16_t addr;          /* 地址 */
    bool value;             /* 当前值 */
    bool forced;            /* 强制标志 */
    uint8_t quality;        /* 质量码 (0=Good, 1=Bad, 2=Uncertain) */
    uint64_t timestamp;     /* 时间戳 */
} plc_io_point_t;

/* PLC程序 */
typedef struct {
    char name[32];          /* 程序名 */
    uint32_t cycle_count;   /* 执行计数 */
    uint32_t exec_time_us;  /* 执行时间 (微秒) */
    bool enabled;           /* 使能标志 */
    void (*init)(void);     /* 初始化函数 */
    void (*run)(void);      /* 运行函数 */
    void (*cleanup)(void);  /* 清理函数 */
} plc_program_t;

/* PLC运行时状态 */
typedef enum {
    PLC_STATE_STOP = 0,     /* 停止 */
    PLC_STATE_STARTING,     /* 启动中 */
    PLC_STATE_RUNNING,      /* 运行中 */
    PLC_STATE_STOPPING,     /* 停止中 */
    PLC_STATE_ERROR         /* 错误 */
} plc_state_t;

/* PLC运行时上下文 */
typedef struct {
    /* 状态 */
    plc_state_t state;
    uint32_t scan_count;
    uint32_t scan_time_us;
    uint32_t last_scan_time_us;
    uint32_t min_scan_time_us;
    uint32_t max_scan_time_us;
    uint64_t total_scan_time_us;

    /* 变量表 */
    plc_variable_t variables[PLC_MAX_VARIABLES];
    uint16_t var_count;

    /* I/O表 */
    plc_io_point_t io_inputs[PLC_MAX_IO_POINTS];
    plc_io_point_t io_outputs[PLC_MAX_IO_POINTS];
    uint16_t input_count;
    uint16_t output_count;

    /* 定时器和计数器 */
    plc_timer_t timers[32];
    plc_counter_t counters[32];

    /* 程序表 */
    plc_program_t programs[PLC_MAX_PROGRAMS];
    uint8_t program_count;

    /* 运行时控制 */
    volatile bool running;
    pthread_t cycle_thread;
    pthread_mutex_t lock;
} plc_runtime_t;

/* 全局运行时实例 */
static plc_runtime_t g_plc = {0};
static volatile int g_app_running = 1;

/**
 * @brief 获取当前时间 (微秒)
 */
static inline uint64_t get_time_us(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;
}

/**
 * @brief 信号处理函数
 */
static void signal_handler(int sig)
{
    (void)sig;
    printf("\n[INFO] Stopping PLC runtime...\n");
    g_app_running = 0;
    g_plc.running = false;
}

/**
 * @brief 初始化PLC运行时
 */
static int plc_init(void)
{
    printf("[PLC] Initializing runtime...\n");

    memset(&g_plc, 0, sizeof(g_plc));
    g_plc.state = PLC_STATE_STOP;
    g_plc.scan_time_us = PLC_SCAN_TIME_US;
    g_plc.min_scan_time_us = 0xFFFFFFFF;

    pthread_mutex_init(&g_plc.lock, NULL);

    printf("[PLC] Runtime initialized\n");
    return 0;
}

/**
 * @brief 清理PLC运行时
 */
static void plc_cleanup(void)
{
    printf("[PLC] Cleaning up...\n");

    pthread_mutex_destroy(&g_plc.lock);

    printf("[PLC] Cleanup complete\n");
}

/**
 * @brief 创建变量
 */
static plc_variable_t *plc_create_variable(const char *name,
                                           plc_var_type_t type,
                                           plc_var_area_t area,
                                           uint16_t addr)
{
    if (g_plc.var_count >= PLC_MAX_VARIABLES) {
        return NULL;
    }

    plc_variable_t *var = &g_plc.variables[g_plc.var_count++];
    strncpy(var->name, name, sizeof(var->name) - 1);
    var->type = type;
    var->area = area;
    var->addr = addr;
    var->forced = false;
    memset(&var->value, 0, sizeof(var->value));
    memset(&var->prev, 0, sizeof(var->prev));

    return var;
}

/**
 * @brief 查找变量
 */
static plc_variable_t *plc_find_variable(const char *name)
{
    for (uint16_t i = 0; i < g_plc.var_count; i++) {
        if (strcmp(g_plc.variables[i].name, name) == 0) {
            return &g_plc.variables[i];
        }
    }
    return NULL;
}

/**
 * @brief 获取变量值 (布尔)
 */
static bool plc_get_bool(plc_var_area_t area, uint16_t addr)
{
    for (uint16_t i = 0; i < g_plc.var_count; i++) {
        plc_variable_t *var = &g_plc.variables[i];
        if (var->area == area && var->addr == addr) {
            if (var->forced) {
                return var->force_val.b;
            }
            return var->value.b;
        }
    }
    return false;
}

/**
 * @brief 设置变量值 (布尔)
 */
static void plc_set_bool(plc_var_area_t area, uint16_t addr, bool value)
{
    for (uint16_t i = 0; i < g_plc.var_count; i++) {
        plc_variable_t *var = &g_plc.variables[i];
        if (var->area == area && var->addr == addr) {
            var->prev.b = var->value.b;
            var->value.b = value;
            return;
        }
    }
}

/**
 * @brief 获取变量值 (整型)
 */
static int32_t plc_get_int(plc_var_area_t area, uint16_t addr)
{
    for (uint16_t i = 0; i < g_plc.var_count; i++) {
        plc_variable_t *var = &g_plc.variables[i];
        if (var->area == area && var->addr == addr) {
            if (var->forced) {
                return var->force_val.i;
            }
            return var->value.i;
        }
    }
    return 0;
}

/**
 * @brief 设置变量值 (整型)
 */
static void plc_set_int(plc_var_area_t area, uint16_t addr, int32_t value)
{
    for (uint16_t i = 0; i < g_plc.var_count; i++) {
        plc_variable_t *var = &g_plc.variables[i];
        if (var->area == area && var->addr == addr) {
            var->prev.i = var->value.i;
            var->value.i = value;
            return;
        }
    }
}

/**
 * @brief 处理定时器
 */
static void plc_process_timers(void)
{
    uint64_t now = get_time_us();

    for (int i = 0; i < 32; i++) {
        plc_timer_t *t = &g_plc.timers[i];

        if (t->in) {
            if (t->et < t->pt) {
                uint64_t elapsed = (now - t->start_time) / 1000;  /* ms */
                t->et = (uint32_t)elapsed;
                if (t->et >= t->pt) {
                    t->et = t->pt;
                    t->q = true;
                }
            }
        } else {
            t->q = false;
            t->et = 0;
            t->start_time = now;
        }
    }
}

/**
 * @brief 处理计数器
 */
static void plc_process_counters(void)
{
    for (int i = 0; i < 32; i++) {
        plc_counter_t *c = &g_plc.counters[i];

        /* 复位 */
        if (c->r) {
            c->cv = 0;
            c->qu = false;
            c->qd = false;
            c->prev_cu = false;
            c->prev_cd = false;
            continue;
        }

        /* 装载 */
        if (c->ld) {
            c->cv = c->pv;
        }

        /* 加计数 (上升沿) */
        if (c->cu && !c->prev_cu) {
            if (c->cv < 32767) {
                c->cv++;
            }
        }
        c->prev_cu = c->cu;

        /* 减计数 (上升沿) */
        if (c->cd && !c->prev_cd) {
            if (c->cv > -32768) {
                c->cv--;
            }
        }
        c->prev_cd = c->cd;

        /* 输出状态 */
        c->qu = (c->cv >= c->pv);
        c->qd = (c->cv <= 0);
        c->q = c->qu;
    }
}

/**
 * @brief 梯形图逻辑示例 - 电机启停控制
 * @details
 *   I0.0 - 启动按钮 (常开)
 *   I0.1 - 停止按钮 (常闭)
 *   M0.0 - 电机运行标志
 *   Q0.0 - 电机输出
 */
static void plc_program_motor_control(void)
{
    /* 读取输入 */
    bool start_btn = plc_get_bool(VAR_AREA_I, 0);   /* I0.0 */
    bool stop_btn = plc_get_bool(VAR_AREA_I, 1);    /* I0.1 */

    /* 自锁电路 */
    bool motor_run = plc_get_bool(VAR_AREA_M, 0);   /* M0.0 */

    /* 梯形图逻辑: M0.0 = (I0.0 OR M0.0) AND I0.1 */
    motor_run = (start_btn || motor_run) && stop_btn;

    /* 写入输出 */
    plc_set_bool(VAR_AREA_M, 0, motor_run);         /* M0.0 */
    plc_set_bool(VAR_AREA_Q, 0, motor_run);         /* Q0.0 */
}

/**
 * @brief 梯形图逻辑示例 - 顺序控制
 * @details 简单的三步骤顺序控制，使用定时器延时
 */
static void plc_program_sequential_control(void)
{
    static uint8_t step = 0;

    /* 使用定时器T0 */
    plc_timer_t *t0 = &g_plc.timers[0];

    switch (step) {
        case 0:  /* 等待启动 */
            if (plc_get_bool(VAR_AREA_I, 2)) {  /* I0.2 启动信号 */
                step = 1;
                t0->in = true;
                t0->pt = 2000;  /* 2秒延时 */
            }
            break;

        case 1:  /* 步骤1: 打开阀门1 */
            plc_set_bool(VAR_AREA_Q, 1, true);   /* Q0.1 阀门1 */
            if (t0->q) {  /* 延时到 */
                t0->in = false;
                step = 2;
                t0->in = true;
                t0->pt = 3000;  /* 3秒延时 */
            }
            break;

        case 2:  /* 步骤2: 打开阀门2，关闭阀门1 */
            plc_set_bool(VAR_AREA_Q, 1, false);  /* Q0.1 */
            plc_set_bool(VAR_AREA_Q, 2, true);   /* Q0.2 阀门2 */
            if (t0->q) {
                t0->in = false;
                step = 0;  /* 回到初始 */
                plc_set_bool(VAR_AREA_Q, 2, false);
            }
            break;
    }
}

/**
 * @brief 梯形图逻辑示例 - 模拟量处理
 * @details 读取模拟量输入，进行滤波和缩放后输出
 */
static void plc_program_analog_processing(void)
{
    static float filtered_value = 0.0f;
    const float alpha = 0.1f;  /* 一阶滤波系数 */

    /* 读取模拟量输入 (0-4095 对应 0-10V) */
    int32_t raw_input = plc_get_int(VAR_AREA_I, 10);  /* IW10 */

    /* 转换为实际值 (0.0 - 10.0V) */
    float voltage = (float)raw_input * 10.0f / 4095.0f;

    /* 一阶低通滤波 */
    filtered_value = alpha * voltage + (1.0f - alpha) * filtered_value;

    /* 温度转换 (假设0-10V对应0-200°C) */
    float temperature = filtered_value * 20.0f;  /* 0-200°C */

    /* 写入输出 */
    plc_set_int(VAR_AREA_Q, 10, (int32_t)(temperature * 10));  /* QW10 */

    /* 超温报警 */
    plc_set_bool(VAR_AREA_Q, 4, temperature > 150.0f);  /* Q0.4 */
}

/**
 * @brief PLC扫描周期任务
 */
static void plc_scan_cycle(void)
{
    uint64_t start_time, end_time;

    pthread_mutex_lock(&g_plc.lock);

    start_time = get_time_us();
    g_plc.scan_count++;

    /* ========== 扫描周期开始 ========== */

    /* 1. 读取输入 */
    /* (实际项目中从硬件读取) */

    /* 2. 处理定时器和计数器 */
    plc_process_timers();
    plc_process_counters();

    /* 3. 执行用户程序 */
    plc_program_motor_control();
    plc_program_sequential_control();
    plc_program_analog_processing();

    /* 4. 写入输出 */
    /* (实际项目中写入硬件) */

    /* ========== 扫描周期结束 ========== */

    end_time = get_time_us();
    g_plc.last_scan_time_us = (uint32_t)(end_time - start_time);

    /* 更新统计 */
    if (g_plc.last_scan_time_us < g_plc.min_scan_time_us) {
        g_plc.min_scan_time_us = g_plc.last_scan_time_us;
    }
    if (g_plc.last_scan_time_us > g_plc.max_scan_time_us) {
        g_plc.max_scan_time_us = g_plc.last_scan_time_us;
    }
    g_plc.total_scan_time_us += g_plc.last_scan_time_us;

    pthread_mutex_unlock(&g_plc.lock);
}

/**
 * @brief PLC运行时线程
 */
static void *plc_cycle_thread(void *arg)
{
    (void)arg;
    struct timespec next_time;

    printf("[PLC] Cycle thread started\n");

    clock_gettime(CLOCK_MONOTONIC, &next_time);

    while (g_plc.running && g_app_running) {
        /* 执行扫描周期 */
        plc_scan_cycle();

        /* 等待下一个周期 */
        next_time.tv_nsec += g_plc.scan_time_us * 1000;
        while (next_time.tv_nsec >= 1000000000L) {
            next_time.tv_nsec -= 1000000000L;
            next_time.tv_sec++;
        }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time, NULL);
    }

    printf("[PLC] Cycle thread stopped\n");
    return NULL;
}

/**
 * @brief 启动PLC
 */
static int plc_start(void)
{
    printf("[PLC] Starting runtime...\n");

    if (g_plc.state == PLC_STATE_RUNNING) {
        printf("[PLC] Already running\n");
        return 0;
    }

    g_plc.state = PLC_STATE_STARTING;
    g_plc.running = true;

    /* 创建扫描周期线程 */
    if (pthread_create(&g_plc.cycle_thread, NULL, plc_cycle_thread, NULL) != 0) {
        g_plc.state = PLC_STATE_ERROR;
        return -1;
    }

    /* 设置实时优先级 */
    struct sched_param param = {.sched_priority = 80};
    pthread_setschedparam(g_plc.cycle_thread, SCHED_FIFO, &param);

    g_plc.state = PLC_STATE_RUNNING;
    printf("[PLC] Runtime started\n");
    return 0;
}

/**
 * @brief 停止PLC
 */
static void plc_stop(void)
{
    printf("[PLC] Stopping runtime...\n");

    g_plc.state = PLC_STATE_STOPPING;
    g_plc.running = false;

    pthread_join(g_plc.cycle_thread, NULL);

    g_plc.state = PLC_STATE_STOP;
    printf("[PLC] Runtime stopped\n");
}

/**
 * @brief 打印PLC状态
 */
static void plc_print_status(void)
{
    pthread_mutex_lock(&g_plc.lock);

    const char *state_str[] = {"STOP", "STARTING", "RUNNING", "STOPPING", "ERROR"};

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║                PLC Runtime Status                        ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ State:          %s\n", state_str[g_plc.state]);
    printf("║ Scan Count:     %u\n", g_plc.scan_count);
    printf("║ Scan Time:      %u us (target: %u us)\n",
           g_plc.last_scan_time_us, g_plc.scan_time_us);
    printf("║ Min Scan Time:  %u us\n", g_plc.min_scan_time_us);
    printf("║ Max Scan Time:  %u us\n", g_plc.max_scan_time_us);
    if (g_plc.scan_count > 0) {
        printf("║ Avg Scan Time:  %llu us\n",
               g_plc.total_scan_time_us / g_plc.scan_count);
    }
    printf("║ Variables:      %u/%d\n", g_plc.var_count, PLC_MAX_VARIABLES);
    printf("╚══════════════════════════════════════════════════════════╝\n");

    pthread_mutex_unlock(&g_plc.lock);
}

/**
 * @brief 初始化演示变量
 */
static void init_demo_variables(void)
{
    /* 数字量输入 */
    plc_create_variable("StartButton", VAR_TYPE_BOOL, VAR_AREA_I, 0);
    plc_create_variable("StopButton", VAR_TYPE_BOOL, VAR_AREA_I, 1);
    plc_create_variable("SeqStart", VAR_TYPE_BOOL, VAR_AREA_I, 2);

    /* 数字量输出 */
    plc_create_variable("MotorRun", VAR_TYPE_BOOL, VAR_AREA_Q, 0);
    plc_create_variable("Valve1", VAR_TYPE_BOOL, VAR_AREA_Q, 1);
    plc_create_variable("Valve2", VAR_TYPE_BOOL, VAR_AREA_Q, 2);
    plc_create_variable("Alarm", VAR_TYPE_BOOL, VAR_AREA_Q, 4);

    /* 中间变量 */
    plc_create_variable("MotorState", VAR_TYPE_BOOL, VAR_AREA_M, 0);

    /* 模拟量 */
    plc_create_variable("RawAnalog", VAR_TYPE_INT, VAR_AREA_I, 10);
    plc_create_variable("Temperature", VAR_TYPE_INT, VAR_AREA_Q, 10);

    /* 初始化定时器 */
    g_plc.timers[0].pt = 0;
    g_plc.timers[0].et = 0;
    g_plc.timers[0].q = false;
    g_plc.timers[0].in = false;
    g_plc.timers[0].start_time = get_time_us();
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║       Mini Soft-PLC Runtime v%d.%d.%d                      ║\n",
           PLC_VERSION_MAJOR, PLC_VERSION_MINOR, PLC_VERSION_PATCH);
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* 注册信号处理 */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* 锁定内存 */
    mlockall(MCL_CURRENT | MCL_FUTURE);

    /* 初始化PLC */
    if (plc_init() != 0) {
        fprintf(stderr, "[ERROR] Failed to initialize PLC\n");
        return 1;
    }

    /* 初始化演示变量 */
    init_demo_variables();

    /* 启动PLC */
    if (plc_start() != 0) {
        fprintf(stderr, "[ERROR] Failed to start PLC\n");
        plc_cleanup();
        return 1;
    }

    printf("[INFO] PLC is running. Press Ctrl+C to stop.\n\n");

    /* 模拟输入变化 */
    uint32_t sim_counter = 0;
    while (g_app_running) {
        sleep(1);
        sim_counter++;

        pthread_mutex_lock(&g_plc.lock);

        /* 模拟启动按钮按下 (每10秒) */
        if (sim_counter % 10 == 0) {
            plc_set_bool(VAR_AREA_I, 0, true);
        } else if (sim_counter % 10 == 2) {
            plc_set_bool(VAR_AREA_I, 0, false);
        }

        /* 模拟停止按钮 (常闭) */
        plc_set_bool(VAR_AREA_I, 1, true);

        /* 模拟顺序启动信号 (每30秒) */
        if (sim_counter % 30 == 5) {
            plc_set_bool(VAR_AREA_I, 2, true);
        } else if (sim_counter % 30 == 6) {
            plc_set_bool(VAR_AREA_I, 2, false);
        }

        /* 模拟模拟量输入 (正弦波) */
        float angle = (float)sim_counter * 0.1f;
        int32_t analog_val = (int32_t)(2047.5f + 2047.5f * sin(angle));
        plc_set_int(VAR_AREA_I, 10, analog_val);

        pthread_mutex_unlock(&g_plc.lock);

        /* 每5秒打印状态 */
        if (sim_counter % 5 == 0) {
            plc_print_status();
        }
    }

    /* 停止PLC */
    plc_stop();

    /* 打印最终状态 */
    plc_print_status();

    /* 清理 */
    plc_cleanup();

    printf("\n[INFO] Program completed\n");

    return 0;
}
```

---

## 5. OPC UA服务器实现

### 5.1 OPC UA概述

```
┌─────────────────────────────────────────────────────────────────┐
│                    OPC UA协议栈架构                             │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    应用层 (Application)                  │   │
│  │         服务器/客户端应用、地址空间、方法调用            │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    服务层 (Services)                     │   │
│  │         会话管理、节点管理、订阅发布、历史访问           │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    安全层 (Security)                     │   │
│  │         身份验证、加密、签名、证书管理                   │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    传输层 (Transport)                    │   │
│  │         OPC TCP、HTTPS、WebSocket                        │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    数据层 (Data)                         │   │
│  │         UA Binary、XML、JSON 编码                        │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```
