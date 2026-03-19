/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 5227
 * Language: c
 * Block ID: 075e2192
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * FreeRTOS 温度监控系统
 * 文件: freertos_temp_monitor.c
 * 功能: 采集温度、显示、报警、网络上报
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* 配置 */
#define TEMP_SAMPLE_PERIOD_MS       1000
#define ALARM_HIGH_THRESHOLD        35.0f
#define ALARM_LOW_THRESHOLD         10.0f
#define QUEUE_SIZE                  10
#define MAX_SENSORS                 4

/* 事件位 */
#define EVENT_SENSOR_OK         (1 << 0)
#define EVENT_ALARM_HIGH        (1 << 1)
#define EVENT_ALARM_LOW         (1 << 2)
#define EVENT_NETWORK_OK        (1 << 3)
#define EVENT_DISPLAY_UPDATE    (1 << 4)

/* 数据结构 */
typedef struct {
    uint8_t sensor_id;
    float temperature;
    uint32_t timestamp;
    uint8_t status;
} TempReading_t;

typedef struct {
    uint32_t total_readings;
    uint32_t alarm_count;
    float avg_temp;
    float max_temp;
    float min_temp;
} TempStats_t;

/* 全局对象 */
static QueueHandle_t xTempQueue = NULL;
static EventGroupHandle_t xEventGroup = NULL;
static SemaphoreHandle_t xStatsMutex = NULL;
static TimerHandle_t xSampleTimer = NULL;

/* 统计 */
static TempStats_t xStats = {0};
static float xCurrentTemps[MAX_SENSORS] = {0};

/*
 * 模拟硬件接口
 */
static float read_sensor_hw(uint8_t sensor_id)
{
    /* 模拟传感器读取 */
    float base = 25.0f;
    float noise = (float)(rand() % 100) / 10.0f - 5.0f;
    float drift = (float)(xTaskGetTickCount() % 1000) / 100.0f;
    return base + noise + drift - (sensor_id * 2);
}

/*
 * 采样任务
 */
static void vSampleTask(void *pvParameters)
{
    (void)pvParameters;
    uint8_t current_sensor = 0;

    printf("[SampleTask] Started\n");

    for (;;) {
        /* 读取所有传感器 */
        for (current_sensor = 0; current_sensor < MAX_SENSORS; current_sensor++) {
            TempReading_t reading;
            reading.sensor_id = current_sensor;
            reading.temperature = read_sensor_hw(current_sensor);
            reading.timestamp = xTaskGetTickCount();
            reading.status = 0;

            /* 验证读数 */
            if (reading.temperature > -40.0f && reading.temperature < 85.0f) {
                reading.status |= EVENT_SENSOR_OK;
            }

            /* 发送到队列 */
            if (xQueueSend(xTempQueue, &reading, pdMS_TO_TICKS(100)) != pdPASS) {
                printf("[SampleTask] Queue full, dropping reading\n");
            }

            xCurrentTemps[current_sensor] = reading.temperature;
        }

        vTaskDelay(pdMS_TO_TICKS(TEMP_SAMPLE_PERIOD_MS));
    }
}

/*
 * 处理任务
 */
static void vProcessTask(void *pvParameters)
{
    (void)pvParameters;
    TempReading_t reading;

    printf("[ProcessTask] Started\n");

    for (;;) {
        /* 等待读数 */
        if (xQueueReceive(xTempQueue, &reading, portMAX_DELAY) == pdPASS) {
            EventBits_t events = 0;

            /* 更新统计 */
            if (xSemaphoreTake(xStatsMutex, pdMS_TO_TICKS(100)) == pdPASS) {
                xStats.total_readings++;

                /* 计算统计 */
                float sum = 0;
                for (int i = 0; i < MAX_SENSORS; i++) {
                    sum += xCurrentTemps[i];
                }
                xStats.avg_temp = sum / MAX_SENSORS;

                /* 更新最大/最小 */
                if (reading.temperature > xStats.max_temp) {
                    xStats.max_temp = reading.temperature;
                }
                if (reading.temperature < xStats.min_temp || xStats.min_temp == 0) {
                    xStats.min_temp = reading.temperature;
                }

                xSemaphoreGive(xStatsMutex);
            }

            /* 检查告警 */
            if (reading.temperature > ALARM_HIGH_THRESHOLD) {
                events |= EVENT_ALARM_HIGH;
                xStats.alarm_count++;
                printf("[ProcessTask] HIGH ALARM: Sensor %u = %.2f C\n",
                       reading.sensor_id, reading.temperature);
            }

            if (reading.temperature < ALARM_LOW_THRESHOLD) {
                events |= EVENT_ALARM_LOW;
                xStats.alarm_count++;
                printf("[ProcessTask] LOW ALARM: Sensor %u = %.2f C\n",
                       reading.sensor_id, reading.temperature);
            }

            /* 设置事件 */
            if (events != 0) {
                xEventGroupSetBits(xEventGroup, events);
            }

            /* 触发显示更新 */
            xEventGroupSetBits(xEventGroup, EVENT_DISPLAY_UPDATE);
        }
    }
}

/*
 * 显示任务
 */
static void vDisplayTask(void *pvParameters)
{
    (void)pvParameters;
    EventBits_t events;

    printf("[DisplayTask] Started\n");

    for (;;) {
        /* 等待显示更新事件 */
        events = xEventGroupWaitBits(
            xEventGroup,
            EVENT_DISPLAY_UPDATE,
            pdTRUE,
            pdFALSE,
            pdMS_TO_TICKS(5000)
        );

        if (events & EVENT_DISPLAY_UPDATE) {
            printf("\n========== Temperature Monitor ==========\n");

            /* 显示当前读数 */
            for (int i = 0; i < MAX_SENSORS; i++) {
                printf("  Sensor %d: %.2f C\n", i, xCurrentTemps[i]);
            }

            /* 显示统计 */
            if (xSemaphoreTake(xStatsMutex, pdMS_TO_TICKS(100)) == pdPASS) {
                printf("\n  Statistics:\n");
                printf("    Total readings: %lu\n", xStats.total_readings);
                printf("    Alarm count: %lu\n", xStats.alarm_count);
                printf("    Average: %.2f C\n", xStats.avg_temp);
                printf("    Max: %.2f C, Min: %.2f C\n",
                       xStats.max_temp, xStats.min_temp);
                xSemaphoreGive(xStatsMutex);
            }

            printf("========================================\n");
        }
    }
}

/*
 * 告警任务
 */
static void vAlarmTask(void *pvParameters)
{
    (void)pvParameters;
    EventBits_t events;

    printf("[AlarmTask] Started\n");

    for (;;) {
        /* 等待告警事件 */
        events = xEventGroupWaitBits(
            xEventGroup,
            EVENT_ALARM_HIGH | EVENT_ALARM_LOW,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY
        );

        if (events & EVENT_ALARM_HIGH) {
            /* 高温告警处理 */
            printf("[AlarmTask] *** HIGH TEMPERATURE ALARM ***\n");
            /* 可触发蜂鸣器、LED等 */
        }

        if (events & EVENT_ALARM_LOW) {
            /* 低温告警处理 */
            printf("[AlarmTask] *** LOW TEMPERATURE ALARM ***\n");
        }
    }
}

/*
 * 网络任务（模拟）
 */
static void vNetworkTask(void *pvParameters)
{
    (void)pvParameters;

    printf("[NetworkTask] Started\n");

    /* 模拟网络连接 */
    vTaskDelay(pdMS_TO_TICKS(2000));
    xEventGroupSetBits(xEventGroup, EVENT_NETWORK_OK);
    printf("[NetworkTask] Network connected\n");

    for (;;) {
        /* 等待网络就绪 */
        EventBits_t events = xEventGroupWaitBits(
            xEventGroup,
            EVENT_NETWORK_OK,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY
        );

        if (events & EVENT_NETWORK_OK) {
            /* 准备上报数据 */
            char json_buffer[256];
            snprintf(json_buffer, sizeof(json_buffer),
                "{\"sensors\":[%.2f,%.2f,%.2f,%.2f],\"avg\":%.2f}",
                xCurrentTemps[0], xCurrentTemps[1],
                xCurrentTemps[2], xCurrentTemps[3],
                xStats.avg_temp);

            printf("[NetworkTask] Uploading: %s\n", json_buffer);

            /* 实际网络发送在这里 */
            vTaskDelay(pdMS_TO_TICKS(100));
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

/*
 * 定时器回调
 */
static void vSampleTimerCallback(TimerHandle_t xTimer)
{
    (void)xTimer;
    /* 可触发采样或定期检查 */
    printf("[Timer] Sample period elapsed\n");
}

/*
 * 主函数
 */
int main(void)
{
    printf("\n=== FreeRTOS Temperature Monitor ===\n\n");

    /* 创建同步对象 */
    xTempQueue = xQueueCreate(QUEUE_SIZE, sizeof(TempReading_t));
    configASSERT(xTempQueue != NULL);

    xEventGroup = xEventGroupCreate();
    configASSERT(xEventGroup != NULL);

    xStatsMutex = xSemaphoreCreateMutex();
    configASSERT(xStatsMutex != NULL);

    /* 创建定时器 */
    xSampleTimer = xTimerCreate(
        "SampleTimer",
        pdMS_TO_TICKS(5000),
        pdTRUE,
        NULL,
        vSampleTimerCallback
    );
    configASSERT(xSampleTimer != NULL);
    xTimerStart(xSampleTimer, 0);

    /* 创建任务 */
    xTaskCreate(vSampleTask, "Sample", 512, NULL, 3, NULL);
    xTaskCreate(vProcessTask, "Process", 512, NULL, 2, NULL);
    xTaskCreate(vDisplayTask, "Display", 512, NULL, 1, NULL);
    xTaskCreate(vAlarmTask, "Alarm", 512, NULL, 4, NULL);
    xTaskCreate(vNetworkTask, "Network", 1024, NULL, 2, NULL);

    printf("[Main] Starting scheduler...\n\n");
    vTaskStartScheduler();

    return 0;
}
