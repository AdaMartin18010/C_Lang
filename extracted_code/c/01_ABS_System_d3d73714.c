/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\01_ABS_System.md
 * Line: 1106
 * Language: c
 * Block ID: d3d73714
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*============================================================================
 * MODULE:    SlipRatioCalculation
 * FILE:      slip_ratio.c
 * DESCRIPTION: 滑移率计算与参考车速估计
 *============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/*============================================================================
 * 常量定义
 *============================================================================*/
#define SR_NUM_WHEELS               4u
#define SR_SAMPLE_TIME_MS           10u

/* 滑移率阈值 */
#define SR_SLIP_OPTIMAL_MIN         0.10f
#define SR_SLIP_OPTIMAL_MAX         0.20f
#define SR_SLIP_CRITICAL            0.30f
#define SR_SLIP_ENTER_ABS           0.15f
#define SR_SLIP_EXIT_ABS            0.08f

/* 加速度约束 */
#define SR_MAX_DECELERATION         15.0f   /* m/s² */
#define SR_MAX_ACCELERATION         8.0f    /* m/s² */
#define SR_MIN_VEHICLE_SPEED        1.0f    /* km/h */

/* 参考车速估计方法选择 */
typedef enum {
    SR_EST_MAX_WHEEL = 0,   /* 最大轮速法 */
    SR_EST_KALMAN,          /* 卡尔曼滤波 */
    SR_EST_FUSION           /* 融合方法 */
} SR_EstimationMethodType;

/*============================================================================
 * 数据结构
 *============================================================================*/

typedef struct {
    float slip_ratio;           /* 滑移率 0-1 */
    float wheel_speed;          /* 轮速 km/h */
    float target_slip;          /* 目标滑移率 */
    bool in_control;            /* 是否处于控制中 */
} SR_WheelChannelType;

typedef struct {
    float vehicle_speed;        /* 参考车速 km/h */
    float vehicle_accel;        /* 车辆加速度 m/s² */
    float estimated_speed;      /* 估计车速 */
    uint8_t confidence;         /* 估计置信度 0-100 */
    bool is_estimated;          /* 是否为估计值 */
    SR_EstimationMethodType method;
} SR_VehicleStateType;

typedef struct {
    SR_WheelChannelType wheel[SR_NUM_WHEELS];
    SR_VehicleStateType vehicle;

    /* 卡尔曼滤波状态 */
    float kalman_x;             /* 状态估计 */
    float kalman_p;             /* 估计误差协方差 */

    /* 滤波系数 */
    float alpha_speed;
    float alpha_accel;
} SR_ModuleType;

/* 卡尔曼滤波参数 */
#define SR_KALMAN_Q               0.1f    /* 过程噪声 */
#define SR_KALMAN_R               1.0f    /* 测量噪声 */

/*============================================================================
 * 静态变量
 *============================================================================*/
static SR_ModuleType sr_module;

/*============================================================================
 * 内部函数原型
 *============================================================================*/
static float SR_CalculateSlipRatio(float vehicle_speed, float wheel_speed);
static void SR_EstimateByMaxWheel(SR_ModuleType *sr);
static void SR_EstimateByKalman(SR_ModuleType *sr);
static float SR_SelectReferenceSpeed(const SR_ModuleType *sr);

/*============================================================================
 * 函数: SR_Init
 *============================================================================*/
void SR_Init(void)
{
    uint8_t i;

    for (i = 0u; i < SR_NUM_WHEELS; i++) {
        sr_module.wheel[i].slip_ratio = 0.0f;
        sr_module.wheel[i].wheel_speed = 0.0f;
        sr_module.wheel[i].target_slip = SR_SLIP_OPTIMAL_MIN;
        sr_module.wheel[i].in_control = false;
    }

    sr_module.vehicle.vehicle_speed = 0.0f;
    sr_module.vehicle.vehicle_accel = 0.0f;
    sr_module.vehicle.confidence = 0u;
    sr_module.vehicle.is_estimated = false;
    sr_module.vehicle.method = SR_EST_MAX_WHEEL;

    sr_module.kalman_x = 0.0f;
    sr_module.kalman_p = 1.0f;

    sr_module.alpha_speed = 0.3f;
    sr_module.alpha_accel = 0.5f;
}

/*============================================================================
 * 函数: SR_CalculateSlipRatio
 * 描述: 计算滑移率
 * 公式: lambda = (V_vehicle - V_wheel) / V_vehicle
 *============================================================================*/
static float SR_CalculateSlipRatio(float vehicle_speed, float wheel_speed)
{
    float slip;

    /* 车速过低时不计算滑移率 */
    if (vehicle_speed < SR_MIN_VEHICLE_SPEED) {
        return 0.0f;
    }

    /* 计算滑移率 */
    slip = (vehicle_speed - wheel_speed) / vehicle_speed;

    /* 限幅处理 */
    if (slip < 0.0f) {
        slip = 0.0f;
    } else if (slip > 1.0f) {
        slip = 1.0f;
    }

    return slip;
}

/*============================================================================
 * 函数: SR_Update_10ms
 * 描述: 10ms周期更新 - 滑移率计算与参考车速估计
 *============================================================================*/
void SR_Update_10ms(const float *wheel_speeds)
{
    uint8_t i;
    float prev_vehicle_speed;
    float new_vehicle_speed;
    float dt;

    dt = SR_SAMPLE_TIME_MS / 1000.0f;
    prev_vehicle_speed = sr_module.vehicle.vehicle_speed;

    /* 更新各轮速 */
    for (i = 0u; i < SR_NUM_WHEELS; i++) {
        sr_module.wheel[i].wheel_speed = wheel_speeds[i];
    }

    /* 参考车速估计 */
    switch (sr_module.vehicle.method) {
        case SR_EST_KALMAN:
            SR_EstimateByKalman(&sr_module);
            break;
        case SR_EST_MAX_WHEEL:
        default:
            SR_EstimateByMaxWheel(&sr_module);
            break;
    }

    new_vehicle_speed = SR_SelectReferenceSpeed(&sr_module);

    /* 加速度约束滤波 */
    float speed_diff = new_vehicle_speed - prev_vehicle_speed;
    float max_accel_change = SR_MAX_ACCELERATION * dt * 3.6f;  /* km/h */
    float max_decel_change = SR_MAX_DECELERATION * dt * 3.6f;

    if (speed_diff > max_accel_change) {
        new_vehicle_speed = prev_vehicle_speed + max_accel_change;
        sr_module.vehicle.is_estimated = true;
    } else if (speed_diff < -max_decel_change) {
        new_vehicle_speed = prev_vehicle_speed - max_decel_change;
        sr_module.vehicle.is_estimated = true;
    } else {
        sr_module.vehicle.is_estimated = false;
    }

    /* 更新车速 */
    sr_module.vehicle.vehicle_speed =
        sr_module.alpha_speed * new_vehicle_speed +
        (1.0f - sr_module.alpha_speed) * prev_vehicle_speed;

    /* 计算车辆加速度 (m/s²) */
    sr_module.vehicle.vehicle_accel =
        (sr_module.vehicle.vehicle_speed - prev_vehicle_speed) / (3.6f * dt);

    /* 更新各轮滑移率 */
    for (i = 0u; i < SR_NUM_WHEELS; i++) {
        sr_module.wheel[i].slip_ratio = SR_CalculateSlipRatio(
            sr_module.vehicle.vehicle_speed,
            sr_module.wheel[i].wheel_speed
        );
    }

    /* 更新置信度 */
    if (sr_module.vehicle.is_estimated) {
        if (sr_module.vehicle.confidence > 5u) {
            sr_module.vehicle.confidence -= 5u;
        }
    } else {
        if (sr_module.vehicle.confidence < 100u) {
            sr_module.vehicle.confidence++;
        }
    }
}

/*============================================================================
 * 函数: SR_EstimateByMaxWheel
 * 描述: 基于最大轮速的参考车速估计
 *============================================================================*/
static void SR_EstimateByMaxWheel(SR_ModuleType *sr)
{
    float max_speed = 0.0f;
    uint8_t i;

    for (i = 0u; i < SR_NUM_WHEELS; i++) {
        if (sr->wheel[i].wheel_speed > max_speed) {
            max_speed = sr->wheel[i].wheel_speed;
        }
    }

    sr->vehicle.estimated_speed = max_speed;
}

/*============================================================================
 * 函数: SR_EstimateByKalman
 * 描述: 基于卡尔曼滤波的参考车速估计
 *============================================================================*/
static void SR_EstimateByKalman(SR_ModuleType *sr)
{
    float max_wheel_speed = 0.0f;
    uint8_t i;

    /* 预测步 */
    float x_pred = sr->kalman_x + sr->vehicle.vehicle_accel * (SR_SAMPLE_TIME_MS / 1000.0f) * 3.6f;
    float p_pred = sr->kalman_p + SR_KALMAN_Q;

    /* 获取测量值 (最大轮速) */
    for (i = 0u; i < SR_NUM_WHEELS; i++) {
        if (sr->wheel[i].wheel_speed > max_wheel_speed) {
            max_wheel_speed = sr->wheel[i].wheel_speed;
        }
    }

    /* 更新步 */
    float k = p_pred / (p_pred + SR_KALMAN_R);  /* 卡尔曼增益 */
    sr->kalman_x = x_pred + k * (max_wheel_speed - x_pred);
    sr->kalman_p = (1.0f - k) * p_pred;

    sr->vehicle.estimated_speed = sr->kalman_x;
}

/*============================================================================
 * 函数: SR_SelectReferenceSpeed
 * 描述: 选择最终参考车速
 *============================================================================*/
static float SR_SelectReferenceSpeed(const SR_ModuleType *sr)
{
    /* 优先使用测量值，必要时使用估计值 */
    return sr->vehicle.estimated_speed;
}

/*============================================================================
 * 函数: SR_GetSlipRatio
 *============================================================================*/
float SR_GetSlipRatio(uint8_t wheel_index)
{
    if (wheel_index < SR_NUM_WHEELS) {
        return sr_module.wheel[wheel_index].slip_ratio;
    }
    return 0.0f;
}

/*============================================================================
 * 函数: SR_GetVehicleSpeed
 *============================================================================*/
float SR_GetVehicleSpeed(void)
{
    return sr_module.vehicle.vehicle_speed;
}

/*============================================================================
 * 函数: SR_GetVehicleAccel
 *============================================================================*/
float SR_GetVehicleAccel(void)
{
    return sr_module.vehicle.vehicle_accel;
}

/*============================================================================
 * 函数: SR_IsInOptimalRange
 * 描述: 检查滑移率是否在最佳区间
 *============================================================================*/
bool SR_IsInOptimalRange(uint8_t wheel_index)
{
    float slip;

    if (wheel_index >= SR_NUM_WHEELS) {
        return false;
    }

    slip = sr_module.wheel[wheel_index].slip_ratio;
    return (slip >= SR_SLIP_OPTIMAL_MIN && slip <= SR_SLIP_OPTIMAL_MAX);
}

/*============================================================================
 * 函数: SR_GetTargetSlip
 * 描述: 获取目标滑移率 (根据路面自适应)
 *============================================================================*/
float SR_GetTargetSlip(uint8_t wheel_index)
{
    /* 可扩展为基于路面识别的自适应目标滑移率 */
    /* 高附路面: 15%, 低附路面: 10% */

    if (wheel_index < SR_NUM_WHEELS) {
        /* 简单实现：固定目标 */
        return SR_SLIP_OPTIMAL_MIN + (SR_SLIP_OPTIMAL_MAX - SR_SLIP_OPTIMAL_MIN) / 2.0f;
    }

    return SR_SLIP_OPTIMAL_MIN;
}
