/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Avionics_Systems\03_Flight_Control.md
 * Line: 28
 * Language: c
 * Block ID: 5c78a567
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// PID控制器结构
typedef struct {
    float kp, ki, kd;      // 增益参数
    float integral;        // 积分项
    float prev_error;      // 上一误差
    float output_limit;    // 输出限幅
} pid_controller_t;

// PID计算
float pid_update(pid_controller_t* pid, float setpoint, float measured) {
    float error = setpoint - measured;

    // 比例项
    float p = pid->kp * error;

    // 积分项 (带抗饱和)
    pid->integral += error;
    pid->integral = fmaxf(fminf(pid->integral, pid->output_limit), -pid->output_limit);
    float i = pid->ki * pid->integral;

    // 微分项
    float d = pid->kd * (error - pid->prev_error);
    pid->prev_error = error;

    // 输出限幅
    float output = p + i + d;
    return fmaxf(fminf(output, pid->output_limit), -pid->output_limit);
}

// 俯仰通道控制示例
float pitch_control(pid_controller_t* pid,
                    float pitch_cmd,    // 俯仰指令 (度)
                    float pitch_rate,   // 俯仰角速度 (度/秒)
                    float pitch_angle) { // 当前俯仰角

    // 角速度内环
    float rate_cmd = pid_update(&pid[0], pitch_cmd, pitch_angle);
    float elevator_cmd = pid_update(&pid[1], rate_cmd, pitch_rate);

    return elevator_cmd;  // 升降舵指令
}
