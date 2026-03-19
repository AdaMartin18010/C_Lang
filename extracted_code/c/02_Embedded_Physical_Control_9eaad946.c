/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 1525
 * Language: c
 * Block ID: 9eaad946
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 完整示例: PID温度控制器
// 硬件: NTC热敏电阻 + 加热器(PWM控制)
// ============================================================

#include <stdint.h>
#include <stdbool.h>

// ========== 配置 ==========
#define TEMP_SETPOINT   50.0f   // 目标温度 50°C
#define TEMP_HYSTERESIS 2.0f    // 回差 2°C
#define PWM_MAX         1000    // PWM最大值
#define ADC_CHANNEL     0       // ADC通道

// PID参数
#define KP  10.0f
#define KI  0.5f
#define KD  1.0f

// ========== 数据结构 ==========
typedef struct {
    float setpoint;
    float input;
    float output;
    float integral;
    float prev_error;
    float kp, ki, kd;
} PID_Controller;

// ========== 函数实现 ==========

void pid_init(PID_Controller *pid, float kp, float ki, float kd) {
    pid->setpoint = 0;
    pid->input = 0;
    pid->output = 0;
    pid->integral = 0;
    pid->prev_error = 0;
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
}

float pid_compute(PID_Controller *pid, float input) {
    pid->input = input;
    float error = pid->setpoint - input;

    // 积分项 (带抗饱和)
    pid->integral += error;
    if (pid->integral > 1000) pid->integral = 1000;
    if (pid->integral < -1000) pid->integral = -1000;

    // 微分项
    float derivative = error - pid->prev_error;
    pid->prev_error = error;

    // 计算输出
    pid->output = pid->kp * error +
                  pid->ki * pid->integral +
                  pid->kd * derivative;

    // 限幅
    if (pid->output > PWM_MAX) pid->output = PWM_MAX;
    if (pid->output < 0) pid->output = 0;

    return pid->output;
}

// ADC读取温度 (NTC)
float read_temperature(void) {
    uint16_t adc = adc_read_average(ADC1, ADC_CHANNEL, 16);
    return ntc_adc_to_temp(adc, 4095);
}

// 设置加热器功率
void set_heater_power(uint16_t power) {
    pwm_set_duty(TIM1, power);
}

// ========== 主程序 ==========

int main(void) {
    // 初始化
    system_init();
    adc_init(ADC1);
    pwm_init_motor(TIM1, 1000);  // 1kHz PWM

    PID_Controller pid;
    pid_init(&pid, KP, KI, KD);
    pid.setpoint = TEMP_SETPOINT;

    // 看门狗
    iwdg_init(1000);  // 1秒超时

    while (1) {
        // 读取当前温度
        float temp = read_temperature();

        // PID计算
        float power = pid_compute(&pid, temp);

        // 输出到加热器
        set_heater_power((uint16_t)power);

        // 安全监控
        if (temp > 80.0f) {
            // 超温保护
            set_heater_power(0);
            log_fault(FAULT_OVER_TEMP, (uint32_t)(temp * 100));
        }

        // 喂狗
        iwdg_feed();

        // 100ms控制周期
        delay_ms(100);
    }
}
