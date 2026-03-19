/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 338
 * Language: c
 * Block ID: e439d970
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * FreeRTOS 任务管理完整示例
 * 文件: freertos_task_example.c
 * 编译: gcc -o freertos_task_example freertos_task_example.c -lfreertos
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>
#include <string.h>

/* 任务参数结构体 - 用于传递多个参数 */
typedef struct {
    uint32_t task_id;
    uint32_t interval_ms;
    const char* name;
} TaskParams_t;

/* 静态任务分配 */
#define STATIC_TASK_STACK_SIZE  256
#define STATIC_TASK_PRIORITY    2

/* 静态任务TCB和栈 */
static StaticTask_t xIdleTaskTCB;
static StaticTask_t xTimerTaskTCB;
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

/* 应用任务 */
static TaskHandle_t xTaskHandles[3] = {NULL};
static StaticTask_t xTaskTCBs[3];
static StackType_t xTaskStacks[3][256];

/* 同步信号量 */
static SemaphoreHandle_t xStartSemaphore = NULL;

/*
 * 任务1: 周期性任务
 * 功能: 周期性执行工作，演示基本任务结构
 */
static void vPeriodicTask(void *pvParameters)
{
    TaskParams_t *params = (TaskParams_t *)pvParameters;
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(params->interval_ms);
    uint32_t execution_count = 0;

    /* 等待启动信号 */
    xSemaphoreTake(xStartSemaphore, portMAX_DELAY);

    /* 初始化上次唤醒时间 */
    xLastWakeTime = xTaskGetTickCount();

    printf("[Task %s] Started, ID=%lu, Interval=%lu ms\n",
           params->name, params->task_id, params->interval_ms);

    for (;;) {
        /* 周期性执行 - 使用vTaskDelayUntil确保精确周期 */
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        execution_count++;
        printf("[Task %s] Execution #%lu at tick %lu\n",
               params->name, execution_count, xTaskGetTickCount());

        /* 模拟工作负载 */
        {
            volatile uint32_t i;
            for (i = 0; i < 10000; i++);
        }

        /* 优先级继承演示 */
        if (execution_count >= 10) {
            printf("[Task %s] Reached max executions, suspending\n", params->name);
            vTaskSuspend(NULL);  /* 挂起自身 */
        }
    }
}

/*
 * 任务2: 事件驱动任务
 * 功能: 等待通知后执行工作
 */
static void vEventDrivenTask(void *pvParameters)
{
    TaskParams_t *params = (TaskParams_t *)pvParameters;
    uint32_t ulNotificationValue;
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(5000);

    printf("[Task %s] Event-driven task started\n", params->name);

    for (;;) {
        /* 等待任务通知，带超时 */
        ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);

        if (ulNotificationValue > 0) {
            printf("[Task %s] Received notification, value=%lu\n",
                   params->name, ulNotificationValue);

            /* 处理事件 */
            switch (ulNotificationValue) {
                case 1:
                    printf("[Task %s] Processing EVENT_TYPE_A\n", params->name);
                    break;
                case 2:
                    printf("[Task %s] Processing EVENT_TYPE_B\n", params->name);
                    break;
                default:
                    printf("[Task %s] Processing UNKNOWN_EVENT\n", params->name);
                    break;
            }
        } else {
            printf("[Task %s] Timeout waiting for notification\n", params->name);
        }
    }
}

/*
 * 任务3: 一次性初始化任务
 * 功能: 执行系统初始化后删除自身
 */
static void vInitTask(void *pvParameters)
{
    (void)pvParameters;

    printf("[InitTask] System initialization started\n");

    /* 执行硬件初始化 */
    printf("[InitTask] Initializing peripherals...\n");
    vTaskDelay(pdMS_TO_TICKS(100));

    /* 初始化软件组件 */
    printf("[InitTask] Initializing software components...\n");
    vTaskDelay(pdMS_TO_TICKS(100));

    /* 释放启动信号量，允许其他任务运行 */
    printf("[InitTask] Releasing start semaphore\n");
    xSemaphoreGive(xStartSemaphore);

    printf("[InitTask] Initialization complete, deleting self\n");

    /* 删除自身 - 任务结束 */
    vTaskDelete(NULL);
}

/*
 * 空闲任务钩子
 * 功能: 在系统空闲时执行低优先级工作
 */
void vApplicationIdleHook(void)
{
    static uint32_t idle_count = 0;
    idle_count++;

    /* 每1000次空闲调用执行一次 */
    if (idle_count >= 1000) {
        idle_count = 0;
        /* 可执行节能操作或后台任务 */
    }
}

/*
 * 栈溢出钩子
 * 功能: 检测任务栈溢出时调用
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    printf("[ERROR] Stack overflow in task: %s\n", pcTaskName);
    /* 在此可添加错误处理，如系统复位 */
    for (;;);
}

/*
 * 内存分配失败钩子
 */
void vApplicationMallocFailedHook(void)
{
    printf("[ERROR] Memory allocation failed\n");
    for (;;);
}

/*
 * 静态分配回调函数
 * 用于FreeRTOS使用静态分配时提供内存
 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/* 任务参数 */
static TaskParams_t xTaskParams[3] = {
    {1, 100, "Periodic100"},
    {2, 500, "Periodic500"},
    {3, 0,   "EventDriven"}
};

/*
 * 主函数
 */
int main(void)
{
    printf("\n=== FreeRTOS Task Management Demo ===\n\n");

    /* 创建启动信号量 */
    xStartSemaphore = xSemaphoreCreateBinary();
    configASSERT(xStartSemaphore != NULL);

    /* 创建初始化任务 - 最高优先级 */
    xTaskCreate(vInitTask,
                "Init",
                256,
                NULL,
                configMAX_PRIORITIES - 1,
                NULL);

    /* 创建周期任务1 - 静态分配 */
    xTaskHandles[0] = xTaskCreateStatic(
        vPeriodicTask,
        "Periodic100",
        256,
        &xTaskParams[0],
        3,
        xTaskStacks[0],
        &xTaskTCBs[0]
    );
    configASSERT(xTaskHandles[0] != NULL);

    /* 创建周期任务2 - 动态分配 */
    BaseType_t result = xTaskCreate(
        vPeriodicTask,
        "Periodic500",
        256,
        &xTaskParams[1],
        2,
        &xTaskHandles[1]
    );
    configASSERT(result == pdPASS);

    /* 创建事件驱动任务 */
    result = xTaskCreate(
        vEventDrivenTask,
        "EventTask",
        256,
        &xTaskParams[2],
        1,
        &xTaskHandles[2]
    );
    configASSERT(result == pdPASS);

    printf("[Main] All tasks created, starting scheduler...\n\n");

    /* 启动调度器 */
    vTaskStartScheduler();

    /* 不应该到达这里 */
    printf("[ERROR] Scheduler returned unexpectedly\n");
    return 1;
}
