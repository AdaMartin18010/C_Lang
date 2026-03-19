/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 2720
 * Language: c
 * Block ID: 48e52ef5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file pid_motor_controller.c
 * @brief 完整的 PID 直流电机控制器
 *
 * 功能特性：
 * - PID 速度闭环控制
 * - 编码器反馈
 * - PWM 驱动
 * - USB 串口调参
 * - 实时数据输出
 */

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// ============= 硬件引脚定义 =============
// 电机驱动 (L298N 或类似)
#define MOTOR_PWM_PIN       16  // ENA
#define MOTOR_DIR1_PIN      17  // IN1
#define MOTOR_DIR2_PIN      18  // IN2

// 编码器 (增量式)
#define ENCODER_A_PIN       19
#define ENCODER_B_PIN       20

// 状态 LED
#define LED_PIN             25

// ============= 配置常量 =============
#define PWM_FREQUENCY       20000       // 20kHz PWM
#define PWM_RESOLUTION      1000        // 1000 级分辨率
#define ENCODER_PPR         360         // 编码器每转脉冲数
#define SAMPLE_RATE_HZ      100         // 控制频率 100Hz
#define SAMPLE_PERIOD_US    (1000000 / SAMPLE_RATE_HZ)

// ============= 数据结构 =============

/**
 * @brief PID 控制器结构体
 */
typedef struct {
    float kp;           // 比例系数
    float ki;           // 积分系数
    float kd;           // 微分系数

    float setpoint;     // 目标值
    float input;        // 当前输入
    float output;       // 输出值

    float integral;     // 积分累积
    float last_error;   // 上次误差
    float last_input;   // 上次输入 (用于微分)

    float output_min;   // 输出限制最小值
    float output_max;   // 输出限制最大值

    float integral_min; // 积分限制最小值 (抗饱和)
    float integral_max; // 积分限制最大值

    bool enabled;       // 控制器使能
    uint32_t sample_time_us;  // 采样周期
} pid_controller_t;

/**
 * @brief 电机状态结构体
 */
typedef struct {
    // 编码器
    volatile int32_t encoder_count;
    volatile int32_t last_encoder_count;
    int32_t delta_counts;

    // 速度计算
    float speed_rpm;            // 当前转速 (RPM)
    float speed_filtered;       // 滤波后转速
    float speed_target_rpm;     // 目标转速

    // PWM 输出
    int16_t pwm_duty;           // -1000 到 +1000
    uint pwm_slice;
    uint pwm_channel;

    // 方向
    bool direction_forward;

    // 统计
    uint32_t total_pulses;
} motor_state_t;

/**
 * @brief 系统状态
 */
typedef struct {
    bool running;
    bool debug_mode;
    uint32_t loop_count;
    uint32_t last_print_time;
    float cpu_load_percent;
} system_state_t;

// ============= 全局变量 =============
pid_controller_t g_pid;
motor_state_t g_motor;
system_state_t g_system;

// ============= 函数声明 =============
void pid_init(pid_controller_t* pid, float kp, float ki, float kd);
float pid_compute(pid_controller_t* pid, float input);
void pid_set_setpoint(pid_controller_t* pid, float setpoint);
void pid_set_tunings(pid_controller_t* pid, float kp, float ki, float kd);
void pid_set_output_limits(pid_controller_t* pid, float min, float max);

void motor_init(void);
void motor_set_pwm(int16_t pwm);
void motor_update_speed(void);
void encoder_isr(uint gpio, uint32_t events);

void control_loop(void);
void timer_irq_handler(void);
void handle_serial_command(const char* cmd);
void print_status(void);

// ============= PID 控制器实现 =============

void pid_init(pid_controller_t* pid, float kp, float ki, float kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    pid->setpoint = 0;
    pid->input = 0;
    pid->output = 0;

    pid->integral = 0;
    pid->last_error = 0;
    pid->last_input = 0;

    pid->output_min = -PWM_RESOLUTION;
    pid->output_max = PWM_RESOLUTION;
    pid->integral_min = -PWM_RESOLUTION / 2;
    pid->integral_max = PWM_RESOLUTION / 2;

    pid->enabled = false;
    pid->sample_time_us = SAMPLE_PERIOD_US;
}

void pid_set_tunings(pid_controller_t* pid, float kp, float ki, float kd) {
    // 根据采样时间调整积分和微分系数
    float sample_time_sec = pid->sample_time_us / 1000000.0f;
    pid->kp = kp;
    pid->ki = ki * sample_time_sec;
    pid->kd = kd / sample_time_sec;
}

void pid_set_output_limits(pid_controller_t* pid, float min, float max) {
    pid->output_min = min;
    pid->output_max = max;

    // 限制积分
    if (pid->integral_max > max) pid->integral_max = max;
    if (pid->integral_min < min) pid->integral_min = min;
}

void pid_set_setpoint(pid_controller_t* pid, float setpoint) {
    pid->setpoint = setpoint;
}

float pid_compute(pid_controller_t* pid, float input) {
    if (!pid->enabled) return 0;

    pid->input = input;

    // 计算误差
    float error = pid->setpoint - input;

    // 比例项
    float p_term = pid->kp * error;

    // 积分项
    pid->integral += error;
    // 积分限幅 (抗饱和)
    if (pid->integral > pid->integral_max) pid->integral = pid->integral_max;
    if (pid->integral < pid->integral_min) pid->integral = pid->integral_min;
    float i_term = pid->ki * pid->integral;

    // 微分项 (使用输入变化而非误差变化，避免设定值突变引起的冲击)
    float d_term = pid->kd * (pid->last_input - input);
    pid->last_input = input;
    pid->last_error = error;

    // 计算输出
    float output = p_term + i_term + d_term;

    // 输出限幅
    if (output > pid->output_max) output = pid->output_max;
    if (output < pid->output_min) output = pid->output_min;

    pid->output = output;
    return output;
}

// ============= 电机控制实现 =============

void motor_init(void) {
    memset(&g_motor, 0, sizeof(g_motor));

    // 初始化方向引脚
    gpio_init(MOTOR_DIR1_PIN);
    gpio_init(MOTOR_DIR2_PIN);
    gpio_set_dir(MOTOR_DIR1_PIN, GPIO_OUT);
    gpio_set_dir(MOTOR_DIR2_PIN, GPIO_OUT);
    gpio_put(MOTOR_DIR1_PIN, 0);
    gpio_put(MOTOR_DIR2_PIN, 0);

    // 初始化 PWM
    gpio_set_function(MOTOR_PWM_PIN, GPIO_FUNC_PWM);
    g_motor.pwm_slice = pwm_gpio_to_slice_num(MOTOR_PWM_PIN);
    g_motor.pwm_channel = pwm_gpio_to_channel(MOTOR_PWM_PIN);

    pwm_config pwm_cfg = pwm_get_default_config();
    float clkdiv = (float)clock_get_hz(clk_sys) / (PWM_FREQUENCY * PWM_RESOLUTION);
    pwm_config_set_clkdiv(&pwm_cfg, clkdiv);
    pwm_config_set_wrap(&pwm_cfg, PWM_RESOLUTION - 1);
    pwm_init(g_motor.pwm_slice, &pwm_cfg, true);
    pwm_set_chan_level(g_motor.pwm_slice, g_motor.pwm_channel, 0);

    // 初始化编码器
    gpio_init(ENCODER_A_PIN);
    gpio_init(ENCODER_B_PIN);
    gpio_set_dir(ENCODER_A_PIN, GPIO_IN);
    gpio_set_dir(ENCODER_B_PIN, GPIO_IN);
    gpio_pull_up(ENCODER_A_PIN);
    gpio_pull_up(ENCODER_B_PIN);

    // 配置编码器中断
    gpio_set_irq_enabled_with_callback(
        ENCODER_A_PIN,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true,
        encoder_isr
    );
}

void motor_set_pwm(int16_t pwm) {
    // 限制范围
    if (pwm > PWM_RESOLUTION) pwm = PWM_RESOLUTION;
    if (pwm < -PWM_RESOLUTION) pwm = -PWM_RESOLUTION;

    g_motor.pwm_duty = pwm;

    if (pwm > 0) {
        // 正转
        gpio_put(MOTOR_DIR1_PIN, 1);
        gpio_put(MOTOR_DIR2_PIN, 0);
        pwm_set_chan_level(g_motor.pwm_slice, g_motor.pwm_channel, pwm);
        g_motor.direction_forward = true;
    } else if (pwm < 0) {
        // 反转
        gpio_put(MOTOR_DIR1_PIN, 0);
        gpio_put(MOTOR_DIR2_PIN, 1);
        pwm_set_chan_level(g_motor.pwm_slice, g_motor.pwm_channel, -pwm);
        g_motor.direction_forward = false;
    } else {
        // 停止
        gpio_put(MOTOR_DIR1_PIN, 0);
        gpio_put(MOTOR_DIR2_PIN, 0);
        pwm_set_chan_level(g_motor.pwm_slice, g_motor.pwm_channel, 0);
    }
}

void encoder_isr(uint gpio, uint32_t events) {
    (void)gpio;
    (void)events;

    bool a = gpio_get(ENCODER_A_PIN);
    bool b = gpio_get(ENCODER_B_PIN);

    // 判断方向
    if (a == b) {
        g_motor.encoder_count++;
    } else {
        g_motor.encoder_count--;
    }

    g_motor.total_pulses++;
}

void motor_update_speed(void) {
    // 计算脉冲变化
    int32_t current_count = g_motor.encoder_count;
    g_motor.delta_counts = current_count - g_motor.last_encoder_count;
    g_motor.last_encoder_count = current_count;

    // 计算 RPM
    // RPM = (delta_counts / PPR) * (60 * SAMPLE_RATE)
    float counts_per_sec = g_motor.delta_counts * SAMPLE_RATE_HZ;
    g_motor.speed_rpm = (counts_per_sec / ENCODER_PPR) * 60.0f;

    // 低通滤波
    float alpha = 0.3f;  // 滤波系数
    g_motor.speed_filtered = alpha * g_motor.speed_rpm +
                             (1 - alpha) * g_motor.speed_filtered;
}

// ============= 控制循环 =============

void control_loop(void) {
    // 更新速度
    motor_update_speed();

    // PID 计算
    float pid_output = pid_compute(&g_pid, g_motor.speed_filtered);

    // 设置电机 PWM
    motor_set_pwm((int16_t)pid_output);

    // 更新系统状态
    g_system.loop_count++;
}

void timer_irq_handler(void) {
    // 清除中断标志
    hw_clear_bits(&timer_hw->intr, 1u << 0);

    // 设置下一次中断
    uint64_t next_alarm = timer_hw->timerawl + SAMPLE_PERIOD_US;
    timer_hw->alarm[0] = (uint32_t)next_alarm;

    // 执行控制循环
    control_loop();
}

void init_control_timer(void) {
    // 配置定时器中断
    hw_set_bits(&timer_hw->inte, 1u << 0);
    irq_set_exclusive_handler(TIMER_IRQ_0, timer_irq_handler);
    irq_set_enabled(TIMER_IRQ_0, true);

    // 设置首次中断
    uint64_t next_alarm = timer_hw->timerawl + SAMPLE_PERIOD_US;
    timer_hw->alarm[0] = (uint32_t)next_alarm;
}

// ============= 串口通信 =============

void handle_serial_command(const char* cmd) {
    char buffer[64];
    strncpy(buffer, cmd, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    // 去除换行符
    char* newline = strchr(buffer, '\n');
    if (newline) *newline = '\0';
    newline = strchr(buffer, '\r');
    if (newline) *newline = '\0';

    // 解析命令
    char* token = strtok(buffer, " ");
    if (!token) return;

    if (strcmp(token, "set") == 0) {
        // set speed <rpm>
        token = strtok(NULL, " ");
        if (token && strcmp(token, "speed") == 0) {
            token = strtok(NULL, " ");
            if (token) {
                float speed = atof(token);
                g_motor.speed_target_rpm = speed;
                pid_set_setpoint(&g_pid, speed);
                printf("OK: 目标速度设置为 %.1f RPM\n", speed);
            }
        }
    }
    else if (strcmp(token, "tune") == 0) {
        // tune kp ki kd
        float kp = atof(strtok(NULL, " "));
        float ki = atof(strtok(NULL, " "));
        float kd = atof(strtok(NULL, " "));
        pid_set_tunings(&g_pid, kp, ki, kd);
        printf("OK: PID 参数设置为 Kp=%.4f Ki=%.4f Kd=%.4f\n", kp, ki, kd);
    }
    else if (strcmp(token, "start") == 0) {
        g_pid.enabled = true;
        printf("OK: PID 控制器已启动\n");
    }
    else if (strcmp(token, "stop") == 0) {
        g_pid.enabled = false;
        motor_set_pwm(0);
        printf("OK: PID 控制器已停止\n");
    }
    else if (strcmp(token, "reset") == 0) {
        g_pid.integral = 0;
        g_pid.last_error = 0;
        printf("OK: PID 积分已重置\n");
    }
    else if (strcmp(token, "debug") == 0) {
        g_system.debug_mode = !g_system.debug_mode;
        printf("OK: 调试模式 %s\n", g_system.debug_mode ? "开启" : "关闭");
    }
    else if (strcmp(token, "status") == 0) {
        print_status();
    }
    else if (strcmp(token, "help") == 0) {
        printf("\n=== PID 电机控制器命令 ===\n");
        printf("set speed <rpm>    - 设置目标速度\n");
        printf("tune <kp> <ki> <kd> - 设置 PID 参数\n");
        printf("start              - 启动控制器\n");
        printf("stop               - 停止控制器\n");
        printf("reset              - 重置积分\n");
        printf("debug              - 切换调试模式\n");
        printf("status             - 显示状态\n");
        printf("help               - 显示帮助\n\n");
    }
}

void print_status(void) {
    printf("\n=== 系统状态 ===\n");
    printf("目标速度: %.1f RPM\n", g_motor.speed_target_rpm);
    printf("当前速度: %.1f RPM (滤波: %.1f)\n",
           g_motor.speed_rpm, g_motor.speed_filtered);
    printf("PWM 输出: %d\n", g_motor.pwm_duty);
    printf("编码器: %ld\n", (long)g_motor.encoder_count);
    printf("PID: Kp=%.4f Ki=%.4f Kd=%.4f\n",
           g_pid.kp, g_pid.ki, g_pid.kd);
    printf("PID 输出: %.1f\n", g_pid.output);
    printf("积分: %.1f\n", g_pid.integral);
    printf("控制器状态: %s\n", g_pid.enabled ? "运行中" : "停止");
    printf("===============\n\n");
}

void debug_output(void) {
    // CSV 格式输出，便于绘图
    uint32_t now = time_us_32();
    printf("%u,%.1f,%.1f,%d,%.1f,%.1f\n",
           now,
           g_motor.speed_target_rpm,
           g_motor.speed_filtered,
           g_motor.pwm_duty,
           g_pid.output,
           g_pid.integral);
}

// ============= 主程序 =============

int main(void) {
    stdio_init_all();

    // 初始化状态 LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // 等待 USB 连接
    sleep_ms(2000);

    printf("\n");
    printf("╔═══════════════════════════════════════╗\n");
    printf("║     PID 直流电机控制器 v1.0           ║\n");
    printf("║     RP2040 @ 133MHz                   ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    printf("\n输入 'help' 查看命令列表\n\n");

    // 初始化系统
    memset(&g_system, 0, sizeof(g_system));
    g_system.running = true;

    // 初始化 PID 控制器
    pid_init(&g_pid, 2.0f, 0.5f, 0.1f);
    pid_set_output_limits(&g_pid, -PWM_RESOLUTION, PWM_RESOLUTION);

    // 初始化电机
    motor_init();

    // 启动定时器控制
    init_control_timer();

    // 主循环
    char cmd_buffer[64];
    int cmd_pos = 0;
    absolute_time_t last_debug_time = get_absolute_time();

    while (g_system.running) {
        // 处理串口输入
        int c = getchar_timeout_us(0);
        if (c != PICO_ERROR_TIMEOUT) {
            if (c == '\n' || c == '\r') {
                if (cmd_pos > 0) {
                    cmd_buffer[cmd_pos] = '\0';
                    handle_serial_command(cmd_buffer);
                    cmd_pos = 0;
                }
            } else if (cmd_pos < sizeof(cmd_buffer) - 1) {
                cmd_buffer[cmd_pos++] = (char)c;
            }
        }

        // 调试输出
        if (g_system.debug_mode) {
            if (absolute_time_diff_us(last_debug_time, get_absolute_time()) > 10000) {
                last_debug_time = get_absolute_time();
                debug_output();
            }
        }

        // 状态 LED 闪烁
        static absolute_time_t last_blink = {0};
        if (absolute_time_diff_us(last_blink, get_absolute_time()) > 500000) {
            last_blink = get_absolute_time();
            gpio_xor_mask(1u << LED_PIN);
        }
    }

    // 清理
    motor_set_pwm(0);
    printf("系统已停止\n");

    return 0;
}
