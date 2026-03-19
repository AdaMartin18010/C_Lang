/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\05_Embedded_Firmware.md
 * Line: 188
 * Language: c
 * Block ID: facfa740
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// gpio_driver.h
#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_AF,       // 复用功能
    GPIO_MODE_ANALOG
} gpio_mode_t;

typedef enum {
    GPIO_SPEED_LOW,
    GPIO_SPEED_MEDIUM,
    GPIO_SPEED_HIGH,
    GPIO_SPEED_VERY_HIGH
} gpio_speed_t;

typedef enum {
    GPIO_PULL_NONE,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN
} gpio_pull_t;

typedef struct {
    uint8_t        pin;
    gpio_mode_t    mode;
    gpio_speed_t   speed;
    gpio_pull_t    pull;
    uint8_t        alternate;  // 复用功能号
} gpio_config_t;

// API声明
void gpio_init(uint8_t port, const gpio_config_t* config);
void gpio_write(uint8_t port, uint8_t pin, bool value);
bool gpio_read(uint8_t port, uint8_t pin);
void gpio_toggle(uint8_t port, uint8_t pin);

#endif
