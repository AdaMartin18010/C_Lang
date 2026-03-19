/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 1196
 * Language: c
 * Block ID: d798725a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 温度传感器: 通常需要线性化或查表
// NTC热敏电阻: R = R0 * exp(B*(1/T - 1/T0))
// ============================================================

#include <math.h>

// NTC参数
#define NTC_R0      10000.0f   // 25°C时阻值 10kΩ
#define NTC_T0      298.15f    // 25°C的绝对温度
#define NTC_B       3950.0f    // B值
#define PULLUP_R    10000.0f   // 上拉电阻

// ADC值转温度 (Steinhart-Hart简化)
float ntc_adc_to_temp(uint16_t adc_val, uint16_t adc_max) {
    if (adc_val == 0) return -999;  // 开路
    if (adc_val >= adc_max) return 999;  // 短路

    // 计算电阻值
    float v_ratio = (float)adc_val / adc_max;
    float r_ntc = PULLUP_R * v_ratio / (1.0f - v_ratio);

    // Steinhart-Hart方程
    float invT = 1.0f / NTC_T0 + (1.0f / NTC_B) * logf(r_ntc / NTC_R0);
    float temp_k = 1.0f / invT;
    float temp_c = temp_k - 273.15f;

    return temp_c;
}

// 查表法 (更快，适合资源受限)
// 预计算的ADC值到温度映射
static const int16_t temp_lut[] = {
    // ADC=0..4095对应的温度(乘以10)
    // 通过脚本预生成
    -400, -395, -390, /* ... */  // 索引0对应ADC=0
};

int16_t temp_lookup(uint16_t adc_val) {
    // 限幅
    if (adc_val >= sizeof(temp_lut)/sizeof(temp_lut[0])) {
        adc_val = sizeof(temp_lut)/sizeof(temp_lut[0]) - 1;
    }
    return temp_lut[adc_val];  // 返回温度*10
}
