/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\README.md
 * Line: 320
 * Language: c
 * Block ID: b6655c79
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#include "pico/stdlib.h"

#define LED_PIN 25

int main(void) {
    // 初始化标准 I/O
    stdio_init_all();

    // 配置 GPIO
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    printf("Pico 启动成功!\n");

    while (1) {
        gpio_put(LED_PIN, 1);
        printf("LED ON\n");
        sleep_ms(500);

        gpio_put(LED_PIN, 0);
        printf("LED OFF\n");
        sleep_ms(500);
    }

    return 0;
}
