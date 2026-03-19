/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\01_ABS_System.md
 * Line: 1848
 * Language: c
 * Block ID: 18d624f5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*============================================================================
 * MODULE:    ABS_PID_Control
 * FILE:      abs_pid_control.c
 * DESCRIPTION: 基于PID的ABS滑移率控制
 *============================================================================*/

typedef struct {
    float kp;                   /* 比例增益 */
    float ki;                   /* 积分增益 */
    float kd;                   /* 微分增益 */

    float error_integral;       /* 积分累积 */
    float error_prev;           /* 上次误差 */
    float error_prev2;          /* 上上次误差 */

    float output_max;           /* 输出上限 */
    float output_min;           /* 输出下限 */

    bool anti_windup;           /* 抗积分饱和使能 */
} ABS_PID_ControllerType;

/*============================================================================
 * 函数: ABS_PID_Init
 *============================================================================*/
void ABS_PID_Init(ABS_PID_ControllerType *pid, float kp, float ki, float kd)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->error_integral = 0.0f;
    pid->error_prev = 0.0f;
    pid->error_prev2 = 0.0f;
    pid->output_max = 100.0f;   /* 对应最大减压速率 */
    pid->output_min = -100.0f;  /* 对应最大增压速率 */
    pid->anti_windup = true;
}

/*============================================================================
 * 函数: ABS_PID_Update
 * 描述: PID更新 - 增量式实现 (抗饱和)
 * 返回: 控制输出 (-100 ~ +100，对应压力变化率)
 *============================================================================*/
float ABS_PID_Update(ABS_PID_ControllerType *pid, float setpoint, float measurement, float dt)
{
    float error;
    float p_term, i_term, d_term;
    float delta_output;
    float output;

    /* 计算误差 */
    error = setpoint - measurement;

    /* 比例项 */
    p_term = pid->kp * (error - pid->error_prev);

    /* 积分项 */
    i_term = pid->ki * dt * error;

    /* 微分项 */
    d_term = pid->kd * (error - 2.0f * pid->error_prev + pid->error_prev2) / dt;

    /* 增量输出 */
    delta_output = p_term + i_term + d_term;

    /* 积分限幅 (抗饱和) */
    if (pid->anti_windup) {
        float integral_max = pid->output_max / pid->ki;
        float integral_min = pid->output_min / pid->ki;

        pid->error_integral += error * dt;
        if (pid->error_integral > integral_max) {
            pid->error_integral = integral_max;
        } else if (pid->error_integral < integral_min) {
            pid->error_integral = integral_min;
        }
    }

    /* 更新历史 */
    pid->error_prev2 = pid->error_prev;
    pid->error_prev = error;

    /* 累积输出 */
    output = pid->error_integral * pid->ki + pid->kp * error;

    /* 输出限幅 */
    if (output > pid->output_max) {
        output = pid->output_max;
    } else if (output < pid->output_min) {
        output = pid->output_min;
    }

    return output;
}

/*============================================================================
 * 函数: ABS_PID_Control_10ms
 * 描述: 将PID输出转换为阀控制指令
 *============================================================================*/
void ABS_PID_Control_10ms(uint8_t channel_id, float slip_target, float slip_actual)
{
    static ABS_PID_ControllerType pid_controllers[4];
    float control_output;
    float dt = 0.01f;  /* 10ms */

    /* PID计算 */
    control_output = ABS_PID_Update(&pid_controllers[channel_id],
                                     slip_target, slip_actual, dt);

    /* 输出映射到阀控制 */
    /* control_output > 0: 需要减小压力 (减压) */
    /* control_output < 0: 需要增加压力 (增压) */
    /* |control_output| 大小决定阀开启时间比例 */

    if (control_output > 10.0f) {
        /* 减压 */
        SetValveState(channel_id, VALVE_DECREASE, control_output);
    } else if (control_output < -10.0f) {
        /* 增压 */
        SetValveState(channel_id, VALVE_INCREASE, -control_output);
    } else {
        /* 保压 */
        SetValveState(channel_id, VALVE_HOLD, 0.0f);
    }
}
