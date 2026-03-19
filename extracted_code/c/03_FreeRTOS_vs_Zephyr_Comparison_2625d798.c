/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 2997
 * Language: c
 * Block ID: 2625d798
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * FreeRTOS 中断处理示例
 * 文件: freertos_interrupt_example.c
 * 说明: 展示中断处理、临界区和延迟处理
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdio.h>
#include <string.h>

/* 中断配置（ARM Cortex-M为例） */
#define IRQ_PRIORITY_LOW        5
#define IRQ_PRIORITY_MEDIUM     3
#define IRQ_PRIORITY_HIGH       1
#define IRQ_PRIORITY_HIGHEST    0

/* 用于中断同步的对象 */
static QueueHandle_t xISRQueue = NULL;
static SemaphoreHandle_t xISRSemaphore = NULL;
static EventGroupHandle_t xISREventGroup = NULL;

/* 统计 */
static volatile uint32_t isr_count = 0;
static volatile uint32_t deferred_count = 0;

/*
 * 临界区演示函数
 */
static void critical_section_demo(void)
{
    /* 方法1: 禁用中断（最简单） */
    taskENTER_CRITICAL();
    {
        /* 访问共享资源 */
        isr_count++;
    }
    taskEXIT_CRITICAL();

    /* 方法2: 禁用中断（带嵌套支持） */
    UBaseType_t uxSavedInterruptStatus;
    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
    {
        isr_count++;
    }
    taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);

    /* 方法3: 使用互斥锁（仅任务级别） */
    /* xSemaphoreTake(xMutex, portMAX_DELAY); */
    /* ... */
    /* xSemaphoreGive(xMutex); */
}

/*
 * 模拟中断处理程序 - 高优先级（零延迟）
 * 注意: 这种中断不使用FreeRTOS API
 */
void TIM2_IRQHandler(void)
{
    /* 最高优先级中断 - 不调用任何RTOS API */
    /* 直接访问硬件，最小延迟 */

    /* 清除中断标志 */
    /* TIM2->SR &= ~TIM_SR_UIF; */

    /* 快速操作 */
    isr_count++;
}

/*
 * 模拟中断处理程序 - 中等优先级
 * 使用FromISR API与RTOS交互
 */
void TIM3_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t data;

    /* 清除中断标志 */
    /* TIM3->SR &= ~TIM_SR_UIF; */

    /* 准备数据 */
    data = isr_count;

    /* 发送到队列 - FromISR版本 */
    xQueueSendFromISR(xISRQueue, &data, &xHigherPriorityTaskWoken);

    /* 释放信号量 */
    xSemaphoreGiveFromISR(xISRSemaphore, &xHigherPriorityTaskWoken);

    /* 设置事件位 */
    xEventGroupSetBitsFromISR(xISREventGroup, (1 << 0), &xHigherPriorityTaskWoken);

    /* 上下文切换（如果需要） */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*
 * 模拟中断处理程序 - 低优先级
 * 使用延迟处理模式
 */
void TIM4_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* 清除中断标志 */
    /* TIM4->SR &= ~TIM_SR_UIF; */

    /* 仅设置标志或通知，实际处理推迟到任务 */
    /* 方法1: 任务通知 */
    /* vTaskNotifyGiveFromISR(xDeferredTaskHandle, &xHigherPriorityTaskWoken); */

    /* 方法2: 释放信号量 */
    xSemaphoreGiveFromISR(xISRSemaphore, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*
 * 延迟处理任务
 * 功能: 处理中断中无法完成的复杂操作
 */
static void vDeferredISRTask(void *pvParameters)
{
    (void)pvParameters;
    uint32_t received_data;

    printf("[DeferredISRTask] Started\n");

    for (;;) {
        /* 等待中断信号 */
        if (xSemaphoreTake(xISRSemaphore, portMAX_DELAY) == pdTRUE) {
            /* 接收队列数据 */
            while (xQueueReceive(xISRQueue, &received_data, 0) == pdPASS) {
                deferred_count++;

                /* 执行复杂处理（可能耗时较长） */
                printf("[DeferredISRTask] Processing ISR data #%lu: %lu\n",
                       deferred_count, received_data);

                /* 模拟处理时间 */
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
    }
}

/*
 * 中断处理守护任务
 * 功能: 协调多个中断源
 */
static void vInterruptDaemonTask(void *pvParameters)
{
    (void)pvParameters;
    EventBits_t uxBits;

    printf("[InterruptDaemon] Started\n");

    for (;;) {
        /* 等待多个中断事件 */
        uxBits = xEventGroupWaitBits(
            xISREventGroup,
            (1 << 0) | (1 << 1) | (1 << 2),
            pdTRUE,        /* 清除已设置的位 */
            pdFALSE,       /* 等待任一事件 */
            portMAX_DELAY
        );

        if (uxBits & (1 << 0)) {
            printf("[InterruptDaemon] Timer interrupt handled\n");
        }
        if (uxBits & (1 << 1)) {
            printf("[InterruptDaemon] UART interrupt handled\n");
        }
        if (uxBits & (1 << 2)) {
            printf("[InterruptDaemon] DMA interrupt handled\n");
        }
    }
}

/*
 * 中断安全的数据结构访问
 */
typedef struct {
    uint32_t counter;
    uint32_t timestamp;
    uint8_t status;
} SharedData_t;

static SharedData_t xSharedData;
static SemaphoreHandle_t xDataMutex = NULL;

static void safe_data_update(uint32_t new_counter)
{
    /* 使用临界区保护短暂操作 */
    taskENTER_CRITICAL();
    xSharedData.counter = new_counter;
    xSharedData.timestamp = xTaskGetTickCount();
    taskEXIT_CRITICAL();
}

static void safe_data_read(SharedData_t *dest)
{
    /* 禁用中断以保护读取操作 */
    UBaseType_t uxSavedInterruptStatus;
    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
    memcpy(dest, &xSharedData, sizeof(SharedData_t));
    taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
}

/*
 * 中断嵌套演示
 */
void EXTI_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* 外部中断处理 */
    if (/* EXTI->PR & EXTI_PR_PR0 */) {
        /* 清除标志 */
        /* EXTI->PR = EXTI_PR_PR0; */

        /* 触发更高优先级的定时器操作 */
        xSemaphoreGiveFromISR(xISRSemaphore, &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*
 * 主函数
 */
int main(void)
{
    printf("\n=== FreeRTOS Interrupt Handling Demo ===\n\n");

    /* 创建同步对象 */
    xISRQueue = xQueueCreate(20, sizeof(uint32_t));
    configASSERT(xISRQueue != NULL);

    xISRSemaphore = xSemaphoreCreateBinary();
    configASSERT(xISRSemaphore != NULL);

    xISREventGroup = xEventGroupCreate();
    configASSERT(xISREventGroup != NULL);

    xDataMutex = xSemaphoreCreateMutex();
    configASSERT(xDataMutex != NULL);

    /* 创建中断处理任务 */
    xTaskCreate(vDeferredISRTask, "DeferredISR", 512, NULL, 3, NULL);
    xTaskCreate(vInterruptDaemonTask, "IntDaemon", 512, NULL, 2, NULL);

    /* 配置中断优先级（平台相关） */
    /* NVIC_SetPriority(TIM2_IRQn, IRQ_PRIORITY_HIGHEST); */
    /* NVIC_SetPriority(TIM3_IRQn, IRQ_PRIORITY_MEDIUM); */
    /* NVIC_SetPriority(TIM4_IRQn, IRQ_PRIORITY_LOW); */

    /* 使能中断 */
    /* NVIC_EnableIRQ(TIM2_IRQn); */
    /* NVIC_EnableIRQ(TIM3_IRQn); */
    /* NVIC_EnableIRQ(TIM4_IRQn); */

    printf("[Main] Starting scheduler...\n");
    vTaskStartScheduler();

    return 0;
}
