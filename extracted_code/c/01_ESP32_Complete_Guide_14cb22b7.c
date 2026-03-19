/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 2340
 * Language: c
 * Block ID: 14cb22b7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * freertos_tasks_example.c
 * ESP32 FreeRTOS 任务管理完整示例
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"

#define TASK_TAG            "FREERTOS"

/* 任务优先级定义 */
#define PRIORITY_HIGH       configMAX_PRIORITIES - 1
#define PRIORITY_NORMAL     configMAX_PRIORITIES / 2
#define PRIORITY_LOW        1

/* 任务句柄 */
static TaskHandle_t xHighPriorityTask = NULL;
static TaskHandle_t xNormalPriorityTask = NULL;
static TaskHandle_t xLowPriorityTask = NULL;

/* 同步原语 */
static SemaphoreHandle_t xMutex = NULL;
static SemaphoreHandle_t xBinarySemaphore = NULL;
static QueueHandle_t xDataQueue = NULL;
static EventGroupHandle_t xEventGroup = NULL;

/**
 * @brief 高优先级任务 - 紧急事件处理
 */
static void vHighPriorityTask(void *pvParameters)
{
    (void) pvParameters;
    uint32_t ulLoopCount = 0;

    ESP_LOGI(TASK_TAG, "高优先级任务启动");

    while (1) {
        /* 等待二进制信号量 */
        if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TASK_TAG, "高优先级任务: 处理紧急事件 #%d", ulLoopCount++);

            /* 模拟紧急处理 */
            vTaskDelay(pdMS_TO_TICKS(50));

            ESP_LOGI(TASK_TAG, "高优先级任务: 事件处理完成");
        }
    }
}

/**
 * @brief 普通优先级任务 - 常规数据处理
 */
static void vNormalPriorityTask(void *pvParameters)
{
    (void) pvParameters;
    uint32_t ulReceivedValue;
    uint32_t ulLoopCount = 0;

    ESP_LOGI(TASK_TAG, "普通优先级任务启动");

    while (1) {
        /* 从队列接收数据 */
        if (xQueueReceive(xDataQueue, &ulReceivedValue, portMAX_DELAY) == pdTRUE) {
            /* 获取互斥锁保护共享资源 */
            if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
                ESP_LOGI(TASK_TAG, "普通优先级任务: 处理数据 %d (计数: %d)",
                         (int)ulReceivedValue, (int)ulLoopCount++);

                /* 模拟数据处理 */
                vTaskDelay(pdMS_TO_TICKS(100));

                xSemaphoreGive(xMutex);
            }
        }
    }
}

/**
 * @brief 低优先级任务 - 后台维护
 */
static void vLowPriorityTask(void *pvParameters)
{
    (void) pvParameters;
    const EventBits_t xBitsToWaitFor = (BIT_0 | BIT_1);
    EventBits_t uxBits;
    uint32_t ulLoopCount = 0;

    ESP_LOGI(TASK_TAG, "低优先级任务启动");

    while (1) {
        /* 等待事件组 */
        uxBits = xEventGroupWaitBits(
            xEventGroup,        /* 事件组 */
            xBitsToWaitFor,     /* 等待的位 */
            pdTRUE,             /* 清除位 */
            pdFALSE,            /* 不需要所有位都设置 */
            pdMS_TO_TICKS(5000) /* 超时 5 秒 */
        );

        if ((uxBits & BIT_0) != 0) {
            ESP_LOGI(TASK_TAG, "低优先级任务: 收到事件 BIT_0 (计数: %d)",
                     (int)ulLoopCount++);
        }

        if ((uxBits & BIT_1) != 0) {
            ESP_LOGI(TASK_TAG, "低优先级任务: 收到事件 BIT_1 (计数: %d)",
                     (int)ulLoopCount++);
        }

        if (uxBits == 0) {
            /* 超时,执行周期性维护 */
            ESP_LOGD(TASK_TAG, "低优先级任务: 执行周期性维护");
        }

        /* 让出 CPU */
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
 * @brief 软件定时器回调
 */
static void vTimerCallback(TimerHandle_t xTimer)
{
    static uint32_t ulTimerCount = 0;

    /* 获取定时器 ID */
    uint32_t ulTimerID = (uint32_t)pvTimerGetTimerID(xTimer);

    ESP_LOGI(TASK_TAG, "定时器 #%d 触发 (ID: %d)",
             (int)ulTimerCount++, (int)ulTimerID);

    /* 发送数据到队列 */
    uint32_t ulData = ulTimerCount;
    xQueueSend(xDataQueue, &ulData, 0);

    /* 设置事件位 */
    xEventGroupSetBits(xEventGroup, BIT_0);
}

/**
 * @brief 生产者任务 - 生成数据
 */
static void vProducerTask(void *pvParameters)
{
    (void) pvParameters;
    uint32_t ulData = 0;

    while (1) {
        /* 生成数据 */
        ulData++;

        /* 发送到队列 (不阻塞) */
        if (xQueueSend(xDataQueue, &ulData, pdMS_TO_TICKS(100)) != pdTRUE) {
            ESP_LOGW(TASK_TAG, "队列已满,数据丢弃");
        }

        /* 每隔 10 个数据触发一次紧急事件 */
        if (ulData % 10 == 0) {
            xSemaphoreGive(xBinarySemaphore);
        }

        /* 每隔 5 个数据设置事件 */
        if (ulData % 5 == 0) {
            xEventGroupSetBits(xEventGroup, BIT_1);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief 任务监控 - 打印任务状态
 */
static void vTaskMonitor(void *pvParameters)
{
    (void) pvParameters;
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    uint32_t ulTotalRunTime, ulStatsAsPercentage;

    while (1) {
        /* 获取任务数量 */
        uxArraySize = uxTaskGetNumberOfTasks();

        /* 分配内存 */
        pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

        if (pxTaskStatusArray != NULL) {
            /* 获取任务状态 */
            uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);

            ESP_LOGI(TASK_TAG, "=== 任务状态报告 ===");
            ESP_LOGI(TASK_TAG, "任务数: %d, 总运行时间: %d",
                     (int)uxArraySize, (int)ulTotalRunTime);

            /* 打印每个任务的信息 */
            for (x = 0; x < uxArraySize; x++) {
                /* 计算 CPU 使用率 */
                ulStatsAsPercentage = pxTaskStatusArray[x].ulRunTimeCounter /
                                      (ulTotalRunTime / 100UL);

                if (ulStatsAsPercentage > 0UL) {
                    ESP_LOGI(TASK_TAG, "  %-16s: 堆栈 %d/%d, CPU %d%%, 优先级 %d",
                             pxTaskStatusArray[x].pcTaskName,
                             (int)pxTaskStatusArray[x].usStackHighWaterMark,
                             (int)pxTaskStatusArray[x].uxCurrentPriority,
                             (int)ulStatsAsPercentage,
                             (int)pxTaskStatusArray[x].uxBasePriority);
                } else {
                    ESP_LOGI(TASK_TAG, "  %-16s: 堆栈 %d, 优先级 %d",
                             pxTaskStatusArray[x].pcTaskName,
                             (int)pxTaskStatusArray[x].usStackHighWaterMark,
                             (int)pxTaskStatusArray[x].uxBasePriority);
                }
            }

            vPortFree(pxTaskStatusArray);
        }

        vTaskDelay(pdMS_TO_TICKS(10000));  /* 每 10 秒报告一次 */
    }
}

/**
 * @brief FreeRTOS 演示初始化
 */
static void freertos_demo_init(void)
{
    /* 创建同步原语 */
    xMutex = xSemaphoreCreateMutex();
    xBinarySemaphore = xSemaphoreCreateBinary();
    xDataQueue = xQueueCreate(20, sizeof(uint32_t));
    xEventGroup = xEventGroupCreate();

    /* 创建任务 */
    xTaskCreatePinnedToCore(vHighPriorityTask, "HighTask", 2048, NULL,
                            PRIORITY_HIGH, &xHighPriorityTask, 1);

    xTaskCreatePinnedToCore(vNormalPriorityTask, "NormalTask", 2048, NULL,
                            PRIORITY_NORMAL, &xNormalPriorityTask, 0);

    xTaskCreatePinnedToCore(vLowPriorityTask, "LowTask", 2048, NULL,
                            PRIORITY_LOW, &xLowPriorityTask, 0);

    xTaskCreatePinnedToCore(vProducerTask, "Producer", 2048, NULL,
                            PRIORITY_NORMAL, NULL, 1);

    xTaskCreatePinnedToCore(vTaskMonitor, "Monitor", 4096, NULL,
                            PRIORITY_LOW, NULL, 0);

    /* 创建软件定时器 */
    TimerHandle_t xTimer = xTimerCreate("DemoTimer",
                                        pdMS_TO_TICKS(2000),  /* 2 秒周期 */
                                        pdTRUE,               /* 自动重载 */
                                        (void *)1,            /* 定时器 ID */
                                        vTimerCallback);

    if (xTimer != NULL) {
        xTimerStart(xTimer, 0);
    }

    ESP_LOGI(TASK_TAG, "FreeRTOS 演示系统初始化完成");
}
