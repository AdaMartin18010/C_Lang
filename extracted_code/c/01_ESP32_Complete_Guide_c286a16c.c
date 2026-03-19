/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 2094
 * Language: c
 * Block ID: c286a16c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * power_management_example.c
 * ESP32 电源管理和低功耗模式示例
 */

#include "esp_pm.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "driver/rtc_io.h"

#define PM_TAG                  "POWER_MGMT"

/**
 * @brief 配置动态频率调节 (DFS)
 *
 * 支持的电源模式:
 * - ESP_PM_CPU_FREQ_MAX: CPU 最大频率, 最高性能
 * - ESP_PM_APB_FREQ_MAX: APB 总线最大频率
 * - ESP_PM_NO_LIGHT_SLEEP: 禁止轻度睡眠
 */
static void configure_power_management(void)
{
    /* 配置电源管理 */
    esp_pm_config_t pm_config = {
        .max_freq_mhz = 240,    /* 最大频率 240MHz */
        .min_freq_mhz = 80,     /* 最小频率 80MHz (轻负载时) */
        .light_sleep_enable = true,  /* 允许轻度睡眠 */
    };

    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
    ESP_LOGI(PM_TAG, "电源管理已配置: %d-%d MHz, Light Sleep: %s",
             pm_config.min_freq_mhz, pm_config.max_freq_mhz,
             pm_config.light_sleep_enable ? "启用" : "禁用");
}

/**
 * @brief 进入轻度睡眠模式
 *
 * 轻度睡眠特点:
 * - CPU 暂停, 但 RAM 保持
 * - 唤醒时间 < 1ms
 * - 可通过 GPIO/定时器/WiFi 等唤醒
 */
static void enter_light_sleep(uint32_t sleep_time_ms)
{
    ESP_LOGI(PM_TAG, "进入轻度睡眠 %d ms", sleep_time_ms);

    /* 获取当前时间 */
    int64_t start_time = esp_timer_get_time();

    /* 进入轻度睡眠 */
    esp_sleep_enable_timer_wakeup(sleep_time_ms * 1000);
    esp_light_sleep_start();

    /* 计算实际睡眠时间 */
    int64_t end_time = esp_timer_get_time();
    int64_t actual_sleep = (end_time - start_time) / 1000;

    /* 获取唤醒原因 */
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    ESP_LOGI(PM_TAG, "从轻度睡眠唤醒");
    ESP_LOGI(PM_TAG, "  计划睡眠: %d ms", sleep_time_ms);
    ESP_LOGI(PM_TAG, "  实际睡眠: %lld ms", actual_sleep);
    ESP_LOGI(PM_TAG, "  唤醒原因: %d", wakeup_reason);
}

/**
 * @brief 深度睡眠配置
 *
 * 深度睡眠特点:
 * - CPU 和大部分外设关闭
 * - 仅 RTC 内存保持 (默认 8KB)
 * - 唤醒后从 reset 向量重启
 * - 电流消耗 < 10uA
 */
static void configure_deep_sleep(void)
{
    /* 深度睡眠唤醒源配置 */

    /* 1. 定时器唤醒 */
    esp_sleep_enable_timer_wakeup(30 * 1000000);  /* 30 秒 */
    ESP_LOGI(PM_TAG, "已启用定时器唤醒: 30 秒");

    /* 2. GPIO 唤醒 (EXT0) - 单引脚 */
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 0));  /* 低电平触发 */
    ESP_LOGI(PM_TAG, "已启用 GPIO33 唤醒 (低电平)");

    /* 3. GPIO 唤醒 (EXT1) - 多引脚 */
    uint64_t ext1_wakeup_pins = (1ULL << GPIO_NUM_34) | (1ULL << GPIO_NUM_35);
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(ext1_wakeup_pins,
                                                  ESP_EXT1_WAKEUP_ANY_HIGH));
    ESP_LOGI(PM_TAG, "已启用 GPIO34/35 唤醒 (任意高电平)");

    /* 4. TouchPad 唤醒 (ESP32 系列支持) */
    /* ESP_ERROR_CHECK(esp_sleep_enable_touchpad_wakeup()); */

    /* 5. ULP 协处理器唤醒 */
    /* ESP_ERROR_CHECK(esp_sleep_enable_ulp_wakeup()); */
}

/**
 * @brief 进入深度睡眠
 */
static void enter_deep_sleep(void)
{
    ESP_LOGI(PM_TAG, "准备进入深度睡眠...");

    /* 保存必要数据到 RTC 慢速内存 (保留) */
    /* RTC_DATA_ATTR static int boot_count = 0; */

    /* 进入深度睡眠 */
    esp_deep_sleep_start();

    /* 不会执行到这里 */
}

/**
 * @brief 处理深度睡眠唤醒
 */
static void handle_deep_sleep_wakeup(void)
{
    /* 获取唤醒原因 */
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:
        ESP_LOGI(PM_TAG, "由 EXT0 (GPIO) 唤醒");
        break;

    case ESP_SLEEP_WAKEUP_EXT1:
        ESP_LOGI(PM_TAG, "由 EXT1 (GPIOs) 唤醒");
        /* 获取唤醒的 GPIO */
        uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
        ESP_LOGI(PM_TAG, "  唤醒引脚掩码: 0x%llx", wakeup_pin_mask);
        break;

    case ESP_SLEEP_WAKEUP_TIMER:
        ESP_LOGI(PM_TAG, "由定时器唤醒");
        break;

    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        ESP_LOGI(PM_TAG, "由 TouchPad 唤醒");
        break;

    case ESP_SLEEP_WAKEUP_ULP:
        ESP_LOGI(PM_TAG, "由 ULP 唤醒");
        break;

    default:
        ESP_LOGI(PM_TAG, "非深度睡眠唤醒 (上电/复位)");
        break;
    }
}

/**
 * @brief RTC GPIO 配置 (深度睡眠中使用)
 */
static void configure_rtc_gpio(void)
{
    /* RTC GPIO 可以在深度睡眠中保持状态 */

    /* 设置 RTC GPIO 为保持模式 */
    rtc_gpio_hold_en(GPIO_NUM_25);   /* 保持 GPIO25 状态 */
    rtc_gpio_hold_en(GPIO_NUM_26);   /* 保持 GPIO26 状态 */

    /* 禁用保持 (如果需要改变状态) */
    /* rtc_gpio_hold_dis(GPIO_NUM_25); */
}

/**
 * @brief 在深度睡眠中保持 WiFi 连接 (ESP32-S3/C3 支持)
 *
 * 注意: 此功能需要配置保持连接的 GPIO
 */
static void maintain_wifi_in_deep_sleep(void)
{
    /* 启用 WiFi 在睡眠中保持连接 (需要 BT/WiFi 共存配置) */
    ESP_ERROR_CHECK(esp_sleep_enable_wifi_wakeup());

    /* 配置 WiFi 唤醒间隔 */
    esp_wifi_set_listen_interval(10);  /* 100ms * 10 = 1s 间隔 */
}
