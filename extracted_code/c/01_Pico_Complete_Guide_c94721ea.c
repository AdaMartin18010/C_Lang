/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 3288
 * Language: c
 * Block ID: c94721ea
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file usb_cdc_example.c
 * @brief USB CDC 虚拟串口示例
 */

#include "pico/stdlib.h"
#include "tusb.h"
#include <stdio.h>

// 在 tusb_config.h 中启用 CDC
// #define CFG_TUD_CDC 1

void cdc_task(void);

int main(void) {
    // 初始化板载 LED
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    // 初始化 TinyUSB
    tusb_init();

    while (1) {
        tud_task();  // 处理 USB 事件
        cdc_task();

        // LED 心跳
        static uint32_t last_blink = 0;
        if (board_millis() - last_blink > 500) {
            last_blink = board_millis();
            gpio_xor_mask(1u << 25);
        }
    }
}

// CDC 接收处理
void cdc_task(void) {
    // 连接状态检查
    if (!tud_cdc_connected()) return;

    // 读取数据
    if (tud_cdc_available()) {
        uint8_t buf[64];
        uint32_t count = tud_cdc_read(buf, sizeof(buf));

        // 回显
        tud_cdc_write(buf, count);
        tud_cdc_write_flush();

        // 处理命令
        for (uint32_t i = 0; i < count; i++) {
            if (buf[i] == '1') {
                gpio_put(25, 1);
            } else if (buf[i] == '0') {
                gpio_put(25, 0);
            }
        }
    }
}

// TinyUSB 回调函数
void tud_cdc_rx_cb(uint8_t itf) {
    (void)itf;
    // 数据接收回调
}

void tud_cdc_tx_complete_cb(uint8_t itf) {
    (void)itf;
    // 发送完成回调
}
