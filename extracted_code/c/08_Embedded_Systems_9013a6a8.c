/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 845
 * Language: c
 * Block ID: 9013a6a8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// 任务函数
void vSensorTask(void *pvParameters) {
    (void)pvParameters;

    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1) {
        // 读取传感器
        int value = ADC_Read(ADC1, 0);

        // 发送到队列
        xQueueSend(sensor_queue, &value, 0);

        // 精确延时 (100Hz)
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
    }
}

void vDisplayTask(void *pvParameters) {
    (void)pvParameters;
    int value;

    while (1) {
        // 等待数据
        if (xQueueReceive(sensor_queue, &value, portMAX_DELAY)) {
            // 更新显示
            LCD_Printf("Value: %d\n", value);
        }
    }
}

// 队列句柄
QueueHandle_t sensor_queue;

int main(void) {
    hardware_init();

    // 创建队列
    sensor_queue = xQueueCreate(10, sizeof(int));

    // 创建任务
    xTaskCreate(vSensorTask, "Sensor", 256, NULL, 2, NULL);
    xTaskCreate(vDisplayTask, "Display", 256, NULL, 1, NULL);

    // 启动调度器
    vTaskStartScheduler();

    // 不应该到达这里
    while (1);
}
