/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 840
 * Language: c
 * Block ID: 6741cfb3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * adc_example.c
 * ESP32 ADC 模数转换示例
 */

#include "driver/adc.h"
#include "esp_adc_cal.h"

#define ADC_CHANNEL         ADC1_CHANNEL_6    /* GPIO34 */
#define ADC_ATTEN           ADC_ATTEN_DB_11   /* 0-3.3V 量程 */
#define ADC_WIDTH           ADC_WIDTH_BIT_12  /* 12 位分辨率 */

static esp_adc_cal_characteristics_t adc_chars;

/**
 * @brief 初始化 ADC
 */
static void adc_init(void)
{
    /* 配置 ADC 宽度 */
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH));

    /* 配置 ADC 衰减 (量程) */
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN));

    /* 校准 ADC (使用默认 Vref, 或使用 efuse 校准值) */
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN, ADC_WIDTH,
                             DEFAULT_VREF, &adc_chars);
}

/**
 * @brief 读取 ADC 电压值
 * @return 电压值 (mV)
 */
static uint32_t adc_read_voltage(void)
{
    /* 多次采样取平均 */
    uint32_t adc_reading = 0;
    const int samples = 64;

    for (int i = 0; i < samples; i++) {
        adc_reading += adc1_get_raw(ADC_CHANNEL);
    }
    adc_reading /= samples;

    /* 转换为电压 (mV) */
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, &adc_chars);

    return voltage;
}

/**
 * @brief 温度传感器读取 (ESP32-S3/C3 内置)
 */
static float read_temperature(void)
{
    /* ESP32-S3/C3 内置温度传感器 */
    float temp_c = 0.0f;

    /* 启用温度传感器 */
    temperature_sensor_handle_t temp_handle = NULL;
    temperature_sensor_config_t temp_sensor = {
        .range_min = -10,
        .range_max = 80,
    };

    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor, &temp_handle));
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_handle));
    ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_handle, &temp_c));
    ESP_ERROR_CHECK(temperature_sensor_disable(temp_handle));
    ESP_ERROR_CHECK(temperature_sensor_uninstall(temp_handle));

    return temp_c;
}
