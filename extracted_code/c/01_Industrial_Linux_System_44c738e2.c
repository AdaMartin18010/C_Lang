/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Industrial_Linux\01_Industrial_Linux_System.md
 * Line: 492
 * Language: c
 * Block ID: 44c738e2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
