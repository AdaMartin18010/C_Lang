/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 1025
 * Language: c
 * Block ID: 35d5d335
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * FreeRTOS 信号量完整示例
 * 文件: freertos_semaphore_example.c
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>

/* 信号量句柄 */
static SemaphoreHandle_t xBinarySem = NULL;
static SemaphoreHandle_t xCountingSem = NULL;
static SemaphoreHandle_t xMutex = NULL;
static SemaphoreHandle_t xRecursiveMutex = NULL;

/* 资源计数 */
static volatile uint32_t shared_resource = 0;

/*
 * 二值信号量使用示例
 * 应用场景: 任务同步、中断通知
 */
static void vBinarySemaphoreTask(void *pvParameters)
{
    (void)pvParameters;
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(1000);

    printf("[BinarySemTask] Waiting for semaphore...\n");

    for (;;) {
        /* 等待信号量 - 可从中断或另一任务释放 */
        if (xSemaphoreTake(xBinarySem, xMaxBlockTime) == pdTRUE) {
            printf("[BinarySemTask] Semaphore received, processing...\n");

            /* 执行需要同步的操作 */
            vTaskDelay(pdMS_TO_TICKS(100));

            printf("[BinarySemTask] Processing complete\n");
        } else {
            printf("[BinarySemTask] Timeout waiting for semaphore\n");
        }
    }
}

/*
 * 计数信号量使用示例
 * 应用场景: 资源池管理、事件计数
 */
static void vCountingSemaphoreProducer(void *pvParameters)
{
    (void)pvParameters;
    uint32_t produced_count = 0;

    for (;;) {
        /* 模拟生产资源 */
        vTaskDelay(pdMS_TO_TICKS(200));
        produced_count++;

        /* 释放计数信号量 - 表示可用资源 */
        if (xSemaphoreGive(xCountingSem) == pdTRUE) {
            printf("[Producer] Produced item #%lu, sem count available\n",
                   produced_count);
        } else {
            printf("[Producer] Semaphore full, item dropped\n");
        }
    }
}

static void vCountingSemaphoreConsumer(void *pvParameters)
{
    (void)pvParameters;
    uint32_t consumed_count = 0;

    for (;;) {
        /* 等待可用资源 */
        if (xSemaphoreTake(xCountingSem, portMAX_DELAY) == pdTRUE) {
            consumed_count++;
            printf("[Consumer] Consumed item #%lu\n", consumed_count);

            /* 处理资源 */
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}

/*
 * 互斥锁使用示例
 * 应用场景: 保护共享资源
 */
static void vMutexTaskA(void *pvParameters)
{
    (void)pvParameters;
    const TickType_t xDelay = pdMS_TO_TICKS(100);

    for (;;) {
        /* 获取互斥锁 - 带超时 */
        if (xSemaphoreTake(xMutex, xDelay) == pdTRUE) {
            printf("[MutexTaskA] Acquired mutex\n");

            /* 访问共享资源 */
            uint32_t temp = shared_resource;
            vTaskDelay(pdMS_TO_TICKS(10));
            shared_resource = temp + 1;
            printf("[MutexTaskA] Resource incremented to %lu\n", shared_resource);

            /* 释放互斥锁 */
            xSemaphoreGive(xMutex);
            printf("[MutexTaskA] Released mutex\n");
        } else {
            printf("[MutexTaskA] Failed to acquire mutex\n");
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

static void vMutexTaskB(void *pvParameters)
{
    (void)pvParameters;

    for (;;) {
        /* 获取互斥锁 */
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            printf("[MutexTaskB] Acquired mutex\n");

            /* 访问共享资源 */
            shared_resource *= 2;
            printf("[MutexTaskB] Resource doubled to %lu\n", shared_resource);

            xSemaphoreGive(xMutex);
            printf("[MutexTaskB] Released mutex\n");
        }

        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

/*
 * 递归互斥锁使用示例
 * 应用场景: 递归调用保护
 */
static void recursive_function(int depth);

static void vRecursiveMutexTask(void *pvParameters)
{
    (void)pvParameters;

    for (;;) {
        printf("[RecursiveTask] Starting recursive operation\n");
        recursive_function(3);
        printf("[RecursiveTask] Recursive operation complete\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void recursive_function(int depth)
{
    if (depth <= 0) {
        return;
    }

    /* 获取递归互斥锁 - 同一任务可多次获取 */
    xSemaphoreTakeRecursive(xRecursiveMutex, portMAX_DELAY);
    printf("[Recursive] Depth %d: Acquired recursive mutex\n", depth);

    /* 递归调用 */
    recursive_function(depth - 1);

    /* 释放递归互斥锁 - 必须与获取次数相同 */
    printf("[Recursive] Depth %d: Releasing recursive mutex\n", depth);
    xSemaphoreGiveRecursive(xRecursiveMutex);
}

/*
 * 中断中的信号量操作
 */
void vApplicationTickHook(void)
{
    static uint32_t tick_count = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    tick_count++;

    /* 每500个tick释放一次二值信号量 */
    if (tick_count >= 500) {
        tick_count = 0;
        xSemaphoreGiveFromISR(xBinarySem, &xHigherPriorityTaskWoken);

        /* 上下文切换（如果需要） */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

int main(void)
{
    printf("\n=== FreeRTOS Semaphore Demo ===\n\n");

    /* 创建二值信号量 */
    xBinarySem = xSemaphoreCreateBinary();
    configASSERT(xBinarySem != NULL);

    /* 创建计数信号量 - 最大10，初始0 */
    xCountingSem = xSemaphoreCreateCounting(10, 0);
    configASSERT(xCountingSem != NULL);

    /* 创建互斥锁 */
    xMutex = xSemaphoreCreateMutex();
    configASSERT(xMutex != NULL);

    /* 创建递归互斥锁 */
    xRecursiveMutex = xSemaphoreCreateRecursiveMutex();
    configASSERT(xRecursiveMutex != NULL);

    /* 创建任务 */
    xTaskCreate(vBinarySemaphoreTask, "BinarySem", 256, NULL, 2, NULL);
    xTaskCreate(vCountingSemaphoreProducer, "Producer", 256, NULL, 2, NULL);
    xTaskCreate(vCountingSemaphoreConsumer, "Consumer", 256, NULL, 2, NULL);
    xTaskCreate(vMutexTaskA, "MutexA", 256, NULL, 2, NULL);
    xTaskCreate(vMutexTaskB, "MutexB", 256, NULL, 2, NULL);
    xTaskCreate(vRecursiveMutexTask, "Recursive", 256, NULL, 2, NULL);

    vTaskStartScheduler();

    return 0;
}
