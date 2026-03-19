/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 512
 * Language: c
 * Block ID: 67573cc6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * gpio_basic_example.c
 * ESP32 GPIO 基础操作示例
 * 功能: LED 控制、按键检测、中断处理
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

/* GPIO 引脚定义 - 根据实际硬件修改 */
#define LED_GPIO_PIN        GPIO_NUM_2   /* 板载 LED (大多数开发板) */
#define BUTTON_GPIO_PIN     GPIO_NUM_0   /* 启动按钮 */
#define EXT_LED_PIN         GPIO_NUM_4   /* 外部 LED */

/* 日志标签 */
static const char *TAG = "GPIO_EXAMPLE";

/* 全局变量 */
static volatile uint32_t g_button_press_count = 0;
static QueueHandle_t g_gpio_evt_queue = NULL;

/* 函数声明 */
static void gpio_init_led(gpio_num_t gpio_num);
static void gpio_init_button(gpio_num_t gpio_num);
static void gpio_isr_handler(void *arg);
static void gpio_task_example(void *arg);

/**
 * @brief 初始化 LED GPIO 为输出模式
 */
static void gpio_init_led(gpio_num_t gpio_num)
{
    /* 配置 GPIO */
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << gpio_num),  /* 选择 GPIO */
        .mode = GPIO_MODE_OUTPUT,             /* 设置为输出模式 */
        .pull_up_en = GPIO_PULLUP_DISABLE,    /* 禁用上拉 */
        .pull_down_en = GPIO_PULLDOWN_DISABLE,/* 禁用下拉 */
        .intr_type = GPIO_INTR_DISABLE,       /* 禁用中断 */
    };

    /* 应用配置 */
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    /* 初始状态设为低电平 (LED 关闭) */
    ESP_ERROR_CHECK(gpio_set_level(gpio_num, 0));

    ESP_LOGI(TAG, "GPIO %d 配置为输出模式", gpio_num);
}

/**
 * @brief 初始化按键 GPIO 为输入模式 (带中断)
 */
static void gpio_init_button(gpio_num_t gpio_num)
{
    /* 配置 GPIO */
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << gpio_num),  /* 选择 GPIO */
        .mode = GPIO_MODE_INPUT,              /* 设置为输入模式 */
        .pull_up_en = GPIO_PULLUP_ENABLE,     /* 启用内部上拉 */
        .pull_down_en = GPIO_PULLDOWN_DISABLE,/* 禁用下拉 */
        .intr_type = GPIO_INTR_NEGEDGE,       /* 下降沿触发中断 */
    };

    /* 应用配置 */
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    ESP_LOGI(TAG, "GPIO %d 配置为输入模式 (带中断)", gpio_num);
}

/**
 * @brief GPIO 中断服务程序
 * @note 中断中只做最小操作,通过队列通知任务
 */
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;

    /* 将 GPIO 号发送到队列 (在中断中安全) */
    xQueueSendFromISR(g_gpio_evt_queue, &gpio_num, NULL);
}

/**
 * @brief GPIO 事件处理任务
 */
static void gpio_task_example(void *arg)
{
    uint32_t io_num;
    uint32_t last_press_time = 0;
    const uint32_t debounce_ms = 200;  /* 消抖时间 200ms */

    while (1) {
        /* 等待队列消息 */
        if (xQueueReceive(g_gpio_evt_queue, &io_num, portMAX_DELAY)) {
            uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;

            /* 简单软件消抖 */
            if ((current_time - last_press_time) > debounce_ms) {
                g_button_press_count++;
                last_press_time = current_time;

                ESP_LOGI(TAG, "GPIO[%d] 中断触发, 按下次数: %d",
                         (int)io_num, (int)g_button_press_count);

                /* 切换 LED 状态 */
                static int led_state = 0;
                led_state = !led_state;
                gpio_set_level(LED_GPIO_PIN, led_state);
                gpio_set_level(EXT_LED_PIN, led_state);
            }
        }
    }
}

/**
 * @brief LED 闪烁任务
 */
static void led_blink_task(void *pvParameters)
{
    gpio_num_t led_pin = (gpio_num_t)(uintptr_t)pvParameters;
    int blink_count = 0;

    ESP_LOGI(TAG, "LED 闪烁任务启动, GPIO: %d", (int)led_pin);

    while (1) {
        /* LED 亮 */
        gpio_set_level(led_pin, 1);
        vTaskDelay(pdMS_TO_TICKS(500));

        /* LED 灭 */
        gpio_set_level(led_pin, 0);
        vTaskDelay(pdMS_TO_TICKS(500));

        blink_count++;
        if (blink_count % 10 == 0) {
            ESP_LOGI(TAG, "LED 已闪烁 %d 次", blink_count);
        }
    }
}

/**
 * @brief 主函数
 */
void app_main(void)
{
    ESP_LOGI(TAG, "ESP32 GPIO 基础示例启动");

    /* 创建队列用于 GPIO 事件 */
    g_gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    if (g_gpio_evt_queue == NULL) {
        ESP_LOGE(TAG, "创建队列失败!");
        return;
    }

    /* 初始化 GPIO */
    gpio_init_led(LED_GPIO_PIN);
    gpio_init_led(EXT_LED_PIN);
    gpio_init_button(BUTTON_GPIO_PIN);

    /* 安装 GPIO 中断服务 */
    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    /* 添加中断处理程序 */
    ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_GPIO_PIN, gpio_isr_handler,
                                          (void *)BUTTON_GPIO_PIN));

    /* 创建 GPIO 事件处理任务 */
    xTaskCreate(gpio_task_example, "gpio_task", 2048, NULL, 10, NULL);

    /* 创建 LED 闪烁任务 */
    xTaskCreate(led_blink_task, "led_blink", 1024,
                (void *)(uintptr_t)EXT_LED_PIN, 5, NULL);

    ESP_LOGI(TAG, "系统初始化完成,等待事件...");
    ESP_LOGI(TAG, "按 GPIO%d 按键切换 LED 状态", (int)BUTTON_GPIO_PIN);
}
