/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\README.md
 * Line: 873
 * Language: c
 * Block ID: 24b8daf4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* os/events.c */
#include "os/osek_types.h"

/* 等待事件 - 仅扩展任务 */
void wait_event(uint32_t mask) {
    CRITICAL_SECTION_ENTER();

    if (g_current_task == NULL || g_current_task->type != EXTENDED) {
        CRITICAL_SECTION_EXIT();
        return;
    }

    g_current_task->wait_events = mask;

    /* 检查事件是否已满足 */
    if ((g_current_task->current_events & mask) == 0) {
        g_current_task->state = WAITING;

        task_control_block_t* waiting_task = g_current_task;
        g_current_task = NULL;

        schedule();

        /* 恢复后清除等待掩码 */
        waiting_task->wait_events = 0;
    }

    CRITICAL_SECTION_EXIT();
}

/* 设置事件 */
void set_event(uint8_t task_id, uint32_t mask) {
    CRITICAL_SECTION_ENTER();

    task_control_block_t* task = &g_task_table[task_id];

    task->current_events |= mask;

    /* 如果任务在等待这些事件，将其唤醒 */
    if (task->state == WAITING && (task->wait_events & mask)) {
        task->state = READY;
        ready_queue_insert(task);

        /* 抢占检查 */
        if (g_current_task && task->priority < g_current_task->priority) {
            schedule();
        }
    }

    CRITICAL_SECTION_EXIT();
}

/* 清除事件 */
void clear_event(uint32_t mask) {
    CRITICAL_SECTION_ENTER();

    if (g_current_task) {
        g_current_task->current_events &= ~mask;
    }

    CRITICAL_SECTION_EXIT();
}
