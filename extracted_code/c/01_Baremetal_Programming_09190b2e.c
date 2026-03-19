/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Baremetal\01_Baremetal_Programming.md
 * Line: 3037
 * Language: c
 * Block ID: 09190b2e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file scheduler.c
 * @brief 无OS任务调度器
 * @details 提供简单的合作式和抢占式(基于中断)任务调度
 */

#include <stdint.h>

/* ============================================================================
 * 任务控制块定义
 * ============================================================================ */

#define MAX_TASKS           8       /* 最大任务数 */
#define TASK_NAME_LEN       16      /* 任务名最大长度 */

/* 任务状态 */
typedef enum {
    TASK_STATE_READY,       /* 就绪 */
    TASK_STATE_RUNNING,     /* 运行中 */
    TASK_STATE_BLOCKED,     /* 阻塞(等待延时) */
    TASK_STATE_SUSPENDED    /* 挂起 */
} TaskState_TypeDef;

/* 任务控制块 */
typedef struct {
    void (*pTask)(void);        /* 任务函数指针 */
    uint32_t delay;             /* 延时计数 */
    uint32_t period;            /* 周期(用于周期性任务) */
    TaskState_TypeDef state;    /* 任务状态 */
    char name[TASK_NAME_LEN];   /* 任务名 */
    uint32_t runCount;          /* 运行次数统计 */
} TaskControlBlock_TypeDef;

/* ============================================================================
 * 调度器全局变量
 * ============================================================================ */

static TaskControlBlock_TypeDef g_taskList[MAX_TASKS];
static uint8_t g_taskCount = 0;
static uint8_t g_currentTask = 0;
static volatile uint32_t g_tickCount = 0;
static uint8_t g_schedulerRunning = 0;

/* ============================================================================
 * 系统滴答定时器 (使用SysTick)
 * ============================================================================ */

/**
 * @brief 初始化系统滴答定时器
 * @param sysClkHz 系统时钟频率(Hz)
 * @param tickHz 滴答频率(Hz)
 */
void Scheduler_Init(uint32_t sysClkHz, uint32_t tickHz)
{
    /* 计算重装载值 */
    uint32_t reload = sysClkHz / tickHz - 1;

    /* SysTick配置 */
    (*(volatile uint32_t *)0xE000E014) = reload;    /* LOAD */
    (*(volatile uint32_t *)0xE000E018) = 0;         /* VAL = 0 */
    (*(volatile uint32_t *)0xE000E010) = 0x07;      /* CTRL = 使能+中断+处理器时钟 */

    /* 初始化任务列表 */
    for (uint8_t i = 0; i < MAX_TASKS; i++) {
        g_taskList[i].pTask = 0;
        g_taskList[i].delay = 0;
        g_taskList[i].period = 0;
        g_taskList[i].state = TASK_STATE_SUSPENDED;
        g_taskList[i].name[0] = '\0';
        g_taskList[i].runCount = 0;
    }

    g_taskCount = 0;
    g_currentTask = 0;
    g_tickCount = 0;
    g_schedulerRunning = 0;
}

/**
 * @brief SysTick中断处理函数
 * @note 需要在启动文件中定义SysTick_Handler
 */
void SysTick_Handler_User(void)
{
    g_tickCount++;

    /* 更新所有任务的延时计数 */
    for (uint8_t i = 0; i < MAX_TASKS; i++) {
        if (g_taskList[i].state == TASK_STATE_BLOCKED && g_taskList[i].delay > 0) {
            g_taskList[i].delay--;
            if (g_taskList[i].delay == 0) {
                g_taskList[i].state = TASK_STATE_READY;
            }
        }
    }
}

/**
 * @brief 获取系统滴答计数
 * @return 当前滴答数
 */
uint32_t Scheduler_GetTick(void)
{
    return g_tickCount;
}

/**
 * @brief 毫秒延时
 * @param ms 延时毫秒数
 */
void Scheduler_Delay(uint32_t ms)
{
    uint32_t start = g_tickCount;
    while ((g_tickCount - start) < ms) {
        __asm volatile ("wfi");  /* 等待中断，降低功耗 */
    }
}

/* ============================================================================
 * 任务管理API
 * ============================================================================ */

/**
 * @brief 添加任务到调度器
 * @param pTask 任务函数指针
 * @param name 任务名
 * @param delay 初始延时(滴答数)
 * @param period 周期(0=单次任务)
 * @return 任务ID (0-MAX_TASKS-1)，-1=失败
 */
int8_t Scheduler_AddTask(void (*pTask)(void), const char *name,
                         uint32_t delay, uint32_t period)
{
    if (g_taskCount >= MAX_TASKS || pTask == 0) {
        return -1;
    }

    /* 查找空闲槽位 */
    uint8_t index = g_taskCount;

    g_taskList[index].pTask = pTask;
    g_taskList[index].delay = delay;
    g_taskList[index].period = period;
    g_taskList[index].state = (delay == 0) ? TASK_STATE_READY : TASK_STATE_BLOCKED;

    /* 复制任务名 */
    uint8_t i = 0;
    while (name[i] && i < TASK_NAME_LEN - 1) {
        g_taskList[index].name[i] = name[i];
        i++;
    }
    g_taskList[index].name[i] = '\0';

    g_taskList[index].runCount = 0;
    g_taskCount++;

    return index;
}

/**
 * @brief 删除任务
 * @param taskId 任务ID
 * @return 1=成功, 0=失败
 */
uint8_t Scheduler_DeleteTask(uint8_t taskId)
{
    if (taskId >= g_taskCount) {
        return 0;
    }

    /* 标记为挂起 */
    g_taskList[taskId].state = TASK_STATE_SUSPENDED;
    g_taskList[taskId].pTask = 0;

    return 1;
}

/**
 * @brief 挂起任务
 * @param taskId 任务ID
 */
void Scheduler_SuspendTask(uint8_t taskId)
{
    if (taskId < g_taskCount) {
        g_taskList[taskId].state = TASK_STATE_SUSPENDED;
    }
}

/**
 * @brief 恢复任务
 * @param taskId 任务ID
 */
void Scheduler_ResumeTask(uint8_t taskId)
{
    if (taskId < g_taskCount && g_taskList[taskId].pTask != 0) {
        g_taskList[taskId].state = TASK_STATE_READY;
    }
}

/**
 * @brief 启动调度器 (合作式调度)
 */
void Scheduler_Start(void)
{
    g_schedulerRunning = 1;

    while (g_schedulerRunning) {
        /* 查找就绪的任务 */
        uint8_t tasksRun = 0;

        for (uint8_t i = 0; i < g_taskCount; i++) {
            g_currentTask = i;

            if (g_taskList[i].state == TASK_STATE_READY) {
                /* 运行任务 */
                g_taskList[i].state = TASK_STATE_RUNNING;
                g_taskList[i].runCount++;

                g_taskList[i].pTask();

                /* 任务返回，更新状态 */
                if (g_taskList[i].period > 0) {
                    /* 周期性任务，重新设置延时 */
                    g_taskList[i].delay = g_taskList[i].period;
                    g_taskList[i].state = TASK_STATE_BLOCKED;
                } else {
                    /* 单次任务，挂起 */
                    g_taskList[i].state = TASK_STATE_SUSPENDED;
                }

                tasksRun++;
            }
        }

        /* 如果没有任务运行，进入低功耗模式 */
        if (tasksRun == 0) {
            __asm volatile ("wfi");
        }
    }
}

/**
 * @brief 停止调度器
 */
void Scheduler_Stop(void)
{
    g_schedulerRunning = 0;
}

/* ============================================================================
 * 时间片轮转调度 (Round-Robin)
 * ============================================================================ */

#define TIME_SLICE_MS       10      /* 时间片大小(ms) */

/**
 * @brief 基于时间片的调度器
 * @details 每个任务运行固定时间后切换到下一个任务
 */
void Scheduler_Start_RoundRobin(void)
{
    uint8_t current = 0;
    uint32_t lastTick = g_tickCount;

    g_schedulerRunning = 1;

    while (g_schedulerRunning) {
        /* 检查是否超过时间片 */
        if ((g_tickCount - lastTick) >= TIME_SLICE_MS) {
            lastTick = g_tickCount;

            /* 切换到下一个就绪任务 */
            uint8_t next = current;
            uint8_t checked = 0;

            do {
                next = (next + 1) % g_taskCount;
                checked++;
            } while (checked < g_taskCount &&
                     g_taskList[next].state != TASK_STATE_READY);

            if (g_taskList[next].state == TASK_STATE_READY) {
                current = next;
                g_currentTask = current;

                g_taskList[current].state = TASK_STATE_RUNNING;
                g_taskList[current].runCount++;

                /* 运行任务 */
                g_taskList[current].pTask();

                /* 恢复就绪状态 */
                g_taskList[current].state = TASK_STATE_READY;
            }
        }
    }
}

/* ============================================================================
 * 实用工具函数
 * ============================================================================ */

/**
 * @brief 获取任务统计信息
 * @param taskId 任务ID
 * @param runCount 运行次数指针
 * @return 任务状态
 */
TaskState_TypeDef Scheduler_GetTaskInfo(uint8_t taskId, uint32_t *runCount)
{
    if (taskId < g_taskCount) {
        *runCount = g_taskList[taskId].runCount;
        return g_taskList[taskId].state;
    }
    return TASK_STATE_SUSPENDED;
}

/**
 * @brief 获取任务名
 * @param taskId 任务ID
 * @return 任务名字符串
 */
const char* Scheduler_GetTaskName(uint8_t taskId)
{
    if (taskId < g_taskCount) {
        return g_taskList[taskId].name;
    }
    return "INVALID";
}

/**
 * @brief 获取就绪任务数
 * @return 就绪任务数量
 */
uint8_t Scheduler_GetReadyCount(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < g_taskCount; i++) {
        if (g_taskList[i].state == TASK_STATE_READY) {
            count++;
        }
    }
    return count;
}
