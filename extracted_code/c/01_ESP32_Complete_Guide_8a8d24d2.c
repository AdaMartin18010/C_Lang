/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 1123
 * Language: c
 * Block ID: 8a8d24d2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * wifi_ap_example.c
 * ESP32 WiFi AP (热点) 模式示例
 */

#include "esp_wifi.h"

#define AP_SSID             "ESP32_AP"
#define AP_PASS             "12345678"
#define AP_MAX_CONNECTIONS  4
#define AP_CHANNEL          6

static const char *TAG_AP = "WIFI_AP";

/**
 * @brief 初始化 WiFi AP 模式
 */
static void wifi_init_softap(void)
{
    /* 初始化网络接口 */
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* 创建 AP 接口 */
    esp_netif_create_default_wifi_ap();

    /* 初始化 WiFi */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* 配置 AP */
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = AP_SSID,
            .ssid_len = strlen(AP_SSID),
            .channel = AP_CHANNEL,
            .password = AP_PASS,
            .max_connection = AP_MAX_CONNECTIONS,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .pmf_cfg = {
                .required = true,
            },
        },
    };

    if (strlen(AP_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    /* 设置模式并启动 AP */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    /* 打印 AP 信息 */
    ESP_LOGI(TAG_AP, "WiFi AP 初始化完成");
    ESP_LOGI(TAG_AP, "SSID: %s", AP_SSID);
    ESP_LOGI(TAG_AP, "密码: %s", AP_PASS);
    ESP_LOGI(TAG_AP, "Channel: %d", AP_CHANNEL);
    ESP_LOGI(TAG_AP, "最大连接数: %d", AP_MAX_CONNECTIONS);

    /* 获取并打印 IP 地址 */
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);
    ESP_LOGI(TAG_AP, "AP IP: " IPSTR, IP2STR(&ip_info.ip));
}

/**
 * @brief 获取已连接设备列表
 */
static void print_connected_stations(void)
{
    wifi_sta_list_t sta_list;

    if (esp_wifi_ap_get_sta_list(&sta_list) == ESP_OK) {
        ESP_LOGI(TAG_AP, "已连接设备数: %d", sta_list.num);

        for (int i = 0; i < sta_list.num; i++) {
            ESP_LOGI(TAG_AP, "  [%d] MAC: %02x:%02x:%02x:%02x:%02x:%02x, RSSI: %d",
                     i + 1,
                     sta_list.sta[i].mac[0], sta_list.sta[i].mac[1],
                     sta_list.sta[i].mac[2], sta_list.sta[i].mac[3],
                     sta_list.sta[i].mac[4], sta_list.sta[i].mac[5],
                     sta_list.sta[i].rssi);
        }
    }
}
