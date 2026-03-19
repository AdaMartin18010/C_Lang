/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 2380
 * Language: c
 * Block ID: 302e11ce
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 完整项目: 多传感器无线网关
// 硬件: ESP32 + I2C传感器 + SPI Flash
// 功能: 采集→缓存→WiFi上传
// ============================================================

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"

// ========== 配置 ==========
#define SENSOR_I2C_ADDR     0x76    // BME280
#define FLASH_CS_PIN        5
#define WIFI_SSID           "YourSSID"
#define WIFI_PASS           "YourPassword"
#define UPLOAD_URL          "http://api.example.com/data"

// ========== 数据结构 ==========
typedef struct {
    uint32_t timestamp;
    float temperature;
    float humidity;
    float pressure;
    uint16_t battery_mv;
} SensorData;

// 环形缓冲区
typedef struct {
    SensorData data[100];
    volatile uint16_t head;
    volatile uint16_t tail;
} DataBuffer;

static DataBuffer buffer;
static QueueHandle_t data_queue;

// ========== I2C传感器驱动 ==========
bool bme280_read(float *temp, float *humi, float *press) {
    uint8_t data[8];

    // 读取数据寄存器
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SENSOR_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0xF7, true);  // 数据起始寄存器
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SENSOR_I2C_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 8, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) return false;

    // 解析数据 (简化，实际需要补偿计算)
    int32_t adc_p = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
    int32_t adc_t = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
    int32_t adc_h = (data[6] << 8) | data[7];

    *temp = adc_t / 100.0f;
    *humi = adc_h / 1024.0f;
    *press = adc_p / 256.0f;

    return true;
}

// ========== 任务 ==========

// 传感器采集任务
void sensor_task(void *pvParameters) {
    // 初始化I2C
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 21,
        .scl_io_num = 22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);

    while (1) {
        SensorData reading;

        if (bme280_read(&reading.temperature, &reading.humidity, &reading.pressure)) {
            reading.timestamp = time(NULL);
            reading.battery_mv = read_battery();  // ADC读取电池电压

            // 发送到队列
            xQueueSend(data_queue, &reading, portMAX_DELAY);
        }

        vTaskDelay(pdMS_TO_TICKS(5000));  // 5秒采样间隔
    }
}

// 存储与上传任务
void storage_task(void *pvParameters) {
    SensorData data;

    while (1) {
        // 等待数据
        if (xQueueReceive(data_queue, &data, portMAX_DELAY)) {
            // 尝试立即上传
            if (wifi_connected()) {
                if (upload_data(&data)) {
                    continue;  // 上传成功
                }
            }

            // 上传失败，存入Flash缓存
            cache_to_flash(&data);
        }
    }
}

// WiFi连接任务
void wifi_task(void *pvParameters) {
    // WiFi初始化 (同前文)
    // ...

    while (1) {
        if (wifi_connected()) {
            // 尝试上传缓存数据
            flush_cached_data();
        }
        vTaskDelay(pdMS_TO_TICKS(30000));  // 30秒检查一次
    }
}

// ========== 主程序 ==========
void app_main(void) {
    // 初始化NVS
    ESP_ERROR_CHECK(nvs_flash_init());

    // 创建队列
    data_queue = xQueueCreate(20, sizeof(SensorData));

    // 创建任务
    xTaskCreate(sensor_task, "sensor", 4096, NULL, 5, NULL);
    xTaskCreate(storage_task, "storage", 4096, NULL, 4, NULL);
    xTaskCreate(wifi_task, "wifi", 8192, NULL, 3, NULL);
}
