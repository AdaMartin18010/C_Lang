/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Physical_Layer\01_GPIO_Driver.md
 * Line: 47
 * Language: c
 * Block ID: 2e0c289f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * GPIO模式定义
 * 基于STM32风格，适用于大多数ARM MCU
 */

typedef enum {
    GPIO_MODE_INPUT = 0,           /* 浮空输入 */
    GPIO_MODE_INPUT_PU,            /* 上拉输入 */
    GPIO_MODE_INPUT_PD,            /* 下拉输入 */
    GPIO_MODE_OUTPUT_PP,           /* 推挽输出 */
    GPIO_MODE_OUTPUT_OD,           /* 开漏输出 */
    GPIO_MODE_AF_PP,               /* 复用推挽 */
    GPIO_MODE_AF_OD,               /* 复用开漏 */
    GPIO_MODE_ANALOG               /* 模拟模式 */
} GPIO_Mode;

typedef enum {
    GPIO_SPEED_LOW = 0,            /* 2MHz */
    GPIO_SPEED_MEDIUM,             /* 10MHz */
    GPIO_SPEED_HIGH,               /* 50MHz */
    GPIO_SPEED_VERY_HIGH           /* 100MHz+ */
} GPIO_Speed;

typedef struct {
    uint32_t pin;                  /* GPIO引脚号 */
    GPIO_Mode mode;                /* 工作模式 */
    GPIO_Speed speed;              /* 输出速度 */
} GPIO_Config;
