/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\README.md
 * Line: 659
 * Language: c
 * Block ID: bfafca71
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* os/osek_types.h */
#ifndef OSEK_TYPES_H
#define OSEK_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* 任务类型 */
typedef enum {
    BASIC,      /* 基础任务 - 不可等待 */
    EXTENDED    /* 扩展任务 - 可等待事件 */
} task_type_t;

/* 任务调度策略 */
typedef enum {
    FULL_PREEMPTIVE,     /* 完全抢占 */
    NON_PREEMPTIVE,      /* 非抢占 */
    MIXED_PREEMPTIVE     /* 混合 */
} schedule_policy_t;

/* 任务状态 */
typedef enum {
    RUNNING,     /* 正在执行 */
    READY,       /* 就绪等待 */
    WAITING,     /* 等待事件 (仅扩展任务) */
    SUSPENDED    /* 挂起 */
} task_state_t;

/* 优先级定义 - 数值越小优先级越高 */
#define TASK_PRIORITY_IDLE      31
#define TASK_PRIORITY_LOW       20
#define TASK_PRIORITY_NORMAL    10
#define TASK_PRIORITY_HIGH       5
#define TASK_PRIORITY_ISR        0

/* 任务控制块 */
typedef struct {
    uint8_t task_id;
    task_type_t type;
    task_state_t state;
    uint8_t priority;
    uint8_t current_priority;  /* 考虑优先级天花板 */
    void (*entry_point)(void);
    uint32_t* stack_base;
    uint32_t stack_size;
    uint32_t* stack_ptr;
    uint32_t wait_events;      /* 等待的事件掩码 */
    uint32_t current_events;   /* 当前事件状态 */
} task_control_block_t;

/* 报警类型 */
typedef struct {
    uint8_t alarm_id;
    uint32_t period;           /* 周期 (ticks) */
    uint32_t countdown;
    task_control_block_t* task;
    void (*callback)(void);
} alarm_t;

#endif
