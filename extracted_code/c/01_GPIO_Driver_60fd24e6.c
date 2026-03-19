/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Physical_Layer\01_GPIO_Driver.md
 * Line: 395
 * Language: c
 * Block ID: 60fd24e6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 软件模拟I2C - GPIO实现
 */

#define I2C_SDA_PIN     GPIO_PIN_7
#define I2C_SCL_PIN     GPIO_PIN_6
#define I2C_GPIO        GPIOB

/* SDA控制宏 */
#define SDA_HIGH()      GPIO_WritePin(I2C_GPIO, I2C_SDA_PIN, 1)
#define SDA_LOW()       GPIO_WritePin(I2C_GPIO, I2C_SDA_PIN, 0)
#define SDA_READ()      GPIO_ReadPin(I2C_GPIO, I2C_SDA_PIN)

/* SCL控制宏 */
#define SCL_HIGH()      GPIO_WritePin(I2C_GPIO, I2C_SCL_PIN, 1)
#define SCL_LOW()       GPIO_WritePin(I2C_GPIO, I2C_SCL_PIN, 0)

void SoftI2C_Init(void)
{
    /* SCL - 开漏输出 */
    GPIO_Config scl_config = {
        .pin = I2C_SCL_PIN,
        .mode = GPIO_MODE_OUTPUT_OD,
        .speed = GPIO_SPEED_HIGH
    };
    GPIO_Init(I2C_GPIO, &scl_config);

    /* SDA - 开漏输出 */
    GPIO_Config sda_config = {
        .pin = I2C_SDA_PIN,
        .mode = GPIO_MODE_OUTPUT_OD,
        .speed = GPIO_SPEED_HIGH
    };
    GPIO_Init(I2C_GPIO, &sda_config);

    SDA_HIGH();
    SCL_HIGH();
}

/* I2C时序 */
void I2C_Delay(void)
{
    for (volatile int i = 0; i < 10; i++);  /* 约5us @ 72MHz */
}

void I2C_Start(void)
{
    SDA_HIGH();
    SCL_HIGH();
    I2C_Delay();
    SDA_LOW();
    I2C_Delay();
    SCL_LOW();
}

void I2C_Stop(void)
{
    SDA_LOW();
    SCL_HIGH();
    I2C_Delay();
    SDA_HIGH();
    I2C_Delay();
}

/* 发送一个字节 */
uint8_t I2C_SendByte(uint8_t byte)
{
    for (int i = 7; i >= 0; i--) {
        if (byte & (1 << i)) {
            SDA_HIGH();
        } else {
            SDA_LOW();
        }
        I2C_Delay();
        SCL_HIGH();
        I2C_Delay();
        SCL_LOW();
    }

    /* 等待ACK */
    SDA_HIGH();  /* 释放SDA */
    I2C_Delay();
    SCL_HIGH();
    I2C_Delay();
    uint8_t ack = SDA_READ();
    SCL_LOW();

    return ack;  /* 0=ACK, 1=NACK */
}
