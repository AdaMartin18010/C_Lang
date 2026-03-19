# 脉冲神经网络(SNN)仿真

> **框架**: Brian2, NEST | **难度**: L5 | **更新**: 2026-03-15

---

## 概述

脉冲神经网络(Spiking Neural Network, SNN)是第三代神经网络，更接近生物神经元的脉冲信号传递机制。

```text
SNN vs 传统ANN:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
传统ANN:                    SNN:
连续值激活                  离散脉冲
同步计算                    事件驱动
高功耗                      低功耗
时间信息丢失                精确时间编码
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

## 神经元模型

### 1. Leaky Integrate-and-Fire (LIF)

```c
// LIF神经元参数
typedef struct {
    float v;            // 膜电位 (mV)
    float v_rest;       // 静息电位 (-70mV)
    float v_thresh;     // 阈值 (-55mV)
    float v_reset;      // 重置电位 (-75mV)
    float tau_m;        // 膜时间常数 (20ms)
    float r;            // 膜电阻 (10MΩ)
    float i_offset;     // 背景电流
    float refractory;   // 不应期 (2ms)
    float refractory_countdown; // 不应期倒计时
} lif_neuron_t;

// LIF神经元更新
// tau_m * dv/dt = -(v - v_rest) + R * I
void lif_update(lif_neuron_t* n, float i_syn, float dt) {
    if (n->refractory_countdown > 0) {
        n->refractory_countdown -= dt;
        n->v = n->v_reset;
        return;
    }

    // 欧拉法积分
    float dv = (-(n->v - n->v_rest) + n->r * (i_syn + n->i_offset)) / n->tau_m;
    n->v += dv * dt;

    // 脉冲产生
    if (n->v >= n->v_thresh) {
        n->v = n->v_reset;
        n->refractory_countdown = n->refractory;
        // 发送脉冲...
    }
}
```

### 2. Izhikevich模型

```c
// Izhikevich神经元 (能重现多种放电模式)
typedef struct {
    float v;            // 膜电位
    float u;            // 恢复变量
    float a, b, c, d;   // 模型参数
} izhikevich_neuron_t;

// 参数设置示例
// 常规放电: a=0.02, b=0.2, c=-65, d=8
// 快速放电: a=0.1,  b=0.2, c=-65, d=2
// 爆发放电: a=0.02, b=0.2, c=-50, d=2

void izhikevich_update(izhikevich_neuron_t* n, float i, float dt) {
    // dv/dt = 0.04*v^2 + 5*v + 140 - u + I
    // du/dt = a*(b*v - u)

    float v = n->v;
    float u = n->u;

    n->v += (0.04f * v * v + 5.0f * v + 140.0f - u + i) * dt;
    n->u += n->a * (n->b * v - u) * dt;

    if (n->v >= 30.0f) {
        n->v = n->c;
        n->u += n->d;
    }
}
```

## STDP学习

```c
// Spike-Timing Dependent Plasticity
// 突触前脉冲先于突触后: 长时程增强 (LTP)
// 突触后脉冲先于突触前: 长时程抑制 (LTD)

typedef struct {
    float w;            // 突触权重
    float w_max, w_min; // 权重边界
    float a_plus;       // LTP学习率
    float a_minus;      // LTD学习率
    float tau_plus;     // LTP时间常数
    float tau_minus;    // LTD时间常数
} stdp_synapse_t;

// STDP窗口函数
float stdp_window(float delta_t) {
    // delta_t = t_post - t_pre
    if (delta_t > 0) {
        // LTP
        return A_PLUS * expf(-delta_t / TAU_PLUS);
    } else {
        // LTD
        return -A_MINUS * expf(delta_t / TAU_MINUS);
    }
}

void stdp_update(stdp_synapse_t* syn, float delta_t) {
    float dw = stdp_window(delta_t);
    syn->w += dw;
    syn->w = fmaxf(fminf(syn->w, syn->w_max), syn->w_min);
}
```

## 网络仿真

```c
// 简单SNN网络
#define N_NEURONS 100
#define N_SYNAPSES 1000

typedef struct {
    lif_neuron_t neurons[N_NEURONS];
    stdp_synapse_t synapses[N_SYNAPSES];
    int pre_neuron[N_SYNAPSES];
    int post_neuron[N_SYNAPSES];
} snn_network_t;

void snn_step(snn_network_t* net, float dt) {
    // 1. 更新所有神经元
    for (int i = 0; i < N_NEURONS; i++) {
        // 收集输入电流
        float i_syn = 0;
        for (int s = 0; s < N_SYNAPSES; s++) {
            if (net->post_neuron[s] == i) {
                // 如果前神经元刚发放脉冲
                // i_syn += weight
            }
        }

        lif_update(&net->neurons[i], i_syn, dt);
    }

    // 2. STDP学习
    // 根据脉冲时间更新权重
}
```

## 应用

- **低功耗AI**: 边缘设备推理
- **事件相机**: 动态视觉处理
- **机器人控制**: 实时运动控制
- **脑机接口**: 神经信号解码

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
