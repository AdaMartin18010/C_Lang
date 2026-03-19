# 机器人案例研究 (Robot Case Studies)

> **层级定位**: 18_Robotics_Physical_Control > 08_Case_Studies
> **难度级别**: ⭐⭐⭐⭐⭐ L5
> **前置知识**: 机器人运动学、动力学、控制、嵌入式系统
> **参考标准**: ROS Industrial, ISO 10218

---

## 案例列表

1. [工业机械臂控制](./01_Industrial_Arm_Control.md)
2. [移动机器人导航](./02_Mobile_Robot_Navigation.md)
3. [无人机飞控系统](./03_Drone_Flight_Control.md)
4. [协作机器人安全](./04_Collaborative_Robot_Safety.md)

---

## 案例1: 六轴工业机械臂完整控制系统

### 系统架构

```
┌─────────────────────────────────────────────────────────────────┐
│                        上位机 (PC)                               │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────────┐  │
│  │   ROS2 MoveIt │  │ 轨迹规划     │  │  视觉检测 (YOLO)     │  │
│  │  运动规划    │  │              │  │                      │  │
│  └──────┬───────┘  └──────┬───────┘  └──────────┬───────────┘  │
│         └─────────────────┴─────────────────────┘              │
│                           │                                     │
│                    EtherCAT/CANopen                             │
└───────────────────────────┼─────────────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────────────┐
│                      运动控制器 (MCU)                            │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  实时控制循环 (1kHz)                                       │  │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────┐    │  │
│  │  │ 位置环 PID  │→│ 速度环 PI   │→│ 电流环 PI       │    │  │
│  │  │             │  │             │  │                 │    │  │
│  │  └─────────────┘  └─────────────┘  └────────┬────────┘    │  │
│  │                                              │              │  │
│  │  ┌───────────────────────────────────────────▼────────┐   │  │
│  │  │              FOC / 六步换相控制                      │   │  │
│  │  └────────────────────────────────────────────────────┘   │  │
│  └──────────────────────────────────────────────────────────┘  │
└───────────────────────────┬─────────────────────────────────────┘
                            │ PWM
┌───────────────────────────▼─────────────────────────────────────┐
│                      伺服驱动器 ×6                               │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  功率级: MOSFET/IGBT 逆变器  │  电流采样  │  编码器接口  │  │
│  └──────────────────────────────────────────────────────────┘  │
└───────────────────────────┬─────────────────────────────────────┘
                            │
                    ┌───────┴───────┐
                    ▼               ▼
            ┌───────────┐   ┌───────────┐
            │  无刷电机  │   │  减速器    │
            │  ×6       │   │  Harmonic │
            └───────────┘   └───────────┘
```

### C实现: 级联控制

```c
// 六轴机械臂控制器 - 完整实现
#define NUM_JOINTS 6
#define CONTROL_FREQ 1000  // 1kHz控制频率

typedef struct {
    // PID参数
    float pos_kp, pos_ki, pos_kd;
    float vel_kp, vel_ki;
    float cur_kp, cur_ki;

    // 状态
    float pos_error_integral;
    float vel_error_integral;
    float cur_error_integral;
    float prev_pos_error;

    // 限幅
    float max_velocity;    // rad/s
    float max_current;     // A
    float max_torque;      // Nm
} JointController;

typedef struct {
    float position;   // rad
    float velocity;   // rad/s
    float current;    // A
    float torque;     // Nm
} JointState;

typedef struct {
    JointController joints[NUM_JOINTS];
    JointState states[NUM_JOINTS];
    float target_positions[NUM_JOINTS];
    float pwm_outputs[NUM_JOINTS];

    uint32_t cycle_count;
    float dt;
} RobotArm;

// 初始化机械臂
void robot_arm_init(RobotArm* arm) {
    memset(arm, 0, sizeof(RobotArm));
    arm->dt = 1.0f / CONTROL_FREQ;

    // 各关节参数 (根据实际机械臂调整)
    for (int i = 0; i < NUM_JOINTS; i++) {
        JointController* ctrl = &arm->joints[i];

        // 位置环 (大惯量关节需要更保守的参数)
        float inertia_scale = (i < 3) ? 0.8f : 1.0f;
        ctrl->pos_kp = 100.0f * inertia_scale;
        ctrl->pos_ki = 5.0f;
        ctrl->pos_kd = 2.0f;

        // 速度环
        ctrl->vel_kp = 0.8f;
        ctrl->vel_ki = 0.1f;

        // 电流环 (电流到转矩的转换)
        ctrl->cur_kp = 2.0f;
        ctrl->cur_ki = 0.5f;

        // 限制
        ctrl->max_velocity = 3.14f;  // 180 deg/s
        ctrl->max_current = 10.0f;    // 10A
        ctrl->max_torque = 50.0f;     // 50Nm
    }
}

// 单关节级联控制
float joint_cascade_control(JointController* ctrl, JointState* state,
                            float target_pos, float dt) {
    // ========== 位置环 ==========
    float pos_error = target_pos - state->position;

    // 积分限幅 (抗饱和)
    ctrl->pos_error_integral += pos_error * dt;
    float integral_limit = 5.0f / (ctrl->pos_ki + 1e-6f);
    if (ctrl->pos_error_integral > integral_limit)
        ctrl->pos_error_integral = integral_limit;
    if (ctrl->pos_error_integral < -integral_limit)
        ctrl->pos_error_integral = -integral_limit;

    // 微分项
    float pos_derivative = (pos_error - ctrl->prev_pos_error) / dt;
    ctrl->prev_pos_error = pos_error;

    // 位置环输出 = 速度指令
    float vel_cmd = ctrl->pos_kp * pos_error +
                    ctrl->pos_ki * ctrl->pos_error_integral +
                    ctrl->pos_kd * pos_derivative;

    // 速度限幅
    if (vel_cmd > ctrl->max_velocity) vel_cmd = ctrl->max_velocity;
    if (vel_cmd < -ctrl->max_velocity) vel_cmd = -ctrl->max_velocity;

    // ========== 速度环 ==========
    float vel_error = vel_cmd - state->velocity;
    ctrl->vel_error_integral += vel_error * dt;

    // 积分限幅
    float vel_integral_limit = ctrl->max_current / (ctrl->vel_ki + 1e-6f);
    if (ctrl->vel_error_integral > vel_integral_limit)
        ctrl->vel_error_integral = vel_integral_limit;
    if (ctrl->vel_error_integral < -vel_integral_limit)
        ctrl->vel_error_integral = -vel_integral_limit;

    float cur_cmd = ctrl->vel_kp * vel_error +
                    ctrl->vel_ki * ctrl->vel_error_integral;

    // 电流限幅
    if (cur_cmd > ctrl->max_current) cur_cmd = ctrl->max_current;
    if (cur_cmd < -ctrl->max_current) cur_cmd = -ctrl->max_current;

    // ========== 电流环 ==========
    float cur_error = cur_cmd - state->current;
    ctrl->cur_error_integral += cur_error * dt;

    // 简单PI控制 (实际应使用FOC)
    float voltage = ctrl->cur_kp * cur_error +
                    ctrl->cur_ki * ctrl->cur_error_integral;

    // 转换为PWM占空比 (-1.0 ~ 1.0)
    float pwm = voltage / 24.0f;  // 假设24V母线
    if (pwm > 1.0f) pwm = 1.0f;
    if (pwm < -1.0f) pwm = -1.0f;

    return pwm;
}

// 完整控制循环
void robot_arm_control_loop(RobotArm* arm) {
    // 读取当前状态 (ADC读取编码器、电流传感器)
    for (int i = 0; i < NUM_JOINTS; i++) {
        arm->states[i].position = read_encoder(i);
        arm->states[i].velocity = read_velocity(i);
        arm->states[i].current = read_current(i);
    }

    // 计算各关节控制输出
    for (int i = 0; i < NUM_JOINTS; i++) {
        arm->pwm_outputs[i] = joint_cascade_control(
            &arm->joints[i],
            &arm->states[i],
            arm->target_positions[i],
            arm->dt
        );
    }

    // 输出PWM
    for (int i = 0; i < NUM_JOINTS; i++) {
        set_pwm(i, arm->pwm_outputs[i]);
    }

    arm->cycle_count++;
}
```

---

## 案例2: AGV移动机器人导航

### 差速底盘运动学

```c
// 差速底盘控制
#define WHEEL_BASE 0.5f   // 轮距 0.5m
#define WHEEL_RADIUS 0.1f // 轮子半径 0.1m

typedef struct {
    float x, y, theta;  // 位姿
    float v, omega;     // 线速度、角速度
} AGVPose;

// 速度指令 → 左右轮速度
void cmd_vel_to_wheel_speeds(float v, float omega,
                             float* left_speed, float* right_speed) {
    // v = (vl + vr) / 2
    // omega = (vr - vl) / L

    *left_speed = v - omega * WHEEL_BASE / 2.0f;
    *right_speed = v + omega * WHEEL_BASE / 2.0f;
}

// 轮速编码器 → 里程计
void update_odometry(AGVPose* pose, float left_delta, float right_delta, float dt) {
    // 计算位移
    float delta_s = (right_delta + left_delta) / 2.0f;
    float delta_theta = (right_delta - left_delta) / WHEEL_BASE;

    // 更新位姿 (使用Runge-Kutta近似)
    float delta_x = delta_s * cosf(pose->theta + delta_theta / 2.0f);
    float delta_y = delta_s * sinf(pose->theta + delta_theta / 2.0f);

    pose->x += delta_x;
    pose->y += delta_y;
    pose->theta += delta_theta;

    // 角度归一化
    while (pose->theta > M_PI) pose->theta -= 2 * M_PI;
    while (pose->theta < -M_PI) pose->theta += 2 * M_PI;

    // 计算速度
    pose->v = delta_s / dt;
    pose->omega = delta_theta / dt;
}

// PID轨迹跟踪
void trajectory_tracking(AGVPose* current, AGVPose* target,
                         float* cmd_v, float* cmd_omega) {
    // 位置误差 (目标坐标系下)
    float ex = target->x - current->x;
    float ey = target->y - current->y;

    // 旋转到机器人坐标系
    float cos_t = cosf(current->theta);
    float sin_t = sinf(current->theta);
    float ex_r = cos_t * ex + sin_t * ey;
    float ey_r = -sin_t * ex + cos_t * ey;

    // 航向误差
    float etheta = target->theta - current->theta;
    while (etheta > M_PI) etheta -= 2 * M_PI;
    while (etheta < -M_PI) etheta += 2 * M_PI;

    // PID控制
    static float integral_ex = 0, integral_ey = 0;
    integral_ex += ex_r * 0.01f;
    integral_ey += ey_r * 0.01f;

    float kp_v = 1.0f, ki_v = 0.1f;
    float kp_w = 2.0f, ki_w = 0.2f;

    *cmd_v = kp_v * ex_r + ki_v * integral_ex;
    *cmd_omega = kp_w * ey_r + ki_w * integral_ey + 0.5f * etheta;

    // 限幅
    if (*cmd_v > 1.0f) *cmd_v = 1.0f;
    if (*cmd_v < -1.0f) *cmd_v = -1.0f;
    if (*cmd_omega > 2.0f) *cmd_omega = 2.0f;
    if (*cmd_omega < -2.0f) *cmd_omega = -2.0f;
}
```

---

## 案例3: 四旋翼无人机飞控

```c
// 四旋翼PID控制器
#define MOTOR_NUM 4

typedef struct {
    float roll, pitch, yaw;      // 姿态角
    float roll_rate, pitch_rate, yaw_rate;  // 角速度
    float altitude;              // 高度
} DroneState;

// 级联姿态控制器
void attitude_controller(DroneState* state, DroneState* target,
                         float* motor_thrusts) {
    // 姿态环 (外环)
    static PID roll_pid = {5.0f, 0.0f, 0.5f, 0, 0, 0};
    static PID pitch_pid = {5.0f, 0.0f, 0.5f, 0, 0, 0};
    static PID yaw_pid = {3.0f, 0.0f, 0.3f, 0, 0, 0};

    float roll_rate_cmd = pid_update(&roll_pid, target->roll, state->roll, 0.001f);
    float pitch_rate_cmd = pid_update(&pitch_pid, target->pitch, state->pitch, 0.001f);
    float yaw_rate_cmd = pid_update(&yaw_pid, target->yaw, state->yaw, 0.001f);

    // 角速度环 (内环)
    static PID roll_rate_pid = {0.3f, 0.05f, 0.01f, 0, 0, 0};
    static PID pitch_rate_pid = {0.3f, 0.05f, 0.01f, 0, 0, 0};
    static PID yaw_rate_pid = {0.5f, 0.1f, 0.02f, 0, 0, 0};

    float roll_torque = pid_update(&roll_rate_pid, roll_rate_cmd, state->roll_rate, 0.001f);
    float pitch_torque = pid_update(&pitch_rate_pid, pitch_rate_cmd, state->pitch_rate, 0.001f);
    float yaw_torque = pid_update(&yaw_rate_pid, yaw_rate_cmd, state->yaw_rate, 0.001f);

    // 混控器 (X型四旋翼)
    // M1(前右) M2(后左) M3(前左) M4(后右)
    // 俯仰: M1,M3 vs M2,M4
    // 横滚: M1,M2 vs M3,M4
    // 偏航: M1,M4 vs M2,M3 (反向旋转)

    float base_thrust = target->altitude * 0.1f;  // 基础油门

    motor_thrusts[0] = base_thrust - pitch_torque - roll_torque + yaw_torque;
    motor_thrusts[1] = base_thrust + pitch_torque + roll_torque + yaw_torque;
    motor_thrusts[2] = base_thrust - pitch_torque + roll_torque - yaw_torque;
    motor_thrusts[3] = base_thrust + pitch_torque - roll_torque - yaw_torque;

    // 限幅 0-100%
    for (int i = 0; i < MOTOR_NUM; i++) {
        if (motor_thrusts[i] < 0) motor_thrusts[i] = 0;
        if (motor_thrusts[i] > 1.0f) motor_thrusts[i] = 1.0f;
    }
}
```

---

## 权威参考

- [1] ROS Industrial: <https://rosindustrial.org/>
- [2] ISO 10218 (工业机器人安全标准)
- [3] "Robotics, Vision and Control" by Peter Corke
- [4] PX4/ArduPilot开源飞控文档

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
> **内容深度**: L5 (研究级)
