/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Physical_Layer\02_I2C_Driver.md
 * Line: 536
 * Language: c
 * Block ID: 0a39a6b0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * TMP102 数字温度传感器驱动
 * 精度: 0.0625°C
 * 接口: I2C
 */

#define TMP102_ADDR     0x48
#define TMP102_REG_TEMP 0x00
#define TMP102_REG_CONF 0x01

typedef struct {
    float temperature;
    bool valid;
} TMP102_Result;

/**
 * 读取温度
 */
TMP102_Result TMP102_ReadTemperature(I2C_TypeDef *I2Cx)
{
    TMP102_Result result = {0, false};
    uint8_t reg = TMP102_REG_TEMP;
    uint8_t data[2];

    /* 发送寄存器地址 */
    if (I2C_Master_Transmit(I2Cx, TMP102_ADDR, &reg, 1, 100) != I2C_OK) {
        return result;
    }

    /* 读取2字节数据 */
    if (I2C_Master_Receive(I2Cx, TMP102_ADDR, data, 2, 100) != I2C_OK) {
        return result;
    }

    /* 转换温度 */
    int16_t temp_raw = ((int16_t)data[0] << 4) | (data[1] >> 4);
    if (temp_raw & 0x800) {  /* 负数 */
        temp_raw |= 0xF000;
    }

    result.temperature = temp_raw * 0.0625f;
    result.valid = true;

    return result;
}
