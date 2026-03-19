# 机器人控制

> **层级定位**: 18_Robotics_Physical_Control > 04_Control

---

## 内容

- [PID控制](./01_PID_Advanced.md)
- [计算力矩控制](./02_Computed_Torque.md)
- [模型预测控制(MPC)](./03_MPC_Introduction.md)

---

## 控制方法

### PID控制

比例-积分-微分控制的实现与调参。

```c
typedef struct {
    float kp, ki, kd;
    float integral;
    float prev_error;
} PID_Controller;

float pid_update(PID_Controller* pid, float error, float dt);
```

---

> **最后更新**: 2026-03-21
