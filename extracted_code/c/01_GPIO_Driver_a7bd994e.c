/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Physical_Layer\01_GPIO_Driver.md
 * Line: 116
 * Language: c
 * Block ID: a7bd994e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * GPIO初始化
 */
void GPIO_Init(GPIO_TypeDef *GPIOx, const GPIO_Config *config)
{
    uint32_t pin = config->pin;
    uint32_t pin_number = 0;

    /* 计算引脚号 */
    while ((pin >> pin_number) != 1) {
        pin_number++;
    }

    /* 配置模式 */
    uint32_t mode = (uint32_t)config->mode;
    GPIOx->MODER &= ~(3U << (pin_number * 2));  /* 清除 */
    GPIOx->MODER |= (mode & 3U) << (pin_number * 2);  /* 设置 */

    /* 配置输出类型 */
    if (mode == GPIO_MODE_OUTPUT_OD || mode == GPIO_MODE_AF_OD) {
        GPIOx->OTYPER |= pin;   /* 开漏 */
    } else {
        GPIOx->OTYPER &= ~pin;  /* 推挽 */
    }

    /* 配置速度 */
    GPIOx->OSPEEDR &= ~(3U << (pin_number * 2));
    GPIOx->OSPEEDR |= ((uint32_t)config->speed << (pin_number * 2));

    /* 配置上拉/下拉 */
    uint32_t pupd = 0;
    if (mode == GPIO_MODE_INPUT_PU) pupd = 1;
    else if (mode == GPIO_MODE_INPUT_PD) pupd = 2;

    GPIOx->PUPDR &= ~(3U << (pin_number * 2));
    GPIOx->PUPDR |= (pupd << (pin_number * 2));
}

/**
 * 设置引脚输出
 */
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint32_t pin, uint8_t value)
{
    if (value) {
        GPIOx->BSRR = pin;           /* 设置 */
    } else {
        GPIOx->BSRR = pin << 16;     /* 清除 */
    }
}

/**
 * 读取引脚输入
 */
uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint32_t pin)
{
    return (GPIOx->IDR & pin) ? 1 : 0;
}

/**
 * 翻转引脚输出
 */
void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint32_t pin)
{
    GPIOx->ODR ^= pin;
}
