/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\README.md
 * Line: 425
 * Language: c
 * Block ID: 43b1cc16
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* abs/abs_controller.h */
#ifndef ABS_CONTROLLER_H
#define ABS_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

/* ABS 控制状态 */
typedef enum {
    ABS_STATE_IDLE,          /* 无制动 */
    ABS_STATE_MONITORING,    /* 监控状态 */
    ABS_STATE_PRESSURE_HOLD, /* 保压 */
    ABS_STATE_PRESSURE_DEC,  /* 减压 */
    ABS_STATE_PRESSURE_INC   /* 增压 */
} abs_state_t;

/* 车轮控制通道 */
typedef struct {
    uint8_t wheel_id;

    /* 传感器输入 */
    uint32_t wheel_speed_rpm;      /* 轮速 (RPM) */
    uint32_t wheel_acceleration;    /* 轮加速度 */
    uint16_t brake_pressure_kpa;    /* 制动压力 */

    /* 控制变量 */
    abs_state_t state;
    uint32_t slip_ratio_permille;   /* 滑移率 (‰) 目标: 100-300‰ */
    int32_t velocity_reference;     /* 参考车速 */

    /* 执行器控制 */
    uint8_t inlet_valve_pwm;        /* 进油阀 PWM */
    uint8_t outlet_valve_pwm;       /* 出油阀 PWM */
    uint8_t pump_motor_pwm;         /* 泵电机 PWM */

    /* 诊断 */
    uint16_t error_flags;
    uint32_t cycle_count;
} abs_channel_t;

/* ABS 控制器主结构 */
typedef struct {
    abs_channel_t channels[4];       /* FL, FR, RL, RR */
    uint32_t vehicle_speed_kmh;      /* 估算车速 */
    uint8_t system_state;
    bool enabled;

    /* 控制参数 */
    uint32_t slip_threshold_high;    /* 高压滑移率阈值 (‰) */
    uint32_t slip_threshold_low;     /* 低压滑移率阈值 (‰) */
    uint32_t accel_threshold;        /* 减速度阈值 */
} abs_controller_t;

abs_controller_t* abs_controller_init(void);
void abs_controller_update(abs_controller_t* abs, uint32_t dt_ms);
void abs_controller_enable(abs_controller_t* abs, bool enable);

#endif
