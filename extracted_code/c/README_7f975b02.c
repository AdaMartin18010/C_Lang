/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\README.md
 * Line: 528
 * Language: c
 * Block ID: 7f975b02
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* nr_realtime.c - 实时调度实现 */
#include "nr_realtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>

uint64_t rt_get_time_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

void rt_set_thread_priority(pthread_t thread, int priority) {
    struct sched_param param;
    param.sched_priority = priority;
    pthread_setschedparam(thread, SCHED_FIFO, &param);
}

/* 调度器主循环 */
static void *scheduler_thread(void *arg) {
    rt_scheduler_t *sched = (rt_scheduler_t *)arg;

    /* 设置实时优先级 */
    rt_set_thread_priority(pthread_self(), RT_PRIORITY_CRITICAL);

    /* 绑定到特定CPU核心 */
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);  /* 绑定到CPU 0 */
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);

    uint64_t next_tti = rt_get_time_us();

    while (sched->running) {
        uint64_t tti_start = rt_get_time_us();

        /* 按优先级执行所有任务 */
        for (uint32_t i = 0; i < sched->num_tasks; i++) {
            rt_task_t *task = &sched->tasks[i];

            uint64_t task_start = rt_get_time_us();
            task->handler(task->arg);
            uint64_t task_end = rt_get_time_us();

            uint32_t execution_us = (uint32_t)(task_end - task_start);
            task->avg_execution_time =
                (task->avg_execution_time * task->execution_count + execution_us) /
                (task->execution_count + 1);
            task->execution_count++;

            /* 检查截止时间 */
            if (execution_us > task->deadline_us) {
                task->miss_count++;
                printf("WARNING: Task %d missed deadline (%u > %u us)\n",
                       task->type, execution_us, task->deadline_us);
            }
        }

        /* 等待下一个TTI */
        next_tti += sched->tti_interval_us;
        rt_sleep_until(next_tti);
    }

    return NULL;
}

int rt_scheduler_init(rt_scheduler_t *sched, uint32_t tti_interval_us) {
    memset(sched, 0, sizeof(rt_scheduler_t));
    sched->tti_interval_us = tti_interval_us;
    sched->tasks = calloc(TASK_COUNT, sizeof(rt_task_t));
    sched->num_tasks = 0;
    return 0;
}

int rt_task_register(rt_scheduler_t *sched, const rt_task_t *task) {
    if (sched->num_tasks >= TASK_COUNT) return -1;

    /* 按优先级插入 */
    uint32_t insert_pos = 0;
    for (; insert_pos < sched->num_tasks; insert_pos++) {
        if (task->priority > sched->tasks[insert_pos].priority) {
            break;
        }
    }

    /* 移动现有任务 */
    for (uint32_t i = sched->num_tasks; i > insert_pos; i--) {
        sched->tasks[i] = sched->tasks[i - 1];
    }

    sched->tasks[insert_pos] = *task;
    sched->num_tasks++;

    return 0;
}

int rt_scheduler_start(rt_scheduler_t *sched) {
    sched->running = true;
    return pthread_create(&sched->thread, NULL, scheduler_thread, sched);
}

void rt_scheduler_stop(rt_scheduler_t *sched) {
    sched->running = false;
    pthread_join(sched->thread, NULL);

    /* 打印统计 */
    printf("\n=== Real-time Statistics ===\n");
    for (uint32_t i = 0; i < sched->num_tasks; i++) {
        rt_task_t *task = &sched->tasks[i];
        printf("Task %d: avg=%.1f us, miss=%u/%u (%.2f%%)\n",
               task->type,
               task->avg_execution_time,
               task->miss_count,
               task->execution_count,
               100.0 * task->miss_count / task->execution_count);
    }
}
