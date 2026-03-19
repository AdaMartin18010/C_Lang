/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 3794
 * Language: c
 * Block ID: 7677b1aa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * debug_config.c
 * ESP32 调试配置和技巧
 */

#include "esp_debug_helpers.h"
#include "esp_log.h"

/**
 * @brief 配置日志级别
 */
static void configure_logging(void)
{
    /* 设置全局日志级别 */
    esp_log_level_set("*", ESP_LOG_INFO);

    /* 为特定组件设置日志级别 */
    esp_log_level_set("wifi", ESP_LOG_WARN);       /* 只显示警告及以上 */
    esp_log_level_set("mqtt", ESP_LOG_DEBUG);      /* 显示所有日志 */
    esp_log_level_set("WEATHER_STATION", ESP_LOG_VERBOSE);
}

/**
 * @brief 打印堆栈回溯
 */
static void print_backtrace(void)
{
    ESP_LOGI(TAG, "===== 堆栈回溯 =====");
    esp_backtrace_print(100);  /* 最多 100 层 */
}

/**
 * @brief 断言处理
 */
void __assert_func(const char *file, int line, const char *func, const char *expr)
{
    ESP_LOGE(TAG, "断言失败: %s:%d (%s): %s", file, line, func, expr);
    print_backtrace();

    /* 可选: 重启或进入调试循环 */
    abort();
}

/**
 * @brief 内存检查
 */
static void check_memory(void)
{
    /* 堆信息 */
    ESP_LOGI(TAG, "堆信息:");
    ESP_LOGI(TAG, "  总空闲: %d", (int)esp_get_free_heap_size());
    ESP_LOGI(TAG, "  最小空闲: %d", (int)esp_get_minimum_free_heap_size());
    ESP_LOGI(TAG, "  最大块: %d", (int)heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));

    /* 内部 RAM */
    ESP_LOGI(TAG, "内部 DRAM:");
    ESP_LOGI(TAG, "  空闲: %d",
             (int)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));

    /* SPIRAM (如可用) */
    if (esp_psram_is_initialized()) {
        ESP_LOGI(TAG, "外部 PSRAM:");
        ESP_LOGI(TAG, "  总大小: %d", (int)esp_psram_get_size());
        ESP_LOGI(TAG, "  空闲: %d",
                 (int)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    }
}
