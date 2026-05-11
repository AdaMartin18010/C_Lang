/*
 * 实时任务调度器实现
 * RMS / EDF 模拟 + 可调度性分析
 * 编译: gcc -Wall -Wextra -O2 -std=c11 -c rt_scheduler.c
 */
#include "rt_scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void task_init(Task *t, int id, int period, int wcet) {
    t->id = id;
    t->period = period;
    t->deadline = period;
    t->wcet = wcet;
    t->priority = 0;
    t->next_release = 0;
    t->deadline_abs = period;
    t->remaining = 0;
    t->executed = 0;
}

int task_set_add(Task *set, size_t *count, int period, int wcet) {
    if (*count >= MAX_TASKS) return -1;
    task_init(&set[*count], (int)(*count) + 1, period, wcet);
    (*count)++;
    return 0;
}

/* ================================================================
 * 可调度性分析
 * ================================================================ */

/* RMS 充分条件：利用率 <= n*(2^(1/n)-1) */
bool rms_schedulability_test(const Task *set, size_t count) {
    double u = 0.0;
    for (size_t i = 0; i < count; i++) {
        u += (double)set[i].wcet / set[i].period;
    }
    double bound = count * (pow(2.0, 1.0 / count) - 1.0);
    printf("RMS Utilization: %.4f, Bound: %.4f\n", u, bound);
    return u <= bound;
}

/* EDF 充分条件：利用率 <= 1 */
bool edf_schedulability_test(const Task *set, size_t count) {
    double u = 0.0;
    for (size_t i = 0; i < count; i++) {
        u += (double)set[i].wcet / set[i].period;
    }
    printf("EDF Utilization: %.4f, Bound: 1.0000\n", u);
    return u <= 1.0;
}

/* ================================================================
 * RMS 优先级分配（周期越短优先级越高）
 * ================================================================ */
static void assign_rms_priorities(Task *set, size_t count) {
    /* 简单冒泡排序分配优先级 */
    for (size_t i = 0; i < count; i++) {
        set[i].priority = (int)i;
    }
    /* 按周期排序 */
    for (size_t i = 0; i < count; i++) {
        for (size_t j = i + 1; j < count; j++) {
            if (set[j].period < set[i].period) {
                Task tmp = set[i]; set[i] = set[j]; set[j] = tmp;
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        set[i].priority = (int)(count - i); /* 周期短 = 高优先级 */
    }
}

/* ================================================================
 * 调度模拟
 * ================================================================ */
int schedule_simulate(Task *set, size_t count, SchedPolicy policy,
                      ScheduleEvent *timeline, int max_events) {
    /* 重置任务状态 */
    for (size_t i = 0; i < count; i++) {
        set[i].next_release = 0;
        set[i].deadline_abs = set[i].period;
        set[i].remaining = 0;
        set[i].executed = 0;
    }

    if (policy == SCHED_RMS) {
        assign_rms_priorities(set, count);
    }

    int hyperperiod = 1;
    for (size_t i = 0; i < count; i++) {
        int g = hyperperiod;
        int b = set[i].period;
        while (b != 0) { int t = b; b = g % b; g = t; }
        hyperperiod = (hyperperiod * set[i].period) / g;
    }
    if (hyperperiod > MAX_TIME) hyperperiod = MAX_TIME;

    int events = 0;
    int deadline_misses = 0;

    for (int t = 0; t < hyperperiod && events < max_events; t++) {
        /* 释放新任务实例 */
        for (size_t i = 0; i < count; i++) {
            if (t == set[i].next_release) {
                set[i].remaining = set[i].wcet;
                set[i].executed = 0;
                set[i].deadline_abs = t + set[i].deadline;
                set[i].next_release += set[i].period;
            }
        }

        /* 选择运行任务 */
        int chosen = -1;
        for (size_t i = 0; i < count; i++) {
            if (set[i].remaining > 0) {
                if (chosen < 0) {
                    chosen = (int)i;
                } else if (policy == SCHED_RMS) {
                    if (set[i].priority > set[chosen].priority) chosen = (int)i;
                } else { /* EDF */
                    if (set[i].deadline_abs < set[chosen].deadline_abs) chosen = (int)i;
                }
            }
        }

        /* 执行一个时间单位 */
        if (chosen >= 0) {
            set[chosen].remaining--;
            set[chosen].executed++;
        }

        /* 检查截止期错过 */
        for (size_t i = 0; i < count; i++) {
            if (t + 1 == set[i].deadline_abs && set[i].remaining > 0) {
                deadline_misses++;
                timeline[events].deadline_miss = (int)i + 1;
            } else {
                timeline[events].deadline_miss = 0;
            }
        }

        timeline[events].time = t;
        timeline[events].task_id = chosen >= 0 ? set[chosen].id : -1;
        events++;
    }

    return deadline_misses;
}

/* ================================================================
 * 输出
 * ================================================================ */
void print_task_set(const Task *set, size_t count) {
    printf("Task Set (%zu tasks):\n", count);
    printf("%-4s %-8s %-8s %-8s %-10s\n", "ID", "Period", "WCET", "Util", "Priority");
    printf("---- -------- -------- -------- ----------\n");
    for (size_t i = 0; i < count; i++) {
        printf("%-4d %-8d %-8d %-8.4f %-10d\n",
               set[i].id, set[i].period, set[i].wcet,
               (double)set[i].wcet / set[i].period, set[i].priority);
    }
    printf("\n");
}

void print_gantt(const ScheduleEvent *timeline, int events, const Task *set, size_t count) {
    (void)set; (void)count;
    printf("Gantt Chart (time -> task):\n");
    printf("Time: ");
    for (int i = 0; i < events && i < 60; i++) {
        printf("%2d ", timeline[i].time);
    }
    printf("\nTask: ");
    for (int i = 0; i < events && i < 60; i++) {
        if (timeline[i].task_id < 0) {
            printf(" - ");
        } else {
            printf("%2d ", timeline[i].task_id);
        }
    }
    printf("\nMiss: ");
    for (int i = 0; i < events && i < 60; i++) {
        if (timeline[i].deadline_miss > 0) {
            printf(" ! ");
        } else {
            printf("   ");
        }
    }
    printf("\n");
}
