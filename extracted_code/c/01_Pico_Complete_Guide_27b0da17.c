/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 3695
 * Language: c
 * Block ID: 27b0da17
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file usb_composite.c
 * @brief USB 复合设备示例 (CDC + MSC)
 */

#include "pico/stdlib.h"
#include "tusb.h"
#include <stdio.h>

// USB 描述符字符串
static char const* string_desc_arr[] = {
    (const char[]) {0x09, 0x04},  // 语言
    "Raspberry Pi",               // 厂商
    "Pico Composite Device",      // 产品
    "123456",                     // 序列号
    "Pico Serial",                // CDC 接口
    "Pico Storage"                // MSC 接口
};

void cdc_task(void) {
    if (!tud_cdc_connected()) return;

    if (tud_cdc_available()) {
        uint8_t buf[64];
        uint32_t count = tud_cdc_read(buf, sizeof(buf));

        // 处理命令
        if (count > 0) {
            switch (buf[0]) {
                case 'i':
                    tud_cdc_write_str("Pico USB Composite Device\r\n");
                    tud_cdc_write_str("Commands: i=info, r=reset disk\r\n");
                    break;

                case 'r':
                    tud_cdc_write_str("Resetting virtual disk...\r\n");
                    // 重置磁盘逻辑
                    break;

                default:
                    tud_cdc_write(buf, count);
                    break;
            }
            tud_cdc_write_flush();
        }
    }
}

int main(void) {
    stdio_init_all();

    printf("USB Composite Device Starting...\n");

    tusb_init();

    while (1) {
        tud_task();
        cdc_task();
    }
}

// 设备描述符回调
uint8_t const* tud_descriptor_device_cb(void) {
    static tusb_desc_device_t desc_device = {
        .bLength = sizeof(tusb_desc_device_t),
        .bDescriptorType = TUSB_DESC_DEVICE,
        .bcdUSB = 0x0200,
        .bDeviceClass = TUSB_CLASS_MISC,
        .bDeviceSubClass = MISC_SUBCLASS_COMMON,
        .bDeviceProtocol = MISC_PROTOCOL_IAD,
        .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
        .idVendor = 0x2E8A,  // Raspberry Pi
        .idProduct = 0x000A,
        .bcdDevice = 0x0100,
        .iManufacturer = 0x01,
        .iProduct = 0x02,
        .iSerialNumber = 0x03,
        .bNumConfigurations = 0x01
    };
    return (uint8_t const*) &desc_device;
}

// 字符串描述符回调
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;

    static uint16_t desc_str[32];

    if (index == 0) {
        // 语言描述符
        desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * 2 + 2);
        desc_str[1] = 0x0409;  // English
        return desc_str;
    }

    if (index >= sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) {
        return NULL;
    }

    const char* str = string_desc_arr[index];
    uint8_t len = strlen(str);

    desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * len + 2);
    for (uint8_t i = 0; i < len; i++) {
        desc_str[1 + i] = str[i];
    }

    return desc_str;
}
