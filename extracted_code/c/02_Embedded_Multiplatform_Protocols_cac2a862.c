/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 2054
 * Language: c
 * Block ID: cac2a862
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// ESP32 WiFi完整示例
// STA模式 + MQTT客户端
// ============================================================

#include "esp_wifi.h"
#include "esp_event.h"
#include "mqtt_client.h"

typedef enum {
    WIFI_DISCONNECTED,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_GOT_IP
} wifi_state_t;

static volatile wifi_state_t wifi_state = WIFI_DISCONNECTED;
static esp_mqtt_client_handle_t mqtt_client = NULL;

// WiFi事件处理
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                wifi_state = WIFI_CONNECTING;
                break;

            case WIFI_EVENT_STA_CONNECTED:
                wifi_state = WIFI_CONNECTED;
                break;

            case WIFI_EVENT_STA_DISCONNECTED:
                wifi_state = WIFI_DISCONNECTED;
                // 5秒后重试
                vTaskDelay(pdMS_TO_TICKS(5000));
                esp_wifi_connect();
                break;

            case WIFI_EVENT_STA_AUTHMODE_CHANGE:
                // 认证模式改变
                break;
        }
    } else if (event_base == IP_EVENT) {
        if (event_id == IP_EVENT_STA_GOT_IP) {
            ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
            ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
            wifi_state = WIFI_GOT_IP;

            // 启动MQTT连接
            mqtt_start();
        }
    }
}

// MQTT事件处理
static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT Connected");
            // 订阅主题
            esp_mqtt_client_subscribe(mqtt_client, "device/cmd", 0);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT Disconnected");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT Subscribed");
            break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT Published");
            break;

        case MQTT_EVENT_DATA:
            // 收到数据
            ESP_LOGI(TAG, "Topic: %.*s", event->topic_len, event->topic);
            ESP_LOGI(TAG, "Data: %.*s", event->data_len, event->data);

            // 解析命令
            process_command(event->data, event->data_len);
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT Error");
            break;
    }
}

void mqtt_start(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://broker.example.com:1883",
        .client_id = "esp32_device_001",
        .username = "device",
        .password = "secret",
        .keepalive = 60,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID,
                                   mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

// 发布传感器数据
void publish_sensor_data(float temperature, float humidity) {
    if (wifi_state != WIFI_GOT_IP || mqtt_client == NULL) return;

    char payload[128];
    snprintf(payload, sizeof(payload),
             "{\"temp\":%.2f,\"humi\":%.2f,\"time\":%lu}",
             temperature, humidity, time(NULL));

    esp_mqtt_client_publish(mqtt_client, "device/data",
                            payload, 0, 0, 0);
}
