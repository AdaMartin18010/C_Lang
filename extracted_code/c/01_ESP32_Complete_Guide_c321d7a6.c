/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 3223
 * Language: c
 * Block ID: c321d7a6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * weather_station.c
 * ESP32 温湿度监测站完整项目
 *
 * 功能:
 * - 读取 SHT30 温湿度传感器
 * - 通过 WiFi 连接到 MQTT 服务器
 * - 每 5 秒上报数据
 * - 本地 LCD 显示
 * - Web 服务器查看实时数据
 * - 支持 OTA 升级
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "mqtt_client.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "esp_ota_ops.h"
#include "esp_timer.h"

/* 项目配置 */
#define PROJECT_NAME        "ESP32 Weather Station"
#define FIRMWARE_VERSION    "1.0.0"

/* WiFi 配置 */
#define WIFI_SSID           "Your_SSID"
#define WIFI_PASS           "Your_Password"

/* MQTT 配置 */
#define MQTT_BROKER         "mqtt://broker.hivemq.com"
#define MQTT_PORT           1883
#define MQTT_TOPIC_TEMP     "esp32/weather/temperature"
#define MQTT_TOPIC_HUM      "esp32/weather/humidity"
#define MQTT_TOPIC_STATUS   "esp32/weather/status"

/* I2C 配置 */
#define I2C_MASTER_NUM      I2C_NUM_0
#define I2C_SDA_PIN         GPIO_NUM_21
#define I2C_SCL_PIN         GPIO_NUM_22
#define I2C_FREQ_HZ         100000

/* 传感器配置 */
#define SHT30_ADDR          0x44

/* 事件组位 */
#define WIFI_CONNECTED_BIT  BIT0
#define MQTT_CONNECTED_BIT  BIT1

/* 日志标签 */
static const char *TAG = "WEATHER_STATION";

/* 全局变量 */
static EventGroupHandle_t s_event_group = NULL;
static esp_mqtt_client_handle_t mqtt_client = NULL;
static float g_temperature = 0.0f;
static float g_humidity = 0.0f;
static uint32_t g_sensor_errors = 0;
static uint32_t g_mqtt_errors = 0;

/* 数据结构 */
typedef struct {
    float temperature;
    float humidity;
    uint32_t timestamp;
    uint32_t error_count;
} sensor_data_t;

static sensor_data_t g_sensor_data = {0};
static SemaphoreHandle_t g_data_mutex = NULL;

/* ==================== 传感器驱动 ==================== */

/**
 * @brief 初始化 I2C 主设备
 */
static esp_err_t sensor_i2c_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN,
        .scl_io_num = I2C_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ_HZ,
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));

    return ESP_OK;
}

/**
 * @brief CRC8 校验计算 (SHT30)
 */
static uint8_t crc8_calculate(const uint8_t *data, size_t len)
{
    const uint8_t POLYNOMIAL = 0x31;
    uint8_t crc = 0xFF;

    for (size_t j = len; j > 0; --j) {
        crc ^= *data++;
        for (int i = 8; i > 0; --i) {
            crc = (crc & 0x80) ? ((crc << 1) ^ POLYNOMIAL) : (crc << 1);
        }
    }
    return crc;
}

/**
 * @brief 读取 SHT30 传感器数据
 */
static esp_err_t sht30_read(float *temperature, float *humidity)
{
    uint8_t cmd[2] = {0x2C, 0x06};  /* 高精度测量 */
    uint8_t data[6];                 /* T_MSB, T_LSB, T_CRC, H_MSB, H_LSB, H_CRC */

    /* 发送测量命令 */
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (SHT30_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd_handle, cmd, 2, true);
    i2c_master_stop(cmd_handle);

    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd_handle,
                                          pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd_handle);

    if (ret != ESP_OK) {
        return ret;
    }

    /* 等待测量完成 (15ms 对于高精度模式) */
    vTaskDelay(pdMS_TO_TICKS(20));

    /* 读取数据 */
    cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (SHT30_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd_handle, data, 5, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd_handle, data + 5, I2C_MASTER_NACK);
    i2c_master_stop(cmd_handle);

    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd_handle,
                               pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd_handle);

    if (ret != ESP_OK) {
        return ret;
    }

    /* CRC 校验 */
    if (crc8_calculate(data, 2) != data[2] ||
        crc8_calculate(data + 3, 2) != data[5]) {
        ESP_LOGE(TAG, "SHT30 CRC 校验失败");
        return ESP_ERR_INVALID_CRC;
    }

    /* 解析温度 */
    uint16_t temp_raw = (data[0] << 8) | data[1];
    *temperature = -45.0f + 175.0f * ((float)temp_raw / 65535.0f);

    /* 解析湿度 */
    uint16_t hum_raw = (data[3] << 8) | data[4];
    *humidity = 100.0f * ((float)hum_raw / 65535.0f);

    /* 限制范围 */
    if (*humidity > 100.0f) *humidity = 100.0f;
    if (*humidity < 0.0f) *humidity = 0.0f;

    return ESP_OK;
}

/* ==================== WiFi 连接 ==================== */

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "WiFi 断开, 5秒后重连...");
        vTaskDelay(pdMS_TO_TICKS(5000));
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "获取 IP: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi 初始化完成");
}

/* ==================== MQTT 客户端 ==================== */

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT 连接成功");
        xEventGroupSetBits(s_event_group, MQTT_CONNECTED_BIT);
        esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_STATUS, "online", 0, 1, 1);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT 断开连接");
        xEventGroupClearBits(s_event_group, MQTT_CONNECTED_BIT);
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGD(TAG, "MQTT 消息已发布");
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT 错误");
        g_mqtt_errors++;
        break;

    default:
        break;
    }
}

static void mqtt_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER,
        .broker.address.port = MQTT_PORT,
        .credentials.client_id = "esp32_weather_station",
        .session.keepalive = 60,
        .session.last_will.topic = MQTT_TOPIC_STATUS,
        .session.last_will.msg = "offline",
        .session.last_will.qos = 1,
        .session.last_will.retain = true,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID,
                                    mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

/* ==================== HTTP Web 服务器 ==================== */

static esp_err_t root_get_handler(httpd_req_t *req)
{
    char resp_str[512];

    xSemaphoreTake(g_data_mutex, portMAX_DELAY);
    snprintf(resp_str, sizeof(resp_str),
        "<!DOCTYPE html>"
        "<html><head><meta charset='UTF-8'><title>ESP32 气象站</title>"
        "<meta http-equiv='refresh' content='5'></head>"
        "<body><h1>ESP32 温湿度监测站</h1>"
        "<p>温度: %.2f °C</p>"
        "<p>湿度: %.2f %%</p>"
        "<p>固件版本: %s</p>"
        "<p>运行时间: %d 秒</p>"
        "<p>传感器错误: %d, MQTT错误: %d</p>"
        "</body></html>",
        g_sensor_data.temperature,
        g_sensor_data.humidity,
        FIRMWARE_VERSION,
        (int)(esp_timer_get_time() / 1000000),
        (int)g_sensor_errors,
        (int)g_mqtt_errors);
    xSemaphoreGive(g_data_mutex);

    httpd_resp_set_type(req, "text/html; charset=utf-8");
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t api_get_handler(httpd_req_t *req)
{
    char resp_str[256];

    xSemaphoreTake(g_data_mutex, portMAX_DELAY);
    snprintf(resp_str, sizeof(resp_str),
        "{\"temperature\":%.2f,\"humidity\":%.2f,\"version\":\"%s\","
        "\"uptime\":%d,\"errors\":%d}",
        g_sensor_data.temperature,
        g_sensor_data.humidity,
        FIRMWARE_VERSION,
        (int)(esp_timer_get_time() / 1000000),
        (int)(g_sensor_errors + g_mqtt_errors));
    xSemaphoreGive(g_data_mutex);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static void start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;

    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_get_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &root_uri);

        httpd_uri_t api_uri = {
            .uri = "/api/data",
            .method = HTTP_GET,
            .handler = api_get_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &api_uri);

        ESP_LOGI(TAG, "Web 服务器启动在端口 %d", config.server_port);
    }
}

/* ==================== 主任务 ==================== */

static void sensor_task(void *pvParameters)
{
    (void)pvParameters;

    while (1) {
        float temp, hum;
        esp_err_t ret = sht30_read(&temp, &hum);

        if (ret == ESP_OK) {
            xSemaphoreTake(g_data_mutex, portMAX_DELAY);
            g_sensor_data.temperature = temp;
            g_sensor_data.humidity = hum;
            g_sensor_data.timestamp = (uint32_t)(esp_timer_get_time() / 1000000);
            xSemaphoreGive(g_data_mutex);

            ESP_LOGI(TAG, "传感器读取: T=%.2f°C, H=%.2f%%", temp, hum);
        } else {
            g_sensor_errors++;
            ESP_LOGE(TAG, "传感器读取失败: %s", esp_err_to_name(ret));
        }

        vTaskDelay(pdMS_TO_TICKS(5000));  /* 5 秒间隔 */
    }
}

static void mqtt_publish_task(void *pvParameters)
{
    (void)pvParameters;
    char payload[64];

    while (1) {
        EventBits_t bits = xEventGroupWaitBits(s_event_group,
                                                MQTT_CONNECTED_BIT,
                                                pdFALSE,
                                                pdTRUE,
                                                pdMS_TO_TICKS(5000));

        if (bits & MQTT_CONNECTED_BIT) {
            xSemaphoreTake(g_data_mutex, portMAX_DELAY);
            float temp = g_sensor_data.temperature;
            float hum = g_sensor_data.humidity;
            xSemaphoreGive(g_data_mutex);

            /* 发布温度 */
            snprintf(payload, sizeof(payload), "%.2f", temp);
            esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_TEMP, payload, 0, 1, 0);

            /* 发布湿度 */
            snprintf(payload, sizeof(payload), "%.2f", hum);
            esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_HUM, payload, 0, 1, 0);

            ESP_LOGI(TAG, "MQTT 数据已发布");
        }

        vTaskDelay(pdMS_TO_TICKS(30000));  /* 30 秒上报间隔 */
    }
}

/* ==================== 主函数 ==================== */

void app_main(void)
{
    /* 初始化 NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* 初始化 */
    ESP_LOGI(TAG, "==============================================");
    ESP_LOGI(TAG, "  %s v%s", PROJECT_NAME, FIRMWARE_VERSION);
    ESP_LOGI(TAG, "==============================================");

    /* 创建资源 */
    s_event_group = xEventGroupCreate();
    g_data_mutex = xSemaphoreCreateMutex();

    /* 初始化硬件 */
    ESP_ERROR_CHECK(sensor_i2c_init());

    /* 初始化网络 */
    wifi_init();

    /* 等待 WiFi 连接 */
    xEventGroupWaitBits(s_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

    /* 启动服务 */
    mqtt_init();
    start_webserver();

    /* 创建任务 */
    xTaskCreate(sensor_task, "sensor", 2048, NULL, 5, NULL);
    xTaskCreate(mqtt_publish_task, "mqtt_pub", 2048, NULL, 4, NULL);

    ESP_LOGI(TAG, "系统启动完成");

    /* 主循环 */
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));

        /* 系统状态报告 */
        ESP_LOGI(TAG, "系统状态 - 堆: %d, 最小堆: %d, 传感器错误: %d, MQTT错误: %d",
                 (int)esp_get_free_heap_size(),
                 (int)esp_get_minimum_free_heap_size(),
                 (int)g_sensor_errors,
                 (int)g_mqtt_errors);
    }
}
