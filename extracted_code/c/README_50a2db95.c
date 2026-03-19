/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\README.md
 * Line: 486
 * Language: c
 * Block ID: 50a2db95
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* abs/abs_controller.c */
#include "abs/abs_controller.h"
#include "safety/safety_mechanisms.h"
#include "platform/critical_section.h"

/* 控制参数 - ASIL D 要求硬编码安全参数 */
#define SLIP_RATIO_TARGET       150   /* 目标滑移率 15% */
#define SLIP_RATIO_THRESHOLD    200   /* 触发 ABS 阈值 20% */
#define DECELERATION_THRESHOLD  800   /* 减速度阈值 m/s² */
#define WHEEL_SPEED_MIN         50    /* 最小工作轮速 RPM */

/* 电磁阀控制时序 (ms) */
#define VALVE_CYCLE_TIME        10
#define PRESSURE_HOLD_TIME      20
#define PRESSURE_DEC_TIME       15
#define PRESSURE_INC_TIME       25

static void calculate_slip_ratio(abs_channel_t* ch, uint32_t vehicle_speed);
static void update_control_state(abs_channel_t* ch, uint32_t dt_ms);
static void execute_valve_control(abs_channel_t* ch);

void abs_controller_update(abs_controller_t* abs, uint32_t dt_ms) {
    CRITICAL_SECTION_ENTER();

    /* 步骤 1: 估算参考车速 */
    uint32_t max_wheel_speed = 0;
    for (int i = 0; i < 4; i++) {
        if (abs->channels[i].wheel_speed_rpm > max_wheel_speed) {
            max_wheel_speed = abs->channels[i].wheel_speed_rpm;
        }
    }

    /* 使用最大轮速作为参考车速估算 */
    abs->vehicle_speed_kmh = wheel_rpm_to_kmh(max_wheel_speed);

    /* 步骤 2: 处理每个车轮通道 */
    for (int i = 0; i < 4; i++) {
        abs_channel_t* ch = &abs->channels[i];

        /* 安全检查: 传感器有效性 */
        if (ch->wheel_speed_rpm == 0xFFFF) {
            ch->error_flags |= ABS_ERR_SENSOR_FAULT;
            ch->state = ABS_STATE_IDLE;
            continue;
        }

        /* 计算滑移率 */
        calculate_slip_ratio(ch, abs->vehicle_speed_kmh);

        /* 状态机更新 */
        if (abs->enabled &&
            ch->wheel_speed_rpm > WHEEL_SPEED_MIN &&
            abs->vehicle_speed_kmh > 10) {
            update_control_state(ch, dt_ms);
        } else {
            ch->state = ABS_STATE_IDLE;
        }

        /* 执行控制输出 */
        execute_valve_control(ch);

        ch->cycle_count++;
    }

    CRITICAL_SECTION_EXIT();
}

static void calculate_slip_ratio(abs_channel_t* ch, uint32_t vehicle_speed) {
    if (vehicle_speed == 0) {
        ch->slip_ratio_permille = 0;
        return;
    }

    uint32_t wheel_speed = wheel_rpm_to_kmh(ch->wheel_speed_rpm);

    /* 滑移率 λ = (V_vehicle - V_wheel) / V_vehicle * 1000‰ */
    if (wheel_speed < vehicle_speed) {
        ch->slip_ratio_permille =
            ((vehicle_speed - wheel_speed) * 1000) / vehicle_speed;
    } else {
        ch->slip_ratio_permille = 0;
    }
}

static void update_control_state(abs_channel_t* ch, uint32_t dt_ms) {
    /* ABS 状态机 */
    switch (ch->state) {
        case ABS_STATE_IDLE:
        case ABS_STATE_MONITORING:
            /* 检查是否需要介入 */
            if (ch->slip_ratio_permille > SLIP_RATIO_THRESHOLD) {
                ch->state = ABS_STATE_PRESSURE_DEC;
                ch->cycle_count = 0;
            }
            break;

        case ABS_STATE_PRESSURE_DEC:
            /* 减压直到滑移率降到目标以下 */
            if (ch->slip_ratio_permille < SLIP_RATIO_TARGET) {
                ch->state = ABS_STATE_PRESSURE_HOLD;
                ch->cycle_count = 0;
            }
            break;

        case ABS_STATE_PRESSURE_HOLD:
            /* 保压观察 */
            if (ch->cycle_count * dt_ms > PRESSURE_HOLD_TIME) {
                if (ch->slip_ratio_permille > SLIP_RATIO_THRESHOLD) {
                    ch->state = ABS_STATE_PRESSURE_DEC;
                } else if (ch->slip_ratio_permille < SLIP_RATIO_TARGET / 2) {
                    ch->state = ABS_STATE_PRESSURE_INC;
                }
                ch->cycle_count = 0;
            }
            break;

        case ABS_STATE_PRESSURE_INC:
            /* 增压 */
            if (ch->cycle_count * dt_ms > PRESSURE_INC_TIME) {
                ch->state = ABS_STATE_PRESSURE_HOLD;
                ch->cycle_count = 0;
            }
            if (ch->slip_ratio_permille > SLIP_RATIO_THRESHOLD) {
                ch->state = ABS_STATE_PRESSURE_DEC;
                ch->cycle_count = 0;
            }
            break;
    }
}

static void execute_valve_control(abs_channel_t* ch) {
    /* 电磁阀控制真值表 */
    switch (ch->state) {
        case ABS_STATE_IDLE:
        case ABS_STATE_MONITORING:
            ch->inlet_valve_pwm = 0;    /* 进油阀开 */
            ch->outlet_valve_pwm = 0;   /* 出油阀关 */
            ch->pump_motor_pwm = 0;     /* 泵关 */
            break;

        case ABS_STATE_PRESSURE_HOLD:
            ch->inlet_valve_pwm = 255;  /* 进油阀关 */
            ch->outlet_valve_pwm = 0;   /* 出油阀关 */
            ch->pump_motor_pwm = 0;
            break;

        case ABS_STATE_PRESSURE_DEC:
            ch->inlet_valve_pwm = 255;  /* 进油阀关 */
            ch->outlet_valve_pwm = 255; /* 出油阀开 */
            ch->pump_motor_pwm = 255;   /* 泵开 */
            break;

        case ABS_STATE_PRESSURE_INC:
            ch->inlet_valve_pwm = 0;    /* 进油阀开 */
            ch->outlet_valve_pwm = 0;   /* 出油阀关 */
            ch->pump_motor_pwm = 0;
            break;
    }

    /* 输出到硬件 */
    pwm_set_duty(PWM_INLET_VALVE(ch->wheel_id), ch->inlet_valve_pwm);
    pwm_set_duty(PWM_OUTLET_VALVE(ch->wheel_id), ch->outlet_valve_pwm);
    pwm_set_duty(PWM_PUMP_MOTOR(ch->wheel_id), ch->pump_motor_pwm);
}
