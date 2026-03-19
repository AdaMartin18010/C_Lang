/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 1574
 * Language: c
 * Block ID: bebb831e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file i2c_example.c
 * @brief I2C 通信示例 - BME280 传感器
 */

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>

// I2C 配置
#define I2C_PORT        i2c0
#define I2C_SDA_PIN     4
#define I2C_SCL_PIN     5
#define I2C_BAUDRATE    400000  // 400kHz

// BME280 地址
#define BME280_ADDR     0x76

// BME280 寄存器
#define BME280_REG_ID           0xD0
#define BME280_REG_RESET        0xE0
#define BME280_REG_CTRL_HUM     0xF2
#define BME280_REG_STATUS       0xF3
#define BME280_REG_CTRL_MEAS    0xF4
#define BME280_REG_CONFIG       0xF5
#define BME280_REG_PRESS_MSB    0xF7
#define BME280_REG_CALIB        0x88

// BME280 校准数据
typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
} bme280_calib_data_t;

bme280_calib_data_t calib_data;
int32_t t_fine;

void i2c_init_custom(void) {
    // 初始化 I2C
    i2c_init(I2C_PORT, I2C_BAUDRATE);

    // 配置 GPIO
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
}

int i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t data) {
    uint8_t buf[2] = {reg, data};
    return i2c_write_blocking(I2C_PORT, addr, buf, 2, false);
}

int i2c_read_reg(uint8_t addr, uint8_t reg, uint8_t* data, size_t len) {
    i2c_write_blocking(I2C_PORT, addr, &reg, 1, true);
    return i2c_read_blocking(I2C_PORT, addr, data, len, false);
}

bool bme280_init(void) {
    // 检查设备 ID
    uint8_t id;
    i2c_read_reg(BME280_ADDR, BME280_REG_ID, &id, 1);

    if (id != 0x60) {
        printf("BME280 未找到! ID=0x%02X\n", id);
        return false;
    }

    printf("BME280 检测到!\n");

    // 软复位
    i2c_write_reg(BME280_ADDR, BME280_REG_RESET, 0xB6);
    sleep_ms(10);

    // 读取校准数据
    uint8_t calib[26];
    i2c_read_reg(BME280_ADDR, BME280_REG_CALIB, calib, 26);

    calib_data.dig_T1 = (calib[1] << 8) | calib[0];
    calib_data.dig_T2 = (calib[3] << 8) | calib[2];
    calib_data.dig_T3 = (calib[5] << 8) | calib[4];
    calib_data.dig_P1 = (calib[7] << 8) | calib[6];
    calib_data.dig_P2 = (calib[9] << 8) | calib[8];
    calib_data.dig_P3 = (calib[11] << 8) | calib[10];
    calib_data.dig_P4 = (calib[13] << 8) | calib[12];
    calib_data.dig_P5 = (calib[15] << 8) | calib[14];
    calib_data.dig_P6 = (calib[17] << 8) | calib[16];
    calib_data.dig_P7 = (calib[19] << 8) | calib[18];
    calib_data.dig_P8 = (calib[21] << 8) | calib[20];
    calib_data.dig_P9 = (calib[23] << 8) | calib[22];

    // 配置传感器
    i2c_write_reg(BME280_ADDR, BME280_REG_CTRL_HUM, 0x01);   // 湿度过采样 x1
    i2c_write_reg(BME280_ADDR, BME280_REG_CTRL_MEAS, 0x27);  // 温度/压力过采样 x1, 正常模式
    i2c_write_reg(BME280_ADDR, BME280_REG_CONFIG, 0x00);     // 配置

    return true;
}

// 温度补偿 (来自 BME280 数据手册)
int32_t compensate_temperature(int32_t adc_T) {
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t)calib_data.dig_T1 << 1))) *
            ((int32_t)calib_data.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)calib_data.dig_T1)) *
              ((adc_T >> 4) - ((int32_t)calib_data.dig_T1))) >> 12) *
            ((int32_t)calib_data.dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

// 压力补偿
uint32_t compensate_pressure(int32_t adc_P) {
    int64_t var1, var2, p;
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)calib_data.dig_P6;
    var2 = var2 + ((var1 * (int64_t)calib_data.dig_P5) << 17);
    var2 = var2 + (((int64_t)calib_data.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)calib_data.dig_P3) >> 8) +
           ((var1 * (int64_t)calib_data.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)calib_data.dig_P1) >> 33;

    if (var1 == 0) return 0;

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)calib_data.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)calib_data.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)calib_data.dig_P7) << 4);

    return (uint32_t)p;
}

void bme280_read(float* temperature, float* pressure, float* humidity) {
    uint8_t data[8];
    i2c_read_reg(BME280_ADDR, BME280_REG_PRESS_MSB, data, 8);

    int32_t adc_P = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | (data[2] >> 4);
    int32_t adc_T = ((int32_t)data[3] << 12) | ((int32_t)data[4] << 4) | (data[5] >> 4);
    int32_t adc_H = ((int32_t)data[6] << 8) | data[7];

    // 补偿
    int32_t temp = compensate_temperature(adc_T);
    *temperature = temp / 100.0f;

    uint32_t press = compensate_pressure(adc_P);
    *pressure = press / 25600.0f;  // hPa

    // 简化湿度计算
    *humidity = adc_H / 1024.0f;  // 简化版
}

// 扫描 I2C 总线上的设备
void i2c_scan(void) {
    printf("扫描 I2C 总线...\n");
    for (uint8_t addr = 0x08; addr < 0x78; addr++) {
        uint8_t dummy;
        int ret = i2c_read_blocking(I2C_PORT, addr, &dummy, 1, false);
        if (ret >= 0) {
            printf("发现设备: 0x%02X\n", addr);
        }
    }
    printf("扫描完成\n");
}

int main(void) {
    stdio_init_all();
    sleep_ms(2000);  // 等待 USB 串口连接

    printf("I2C BME280 示例启动\n");

    i2c_init_custom();
    i2c_scan();

    if (!bme280_init()) {
        printf("BME280 初始化失败\n");
        while (1) tight_loop_contents();
    }

    while (1) {
        float temp, press, hum;
        bme280_read(&temp, &press, &hum);

        printf("温度: %.2f°C, 压力: %.2f hPa, 湿度: %.1f%%\n",
               temp, press, hum);

        sleep_ms(1000);
    }

    return 0;
}
