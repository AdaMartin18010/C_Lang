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
    - [5.2 open62541 OPC UA服务器实现](#52-open62541-opc-ua服务器实现)
  - [6. 工业现场总线](#6-工业现场总线)
    - [6.1 Modbus RTU/TCP通信](#61-modbus-rtutcp通信)
    - [6.2 CANopen通信](#62-canopen通信)
  - [7. 数据采集系统](#7-数据采集系统)
    - [7.1 高性能数据采集架构](#71-高性能数据采集架构)
    - [7.2 环形缓冲区实现](#72-环形缓冲区实现)
  - [8. 边缘计算框架](#8-边缘计算框架)
    - [8.1 边缘计算架构](#81-边缘计算架构)
    - [8.2 边缘规则引擎](#82-边缘规则引擎)
  - [9. 系统可靠性设计](#9-系统可靠性设计)
    - [9.1 可靠性架构](#91-可靠性架构)
    - [9.2 看门狗与进程监控](#92-看门狗与进程监控)
  - [10. 实际项目:工业数据采集器(完整代码)](#10-实际项目工业数据采集器完整代码)
    - [10.1 项目概述](#101-项目概述)
    - [10.2 工业数据采集器完整代码](#102-工业数据采集器完整代码)
  - [11. 远程OTA升级](#11-远程ota升级)
    - [11.1 OTA升级架构](#111-ota升级架构)
    - [11.2 OTA升级客户端实现](#112-ota升级客户端实现)
  - [附录](#附录)
    - [A. 编译依赖安装](#a-编译依赖安装)
    - [B. 参考资料](#b-参考资料)
    - [C. 术语表](#c-术语表)

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

### 5.2 open62541 OPC UA服务器实现

```c
/**
 * @file opcua_server.c
 * @brief open62541 OPC UA服务器实现
 * @details 使用open62541库实现一个完整的OPC UA服务器，
 *          包含变量、方法和事件支持
 *
 * 依赖: libopen62541-dev
 * 编译: gcc -o opcua_server opcua_server.c -lopen62541 -lpthread
 * 运行: ./opcua_server
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/plugin/accesscontrol_default.h>

/* 版本信息 */
#define SERVER_VERSION      "1.0.0"
#define SERVER_PORT         4840
#define NAMESPACE_URI       "urn:industrial:opcua:server"

/* 服务器上下文 */
typedef struct {
    UA_Server *server;
    UA_ServerConfig *config;
    volatile UA_Boolean running;

    /* 模拟数据 */
    double temperature;
    double pressure;
    double flow_rate;
    UA_Boolean motor_running;
    uint32_t cycle_count;

    /* 节点ID */
    UA_NodeId temp_node;
    UA_NodeId pressure_node;
    UA_NodeId flow_node;
    UA_NodeId motor_node;
    UA_NodeId counter_node;
} server_context_t;

static server_context_t g_context = {0};

/**
 * @brief 信号处理函数
 */
static void signal_handler(int sig)
{
    (void)sig;
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Received signal, shutting down...");
    g_context.running = false;
}

/**
 * @brief 获取当前时间戳
 */
static UA_DateTime get_current_timestamp(void)
{
    return UA_DateTime_now();
}

/**
 * @brief 温度变量回调 - 在读取前调用
 */
static void before_read_temperature(UA_Server *server,
                                     const UA_NodeId *sessionId,
                                     void *sessionContext,
                                     const UA_NodeId *nodeId,
                                     void *nodeContext,
                                     const UA_NumericRange *range)
{
    (void)sessionId;
    (void)sessionContext;
    (void)nodeId;
    (void)nodeContext;
    (void)range;

    /* 模拟温度变化 */
    g_context.temperature = 20.0 + 5.0 * sin((double)time(NULL) / 10.0) +
                           (double)(rand() % 100) / 100.0;

    UA_Variant value;
    UA_Variant_setScalar(&value, &g_context.temperature, &UA_TYPES[UA_TYPES_DOUBLE]);
    UA_Server_writeValue(server, g_context.temp_node, value);
}

/**
 * @brief 压力变量回调 - 在读取前调用
 */
static void before_read_pressure(UA_Server *server,
                                  const UA_NodeId *sessionId,
                                  void *sessionContext,
                                  const UA_NodeId *nodeId,
                                  void *nodeContext,
                                  const UA_NumericRange *range)
{
    (void)sessionId;
    (void)sessionContext;
    (void)nodeId;
    (void)nodeContext;
    (void)range;

    /* 模拟压力变化 */
    g_context.pressure = 100.0 + 10.0 * cos((double)time(NULL) / 15.0) +
                        (double)(rand() % 50) / 10.0;

    UA_Variant value;
    UA_Variant_setScalar(&value, &g_context.pressure, &UA_TYPES[UA_TYPES_DOUBLE]);
    UA_Server_writeValue(server, g_context.pressure_node, value);
}

/**
 * @brief 流量变量回调
 */
static void before_read_flow(UA_Server *server,
                              const UA_NodeId *sessionId,
                              void *sessionContext,
                              const UA_NodeId *nodeId,
                              void *nodeContext,
                              const UA_NumericRange *range)
{
    (void)sessionId;
    (void)sessionContext;
    (void)nodeId;
    (void)nodeContext;
    (void)range;

    /* 流量与电机状态相关 */
    if (g_context.motor_running) {
        g_context.flow_rate = 50.0 + (double)(rand() % 100) / 10.0;
    } else {
        g_context.flow_rate = 0.0;
    }

    UA_Variant value;
    UA_Variant_setScalar(&value, &g_context.flow_rate, &UA_TYPES[UA_TYPES_DOUBLE]);
    UA_Server_writeValue(server, g_context.flow_node, value);
}

/**
 * @brief 启动电机方法回调
 */
static UA_StatusCode method_start_motor(UA_Server *server,
                                        const UA_NodeId *sessionId,
                                        void *sessionContext,
                                        const UA_NodeId *methodId,
                                        void *methodContext,
                                        const UA_NodeId *objectId,
                                        void *objectContext,
                                        size_t inputSize,
                                        const UA_Variant *input,
                                        size_t outputSize,
                                        UA_Variant *output)
{
    (void)server;
    (void)sessionId;
    (void)sessionContext;
    (void)methodId;
    (void)methodContext;
    (void)objectId;
    (void)objectContext;
    (void)inputSize;
    (void)input;
    (void)outputSize;

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Method 'StartMotor' called");

    if (!g_context.motor_running) {
        g_context.motor_running = true;
        g_context.cycle_count = 0;
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Motor started");

        /* 返回成功消息 */
        UA_String success = UA_STRING("Motor started successfully");
        UA_Variant_setScalarCopy(output, &success, &UA_TYPES[UA_TYPES_STRING]);
    } else {
        UA_String warning = UA_STRING("Motor is already running");
        UA_Variant_setScalarCopy(output, &warning, &UA_TYPES[UA_TYPES_STRING]);
    }

    return UA_STATUSCODE_GOOD;
}

/**
 * @brief 停止电机方法回调
 */
static UA_StatusCode method_stop_motor(UA_Server *server,
                                       const UA_NodeId *sessionId,
                                       void *sessionContext,
                                       const UA_NodeId *methodId,
                                       void *methodContext,
                                       const UA_NodeId *objectId,
                                       void *objectContext,
                                       size_t inputSize,
                                       const UA_Variant *input,
                                       size_t outputSize,
                                       UA_Variant *output)
{
    (void)server;
    (void)sessionId;
    (void)sessionContext;
    (void)methodId;
    (void)methodContext;
    (void)objectId;
    (void)objectContext;
    (void)inputSize;
    (void)input;
    (void)outputSize;

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Method 'StopMotor' called");

    if (g_context.motor_running) {
        g_context.motor_running = false;
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Motor stopped");

        UA_String success = UA_STRING("Motor stopped successfully");
        UA_Variant_setScalarCopy(output, &success, &UA_TYPES[UA_TYPES_STRING]);
    } else {
        UA_String warning = UA_STRING("Motor is already stopped");
        UA_Variant_setScalarCopy(output, &warning, &UA_TYPES[UA_TYPES_STRING]);
    }

    return UA_STATUSCODE_GOOD;
}

/**
 * @brief 获取系统信息方法
 */
static UA_StatusCode method_get_system_info(UA_Server *server,
                                            const UA_NodeId *sessionId,
                                            void *sessionContext,
                                            const UA_NodeId *methodId,
                                            void *methodContext,
                                            const UA_NodeId *objectId,
                                            void *objectContext,
                                            size_t inputSize,
                                            const UA_Variant *input,
                                            size_t outputSize,
                                            UA_Variant *output)
{
    (void)server;
    (void)sessionId;
    (void)sessionContext;
    (void)methodId;
    (void)methodContext;
    (void)objectId;
    (void)objectContext;
    (void)inputSize;
    (void)input;
    (void)outputSize;

    /* 创建系统信息字符串 */
    char info[256];
    snprintf(info, sizeof(info),
             "Server Version: %s\n"
             "Motor State: %s\n"
             "Cycle Count: %u\n"
             "Uptime: %ld seconds",
             SERVER_VERSION,
             g_context.motor_running ? "Running" : "Stopped",
             g_context.cycle_count,
             (long)time(NULL));

    UA_String result = UA_STRING(info);
    UA_Variant_setScalarCopy(output, &result, &UA_TYPES[UA_TYPES_STRING]);

    return UA_STATUSCODE_GOOD;
}

/**
 * @brief 创建设备对象模型
 */
static void create_device_model(UA_Server *server)
{
    UA_StatusCode retval;

    /* 添加命名空间 */
    UA_UInt16 nsIndex = UA_Server_addNamespace(server, NAMESPACE_URI);
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                "Namespace index: %d", nsIndex);

    /* ===== 创建对象类型: ProcessDevice ===== */
    UA_ObjectTypeAttributes deviceTypeAttr = UA_ObjectTypeAttributes_default;
    deviceTypeAttr.displayName = UA_LOCALIZEDTEXT("en-US", "ProcessDeviceType");
    deviceTypeAttr.description = UA_LOCALIZEDTEXT("en-US", "Industrial process device");

    UA_NodeId deviceTypeId;
    retval = UA_Server_addObjectTypeNode(server,
                                         UA_NODEID_NULL,
                                         UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
                                         UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
                                         UA_QUALIFIEDNAME(nsIndex, "ProcessDeviceType"),
                                         deviceTypeAttr,
                                         NULL,
                                         &deviceTypeId);
    if (retval != UA_STATUSCODE_GOOD) {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Failed to add device type: %s", UA_StatusCode_name(retval));
        return;
    }

    /* ===== 创建设备实例 ===== */
    UA_ObjectAttributes deviceAttr = UA_ObjectAttributes_default;
    deviceAttr.displayName = UA_LOCALIZEDTEXT("en-US", "IndustrialDevice1");
    deviceAttr.description = UA_LOCALIZEDTEXT("en-US", "Main process device");

    UA_NodeId deviceId;
    retval = UA_Server_addObjectNode(server,
                                     UA_NODEID_NULL,
                                     UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                                     UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                     UA_QUALIFIEDNAME(nsIndex, "IndustrialDevice1"),
                                     deviceTypeId,
                                     deviceAttr,
                                     NULL,
                                     &deviceId);
    if (retval != UA_STATUSCODE_GOOD) {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Failed to add device instance: %s", UA_StatusCode_name(retval));
        return;
    }

    /* ===== 添加温度变量 ===== */
    UA_VariableAttributes tempAttr = UA_VariableAttributes_default;
    tempAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Temperature");
    tempAttr.description = UA_LOCALIZEDTEXT("en-US", "Process temperature in Celsius");
    tempAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_HISTORYREAD;
    UA_Double tempValue = 25.0;
    UA_Variant_setScalar(&tempAttr.value, &tempValue, &UA_TYPES[UA_TYPES_DOUBLE]);
    tempAttr.dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;

    /* 添加工程单位属性 */
    UA_Argument euInfo;
    UA_Argument_init(&euInfo);
    euInfo.description = UA_LOCALIZEDTEXT("en-US", "Engineering units");
    euInfo.name = UA_STRING("EngineeringUnits");
    euInfo.dataType = UA_TYPES[UA_TYPES_EUINFORMATION].typeId;
    euInfo.valueRank = UA_VALUERANK_SCALAR;

    retval = UA_Server_addVariableNode(server,
                                       UA_NODEID_NULL,
                                       deviceId,
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                       UA_QUALIFIEDNAME(nsIndex, "Temperature"),
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                       tempAttr,
                                       NULL,
                                       &g_context.temp_node);
    if (retval == UA_STATUSCODE_GOOD) {
        /* 设置数据源回调 */
        UA_ValueCallback callback;
        callback.onRead = before_read_temperature;
        callback.onWrite = NULL;
        UA_Server_setVariableNode_valueCallback(server, g_context.temp_node, callback);
    }

    /* ===== 添加压力变量 ===== */
    UA_VariableAttributes pressureAttr = UA_VariableAttributes_default;
    pressureAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Pressure");
    pressureAttr.description = UA_LOCALIZEDTEXT("en-US", "Process pressure in kPa");
    pressureAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_HISTORYREAD;
    UA_Double pressureValue = 100.0;
    UA_Variant_setScalar(&pressureAttr.value, &pressureValue, &UA_TYPES[UA_TYPES_DOUBLE]);
    pressureAttr.dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;

    retval = UA_Server_addVariableNode(server,
                                       UA_NODEID_NULL,
                                       deviceId,
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                       UA_QUALIFIEDNAME(nsIndex, "Pressure"),
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                       pressureAttr,
                                       NULL,
                                       &g_context.pressure_node);
    if (retval == UA_STATUSCODE_GOOD) {
        UA_ValueCallback callback;
        callback.onRead = before_read_pressure;
        callback.onWrite = NULL;
        UA_Server_setVariableNode_valueCallback(server, g_context.pressure_node, callback);
    }

    /* ===== 添加流量变量 ===== */
    UA_VariableAttributes flowAttr = UA_VariableAttributes_default;
    flowAttr.displayName = UA_LOCALIZEDTEXT("en-US", "FlowRate");
    flowAttr.description = UA_LOCALIZEDTEXT("en-US", "Flow rate in L/min");
    flowAttr.accessLevel = UA_ACCESSLEVELMASK_READ;
    UA_Double flowValue = 0.0;
    UA_Variant_setScalar(&flowAttr.value, &flowValue, &UA_TYPES[UA_TYPES_DOUBLE]);
    flowAttr.dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;

    retval = UA_Server_addVariableNode(server,
                                       UA_NODEID_NULL,
                                       deviceId,
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                       UA_QUALIFIEDNAME(nsIndex, "FlowRate"),
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                       flowAttr,
                                       NULL,
                                       &g_context.flow_node);
    if (retval == UA_STATUSCODE_GOOD) {
        UA_ValueCallback callback;
        callback.onRead = before_read_flow;
        callback.onWrite = NULL;
        UA_Server_setVariableNode_valueCallback(server, g_context.flow_node, callback);
    }

    /* ===== 添加电机状态变量 ===== */
    UA_VariableAttributes motorAttr = UA_VariableAttributes_default;
    motorAttr.displayName = UA_LOCALIZEDTEXT("en-US", "MotorRunning");
    motorAttr.description = UA_LOCALIZEDTEXT("en-US", "Motor running status");
    motorAttr.accessLevel = UA_ACCESSLEVELMASK_READ;
    UA_Boolean motorValue = false;
    UA_Variant_setScalar(&motorAttr.value, &motorValue, &UA_TYPES[UA_TYPES_BOOLEAN]);
    motorAttr.dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;

    retval = UA_Server_addVariableNode(server,
                                       UA_NODEID_NULL,
                                       deviceId,
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                       UA_QUALIFIEDNAME(nsIndex, "MotorRunning"),
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                       motorAttr,
                                       NULL,
                                       &g_context.motor_node);

    /* ===== 添加计数器变量 ===== */
    UA_VariableAttributes counterAttr = UA_VariableAttributes_default;
    counterAttr.displayName = UA_LOCALIZEDTEXT("en-US", "CycleCount");
    counterAttr.description = UA_LOCALIZEDTEXT("en-US", "Number of production cycles");
    counterAttr.accessLevel = UA_ACCESSLEVELMASK_READ;
    UA_UInt32 counterValue = 0;
    UA_Variant_setScalar(&counterAttr.value, &counterValue, &UA_TYPES[UA_TYPES_UINT32]);
    counterAttr.dataType = UA_TYPES[UA_TYPES_UINT32].typeId;

    retval = UA_Server_addVariableNode(server,
                                       UA_NODEID_NULL,
                                       deviceId,
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                       UA_QUALIFIEDNAME(nsIndex, "CycleCount"),
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                       counterAttr,
                                       NULL,
                                       &g_context.counter_node);

    /* ===== 添加方法: 启动电机 ===== */
    UA_Argument outputArgument;
    UA_Argument_init(&outputArgument);
    outputArgument.description = UA_LOCALIZEDTEXT("en-US", "Operation result");
    outputArgument.name = UA_STRING("Result");
    outputArgument.dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    outputArgument.valueRank = UA_VALUERANK_SCALAR;

    UA_MethodAttributes startAttr = UA_MethodAttributes_default;
    startAttr.displayName = UA_LOCALIZEDTEXT("en-US", "StartMotor");
    startAttr.description = UA_LOCALIZEDTEXT("en-US", "Start the main motor");
    startAttr.executable = true;
    startAttr.userExecutable = true;

    retval = UA_Server_addMethodNode(server,
                                     UA_NODEID_NULL,
                                     deviceId,
                                     UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                     UA_QUALIFIEDNAME(nsIndex, "StartMotor"),
                                     startAttr,
                                     &method_start_motor,
                                     0, NULL,
                                     1, &outputArgument,
                                     NULL,
                                     NULL);

    /* ===== 添加方法: 停止电机 ===== */
    UA_MethodAttributes stopAttr = UA_MethodAttributes_default;
    stopAttr.displayName = UA_LOCALIZEDTEXT("en-US", "StopMotor");
    stopAttr.description = UA_LOCALIZEDTEXT("en-US", "Stop the main motor");
    stopAttr.executable = true;
    stopAttr.userExecutable = true;

    retval = UA_Server_addMethodNode(server,
                                     UA_NODEID_NULL,
                                     deviceId,
                                     UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                     UA_QUALIFIEDNAME(nsIndex, "StopMotor"),
                                     stopAttr,
                                     &method_stop_motor,
                                     0, NULL,
                                     1, &outputArgument,
                                     NULL,
                                     NULL);

    /* ===== 添加方法: 获取系统信息 ===== */
    UA_MethodAttributes infoAttr = UA_MethodAttributes_default;
    infoAttr.displayName = UA_LOCALIZEDTEXT("en-US", "GetSystemInfo");
    infoAttr.description = UA_LOCALIZEDTEXT("en-US", "Get system information");
    infoAttr.executable = true;
    infoAttr.userExecutable = true;

    retval = UA_Server_addMethodNode(server,
                                     UA_NODEID_NULL,
                                     deviceId,
                                     UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                     UA_QUALIFIEDNAME(nsIndex, "GetSystemInfo"),
                                     infoAttr,
                                     &method_get_system_info,
                                     0, NULL,
                                     1, &outputArgument,
                                     NULL,
                                     NULL);

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                "Device model created successfully");
}

/**
 * @brief 周期性回调 - 更新数据
 */
static void cyclic_callback(UA_Server *server, void *data)
{
    (void)server;
    (void)data;

    /* 更新周期计数 */
    if (g_context.motor_running) {
        g_context.cycle_count++;
    }

    /* 每100周期打印一次状态 */
    if (g_context.cycle_count % 100 == 0) {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                    "Status: Motor=%s, Temp=%.2f°C, Pressure=%.2fkPa, Cycles=%u",
                    g_context.motor_running ? "ON" : "OFF",
                    g_context.temperature,
                    g_context.pressure,
                    g_context.cycle_count);
    }
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    UA_StatusCode retval;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║    Industrial OPC UA Server v%s                        ║\n", SERVER_VERSION);
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* 注册信号处理 */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* 初始化服务器配置 */
    g_context.config = UA_ServerConfig_new_default();
    if (!g_context.config) {
        fprintf(stderr, "[ERROR] Failed to create server config\n");
        return 1;
    }

    /* 设置服务器端口 */
    g_context.config->networkLayersSize = 1;
    g_context.config->networkLayers =
        UA_ServerNetworkLayerTCP(UA_ConnectionConfig_default, SERVER_PORT);

    /* 创建服务器 */
    g_context.server = UA_Server_new(g_context.config);
    if (!g_context.server) {
        UA_ServerConfig_delete(g_context.config);
        fprintf(stderr, "[ERROR] Failed to create server\n");
        return 1;
    }

    /* 创建设备模型 */
    create_device_model(g_context.server);

    /* 添加周期性回调 (每1000ms) */
    UA_Server_addRepeatedCallback(g_context.server, cyclic_callback,
                                   NULL, 1000.0, NULL);

    /* 启动服务器 */
    g_context.running = true;

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                "Server starting on port %d...", SERVER_PORT);
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                "Use any OPC UA client to connect (e.g., UA Expert)");
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                "Press Ctrl+C to stop\n");

    /* 运行服务器 */
    retval = UA_Server_run(g_context.server, &g_context.running);

    /* 清理 */
    UA_Server_delete(g_context.server);
    UA_ServerConfig_delete(g_context.config);

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Server stopped");

    return (int)retval;
}
```

---

## 6. 工业现场总线

### 6.1 Modbus RTU/TCP通信

```c
/**
 * @file modbus_master.c
 * @brief Modbus RTU/TCP主站实现
 * @details 支持Modbus RTU (串口) 和 Modbus TCP (网络) 通信
 *
 * 编译: gcc -o modbus_master modbus_master.c
 * 运行: sudo ./modbus_master
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <termios.h>

/* Modbus功能码 */
#define MODBUS_FC_READ_COILS            0x01
#define MODBUS_FC_READ_DISCRETE_INPUTS  0x02
#define MODBUS_FC_READ_HOLDING_REGISTERS 0x03
#define MODBUS_FC_READ_INPUT_REGISTERS  0x04
#define MODBUS_FC_WRITE_SINGLE_COIL     0x05
#define MODBUS_FC_WRITE_SINGLE_REGISTER 0x06
#define MODBUS_FC_WRITE_MULTIPLE_COILS  0x0F
#define MODBUS_FC_WRITE_MULTIPLE_REGISTERS 0x10

/* Modbus异常码 */
#define MODBUS_EXCEPTION_ILLEGAL_FUNCTION       0x01
#define MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS   0x02
#define MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE     0x03
#define MODBUS_EXCEPTION_SLAVE_DEVICE_FAILURE   0x04

/* 配置 */
#define MODBUS_MAX_PDU_LENGTH       253
#define MODBUS_MAX_ADU_LENGTH       256
#define MODBUS_TCP_DEFAULT_PORT     502
#define MODBUS_TCP_MAX_CONNECTIONS  5

/* 传输模式 */
typedef enum {
    MODBUS_RTU = 0,
    MODBUS_TCP = 1
} modbus_mode_t;

/* 错误码 */
typedef enum {
    MODBUS_OK = 0,
    MODBUS_ERR_INVALID_PARAM = -1,
    MODBUS_ERR_CONNECTION = -2,
    MODBUS_ERR_TIMEOUT = -3,
    MODBUS_ERR_CRC = -4,
    MODBUS_ERR_EXCEPTION = -5,
    MODBUS_ERR_BUFFER = -6
} modbus_error_t;

/* Modbus上下文 */
typedef struct {
    modbus_mode_t mode;             /* 传输模式 */
    int fd;                         /* 文件描述符 (串口或socket) */

    /* RTU参数 */
    char device[64];                /* 串口设备 */
    int baudrate;                   /* 波特率 */
    char parity;                    /* 校验位: 'N', 'E', 'O' */
    int data_bits;                  /* 数据位 */
    int stop_bits;                  /* 停止位 */
    uint8_t slave_id;               /* 从站地址 */

    /* TCP参数 */
    char ip_address[16];            /* IP地址 */
    int tcp_port;                   /* TCP端口 */
    struct sockaddr_in tcp_addr;    /* TCP地址 */

    /* 通信参数 */
    uint32_t response_timeout_ms;   /* 响应超时 */
    uint32_t byte_timeout_ms;       /* 字节间超时 */

    /* 统计 */
    uint32_t tx_count;
    uint32_t rx_count;
    uint32_t error_count;
} modbus_context_t;

/**
 * @brief CRC16计算 (Modbus标准)
 */
static uint16_t modbus_crc16(const uint8_t *data, uint16_t length)
{
    static const uint16_t crc_table[256] = {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
        0x8E01, 0x4EC0, 0x4F80, 0x8F41, 0x4D00, 0x8DC1, 0x8C81, 0x4C40,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
    };

    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc = (crc >> 8) ^ crc_table[(crc ^ data[i]) & 0xFF];
    }
    return crc;
}

/**
 * @brief 创建RTU上下文
 */
modbus_context_t *modbus_new_rtu(const char *device, int baudrate,
                                  char parity, int data_bits, int stop_bits)
{
    modbus_context_t *ctx = calloc(1, sizeof(modbus_context_t));
    if (!ctx) {
        return NULL;
    }

    ctx->mode = MODBUS_RTU;
    ctx->fd = -1;
    strncpy(ctx->device, device, sizeof(ctx->device) - 1);
    ctx->baudrate = baudrate;
    ctx->parity = parity;
    ctx->data_bits = data_bits;
    ctx->stop_bits = stop_bits;
    ctx->slave_id = 1;
    ctx->response_timeout_ms = 500;
    ctx->byte_timeout_ms = 50;

    return ctx;
}

/**
 * @brief 创建TCP上下文
 */
modbus_context_t *modbus_new_tcp(const char *ip_address, int port)
{
    modbus_context_t *ctx = calloc(1, sizeof(modbus_context_t));
    if (!ctx) {
        return NULL;
    }

    ctx->mode = MODBUS_TCP;
    ctx->fd = -1;
    strncpy(ctx->ip_address, ip_address, sizeof(ctx->ip_address) - 1);
    ctx->tcp_port = (port == 0) ? MODBUS_TCP_DEFAULT_PORT : port;
    ctx->response_timeout_ms = 500;
    ctx->byte_timeout_ms = 50;

    return ctx;
}

/**
 * @brief 设置从站地址
 */
void modbus_set_slave(modbus_context_t *ctx, int slave)
{
    if (ctx) {
        ctx->slave_id = (uint8_t)slave;
    }
}

/**
 * @brief 连接RTU设备
 */
int modbus_connect_rtu(modbus_context_t *ctx)
{
    struct termios tios;

    /* 打开串口 */
    ctx->fd = open(ctx->device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (ctx->fd < 0) {
        perror("open");
        return MODBUS_ERR_CONNECTION;
    }

    /* 清除非阻塞标志 */
    int flags = fcntl(ctx->fd, F_GETFL);
    fcntl(ctx->fd, F_SETFL, flags & ~O_NONBLOCK);

    /* 配置串口 */
    tcgetattr(ctx->fd, &tios);
    cfmakeraw(&tios);

    /* 设置波特率 */
    speed_t speed;
    switch (ctx->baudrate) {
        case 9600:   speed = B9600;   break;
        case 19200:  speed = B19200;  break;
        case 38400:  speed = B38400;  break;
        case 57600:  speed = B57600;  break;
        case 115200: speed = B115200; break;
        default:     speed = B9600;   break;
    }
    cfsetispeed(&tios, speed);
    cfsetospeed(&tios, speed);

    /* 设置数据位 */
    tios.c_cflag &= ~CSIZE;
    switch (ctx->data_bits) {
        case 5: tios.c_cflag |= CS5; break;
        case 6: tios.c_cflag |= CS6; break;
        case 7: tios.c_cflag |= CS7; break;
        case 8: default: tios.c_cflag |= CS8; break;
    }

    /* 设置校验位 */
    switch (ctx->parity) {
        case 'E':
            tios.c_cflag |= PARENB;
            tios.c_cflag &= ~PARODD;
            break;
        case 'O':
            tios.c_cflag |= PARENB;
            tios.c_cflag |= PARODD;
            break;
        case 'N': default:
            tios.c_cflag &= ~PARENB;
            break;
    }

    /* 设置停止位 */
    if (ctx->stop_bits == 2) {
        tios.c_cflag |= CSTOPB;
    } else {
        tios.c_cflag &= ~CSTOPB;
    }

    /* 启用接收 */
    tios.c_cflag |= CREAD | CLOCAL;

    /* 应用配置 */
    tcsetattr(ctx->fd, TCSANOW, &tios);
    tcflush(ctx->fd, TCIOFLUSH);

    printf("[MODBUS] RTU connected: %s@%d %d%c%d\n",
           ctx->device, ctx->baudrate, ctx->data_bits, ctx->parity, ctx->stop_bits);

    return MODBUS_OK;
}

/**
 * @brief 连接TCP设备
 */
int modbus_connect_tcp(modbus_context_t *ctx)
{
    /* 创建socket */
    ctx->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (ctx->fd < 0) {
        perror("socket");
        return MODBUS_ERR_CONNECTION;
    }

    /* 配置地址 */
    memset(&ctx->tcp_addr, 0, sizeof(ctx->tcp_addr));
    ctx->tcp_addr.sin_family = AF_INET;
    ctx->tcp_addr.sin_port = htons(ctx->tcp_port);
    inet_pton(AF_INET, ctx->ip_address, &ctx->tcp_addr.sin_addr);

    /* 连接 */
    if (connect(ctx->fd, (struct sockaddr *)&ctx->tcp_addr,
                sizeof(ctx->tcp_addr)) < 0) {
        perror("connect");
        close(ctx->fd);
        ctx->fd = -1;
        return MODBUS_ERR_CONNECTION;
    }

    printf("[MODBUS] TCP connected: %s:%d\n", ctx->ip_address, ctx->tcp_port);

    return MODBUS_OK;
}

/**
 * @brief 连接
 */
int modbus_connect(modbus_context_t *ctx)
{
    if (!ctx) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    if (ctx->mode == MODBUS_RTU) {
        return modbus_connect_rtu(ctx);
    } else {
        return modbus_connect_tcp(ctx);
    }
}

/**
 * @brief 关闭连接
 */
void modbus_close(modbus_context_t *ctx)
{
    if (ctx && ctx->fd >= 0) {
        close(ctx->fd);
        ctx->fd = -1;
    }
}

/**
 * @brief 释放上下文
 */
void modbus_free(modbus_context_t *ctx)
{
    if (ctx) {
        modbus_close(ctx);
        free(ctx);
    }
}

/**
 * @brief 发送和接收数据 (RTU)
 */
static int modbus_send_recv_rtu(modbus_context_t *ctx,
                                 const uint8_t *req, int req_len,
                                 uint8_t *rsp, int rsp_max)
{
    uint8_t adu[MODBUS_MAX_ADU_LENGTH];
    int adu_len = 0;

    /* 构建ADU: 从站地址 + PDU + CRC */
    adu[0] = ctx->slave_id;
    memcpy(adu + 1, req, req_len);
    adu_len = 1 + req_len;

    /* 计算CRC */
    uint16_t crc = modbus_crc16(adu, adu_len);
    adu[adu_len++] = crc & 0xFF;
    adu[adu_len++] = (crc >> 8) & 0xFF;

    /* 清除输入缓冲区 */
    tcflush(ctx->fd, TCIFLUSH);

    /* 发送 */
    int sent = write(ctx->fd, adu, adu_len);
    if (sent != adu_len) {
        return MODBUS_ERR_CONNECTION;
    }
    ctx->tx_count++;

    /* 等待响应 (简化处理) */
    usleep(ctx->response_timeout_ms * 1000);

    /* 接收 */
    int received = read(ctx->fd, adu, sizeof(adu));
    if (received < 5) {
        return MODBUS_ERR_TIMEOUT;
    }
    ctx->rx_count++;

    /* 验证从站地址 */
    if (adu[0] != ctx->slave_id) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    /* 验证CRC */
    crc = modbus_crc16(adu, received - 2);
    if ((adu[received - 2] != (crc & 0xFF)) ||
        (adu[received - 1] != ((crc >> 8) & 0xFF))) {
        ctx->error_count++;
        return MODBUS_ERR_CRC;
    }

    /* 提取PDU */
    int pdu_len = received - 3;  /* - 从站地址 - CRC2 */
    if (pdu_len > rsp_max) {
        pdu_len = rsp_max;
    }
    memcpy(rsp, adu + 1, pdu_len);

    return pdu_len;
}

/**
 * @brief 发送和接收数据 (TCP)
 */
static int modbus_send_recv_tcp(modbus_context_t *ctx,
                                 const uint8_t *req, int req_len,
                                 uint8_t *rsp, int rsp_max)
{
    uint8_t adu[MODBUS_MAX_ADU_LENGTH];
    int adu_len = 0;

    /* 构建MBAP头: 事务ID(2) + 协议ID(2) + 长度(2) + 单元ID(1) */
    static uint16_t transaction_id = 0;
    adu[0] = (transaction_id >> 8) & 0xFF;
    adu[1] = transaction_id & 0xFF;
    transaction_id++;
    adu[2] = 0x00;  /* 协议ID高字节 */
    adu[3] = 0x00;  /* 协议ID低字节 (Modbus) */
    adu[4] = ((req_len + 1) >> 8) & 0xFF;
    adu[5] = (req_len + 1) & 0xFF;
    adu[6] = ctx->slave_id;

    memcpy(adu + 7, req, req_len);
    adu_len = 7 + req_len;

    /* 发送 */
    int sent = send(ctx->fd, adu, adu_len, 0);
    if (sent != adu_len) {
        return MODBUS_ERR_CONNECTION;
    }
    ctx->tx_count++;

    /* 接收MBAP头 */
    int received = recv(ctx->fd, adu, 7, MSG_WAITALL);
    if (received != 7) {
        return MODBUS_ERR_TIMEOUT;
    }

    /* 获取数据长度 */
    int data_len = (adu[4] << 8) | adu[5];
    data_len -= 1;  /* 减去单元ID */

    if (data_len > rsp_max) {
        data_len = rsp_max;
    }

    /* 接收数据 */
    received = recv(ctx->fd, rsp, data_len, MSG_WAITALL);
    if (received != data_len) {
        return MODBUS_ERR_TIMEOUT;
    }
    ctx->rx_count++;

    return data_len;
}

/**
 * @brief 读取线圈
 */
int modbus_read_bits(modbus_context_t *ctx, int addr, int nb, uint8_t *dest)
{
    if (!ctx || ctx->fd < 0 || nb > 2000) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t req[5];
    uint8_t rsp[MODBUS_MAX_PDU_LENGTH];

    req[0] = MODBUS_FC_READ_COILS;
    req[1] = (addr >> 8) & 0xFF;
    req[2] = addr & 0xFF;
    req[3] = (nb >> 8) & 0xFF;
    req[4] = nb & 0xFF;

    int rsp_len;
    if (ctx->mode == MODBUS_RTU) {
        rsp_len = modbus_send_recv_rtu(ctx, req, 5, rsp, sizeof(rsp));
    } else {
        rsp_len = modbus_send_recv_tcp(ctx, req, 5, rsp, sizeof(rsp));
    }

    if (rsp_len < 0) {
        return rsp_len;
    }

    /* 检查异常 */
    if (rsp[0] & 0x80) {
        ctx->error_count++;
        return MODBUS_ERR_EXCEPTION;
    }

    /* 提取数据 */
    int byte_count = rsp[1];
    memcpy(dest, rsp + 2, byte_count);

    return nb;
}

/**
 * @brief 读取保持寄存器
 */
int modbus_read_registers(modbus_context_t *ctx, int addr, int nb, uint16_t *dest)
{
    if (!ctx || ctx->fd < 0 || nb > 125) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t req[5];
    uint8_t rsp[MODBUS_MAX_PDU_LENGTH];

    req[0] = MODBUS_FC_READ_HOLDING_REGISTERS;
    req[1] = (addr >> 8) & 0xFF;
    req[2] = addr & 0xFF;
    req[3] = (nb >> 8) & 0xFF;
    req[4] = nb & 0xFF;

    int rsp_len;
    if (ctx->mode == MODBUS_RTU) {
        rsp_len = modbus_send_recv_rtu(ctx, req, 5, rsp, sizeof(rsp));
    } else {
        rsp_len = modbus_send_recv_tcp(ctx, req, 5, rsp, sizeof(rsp));
    }

    if (rsp_len < 0) {
        return rsp_len;
    }

    /* 检查异常 */
    if (rsp[0] & 0x80) {
        ctx->error_count++;
        return MODBUS_ERR_EXCEPTION;
    }

    /* 提取数据 */
    int byte_count = rsp[1];
    for (int i = 0; i < byte_count / 2; i++) {
        dest[i] = (rsp[2 + i * 2] << 8) | rsp[3 + i * 2];
    }

    return nb;
}

/**
 * @brief 写入单个寄存器
 */
int modbus_write_register(modbus_context_t *ctx, int addr, const uint16_t value)
{
    if (!ctx || ctx->fd < 0) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t req[5];
    uint8_t rsp[MODBUS_MAX_PDU_LENGTH];

    req[0] = MODBUS_FC_WRITE_SINGLE_REGISTER;
    req[1] = (addr >> 8) & 0xFF;
    req[2] = addr & 0xFF;
    req[3] = (value >> 8) & 0xFF;
    req[4] = value & 0xFF;

    int rsp_len;
    if (ctx->mode == MODBUS_RTU) {
        rsp_len = modbus_send_recv_rtu(ctx, req, 5, rsp, sizeof(rsp));
    } else {
        rsp_len = modbus_send_recv_tcp(ctx, req, 5, rsp, sizeof(rsp));
    }

    if (rsp_len < 0) {
        return rsp_len;
    }

    return (rsp[0] & 0x80) ? MODBUS_ERR_EXCEPTION : 1;
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║          Modbus Master Demo Application                  ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* RTU示例 */
    printf("[DEMO] RTU Mode Example\n");
    printf("=======================\n");

    modbus_context_t *ctx_rtu = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N', 8, 1);
    if (ctx_rtu) {
        modbus_set_slave(ctx_rtu, 1);

        if (modbus_connect(ctx_rtu) == MODBUS_OK) {
            /* 读取保持寄存器 */
            uint16_t regs[10];
            int ret = modbus_read_registers(ctx_rtu, 0, 4, regs);
            if (ret > 0) {
                printf("[RTU] Read registers: ");
                for (int i = 0; i < ret; i++) {
                    printf("%.4X ", regs[i]);
                }
                printf("\n");
            }

            modbus_close(ctx_rtu);
        }

        modbus_free(ctx_rtu);
    }

    printf("\n");

    /* TCP示例 */
    printf("[DEMO] TCP Mode Example\n");
    printf("=======================\n");

    modbus_context_t *ctx_tcp = modbus_new_tcp("192.168.1.100", 502);
    if (ctx_tcp) {
        modbus_set_slave(ctx_tcp, 1);

        if (modbus_connect(ctx_tcp) == MODBUS_OK) {
            /* 读取线圈 */
            uint8_t bits[16];
            int ret = modbus_read_bits(ctx_tcp, 0, 16, bits);
            if (ret > 0) {
                printf("[TCP] Read coils: ");
                for (int i = 0; i < ret; i++) {
                    printf("%d", (bits[i / 8] >> (i % 8)) & 1);
                }
                printf("\n");
            }

            /* 读取寄存器 */
            uint16_t regs[10];
            ret = modbus_read_registers(ctx_tcp, 0, 4, regs);
            if (ret > 0) {
                printf("[TCP] Read registers: ");
                for (int i = 0; i < ret; i++) {
                    printf("%.4X ", regs[i]);
                }
                printf("\n");
            }

            /* 写入寄存器 */
            ret = modbus_write_register(ctx_tcp, 0, 0x1234);
            printf("[TCP] Write register: %s\n",
                   ret > 0 ? "OK" : "FAILED");

            modbus_close(ctx_tcp);
        }

        modbus_free(ctx_tcp);
    }

    printf("\n[DEMO] Completed\n");

    return 0;
}
```


### 6.2 CANopen通信

```c
/**
 * @file canopen_node.c
 * @brief CANopen节点实现
 * @details 使用SocketCAN实现CANopen协议通信
 *
 * 编译: gcc -o canopen_node canopen_node.c
 * 运行: sudo ./canopen_node (需要root权限访问CAN接口)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>

/* CANopen Object Dictionary索引 */
#define OD_DEVICE_TYPE          0x1000
#define OD_ERROR_REGISTER       0x1001
#define OD_MANUFACTURER_STATUS  0x1002
#define OD_PRE_DEFINED_ERROR    0x1003
#define OD_COB_ID_SYNC          0x1005
#define OD_COMMUNICATION_CYCLE  0x1006
#define OD_MANUFACTURER_NAME    0x1018
#define OD_IDENTITY             0x1018
#define OD_STORE_PARAMETERS     0x1010
#define OD_RESTORE_PARAMETERS   0x1011

/* CANopen COB-ID定义 */
#define COB_ID_NMT              0x000
#define COB_ID_SYNC             0x080
#define COB_ID_EMCY_BASE        0x080
#define COB_ID_TPDO1_BASE       0x180
#define COB_ID_RPDO1_BASE       0x200
#define COB_ID_TPDO2_BASE       0x280
#define COB_ID_RPDO2_BASE       0x300
#define COB_ID_TPDO3_BASE       0x380
#define COB_ID_RPDO3_BASE       0x400
#define COB_ID_TPDO4_BASE       0x480
#define COB_ID_RPDO4_BASE       0x500
#define COB_ID_TSDO_BASE        0x580
#define COB_ID_RSDO_BASE        0x600
#define COB_ID_HEARTBEAT_BASE   0x700

/* NMT命令 */
#define NMT_CMD_START           0x01
#define NMT_CMD_STOP            0x02
#define NMT_CMD_PREOP           0x80
#define NMT_CMD_RESET_NODE      0x81
#define NMT_CMD_RESET_COMM      0x82

/* SDO命令字节 */
#define SDO_CCS_DOWNLOAD_INIT   1
#define SDO_CCS_DOWNLOAD_SEG    0
#define SDO_CCS_UPLOAD_INIT     2
#define SDO_CCS_UPLOAD_SEG      3
#define SDO_CCS_ABORT           4

#define SDO_SCS_UPLOAD_INIT     2
#define SDO_SCS_UPLOAD_SEG      0
#define SDO_SCS_DOWNLOAD_INIT   3
#define SDO_SCS_DOWNLOAD_SEG    1

/* 节点状态 */
typedef enum {
    NMT_STATE_BOOTUP = 0,
    NMT_STATE_STOPPED = 4,
    NMT_STATE_OPERATIONAL = 5,
    NMT_STATE_PREOPERATIONAL = 127
} canopen_nmt_state_t;

/* Object Dictionary条目 */
typedef struct {
    uint16_t index;
    uint8_t subindex;
    uint8_t access_type;    /* 0=RO, 1=WO, 2=RW, 3=CONST */
    uint8_t data_type;
    uint32_t default_value;
    uint32_t value;
    uint32_t min_value;
    uint32_t max_value;
    const char *description;
} od_entry_t;

/* CANopen节点上下文 */
typedef struct {
    int socket_fd;
    uint8_t node_id;
    canopen_nmt_state_t state;
    struct sockaddr_can addr;
    struct ifreq ifr;

    /* Object Dictionary */
    od_entry_t *od;
    uint16_t od_size;

    /* PDO配置 */
    uint32_t tpdo1_cob_id;
    uint32_t rpdo1_cob_id;
    uint16_t tpdo1_mapping[8];
    uint8_t tpdo1_mapping_count;

    /* 心跳 */
    uint16_t heartbeat_time;
    uint32_t last_heartbeat;

    /* 统计 */
    uint32_t tx_count;
    uint32_t rx_count;
    uint32_t error_count;
} canopen_node_t;

/* 默认Object Dictionary */
static od_entry_t default_od[] = {
    /* 设备信息 */
    {0x1000, 0x00, 0, 0x07, 0x00020192, 0x00020192, 0, 0xFFFFFFFF, "Device type"},
    {0x1001, 0x00, 0, 0x05, 0x00, 0x00, 0, 0xFF, "Error register"},
    {0x1008, 0x00, 3, 0x09, 0, 0, 0, 0, "Manufacturer device name"},

    /* 身份 */
    {0x1018, 0x00, 0, 0x05, 0x04, 0x04, 0, 0xFF, "Identity object entries"},
    {0x1018, 0x01, 0, 0x07, 0x00000000, 0x00000000, 0, 0xFFFFFFFF, "Vendor ID"},
    {0x1018, 0x02, 0, 0x07, 0x00000001, 0x00000001, 0, 0xFFFFFFFF, "Product code"},
    {0x1018, 0x03, 0, 0x07, 0x00000001, 0x00000001, 0, 0xFFFFFFFF, "Revision number"},
    {0x1018, 0x04, 0, 0x07, 0x00000000, 0x00000000, 0, 0xFFFFFFFF, "Serial number"},

    /* 心跳时间 */
    {0x1017, 0x00, 2, 0x06, 1000, 1000, 0, 65535, "Producer heartbeat time"},

    /* TPDO1通信参数 */
    {0x1800, 0x00, 0, 0x05, 0x05, 0x05, 0, 0xFF, "TPDO1 entries"},
    {0x1800, 0x01, 2, 0x07, 0x40000180, 0x40000180, 0, 0xFFFFFFFF, "TPDO1 COB-ID"},
    {0x1800, 0x02, 2, 0x05, 0xFE, 0xFE, 0, 0xFF, "TPDO1 transmission type"},
    {0x1800, 0x03, 2, 0x06, 0, 0, 0, 65535, "TPDO1 inhibit time"},
    {0x1800, 0x05, 2, 0x06, 0, 0, 0, 65535, "TPDO1 event timer"},

    /* TPDO1映射参数 */
    {0x1A00, 0x00, 0, 0x05, 0x02, 0x02, 0, 0x08, "TPDO1 mapped objects"},
    {0x1A00, 0x01, 2, 0x07, 0x60000108, 0x60000108, 0, 0xFFFFFFFF, "Map: 6000:01"},
    {0x1A00, 0x02, 2, 0x07, 0x60000210, 0x60000210, 0, 0xFFFFFFFF, "Map: 6000:02"},

    /* 应用数据 */
    {0x6000, 0x00, 0, 0x05, 0x02, 0x02, 0, 0xFF, "Application data entries"},
    {0x6000, 0x01, 2, 0x05, 0x00, 0x00, 0, 0xFF, "Digital inputs"},
    {0x6000, 0x02, 2, 0x06, 0x0000, 0x0000, 0, 0xFFFF, "Analog input 1"},
    {0x6000, 0x03, 2, 0x06, 0x0000, 0x0000, 0, 0xFFFF, "Analog input 2"},

    {0x6200, 0x00, 0, 0x05, 0x02, 0x02, 0, 0xFF, "Output data entries"},
    {0x6200, 0x01, 2, 0x05, 0x00, 0x00, 0, 0xFF, "Digital outputs"},
    {0x6200, 0x02, 2, 0x06, 0x0000, 0x0000, 0, 0xFFFF, "Analog output 1"},
};

#define DEFAULT_OD_SIZE (sizeof(default_od) / sizeof(default_od[0]))

/**
 * @brief 创建CANopen节点
 */
canopen_node_t *canopen_create_node(uint8_t node_id, const char *interface)
{
    canopen_node_t *node = calloc(1, sizeof(canopen_node_t));
    if (!node) {
        return NULL;
    }

    node->node_id = node_id;
    node->state = NMT_STATE_BOOTUP;

    /* 复制Object Dictionary */
    node->od_size = DEFAULT_OD_SIZE;
    node->od = malloc(sizeof(default_od));
    if (!node->od) {
        free(node);
        return NULL;
    }
    memcpy(node->od, default_od, sizeof(default_od));

    /* 创建socket */
    node->socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (node->socket_fd < 0) {
        perror("socket");
        free(node->od);
        free(node);
        return NULL;
    }

    /* 配置接口 */
    strncpy(node->ifr.ifr_name, interface, IFNAMSIZ - 1);
    if (ioctl(node->socket_fd, SIOCGIFINDEX, &node->ifr) < 0) {
        perror("ioctl");
        close(node->socket_fd);
        free(node->od);
        free(node);
        return NULL;
    }

    node->addr.can_family = AF_CAN;
    node->addr.can_ifindex = node->ifr.ifr_ifindex;

    /* 绑定 */
    if (bind(node->socket_fd, (struct sockaddr *)&node->addr,
             sizeof(node->addr)) < 0) {
        perror("bind");
        close(node->socket_fd);
        free(node->od);
        free(node);
        return NULL;
    }

    /* 配置接收过滤器 */
    struct can_filter rfilter[4];
    rfilter[0].can_id = COB_ID_NMT;
    rfilter[0].can_mask = CAN_EFF_MASK;
    rfilter[1].can_id = COB_ID_RSDO_BASE + node_id;
    rfilter[1].can_mask = CAN_EFF_MASK;
    rfilter[2].can_id = COB_ID_RPDO1_BASE + node_id;
    rfilter[2].can_mask = CAN_EFF_MASK;
    rfilter[3].can_id = COB_ID_SYNC;
    rfilter[3].can_mask = CAN_EFF_MASK;

    setsockopt(node->socket_fd, SOL_CAN_RAW, CAN_RAW_FILTER,
               &rfilter, sizeof(rfilter));

    printf("[CANopen] Node %d created on interface %s\n", node_id, interface);

    return node;
}

/**
 * @brief 销毁CANopen节点
 */
void canopen_destroy_node(canopen_node_t *node)
{
    if (node) {
        if (node->socket_fd >= 0) {
            close(node->socket_fd);
        }
        free(node->od);
        free(node);
    }
}

/**
 * @brief 发送CAN帧
 */
int canopen_send(canopen_node_t *node, uint32_t cob_id,
                  const uint8_t *data, uint8_t len)
{
    struct can_frame frame;

    frame.can_id = cob_id;
    frame.can_dlc = len;
    memcpy(frame.data, data, len);

    int ret = write(node->socket_fd, &frame, sizeof(frame));
    if (ret == sizeof(frame)) {
        node->tx_count++;
        return 0;
    }

    node->error_count++;
    return -1;
}

/**
 * @brief 接收CAN帧
 */
int canopen_receive(canopen_node_t *node, struct can_frame *frame,
                     int timeout_ms)
{
    struct timeval tv;
    fd_set rdfs;

    FD_ZERO(&rdfs);
    FD_SET(node->socket_fd, &rdfs);

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int ret = select(node->socket_fd + 1, &rdfs, NULL, NULL, &tv);
    if (ret < 0) {
        return -1;
    }
    if (ret == 0) {
        return 0;  /* 超时 */
    }

    ret = read(node->socket_fd, frame, sizeof(*frame));
    if (ret == sizeof(*frame)) {
        node->rx_count++;
        return 1;
    }

    return -1;
}

/**
 * @brief 发送心跳报文
 */
void canopen_send_heartbeat(canopen_node_t *node)
{
    uint8_t data = node->state;
    canopen_send(node, COB_ID_HEARTBEAT_BASE + node->node_id, &data, 1);
}

/**
 * @brief 发送启动报文
 */
void canopen_send_bootup(canopen_node_t *node)
{
    uint8_t data = 0;  /* Boot-up state */
    canopen_send(node, COB_ID_HEARTBEAT_BASE + node->node_id, &data, 1);
    printf("[CANopen] Node %d boot-up\n", node->node_id);
}

/**
 * @brief 发送TPDO
 */
void canopen_send_tpdo1(canopen_node_t *node)
{
    uint8_t data[8] = {0};
    uint8_t len = 0;

    /* 根据映射打包数据 */
    for (int i = 0; i < node->od[18].value; i++) {
        uint32_t mapping = node->od[19 + i].value;
        uint16_t index = (mapping >> 16) & 0xFFFF;
        uint8_t subindex = (mapping >> 8) & 0xFF;
        uint8_t bitlen = mapping & 0xFF;

        /* 查找OD条目 */
        for (int j = 0; j < node->od_size; j++) {
            if (node->od[j].index == index && node->od[j].subindex == subindex) {
                /* 复制数据 (简化实现，假设8位对齐) */
                if (bitlen <= 8) {
                    data[len++] = node->od[j].value & 0xFF;
                } else if (bitlen <= 16) {
                    data[len++] = node->od[j].value & 0xFF;
                    data[len++] = (node->od[j].value >> 8) & 0xFF;
                }
                break;
            }
        }
    }

    canopen_send(node, COB_ID_TPDO1_BASE + node->node_id, data, len);
}

/**
 * @brief 查找OD条目
 */
od_entry_t *canopen_find_od(canopen_node_t *node, uint16_t index, uint8_t subindex)
{
    for (int i = 0; i < node->od_size; i++) {
        if (node->od[i].index == index && node->od[i].subindex == subindex) {
            return &node->od[i];
        }
    }
    return NULL;
}

/**
 * @brief 处理SDO上传请求
 */
void canopen_handle_sdo_upload(canopen_node_t *node, const struct can_frame *req)
{
    uint8_t scs = (req->data[0] >> 5) & 0x07;
    uint16_t index = req->data[1] | (req->data[2] << 8);
    uint8_t subindex = req->data[3];

    struct can_frame rsp;
    rsp.can_id = COB_ID_TSDO_BASE + node->node_id;
    rsp.can_dlc = 8;
    memset(rsp.data, 0, 8);

    od_entry_t *entry = canopen_find_od(node, index, subindex);

    if (!entry) {
        /* 对象不存在 */
        rsp.data[0] = 0x80;
        rsp.data[1] = req->data[1];
        rsp.data[2] = req->data[2];
        rsp.data[3] = req->data[3];
        rsp.data[4] = 0x06;  /* Object does not exist */
        rsp.data[5] = 0x02;
        rsp.data[6] = 0x00;
        rsp.data[7] = 0x00;
    } else if (entry->access_type == 1) {
        /* 只写对象 */
        rsp.data[0] = 0x80;
        rsp.data[1] = req->data[1];
        rsp.data[2] = req->data[2];
        rsp.data[3] = req->data[3];
        rsp.data[4] = 0x01;  /* Attempt to read a write only object */
        rsp.data[5] = 0x00;
        rsp.data[6] = 0x02;
        rsp.data[7] = 0x00;
    } else {
        /* 响应上传请求 */
        rsp.data[0] = (SDO_SCS_UPLOAD_INIT << 5) |
                      ((4 - entry->data_type / 8) << 2) | 0x02;
        rsp.data[1] = req->data[1];
        rsp.data[2] = req->data[2];
        rsp.data[3] = req->data[3];

        /* 复制数据 */
        if (entry->data_type <= 8) {
            rsp.data[4] = entry->value & 0xFF;
        } else if (entry->data_type <= 16) {
            rsp.data[4] = entry->value & 0xFF;
            rsp.data[5] = (entry->value >> 8) & 0xFF;
        } else {
            rsp.data[4] = entry->value & 0xFF;
            rsp.data[5] = (entry->value >> 8) & 0xFF;
            rsp.data[6] = (entry->value >> 16) & 0xFF;
            rsp.data[7] = (entry->value >> 24) & 0xFF;
        }
    }

    write(node->socket_fd, &rsp, sizeof(rsp));
    node->tx_count++;
}

/**
 * @brief 处理SDO下载请求
 */
void canopen_handle_sdo_download(canopen_node_t *node, const struct can_frame *req)
{
    uint16_t index = req->data[1] | (req->data[2] << 8);
    uint8_t subindex = req->data[3];

    struct can_frame rsp;
    rsp.can_id = COB_ID_TSDO_BASE + node->node_id;
    rsp.can_dlc = 8;
    memset(rsp.data, 0, 8);

    od_entry_t *entry = canopen_find_od(node, index, subindex);

    if (!entry) {
        rsp.data[0] = 0x80;
        rsp.data[4] = 0x06;
        rsp.data[5] = 0x02;
    } else if (entry->access_type == 0) {
        /* 只读对象 */
        rsp.data[0] = 0x80;
        rsp.data[4] = 0x02;
        rsp.data[5] = 0x00;
        rsp.data[6] = 0x02;
    } else {
        /* 更新值 */
        uint32_t value = req->data[4] | (req->data[5] << 8) |
                        (req->data[6] << 16) | (req->data[7] << 24);

        if (value >= entry->min_value && value <= entry->max_value) {
            entry->value = value;
            rsp.data[0] = (SDO_SCS_DOWNLOAD_INIT << 5) | 0x02;
        } else {
            rsp.data[0] = 0x80;
            rsp.data[4] = 0x30;
            rsp.data[5] = 0x00;
            rsp.data[6] = 0x02;
        }
    }

    rsp.data[1] = req->data[1];
    rsp.data[2] = req->data[2];
    rsp.data[3] = req->data[3];

    write(node->socket_fd, &rsp, sizeof(rsp));
    node->tx_count++;
}

/**
 * @brief 处理NMT命令
 */
void canopen_handle_nmt(canopen_node_t *node, const struct can_frame *frame)
{
    uint8_t cmd = frame->data[0];
    uint8_t target_id = frame->data[1];

    /* 检查是否针对本节点或所有节点 */
    if (target_id != 0 && target_id != node->node_id) {
        return;
    }

    switch (cmd) {
        case NMT_CMD_START:
            node->state = NMT_STATE_OPERATIONAL;
            printf("[CANopen] Node %d -> OPERATIONAL\n", node->node_id);
            break;

        case NMT_CMD_STOP:
            node->state = NMT_STATE_STOPPED;
            printf("[CANopen] Node %d -> STOPPED\n", node->node_id);
            break;

        case NMT_CMD_PREOP:
            node->state = NMT_STATE_PREOPERATIONAL;
            printf("[CANopen] Node %d -> PRE-OPERATIONAL\n", node->node_id);
            break;

        case NMT_CMD_RESET_NODE:
            printf("[CANopen] Node %d -> RESET NODE\n", node->node_id);
            /* 重置节点 */
            break;

        case NMT_CMD_RESET_COMM:
            printf("[CANopen] Node %d -> RESET COMMUNICATION\n", node->node_id);
            /* 重置通信 */
            break;
    }
}

/**
 * @brief 处理接收到的报文
 */
void canopen_process_frame(canopen_node_t *node, const struct can_frame *frame)
{
    uint32_t cob_id = frame->can_id;

    if (cob_id == COB_ID_NMT) {
        /* NMT命令 */
        canopen_handle_nmt(node, frame);
    } else if (cob_id == COB_ID_RSDO_BASE + node->node_id) {
        /* SDO请求 */
        uint8_t ccs = (frame->data[0] >> 5) & 0x07;
        if (ccs == SDO_CCS_UPLOAD_INIT) {
            canopen_handle_sdo_upload(node, frame);
        } else if (ccs == SDO_CCS_DOWNLOAD_INIT) {
            canopen_handle_sdo_download(node, frame);
        }
    } else if (cob_id == COB_ID_RPDO1_BASE + node->node_id) {
        /* RPDO1接收 */
        printf("[CANopen] RPDO1 received: ");
        for (int i = 0; i < frame->can_dlc; i++) {
            printf("%.2X ", frame->data[i]);
        }
        printf("\n");
    } else if (cob_id == COB_ID_SYNC) {
        /* SYNC接收 - 触发TPDO */
        canopen_send_tpdo1(node);
    }
}

/**
 * @brief 主循环
 */
void canopen_run(canopen_node_t *node)
{
    struct can_frame frame;
    struct timeval last_heartbeat, now;

    gettimeofday(&last_heartbeat, NULL);

    /* 发送boot-up报文 */
    canopen_send_bootup(node);
    node->state = NMT_STATE_PREOPERATIONAL;

    /* 读取心跳时间 */
    od_entry_t *hb_entry = canopen_find_od(node, 0x1017, 0x00);
    node->heartbeat_time = hb_entry ? hb_entry->value : 1000;

    printf("[CANopen] Node %d running (heartbeat: %d ms)\n",
           node->node_id, node->heartbeat_time);

    while (1) {
        /* 接收报文 */
        int ret = canopen_receive(node, &frame, 10);
        if (ret > 0) {
            canopen_process_frame(node, &frame);
        }

        /* 发送心跳 */
        gettimeofday(&now, NULL);
        long elapsed = (now.tv_sec - last_heartbeat.tv_sec) * 1000 +
                       (now.tv_usec - last_heartbeat.tv_usec) / 1000;

        if (elapsed >= node->heartbeat_time) {
            canopen_send_heartbeat(node);
            last_heartbeat = now;
        }

        /* 模拟数据变化 */
        static int counter = 0;
        if (++counter % 100 == 0) {
            /* 更新模拟输入值 */
            od_entry_t *ai1 = canopen_find_od(node, 0x6000, 0x02);
            if (ai1) {
                ai1->value = (uint16_t)(rand() % 4096);
            }
        }
    }
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║          CANopen Node Demo Application                   ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("[INFO] This demo requires a CAN interface (e.g., can0)\n");
    printf("[INFO] Setup: ip link set can0 up type can bitrate 500000\n\n");

    /* 创建节点 */
    canopen_node_t *node = canopen_create_node(1, "can0");
    if (!node) {
        fprintf(stderr, "[ERROR] Failed to create CANopen node\n");
        fprintf(stderr, "[INFO] Make sure CAN interface is available\n");
        return 1;
    }

    /* 运行节点 */
    canopen_run(node);

    /* 清理 */
    canopen_destroy_node(node);

    return 0;
}
```

---

## 7. 数据采集系统

### 7.1 高性能数据采集架构

```
┌─────────────────────────────────────────────────────────────────┐
│                   工业数据采集系统架构                          │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                   数据采集层                           │   │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐   │   │
│  │  │ 传感器   │ │ 仪表     │ │ PLC      │ │ DCS      │   │   │
│  │  │ 接口     │ │ 接口     │ │ 接口     │ │ 接口     │   │   │
│  │  └──────────┘ └──────────┘ └──────────┘ └──────────┘   │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                   协议适配层                           │   │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐   │   │
│  │  │ Modbus   │ │ OPC UA   │ │ EtherCAT │ │ 专用协议 │   │   │
│  │  │ RTU/TCP  │ │ Client   │ │ 主站     │ │          │   │   │
│  │  └──────────┘ └──────────┘ └──────────┘ └──────────┘   │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                   数据处理层                           │   │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐   │   │
│  │  │ 数据缓存 │ │ 数据滤波 │ │ 数据转换 │ │ 数据压缩 │   │   │
│  │  │ 队列     │ │ 算法     │ │ 引擎     │ │ 算法     │   │   │
│  │  └──────────┘ └──────────┘ └──────────┘ └──────────┘   │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                   存储转发层                           │   │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐   │   │
│  │  │ 本地存储 │ │ 时序数据库│ │ 消息队列 │ │ 云网关   │   │   │
│  │  │ SQLite   │ │ InfluxDB │ │ MQTT     │ │ 上传     │   │   │
│  │  └──────────┘ └──────────┘ └──────────┘ └──────────┘   │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

### 7.2 环形缓冲区实现

```c
/**
 * @file ring_buffer.c
 * @brief 无锁环形缓冲区实现
 * @details 用于高性能数据采集的多生产者单消费者无锁队列
 *
 * 编译: gcc -o ring_buffer ring_buffer.c -lpthread
 * 运行: ./ring_buffer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>

/* 缓存行大小 */
#define CACHE_LINE_SIZE     64

/* 数据点结构 */
typedef struct {
    uint64_t timestamp_ns;      /* 纳秒时间戳 */
    uint32_t tag_id;            /* 测点ID */
    double value;               /* 数值 */
    uint8_t quality;            /* 质量码 (0=Good, 1=Bad) */
    uint8_t reserved[7];        /* 对齐到32字节 */
} __attribute__((packed, aligned(32))) data_point_t;

/* 环形缓冲区结构 */
typedef struct {
    /* 生产者位置 - 独立缓存行 */
    alignas(CACHE_LINE_SIZE)
    atomic_size_t head;

    /* 消费者位置 - 独立缓存行 */
    alignas(CACHE_LINE_SIZE)
    atomic_size_t tail;

    /* 缓冲区大小 - 2的幂次方 */
    size_t capacity;
    size_t mask;

    /* 溢出计数 */
    atomic_size_t overflow_count;

    /* 数据缓冲区 */
    data_point_t *buffer;
} ring_buffer_t;

/* 统计信息 */
typedef struct {
    uint64_t produced;
    uint64_t consumed;
    uint64_t dropped;
    double avg_latency_ns;
    double max_latency_ns;
} buffer_stats_t;

/**
 * @brief 创建环形缓冲区
 * @param capacity 容量 (必须是2的幂次方)
 * @return 缓冲区指针
 */
ring_buffer_t *ring_buffer_create(size_t capacity)
{
    /* 确保容量是2的幂次方 */
    if ((capacity & (capacity - 1)) != 0) {
        fprintf(stderr, "[ERROR] Capacity must be power of 2\n");
        return NULL;
    }

    ring_buffer_t *rb = aligned_alloc(CACHE_LINE_SIZE, sizeof(ring_buffer_t));
    if (!rb) {
        return NULL;
    }

    memset(rb, 0, sizeof(ring_buffer_t));
    rb->capacity = capacity;
    rb->mask = capacity - 1;

    /* 分配缓冲区内存 */
    rb->buffer = aligned_alloc(64, capacity * sizeof(data_point_t));
    if (!rb->buffer) {
        free(rb);
        return NULL;
    }

    memset(rb->buffer, 0, capacity * sizeof(data_point_t));

    atomic_init(&rb->head, 0);
    atomic_init(&rb->tail, 0);
    atomic_init(&rb->overflow_count, 0);

    return rb;
}

/**
 * @brief 销毁环形缓冲区
 */
void ring_buffer_destroy(ring_buffer_t *rb)
{
    if (rb) {
        free(rb->buffer);
        free(rb);
    }
}

/**
 * @brief 检查缓冲区是否为空
 */
static inline bool ring_buffer_is_empty(ring_buffer_t *rb)
{
    return atomic_load_explicit(&rb->head, memory_order_acquire) ==
           atomic_load_explicit(&rb->tail, memory_order_acquire);
}

/**
 * @brief 检查缓冲区是否已满
 */
static inline bool ring_buffer_is_full(ring_buffer_t *rb)
{
    size_t head = atomic_load_explicit(&rb->head, memory_order_acquire);
    size_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire);
    return ((head - tail) & rb->mask) == rb->mask;
}

/**
 * @brief 获取已用空间
 */
static inline size_t ring_buffer_used(ring_buffer_t *rb)
{
    size_t head = atomic_load_explicit(&rb->head, memory_order_acquire);
    size_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire);
    return (head - tail) & rb->mask;
}

/**
 * @brief 获取可用空间
 */
static inline size_t ring_buffer_available(ring_buffer_t *rb)
{
    return rb->capacity - ring_buffer_used(rb) - 1;
}

/**
 * @brief 写入单个数据点
 * @return 成功返回true，失败返回false
 */
bool ring_buffer_push(ring_buffer_t *rb, const data_point_t *point)
{
    size_t head = atomic_load_explicit(&rb->head, memory_order_relaxed);
    size_t next_head = (head + 1) & rb->mask;
    size_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire);

    /* 检查是否已满 */
    if (next_head == tail) {
        atomic_fetch_add_explicit(&rb->overflow_count, 1, memory_order_relaxed);
        return false;
    }

    /* 写入数据 */
    rb->buffer[head] = *point;

    /* 更新head (发布语义) */
    atomic_store_explicit(&rb->head, next_head, memory_order_release);

    return true;
}

/**
 * @brief 批量写入数据点
 * @return 实际写入数量
 */
size_t ring_buffer_push_batch(ring_buffer_t *rb, const data_point_t *points,
                               size_t count)
{
    size_t written = 0;

    for (size_t i = 0; i < count; i++) {
        if (!ring_buffer_push(rb, &points[i])) {
            break;
        }
        written++;
    }

    return written;
}

/**
 * @brief 读取单个数据点
 * @return 成功返回true，失败返回false
 */
bool ring_buffer_pop(ring_buffer_t *rb, data_point_t *point)
{
    size_t tail = atomic_load_explicit(&rb->tail, memory_order_relaxed);
    size_t head = atomic_load_explicit(&rb->head, memory_order_acquire);

    /* 检查是否为空 */
    if (head == tail) {
        return false;
    }

    /* 读取数据 */
    *point = rb->buffer[tail];

    /* 更新tail (发布语义) */
    size_t next_tail = (tail + 1) & rb->mask;
    atomic_store_explicit(&rb->tail, next_tail, memory_order_release);

    return true;
}

/**
 * @brief 批量读取数据点
 * @return 实际读取数量
 */
size_t ring_buffer_pop_batch(ring_buffer_t *rb, data_point_t *points,
                              size_t max_count)
{
    size_t read = 0;

    for (size_t i = 0; i < max_count; i++) {
        if (!ring_buffer_pop(rb, &points[i])) {
            break;
        }
        read++;
    }

    return read;
}

/* 测试配置 */
#define BUFFER_SIZE         (1024 * 1024)   /* 1M数据点 */
#define PRODUCER_COUNT      4
#define TEST_DURATION_SEC   10
#define POINTS_PER_BATCH    32

static ring_buffer_t *g_rb = NULL;
static atomic_size_t g_produced_count = 0;
static atomic_size_t g_consumed_count = 0;
static atomic_size_t g_dropped_count = 0;

/**
 * @brief 获取当前时间 (纳秒)
 */
static inline uint64_t get_time_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

/**
 * @brief 生产者线程
 */
static void *producer_thread(void *arg)
{
    int producer_id = (int)(intptr_t)arg;
    data_point_t batch[POINTS_PER_BATCH];
    uint64_t tag_base = producer_id * 1000;

    printf("[PRODUCER%d] Started\n", producer_id);

    uint64_t start_time = get_time_ns();

    while (true) {
        /* 准备数据 */
        for (int i = 0; i < POINTS_PER_BATCH; i++) {
            batch[i].timestamp_ns = get_time_ns();
            batch[i].tag_id = tag_base + (i % 100);
            batch[i].value = (double)(rand() % 10000) / 100.0;
            batch[i].quality = 0;
        }

        /* 批量写入 */
        size_t written = ring_buffer_push_batch(g_rb, batch, POINTS_PER_BATCH);

        if (written == POINTS_PER_BATCH) {
            atomic_fetch_add(&g_produced_count, written);
        } else {
            atomic_fetch_add(&g_produced_count, written);
            atomic_fetch_add(&g_dropped_count, POINTS_PER_BATCH - written);
        }

        /* 检查超时 */
        if (get_time_ns() - start_time > TEST_DURATION_SEC * 1000000000ULL) {
            break;
        }

        /* 模拟工作负载 */
        // usleep(1);
    }

    printf("[PRODUCER%d] Stopped\n", producer_id);
    return NULL;
}

/**
 * @brief 消费者线程
 */
static void *consumer_thread(void *arg)
{
    (void)arg;
    data_point_t batch[POINTS_PER_BATCH * 4];

    printf("[CONSUMER] Started\n");

    uint64_t start_time = get_time_ns();

    while (true) {
        /* 批量读取 */
        size_t read = ring_buffer_pop_batch(g_rb, batch, POINTS_PER_BATCH * 4);

        if (read > 0) {
            atomic_fetch_add(&g_consumed_count, read);

            /* 模拟处理 */
            for (size_t i = 0; i < read; i++) {
                volatile double processed = batch[i].value * 1.5;
                (void)processed;
            }
        } else {
            /* 缓冲区空，短暂等待 */
            usleep(10);
        }

        /* 检查超时 */
        if (get_time_ns() - start_time > TEST_DURATION_SEC * 1000000000ULL + 1000000000ULL) {
            break;
        }
    }

    /* 消费剩余数据 */
    while (true) {
        size_t read = ring_buffer_pop_batch(g_rb, batch, POINTS_PER_BATCH * 4);
        if (read == 0) break;
        atomic_fetch_add(&g_consumed_count, read);
    }

    printf("[CONSUMER] Stopped\n");
    return NULL;
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║       Lock-Free Ring Buffer Performance Test             ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* 创建环形缓冲区 */
    g_rb = ring_buffer_create(BUFFER_SIZE);
    if (!g_rb) {
        fprintf(stderr, "[ERROR] Failed to create ring buffer\n");
        return 1;
    }

    printf("[INFO] Buffer size: %zu data points (%zu MB)\n",
           BUFFER_SIZE, BUFFER_SIZE * sizeof(data_point_t) / (1024 * 1024));
    printf("[INFO] Producers: %d\n", PRODUCER_COUNT);
    printf("[INFO] Test duration: %d seconds\n\n", TEST_DURATION_SEC);

    /* 锁定内存 */
    mlockall(MCL_CURRENT | MCL_FUTURE);

    /* 创建线程 */
    pthread_t producers[PRODUCER_COUNT];
    pthread_t consumer;

    uint64_t test_start = get_time_ns();

    /* 启动生产者 */
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_create(&producers[i], NULL, producer_thread,
                       (void *)(intptr_t)i);
    }

    /* 启动消费者 */
    pthread_create(&consumer, NULL, consumer_thread, NULL);

    /* 等待测试完成 */
    sleep(TEST_DURATION_SEC + 1);

    /* 等待线程结束 */
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_join(producers[i], NULL);
    }
    pthread_join(consumer, NULL);

    uint64_t test_end = get_time_ns();
    double test_duration = (test_end - test_start) / 1e9;

    /* 获取统计 */
    size_t produced = atomic_load(&g_produced_count);
    size_t consumed = atomic_load(&g_consumed_count);
    size_t dropped = atomic_load(&g_dropped_count);
    size_t overflow = atomic_load(&g_rb->overflow_count);

    /* 打印结果 */
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║                  Test Results                            ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ Test Duration:     %.2f seconds                          ║\n", test_duration);
    printf("║ Points Produced:   %zu                                   ║\n", produced);
    printf("║ Points Consumed:   %zu                                   ║\n", consumed);
    printf("║ Points Dropped:    %zu                                   ║\n", dropped);
    printf("║ Overflow Count:    %zu                                   ║\n", overflow);
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ Throughput:         %.2f M points/second                ║\n",
           produced / test_duration / 1e6);
    printf("║ Throughput:         %.2f MB/second                      ║\n",
           (produced * sizeof(data_point_t)) / test_duration / (1024 * 1024));
    printf("║ Drop Rate:          %.4f%%                              ║\n",
           dropped * 100.0 / produced);
    printf("╚══════════════════════════════════════════════════════════╝\n");

    /* 清理 */
    ring_buffer_destroy(g_rb);

    return 0;
}
```

---

## 8. 边缘计算框架

### 8.1 边缘计算架构

```
┌─────────────────────────────────────────────────────────────────┐
│                    边缘计算架构                                 │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                   应用服务层                           │   │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐   │   │
│  │  │ 预测性维护│ │ 质量检测 │ │ 能耗优化 │ │ 远程监控 │   │   │
│  │  └──────────┘ └──────────┘ └──────────┘ └──────────┘   │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                   分析处理层                           │   │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐   │   │
│  │  │ 流处理   │ │ 规则引擎 │ │ 机器学习 │ │ 时序分析 │   │   │
│  │  │ Flink    │ │ Drools   │ │ TensorFlow│ │ InfluxDB │   │   │
│  │  └──────────┘ └──────────┘ └──────────┘ └──────────┘   │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                   数据管理层                           │   │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐   │   │
│  │  │ 数据缓存 │ │ 数据聚合 │ │ 数据清洗 │ │ 数据路由 │   │   │
│  │  │ Redis    │ │ SQL/NoSQL│ │ ETL      │ │ MQTT/Kafka│   │   │
│  │  └──────────┘ └──────────┘ └──────────┘ └──────────┘   │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                   设备连接层                           │   │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐   │   │
│  │  │ OPC UA   │ │ Modbus   │ │ EtherCAT │ │ HTTP/REST│   │   │
│  │  │ 客户端   │ │ 主站     │ │ 主站     │ │ API      │   │   │
│  │  └──────────┘ └──────────┘ └──────────┘ └──────────┘   │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

### 8.2 边缘规则引擎

```c
/**
 * @file rule_engine.c
 * @brief 边缘规则引擎实现
 * @details 轻量级规则引擎，支持条件判断、动作执行和告警触发
 *
 * 编译: gcc -o rule_engine rule_engine.c -lm
 * 运行: ./rule_engine
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define MAX_RULES           64
#define MAX_CONDITIONS      8
#define MAX_ACTIONS         4
#define MAX_TAG_NAME        32
#define MAX_EXPRESSION      256

/* 数据类型 */
typedef enum {
    TYPE_BOOL = 0,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
} value_type_t;

/* 数据值 */
typedef struct {
    value_type_t type;
    union {
        bool b;
        int64_t i;
        double f;
        char s[64];
    } data;
    uint64_t timestamp;
    uint8_t quality;
} data_value_t;

/* 测点 */
typedef struct {
    char name[MAX_TAG_NAME];
    data_value_t value;
    void (*on_change)(struct tag *);
} tag_t;

/* 比较操作符 */
typedef enum {
    OP_EQ = 0,      /* == */
    OP_NE,          /* != */
    OP_GT,          /* > */
    OP_GE,          /* >= */
    OP_LT,          /* < */
    OP_LE,          /* <= */
    OP_IN_RANGE,    /* within range */
    OP_OUT_RANGE    /* outside range */
} operator_t;

/* 条件 */
typedef struct {
    char tag_name[MAX_TAG_NAME];
    operator_t op;
    data_value_t threshold;
    data_value_t threshold2;    /* 用于范围比较 */
    uint32_t duration_ms;       /* 持续时间要求 */
    uint64_t trigger_time;      /* 首次触发时间 */
    bool last_result;
} condition_t;

/* 动作类型 */
typedef enum {
    ACTION_LOG = 0,
    ACTION_ALARM,
    ACTION_WRITE_TAG,
    ACTION_EXEC_CMD,
    ACTION_SEND_NOTIFY
} action_type_t;

/* 动作 */
typedef struct {
    action_type_t type;
    char params[256];
    data_value_t value;
} action_t;

/* 规则 */
typedef struct {
    char name[64];
    bool enabled;
    condition_t conditions[MAX_CONDITIONS];
    uint8_t condition_count;
    bool logic_and;             /* true=AND, false=OR */
    action_t actions[MAX_ACTIONS];
    uint8_t action_count;
    bool triggered;
    uint64_t trigger_count;
    uint64_t last_trigger;
    uint32_t cooldown_ms;       /* 冷却时间 */
} rule_t;

/* 规则引擎 */
typedef struct {
    tag_t *tags;
    uint32_t tag_count;
    rule_t rules[MAX_RULES];
    uint32_t rule_count;
    void (*alarm_callback)(const char *, const char *);
} rule_engine_t;

/* 全局引擎 */
static rule_engine_t g_engine = {0};

/**
 * @brief 获取当前时间 (毫秒)
 */
static uint64_t get_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/**
 * @brief 查找测点
 */
static tag_t *find_tag(const char *name)
{
    for (uint32_t i = 0; i < g_engine.tag_count; i++) {
        if (strcmp(g_engine.tags[i].name, name) == 0) {
            return &g_engine.tags[i];
        }
    }
    return NULL;
}

/**
 * @brief 设置测点值
 */
void set_tag_value(const char *name, const data_value_t *value)
{
    tag_t *tag = find_tag(name);
    if (!tag) {
        return;
    }

    /* 检查值是否变化 */
    bool changed = false;
    if (tag->value.type != value->type) {
        changed = true;
    } else {
        switch (value->type) {
            case TYPE_BOOL:
                changed = (tag->value.data.b != value->data.b);
                break;
            case TYPE_INT:
                changed = (tag->value.data.i != value->data.i);
                break;
            case TYPE_FLOAT:
                changed = fabs(tag->value.data.f - value->data.f) > 0.0001;
                break;
            case TYPE_STRING:
                changed = strcmp(tag->value.data.s, value->data.s) != 0;
                break;
        }
    }

    tag->value = *value;
    tag->value.timestamp = get_time_ms();

    if (changed && tag->on_change) {
        tag->on_change(tag);
    }
}

/**
 * @brief 获取测点值
 */
bool get_tag_value(const char *name, data_value_t *value)
{
    tag_t *tag = find_tag(name);
    if (!tag) {
        return false;
    }
    *value = tag->value;
    return true;
}

/**
 * @brief 比较值
 */
static bool compare_values(const data_value_t *v1,
                            const data_value_t *v2,
                            operator_t op,
                            const data_value_t *v3)
{
    if (v1->type != v2->type) {
        return false;
    }

    double val1, val2, val3 = 0;

    /* 转换为浮点数比较 */
    switch (v1->type) {
        case TYPE_BOOL:
            val1 = v1->data.b ? 1.0 : 0.0;
            val2 = v2->data.b ? 1.0 : 0.0;
            if (v3) val3 = v3->data.b ? 1.0 : 0.0;
            break;
        case TYPE_INT:
            val1 = (double)v1->data.i;
            val2 = (double)v2->data.i;
            if (v3) val3 = (double)v3->data.i;
            break;
        case TYPE_FLOAT:
            val1 = v1->data.f;
            val2 = v2->data.f;
            if (v3) val3 = v3->data.f;
            break;
        case TYPE_STRING:
            int cmp = strcmp(v1->data.s, v2->data.s);
            switch (op) {
                case OP_EQ: return cmp == 0;
                case OP_NE: return cmp != 0;
                case OP_GT: return cmp > 0;
                case OP_GE: return cmp >= 0;
                case OP_LT: return cmp < 0;
                case OP_LE: return cmp <= 0;
                default: return false;
            }
    }

    switch (op) {
        case OP_EQ: return fabs(val1 - val2) < 0.0001;
        case OP_NE: return fabs(val1 - val2) >= 0.0001;
        case OP_GT: return val1 > val2;
        case OP_GE: return val1 >= val2;
        case OP_LT: return val1 < val2;
        case OP_LE: return val1 <= val2;
        case OP_IN_RANGE: return val1 >= val2 && val1 <= val3;
        case OP_OUT_RANGE: return val1 < val2 || val1 > val3;
        default: return false;
    }
}

/**
 * @brief 评估条件
 */
static bool evaluate_condition(condition_t *cond)
{
    data_value_t current;
    if (!get_tag_value(cond->tag_name, &current)) {
        return false;
    }

    bool result = compare_values(&current, &cond->threshold,
                                  cond->op,
                                  cond->op >= OP_IN_RANGE ? &cond->threshold2 : NULL);

    /* 处理持续时间 */
    if (cond->duration_ms > 0) {
        uint64_t now = get_time_ms();

        if (result) {
            if (!cond->last_result) {
                /* 首次触发 */
                cond->trigger_time = now;
                result = false;
            } else {
                /* 检查持续时间 */
                result = (now - cond->trigger_time) >= cond->duration_ms;
            }
        } else {
            cond->trigger_time = 0;
        }

        cond->last_result = result;
    }

    return result;
}

/**
 * @brief 执行动作
 */
static void execute_action(const action_t *action, const rule_t *rule)
{
    switch (action->type) {
        case ACTION_LOG:
            printf("[RULE:%s] LOG: %s\n", rule->name, action->params);
            break;

        case ACTION_ALARM:
            printf("[RULE:%s] ALARM: %s (Severity: %s)\n",
                   rule->name, action->params,
                   action->value.data.s);
            if (g_engine.alarm_callback) {
                g_engine.alarm_callback(rule->name, action->params);
            }
            break;

        case ACTION_WRITE_TAG:
            printf("[RULE:%s] WRITE: %s = ", rule->name, action->params);
            switch (action->value.type) {
                case TYPE_BOOL: printf("%s\n", action->value.data.b ? "true" : "false"); break;
                case TYPE_INT: printf("%ld\n", action->value.data.i); break;
                case TYPE_FLOAT: printf("%.2f\n", action->value.data.f); break;
                case TYPE_STRING: printf("%s\n", action->value.data.s); break;
            }
            set_tag_value(action->params, &action->value);
            break;

        case ACTION_EXEC_CMD:
            printf("[RULE:%s] EXEC: %s\n", rule->name, action->params);
            // system(action->params);
            break;

        case ACTION_SEND_NOTIFY:
            printf("[RULE:%s] NOTIFY: %s\n", rule->name, action->params);
            break;
    }
}

/**
 * @brief 评估规则
 */
static void evaluate_rule(rule_t *rule)
{
    if (!rule->enabled) {
        return;
    }

    /* 检查冷却时间 */
    if (rule->last_trigger > 0) {
        uint64_t elapsed = get_time_ms() - rule->last_trigger;
        if (elapsed < rule->cooldown_ms) {
            return;
        }
    }

    /* 评估条件 */
    bool conditions_met = rule->logic_and;

    for (uint8_t i = 0; i < rule->condition_count; i++) {
        bool result = evaluate_condition(&rule->conditions[i]);

        if (rule->logic_and) {
            conditions_met = conditions_met && result;
            if (!conditions_met) break;
        } else {
            conditions_met = conditions_met || result;
            if (conditions_met) break;
        }
    }

    /* 处理触发 */
    if (conditions_met && !rule->triggered) {
        rule->triggered = true;
        rule->trigger_count++;
        rule->last_trigger = get_time_ms();

        printf("\n[RULE:%s] TRIGGERED! (Count: %lu)\n",
               rule->name, rule->trigger_count);

        /* 执行动作 */
        for (uint8_t i = 0; i < rule->action_count; i++) {
            execute_action(&rule->actions[i], rule);
        }
    } else if (!conditions_met) {
        rule->triggered = false;
    }
}

/**
 * @brief 添加规则
 */
rule_t *add_rule(const char *name)
{
    if (g_engine.rule_count >= MAX_RULES) {
        return NULL;
    }

    rule_t *rule = &g_engine.rules[g_engine.rule_count++];
    memset(rule, 0, sizeof(rule_t));
    strncpy(rule->name, name, sizeof(rule->name) - 1);
    rule->enabled = true;
    rule->logic_and = true;
    return rule;
}

/**
 * @brief 运行规则引擎
 */
void run_rule_engine(void)
{
    for (uint32_t i = 0; i < g_engine.rule_count; i++) {
        evaluate_rule(&g_engine.rules[i]);
    }
}

/**
 * @brief 示例: 告警回调
 */
static void on_alarm(const char *rule_name, const char *message)
{
    printf("[ALARM HANDLER] Rule '%s': %s\n", rule_name, message);
    /* 这里可以发送邮件、短信等 */
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║          Edge Rule Engine Demo                           ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* 初始化引擎 */
    g_engine.alarm_callback = on_alarm;

    /* 创建测点 */
    tag_t tags[] = {
        {.name = "Temperature", .value = {.type = TYPE_FLOAT, .data.f = 25.0}},
        {.name = "Pressure", .value = {.type = TYPE_FLOAT, .data.f = 1.0}},
        {.name = "MotorSpeed", .value = {.type = TYPE_INT, .data.i = 0}},
        {.name = "AlarmActive", .value = {.type = TYPE_BOOL, .data.b = false}},
    };
    g_engine.tags = tags;
    g_engine.tag_count = sizeof(tags) / sizeof(tags[0]);

    /* 规则1: 温度超高报警 */
    rule_t *rule1 = add_rule("HighTemperatureAlarm");
    if (rule1) {
        rule1->cooldown_ms = 5000;  /* 5秒冷却 */

        /* 条件: Temperature > 80 */
        condition_t *c1 = &rule1->conditions[rule1->condition_count++];
        strncpy(c1->tag_name, "Temperature", sizeof(c1->tag_name) - 1);
        c1->op = OP_GT;
        c1->threshold.type = TYPE_FLOAT;
        c1->threshold.data.f = 80.0;
        c1->duration_ms = 2000;  /* 持续2秒 */

        /* 动作1: 记录日志 */
        action_t *a1 = &rule1->actions[rule1->action_count++];
        a1->type = ACTION_LOG;
        strncpy(a1->params, "Temperature exceeds threshold", sizeof(a1->params) - 1);

        /* 动作2: 触发告警 */
        action_t *a2 = &rule1->actions[rule1->action_count++];
        a2->type = ACTION_ALARM;
        strncpy(a2->params, "High temperature detected", sizeof(a2->params) - 1);
        a2->value.type = TYPE_STRING;
        strncpy(a2->value.data.s, "CRITICAL", sizeof(a2->value.data.s) - 1);

        /* 动作3: 设置告警标志 */
        action_t *a3 = &rule1->actions[rule1->action_count++];
        a3->type = ACTION_WRITE_TAG;
        strncpy(a3->params, "AlarmActive", sizeof(a3->params) - 1);
        a3->value.type = TYPE_BOOL;
        a3->value.data.b = true;
    }

    /* 规则2: 压力范围检查 */
    rule_t *rule2 = add_rule("PressureRangeCheck");
    if (rule2) {
        /* 条件: Pressure < 0.5 OR Pressure > 2.0 */
        condition_t *c1 = &rule2->conditions[rule2->condition_count++];
        strncpy(c1->tag_name, "Pressure", sizeof(c1->tag_name) - 1);
        c1->op = OP_OUT_RANGE;
        c1->threshold.type = TYPE_FLOAT;
        c1->threshold.data.f = 0.5;
        c1->threshold2.type = TYPE_FLOAT;
        c1->threshold2.data.f = 2.0;

        /* 动作 */
        action_t *a1 = &rule2->actions[rule2->action_count++];
        a1->type = ACTION_LOG;
        strncpy(a1->params, "Pressure out of range", sizeof(a1->params) - 1);
    }

    /* 规则3: 电机启动 */
    rule_t *rule3 = add_rule("MotorControl");
    if (rule3) {
        /* 条件: Temperature < 60 AND Pressure < 1.5 */
        condition_t *c1 = &rule3->conditions[rule3->condition_count++];
        strncpy(c1->tag_name, "Temperature", sizeof(c1->tag_name) - 1);
        c1->op = OP_LT;
        c1->threshold.type = TYPE_FLOAT;
        c1->threshold.data.f = 60.0;

        condition_t *c2 = &rule3->conditions[rule3->condition_count++];
        strncpy(c2->tag_name, "Pressure", sizeof(c2->tag_name) - 1);
        c2->op = OP_LT;
        c2->threshold.type = TYPE_FLOAT;
        c2->threshold.data.f = 1.5;

        /* 动作: 启动电机 */
        action_t *a1 = &rule3->actions[rule3->action_count++];
        a1->type = ACTION_WRITE_TAG;
        strncpy(a1->params, "MotorSpeed", sizeof(a1->params) - 1);
        a1->value.type = TYPE_INT;
        a1->value.data.i = 1500;
    }

    printf("[INFO] %d rules created\n\n", g_engine.rule_count);

    /* 模拟数据变化并评估规则 */
    printf("=== Simulation Start ===\n\n");

    /* 场景1: 正常状态 */
    printf("[SCENE 1] Normal state\n");
    data_value_t v;
    v.type = TYPE_FLOAT; v.data.f = 25.0; set_tag_value("Temperature", &v);
    v.type = TYPE_FLOAT; v.data.f = 1.0; set_tag_value("Pressure", &v);
    run_rule_engine();
    printf("MotorSpeed: %ld\n\n", tags[2].value.data.i);

    /* 场景2: 温度升高 */
    printf("[SCENE 2] Temperature rising...\n");
    for (int i = 0; i < 10; i++) {
        v.type = TYPE_FLOAT;
        v.data.f = 25.0 + i * 6.0;  /* 25 -> 79 */
        set_tag_value("Temperature", &v);
        run_rule_engine();
        usleep(100000);
    }
    printf("\n");

    /* 场景3: 温度超过阈值 */
    printf("[SCENE 3] Temperature exceeds threshold\n");
    v.type = TYPE_FLOAT; v.data.f = 85.0; set_tag_value("Temperature", &v);
    run_rule_engine();
    usleep(2500000);  /* 等待2.5秒 */
    run_rule_engine();  /* 再次评估，触发持续条件 */
    printf("\n");

    /* 场景4: 压力异常 */
    printf("[SCENE 4] Pressure anomaly\n");
    v.type = TYPE_FLOAT; v.data.f = 0.3; set_tag_value("Pressure", &v);
    run_rule_engine();
    printf("\n");

    /* 场景5: 恢复正常 */
    printf("[SCENE 5] Return to normal\n");
    v.type = TYPE_FLOAT; v.data.f = 55.0; set_tag_value("Temperature", &v);
    v.type = TYPE_FLOAT; v.data.f = 1.2; set_tag_value("Pressure", &v);
    v.type = TYPE_BOOL; v.data.b = false; set_tag_value("AlarmActive", &v);
    run_rule_engine();
    printf("MotorSpeed: %ld\n\n", tags[2].value.data.i);

    printf("=== Simulation End ===\n");

    return 0;
}
```

---

## 9. 系统可靠性设计

### 9.1 可靠性架构

```
┌─────────────────────────────────────────────────────────────────┐
│                   可靠性设计层次                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  1. 硬件可靠性                                            │  │
│  │  - 冗余电源 (双路供电 + UPS)                              │  │
│  │  - ECC内存 (错误检测与纠正)                               │  │
│  │  - 工业级存储 (SLC SSD, 写保护)                          │  │
│  │  - 看门狗定时器 (硬件/软件)                               │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  2. 软件可靠性                                            │  │
│  │  - 进程监控 (systemd/watchdog)                           │  │
│  │  - 内存保护 (mprotect, ASLR)                             │  │
│  │  - 异常处理 (信号处理、核心转储)                          │  │
│  │  - 资源限制 (cgroups, ulimit)                            │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  3. 数据可靠性                                            │  │
│  │  - 双缓冲机制 (乒乓缓冲)                                  │  │
│  │  - 数据校验 (CRC, 校验和)                                 │  │
│  │  - 日志冗余 (WAL, 循环日志)                               │  │
│  │  - 持久化存储 (fsync, 原子写入)                           │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  4. 通信可靠性                                            │  │
│  │  - 心跳检测 (keepalive)                                   │  │
│  │  - 超时重传 (指数退避)                                    │  │
│  │  - 断线重连 (自动恢复)                                    │  │
│  │  - 数据缓冲 (离线缓存)                                    │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 9.2 看门狗与进程监控

```c
/**
 * @file reliability_framework.c
 * @brief 系统可靠性框架实现
 * @details 包含看门狗、心跳检测、故障恢复等功能
 *
 * 编译: gcc -o reliability_framework reliability_framework.c
 * 运行: sudo ./reliability_framework
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>

/* 看门狗配置 */
#define WATCHDOG_DEV        "/dev/watchdog"
#define WATCHDOG_TIMEOUT    30      /* 看门狗超时时间(秒) */

/* 心跳配置 */
#define HEARTBEAT_INTERVAL_MS   1000    /* 心跳间隔 */
#define HEARTBEAT_TIMEOUT_MS    5000    /* 心跳超时 */

/* 健康检查项 */
typedef enum {
    HEALTH_CPU = 0,
    HEALTH_MEMORY,
    HEALTH_DISK,
    HEALTH_NETWORK,
    HEALTH_APPLICATION,
    HEALTH_COUNT
} health_item_t;

/* 健康状态 */
typedef enum {
    STATUS_HEALTHY = 0,
    STATUS_WARNING,
    STATUS_CRITICAL,
    STATUS_FAILED
} health_status_t;

/* 健康检查结构 */
typedef struct {
    health_item_t item;
    health_status_t status;
    char description[128];
    uint64_t last_check;
    uint64_t failure_count;
} health_check_t;

/* 看门狗上下文 */
typedef struct {
    int fd;
    int timeout;
    bool enabled;
} watchdog_t;

/* 心跳上下文 */
typedef struct {
    uint64_t last_beat;
    uint64_t interval_ms;
    uint64_t timeout_ms;
    bool alive;
} heartbeat_t;

/* 可靠性管理器 */
typedef struct {
    watchdog_t watchdog;
    heartbeat_t heartbeat;
    health_check_t health_checks[HEALTH_COUNT];
    bool running;
    void (*on_failure)(health_item_t, const char *);
    void (*on_recovery)(health_item_t);
} reliability_manager_t;

static reliability_manager_t g_manager = {0};

/**
 * @brief 获取当前时间 (毫秒)
 */
static uint64_t get_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/**
 * @brief 初始化硬件看门狗
 */
static int watchdog_init(watchdog_t *wd)
{
    wd->fd = open(WATCHDOG_DEV, O_RDWR);
    if (wd->fd < 0) {
        fprintf(stderr, "[WATCHDOG] Failed to open %s: %s\n",
                WATCHDOG_DEV, strerror(errno));
        fprintf(stderr, "[WATCHDOG] Running in software-only mode\n");
        wd->enabled = false;
        return 0;  /* 软看门狗模式 */
    }

    /* 获取当前超时时间 */
    int timeout;
    if (ioctl(wd->fd, WDIOC_GETTIMEOUT, &timeout) == 0) {
        printf("[WATCHDOG] Current timeout: %d seconds\n", timeout);
    }

    /* 设置超时时间 */
    wd->timeout = WATCHDOG_TIMEOUT;
    if (ioctl(wd->fd, WDIOC_SETTIMEOUT, &wd->timeout) == 0) {
        printf("[WATCHDOG] Timeout set to %d seconds\n", wd->timeout);
    }

    /* 获取支持的功能 */
    struct watchdog_info info;
    if (ioctl(wd->fd, WDIOC_GETSUPPORT, &info) == 0) {
        printf("[WATCHDOG] Identity: %s\n", info.identity);
    }

    wd->enabled = true;
    printf("[WATCHDOG] Hardware watchdog initialized\n");

    return 0;
}

/**
 * @brief 喂狗
 */
static void watchdog_feed(watchdog_t *wd)
{
    if (wd->enabled && wd->fd >= 0) {
        int ret = ioctl(wd->fd, WDIOC_KEEPALIVE, 0);
        if (ret < 0) {
            perror("WDIOC_KEEPALIVE");
        }
    }
}

/**
 * @brief 禁用看门狗 (正常关闭时)
 */
static void watchdog_disable(watchdog_t *wd)
{
    if (wd->enabled && wd->fd >= 0) {
        /* 发送 'V' 魔法字符禁用看门狗 */
        write(wd->fd, "V", 1);
        close(wd->fd);
        wd->enabled = false;
        printf("[WATCHDOG] Disabled\n");
    }
}

/**
 * @brief 检查CPU健康
 */
static health_status_t check_cpu_health(void)
{
    FILE *fp = fopen("/proc/loadavg", "r");
    if (!fp) {
        return STATUS_FAILED;
    }

    double load1, load5, load15;
    if (fscanf(fp, "%lf %lf %lf", &load1, &load5, &load15) != 3) {
        fclose(fp);
        return STATUS_FAILED;
    }
    fclose(fp);

    /* 简单判断: 1分钟负载 > 8 为严重, > 4 为警告 */
    if (load1 > 8.0) {
        return STATUS_CRITICAL;
    } else if (load1 > 4.0) {
        return STATUS_WARNING;
    }

    return STATUS_HEALTHY;
}

/**
 * @brief 检查内存健康
 */
static health_status_t check_memory_health(void)
{
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        return STATUS_FAILED;
    }

    char line[256];
    uint64_t total = 0, available = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "MemTotal: %lu", &total) == 1) {
            /* kB */
        }
        if (sscanf(line, "MemAvailable: %lu", &available) == 1) {
            /* kB */
        }
    }
    fclose(fp);

    if (total == 0) {
        return STATUS_FAILED;
    }

    /* 计算可用内存比例 */
    double ratio = (double)available / total;

    if (ratio < 0.1) {
        return STATUS_CRITICAL;
    } else if (ratio < 0.2) {
        return STATUS_WARNING;
    }

    return STATUS_HEALTHY;
}

/**
 * @brief 检查磁盘健康
 */
static health_status_t check_disk_health(void)
{
    /* 检查根分区可用空间 */
    FILE *fp = popen("df / | tail -1 | awk '{print $5}' | tr -d '%'", "r");
    if (!fp) {
        return STATUS_FAILED;
    }

    int usage;
    if (fscanf(fp, "%d", &usage) != 1) {
        pclose(fp);
        return STATUS_FAILED;
    }
    pclose(fp);

    if (usage > 95) {
        return STATUS_CRITICAL;
    } else if (usage > 85) {
        return STATUS_WARNING;
    }

    return STATUS_HEALTHY;
}

/**
 * @brief 检查网络健康
 */
static health_status_t check_network_health(void)
{
    /* 简单检查: 默认路由是否存在 */
    FILE *fp = fopen("/proc/net/route", "r");
    if (!fp) {
        return STATUS_FAILED;
    }

    char line[256];
    bool has_default_route = false;

    /* 跳过标题行 */
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        char iface[16];
        unsigned int dest;
        if (sscanf(line, "%s %x", iface, &dest) == 2) {
            if (dest == 0) {  /* 默认路由 */
                has_default_route = true;
                break;
            }
        }
    }
    fclose(fp);

    return has_default_route ? STATUS_HEALTHY : STATUS_WARNING;
}

/**
 * @brief 执行健康检查
 */
static void perform_health_checks(reliability_manager_t *mgr)
{
    uint64_t now = get_time_ms();

    /* CPU */
    health_status_t cpu_status = check_cpu_health();
    if (cpu_status != mgr->health_checks[HEALTH_CPU].status) {
        if (cpu_status == STATUS_HEALTHY) {
            if (mgr->on_recovery) {
                mgr->on_recovery(HEALTH_CPU);
            }
        } else {
            if (mgr->on_failure) {
                mgr->on_failure(HEALTH_CPU, "CPU load high");
            }
        }
    }
    mgr->health_checks[HEALTH_CPU].status = cpu_status;
    mgr->health_checks[HEALTH_CPU].last_check = now;

    /* Memory */
    health_status_t mem_status = check_memory_health();
    if (mem_status != mgr->health_checks[HEALTH_MEMORY].status) {
        if (mem_status == STATUS_HEALTHY) {
            if (mgr->on_recovery) mgr->on_recovery(HEALTH_MEMORY);
        } else {
            if (mgr->on_failure) mgr->on_failure(HEALTH_MEMORY, "Memory low");
        }
    }
    mgr->health_checks[HEALTH_MEMORY].status = mem_status;
    mgr->health_checks[HEALTH_MEMORY].last_check = now;

    /* Disk */
    health_status_t disk_status = check_disk_health();
    mgr->health_checks[HEALTH_DISK].status = disk_status;
    mgr->health_checks[HEALTH_DISK].last_check = now;

    /* Network */
    health_status_t net_status = check_network_health();
    mgr->health_checks[HEALTH_NETWORK].status = net_status;
    mgr->health_checks[HEALTH_NETWORK].last_check = now;
}

/**
 * @brief 打印健康状态
 */
static void print_health_status(const reliability_manager_t *mgr)
{
    const char *status_str[] = {"HEALTHY", "WARNING", "CRITICAL", "FAILED"};
    const char *item_str[] = {"CPU", "Memory", "Disk", "Network", "Application"};

    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║              System Health Status                        ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");

    for (int i = 0; i < HEALTH_COUNT; i++) {
        printf("║ %-12s: %-10s\n",
               item_str[i],
               status_str[mgr->health_checks[i].status]);
    }

    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ Watchdog:    %s\n",
           mgr->watchdog.enabled ? "ENABLED" : "SOFTWARE ONLY");
    printf("║ Heartbeat:    %s\n",
           mgr->heartbeat.alive ? "ALIVE" : "TIMEOUT");
    printf("╚══════════════════════════════════════════════════════════╝\n");
}

/**
 * @brief 故障回调
 */
static void on_health_failure(health_item_t item, const char *desc)
{
    const char *item_str[] = {"CPU", "Memory", "Disk", "Network", "Application"};
    fprintf(stderr, "[HEALTH] %s FAILURE: %s\n", item_str[item], desc);

    /* 可以在这里添加告警通知 */
}

/**
 * @brief 恢复回调
 */
static void on_health_recovery(health_item_t item)
{
    const char *item_str[] = {"CPU", "Memory", "Disk", "Network", "Application"};
    printf("[HEALTH] %s RECOVERED\n", item_str[item]);
}

/**
 * @brief 信号处理
 */
static void signal_handler(int sig)
{
    (void)sig;
    printf("\n[INFO] Shutting down gracefully...\n");
    g_manager.running = false;
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║        System Reliability Framework Demo                 ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* 初始化 */
    g_manager.on_failure = on_health_failure;
    g_manager.on_recovery = on_health_recovery;
    g_manager.heartbeat.interval_ms = HEARTBEAT_INTERVAL_MS;
    g_manager.heartbeat.timeout_ms = HEARTBEAT_TIMEOUT_MS;
    g_manager.running = true;

    /* 初始化看门狗 */
    if (watchdog_init(&g_manager.watchdog) != 0) {
        fprintf(stderr, "[ERROR] Failed to initialize watchdog\n");
    }

    /* 注册信号处理 */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    printf("[INFO] System reliability monitoring started\n");
    printf("[INFO] Press Ctrl+C to stop\n\n");

    uint64_t last_health_check = 0;
    uint64_t last_status_print = 0;

    /* 主循环 */
    while (g_manager.running) {
        uint64_t now = get_time_ms();

        /* 喂狗 */
        watchdog_feed(&g_manager.watchdog);
        g_manager.heartbeat.last_beat = now;
        g_manager.heartbeat.alive = true;

        /* 执行健康检查 (每5秒) */
        if (now - last_health_check >= 5000) {
            perform_health_checks(&g_manager);
            last_health_check = now;
        }

        /* 打印状态 (每10秒) */
        if (now - last_status_print >= 10000) {
            print_health_status(&g_manager);
            last_status_print = now;
        }

        usleep(100000);  /* 100ms */
    }

    /* 清理 */
    watchdog_disable(&g_manager.watchdog);

    printf("\n[INFO] System reliability monitoring stopped\n");

    return 0;
}
```

---

## 10. 实际项目:工业数据采集器(完整代码)

### 10.1 项目概述

这是一个完整的工业数据采集器实现，支持多种工业协议、数据缓存、边缘计算和云端上传。

```
┌─────────────────────────────────────────────────────────────────┐
│               工业数据采集器架构                                │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐             │
│  │ Modbus RTU  │  │ Modbus TCP  │  │ OPC UA      │             │
│  │ 采集器      │  │ 采集器      │  │ 客户端      │             │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘             │
│         │                │                │                     │
│         └────────────────┼────────────────┘                     │
│                          ▼                                      │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │                    数据总线 (消息队列)                    │  │
│  └──────────────────────────────────────────────────────────┘  │
│                          │                                      │
│         ┌────────────────┼────────────────┐                    │
│         ▼                ▼                ▼                    │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐             │
│  │ 规则引擎    │  │ 本地存储    │  │ MQTT上传    │             │
│  │ (边缘计算)  │  │ (SQLite)    │  │ (云平台)    │             │
│  └─────────────┘  └─────────────┘  └─────────────┘             │
└─────────────────────────────────────────────────────────────────┘
```


### 10.2 工业数据采集器完整代码

```c
/**
 * @file industrial_data_collector.c
 * @brief 工业数据采集器 - 完整实现
 * @details
 *   - 支持Modbus RTU/TCP设备采集
 *   - 支持OPC UA设备采集
 *   - 本地SQLite数据存储
 *   - 边缘规则计算
 *   - MQTT云端上传
 *   - 配置热加载
 *   - 完善的日志系统
 *
 * 编译: gcc -o collector industrial_data_collector.c \
 *        -lsqlite3 -lmosquitto -lpthread -lrt -lm
 *
 * 运行: sudo ./collector -c config.json
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <sqlite3.h>
#include <mosquitto.h>

/* ============ 版本与配置 ============ */
#define COLLECTOR_VERSION   "2.1.0"
#define DEFAULT_CONFIG_FILE "/etc/industrial_collector/config.json"
#define LOG_FILE            "/var/log/industrial_collector/collector.log"
#define DB_FILE             "/var/lib/industrial_collector/data.db"

/* 采集参数 */
#define MAX_DEVICES         32
#define MAX_TAGS_PER_DEVICE 128
#define MAX_TAG_NAME        64
#define MAX_BATCH_SIZE      1000
#define COLLECTOR_QUEUE_SIZE (1024 * 1024)

/* 日志级别 */
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
} log_level_t;

static log_level_t g_log_level = LOG_INFO;
static FILE *g_log_file = NULL;
static pthread_mutex_t g_log_mutex = PTHREAD_MUTEX_INITIALIZER;

/* ============ 数据结构定义 ============ */

/* 数据质量 */
typedef enum {
    QUALITY_GOOD = 0,
    QUALITY_BAD = 1,
    QUALITY_UNCERTAIN = 2,
    QUALITY_LAST_KNOWN = 3
} data_quality_t;

/* 数据点 */
typedef struct {
    char tag_name[MAX_TAG_NAME];
    uint32_t device_id;
    uint32_t tag_id;
    double value;
    data_quality_t quality;
    uint64_t timestamp_ms;
    uint64_t collection_time_ms;
} data_point_t;

/* 环形缓冲区 */
typedef struct {
    data_point_t *buffer;
    size_t capacity;
    size_t head;
    size_t tail;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    size_t overflow_count;
} collector_queue_t;

/* 设备类型 */
typedef enum {
    DEVICE_MODBUS_RTU = 0,
    DEVICE_MODBUS_TCP,
    DEVICE_OPC_UA,
    DEVICE_SIMULATION  /* 用于测试 */
} device_type_t;

/* 设备配置 */
typedef struct {
    uint32_t id;
    char name[64];
    device_type_t type;
    bool enabled;

    /* 连接参数 */
    union {
        struct {
            char port[64];
            int baudrate;
            char parity;
            int data_bits;
            int stop_bits;
            uint8_t slave_id;
        } modbus_rtu;

        struct {
            char host[64];
            int port;
            uint8_t slave_id;
        } modbus_tcp;

        struct {
            char endpoint[128];
            char username[64];
            char password[64];
        } opc_ua;
    } connection;

    /* 采集参数 */
    uint32_t scan_interval_ms;
    uint32_t timeout_ms;
    uint32_t retry_count;

    /* 标签列表 */
    struct {
        char name[MAX_TAG_NAME];
        char address[32];
        uint16_t register_addr;
        uint8_t register_type;  /* 0=线圈, 1=输入, 3=保持寄存器, 4=输入寄存器 */
        double scale;
        double offset;
        bool enabled;
    } tags[MAX_TAGS_PER_DEVICE];
    uint32_t tag_count;

    /* 运行时状态 */
    pthread_t thread;
    bool running;
    uint32_t error_count;
    uint64_t last_scan_time;
} device_t;

/* 系统配置 */
typedef struct {
    /* 采集配置 */
    uint32_t collection_interval_ms;
    uint32_t batch_size;

    /* 存储配置 */
    bool local_storage_enabled;
    uint32_t retention_days;

    /* MQTT配置 */
    bool mqtt_enabled;
    char mqtt_broker[128];
    int mqtt_port;
    char mqtt_client_id[64];
    char mqtt_username[64];
    char mqtt_password[64];
    char mqtt_topic_prefix[64];
    uint32_t mqtt_qos;

    /* 日志配置 */
    char log_level[16];
    char log_file[256];
    uint32_t max_log_size_mb;
    uint32_t max_log_files;
} system_config_t;

/* 全局上下文 */
typedef struct {
    system_config_t config;
    device_t devices[MAX_DEVICES];
    uint32_t device_count;

    collector_queue_t data_queue;

    sqlite3 *db;
    struct mosquitto *mosq;
    bool mqtt_connected;

    pthread_t storage_thread;
    pthread_t mqtt_thread;
    pthread_t monitor_thread;

    volatile bool running;
    uint64_t start_time;

    /* 统计 */
    uint64_t total_collected;
    uint64_t total_stored;
    uint64_t total_sent;
    uint64_t total_dropped;
} collector_context_t;

static collector_context_t g_ctx = {0};

/* ============ 日志系统 ============ */

static const char *log_level_str[] = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};

static void logger_init(void)
{
    g_log_file = fopen(LOG_FILE, "a");
    if (!g_log_file) {
        g_log_file = stdout;
    }
}

static void logger_close(void)
{
    if (g_log_file && g_log_file != stdout) {
        fclose(g_log_file);
    }
}

static void log_message(log_level_t level, const char *format, ...)
{
    if (level < g_log_level) {
        return;
    }

    pthread_mutex_lock(&g_log_mutex);

    /* 获取时间 */
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm tm;
    localtime_r(&ts.tv_sec, &tm);

    /* 打印时间戳和级别 */
    fprintf(g_log_file, "[%04d-%02d-%02d %02d:%02d:%02d.%03d] [%s] ",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec, (int)(ts.tv_nsec / 1000000),
            log_level_str[level]);

    /* 打印消息 */
    va_list args;
    va_start(args, format);
    vfprintf(g_log_file, format, args);
    va_end(args);

    fprintf(g_log_file, "\n");
    fflush(g_log_file);

    pthread_mutex_unlock(&g_log_mutex);
}

#define LOG_DEBUG(...)   log_message(LOG_DEBUG, __VA_ARGS__)
#define LOG_INFO(...)    log_message(LOG_INFO, __VA_ARGS__)
#define LOG_WARNING(...) log_message(LOG_WARNING, __VA_ARGS__)
#define LOG_ERROR(...)   log_message(LOG_ERROR, __VA_ARGS__)
#define LOG_FATAL(...)   log_message(LOG_FATAL, __VA_ARGS__)

/* ============ 环形缓冲区实现 ============ */

static int queue_init(collector_queue_t *queue, size_t capacity)
{
    queue->buffer = calloc(capacity, sizeof(data_point_t));
    if (!queue->buffer) {
        return -1;
    }
    queue->capacity = capacity;
    queue->head = 0;
    queue->tail = 0;
    queue->overflow_count = 0;

    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);

    return 0;
}

static void queue_destroy(collector_queue_t *queue)
{
    free(queue->buffer);
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
}

static size_t queue_size(collector_queue_t *queue)
{
    pthread_mutex_lock(&queue->lock);
    size_t size = (queue->head - queue->tail) % queue->capacity;
    pthread_mutex_unlock(&queue->lock);
    return size;
}

static bool queue_push(collector_queue_t *queue, const data_point_t *point)
{
    pthread_mutex_lock(&queue->lock);

    size_t next_head = (queue->head + 1) % queue->capacity;
    if (next_head == queue->tail) {
        /* 队列满 */
        queue->overflow_count++;
        pthread_mutex_unlock(&queue->lock);
        return false;
    }

    queue->buffer[queue->head] = *point;
    queue->head = next_head;

    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->lock);

    return true;
}

static bool queue_pop(collector_queue_t *queue, data_point_t *point, int timeout_ms)
{
    pthread_mutex_lock(&queue->lock);

    while (queue->head == queue->tail) {
        /* 队列为空，等待 */
        if (timeout_ms >= 0) {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += timeout_ms / 1000;
            ts.tv_nsec += (timeout_ms % 1000) * 1000000;
            if (ts.tv_nsec >= 1000000000) {
                ts.tv_sec++;
                ts.tv_nsec -= 1000000000;
            }

            int ret = pthread_cond_timedwait(&queue->not_empty, &queue->lock, &ts);
            if (ret == ETIMEDOUT) {
                pthread_mutex_unlock(&queue->lock);
                return false;
            }
        } else {
            pthread_cond_wait(&queue->not_empty, &queue->lock);
        }

        if (!g_ctx.running) {
            pthread_mutex_unlock(&queue->lock);
            return false;
        }
    }

    *point = queue->buffer[queue->tail];
    queue->tail = (queue->tail + 1) % queue->capacity;

    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->lock);

    return true;
}

static size_t queue_pop_batch(collector_queue_t *queue, data_point_t *points,
                               size_t max_count)
{
    pthread_mutex_lock(&queue->lock);

    size_t available = (queue->head - queue->tail) % queue->capacity;
    size_t count = (available < max_count) ? available : max_count;

    for (size_t i = 0; i < count; i++) {
        points[i] = queue->buffer[queue->tail];
        queue->tail = (queue->tail + 1) % queue->capacity;
    }

    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->lock);

    return count;
}

/* ============ 数据库操作 ============ */

static int db_init(void)
{
    int rc = sqlite3_open(DB_FILE, &g_ctx.db);
    if (rc != SQLITE_OK) {
        LOG_ERROR("Cannot open database: %s", sqlite3_errmsg(g_ctx.db));
        return -1;
    }

    /* 创建表 */
    const char *create_table_sql =
        "CREATE TABLE IF NOT EXISTS data_points ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  tag_name TEXT NOT NULL,"
        "  device_id INTEGER,"
        "  value REAL,"
        "  quality INTEGER,"
        "  timestamp_ms INTEGER,"
        "  created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_timestamp ON data_points(timestamp_ms);"
        "CREATE INDEX IF NOT EXISTS idx_tag ON data_points(tag_name);";

    char *err_msg = NULL;
    rc = sqlite3_exec(g_ctx.db, create_table_sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        LOG_ERROR("SQL error: %s", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    /* 启用WAL模式 */
    sqlite3_exec(g_ctx.db, "PRAGMA journal_mode=WAL;", NULL, NULL, NULL);
    sqlite3_exec(g_ctx.db, "PRAGMA synchronous=NORMAL;", NULL, NULL, NULL);

    LOG_INFO("Database initialized: %s", DB_FILE);
    return 0;
}

static void db_close(void)
{
    if (g_ctx.db) {
        sqlite3_close(g_ctx.db);
        g_ctx.db = NULL;
    }
}

static int db_insert_batch(const data_point_t *points, size_t count)
{
    if (!g_ctx.db || count == 0) {
        return 0;
    }

    sqlite3_exec(g_ctx.db, "BEGIN TRANSACTION;", NULL, NULL, NULL);

    const char *insert_sql = "INSERT INTO data_points "
                             "(tag_name, device_id, value, quality, timestamp_ms) "
                             "VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(g_ctx.db, insert_sql, -1, &stmt, NULL);

    for (size_t i = 0; i < count; i++) {
        sqlite3_bind_text(stmt, 1, points[i].tag_name, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, points[i].device_id);
        sqlite3_bind_double(stmt, 3, points[i].value);
        sqlite3_bind_int(stmt, 4, points[i].quality);
        sqlite3_bind_int64(stmt, 5, points[i].timestamp_ms);

        sqlite3_step(stmt);
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(g_ctx.db, "COMMIT;", NULL, NULL, NULL);

    return (int)count;
}

/* ============ MQTT操作 ============ */

static void mqtt_connect_callback(struct mosquitto *mosq, void *obj, int rc)
{
    (void)mosq;
    (void)obj;

    if (rc == 0) {
        g_ctx.mqtt_connected = true;
        LOG_INFO("MQTT connected");
    } else {
        g_ctx.mqtt_connected = false;
        LOG_ERROR("MQTT connection failed: %s", mosquitto_connack_string(rc));
    }
}

static void mqtt_disconnect_callback(struct mosquitto *mosq, void *obj, int rc)
{
    (void)mosq;
    (void)obj;

    g_ctx.mqtt_connected = false;
    LOG_WARNING("MQTT disconnected: %d", rc);
}

static int mqtt_init(void)
{
    if (!g_ctx.config.mqtt_enabled) {
        LOG_INFO("MQTT disabled");
        return 0;
    }

    mosquitto_lib_init();

    g_ctx.mosq = mosquitto_new(g_ctx.config.mqtt_client_id, true, NULL);
    if (!g_ctx.mosq) {
        LOG_ERROR("Failed to create MQTT client");
        return -1;
    }

    mosquitto_connect_callback_set(g_ctx.mosq, mqtt_connect_callback);
    mosquitto_disconnect_callback_set(g_ctx.mosq, mqtt_disconnect_callback);

    if (strlen(g_ctx.config.mqtt_username) > 0) {
        mosquitto_username_pw_set(g_ctx.mosq,
                                  g_ctx.config.mqtt_username,
                                  g_ctx.config.mqtt_password);
    }

    int rc = mosquitto_connect(g_ctx.mosq,
                               g_ctx.config.mqtt_broker,
                               g_ctx.config.mqtt_port,
                               60);
    if (rc != MOSQ_ERR_SUCCESS) {
        LOG_ERROR("MQTT connect error: %s", mosquitto_strerror(rc));
        return -1;
    }

    mosquitto_loop_start(g_ctx.mosq);
    LOG_INFO("MQTT client initialized: %s:%d",
             g_ctx.config.mqtt_broker, g_ctx.config.mqtt_port);

    return 0;
}

static void mqtt_close(void)
{
    if (g_ctx.mosq) {
        mosquitto_loop_stop(g_ctx.mosq, true);
        mosquitto_disconnect(g_ctx.mosq);
        mosquitto_destroy(g_ctx.mosq);
        g_ctx.mosq = NULL;
    }
    mosquitto_lib_cleanup();
}

static int mqtt_publish_data(const data_point_t *point)
{
    if (!g_ctx.mosq || !g_ctx.mqtt_connected) {
        return -1;
    }

    char topic[256];
    char payload[256];

    snprintf(topic, sizeof(topic), "%s/%s",
             g_ctx.config.mqtt_topic_prefix, point->tag_name);

    snprintf(payload, sizeof(payload),
             "{\"value\":%.4f,\"quality\":%d,\"timestamp\":%llu}",
             point->value, point->quality,
             (unsigned long long)point->timestamp_ms);

    int rc = mosquitto_publish(g_ctx.mosq, NULL, topic,
                               strlen(payload), payload,
                               g_ctx.config.mqtt_qos, false);

    return (rc == MOSQ_ERR_SUCCESS) ? 0 : -1;
}

/* ============ 设备采集 ============ */

static uint64_t get_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/* Modbus CRC16计算 */
static uint16_t modbus_crc16(const uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/* 模拟设备采集 (用于测试) */
static void *device_simulation_thread(void *arg)
{
    device_t *dev = (device_t *)arg;

    LOG_INFO("[Device %s] Simulation thread started", dev->name);

    /* 预分配批量缓冲区 */
    data_point_t batch[MAX_BATCH_SIZE];
    size_t batch_count = 0;

    while (dev->running && g_ctx.running) {
        uint64_t scan_start = get_time_ms();

        /* 采集所有标签 */
        for (uint32_t i = 0; i < dev->tag_count; i++) {
            if (!dev->tags[i].enabled) continue;

            /* 生成模拟数据 */
            double value;
            if (strstr(dev->tags[i].name, "Temperature")) {
                value = 20.0 + 10.0 * sin((double)scan_start / 10000.0) +
                       (double)(rand() % 100) / 100.0;
            } else if (strstr(dev->tags[i].name, "Pressure")) {
                value = 100.0 + 5.0 * cos((double)scan_start / 15000.0) +
                       (double)(rand() % 50) / 10.0;
            } else if (strstr(dev->tags[i].name, "Motor")) {
                value = (scan_start / 1000) % 2 ? 1500.0 : 0.0;
            } else {
                value = (double)(rand() % 1000);
            }

            /* 应用缩放 */
            value = value * dev->tags[i].scale + dev->tags[i].offset;

            /* 填充数据点 */
            data_point_t *point = &batch[batch_count++];
            snprintf(point->tag_name, MAX_TAG_NAME, "%s.%s",
                     dev->name, dev->tags[i].name);
            point->device_id = dev->id;
            point->tag_id = i;
            point->value = value;
            point->quality = QUALITY_GOOD;
            point->timestamp_ms = scan_start;
            point->collection_time_ms = get_time_ms();

            /* 批量提交 */
            if (batch_count >= MAX_BATCH_SIZE) {
                for (size_t j = 0; j < batch_count; j++) {
                    if (!queue_push(&g_ctx.data_queue, &batch[j])) {
                        g_ctx.total_dropped++;
                    } else {
                        g_ctx.total_collected++;
                    }
                }
                batch_count = 0;
            }
        }

        dev->last_scan_time = get_time_ms();

        /* 等待下一个周期 */
        uint64_t elapsed = get_time_ms() - scan_start;
        if (elapsed < dev->scan_interval_ms) {
            usleep((dev->scan_interval_ms - elapsed) * 1000);
        }
    }

    /* 提交剩余数据 */
    for (size_t j = 0; j < batch_count; j++) {
        if (!queue_push(&g_ctx.data_queue, &batch[j])) {
            g_ctx.total_dropped++;
        }
    }

    LOG_INFO("[Device %s] Simulation thread stopped", dev->name);
    return NULL;
}

/* ============ 后台线程 ============ */

/* 存储线程 */
static void *storage_thread(void *arg)
{
    (void)arg;

    LOG_INFO("Storage thread started");

    data_point_t batch[MAX_BATCH_SIZE];

    while (g_ctx.running) {
        /* 批量获取数据 */
        size_t count = queue_pop_batch(&g_ctx.data_queue, batch, MAX_BATCH_SIZE);

        if (count > 0) {
            /* 存储到数据库 */
            if (g_ctx.config.local_storage_enabled) {
                int stored = db_insert_batch(batch, count);
                if (stored > 0) {
                    g_ctx.total_stored += stored;
                }
            }

            /* 发送到MQTT */
            if (g_ctx.config.mqtt_enabled && g_ctx.mqtt_connected) {
                for (size_t i = 0; i < count; i++) {
                    if (mqtt_publish_data(&batch[i]) == 0) {
                        g_ctx.total_sent++;
                    }
                }
            }
        } else {
            /* 队列为空，短暂等待 */
            usleep(10000);
        }
    }

    LOG_INFO("Storage thread stopped");
    return NULL;
}

/* 监控线程 */
static void *monitor_thread(void *arg)
{
    (void)arg;

    LOG_INFO("Monitor thread started");

    while (g_ctx.running) {
        sleep(60);  /* 每分钟报告一次 */

        if (!g_ctx.running) break;

        LOG_INFO("=== Collector Statistics ===");
        LOG_INFO("  Queue size: %zu", queue_size(&g_ctx.data_queue));
        LOG_INFO("  Total collected: %llu", (unsigned long long)g_ctx.total_collected);
        LOG_INFO("  Total stored: %llu", (unsigned long long)g_ctx.total_stored);
        LOG_INFO("  Total sent: %llu", (unsigned long long)g_ctx.total_sent);
        LOG_INFO("  Total dropped: %llu", (unsigned long long)g_ctx.total_dropped);
        LOG_INFO("  MQTT connected: %s", g_ctx.mqtt_connected ? "Yes" : "No");

        for (uint32_t i = 0; i < g_ctx.device_count; i++) {
            device_t *dev = &g_ctx.devices[i];
            LOG_INFO("  Device %s: %s, errors: %u",
                     dev->name,
                     dev->running ? "Running" : "Stopped",
                     dev->error_count);
        }
    }

    LOG_INFO("Monitor thread stopped");
    return NULL;
}

/* ============ 配置加载 ============ */

static int load_config(const char *config_file)
{
    /* 简化实现：硬编码配置 */
    /* 实际项目中应解析JSON配置文件 */

    LOG_INFO("Loading configuration...");

    /* 系统配置 */
    strcpy(g_ctx.config.log_level, "INFO");
    g_ctx.config.local_storage_enabled = true;
    g_ctx.config.retention_days = 30;
    g_ctx.config.mqtt_enabled = true;
    strcpy(g_ctx.config.mqtt_broker, "localhost");
    g_ctx.config.mqtt_port = 1883;
    snprintf(g_ctx.config.mqtt_client_id, sizeof(g_ctx.config.mqtt_client_id),
             "collector_%d", getpid());
    strcpy(g_ctx.config.mqtt_topic_prefix, "industrial/data");
    g_ctx.config.mqtt_qos = 1;

    /* 创建设备1: 模拟温度/压力传感器 */
    device_t *dev1 = &g_ctx.devices[g_ctx.device_count++];
    dev1->id = 1;
    strcpy(dev1->name, "SensorUnit1");
    dev1->type = DEVICE_SIMULATION;
    dev1->enabled = true;
    dev1->scan_interval_ms = 1000;

    /* 添加标签 */
    strcpy(dev1->tags[dev1->tag_count].name, "Temperature");
    dev1->tags[dev1->tag_count].scale = 1.0;
    dev1->tags[dev1->tag_count].offset = 0.0;
    dev1->tags[dev1->tag_count].enabled = true;
    dev1->tag_count++;

    strcpy(dev1->tags[dev1->tag_count].name, "Pressure");
    dev1->tags[dev1->tag_count].scale = 1.0;
    dev1->tags[dev1->tag_count].offset = 0.0;
    dev1->tags[dev1->tag_count].enabled = true;
    dev1->tag_count++;

    strcpy(dev1->tags[dev1->tag_count].name, "FlowRate");
    dev1->tags[dev1->tag_count].scale = 1.0;
    dev1->tags[dev1->tag_count].offset = 0.0;
    dev1->tags[dev1->tag_count].enabled = true;
    dev1->tag_count++;

    /* 创建设备2: 模拟电机 */
    device_t *dev2 = &g_ctx.devices[g_ctx.device_count++];
    dev2->id = 2;
    strcpy(dev2->name, "MotorController");
    dev2->type = DEVICE_SIMULATION;
    dev2->enabled = true;
    dev2->scan_interval_ms = 500;

    strcpy(dev2->tags[dev2->tag_count].name, "MotorSpeed");
    dev2->tags[dev2->tag_count].scale = 1.0;
    dev2->tags[dev2->tag_count].offset = 0.0;
    dev2->tags[dev2->tag_count].enabled = true;
    dev2->tag_count++;

    strcpy(dev2->tags[dev2->tag_count].name, "MotorCurrent");
    dev2->tags[dev2->tag_count].scale = 0.1;
    dev2->tags[dev2->tag_count].offset = 0.0;
    dev2->tags[dev2->tag_count].enabled = true;
    dev2->tag_count++;

    LOG_INFO("Configuration loaded: %u devices", g_ctx.device_count);
    return 0;
}

/* ============ 主程序 ============ */

static void signal_handler(int sig)
{
    (void)sig;
    LOG_INFO("Received signal %d, shutting down...", sig);
    g_ctx.running = false;
}

static void print_usage(const char *program)
{
    printf("Industrial Data Collector v%s\n", COLLECTOR_VERSION);
    printf("Usage: %s [options]\n", program);
    printf("Options:\n");
    printf("  -c <file>   Configuration file (default: %s)\n", DEFAULT_CONFIG_FILE);
    printf("  -d          Run as daemon\n");
    printf("  -v          Show version\n");
    printf("  -h          Show this help\n");
}

int main(int argc, char *argv[])
{
    const char *config_file = DEFAULT_CONFIG_FILE;
    bool daemon_mode = false;

    /* 解析命令行 */
    int opt;
    while ((opt = getopt(argc, argv, "c:dvh")) != -1) {
        switch (opt) {
            case 'c':
                config_file = optarg;
                break;
            case 'd':
                daemon_mode = true;
                break;
            case 'v':
                printf("Industrial Data Collector v%s\n", COLLECTOR_VERSION);
                return 0;
            case 'h':
            default:
                print_usage(argv[0]);
                return (opt == 'h') ? 0 : 1;
        }
    }

    /* 初始化日志 */
    logger_init();

    LOG_INFO("============================================");
    LOG_INFO("Industrial Data Collector v%s Starting...", COLLECTOR_VERSION);
    LOG_INFO("============================================");

    /* 守护进程模式 */
    if (daemon_mode) {
        if (daemon(0, 0) < 0) {
            LOG_FATAL("Failed to daemonize: %s", strerror(errno));
            return 1;
        }
        LOG_INFO("Running as daemon");
    }

    /* 锁定内存 */
    if (mlockall(MCL_CURRENT | MCL_FUTURE) < 0) {
        LOG_WARNING("Failed to lock memory: %s", strerror(errno));
    }

    /* 注册信号处理 */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGPIPE, SIG_IGN);

    /* 加载配置 */
    if (load_config(config_file) != 0) {
        LOG_FATAL("Failed to load configuration");
        return 1;
    }

    /* 初始化队列 */
    if (queue_init(&g_ctx.data_queue, COLLECTOR_QUEUE_SIZE) != 0) {
        LOG_FATAL("Failed to initialize data queue");
        return 1;
    }

    /* 初始化数据库 */
    if (db_init() != 0) {
        LOG_FATAL("Failed to initialize database");
        return 1;
    }

    /* 初始化MQTT */
    if (mqtt_init() != 0) {
        LOG_WARNING("Failed to initialize MQTT, continuing without cloud upload");
    }

    /* 启动后台线程 */
    pthread_create(&g_ctx.storage_thread, NULL, storage_thread, NULL);
    pthread_create(&g_ctx.monitor_thread, NULL, monitor_thread, NULL);

    /* 启动设备采集线程 */
    g_ctx.running = true;
    g_ctx.start_time = get_time_ms();

    for (uint32_t i = 0; i < g_ctx.device_count; i++) {
        device_t *dev = &g_ctx.devices[i];
        if (dev->enabled) {
            dev->running = true;
            pthread_create(&dev->thread, NULL, device_simulation_thread, dev);
        }
    }

    LOG_INFO("Collector started successfully");

    /* 主循环 */
    while (g_ctx.running) {
        sleep(1);
    }

    LOG_INFO("Shutting down...");

    /* 停止设备线程 */
    for (uint32_t i = 0; i < g_ctx.device_count; i++) {
        device_t *dev = &g_ctx.devices[i];
        dev->running = false;
        pthread_join(dev->thread, NULL);
    }

    /* 等待队列处理完成 */
    while (queue_size(&g_ctx.data_queue) > 0) {
        usleep(100000);
    }

    /* 停止后台线程 */
    pthread_join(g_ctx.storage_thread, NULL);
    pthread_join(g_ctx.monitor_thread, NULL);

    /* 清理 */
    mqtt_close();
    db_close();
    queue_destroy(&g_ctx.data_queue);
    logger_close();

    printf("Collector stopped.\n");

    return 0;
}
```

---

## 11. 远程OTA升级

### 11.1 OTA升级架构

```
┌─────────────────────────────────────────────────────────────────┐
│                    OTA升级系统架构                              │
├─────────────────────────────────────────────────────────────────┤
│  ┌──────────────────────────────────────────────────────────┐  │
│  │                     云端OTA服务器                         │  │
│  │  - 固件版本管理                                          │  │
│  │  - 增量/全量包生成                                        │  │
│  │  - 设备分组管理                                          │  │
│  │  - 升级策略配置 (灰度/强制/定时)                          │  │
│  └──────────────────────────────────────────────────────────┘  │
│                              │                                  │
│                              ▼ HTTPS/MQTT                      │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │                    边缘设备OTA客户端                      │  │
│  ├──────────────────────────────────────────────────────────┤  │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  │  │
│  │  │ 版本检查 │→ │ 包下载   │→ │ 完整性   │→ │ 升级执行 │  │  │
│  │  │          │  │ 断点续传 │  │ 校验     │  │ AB分区   │  │  │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘  │  │
│  ├──────────────────────────────────────────────────────────┤  │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐               │  │
│  │  │ 升级回滚 │  │ 状态报告 │  │ 日志记录 │               │  │
│  │  │ (失败时) │  │          │  │          │               │  │
│  │  └──────────┘  └──────────┘  └──────────┘               │  │
│  └──────────────────────────────────────────────────────────┘  │
│                              │                                  │
│                              ▼                                  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │                    安全机制                               │  │
│  │  - 固件签名验证 (RSA/ECC)                                │  │
│  │  - 安全启动 (Secure Boot)                                │  │
│  │  - 加密传输 (TLS 1.3)                                    │  │
│  │  - 防回滚保护                                            │  │
│  └──────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

### 11.2 OTA升级客户端实现

```c
/**
 * @file ota_client.c
 * @brief OTA升级客户端实现
 * @details 支持HTTPS固件下载、签名验证、AB分区升级
 *
 * 编译: gcc -o ota_client ota_client.c -lcurl -lcrypto -ljson-c -lpthread
 * 运行: sudo ./ota_client
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <curl/curl.h>
#include <json-c/json.h>

/* OTA配置 */
#define OTA_VERSION             "1.0.0"
#define OTA_SERVER_URL          "https://ota.example.com/api/v1"
#define OTA_DEVICE_ID           "DEV001"
#define OTA_CHECK_INTERVAL_SEC  3600

/* 固件配置 */
#define FIRMWARE_PATH           "/tmp/firmware.bin"
#define FIRMWARE_PART_A         "/dev/mmcblk0p2"
#define FIRMWARE_PART_B         "/dev/mmcblk0p3"
#define BOOT_FLAG_PATH          "/boot/ota_flag"

/* 缓冲区大小 */
#define DOWNLOAD_BUFFER_SIZE    (64 * 1024)
#define SHA256_HASH_SIZE        32

/* OTA状态 */
typedef enum {
    OTA_STATE_IDLE = 0,
    OTA_STATE_CHECKING,
    OTA_STATE_DOWNLOADING,
    OTA_STATE_VERIFYING,
    OTA_STATE_UPGRADING,
    OTA_STATE_REBOOTING,
    OTA_STATE_ROLLBACK
} ota_state_t;

/* 升级结果 */
typedef enum {
    OTA_RESULT_SUCCESS = 0,
    OTA_RESULT_NO_UPDATE,
    OTA_RESULT_DOWNLOAD_FAILED,
    OTA_RESULT_VERIFY_FAILED,
    OTA_RESULT_WRITE_FAILED,
    OTA_RESULT_INSUFFICIENT_SPACE,
    OTA_RESULT_BATTERY_LOW,
    OTA_RESULT_NETWORK_ERROR
} ota_result_t;

/* 固件信息 */
typedef struct {
    char version[32];
    char url[256];
    char hash[65];  /* SHA256 hex string */
    uint64_t size;
    char release_notes[512];
    bool force_update;
    bool delta_update;
    char delta_base[32];  /* 增量升级的基础版本 */
} firmware_info_t;

/* 设备信息 */
typedef struct {
    char device_id[32];
    char current_version[32];
    char hardware_version[32];
    char active_partition;  /* 'A' or 'B' */
    uint32_t battery_level; /* 百分比 */
    uint64_t free_space;
} device_info_t;

/* OTA上下文 */
typedef struct {
    ota_state_t state;
    firmware_info_t firmware;
    device_info_t device;
    char public_key_path[128];
    int download_progress;
    void (*progress_callback)(int percent);
    void (*status_callback)(ota_state_t state, const char *msg);
} ota_context_t;

static ota_context_t g_ctx = {0};

/* ============ 日志 ============ */
#define LOG_INFO(fmt, ...)  printf("[OTA-INFO] " fmt "\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) fprintf(stderr, "[OTA-ERROR] " fmt "\n", ##__VA_ARGS__)

/* ============ 工具函数 ============ */

/**
 * @brief 执行shell命令并获取输出
 */
static int exec_command(const char *cmd, char *output, size_t output_size)
{
    FILE *fp = popen(cmd, "r");
    if (!fp) {
        return -1;
    }

    if (output && output_size > 0) {
        fgets(output, output_size, fp);
        /* 移除换行符 */
        size_t len = strlen(output);
        if (len > 0 && output[len - 1] == '\n') {
            output[len - 1] = '\0';
        }
    }

    int status = pclose(fp);
    return WEXITSTATUS(status);
}

/**
 * @brief 获取当前活动分区
 */
static char get_active_partition(void)
{
    char root_dev[64] = {0};

    /* 读取当前根分区 */
    FILE *fp = fopen("/proc/cmdline", "r");
    if (fp) {
        char cmdline[256];
        if (fgets(cmdline, sizeof(cmdline), fp)) {
            char *root = strstr(cmdline, "root=");
            if (root) {
                sscanf(root + 5, "%s", root_dev);
            }
        }
        fclose(fp);
    }

    /* 判断是A分区还是B分区 */
    if (strstr(root_dev, "mmcblk0p2") || strstr(root_dev, "p2")) {
        return 'A';
    }
    return 'B';
}

/**
 * @brief 获取当前版本
 */
static void get_current_version(char *version, size_t size)
{
    /* 从文件或命令读取 */
    FILE *fp = fopen("/etc/firmware_version", "r");
    if (fp) {
        fgets(version, size, fp);
        fclose(fp);
        /* 移除换行符 */
        size_t len = strlen(version);
        if (len > 0 && version[len - 1] == '\n') {
            version[len - 1] = '\0';
        }
    } else {
        strncpy(version, "1.0.0", size - 1);
    }
}

/* ============ 下载功能 ============ */

typedef struct {
    FILE *fp;
    size_t downloaded;
    size_t total;
} download_context_t;

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    download_context_t *ctx = (download_context_t *)userdata;
    size_t written = fwrite(ptr, size, nmemb, ctx->fp);
    ctx->downloaded += written * size;

    /* 更新进度 */
    if (ctx->total > 0 && g_ctx.progress_callback) {
        int progress = (int)((ctx->downloaded * 100) / ctx->total);
        if (progress != g_ctx.download_progress) {
            g_ctx.download_progress = progress;
            g_ctx.progress_callback(progress);
        }
    }

    return written;
}

static int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow,
                              curl_off_t ultotal, curl_off_t ulnow)
{
    (void)clientp;
    (void)ultotal;
    (void)ulnow;

    if (dltotal > 0) {
        download_context_t *ctx = (download_context_t *)clientp;
        ctx->total = dltotal;
    }

    return 0;
}

/**
 * @brief 下载固件
 */
static ota_result_t download_firmware(const char *url, const char *output_path)
{
    CURL *curl;
    CURLcode res;
    download_context_t dl_ctx = {0};

    LOG_INFO("Downloading firmware from: %s", url);

    dl_ctx.fp = fopen(output_path, "wb");
    if (!dl_ctx.fp) {
        LOG_ERROR("Failed to create file: %s", output_path);
        return OTA_RESULT_INSUFFICIENT_SPACE;
    }

    curl = curl_easy_init();
    if (!curl) {
        fclose(dl_ctx.fp);
        return OTA_RESULT_NETWORK_ERROR;
    }

    /* 设置下载选项 */
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &dl_ctx);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300L);  /* 5分钟超时 */
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &dl_ctx);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    /* 启用SSL验证 */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    g_ctx.state = OTA_STATE_DOWNLOADING;
    if (g_ctx.status_callback) {
        g_ctx.status_callback(g_ctx.state, "Downloading firmware...");
    }

    res = curl_easy_perform(curl);

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_easy_cleanup(curl);
    fclose(dl_ctx.fp);

    if (res != CURLE_OK) {
        LOG_ERROR("Download failed: %s", curl_easy_strerror(res));
        unlink(output_path);
        return OTA_RESULT_DOWNLOAD_FAILED;
    }

    if (http_code != 200) {
        LOG_ERROR("HTTP error: %ld", http_code);
        unlink(output_path);
        return OTA_RESULT_DOWNLOAD_FAILED;
    }

    LOG_INFO("Download complete: %zu bytes", dl_ctx.downloaded);
    return OTA_RESULT_SUCCESS;
}

/* ============ 签名验证 ============ */

/**
 * @brief 验证固件签名 (RSA-SHA256)
 */
static bool verify_firmware_signature(const char *firmware_path,
                                       const char *signature_path,
                                       const char *public_key_path)
{
    EVP_MD_CTX *ctx = NULL;
    EVP_PKEY *pkey = NULL;
    FILE *fp = NULL;
    bool result = false;

    LOG_INFO("Verifying firmware signature...");

    /* 读取公钥 */
    fp = fopen(public_key_path, "r");
    if (!fp) {
        LOG_ERROR("Failed to open public key: %s", public_key_path);
        return false;
    }

    pkey = PEM_read_PUBKEY(fp, NULL, NULL, NULL);
    fclose(fp);

    if (!pkey) {
        LOG_ERROR("Failed to read public key");
        return false;
    }

    /* 读取签名 */
    uint8_t signature[256];
    fp = fopen(signature_path, "rb");
    if (!fp) {
        LOG_ERROR("Failed to open signature file");
        EVP_PKEY_free(pkey);
        return false;
    }

    size_t sig_len = fread(signature, 1, sizeof(signature), fp);
    fclose(fp);

    /* 创建验证上下文 */
    ctx = EVP_MD_CTX_new();
    if (!ctx) {
        goto cleanup;
    }

    if (EVP_DigestVerifyInit(ctx, NULL, EVP_sha256(), NULL, pkey) != 1) {
        goto cleanup;
    }

    /* 读取固件并计算哈希 */
    fp = fopen(firmware_path, "rb");
    if (!fp) {
        goto cleanup;
    }

    uint8_t buffer[8192];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        if (EVP_DigestVerifyUpdate(ctx, buffer, bytes_read) != 1) {
            fclose(fp);
            goto cleanup;
        }
    }
    fclose(fp);

    /* 验证签名 */
    if (EVP_DigestVerifyFinal(ctx, signature, sig_len) == 1) {
        result = true;
        LOG_INFO("Signature verification PASSED");
    } else {
        LOG_ERROR("Signature verification FAILED");
    }

cleanup:
    if (ctx) EVP_MD_CTX_free(ctx);
    if (pkey) EVP_PKEY_free(pkey);

    return result;
}

/**
 * @brief 计算文件SHA256
 */
static bool calculate_file_sha256(const char *filepath, char *hash_str)
{
    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        return false;
    }

    SHA256_CTX ctx;
    SHA256_Init(&ctx);

    uint8_t buffer[8192];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        SHA256_Update(&ctx, buffer, bytes_read);
    }
    fclose(fp);

    uint8_t hash[SHA256_HASH_SIZE];
    SHA256_Final(hash, &ctx);

    /* 转换为hex字符串 */
    for (int i = 0; i < SHA256_HASH_SIZE; i++) {
        sprintf(hash_str + (i * 2), "%02x", hash[i]);
    }
    hash_str[64] = '\0';

    return true;
}

/* ============ 升级执行 ============ */

/**
 * @brief 写入固件到分区
 */
static bool write_firmware_to_partition(const char *firmware_path,
                                          const char *partition)
{
    LOG_INFO("Writing firmware to %s...", partition);

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "dd if=%s of=%s bs=1M status=progress conv=fsync",
             firmware_path, partition);

    int ret = system(cmd);
    if (ret != 0) {
        LOG_ERROR("Failed to write firmware");
        return false;
    }

    LOG_INFO("Firmware written successfully");
    return true;
}

/**
 * @brief 设置下次启动分区
 */
static bool set_boot_partition(char partition)
{
    FILE *fp = fopen(BOOT_FLAG_PATH, "w");
    if (!fp) {
        return false;
    }

    fprintf(fp, "BOOT_PART=%c\n", partition);
    fprintf(fp, "OTA_UPGRADE=1\n");
    fprintf(fp, "OTA_VERSION=%s\n", g_ctx.firmware.version);
    fprintf(fp, "OTA_TIMESTAMP=%lu\n", (unsigned long)time(NULL));
    fclose(fp);

    LOG_INFO("Boot partition set to: %c", partition);
    return true;
}

/**
 * @brief 执行系统重启
 */
static void system_reboot(void)
{
    LOG_INFO("System rebooting...");
    sync();
    sleep(2);
    reboot(LINUX_REBOOT_CMD_RESTART);
}

/**
 * @brief 报告升级状态到服务器
 */
static void report_upgrade_status(const char *status, const char *message)
{
    /* 构造JSON */
    struct json_object *root = json_object_new_object();
    json_object_object_add(root, "device_id",
                           json_object_new_string(g_ctx.device.device_id));
    json_object_object_add(root, "status", json_object_new_string(status));
    json_object_object_add(root, "message", json_object_new_string(message));
    json_object_object_add(root, "version",
                           json_object_new_string(g_ctx.firmware.version));
    json_object_object_add(root, "timestamp",
                           json_object_new_int64(time(NULL)));

    const char *json_str = json_object_to_json_string(root);
    LOG_INFO("Report status: %s", json_str);

    /* 实际项目中通过HTTP/MQTT发送 */

    json_object_put(root);
}

/* ============ 主OTA流程 ============ */

/**
 * @brief 检查更新
 */
static ota_result_t check_for_update(void)
{
    LOG_INFO("Checking for firmware update...");
    g_ctx.state = OTA_STATE_CHECKING;

    /* 获取设备信息 */
    strncpy(g_ctx.device.device_id, OTA_DEVICE_ID,
            sizeof(g_ctx.device.device_id) - 1);
    get_current_version(g_ctx.device.current_version,
                        sizeof(g_ctx.device.current_version));
    g_ctx.device.active_partition = get_active_partition();

    LOG_INFO("Current version: %s, Partition: %c",
             g_ctx.device.current_version, g_ctx.device.active_partition);

    /* 构造检查请求 */
    struct json_object *req = json_object_new_object();
    json_object_object_add(req, "device_id",
                           json_object_new_string(g_ctx.device.device_id));
    json_object_object_add(req, "current_version",
                           json_object_new_string(g_ctx.device.current_version));
    json_object_object_add(req, "hardware_version",
                           json_object_new_string("HW-1.0"));

    /* 实际项目中发送HTTP POST请求到OTA服务器 */
    /* 这里模拟服务器响应 */

    /* 模拟: 假设有新版本 */
    strncpy(g_ctx.firmware.version, "2.0.0", sizeof(g_ctx.firmware.version) - 1);
    strncpy(g_ctx.firmware.url,
            "https://ota.example.com/firmware/v2.0.0/firmware.bin",
            sizeof(g_ctx.firmware.url) - 1);
    strncpy(g_ctx.firmware.hash,
            "aabbccdd00112233445566778899aabbccdd00112233445566778899aabbccdd",
            sizeof(g_ctx.firmware.hash) - 1);
    g_ctx.firmware.size = 16 * 1024 * 1024;  /* 16MB */
    strncpy(g_ctx.firmware.release_notes, "Bug fixes and improvements",
            sizeof(g_ctx.firmware.release_notes) - 1);
    g_ctx.firmware.force_update = false;
    g_ctx.firmware.delta_update = false;

    json_object_put(req);

    /* 检查版本是否需要升级 */
    if (strcmp(g_ctx.firmware.version, g_ctx.device.current_version) == 0) {
        LOG_INFO("Already up to date");
        return OTA_RESULT_NO_UPDATE;
    }

    LOG_INFO("New firmware available: %s", g_ctx.firmware.version);
    return OTA_RESULT_SUCCESS;
}

/**
 * @brief 执行OTA升级
 */
static ota_result_t perform_ota_upgrade(void)
{
    ota_result_t result;
    char calculated_hash[65];

    /* 1. 下载固件 */
    result = download_firmware(g_ctx.firmware.url, FIRMWARE_PATH);
    if (result != OTA_RESULT_SUCCESS) {
        return result;
    }

    /* 2. 验证文件哈希 */
    g_ctx.state = OTA_STATE_VERIFYING;
    if (g_ctx.status_callback) {
        g_ctx.status_callback(g_ctx.state, "Verifying firmware integrity...");
    }

    if (!calculate_file_sha256(FIRMWARE_PATH, calculated_hash)) {
        LOG_ERROR("Failed to calculate file hash");
        return OTA_RESULT_VERIFY_FAILED;
    }

    if (strcasecmp(calculated_hash, g_ctx.firmware.hash) != 0) {
        LOG_ERROR("Hash mismatch! Expected: %s, Got: %s",
                  g_ctx.firmware.hash, calculated_hash);
        return OTA_RESULT_VERIFY_FAILED;
    }
    LOG_INFO("Hash verification passed");

    /* 3. 验证数字签名 */
    char sig_path[256];
    snprintf(sig_path, sizeof(sig_path), "%s.sig", FIRMWARE_PATH);

    if (access(g_ctx.public_key_path, F_OK) == 0) {
        if (!verify_firmware_signature(FIRMWARE_PATH, sig_path,
                                       g_ctx.public_key_path)) {
            return OTA_RESULT_VERIFY_FAILED;
        }
    }

    /* 4. 写入目标分区 */
    g_ctx.state = OTA_STATE_UPGRADING;
    if (g_ctx.status_callback) {
        g_ctx.status_callback(g_ctx.state, "Writing firmware...");
    }

    char target_partition[32];
    if (g_ctx.device.active_partition == 'A') {
        strncpy(target_partition, FIRMWARE_PART_B, sizeof(target_partition) - 1);
    } else {
        strncpy(target_partition, FIRMWARE_PART_A, sizeof(target_partition) - 1);
    }

    if (!write_firmware_to_partition(FIRMWARE_PATH, target_partition)) {
        return OTA_RESULT_WRITE_FAILED;
    }

    /* 5. 设置启动标志 */
    char next_partition = (g_ctx.device.active_partition == 'A') ? 'B' : 'A';
    if (!set_boot_partition(next_partition)) {
        LOG_ERROR("Failed to set boot partition");
        return OTA_RESULT_WRITE_FAILED;
    }

    /* 6. 报告状态并重启 */
    report_upgrade_status("SUCCESS", "Firmware installed, rebooting");

    g_ctx.state = OTA_STATE_REBOOTING;
    if (g_ctx.status_callback) {
        g_ctx.status_callback(g_ctx.state, "Rebooting system...");
    }

    /* 清理临时文件 */
    unlink(FIRMWARE_PATH);

    /* 重启 */
    system_reboot();

    return OTA_RESULT_SUCCESS;
}

/* ============ 回调函数 ============ */

static void on_download_progress(int percent)
{
    static int last_percent = -1;
    if (percent != last_percent) {
        printf("\rDownload progress: %d%%", percent);
        fflush(stdout);
        last_percent = percent;
        if (percent >= 100) {
            printf("\n");
        }
    }
}

static void on_status_change(ota_state_t state, const char *msg)
{
    const char *state_str[] = {
        "IDLE", "CHECKING", "DOWNLOADING", "VERIFYING",
        "UPGRADING", "REBOOTING", "ROLLBACK"
    };
    LOG_INFO("[State: %s] %s", state_str[state], msg);
}

/* ============ 主函数 ============ */

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║       Industrial OTA Client v%-26s   ║\n", OTA_VERSION);
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* 初始化 */
    curl_global_init(CURL_GLOBAL_DEFAULT);

    g_ctx.progress_callback = on_download_progress;
    g_ctx.status_callback = on_status_change;
    strncpy(g_ctx.public_key_path, "/etc/ota/public_key.pem",
            sizeof(g_ctx.public_key_path) - 1);

    /* 检查更新 */
    ota_result_t result = check_for_update();
    if (result == OTA_RESULT_NO_UPDATE) {
        LOG_INFO("No update available");
        curl_global_cleanup();
        return 0;
    } else if (result != OTA_RESULT_SUCCESS) {
        LOG_ERROR("Update check failed");
        curl_global_cleanup();
        return 1;
    }

    /* 执行升级 */
    LOG_INFO("Starting OTA upgrade to version %s", g_ctx.firmware.version);
    LOG_INFO("Release notes: %s", g_ctx.firmware.release_notes);

    result = perform_ota_upgrade();
    if (result != OTA_RESULT_SUCCESS) {
        LOG_ERROR("OTA upgrade failed: %d", result);
        report_upgrade_status("FAILED", "Upgrade failed");
        curl_global_cleanup();
        return 1;
    }

    curl_global_cleanup();
    return 0;
}
```

---

## 附录

### A. 编译依赖安装

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    libpthread-stubs0-dev \
    libsqlite3-dev \
    libmosquitto-dev \
    libcurl4-openssl-dev \
    libjson-c-dev \
    libssl-dev \
    libopen62541-dev \
    libethercat-dev \

# PREEMPT_RT内核
sudo apt-get install linux-image-rt

# Xenomai (如需要)
sudo apt-get install libxenomai-dev
```

### B. 参考资料

1. **IgH EtherCAT Master** - <https://etherlab.org/en/ethercat/>
2. **open62541 OPC UA** - <https://open62541.org/>
3. **PREEMPT_RT Patch** - <https://wiki.linuxfoundation.org/realtime/start>
4. **Xenomai** - <https://xenomai.org/>
5. **Modbus协议规范** - <https://modbus.org/specs.php>
6. **CANopen CiA标准** - <https://www.can-cia.org/canopen/>

### C. 术语表

| 术语 | 说明 |
|------|------|
| RTOS | Real-Time Operating System，实时操作系统 |
| SCADA | Supervisory Control And Data Acquisition，数据采集与监控系统 |
| MES | Manufacturing Execution System，制造执行系统 |
| HMI | Human-Machine Interface，人机界面 |
| PLC | Programmable Logic Controller，可编程逻辑控制器 |
| OPC UA | OLE for Process Control Unified Architecture |
| DDS | Data Distribution Service，数据分发服务 |
| TSN | Time-Sensitive Networking，时间敏感网络 |

---

*文档版本: 1.0*
*最后更新: 2026-03-17*
