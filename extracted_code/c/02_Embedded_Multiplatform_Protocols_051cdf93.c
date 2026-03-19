/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 308
 * Language: c
 * Block ID: 051cdf93
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// ESP32 GPIO架构特殊性
// ESP32有34个GPIO，通过GPIO矩阵可以灵活映射外设到任意引脚
// ============================================================

#include "driver/gpio.h"
#include "esp_intr_alloc.h"

// ESP32 GPIO配置 (比STM32更灵活)
void esp32_gpio_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_18),  // GPIO18
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    // 设置输出
    gpio_set_level(GPIO_NUM_18, 1);
}

// ESP32特有的GPIO矩阵 - 任意外设可映射到任意GPIO
// 这是ESP32最大的优势之一！
void esp32_gpio_matrix(void) {
    // UART0默认使用GPIO1(TX)和GPIO3(RX)
    // 但可以通过矩阵重新映射到任意GPIO

    // 将UART0 TX映射到GPIO18
    gpio_matrix_out(GPIO_NUM_18, U0TXD_OUT_IDX, false, false);

    // 将UART0 RX映射到GPIO19
    gpio_matrix_in(GPIO_NUM_19, U0RXD_IN_IDX, false);
}

// ESP32中断矩阵 - 灵活的中断路由
void esp32_interrupt_setup(void) {
    // GPIO中断
    gpio_set_intr_type(GPIO_NUM_4, GPIO_INTR_POSEDGE);  // 上升沿触发
    gpio_intr_enable(GPIO_NUM_4);

    // 注册中断处理 (ESP32使用FreeRTOS中断API)
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    gpio_isr_handler_add(GPIO_NUM_4, gpio_isr_handler, (void*)4);
}

// ISR处理 (ESP32要求IRAM_ATTR，中断处理放在IRAM)
static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t)arg;
    // 中断处理必须在IRAM中，不能访问Flash
    // 通常设置标志，在主任务中处理
    gpio_intr_disable(gpio_num);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // 通知任务...
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
