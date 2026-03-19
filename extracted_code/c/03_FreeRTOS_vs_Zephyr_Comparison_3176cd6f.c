/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 2222
 * Language: c
 * Block ID: 3176cd6f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * FreeRTOS 队列完整示例
 * 文件: freertos_queue_example.c
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>

/* 消息类型定义 */
typedef enum {
    MSG_TYPE_SENSOR_DATA = 0,
    MSG_TYPE_CONTROL_CMD,
    MSG_TYPE_STATUS_REQ,
    MSG_TYPE_CONFIG_UPDATE,
    MSG_TYPE_MAX
} MessageType_t;

/* 传感器数据结构 */
typedef struct {
    uint32_t timestamp;
    float temperature;
    float humidity;
    float pressure;
    uint32_t sequence_num;
} SensorData_t;

/* 控制命令结构 */
typedef struct {
    uint32_t command_id;
    uint32_t param1;
    uint32_t param2;
    char description[32];
} ControlCommand_t;

/* 通用消息结构 */
typedef struct {
    MessageType_t type;
    union {
        SensorData_t sensor;
        ControlCommand_t command;
        uint32_t raw_data[16];
    } payload;
} Message_t;

/* 队列句柄 */
static QueueHandle_t xSensorQueue = NULL;
static QueueHandle_t xCommandQueue = NULL;
static QueueHandle_t xPriorityQueue = NULL;

/* 统计信息 */
static volatile uint32_t messages_sent = 0;
static volatile uint32_t messages_received = 0;
static volatile uint32_t queue_full_count = 0;

/*
 * 传感器数据采集任务
 * 功能: 采集传感器数据并发送到队列
 */
static void vSensorTask(void *pvParameters)
{
    (void)pvParameters;
    SensorData_t sensor_data;
    uint32_t sequence = 0;

    printf("[SensorTask] Started\n");

    for (;;) {
        /* 模拟传感器读取 */
        sequence++;
        sensor_data.timestamp = xTaskGetTickCount();
        sensor_data.temperature = 25.0f + (float)(sequence % 10);
        sensor_data.humidity = 50.0f + (float)(sequence % 20);
        sensor_data.pressure = 1013.25f + (float)(sequence % 5);
        sensor_data.sequence_num = sequence;

        /* 创建消息 */
        Message_t msg;
        msg.type = MSG_TYPE_SENSOR_DATA;
        memcpy(&msg.payload.sensor, &sensor_data, sizeof(SensorData_t));

        /* 发送到队列 - 带超时 */
        if (xQueueSend(xSensorQueue, &msg, pdMS_TO_TICKS(100)) == pdPASS) {
            messages_sent++;

            if (sequence % 10 == 0) {
                printf("[SensorTask] Sent sensor data #%lu (T=%.1f, H=%.1f)\n",
                       sequence, sensor_data.temperature, sensor_data.humidity);
            }
        } else {
            queue_full_count++;
            printf("[SensorTask] Queue full, data dropped\n");
        }

        /* 采集周期 */
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/*
 * 命令处理任务
 * 功能: 从队列接收并执行命令
 */
static void vCommandTask(void *pvParameters)
{
    (void)pvParameters;
    Message_t msg;

    printf("[CommandTask] Started\n");

    for (;;) {
        /* 接收命令 - 永久等待 */
        if (xQueueReceive(xCommandQueue, &msg, portMAX_DELAY) == pdPASS) {
            messages_received++;

            if (msg.type == MSG_TYPE_CONTROL_CMD) {
                ControlCommand_t *cmd = &msg.payload.command;
                printf("[CommandTask] Executing command %lu: %s\n",
                       cmd->command_id, cmd->description);
                printf("  Parameters: %lu, %lu\n", cmd->param1, cmd->param2);

                /* 模拟命令执行 */
                vTaskDelay(pdMS_TO_TICKS(50));

                printf("[CommandTask] Command %lu completed\n", cmd->command_id);
            }
        }
    }
}

/*
 * 数据处理任务
 * 功能: 从多个队列接收数据（使用队列集）
 */
static QueueSetHandle_t xQueueSet = NULL;

static void vDataProcessingTask(void *pvParameters)
{
    (void)pvParameters;
    QueueSetMemberHandle_t xActivatedMember;
    Message_t msg;

    printf("[DataProcTask] Started\n");

    for (;;) {
        /* 等待任一队列有数据 */
        xActivatedMember = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);

        if (xActivatedMember == (QueueSetMemberHandle_t)xSensorQueue) {
            /* 从传感器队列接收 */
            if (xQueueReceive(xSensorQueue, &msg, 0) == pdPASS) {
                messages_received++;

                /* 处理传感器数据 */
                float avg_temp = msg.payload.sensor.temperature;
                printf("[DataProcTask] Processing sensor data #%lu (avg_temp=%.2f)\n",
                       msg.payload.sensor.sequence_num, avg_temp);

                /* 温度告警检查 */
                if (avg_temp > 30.0f) {
                    printf("[DataProcTask] WARNING: High temperature detected!\n");
                }
            }
        }
        else if (xActivatedMember == (QueueSetMemberHandle_t)xCommandQueue) {
            /* 命令转发到专用处理任务 */
            if (xQueueReceive(xCommandQueue, &msg, 0) == pdPASS) {
                messages_received++;
                printf("[DataProcTask] Forwarding command to handler\n");
                /* 实际应用中可能转发到工作队列 */
            }
        }
    }
}

/*
 * 优先级命令任务
 * 功能: 发送高优先级命令（覆盖队列）
 */
static void vPriorityCommandTask(void *pvParameters)
{
    (void)pvParameters;
    uint32_t cmd_id = 0;

    printf("[PriorityTask] Started\n");

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(5000));

        /* 创建紧急命令 */
        Message_t msg;
        msg.type = MSG_TYPE_CONTROL_CMD;
        msg.payload.command.command_id = ++cmd_id;
        msg.payload.command.param1 = 0xFF;
        msg.payload.command.param2 = 0xFF;
        snprintf(msg.payload.command.description, 32, "EMERGENCY_STOP");

        /* 覆盖发送到优先级队列 */
        if (xQueueOverwrite(xPriorityQueue, &msg) == pdPASS) {
            printf("[PriorityTask] Emergency command %lu sent (overwritten)\n", cmd_id);
        }
    }
}

/*
 * 队列监控任务
 */
static void vQueueMonitorTask(void *pvParameters)
{
    (void)pvParameters;
    UBaseType_t uxMessagesWaiting;

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(5000));

        printf("\n[QueueMonitor] Statistics:\n");
        printf("  Messages sent: %lu\n", messages_sent);
        printf("  Messages received: %lu\n", messages_received);
        printf("  Queue full events: %lu\n", queue_full_count);

        uxMessagesWaiting = uxQueueMessagesWaiting(xSensorQueue);
        printf("  Sensor queue messages: %u\n", uxMessagesWaiting);

        uxMessagesWaiting = uxQueueMessagesWaiting(xCommandQueue);
        printf("  Command queue messages: %u\n", uxMessagesWaiting);

        UBaseType_t uxSpaces = uxQueueSpacesAvailable(xSensorQueue);
        printf("  Sensor queue spaces: %u\n", uxSpaces);
    }
}

/*
 * 使用队列查看功能
 */
static void vQueuePeekTask(void *pvParameters)
{
    (void)pvParameters;
    Message_t msg;

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));

        /* 查看队列头部而不移除 */
        if (xQueuePeek(xSensorQueue, &msg, pdMS_TO_TICKS(100)) == pdPASS) {
            printf("[PeekTask] Next sensor data: seq=%lu, temp=%.1f\n",
                   msg.payload.sensor.sequence_num,
                   msg.payload.sensor.temperature);
        }
    }
}

/*
 * 中断中发送数据
 */
void vApplicationTickHook(void)
{
    static uint32_t tick_count = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    tick_count++;

    if (tick_count >= 1000) {
        tick_count = 0;

        Message_t msg;
        msg.type = MSG_TYPE_STATUS_REQ;
        msg.payload.raw_data[0] = xTaskGetTickCount();

        xQueueSendFromISR(xPriorityQueue, &msg, &xHigherPriorityTaskWoken);

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/*
 * 静态队列分配
 */
#define QUEUE_LENGTH    10
#define ITEM_SIZE       sizeof(Message_t)

static uint8_t ucQueueStorage[QUEUE_LENGTH * ITEM_SIZE];
static StaticQueue_t xStaticQueue;

static QueueHandle_t xStaticQueueHandle = NULL;

int main(void)
{
    printf("\n=== FreeRTOS Queue Demo ===\n\n");

    /* 创建传感器队列 - 深度20 */
    xSensorQueue = xQueueCreate(20, sizeof(Message_t));
    configASSERT(xSensorQueue != NULL);

    /* 创建命令队列 */
    xCommandQueue = xQueueCreate(10, sizeof(Message_t));
    configASSERT(xCommandQueue != NULL);

    /* 创建优先级队列（长度为1，用于覆盖） */
    xPriorityQueue = xQueueCreate(1, sizeof(Message_t));
    configASSERT(xPriorityQueue != NULL);

    /* 创建队列集 */
    xQueueSet = xQueueCreateSet(30);
    configASSERT(xQueueSet != NULL);

    /* 添加队列到队列集 */
    xQueueAddToSet(xSensorQueue, xQueueSet);
    xQueueAddToSet(xCommandQueue, xQueueSet);

    /* 创建静态队列 */
    xStaticQueueHandle = xQueueCreateStatic(
        QUEUE_LENGTH,
        ITEM_SIZE,
        ucQueueStorage,
        &xStaticQueue
    );
    configASSERT(xStaticQueueHandle != NULL);

    /* 创建任务 */
    xTaskCreate(vSensorTask, "Sensor", 512, NULL, 2, NULL);
    xTaskCreate(vCommandTask, "Command", 512, NULL, 2, NULL);
    xTaskCreate(vDataProcessingTask, "DataProc", 512, NULL, 3, NULL);
    xTaskCreate(vPriorityCommandTask, "Priority", 256, NULL, 4, NULL);
    xTaskCreate(vQueueMonitorTask, "Monitor", 256, NULL, 1, NULL);
    xTaskCreate(vQueuePeekTask, "Peek", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    return 0;
}
