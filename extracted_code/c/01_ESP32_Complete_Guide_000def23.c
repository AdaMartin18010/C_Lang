/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 2613
 * Language: c
 * Block ID: 000def23
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * inter_task_communication.c
 * ESP32 任务间通信高级示例
 */

#include "freertos/stream_buffer.h"
#include "freertos/message_buffer.h"

/* 流缓冲区 (适合字节流数据) */
static StreamBufferHandle_t xStreamBuffer = NULL;

/* 消息缓冲区 (适合变长消息) */
static MessageBufferHandle_t xMessageBuffer = NULL;

/* 通知值 (轻量级同步) */
static TaskHandle_t xNotifiedTask = NULL;

/**
 * @brief 使用流缓冲区传输数据
 */
static void stream_buffer_example(void)
{
    /* 创建流缓冲区 (大小 256 字节, 触发级别 1) */
    xStreamBuffer = xStreamBufferCreate(256, 1);

    if (xStreamBuffer == NULL) {
        ESP_LOGE(TAG, "流缓冲区创建失败");
        return;
    }

    /* 发送任务 */
    uint8_t txData[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    size_t xBytesSent = xStreamBufferSend(xStreamBuffer,
                                           txData,
                                           sizeof(txData),
                                           portMAX_DELAY);
    ESP_LOGI(TAG, "流缓冲区发送 %d 字节", (int)xBytesSent);

    /* 接收任务 */
    uint8_t rxData[10];
    size_t xReceivedBytes = xStreamBufferReceive(xStreamBuffer,
                                                  rxData,
                                                  sizeof(rxData),
                                                  portMAX_DELAY);
    ESP_LOGI(TAG, "流缓冲区接收 %d 字节", (int)xReceivedBytes);
}

/**
 * @brief 使用任务通知 (最高效)
 */
static void task_notification_example(void)
{
    /* 发送通知 (轻量级, 不经过内核) */
    xTaskNotifyGive(xNotifiedTask);

    /* 或发送带值的通知 */
    uint32_t ulValue = 0x12345678;
    xTaskNotify(xNotifiedTask, ulValue, eSetValueWithOverwrite);
}

/**
 * @brief 接收通知的任务
 */
static void vNotificationReceiver(void *pvParameters)
{
    (void) pvParameters;
    uint32_t ulNotificationValue;

    while (1) {
        /* 等待通知 */
        if (xTaskNotifyWait(0x00,              /* 进入时不清除 */
                           ULONG_MAX,         /* 退出时清除所有 */
                           &ulNotificationValue,
                           portMAX_DELAY) == pdTRUE) {

            ESP_LOGI(TAG, "收到通知值: 0x%08X", (unsigned int)ulNotificationValue);
        }
    }
}
