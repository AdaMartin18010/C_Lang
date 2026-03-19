# 机器人控制 (Robot Control)

> **层级定位**: 18_Robotics_Physical_Control > 04_Control
> **难度级别**: ⭐⭐⭐⭐⭐ L5
> **前置知识**: 控制理论、PID控制、机器人运动学、嵌入式C
> **参考标准**: IEEE Control Systems Society, IFAC

---

## 内容

- [PID控制](./01_PID_Advanced.md)
- [计算力矩控制](./02_Computed_Torque.md)
- [模型预测控制(MPC)](./03_MPC_Introduction.md)

---

## 控制方法

### PID控制原理与实现

比例-积分-微分控制是机器人控制的基础，其实现需考虑离散化、抗积分饱和、微分滤波等实际问题。

```c
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    // PID参数
    float kp, ki, kd;

    // 状态变量
    float integral;
    float prev_error;
    float prev_measurement;

    // 限幅设置
    float output_min, output_max;
    float integral_limit;

    // 采样时间
    float dt;

    // 滤波系数 (0-1, 越小滤波越强)
    float derivative_filter_alpha;
    float filtered_derivative;

    // 模式标志
    bool integral_windup_enabled;
} PID_Controller;

// 初始化PID控制器
void pid_init(PID_Controller* pid, float kp, float ki, float kd, float dt) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->dt = dt;

    pid->integral = 0;
    pid->prev_error = 0;
    pid->prev_measurement = 0;
    pid->filtered_derivative = 0;

    pid->output_min = -1000;
    pid->output_max = 1000;
    pid->integral_limit = 500;

    pid->derivative_filter_alpha = 0.1f;
    pid->integral_windup_enabled = true;
}

// PID更新 (带抗积分饱和和微分滤波)
float pid_update(PID_Controller* pid, float setpoint, float measurement) {
    // 计算误差
    float error = setpoint - measurement;

    // 比例项
    float p_term = pid->kp * error;

    // 积分项 (带抗饱和)
    pid->integral += error * pid->dt;

    if (pid->integral_windup_enabled) {
        // 积分限幅 (抗积分饱和)
        if (pid->integral > pid->integral_limit) {
            pid->integral = pid->integral_limit;
        } else if (pid->integral < -pid->integral_limit) {
            pid->integral = -pid->integral_limit;
        }
    }

    float i_term = pid->ki * pid->integral;

    // 微分项 (基于测量值的微分避免setpoint变化引起的冲击)
    float derivative = (measurement - pid->prev_measurement) / pid->dt;

    // 低通滤波微分项
    pid->filtered_derivative =
        pid->derivative_filter_alpha * derivative +
        (1 - pid->derivative_filter_alpha) * pid->filtered_derivative;

    float d_term = -pid->kd * pid->filtered_derivative;  // 负号因为derivative = -d(error)/dt

    // 计算输出
    float output = p_term + i_term + d_term;

    // 输出限幅
    if (output > pid->output_max) {
        output = pid->output_max;
        // 条件积分: 只在有利于减小积分时积分
        if (error > 0) {
            pid->integral -= error * pid->dt;
        }
    } else if (output < pid->output_min) {
        output = pid->output_min;
        if (error < 0) {
            pid->integral -= error * pid->dt;
        }
    }

    // 保存状态
    pid->prev_error = error;
    pid->prev_measurement = measurement;

    return output;
}

// 带前馈的PID控制
float pid_update_with_feedforward(
    PID_Controller* pid,
    float setpoint,
    float measurement,
    float velocity_ff,      // 速度前馈
    float acceleration_ff,  // 加速度前馈
    float ff_gain_v,        // 速度前馈增益
    float ff_gain_a         // 加速度前馈增益
) {
    float pid_output = pid_update(pid, setpoint, measurement);
    float feedforward = ff_gain_v * velocity_ff + ff_gain_a * acceleration_ff;

    return pid_output + feedforward;
}
```

---

## 深入理解

### 1. 机器人PID控制的特殊考量

#### 级联PID控制 (Cascade PID)

机器人关节控制通常采用位置-速度-电流三级级联：

```c
typedef struct {
    PID_Controller position_pid;
    PID_Controller velocity_pid;
    PID_Controller current_pid;

    float max_velocity;
    float max_current;
} CascadeController;

// 级联控制更新
float cascade_update(
    CascadeController* ctrl,
    float position_setpoint,
    float current_position,
    float current_velocity,
    float current_current
) {
    // 位置环 -> 速度指令
    float velocity_cmd = pid_update(&ctrl->position_pid, position_setpoint, current_position);

    // 速度限幅
    if (velocity_cmd > ctrl->max_velocity) velocity_cmd = ctrl->max_velocity;
    if (velocity_cmd < -ctrl->max_velocity) velocity_cmd = -ctrl->max_velocity;

    // 速度环 -> 电流指令
    float current_cmd = pid_update(&ctrl->velocity_pid, velocity_cmd, current_velocity);

    // 电流限幅
    if (current_cmd > ctrl->max_current) current_cmd = ctrl->max_current;
    if (current_cmd < -ctrl->max_current) current_cmd = -ctrl->max_current;

    // 电流环 -> 电压输出 (PWM占空比)
    float voltage = pid_update(&ctrl->current_pid, current_cmd, current_current);

    return voltage;
}
```

### 2. 多轴协调控制

#### 轨迹跟踪控制

```c
typedef struct {
    float position;
    float velocity;
    float acceleration;
} TrajectoryPoint;

// S曲线轨迹规划
void scurve_trajectory(
    float q0, float q1,        // 起始和目标位置
    float v_max, float a_max, float j_max,  // 最大速度/加速度/加加速度
    float t,                   // 当前时间
    TrajectoryPoint* out
) {
    float dq = q1 - q0;
    float dt = sqrt(fabs(dq) / a_max);  // 简化计算

    if (t < dt) {
        // 加速段
        out->acceleration = a_max;
        out->velocity = a_max * t;
        out->position = q0 + 0.5 * a_max * t * t;
    } else if (t < 2*dt) {
        // 减速段
        float t_dec = t - dt;
        out->acceleration = -a_max;
        out->velocity = a_max * dt - a_max * t_dec;
        out->position = q0 + 0.5 * a_max * dt * dt +
                        a_max * dt * t_dec - 0.5 * a_max * t_dec * t_dec;
    } else {
        // 到达目标
        out->position = q1;
        out->velocity = 0;
        out->acceleration = 0;
    }
}

// 多轴同步控制
void multi_axis_synchronized_control(
    int num_axes,
    PID_Controller* pids,
    TrajectoryPoint* trajectories,
    float* current_positions,
    float* outputs
) {
    for (int i = 0; i < num_axes; i++) {
        // 前馈 + 反馈控制
        float ff = trajectories[i].acceleration * 0.01f;  // 前馈增益
        float fb = pid_update(&pids[i],
                              trajectories[i].position,
                              current_positions[i]);

        outputs[i] = ff + fb;
    }
}
```

### 3. 实际应用案例

#### 案例1: 机械臂关节位置控制

```c
// 6轴机械臂关节控制
#define NUM_JOINTS 6

typedef struct {
    float position;
    float velocity;
    float current;
} JointState;

typedef struct {
    CascadeController controllers[NUM_JOINTS];
    JointState states[NUM_JOINTS];
    float pwm_outputs[NUM_JOINTS];
} RobotArmController;

// 初始化机械臂控制器
void robot_arm_init(RobotArmController* arm) {
    // 根据各关节特性调整PID参数
    // 大惯量关节需要更小的Kp和更大的Ki
    for (int i = 0; i < NUM_JOINTS; i++) {
        float kp = (i < 3) ? 50.0f : 30.0f;   // 基座关节惯量大
        float ki = (i < 3) ? 2.0f : 1.0f;
        float kd = (i < 3) ? 5.0f : 3.0f;

        pid_init(&arm->controllers[i].position_pid, kp, ki, kd, 0.001f);
        pid_init(&arm->controllers[i].velocity_pid, 0.5f, 0.1f, 0.0f, 0.001f);
        pid_init(&arm->controllers[i].current_pid, 0.1f, 0.01f, 0.0f, 0.001f);

        arm->controllers[i].max_velocity = 3.0f;  // rad/s
        arm->controllers[i].max_current = 10.0f;  // A
    }
}

// 实时控制循环 (1kHz)
void robot_arm_control_loop(RobotArmController* arm, float* target_positions) {
    // 读取当前状态 (ADC读取编码器、电流传感器)
    for (int i = 0; i < NUM_JOINTS; i++) {
        arm->states[i].position = read_encoder(i);
        arm->states[i].velocity = read_velocity(i);
        arm->states[i].current = read_current(i);
    }

    // 计算各关节控制输出
    for (int i = 0; i < NUM_JOINTS; i++) {
        arm->pwm_outputs[i] = cascade_update(
            &arm->controllers[i],
            target_positions[i],
            arm->states[i].position,
            arm->states[i].velocity,
            arm->states[i].current
        );
    }

    // 输出PWM
    for (int i = 0; i < NUM_JOINTS; i++) {
        set_pwm(i, arm->pwm_outputs[i]);
    }
}
```

#### 案例2: 机器人路径跟踪 (基于WPILib的FRC机器人)

```c
// FRC机器人底盘路径跟踪控制
// 参考: WPILib Robotics Library

#include <math.h>

typedef struct {
    float x, y, theta;  // 位姿
} Pose2d;

typedef struct {
    float left_speed;
    float right_speed;
} DifferentialDriveOutput;

// RAMSETE控制器 (非线性轨迹跟踪)
typedef struct {
    float zeta;    // 阻尼系数 (0 < zeta < 1)
    float b;       // 比例系数
} RamseteController;

void ramsete_controller_init(RamseteController* controller, float zeta, float b) {
    controller->zeta = zeta;
    controller->b = b;
}

// RAMSETE控制律计算
void ramsete_calculate(
    RamseteController* controller,
    Pose2d* current_pose,
    Pose2d* desired_pose,
    float desired_linear_velocity,
    float desired_angular_velocity,
    DifferentialDriveOutput* output
) {
    // 计算位姿误差 (在目标坐标系下)
    float ex = desired_pose->x - current_pose->x;
    float ey = desired_pose->y - current_pose->y;

    // 旋转误差到目标坐标系
    float cos_theta = cos(desired_pose->theta);
    float sin_theta = sin(desired_pose->theta);

    float ex_local =  cos_theta * ex + sin_theta * ey;
    float ey_local = -sin_theta * ex + cos_theta * ey;
    float etheta = desired_pose->theta - current_pose->theta;

    // 归一化角度
    while (etheta > M_PI) etheta -= 2*M_PI;
    while (etheta < -M_PI) etheta += 2*M_PI;

    // 计算增益
    float k = 2.0f * controller->zeta *
              sqrt(desired_angular_velocity*desired_angular_velocity +
                   controller->b * desired_linear_velocity*desired_linear_velocity);

    // 计算输出速度
    float v = desired_linear_velocity * cos(etheta) + k * ex_local;
    float omega = desired_angular_velocity + k * etheta +
                  (controller->b * desired_linear_velocity * sin(etheta) * ey_local) / etheta;

    // 转换为左右轮速
    float track_width = 0.6;  // 轮距 (米)
    output->left_speed = v - omega * track_width / 2.0f;
    output->right_speed = v + omega * track_width / 2.0f;
}

// PID路径修正
void pid_path_correction(
    PID_Controller* lateral_pid,
    PID_Controller* heading_pid,
    float lateral_error,    // 横向偏差
    float heading_error,    // 航向偏差
    float base_speed,
    DifferentialDriveOutput* output
) {
    float lateral_correction = pid_update(lateral_pid, 0, lateral_error);
    float heading_correction = pid_update(heading_pid, 0, heading_error);

    output->left_speed = base_speed - lateral_correction - heading_correction;
    output->right_speed = base_speed + lateral_correction + heading_correction;
}
```

### 4. 高级控制技术

#### 自适应PID控制

```c
typedef struct {
    PID_Controller base;

    // 自适应参数
    float performance_index;
    float adaptation_rate;

    // 历史性能记录
    float error_history[100];
    int history_index;
} AdaptivePID;

// 基于性能的PID参数自适应调整
void adaptive_pid_tune(AdaptivePID* apid) {
    // 计算性能指标 (ITAE: 时间乘以绝对误差积分)
    float itae = 0;
    for (int i = 0; i < 100; i++) {
        itae += fabs(apid->error_history[i]) * i;
    }

    apid->performance_index = itae;

    // 根据性能调整参数
    if (itae > 1000) {
        // 性能差，增大Kp
        apid->base.kp *= (1 + apid->adaptation_rate);
    } else if (itae < 100) {
        // 性能好，适当减小Kp以减小超调
        apid->base.kp *= (1 - apid->adaptation_rate * 0.5f);
    }

    // 限制参数范围
    if (apid->base.kp > 200) apid->base.kp = 200;
    if (apid->base.kp < 1) apid->base.kp = 1;
}
```

---

## 最佳实践

1. **参数整定方法**:
   - Ziegler-Nichols方法: 纯比例测试找到临界增益
   - 基于模型的整定: 使用系统辨识获取参数
   - 在线自整定: 继电反馈测试

2. **抗干扰设计**:
   - 使用基于测量的微分项避免setpoint冲击
   - 积分分离: 大误差时不积分
   - 输出滤波: 减小高频噪声

3. **实时性保证**:
   - 控制周期必须严格定时
   - 使用浮点单元(FPU)加速计算
   - 关键计算使用定点数优化

4. **安全机制**:
   - 位置/速度/电流多级限幅
   - 故障检测和急停
   - 软限位和硬限位

---

## 权威参考

- [1] Franklin, G.F. et al. "Feedback Control of Dynamic Systems", Pearson, 2019
- [2] Åström, K.J. & Hägglund, T. "PID Controllers: Theory, Design, and Tuning", ISA, 1995
- [3] WPILib Documentation: PID Control for Robotics
- [4] Khalil, H.K. "Nonlinear Systems", Pearson, 2014

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
> **内容深度**: L5 (研究级)
