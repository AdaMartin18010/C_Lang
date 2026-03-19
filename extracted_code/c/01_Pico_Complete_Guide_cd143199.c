/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 3363
 * Language: c
 * Block ID: cd143199
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file usb_hid_example.c
 * @brief USB HID 键盘和鼠标示例
 */

#include "pico/stdlib.h"
#include "tusb.h"
#include "class/hid/hid_device.h"

// HID 报告描述符
uint8_t const desc_hid_report[] = {
    // 键盘报告
    TUD_HID_REPORT_DESC_KEYBOARD(),

    // 鼠标报告
    TUD_HID_REPORT_DESC_MOUSE()
};

// HID 接口配置
enum {
    ITF_NUM_HID,
    ITF_NUM_TOTAL
};

// 设备描述符
static tusb_desc_device_t const desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = 0xCafe,
    .idProduct = 0x0001,
    .bcdDevice = 0x0100,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01
};

// 键盘按键发送
void hid_keyboard_send_key(uint8_t keycode) {
    uint8_t keycode_array[6] = {0};
    keycode_array[0] = keycode;

    // 按下
    tud_hid_keyboard_report(0, 0, keycode_array);
    sleep_ms(10);

    // 释放
    tud_hid_keyboard_report(0, 0, NULL);
}

// 鼠标移动
void hid_mouse_move(int8_t x, int8_t y, uint8_t buttons) {
    tud_hid_mouse_report(1, buttons, x, y, 0, 0);
}

// 字符串发送
void hid_send_string(const char* str) {
    while (*str) {
        uint8_t keycode = 0;
        uint8_t modifier = 0;

        // 简单的 ASCII 到键码转换
        char c = *str;
        if (c >= 'a' && c <= 'z') {
            keycode = HID_KEY_A + (c - 'a');
        } else if (c >= 'A' && c <= 'Z') {
            keycode = HID_KEY_A + (c - 'A');
            modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
        } else if (c >= '0' && c <= '9') {
            keycode = HID_KEY_0 + (c - '0');
            if (c == '0') keycode = HID_KEY_0;
        } else if (c == ' ') {
            keycode = HID_KEY_SPACE;
        }

        if (keycode != 0) {
            uint8_t keycode_array[6] = {keycode, 0, 0, 0, 0, 0};
            tud_hid_keyboard_report(0, modifier, keycode_array);
            sleep_ms(10);
            tud_hid_keyboard_report(0, 0, NULL);
            sleep_ms(10);
        }

        str++;
    }
}

int main(void) {
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    tusb_init();

    uint32_t count = 0;

    while (1) {
        tud_task();

        // 等待 USB 就绪
        if (!tud_hid_ready()) continue;

        // 每 3 秒发送一个测试字符串
        static uint32_t last_send = 0;
        if (board_millis() - last_send > 3000) {
            last_send = board_millis();

            gpio_put(25, 1);

            // 发送 "Hello from Pico!"
            hid_send_string("Hello from Pico! ");

            // 发送数字计数
            char num_str[16];
            sprintf(num_str, "%lu", count++);
            hid_send_string(num_str);

            // 发送回车
            hid_keyboard_send_key(HID_KEY_RETURN);

            gpio_put(25, 0);
        }
    }
}

// TinyUSB HID 回调
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id,
                                hid_report_type_t report_type,
                                uint8_t* buffer, uint16_t reqlen) {
    (void)itf;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;
    return 0;
}

void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id,
                           hid_report_type_t report_type,
                           uint8_t const* buffer, uint16_t bufsize) {
    (void)itf;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)bufsize;
}
