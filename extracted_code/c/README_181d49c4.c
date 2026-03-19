/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Cryogenic_Superconducting\README.md
 * Line: 429
 * Language: c
 * Block ID: 181d49c4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 稀释制冷机温度级
typedef enum {
    TEMP_STAGE_300K,    // 室温
    TEMP_STAGE_50K,     // 一级冷屏
    TEMP_STAGE_4K,      // 二级冷屏
    TEMP_STAGE_1K,      // 1K 壶
    TEMP_STAGE_100MK,   // 仍在盘
    TEMP_STAGE_10MK     // 混合室
} TempStage;

// 温度传感器类型
typedef enum {
    SENSOR_CERNOX,      // 4K-300K
    SENSOR_RUTHENIUM,   // 50mK-4K
    SENSOR_ROX,         // 10mK-1K
    SENSOR_UVS          // 超低温
} TempSensorType;

// PID 温控参数
typedef struct {
    float kp, ki, kd;
    float setpoint;
    float integral;
    float prev_error;
    float output_limit;
} PID_Controller;

// 温度控制循环
void temperature_control_loop(PID_Controller *pid,
                              float current_temp,
                              float *heater_power) {
    float error = pid->setpoint - current_temp;

    // 积分项
    pid->integral += error;

    // 微分项
    float derivative = error - pid->prev_error;

    // PID 计算
    float output = pid->kp * error +
                   pid->ki * pid->integral +
                   pid->kd * derivative;

    // 限幅
    if (output > pid->output_limit) output = pid->output_limit;
    if (output < 0) output = 0;

    *heater_power = output;
    pid->prev_error = error;
}

// 自动降温程序
void auto_cooldown_sequence(void) {
    printf("Starting auto cooldown sequence...\n");

    // 1. 预冷 - 液氮
    set_temp_setpoint(TEMP_STAGE_50K, 77.0f);
    wait_for_temperature(TEMP_STAGE_50K, 80.0f, 600);  // 10分钟

    // 2. 主冷 - 液氦
    set_temp_setpoint(TEMP_STAGE_4K, 4.2f);
    wait_for_temperature(TEMP_STAGE_4K, 5.0f, 3600);   // 1小时

    // 3. 启动稀释制冷
    enable_dilution_cooling();
    set_temp_setpoint(TEMP_STAGE_1K, 1.5f);
    wait_for_temperature(TEMP_STAGE_1K, 2.0f, 1800);

    // 4. 基温
    set_temp_setpoint(TEMP_STAGE_10MK, 0.01f);
    wait_for_temperature(TEMP_STAGE_10MK, 0.015f, 7200);

    printf("Cooldown complete. Base temperature reached.\n");
}
