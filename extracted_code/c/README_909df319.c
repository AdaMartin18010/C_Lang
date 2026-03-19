/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\README.md
 * Line: 465
 * Language: c
 * Block ID: 909df319
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* nr_realtime.h - 实时处理框架 */
#ifndef NR_REALTIME_H
#define NR_REALTIME_H

#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* 实时优先级 */
typedef enum {
    RT_PRIORITY_LOW = 1,
    RT_PRIORITY_NORMAL = 50,
    RT_PRIORITY_HIGH = 90,
    RT_PRIORITY_CRITICAL = 99
} rt_priority_t;

/* 任务类型 */
typedef enum {
    TASK_TTI_PROCESSING = 0,
    TASK_SLOT_PROCESSING,
    TASK_SYMBOL_PROCESSING,
    TASK_CHANNEL_ESTIMATION,
    TASK_ENCODING,
    TASK_DECODING,
    TASK_COUNT
} task_type_t;

/* 任务定义 */
typedef struct {
    task_type_t type;
    rt_priority_t priority;
    uint32_t deadline_us;
    void (*handler)(void *arg);
    void *arg;
    uint32_t execution_count;
    uint32_t miss_count;
    double avg_execution_time;
} rt_task_t;

/* 调度器 */
typedef struct {
    pthread_t thread;
    rt_task_t *tasks;
    uint32_t num_tasks;
    bool running;
    uint32_t tti_interval_us;
} rt_scheduler_t;

/* API */
int rt_scheduler_init(rt_scheduler_t *sched, uint32_t tti_interval_us);
int rt_task_register(rt_scheduler_t *sched, const rt_task_t *task);
int rt_scheduler_start(rt_scheduler_t *sched);
void rt_scheduler_stop(rt_scheduler_t *sched);

/* 时间工具 */
uint64_t rt_get_time_us(void);
void rt_sleep_until(uint64_t target_time_us);

#endif /* NR_REALTIME_H */
