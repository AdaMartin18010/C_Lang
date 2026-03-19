/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\README.md
 * Line: 724
 * Language: c
 * Block ID: b7c2517f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* os/scheduler.c */
#include "os/osek_types.h"
#include "platform/context_switch.h"

#define MAX_TASKS       16
#define MAX_ALARMS      8

static task_control_block_t* g_ready_queue[MAX_TASKS];
static uint8_t g_ready_count = 0;
static task_control_block_t* g_current_task = NULL;
static task_control_block_t g_task_table[MAX_TASKS];
static alarm_t g_alarms[MAX_ALARMS];
static volatile uint32_t g_system_tick = 0;

/* 就绪队列管理 */
static void ready_queue_insert(task_control_block_t* task) {
    /* 按优先级插入 (优先级数值小的在前) */
    uint8_t i = 0;
    while (i < g_ready_count && g_ready_queue[i]->priority <= task->priority) {
        i++;
    }

    /* 移动元素腾出位置 */
    for (uint8_t j = g_ready_count; j > i; j--) {
        g_ready_queue[j] = g_ready_queue[j - 1];
    }

    g_ready_queue[i] = task;
    g_ready_count++;
    task->state = READY;
}

static task_control_block_t* ready_queue_highest(void) {
    if (g_ready_count == 0) return NULL;
    return g_ready_queue[0];
}

static void ready_queue_remove(task_control_block_t* task) {
    for (uint8_t i = 0; i < g_ready_count; i++) {
        if (g_ready_queue[i] == task) {
            for (uint8_t j = i; j < g_ready_count - 1; j++) {
                g_ready_queue[j] = g_ready_queue[j + 1];
            }
            g_ready_count--;
            break;
        }
    }
}

/* 任务激活 */
void activate_task(uint8_t task_id) {
    CRITICAL_SECTION_ENTER();

    task_control_block_t* task = &g_task_table[task_id];

    if (task->state == SUSPENDED) {
        /* 初始化任务栈 */
        task->stack_ptr = task->stack_base + task->stack_size;
        task->current_priority = task->priority;

        ready_queue_insert(task);

        /* 抢占检查 */
        if (g_current_task == NULL ||
            task->priority < g_current_task->priority) {
            schedule();
        }
    }

    CRITICAL_SECTION_EXIT();
}

/* 任务终止 */
void terminate_task(void) {
    CRITICAL_SECTION_ENTER();

    if (g_current_task) {
        g_current_task->state = SUSPENDED;
        g_current_task = NULL;
    }

    schedule();

    CRITICAL_SECTION_EXIT();
}

/* 调度器 */
void schedule(void) {
    task_control_block_t* next = ready_queue_highest();

    if (next == NULL) {
        /* 无就绪任务 - 进入空闲 */
        return;
    }

    if (next == g_current_task) {
        return;  /* 继续执行当前任务 */
    }

    task_control_block_t* prev = g_current_task;

    if (prev && prev->state == RUNNING) {
        prev->state = READY;
        ready_queue_insert(prev);
    }

    ready_queue_remove(next);
    next->state = RUNNING;
    g_current_task = next;

    /* 上下文切换 */
    context_switch(&prev->stack_ptr, next->stack_ptr);
}

/* 系统节拍处理 (ISR) */
void sys_tick_handler(void) {
    g_system_tick++;

    /* 更新报警 */
    for (uint8_t i = 0; i < MAX_ALARMS; i++) {
        alarm_t* alarm = &g_alarms[i];
        if (alarm->period == 0) continue;

        alarm->countdown--;
        if (alarm->countdown == 0) {
            alarm->countdown = alarm->period;

            if (alarm->task) {
                activate_task(alarm->task->task_id);
            }
            if (alarm->callback) {
                alarm->callback();
            }
        }
    }

    /* 抢占调度 */
    if (g_current_task && g_ready_count > 0) {
        task_control_block_t* highest = g_ready_queue[0];
        if (highest->priority < g_current_task->priority) {
            schedule();
        }
    }
}
