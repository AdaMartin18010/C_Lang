/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 1331
 * Language: c
 * Block ID: 2640fb71
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file adc_basic.c
 * @brief ADC 基础读取示例
 */

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"

// ADC 输入引脚
#define ADC_POT_PIN     26  // 电位器
#define ADC_LDR_PIN     27  // 光敏电阻
#define ADC_TEMP_PIN    28  // 外部温度传感器 (如 LM35)

// 参考电压 (Vref)
#define ADC_VREF        3.3f
#define ADC_RESOLUTION  4096.0f  // 12-bit

void adc_init_all(void) {
    // 初始化 ADC 硬件
    adc_init();

    // 启用温度传感器
    adc_set_temp_sensor_enabled(true);
}

float adc_read_voltage(uint input) {
    adc_select_input(input);
    uint16_t raw = adc_read();
    return (raw / ADC_RESOLUTION) * ADC_VREF;
}

float adc_read_temperature_internal(void) {
    // 内部温度传感器读取
    adc_select_input(4);  // 温度传感器是输入 4
    uint16_t raw = adc_read();

    // 温度计算公式 (来自数据手册)
    // Vbe = raw * Vref / 4096
    // Temperature = 27 - (Vbe - 0.706) / 0.001721
    float voltage = raw * ADC_VREF / ADC_RESOLUTION;
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;

    return temperature;
}

float lm35_read_temperature(uint adc_input) {
    // LM35: 10mV/°C
    float voltage = adc_read_voltage(adc_input);
    return voltage * 100.0f;  // °C
}

float read_battery_voltage(uint adc_input, float divider_ratio) {
    // 通过分压电阻读取电池电压
    float voltage = adc_read_voltage(adc_input);
    return voltage * divider_ratio;
}

int main(void) {
    stdio_init_all();
    adc_init_all();

    // 配置 GPIO 为 ADC 功能 (不需要，adc_select_input 自动处理)

    printf("ADC 读取示例启动\n");

    while (1) {
        // 读取电位器 (ADC0 - GPIO26)
        float pot_voltage = adc_read_voltage(0);

        // 读取光敏电阻 (ADC1 - GPIO27)
        float ldr_voltage = adc_read_voltage(1);

        // 读取内部温度
        float int_temp = adc_read_temperature_internal();

        // 读取外部温度 (假设连接到 ADC2)
        // float ext_temp = lm35_read_temperature(2);

        printf("电位器: %.2fV, 光敏: %.2fV, 内部温度: %.1f°C\n",
               pot_voltage, ldr_voltage, int_temp);

        sleep_ms(500);
    }

    return 0;
}
