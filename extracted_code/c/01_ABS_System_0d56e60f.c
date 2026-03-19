/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\01_ABS_System.md
 * Line: 741
 * Language: c
 * Block ID: 0d56e60f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*============================================================================
 * MODULE:    WheelSpeedProcessing
 * FILE:      wheel_speed.c
 * VERSION:   3.2.1
 * DESCRIPTION:
 *   轮速信号处理模块 - 符合MISRA C:2012和ISO 26262 ASIL-D要求
 *   支持电磁感应式和霍尔式传感器
 *============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/*============================================================================
 * 配置常量
 *============================================================================*/
#define WSP_NUM_WHEELS              4u      /* 车轮数量 */
#define WSP_TEETH_COUNT             48u     /* 齿圈齿数 */
#define WSP_WHEEL_CIRCUMFERENCE_MM  2000u   /* 轮胎周长 (mm) */
#define WSP_SAMPLE_PERIOD_MS        10u     /* 采样周期 (ms) */
#define WSP_TIMER_FREQ_HZ           1000000u /* 定时器频率 1MHz (us) */

/* 频率限制 */
#define WSP_FREQ_MIN_HZ             3u      /* 约3 km/h */
#define WSP_FREQ_MAX_HZ             2500u   /* 约250 km/h */

/* 信号质量阈值 */
#define WSP_QUALITY_GOOD            80u
#define WSP_QUALITY_FAIR            50u
#define WSP_QUALITY_POOR            20u

/* 故障检测阈值 */
#define WSP_TIMEOUT_MS              200u    /* 信号超时 (ms) */
#define WSP_JUMP_THRESHOLD_KMH      30.0f   /* 跳变检测阈值 (km/h) */
#define WSP_NOISE_THRESHOLD         5u      /* 连续异常计数阈值 */

/*============================================================================
 * 类型定义
 *============================================================================*/

typedef enum {
    WSP_SENSOR_OK = 0,
    WSP_SENSOR_OPEN_CIRCUIT,
    WSP_SENSOR_SHORT_CIRCUIT,
    WSP_SENSOR_SIGNAL_LOSS,
    WSP_SENSOR_NOISE_EXCESSIVE,
    WSP_SENSOR_PLAUSIBILITY_ERROR
} WSP_SensorStatusType;

typedef enum {
    WSP_WHEEL_FL = 0,   /* 左前 */
    WSP_WHEEL_FR,       /* 右前 */
    WSP_WHEEL_RL,       /* 左后 */
    WSP_WHEEL_RR        /* 右后 */
} WSP_WheelIndexType;

typedef struct {
    uint32_t period_us;              /* 脉冲周期 (us) */
    uint32_t period_filtered;        /* 滤波后周期 */
    uint32_t last_capture_time;      /* 上次捕获时间 */
    uint32_t pulse_count;            /* 脉冲计数 */

    float speed_kmh;                 /* 轮速 (km/h) */
    float speed_filtered;            /* 滤波后轮速 */
    float acceleration;              /* 轮加速度 (m/s²) */

    uint8_t signal_quality;          /* 信号质量 0-100 */
    uint8_t noise_counter;           /* 噪声计数器 */
    WSP_SensorStatusType status;     /* 传感器状态 */

    /* 诊断相关 */
    uint32_t last_valid_time;        /* 上次有效信号时间 */
    uint32_t stuck_counter;          /* 卡滞计数器 */
} WSP_ChannelType;

typedef struct {
    WSP_ChannelType channel[WSP_NUM_WHEELS];
    uint32_t system_time_ms;         /* 系统时间 */
    bool initialization_complete;    /* 初始化完成标志 */
} WSP_ModuleType;

/*============================================================================
 * 静态变量
 *============================================================================*/
static WSP_ModuleType wsp_module;
static const float WSP_SPEED_CONST =
    ((float)WSP_WHEEL_CIRCUMFERENCE_MM * 3.6f) / (float)WSP_TEETH_COUNT; /* = 150.0 */

/* 滤波系数 */
#define WSP_FILTER_ALPHA            0.3f    /* 一阶低通滤波系数 */
#define WSP_FILTER_ALPHA_ACCEL      0.5f    /* 加速度滤波系数 */

/*============================================================================
 * 内部函数原型
 *============================================================================*/
static void WSP_UpdateSignalQuality(WSP_ChannelType *ch, bool valid);
static float WSP_LowPassFilter(float current, float previous, float alpha);
static bool WSP_CheckPlausibility(const WSP_ChannelType *ch, float new_speed);
static void WSP_DiagnoseChannel(WSP_ChannelType *ch, uint32_t current_time);

/*============================================================================
 * 函数: WSP_Init
 * 描述: 初始化轮速处理模块
 *============================================================================*/
void WSP_Init(void)
{
    uint8_t i;

    for (i = 0u; i < WSP_NUM_WHEELS; i++) {
        wsp_module.channel[i].period_us = 0u;
        wsp_module.channel[i].period_filtered = 0u;
        wsp_module.channel[i].last_capture_time = 0u;
        wsp_module.channel[i].pulse_count = 0u;
        wsp_module.channel[i].speed_kmh = 0.0f;
        wsp_module.channel[i].speed_filtered = 0.0f;
        wsp_module.channel[i].acceleration = 0.0f;
        wsp_module.channel[i].signal_quality = 0u;
        wsp_module.channel[i].noise_counter = 0u;
        wsp_module.channel[i].status = WSP_SENSOR_SIGNAL_LOSS;
        wsp_module.channel[i].last_valid_time = 0u;
        wsp_module.channel[i].stuck_counter = 0u;
    }

    wsp_module.system_time_ms = 0u;
    wsp_module.initialization_complete = true;
}

/*============================================================================
 * 函数: WSP_CaptureISR
 * 描述: 轮速捕获中断服务程序 - 最小化执行时间
 * 警告: 本函数在中断上下文中执行，禁止使用浮点运算
 *============================================================================*/
void WSP_CaptureISR(WSP_WheelIndexType wheel_id, uint32_t capture_time)
{
    WSP_ChannelType *ch;
    uint32_t period;

    /* 参数检查 */
    if (wheel_id >= WSP_NUM_WHEELS) {
        return;  /* 无效的车轮索引 */
    }

    ch = &wsp_module.channel[wheel_id];

    /* 计算周期 */
    if (capture_time >= ch->last_capture_time) {
        period = capture_time - ch->last_capture_time;
    } else {
        /* 定时器溢出处理 */
        period = (0xFFFFFFFFu - ch->last_capture_time) + capture_time + 1u;
    }

    /* 周期合理性检查 */
    if ((period >= (WSP_TIMER_FREQ_HZ / WSP_FREQ_MAX_HZ)) &&
        (period <= (WSP_TIMER_FREQ_HZ / WSP_FREQ_MIN_HZ))) {
        ch->period_us = period;
        ch->pulse_count++;
        ch->last_valid_time = wsp_module.system_time_ms;
        ch->stuck_counter = 0u;
    } else {
        /* 异常信号，增加噪声计数 */
        if (ch->noise_counter < 255u) {
            ch->noise_counter++;
        }
    }

    ch->last_capture_time = capture_time;
}

/*============================================================================
 * 函数: WSP_Process_10ms
 * 描述: 10ms周期处理任务 - 浮点运算和复杂逻辑
 *============================================================================*/
void WSP_Process_10ms(void)
{
    uint8_t i;
    uint32_t current_time;
    WSP_ChannelType *ch;
    float instant_speed;
    float prev_speed;

    wsp_module.system_time_ms += WSP_SAMPLE_PERIOD_MS;
    current_time = wsp_module.system_time_ms;

    for (i = 0u; i < WSP_NUM_WHEELS; i++) {
        ch = &wsp_module.channel[i];
        prev_speed = ch->speed_filtered;

        /* 检查信号超时 */
        if ((current_time - ch->last_valid_time) > WSP_TIMEOUT_MS) {
            /* 信号丢失 */
            ch->status = WSP_SENSOR_SIGNAL_LOSS;
            ch->speed_kmh = 0.0f;
            ch->speed_filtered = WSP_LowPassFilter(0.0f, prev_speed, WSP_FILTER_ALPHA);
            ch->acceleration = 0.0f;
            WSP_UpdateSignalQuality(ch, false);
            continue;
        }

        /* 计算瞬时速度: v = (周长 * 3600) / (齿数 * 周期_us) * 1000 */
        /* 简化为: v = 7200000 / (48 * 周期_us) = 150000 / 周期_us */
        if (ch->period_us > 0u) {
            instant_speed = WSP_SPEED_CONST * 1000.0f / (float)ch->period_us;
        } else {
            instant_speed = 0.0f;
        }

        /* 速度限幅 */
        if (instant_speed > 300.0f) {
            instant_speed = 300.0f;
        }

        /* 合理性检查 */
        if (!WSP_CheckPlausibility(ch, instant_speed)) {
            ch->noise_counter++;
            if (ch->noise_counter > WSP_NOISE_THRESHOLD) {
                ch->status = WSP_SENSOR_NOISE_EXCESSIVE;
            }
            /* 使用预测值 */
            instant_speed = prev_speed + ch->acceleration * 0.01f * 3.6f;
        } else {
            ch->noise_counter = 0u;
        }

        ch->speed_kmh = instant_speed;

        /* 低通滤波 */
        ch->speed_filtered = WSP_LowPassFilter(instant_speed, prev_speed, WSP_FILTER_ALPHA);

        /* 计算加速度 (m/s²) */
        /* a = (v_new - v_old) / (3.6 * dt) */
        ch->acceleration = WSP_LowPassFilter(
            (ch->speed_filtered - prev_speed) / (3.6f * 0.01f),
            ch->acceleration,
            WSP_FILTER_ALPHA_ACCEL
        );

        /* 执行诊断 */
        WSP_DiagnoseChannel(ch, current_time);

        /* 更新信号质量 */
        WSP_UpdateSignalQuality(ch, (ch->status == WSP_SENSOR_OK));
    }
}

/*============================================================================
 * 函数: WSP_LowPassFilter
 * 描述: 一阶低通滤波器
 *============================================================================*/
static float WSP_LowPassFilter(float current, float previous, float alpha)
{
    /* alpha: 滤波系数 (0-1)，越大响应越快 */
    return (alpha * current) + ((1.0f - alpha) * previous);
}

/*============================================================================
 * 函数: WSP_CheckPlausibility
 * 描述: 速度合理性检查
 *============================================================================*/
static bool WSP_CheckPlausibility(const WSP_ChannelType *ch, float new_speed)
{
    float speed_diff;

    /* 检查跳变 */
    speed_diff = new_speed - ch->speed_filtered;
    if (speed_diff < 0.0f) {
        speed_diff = -speed_diff;
    }

    /* 10ms内速度变化不应超过30km/h */
    if (speed_diff > WSP_JUMP_THRESHOLD_KMH) {
        return false;
    }

    /* 减速度合理性检查 (最大约-15m/s²) */
    if (new_speed < ch->speed_filtered) {
        if ((ch->speed_filtered - new_speed) > 0.54f) {  /* 15 * 3.6 * 0.01 */
            return false;
        }
    }

    return true;
}

/*============================================================================
 * 函数: WSP_UpdateSignalQuality
 * 描述: 更新信号质量指标
 *============================================================================*/
static void WSP_UpdateSignalQuality(WSP_ChannelType *ch, bool valid)
{
    if (valid) {
        if (ch->signal_quality < 100u) {
            ch->signal_quality++;
        }
    } else {
        if (ch->signal_quality > 0u) {
            ch->signal_quality -= 2u;  /* 衰减更快 */
        }
    }
}

/*============================================================================
 * 函数: WSP_DiagnoseChannel
 * 描述: 通道诊断
 *============================================================================*/
static void WSP_DiagnoseChannel(WSP_ChannelType *ch, uint32_t current_time)
{
    /* 卡滞检测 */
    if (ch->pulse_count == ch->stuck_counter) {
        ch->stuck_counter++;
        if (ch->stuck_counter > 20u) {  /* 200ms无变化 */
            ch->status = WSP_SENSOR_SIGNAL_LOSS;
        }
    }

    /* 根据信号质量更新状态 */
    if (ch->signal_quality >= WSP_QUALITY_GOOD) {
        ch->status = WSP_SENSOR_OK;
    } else if (ch->signal_quality >= WSP_QUALITY_FAIR) {
        if (ch->status != WSP_SENSOR_NOISE_EXCESSIVE) {
            ch->status = WSP_SENSOR_OK;
        }
    }
}

/*============================================================================
 * 函数: WSP_GetSpeed
 * 描述: 获取指定车轮速度
 *============================================================================*/
float WSP_GetSpeed(WSP_WheelIndexType wheel_id)
{
    if (wheel_id < WSP_NUM_WHEELS) {
        return wsp_module.channel[wheel_id].speed_filtered;
    }
    return 0.0f;
}

/*============================================================================
 * 函数: WSP_GetAcceleration
 * 描述: 获取指定车轮加速度
 *============================================================================*/
float WSP_GetAcceleration(WSP_WheelIndexType wheel_id)
{
    if (wheel_id < WSP_NUM_WHEELS) {
        return wsp_module.channel[wheel_id].acceleration;
    }
    return 0.0f;
}

/*============================================================================
 * 函数: WSP_GetStatus
 * 描述: 获取指定车轮传感器状态
 *============================================================================*/
WSP_SensorStatusType WSP_GetStatus(WSP_WheelIndexType wheel_id)
{
    if (wheel_id < WSP_NUM_WHEELS) {
        return wsp_module.channel[wheel_id].status;
    }
    return WSP_SENSOR_SIGNAL_LOSS;
}
