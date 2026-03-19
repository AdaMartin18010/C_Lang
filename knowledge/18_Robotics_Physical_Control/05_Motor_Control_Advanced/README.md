# 高级电机控制

> **层级定位**: 18_Robotics_Physical_Control > 05_Motor_Control_Advanced
> **难度级别**: L5
> **前置知识**: 电机基础、控制理论、嵌入式C

---

## 目录

1. [FOC理论基础](./01_FOC_Theory.md)
2. [SVPWM实现](./02_SVPWM.md)
3. [无感FOC](./03_Sensorless_FOC.md)
4. [电机参数辨识](./04_Parameter_Identification.md)

---

## 高级控制技术

### 磁场定向控制 (FOC)

将三相电流分解为d-q轴，分别控制磁场和转矩。

```c
typedef struct {
    float id;  // d轴电流
    float iq;  // q轴电流
    float vd;  // d轴电压
    float vq;  // q轴电压
    float theta;  // 电角度
} FOC_State;

void foc_update(FOC_State* state, float i_a, float i_b, float i_c);
```

### 空间矢量PWM (SVPWM)

比正弦PWM更高效的调制方式。

```c
void svpwm_calculate(float vd, float vq, float vdc,
                     float* duty_a, float* duty_b, float* duty_c);
```

---

> **最后更新**: 2026-03-21


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
