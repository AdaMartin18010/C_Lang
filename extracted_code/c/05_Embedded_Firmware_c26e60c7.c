/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\05_Embedded_Firmware.md
 * Line: 233
 * Language: c
 * Block ID: c26e60c7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// gpio_driver.c
#include "gpio_driver.h"

// GPIO寄存器基地址
static GPIO_TypeDef* const GPIO_PORTS[] = {
    GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI
};

void gpio_init(uint8_t port, const gpio_config_t* cfg) {
    GPIO_TypeDef* GPIOx = GPIO_PORTS[port];
    uint8_t pin = cfg->pin;

    // 1. 使能GPIO时钟
    RCC->AHB1ENR |= (1 << port);

    // 2. 配置模式
    GPIOx->MODER &= ~(3 << (pin * 2));
    GPIOx->MODER |= (cfg->mode << (pin * 2));

    // 3. 配置输出速度
    GPIOx->OSPEEDR &= ~(3 << (pin * 2));
    GPIOx->OSPEEDR |= (cfg->speed << (pin * 2));

    // 4. 配置上拉/下拉
    GPIOx->PUPDR &= ~(3 << (pin * 2));
    GPIOx->PUPDR |= (cfg->pull << (pin * 2));

    // 5. 配置复用功能(如果需要)
    if (cfg->mode == GPIO_MODE_AF) {
        if (pin < 8) {
            GPIOx->AFR[0] &= ~(0xF << (pin * 4));
            GPIOx->AFR[0] |= (cfg->alternate << (pin * 4));
        } else {
            GPIOx->AFR[1] &= ~(0xF << ((pin - 8) * 4));
            GPIOx->AFR[1] |= (cfg->alternate << ((pin - 8) * 4));
        }
    }
}

void gpio_write(uint8_t port, uint8_t pin, bool value) {
    GPIO_TypeDef* GPIOx = GPIO_PORTS[port];
    if (value) {
        GPIOx->BSRR = (1 << pin);
    } else {
        GPIOx->BSRR = (1 << (pin + 16));
    }
}

bool gpio_read(uint8_t port, uint8_t pin) {
    GPIO_TypeDef* GPIOx = GPIO_PORTS[port];
    return (GPIOx->IDR & (1 << pin)) != 0;
}

void gpio_toggle(uint8_t port, uint8_t pin) {
    GPIO_TypeDef* GPIOx = GPIO_PORTS[port];
    GPIOx->ODR ^= (1 << pin);
}
