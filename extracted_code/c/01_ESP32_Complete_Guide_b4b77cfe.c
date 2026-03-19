/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 355
 * Language: c
 * Block ID: b4b77cfe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * main/hello_world_main.c
 * ESP32 Hello World 示例 - 完整的入门项目
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

/* 任务函数声明 */
static void print_chip_info(void);
static void memory_monitor_task(void *pvParameters);

/**
 * @brief 应用程序入口点
 */
void app_main(void)
{
    printf("\n========================================\n");
    printf("    ESP32 Hello World - 系统信息\n");
    printf("========================================\n\n");

    /* 打印芯片信息 */
    print_chip_info();

    /* 创建内存监控任务 */
    xTaskCreate(
        memory_monitor_task,        /* 任务函数 */
        "mem_monitor",              /* 任务名称 */
        2048,                       /* 堆栈大小 (字) */
        NULL,                       /* 任务参数 */
        1,                          /* 优先级 */
        NULL                        /* 任务句柄 */
    );

    /* 主循环 */
    int counter = 0;
    while (1) {
        printf("Hello World! [%d]\n", counter++);
        vTaskDelay(pdMS_TO_TICKS(1000));  /* 延迟 1 秒 */
    }
}

/**
 * @brief 打印 ESP32 芯片详细信息
 */
static void print_chip_info(void)
{
    esp_chip_info_t chip_info;
    uint32_t flash_size;

    /* 获取芯片信息 */
    esp_chip_info(&chip_info);

    printf("芯片型号: %s\n",
        chip_info.model == CHIP_ESP32 ? "ESP32" :
        chip_info.model == CHIP_ESP32S2 ? "ESP32-S2" :
        chip_info.model == CHIP_ESP32S3 ? "ESP32-S3" :
        chip_info.model == CHIP_ESP32C3 ? "ESP32-C3" :
        chip_info.model == CHIP_ESP32C2 ? "ESP32-C2" :
        chip_info.model == CHIP_ESP32C6 ? "ESP32-C6" : "Unknown");

    printf("CPU 核心数: %d\n", chip_info.cores);
    printf("WiFi: %s\n", (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "支持" : "不支持");
    printf("蓝牙: %s\n", (chip_info.features & CHIP_FEATURE_BT) ? "经典蓝牙" :
                         (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "不支持");
    printf("IEEE 802.15.4: %s\n", (chip_info.features & CHIP_FEATURE_IEEE802154) ? "支持" : "不支持");

    /* 获取 Flash 大小 */
    if (esp_flash_get_size(NULL, &flash_size) == ESP_OK) {
        printf("Flash 大小: %ld MB\n", flash_size / (1024 * 1024));
    }

    /* 获取 SDK 版本 */
    printf("ESP-IDF 版本: %s\n", esp_get_idf_version());
    printf("----------------------------------------\n\n");
}

/**
 * @brief 内存监控任务 - 定期报告内存使用情况
 */
static void memory_monitor_task(void *pvParameters)
{
    (void) pvParameters;  /* 未使用参数 */

    while (1) {
        /* 获取内存信息 */
        size_t free_heap = esp_get_free_heap_size();
        size_t min_free_heap = esp_get_minimum_free_heap_size();

        printf("[内存监控] 空闲堆: %d bytes, 最小空闲: %d bytes\n",
               (int)free_heap, (int)min_free_heap);

        /* 每 5 秒报告一次 */
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
