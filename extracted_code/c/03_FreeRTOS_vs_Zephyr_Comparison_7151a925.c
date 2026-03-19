/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 1588
 * Language: c
 * Block ID: 7151a925
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * FreeRTOS 事件组示例
 * 文件: freertos_eventgroup_example.c
 */

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include <stdio.h>

/* 事件位定义 */
#define EVENT_WIFI_CONNECTED    (1 << 0)    /* bit 0: WiFi连接 */
#define EVENT_WIFI_GOT_IP       (1 << 1)    /* bit 1: 获取IP */
#define EVENT_MQTT_CONNECTED    (1 << 2)    /* bit 2: MQTT连接 */
#define EVENT_SENSOR_READY      (1 << 3)    /* bit 3: 传感器就绪 */
#define EVENT_DATA_READY        (1 << 4)    /* bit 4: 数据就绪 */

/* 复合事件 */
#define EVENT_NETWORK_READY     (EVENT_WIFI_CONNECTED | EVENT_WIFI_GOT_IP)
#define EVENT_ALL_READY         (EVENT_NETWORK_READY | EVENT_MQTT_CONNECTED)

/* 事件组句柄 */
static EventGroupHandle_t xEventGroup = NULL;

/*
 * WiFi管理任务
 */
static void vWiFiTask(void *pvParameters)
{
    (void)pvParameters;

    for (;;) {
        printf("[WiFiTask] Connecting to WiFi...\n");
        vTaskDelay(pdMS_TO_TICKS(2000));

        /* WiFi连接成功 */
        xEventGroupSetBits(xEventGroup, EVENT_WIFI_CONNECTED);
        printf("[WiFiTask] WiFi connected\n");

        vTaskDelay(pdMS_TO_TICKS(1000));

        /* 获取IP地址 */
        xEventGroupSetBits(xEventGroup, EVENT_WIFI_GOT_IP);
        printf("[WiFiTask] Got IP address\n");

        /* 等待断开连接事件或其他操作 */
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

/*
 * MQTT客户端任务
 */
static void vMQTTTask(void *pvParameters)
{
    (void)pvParameters;
    EventBits_t uxBits;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(10000);

    for (;;) {
        printf("[MQTTTask] Waiting for network...\n");

        /* 等待网络就绪事件 - 需要WiFi连接且获取IP */
        uxBits = xEventGroupWaitBits(
            xEventGroup,
            EVENT_NETWORK_READY,      /* 等待的位 */
            pdFALSE,                   /* 退出时不清除 */
            pdTRUE,                    /* 必须所有位都设置 */
            xTicksToWait
        );

        if ((uxBits & EVENT_NETWORK_READY) == EVENT_NETWORK_READY) {
            printf("[MQTTTask] Network ready, connecting to MQTT broker...\n");
            vTaskDelay(pdMS_TO_TICKS(1000));

            /* MQTT连接成功 */
            xEventGroupSetBits(xEventGroup, EVENT_MQTT_CONNECTED);
            printf("[MQTTTask] MQTT connected\n");
        } else {
            printf("[MQTTTask] Network timeout\n");
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

/*
 * 传感器任务
 */
static void vSensorTask(void *pvParameters)
{
    (void)pvParameters;
    EventBits_t uxBits;

    for (;;) {
        printf("[SensorTask] Initializing sensors...\n");
        vTaskDelay(pdMS_TO_TICKS(500));

        /* 传感器就绪 */
        xEventGroupSetBits(xEventGroup, EVENT_SENSOR_READY);
        printf("[SensorTask] Sensors ready\n");

        /* 等待所有系统就绪 */
        uxBits = xEventGroupWaitBits(
            xEventGroup,
            EVENT_ALL_READY,
            pdFALSE,
            pdTRUE,
            portMAX_DELAY
        );

        if ((uxBits & EVENT_ALL_READY) == EVENT_ALL_READY) {
            printf("[SensorTask] All systems ready, starting data collection\n");

            for (int i = 0; i < 5; i++) {
                vTaskDelay(pdMS_TO_TICKS(1000));
                printf("[SensorTask] Collected sample #%d\n", i + 1);

                /* 数据就绪事件 */
                xEventGroupSetBits(xEventGroup, EVENT_DATA_READY);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

/*
 * 数据处理任务
 */
static void vDataProcessingTask(void *pvParameters)
{
    (void)pvParameters;
    EventBits_t uxBits;
    uint32_t data_count = 0;

    for (;;) {
        /* 等待数据就绪事件 */
        uxBits = xEventGroupWaitBits(
            xEventGroup,
            EVENT_DATA_READY,
            pdTRUE,                    /* 清除该位 */
            pdFALSE,
            portMAX_DELAY
        );

        if (uxBits & EVENT_DATA_READY) {
            data_count++;
            printf("[DataTask] Processing data packet #%lu\n", data_count);
        }
    }
}

/*
 * 系统监控任务
 */
static void vMonitorTask(void *pvParameters)
{
    (void)pvParameters;
    EventBits_t uxBits;

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(3000));

        /* 获取当前事件状态（不清除） */
        uxBits = xEventGroupGetBits(xEventGroup);

        printf("[Monitor] Event status:\n");
        printf("  WiFi Connected: %s\n",
               (uxBits & EVENT_WIFI_CONNECTED) ? "YES" : "NO");
        printf("  Got IP: %s\n",
               (uxBits & EVENT_WIFI_GOT_IP) ? "YES" : "NO");
        printf("  MQTT Connected: %s\n",
               (uxBits & EVENT_MQTT_CONNECTED) ? "YES" : "NO");
        printf("  Sensor Ready: %s\n",
               (uxBits & EVENT_SENSOR_READY) ? "YES" : "NO");
        printf("  Data Ready: %s\n",
               (uxBits & EVENT_DATA_READY) ? "YES" : "NO");
    }
}

/*
 * 中断中使用事件组
 */
void vApplicationTickHook(void)
{
    static uint32_t tick_count = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    tick_count++;

    /* 模拟周期性中断事件 */
    if (tick_count >= 1000) {
        tick_count = 0;
        xEventGroupSetBitsFromISR(
            xEventGroup,
            EVENT_SENSOR_READY,
            &xHigherPriorityTaskWoken
        );
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/*
 * 同步点示例 - 多任务同步
 */
static void vSyncPointTask(void *pvParameters)
{
    uint32_t task_id = (uint32_t)pvParameters;

    printf("[SyncTask%u] Started, doing initialization...\n", task_id);

    /* 模拟不同初始化时间 */
    vTaskDelay(pdMS_TO_TICKS(500 * task_id));

    printf("[SyncTask%u] Initialization complete, waiting at barrier\n", task_id);

    /* 设置自己的完成位 */
    xEventGroupSync(
        xEventGroup,
        (1 << (5 + task_id)),      /* 设置自己的位 */
        (1 << 5) | (1 << 6) | (1 << 7), /* 等待所有任务 */
        portMAX_DELAY
    );

    printf("[SyncTask%u] All tasks reached barrier, continuing\n", task_id);

    vTaskDelete(NULL);
}

int main(void)
{
    printf("\n=== FreeRTOS Event Group Demo ===\n\n");

    /* 创建事件组 */
    xEventGroup = xEventGroupCreate();
    configASSERT(xEventGroup != NULL);

    /* 创建任务 */
    xTaskCreate(vWiFiTask, "WiFi", 256, NULL, 3, NULL);
    xTaskCreate(vMQTTTask, "MQTT", 256, NULL, 2, NULL);
    xTaskCreate(vSensorTask, "Sensor", 256, NULL, 2, NULL);
    xTaskCreate(vDataProcessingTask, "DataProc", 256, NULL, 1, NULL);
    xTaskCreate(vMonitorTask, "Monitor", 256, NULL, 1, NULL);

    /* 创建同步任务 */
    xTaskCreate(vSyncPointTask, "Sync1", 256, (void*)1, 2, NULL);
    xTaskCreate(vSyncPointTask, "Sync2", 256, (void*)2, 2, NULL);
    xTaskCreate(vSyncPointTask, "Sync3", 256, (void*)3, 2, NULL);

    vTaskStartScheduler();

    return 0;
}
