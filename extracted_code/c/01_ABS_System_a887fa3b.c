/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\01_ABS_System.md
 * Line: 1444
 * Language: c
 * Block ID: a887fa3b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*============================================================================
 * MODULE:    ABS_LogicThresholdControl
 * FILE:      abs_logic_control.c
 * DESCRIPTION: 基于逻辑门限的ABS控制 - 博世经典算法
 *============================================================================*/

#include <stdint.h>
#include <stdbool.h>

/*============================================================================
 * 常量定义
 *============================================================================*/
#define ABS_NUM_CHANNELS            4u
#define ABS_CYCLE_TIME_MS           10u

/* 控制状态 */
typedef enum {
    ABS_STATE_INACTIVE = 0,
    ABS_STATE_PRESSURE_HOLD,
    ABS_STATE_PRESSURE_DECREASE,
    ABS_STATE_PRESSURE_INCREASE,
    ABS_STATE_PRESSURE_DECREASE_FAST
} ABS_ControlStateType;

/* 电磁阀状态 */
typedef enum {
    VALVE_STATE_OPEN = 0,
    VALVE_STATE_CLOSE
} ABS_ValveStateType;

/* 控制参数结构 */
typedef struct {
    /* 进入条件 */
    float slip_threshold_enter;     /* 进入ABS滑移率阈值 */
    float decel_threshold_enter;    /* 进入ABS减速度阈值 (m/s²) */

    /* 退出条件 */
    float slip_threshold_exit;      /* 退出ABS滑移率阈值 */
    float decel_threshold_exit;     /* 退出ABS减速度阈值 */

    /* 状态转换阈值 */
    float slip_threshold_high;      /* 高滑移率阈值 (减压) */
    float slip_threshold_low;       /* 低滑移率阈值 (增压) */
    float decel_threshold_critical; /* 紧急减压减速度阈值 */

    /* 时间参数 (ms) */
    uint16_t time_hold_min;         /* 最小保压时间 */
    uint16_t time_dec_min;          /* 最小减压时间 */
    uint16_t time_inc_min;          /* 最小增压时间 */
    uint16_t time_inc_max;          /* 最大增压时间 */
    uint16_t time_stability;        /* 稳定性检查时间 */
} ABS_ParametersType;

/* 通道控制结构 */
typedef struct {
    ABS_ControlStateType state;         /* 当前状态 */
    ABS_ControlStateType last_state;    /* 上次状态 */

    uint16_t timer;                     /* 状态计时器 */
    uint16_t cycle_count;               /* 控制循环计数 */

    bool is_active;                     /* 该通道ABS激活 */
    bool enable;                        /* 通道使能 */

    /* 历史数据 */
    float slip_history[4];              /* 滑移率历史 */
    float decel_history[4];             /* 减速度历史 */
    uint8_t history_index;
} ABS_ChannelType;

/* 电磁阀输出 */
typedef struct {
    ABS_ValveStateType inlet_valve;     /* 进油阀 (常开) */
    ABS_ValveStateType outlet_valve;    /* 出油阀 (常闭) */
    bool pump_motor;                    /* 泵电机 */
} ABS_ValveOutputsType;

/*============================================================================
 * 默认参数 - 干燥沥青路面标定
 *============================================================================*/
static const ABS_ParametersType ABS_DefaultParams = {
    .slip_threshold_enter       = 0.15f,    /* 15%滑移率进入 */
    .decel_threshold_enter      = -8.0f,    /* -8 m/s²进入 */
    .slip_threshold_exit        = 0.08f,    /* 8%滑移率退出 */
    .decel_threshold_exit       = -5.0f,    /* -5 m/s²退出 */
    .slip_threshold_high        = 0.25f,    /* 25%高滑移率 */
    .slip_threshold_low         = 0.10f,    /* 10%低滑移率 */
    .decel_threshold_critical   = -12.0f,   /* -12 m/s²紧急减压 */
    .time_hold_min              = 20u,      /* 20ms */
    .time_dec_min               = 20u,      /* 20ms */
    .time_inc_min               = 30u,      /* 30ms */
    .time_inc_max               = 100u,     /* 100ms */
    .time_stability             = 50u       /* 50ms */
};

/*============================================================================
 * 静态变量
 *============================================================================*/
static ABS_ChannelType abs_channels[ABS_NUM_CHANNELS];
static ABS_ParametersType abs_params;
static bool abs_enabled;

/*============================================================================
 * 内部函数原型
 *============================================================================*/
static void ABS_UpdateHistory(ABS_ChannelType *ch, float slip, float decel);
static bool ABS_CheckEntryCondition(const ABS_ChannelType *ch, float slip, float decel);
static bool ABS_CheckExitCondition(const ABS_ChannelType *ch, float slip, float decel);

/*============================================================================
 * 函数: ABS_LogicControl_Init
 *============================================================================*/
void ABS_LogicControl_Init(void)
{
    uint8_t i;
    uint8_t j;

    abs_params = ABS_DefaultParams;
    abs_enabled = true;

    for (i = 0u; i < ABS_NUM_CHANNELS; i++) {
        abs_channels[i].state = ABS_STATE_INACTIVE;
        abs_channels[i].last_state = ABS_STATE_INACTIVE;
        abs_channels[i].timer = 0u;
        abs_channels[i].cycle_count = 0u;
        abs_channels[i].is_active = false;
        abs_channels[i].enable = true;
        abs_channels[i].history_index = 0u;

        for (j = 0u; j < 4u; j++) {
            abs_channels[i].slip_history[j] = 0.0f;
            abs_channels[i].decel_history[j] = 0.0f;
        }
    }
}

/*============================================================================
 * 函数: ABS_LogicControl_10ms
 * 描述: 单通道ABS逻辑控制 - 10ms周期
 *============================================================================*/
void ABS_LogicControl_10ms(uint8_t channel_id, float slip_ratio, float wheel_decel)
{
    ABS_ChannelType *ch;

    if (channel_id >= ABS_NUM_CHANNELS) {
        return;
    }

    if (!abs_enabled || !abs_channels[channel_id].enable) {
        return;
    }

    ch = &abs_channels[channel_id];

    /* 更新历史数据 */
    ABS_UpdateHistory(ch, slip_ratio, wheel_decel);

    /* 状态机 */
    switch (ch->state) {
        /*==============================================================
         * 状态: INACTIVE - ABS未激活
         *==============================================================*/
        case ABS_STATE_INACTIVE:
            ch->timer = 0u;
            ch->cycle_count = 0u;

            /* 检查进入条件 */
            if (ABS_CheckEntryCondition(ch, slip_ratio, wheel_decel)) {
                ch->state = ABS_STATE_PRESSURE_HOLD;
                ch->is_active = true;
            }
            break;

        /*==============================================================
         * 状态: PRESSURE_HOLD - 保压
         *==============================================================*/
        case ABS_STATE_PRESSURE_HOLD:
            ch->timer += ABS_CYCLE_TIME_MS;

            /* 保压时间检查 */
            if (ch->timer >= abs_params.time_hold_min) {
                /* 根据滑移率和减速度决定下一步 */
                if (slip_ratio > abs_params.slip_threshold_high ||
                    wheel_decel < abs_params.decel_threshold_critical) {
                    /* 滑移率过高或减速度过大，需要减压 */
                    ch->state = ABS_STATE_PRESSURE_DECREASE;
                    ch->timer = 0u;
                }
                else if (slip_ratio < abs_params.slip_threshold_low &&
                         wheel_decel > abs_params.decel_threshold_enter) {
                    /* 滑移率降低，可以增压 */
                    ch->state = ABS_STATE_PRESSURE_INCREASE;
                    ch->timer = 0u;
                }
                /* 否则继续保压 */
            }

            /* 检查紧急减压条件 */
            if (wheel_decel < abs_params.decel_threshold_critical) {
                ch->state = ABS_STATE_PRESSURE_DECREASE_FAST;
                ch->timer = 0u;
            }
            break;

        /*==============================================================
         * 状态: PRESSURE_DECREASE - 正常减压
         *==============================================================*/
        case ABS_STATE_PRESSURE_DECREASE:
            ch->timer += ABS_CYCLE_TIME_MS;

            /* 最小减压时间检查 */
            if (ch->timer >= abs_params.time_dec_min) {
                /* 检查减压效果 */
                if (slip_ratio < abs_params.slip_threshold_enter ||
                    wheel_decel > abs_params.decel_threshold_enter) {
                    /* 条件改善，转为保压 */
                    ch->state = ABS_STATE_PRESSURE_HOLD;
                    ch->timer = 0u;
                    ch->cycle_count++;
                }
            }

            /* 长时间减压后的安全保护 */
            if (ch->timer > 200u) {
                ch->state = ABS_STATE_PRESSURE_HOLD;
                ch->timer = 0u;
            }
            break;

        /*==============================================================
         * 状态: PRESSURE_DECREASE_FAST - 紧急快速减压
         *==============================================================*/
        case ABS_STATE_PRESSURE_DECREASE_FAST:
            ch->timer += ABS_CYCLE_TIME_MS;

            /* 快速减压直到条件改善 */
            if (wheel_decel > abs_params.decel_threshold_enter + 2.0f) {
                ch->state = ABS_STATE_PRESSURE_HOLD;
                ch->timer = 0u;
                ch->cycle_count++;
            }

            /* 安全超时 */
            if (ch->timer > 100u) {
                ch->state = ABS_STATE_PRESSURE_HOLD;
                ch->timer = 0u;
            }
            break;

        /*==============================================================
         * 状态: PRESSURE_INCREASE - 增压
         *==============================================================*/
        case ABS_STATE_PRESSURE_INCREASE:
            ch->timer += ABS_CYCLE_TIME_MS;

            /* 检查是否需要停止增压 */
            if (slip_ratio > abs_params.slip_threshold_enter) {
                /* 滑移率再次升高，需要保压 */
                ch->state = ABS_STATE_PRESSURE_HOLD;
                ch->timer = 0u;
            }
            else if (ch->timer >= abs_params.time_inc_max) {
                /* 最大增压时间到，检查退出条件 */
                if (ABS_CheckExitCondition(ch, slip_ratio, wheel_decel)) {
                    ch->state = ABS_STATE_INACTIVE;
                    ch->is_active = false;
                }
                else {
                    ch->state = ABS_STATE_PRESSURE_HOLD;
                    ch->timer = 0u;
                }
            }
            break;

        default:
            ch->state = ABS_STATE_INACTIVE;
            break;
    }

    ch->last_state = ch->state;
}

/*============================================================================
 * 函数: ABS_UpdateHistory
 *============================================================================*/
static void ABS_UpdateHistory(ABS_ChannelType *ch, float slip, float decel)
{
    ch->slip_history[ch->history_index] = slip;
    ch->decel_history[ch->history_index] = decel;
    ch->history_index = (ch->history_index + 1u) & 0x03u;  /* 循环索引 0-3 */
}

/*============================================================================
 * 函数: ABS_CheckEntryCondition
 *============================================================================*/
static bool ABS_CheckEntryCondition(const ABS_ChannelType *ch, float slip, float decel)
{
    /* 滑移率条件 */
    bool slip_condition = (slip > abs_params.slip_threshold_enter);

    /* 减速度条件 */
    bool decel_condition = (decel < abs_params.decel_threshold_enter);

    /* 需要满足任一条件，且非已激活 */
    return (!ch->is_active) && (slip_condition || decel_condition);
}

/*============================================================================
 * 函数: ABS_CheckExitCondition
 *============================================================================*/
static bool ABS_CheckExitCondition(const ABS_ChannelType *ch, float slip, float decel)
{
    /* 滑移率和减速度都恢复正常 */
    bool slip_normal = (slip < abs_params.slip_threshold_exit);
    bool decel_normal = (decel > abs_params.decel_threshold_exit);

    /* 连续多个周期稳定 */
    return slip_normal && decel_normal && (ch->timer >= abs_params.time_stability);
}

/*============================================================================
 * 函数: ABS_GetValveOutputs
 *============================================================================*/
void ABS_GetValveOutputs(uint8_t channel_id, ABS_ValveOutputsType *outputs)
{
    const ABS_ChannelType *ch;

    if (channel_id >= ABS_NUM_CHANNELS || outputs == NULL) {
        return;
    }

    ch = &abs_channels[channel_id];

    switch (ch->state) {
        case ABS_STATE_INACTIVE:
            outputs->inlet_valve = VALVE_STATE_OPEN;    /* 常开 */
            outputs->outlet_valve = VALVE_STATE_CLOSE;  /* 常闭 */
            outputs->pump_motor = false;
            break;

        case ABS_STATE_PRESSURE_HOLD:
            outputs->inlet_valve = VALVE_STATE_CLOSE;
            outputs->outlet_valve = VALVE_STATE_CLOSE;
            outputs->pump_motor = (ch->cycle_count > 0u);  /* 循环中保持泵运行 */
            break;

        case ABS_STATE_PRESSURE_DECREASE:
        case ABS_STATE_PRESSURE_DECREASE_FAST:
            outputs->inlet_valve = VALVE_STATE_CLOSE;
            outputs->outlet_valve = VALVE_STATE_OPEN;
            outputs->pump_motor = true;
            break;

        case ABS_STATE_PRESSURE_INCREASE:
            outputs->inlet_valve = VALVE_STATE_OPEN;
            outputs->outlet_valve = VALVE_STATE_CLOSE;
            outputs->pump_motor = false;
            break;

        default:
            outputs->inlet_valve = VALVE_STATE_OPEN;
            outputs->outlet_valve = VALVE_STATE_CLOSE;
            outputs->pump_motor = false;
            break;
    }
}

/*============================================================================
 * 函数: ABS_SetParameters
 * 描述: 设置控制参数 (用于不同路面标定)
 *============================================================================*/
void ABS_SetParameters(const ABS_ParametersType *params)
{
    if (params != NULL) {
        abs_params = *params;
    }
}

/*============================================================================
 * 函数: ABS_IsActive
 *============================================================================*/
bool ABS_IsActive(uint8_t channel_id)
{
    if (channel_id < ABS_NUM_CHANNELS) {
        return abs_channels[channel_id].is_active;
    }
    return false;
}

/*============================================================================
 * 函数: ABS_GetState
 *============================================================================*/
ABS_ControlStateType ABS_GetState(uint8_t channel_id)
{
    if (channel_id < ABS_NUM_CHANNELS) {
        return abs_channels[channel_id].state;
    }
    return ABS_STATE_INACTIVE;
}
