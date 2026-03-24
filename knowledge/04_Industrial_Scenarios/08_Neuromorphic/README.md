---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
﻿# 神经形态计算


---

## 📑 目录

- [神经形态计算](#神经形态计算)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [核心特点](#核心特点)
    - [1. 事件驱动（Event-Driven）](#1-事件驱动event-driven)
    - [2. 稀疏表示（Sparse Representation）](#2-稀疏表示sparse-representation)
  - [神经形态芯片](#神经形态芯片)
    - [Intel Loihi](#intel-loihi)
    - [IBM TrueNorth](#ibm-truenorth)
    - [清华大学天机芯](#清华大学天机芯)
  - [脉冲神经网络（SNN）仿真](#脉冲神经网络snn仿真)
    - [神经元模型](#神经元模型)
      - [Leaky Integrate-and-Fire (LIF)](#leaky-integrate-and-fire-lif)
      - [Izhikevich 模型](#izhikevich-模型)
    - [突触模型](#突触模型)
    - [网络仿真框架](#网络仿真框架)
  - [嵌入式神经形态](#嵌入式神经形态)
    - [低功耗实现](#低功耗实现)
    - [中断驱动实现](#中断驱动实现)
  - [应用案例](#应用案例)
    - [手势识别](#手势识别)
  - [参考资源](#参考资源)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 概述

神经形态计算（Neuromorphic Computing）是一种受生物神经系统启发的计算范式，通过模拟大脑神经元和突触的工作方式，实现高能效、自适应的智能计算。

## 核心特点

### 1. 事件驱动（Event-Driven）

与传统计算的时钟驱动不同，神经形态系统只在有输入事件时处理：

```c
// 事件驱动神经元模型
typedef struct {
    float membrane_potential;
    float threshold;
    float leak_rate;
    uint64_t last_spike_time;
    int refractory_period;
} Neuron;

// 处理输入事件
void process_event(Neuron *neuron, float input_current,
                   uint64_t current_time) {
    // 检查不应期
    if (current_time - neuron->last_spike_time <
        neuron->refractory_period) {
        return;
    }

    // 膜电位衰减（漏电流）
    uint64_t dt = current_time - neuron->last_spike_time;
    neuron->membrane_potential *= exp(-dt * neuron->leak_rate);

    // 累加输入电流
    neuron->membrane_potential += input_current;

    // 检查是否触发脉冲
    if (neuron->membrane_potential >= neuron->threshold) {
        // 发射脉冲
        emit_spike(neuron);
        neuron->membrane_potential = 0.0f;
        neuron->last_spike_time = current_time;
    }
}
```

### 2. 稀疏表示（Sparse Representation）

信息通过稀疏的脉冲序列编码：

```c
// 脉冲编码示例
typedef struct {
    uint64_t timestamp;
    int neuron_id;
    float amplitude;
} SpikeEvent;

// 率编码转脉冲序列
void rate_to_spikes(float rate, int duration_ms,
                    SpikeEvent *spikes, int *num_spikes) {
    float lambda = rate / 1000.0f; // 转换为每毫秒
    *num_spikes = 0;

    for (int t = 0; t < duration_ms; t++) {
        // 泊松过程生成脉冲
        float p = (float)rand() / RAND_MAX;
        if (p < lambda) {
            spikes[*num_spikes].timestamp = t;
            spikes[*num_spikes].neuron_id = 0;
            spikes[*num_spikes].amplitude = 1.0f;
            (*num_spikes)++;
        }
    }
}
```

## 神经形态芯片

### Intel Loihi

Intel Loihi 是第二代神经形态研究芯片：

```c
// Loihi 神经元配置示例
typedef struct {
    // 膜电位参数
    int v_decay;           // 膜电位衰减
    int v_th_mant;         // 阈值尾数
    int v_th_exp;          // 阈值指数

    // 脉冲延迟
    int ds_offset;         // 树突延迟偏移
    int dm_offset;         // 轴突延迟偏移

    // 不应期
    int refract_delay;

    // 突触连接
    int num_axons;
    int num_synapses;
} LoihiNeuronConfig;

// 配置 LIF 神经元
void configure_lif_neuron(LoihiNeuronConfig *cfg) {
    cfg->v_decay = 128;      // 衰减因子
    cfg->v_th_mant = 100;    // 阈值 = 100 << 6
    cfg->v_th_exp = 6;
    cfg->refract_delay = 2;
}
```

### IBM TrueNorth

TrueNorth 采用异步数字电路设计：

```c
// TrueNorth 核心配置
#define CORE_SIZE 256
#define NUM_NEURONS 256
#define NUM_AXONS 256

typedef struct {
    uint8_t synaptic_weights[NUM_AXONS];
    uint8_t synaptic_delays[NUM_AXONS];
    uint16_t neuron_params[16];  // 神经元参数表索引
} TrueNorthCore;

// 突触交叉开关
void compute_core(TrueNorthCore *core,
                  uint8_t axon_inputs[NUM_AXONS],
                  uint8_t neuron_outputs[NUM_NEURONS]) {
    for (int n = 0; n < NUM_NEURONS; n++) {
        int32_t sum = 0;
        for (int a = 0; a < NUM_AXONS; a++) {
            if (axon_inputs[a]) {
                sum += core->synaptic_weights[a];
            }
        }
        neuron_outputs[n] = (sum > THRESHOLD) ? 1 : 0;
    }
}
```

### 清华大学天机芯

天机芯支持神经网络和传统计算：

```c
// 天机芯混合计算架构
typedef enum {
    MODE_ANN,    // 人工神经网络模式
    MODE_SNN,    // 脉冲神经网络模式
    MODE_HYBRID  // 混合模式
} TianjicMode;

typedef struct {
    TianjicMode mode;
    union {
        // ANN 部分
        struct {
            float weights[16][16];
            float bias[16];
            int activation_type;
        } ann;

        // SNN 部分
        struct {
            int16_t weights[16][16];
            int16_t threshold;
            int8_t leak;
            int8_t delay;
        } snn;
    };
} TianjicCore;
```

## 脉冲神经网络（SNN）仿真

### 神经元模型

#### Leaky Integrate-and-Fire (LIF)

```c
// LIF 神经元仿真
typedef struct {
    float v;           // 膜电位
    float v_rest;      // 静息电位
    float v_reset;     // 重置电位
    float v_th;        // 阈值电位
    float tau_m;       // 膜时间常数
    float r;           // 膜电阻
    float i;           // 输入电流
    int spiked;        // 是否发放脉冲
} LIFNeuron;

void update_lif(LIFNeuron *n, float dt) {
    // 欧拉法求解微分方程
    // dv/dt = (v_rest - v + r*i) / tau_m
    float dv = (n->v_rest - n->v + n->r * n->i) / n->tau_m;
    n->v += dv * dt;

    // 检查脉冲
    if (n->v >= n->v_th) {
        n->spiked = 1;
        n->v = n->v_reset;
    } else {
        n->spiked = 0;
    }
}
```

#### Izhikevich 模型

```c
// Izhikevich 神经元模型
typedef struct {
    float v;           // 膜电位
    float u;           // 恢复变量
    float a, b, c, d;  // 模型参数
    int spiked;
} IzhikevichNeuron;

// 预定义神经元类型
void set_regular_spiking(IzhikevichNeuron *n) {
    n->a = 0.02f;
    n->b = 0.2f;
    n->c = -65.0f;
    n->d = 8.0f;
}

void set_intrinsically_bursting(IzhikevichNeuron *n) {
    n->a = 0.02f;
    n->b = 0.2f;
    n->c = -55.0f;
    n->d = 4.0f;
}

void update_izhikevich(IzhikevichNeuron *n, float i, float dt) {
    if (n->v >= 30.0f) {
        n->v = n->c;
        n->u += n->d;
        n->spiked = 1;
    } else {
        // dv/dt = 0.04v^2 + 5v + 140 - u + i
        float dv = 0.04f * n->v * n->v + 5.0f * n->v + 140.0f
                   - n->u + i;
        // du/dt = a(bv - u)
        float du = n->a * (n->b * n->v - n->u);

        n->v += dv * dt;
        n->u += du * dt;
        n->spiked = 0;
    }
}
```

### 突触模型

```c
// STDP (Spike-Timing-Dependent Plasticity)
typedef struct {
    float weight;
    float a_plus;      // 增强系数
    float a_minus;     // 抑制系数
    float tau_plus;    // 增强时间常数
    float tau_minus;   // 抑制时间常数
    float w_max;       // 最大权重
    float w_min;       // 最小权重
} STDPSynapse;

void apply_stdp(STDPSynapse *syn,
                int64_t pre_time, int64_t post_time) {
    int64_t dt = post_time - pre_time;
    float dw;

    if (dt > 0) {
        // 后神经元在预神经元之后发放 - 增强
        dw = syn->a_plus * exp(-dt / syn->tau_plus);
    } else {
        // 抑制
        dw = -syn->a_minus * exp(dt / syn->tau_minus);
    }

    syn->weight += dw;

    // 边界约束
    if (syn->weight > syn->w_max) syn->weight = syn->w_max;
    if (syn->weight < syn->w_min) syn->weight = syn->w_min;
}
```

### 网络仿真框架

```c
// 基于事件的 SNN 仿真器
typedef struct {
    int num_neurons;
    LIFNeuron *neurons;
    float **weights;

    // 事件队列
    SpikeEvent *event_queue;
    int queue_head;
    int queue_tail;
} SNN;

void simulate_step(SNN *net, float dt) {
    // 处理当前时刻的所有事件
    while (net->queue_head < net->queue_tail) {
        SpikeEvent *evt = &net->event_queue[net->queue_head];
        // 传播到连接的目标神经元
        for (int i = 0; i < net->num_neurons; i++) {
            if (net->weights[evt->neuron_id][i] > 0) {
                net->neurons[i].i += evt->amplitude *
                                     net->weights[evt->neuron_id][i];
            }
        }
        net->queue_head++;
    }

    // 更新所有神经元
    for (int i = 0; i < net->num_neurons; i++) {
        update_lif(&net->neurons[i], dt);
        if (net->neurons[i].spiked) {
            // 生成新事件
            SpikeEvent new_evt = {
                .timestamp = current_time,
                .neuron_id = i,
                .amplitude = 1.0f
            };
            net->event_queue[net->queue_tail++] = new_evt;
        }
    }
}
```

## 嵌入式神经形态

### 低功耗实现

```c
// 定点数神经元（适合 MCU）
typedef struct {
    int16_t v;         // Q8.8 定点数
    int16_t v_th;
    int16_t leak;      // 衰减系数
    int16_t i;
} FixedLIFNeuron;

// 定点数更新
void update_fixed_lif(FixedLIFNeuron *n) {
    // v = v - (v >> leak_bits) + i
    n->v -= (n->v >> 4);  // 假设 leak_bits = 4
    n->v += n->i;

    if (n->v >= n->v_th) {
        n->v = 0;
        // 触发脉冲
    }
}
```

### 中断驱动实现

```c
// 基于中断的脉冲处理
volatile uint8_t spike_flag = 0;
volatile int spike_neuron_id = -1;

// 定时器中断处理
void TIM_IRQHandler(void) {
    for (int i = 0; i < num_neurons; i++) {
        if (neurons[i].next_spike_time == current_tick) {
            spike_flag = 1;
            spike_neuron_id = i;
            // 触发输出
            GPIO_SetBits(SPIKE_OUT_PORT, (1 << i));
        }
    }
    current_tick++;
}

// 主循环
int main(void) {
    init_neurons();
    init_timer();

    while (1) {
        if (spike_flag) {
            process_spike(spike_neuron_id);
            spike_flag = 0;

            // 进入低功耗模式直到下一个事件
            __WFI();
        }
    }
}
```

## 应用案例

### 手势识别

```c
// 基于 DVS（动态视觉传感器）的手势识别
void process_dvs_event(int x, int y, int polarity,
                       uint64_t timestamp) {
    // 映射到输入层
    int input_id = y * DVS_WIDTH + x;

    // 生成脉冲事件
    SpikeEvent evt = {
        .timestamp = timestamp,
        .neuron_id = input_id,
        .amplitude = polarity ? 1.0f : -1.0f
    };

    inject_event(&snn, evt);
}

// 读出类
int classify_gesture(SNN *net) {
    int spike_counts[OUTPUT_CLASSES] = {0};

    // 统计各类输出神经元的脉冲数
    for (int i = 0; i < OUTPUT_CLASSES; i++) {
        spike_counts[i] = count_spikes(&net->neurons[OUTPUT_START + i]);
    }

    // 选择发放最多的类别
    int max_class = 0;
    for (int i = 1; i < OUTPUT_CLASSES; i++) {
        if (spike_counts[i] > spike_counts[max_class]) {
            max_class = i;
        }
    }
    return max_class;
}
```

## 参考资源

- [Neuromorphic Computing: From Materials to Systems Architecture](https://ieeexplore.ieee.org/)
- [Intel Loihi Research Community](https://neuromorphic.intel.com/)
- [Brian2 - SNN Simulator](https://briansimulator.org/)
- [Norse - Deep Learning with Spiking Neural Networks](https://github.com/electronicvisions/norse)


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
