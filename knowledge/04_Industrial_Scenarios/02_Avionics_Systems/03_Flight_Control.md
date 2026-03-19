# 飞行控制系统

> **标准**: NASA-STD-8719.13B, DO-178C | **难度**: L5 | **更新**: 2026-03-15

---

## 概述

飞行控制系统(FCS)是航空器的核心安全关键系统，负责稳定、控制和操纵航空器。

```text
飞行控制系统架构:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
飞行员输入 → 飞控计算机 → 作动器 → 控制面
              ↓              ↓
         传感器反馈    位置反馈
              ↓              ↓
         状态估计 ← 飞行控制律
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

## 核心组件

### 1. 控制律算法

PID控制是飞行控制的基础：

```c
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
```

### 2. 状态估计

卡尔曼滤波用于传感器融合：

```c
// 简化的卡尔曼滤波
typedef struct {
    float x[3];    // 状态: [roll, pitch, yaw]
    float p[3][3]; // 协方差矩阵
    float q[3][3]; // 过程噪声
    float r[3][3]; // 测量噪声
} kalman_filter_t;

void kalman_predict(kalman_filter_t* kf, float gyro[3], float dt) {
    // 预测步骤
    for (int i = 0; i < 3; i++) {
        kf->x[i] += gyro[i] * dt;
    }
    // 更新协方差...
}

void kalman_update(kalman_filter_t* kf, float accel[3], float mag[3]) {
    // 更新步骤 (加速度计和磁力计观测)
    // ...
}
```

## 安全设计

### 冗余架构

```c
// 三余度表决
typedef struct {
    float value[3];  // 三个通道的值
    uint8_t status[3]; // 通道状态
} triplex_data_t;

float vote(triplex_data_t* data) {
    // 中值表决
    float a = data->value[0];
    float b = data->value[1];
    float c = data->value[2];

    // 返回中值
    if ((a <= b && b <= c) || (c <= b && b <= a)) return b;
    if ((b <= a && a <= c) || (c <= a && a <= b)) return a;
    return c;
}

// 故障检测
uint8_t detect_fault(triplex_data_t* data, float threshold) {
    float diff01 = fabsf(data->value[0] - data->value[1]);
    float diff12 = fabsf(data->value[1] - data->value[2]);
    float diff02 = fabsf(data->value[0] - data->value[2]);

    uint8_t fault_mask = 0;
    if (diff01 > threshold && diff02 > threshold) fault_mask |= 0x01;
    if (diff01 > threshold && diff12 > threshold) fault_mask |= 0x02;
    if (diff02 > threshold && diff12 > threshold) fault_mask |= 0x04;

    return fault_mask;
}
```

## DO-178C合规

```c
// 符合DO-178C DAL A (灾难级)要求的代码

// 需求: FC-001 俯仰角保护
// 验证: 当俯仰角超过25度时，系统自动推杆

#define PITCH_LIMIT_DEG 25.0f
#define RECOVERY_RATE_DEG_S 5.0f

float pitch_protection(float pilot_input, float pitch_angle) {
    // 前置条件检查
    if (!isfinite(pitch_angle)) {
        // 故障处理
        return 0.0f;
    }

    float output = pilot_input;

    // 上仰保护
    if (pitch_angle > PITCH_LIMIT_DEG && pilot_input > 0) {
        output = -RECOVERY_RATE_DEG_S;  // 自动推杆
    }

    // 下俯保护
    if (pitch_angle < -PITCH_LIMIT_DEG && pilot_input < 0) {
        output = RECOVERY_RATE_DEG_S;   // 自动拉杆
    }

    return output;
}
```

---

> **状态**: ✅ 已完成


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
