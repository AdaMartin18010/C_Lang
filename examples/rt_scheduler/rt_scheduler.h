/*
 * 实时任务调度器 - RMS / EDF 模拟
 * 头文件
 */
#ifndef RT_SCHEDULER_H
#define RT_SCHEDULER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_TASKS 16
#define MAX_TIME  1000

typedef struct {
    int id;
    int period;       /* T: 周期 */
    int deadline;     /* D: 相对截止期（通常 = period） */
    int wcet;         /* C: 最坏执行时间 */
    int priority;     /* 调度器分配 */
    int next_release; /* 下次释放时间 */
    int deadline_abs; /* 绝对截止期 */
    int remaining;    /* 剩余执行时间 */
    int executed;     /* 本次已执行时间 */
} Task;

typedef enum {
    SCHED_RMS,   /* Rate Monotonic: 周期越短优先级越高 */
    SCHED_EDF,   /* Earliest Deadline First */
} SchedPolicy;

/* 任务集管理 */
void task_init(Task *t, int id, int period, int wcet);
int task_set_add(Task *set, size_t *count, int period, int wcet);

/* 可调度性分析 */
bool rms_schedulability_test(const Task *set, size_t count);
bool edf_schedulability_test(const Task *set, size_t count);

/* 调度模拟 */
typedef struct {
    int time;
    int task_id;   /* -1 = idle */
    int deadline_miss;
} ScheduleEvent;

int schedule_simulate(Task *set, size_t count, SchedPolicy policy,
                      ScheduleEvent *timeline, int max_events);

/* 输出 */
void print_gantt(const ScheduleEvent *timeline, int events, const Task *set, size_t count);
void print_task_set(const Task *set, size_t count);

#endif /* RT_SCHEDULER_H */
