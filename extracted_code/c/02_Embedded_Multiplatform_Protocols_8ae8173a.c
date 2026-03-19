/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 370
 * Language: c
 * Block ID: 8ae8173a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// ESP32默认运行FreeRTOS，双核对称运行
// 需要理解任务亲和性(Core Affinity)
// ============================================================

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 创建绑定到核心0的任务
void task_create_core0(void) {
    xTaskCreatePinnedToCore(
        sensor_task,        // 任务函数
        "sensor",           // 任务名
        4096,               // 栈大小
        NULL,               // 参数
        5,                  // 优先级
        NULL,               // 任务句柄
        0                   // 核心0 (PRO_CPU)
    );
}

// 创建绑定到核心1的任务
void task_create_core1(void) {
    xTaskCreatePinnedToCore(
        wifi_task,          // WiFi任务通常放在核心0
        "wifi",             // 因为WiFi中断在核心0
        8192,
        NULL,
        5,
        NULL,
        1                   // 核心1 (APP_CPU)
    );
}

// 双核任务间通信
#include "freertos/queue.h"

static QueueHandle_t data_queue;

void dual_core_communication(void) {
    // 创建队列
    data_queue = xQueueCreate(10, sizeof(sensor_data_t));

    // 核心0: 传感器采集
    xTaskCreatePinnedToCore(core0_sensor_task, "sensor", 4096, NULL, 5, NULL, 0);

    // 核心1: 数据处理+上传
    xTaskCreatePinnedToCore(core1_process_task, "process", 4096, NULL, 5, NULL, 1);
}

void core0_sensor_task(void *pvParameters) {
    sensor_data_t data;
    while (1) {
        // 读取传感器
        read_sensor(&data);

        // 发送到队列 (不阻塞)
        xQueueSend(data_queue, &data, 0);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void core1_process_task(void *pvParameters) {
    sensor_data_t data;
    while (1) {
        // 接收数据 (阻塞等待)
        if (xQueueReceive(data_queue, &data, portMAX_DELAY)) {
            // 处理并上传
            process_and_upload(&data);
        }
    }
}
