/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Physical_Layer\02_I2C_Driver.md
 * Line: 118
 * Language: c
 * Block ID: e1189074
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef enum {
    I2C_SPEED_STANDARD = 100000,   /* 100kHz */
    I2C_SPEED_FAST = 400000        /* 400kHz */
} I2C_Speed;

typedef struct {
    I2C_Speed speed;
    uint32_t own_address;          /* 自身地址 (从模式) */
    uint8_t duty_cycle;            /* 快速模式占空比 */
} I2C_Config;

/**
 * I2C初始化
 */
void I2C_Init(I2C_TypeDef *I2Cx, const I2C_Config *config, uint32_t pclk1)
{
    /* 禁用I2C */
    I2Cx->CR1 &= ~I2C_CR1_PE;

    /* 配置时钟 */
    uint32_t freq_range = pclk1 / 1000000;  /* MHz */
    I2Cx->CR2 = freq_range;

    /* 配置CCR */
    uint32_t ccr;
    if (config->speed == I2C_SPEED_STANDARD) {
        /* 标准模式: CCR = Fpclk1 / (2 * Fi2c) */
        ccr = pclk1 / (2 * I2C_SPEED_STANDARD);
        if (ccr < 4) ccr = 4;
    } else {
        /* 快速模式 */
        if (config->duty_cycle == 0) {
            /* 占空比 2:1 */
            ccr = pclk1 / (3 * I2C_SPEED_FAST);
        } else {
            /* 占空比 16:9 */
            ccr = pclk1 / (25 * I2C_SPEED_FAST);
            ccr |= (1U << 15);  /* 设置占空比位 */
        }
        ccr |= (1U << 15);  /* 快速模式 */
    }
    I2Cx->CCR = ccr;

    /* 配置最大上升时间 */
    if (config->speed == I2C_SPEED_STANDARD) {
        I2Cx->TRISE = freq_range + 1;  /* 标准模式 */
    } else {
        I2Cx->TRISE = (freq_range * 300 / 1000) + 1;  /* 快速模式300ns */
    }

    /* 配置自身地址 (从模式) */
    if (config->own_address != 0) {
        I2Cx->OAR1 = (config->own_address << 1) | (1U << 14);  /* 7位地址 */
    }

    /* 使能I2C */
    I2Cx->CR1 |= I2C_CR1_PE;
}
