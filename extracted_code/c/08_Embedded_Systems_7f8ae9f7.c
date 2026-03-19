/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 500
 * Language: c
 * Block ID: 7f8ae9f7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// gpio_driver.h
typedef enum {
    GPIO_MODE_INPUT = 0,
    GPIO_MODE_OUTPUT = 1,
    GPIO_MODE_AF = 2,
    GPIO_MODE_ANALOG = 3
} GPIO_Mode;

typedef enum {
    GPIO_SPEED_LOW = 0,
    GPIO_SPEED_MEDIUM = 1,
    GPIO_SPEED_HIGH = 2,
    GPIO_SPEED_VERY_HIGH = 3
} GPIO_Speed;

typedef enum {
    GPIO_NOPULL = 0,
    GPIO_PULLUP = 1,
    GPIO_PULLDOWN = 2
} GPIO_Pull;

typedef struct {
    uint8_t pin;
    GPIO_Mode mode;
    GPIO_Speed speed;
    GPIO_Pull pull;
    uint8_t af;  // 复用功能
} GPIO_Config;

// gpio_driver.c
void GPIO_Init(GPIO_TypeDef *GPIOx, const GPIO_Config *config) {
    uint32_t pin = config->pin;

    // 使能时钟
    if (GPIOx == GPIOA) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    else if (GPIOx == GPIOB) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    // ...

    // 配置模式
    GPIOx->MODER &= ~(3 << (pin * 2));
    GPIOx->MODER |= (config->mode << (pin * 2));

    // 配置输出类型
    GPIOx->OTYPER &= ~(1 << pin);

    // 配置速度
    GPIOx->OSPEEDR &= ~(3 << (pin * 2));
    GPIOx->OSPEEDR |= (config->speed << (pin * 2));

    // 配置上拉/下拉
    GPIOx->PUPDR &= ~(3 << (pin * 2));
    GPIOx->PUPDR |= (config->pull << (pin * 2));

    // 配置复用功能
    if (config->mode == GPIO_MODE_AF) {
        if (pin < 8) {
            GPIOx->AFR[0] &= ~(0xF << (pin * 4));
            GPIOx->AFR[0] |= (config->af << (pin * 4));
        } else {
            GPIOx->AFR[1] &= ~(0xF << ((pin - 8) * 4));
            GPIOx->AFR[1] |= (config->af << ((pin - 8) * 4));
        }
    }
}

void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t value) {
    if (value) {
        GPIOx->BSRR = (1 << pin);      // 置位
    } else {
        GPIOx->BSRR = (1 << (pin + 16)); // 复位
    }
}

uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t pin) {
    return (GPIOx->IDR >> pin) & 1;
}

// 使用示例
void gpio_example(void) {
    // 配置PA5为输出 (LED)
    GPIO_Config led_cfg = {
        .pin = 5,
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_LOW,
        .pull = GPIO_NOPULL
    };
    GPIO_Init(GPIOA, &led_cfg);

    // 闪烁LED
    while (1) {
        GPIO_WritePin(GPIOA, 5, 1);
        delay_ms(500);
        GPIO_WritePin(GPIOA, 5, 0);
        delay_ms(500);
    }
}
