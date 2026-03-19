/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\README.md
 * Line: 148
 * Language: c
 * Block ID: 6242370f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file SpeedSensor_SWC.c
 * @brief 轮速传感器软件组件 - AUTOSAR应用层实现
 */

#include "Rte_SpeedSensor.h"
#include "Os.h"

#define WHEEL_CIRCUMFERENCE_MM      2050.0f
#define PULSE_PER_REVOLUTION        48

typedef struct {
    float speed_kmh;
    uint16_t pulse_count;
    uint32_t last_time_ms;
    boolean valid;
} WheelSpeedData_t;

static WheelSpeedData_t g_wheels[4];
static float g_vehicle_speed = 0.0f;

/**
 * @brief 初始化Runnable
 */
FUNC(void, SpeedSensor_CODE) SpeedSensor_Init(void)
{
    for (int i = 0; i < 4; i++) {
        g_wheels[i].speed_kmh = 0.0f;
        g_wheels[i].valid = FALSE;
    }
}

/**
 * @brief 周期性10ms处理Runnable
 */
FUNC(void, SpeedSensor_CODE) SpeedSensor_MainFunction(void)
{
    /* 读取传感器数据 */
    for (int i = 0; i < 4; i++) {
        uint16_t current_pulse;
        Rte_Call_RP_IoHwAb_GetPulseCounter(i, &current_pulse);

        uint32_t current_time = GetCounterValue();
        uint16_t delta_pulse = current_pulse - g_wheels[i].pulse_count;
        uint32_t delta_time = current_time - g_wheels[i].last_time_ms;

        if (delta_time > 0) {
            float distance_mm = (float)delta_pulse * WHEEL_CIRCUMFERENCE_MM
                               / PULSE_PER_REVOLUTION;
            float time_s = (float)delta_time / 1000.0f;
            g_wheels[i].speed_kmh = (distance_mm / time_s) * 3.6f / 1000.0f;
            g_wheels[i].valid = TRUE;
        }

        g_wheels[i].pulse_count = current_pulse;
        g_wheels[i].last_time_ms = current_time;
    }

    /* 计算整车速度 (平均值) */
    float sum = 0.0f;
    int valid_count = 0;
    for (int i = 0; i < 4; i++) {
        if (g_wheels[i].valid) {
            sum += g_wheels[i].speed_kmh;
            valid_count++;
        }
    }
    g_vehicle_speed = (valid_count > 0) ? sum / valid_count : 0.0f;

    /* 通过RTE发送数据 */
    Rte_Write_PP_SpeedData_VehicleSpeed(g_vehicle_speed);
}
