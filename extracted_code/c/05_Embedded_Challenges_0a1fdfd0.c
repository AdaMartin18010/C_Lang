/*
 * Auto-generated from: 12_Practice_Exercises\05_Embedded_Challenges.md
 * Line: 401
 * Language: c
 * Block ID: 0a1fdfd0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define MAX_TASKS 8
#define STACK_SIZE 1024

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_SUSPENDED
} TaskState;

typedef struct {
    uint32_t *stack_ptr;
    void (*task_func)(void);
    TaskState state;
    uint32_t stack[STACK_SIZE];
    uint32_t delay_until;
    uint8_t priority;
} Task;

typedef struct {
    Task tasks[MAX_TASKS];
    uint8_t task_count;
    uint8_t current_task;
    volatile uint32_t tick_count;
} Scheduler;

Scheduler scheduler = {0};
