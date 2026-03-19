/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Physical_Layer\02_I2C_Driver.md
 * Line: 333
 * Language: c
 * Block ID: 5c02d464
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 软件模拟I2C实现
 * 适用于没有硬件I2C或引脚冲突的场景
 */

/* 引脚定义 */
#define SOFT_I2C_SDA_PORT   GPIOB
#define SOFT_I2C_SDA_PIN    GPIO_PIN_7
#define SOFT_I2C_SCL_PORT   GPIOB
#define SOFT_I2C_SCL_PIN    GPIO_PIN_6

/* 延时宏 - 约5us @ 72MHz */
#define I2C_DELAY()         { for(volatile int i=0; i<20; i++); }

/* 控制宏 */
#define SDA_HIGH()          GPIO_WritePin(SOFT_I2C_SDA_PORT, SOFT_I2C_SDA_PIN, 1)
#define SDA_LOW()           GPIO_WritePin(SOFT_I2C_SDA_PORT, SOFT_I2C_SDA_PIN, 0)
#define SDA_READ()          GPIO_ReadPin(SOFT_I2C_SDA_PORT, SOFT_I2C_SDA_PIN)
#define SCL_HIGH()          GPIO_WritePin(SOFT_I2C_SCL_PORT, SOFT_I2C_SCL_PIN, 1)
#define SCL_LOW()           GPIO_WritePin(SOFT_I2C_SCL_PORT, SOFT_I2C_SCL_PIN, 0)

void SoftI2C_Init(void)
{
    /* 配置为开漏输出 */
    GPIO_Config config = {
        .mode = GPIO_MODE_OUTPUT_OD,
        .speed = GPIO_SPEED_HIGH
    };

    config.pin = SOFT_I2C_SDA_PIN;
    GPIO_Init(SOFT_I2C_SDA_PORT, &config);

    config.pin = SOFT_I2C_SCL_PIN;
    GPIO_Init(SOFT_I2C_SCL_PORT, &config);

    /* 初始状态高 */
    SDA_HIGH();
    SCL_HIGH();
}

void SoftI2C_Start(void)
{
    SDA_HIGH();
    SCL_HIGH();
    I2C_DELAY();
    SDA_LOW();
    I2C_DELAY();
    SCL_LOW();
}

void SoftI2C_Stop(void)
{
    SDA_LOW();
    SCL_HIGH();
    I2C_DELAY();
    SDA_HIGH();
    I2C_DELAY();
}

void SoftI2C_SendByte(uint8_t byte)
{
    for (int i = 7; i >= 0; i--) {
        SCL_LOW();
        if (byte & (1 << i)) {
            SDA_HIGH();
        } else {
            SDA_LOW();
        }
        I2C_DELAY();
        SCL_HIGH();
        I2C_DELAY();
    }
    SCL_LOW();
}

uint8_t SoftI2C_ReadByte(uint8_t ack)
{
    uint8_t byte = 0;

    SDA_HIGH();  /* 释放SDA */

    for (int i = 7; i >= 0; i--) {
        SCL_LOW();
        I2C_DELAY();
        SCL_HIGH();
        I2C_DELAY();
        if (SDA_READ()) {
            byte |= (1 << i);
        }
    }
    SCL_LOW();

    /* 发送ACK/NACK */
    if (ack) {
        SDA_LOW();
    } else {
        SDA_HIGH();
    }
    I2C_DELAY();
    SCL_HIGH();
    I2C_DELAY();
    SCL_LOW();

    return byte;
}

uint8_t SoftI2C_WaitAck(void)
{
    uint8_t ack;

    SDA_HIGH();
    I2C_DELAY();
    SCL_HIGH();
    I2C_DELAY();
    ack = SDA_READ();
    SCL_LOW();

    return ack;  /* 0=ACK, 1=NACK */
}
