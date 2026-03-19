/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 1364
 * Language: c
 * Block ID: b5df5d0b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// main.c - 环境监测系统

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// 传感器数据
typedef struct {
    float temperature;
    float humidity;
    float pressure;
    uint32_t timestamp;
} SensorData;

// 全局资源
QueueHandle_t sensor_queue;
QueueHandle_t display_queue;
SemaphoreHandle_t i2c_mutex;

// 任务句柄
TaskHandle_t sensor_task_handle;
TaskHandle_t lora_task_handle;

// 传感器采集任务
void vSensorTask(void *pvParameters) {
    SensorData data;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    // 初始化传感器
    BME280_Init();

    while (1) {
        // 读取传感器
        xSemaphoreTake(i2c_mutex, portMAX_DELAY);
        data.temperature = BME280_ReadTemperature();
        data.humidity = BME280_ReadHumidity();
        data.pressure = BME280_ReadPressure();
        xSemaphoreGive(i2c_mutex);

        data.timestamp = RTC_GetTimestamp();

        // 发送到队列
        xQueueSend(sensor_queue, &data, 0);

        // 100ms周期
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
    }
}

// LoRa传输任务
void vLoRaTask(void *pvParameters) {
    SensorData data;
    uint8_t payload[16];

    // 初始化LoRa
    SX1262_Init();
    LoRaWAN_Join();

    while (1) {
        // 聚合数据 (60秒平均值)
        float temp_sum = 0, hum_sum = 0, press_sum = 0;
        int count = 0;
        uint32_t start_time = RTC_GetTimestamp();

        while (RTC_GetTimestamp() - start_time < 60) {
            if (xQueueReceive(sensor_queue, &data, pdMS_TO_TICKS(1000))) {
                temp_sum += data.temperature;
                hum_sum += data.humidity;
                press_sum += data.pressure;
                count++;
            }
        }

        if (count > 0) {
            // 打包数据
            int16_t temp = (int16_t)(temp_sum / count * 100);
            uint16_t hum = (uint16_t)(hum_sum / count * 100);
            uint16_t press = (uint16_t)((press_sum / count - 90000) / 10);

            payload[0] = temp >> 8;
            payload[1] = temp & 0xFF;
            payload[2] = hum >> 8;
            payload[3] = hum & 0xFF;
            payload[4] = press >> 8;
            payload[5] = press & 0xFF;

            // 发送
            LoRaWAN_Send(payload, 6);
        }

        // 进入低功耗
        enter_low_power();
    }
}

// 显示任务
void vDisplayTask(void *pvParameters) {
    SensorData data;
    char line[32];

    OLED_Init();

    while (1) {
        // 获取最新数据 (非阻塞)
        if (xQueuePeek(display_queue, &data, 0)) {
            // 更新显示
            OLED_Clear();

            snprintf(line, sizeof(line), "Temp: %.1f C", data.temperature);
            OLED_DrawString(0, 0, line);

            snprintf(line, sizeof(line), "Hum:  %.1f %%", data.humidity);
            OLED_DrawString(0, 16, line);

            snprintf(line, sizeof(line), "Pres: %.0f hPa", data.pressure / 100);
            OLED_DrawString(0, 32, line);

            OLED_Update();
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main(void) {
    // 硬件初始化
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();
    I2C_Init();
    SPI_Init();
    RTC_Init();

    // 创建同步原语
    i2c_mutex = xSemaphoreCreateMutex();
    sensor_queue = xQueueCreate(10, sizeof(SensorData));
    display_queue = xQueueCreate(1, sizeof(SensorData));

    // 创建任务
    xTaskCreate(vSensorTask, "Sensor", 256, NULL, 3, &sensor_task_handle);
    xTaskCreate(vLoRaTask, "LoRa", 512, NULL, 2, &lora_task_handle);
    xTaskCreate(vDisplayTask, "Display", 256, NULL, 1, NULL);

    // 启动
    vTaskStartScheduler();

    while (1);
}
