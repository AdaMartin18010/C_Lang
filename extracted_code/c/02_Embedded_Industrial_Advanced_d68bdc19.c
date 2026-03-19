/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Industrial_Advanced.md
 * Line: 125
 * Language: c
 * Block ID: d68bdc19
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// ESP32-C6: 首款支持WiFi 6的ESP32芯片
// WiFi 6 (802.11ax) 关键特性:
// - OFDMA: 正交频分多址，提高多设备效率
// - MU-MIMO: 多用户多入多出
// - TWT: 目标唤醒时间，超低功耗
// - BSS Coloring: 提高密集环境性能
// ============================================================

#include "esp_wifi.h"
#include "esp_wifi_types.h"

// WiFi 6 特定配置
void wifi6_init(void) {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 配置为802.11ax模式 (WiFi 6)
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "YourWiFi6AP",
            .password = "password",
            .threshold.authmode = WIFI_AUTH_WPA3_PSK,  // WPA3更安全
            // WiFi 6特有的BSS过渡管理
            .btm_enabled = true,   // BSS Transition Management
            .rm_enabled = true,    // Radio Measurement
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // 启用802.11ax模式
    ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_STA,
                                          WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_11AX));

    ESP_ERROR_CHECK(esp_wifi_start());
}

// TWT (Target Wake Time) 配置
// WiFi 6的低功耗关键特性
void wifi6_twt_setup(void) {
    // TWT协议参数
    wifi_twt_config_t twt_config = {
        .flow_id = 0,                    // TWT流ID
        .flow_type = WIFI_TWT_FLOW_TYPE_INDIVIDUAL,  // 个人TWT协议
        .trigger_enabled = true,         // 触发帧启用
        .announce_enabled = false,       // 不提前通知
        .twt_wake_interval = 100,        // 唤醒间隔 (100个单位)
        .twt_wake_interval_unit = WIFI_TWT_WAKE_INTERVAL_UNIT_256US,
        .min_wake_duration = 10,         // 最小唤醒持续时间 (ms)
    };

    // 请求TWT协议
    ESP_ERROR_CHECK(esp_wifi_sta_twt_setup(&twt_config));

    // 现在STA可以在TWT休眠期间深度睡眠，只在指定时间唤醒
}

// 监听TWT事件
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_STA_TWT_SETUP) {
        wifi_event_sta_twt_setup_t *evt = (wifi_event_sta_twt_setup_t *)event_data;
        ESP_LOGI(TAG, "TWT Setup: flow_id=%d, status=%s",
                 evt->flow_id, evt->status ? "success" : "fail");
    }
}
