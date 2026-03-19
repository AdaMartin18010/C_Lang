/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 2282
 * Language: c
 * Block ID: f81250ec
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * wifi_power_save.c
 * ESP32 WiFi 省电模式示例
 */

#include "esp_wifi.h"

/**
 * @brief 配置 WiFi 省电模式
 */
static void wifi_power_save_init(void)
{
    /* 配置 WiFi 为 STA 模式 */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    /* 配置省电模式 */
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);  /* 最小功耗模式 */

    /* 省电模式选项:
     * - WIFI_PS_NONE:        不省电, 全速运行
     * - WIFI_PS_MIN_MODEM:   最小功耗, DTIM 1 (默认)
     * - WIFI_PS_MAX_MODEM:   最大功耗, DTIM 3 或更高
     */

    /* 配置监听间隔 (单位: 100ms) */
    ESP_ERROR_CHECK(esp_wifi_set_listen_interval(10));  /* 1000ms */

    ESP_LOGI(TAG, "WiFi 省电模式已配置");
}

/**
 * @brief 动态调整 WiFi 功耗
 *
 * @param high_throughput true: 高性能模式, false: 省电模式
 */
static void wifi_set_power_mode(bool high_throughput)
{
    if (high_throughput) {
        /* 高性能模式 - 关闭省电 */
        esp_wifi_set_ps(WIFI_PS_NONE);
        ESP_LOGI(TAG, "WiFi 切换到高性能模式");
    } else {
        /* 省电模式 */
        esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
        ESP_LOGI(TAG, "WiFi 切换到最大省电模式");
    }
}
