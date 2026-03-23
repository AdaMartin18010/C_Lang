# 脉冲神经网络(SNN)C语言模拟


---

## 📑 目录

- [脉冲神经网络(SNN)C语言模拟](#脉冲神经网络snnc语言模拟)
  - [📑 目录](#-目录)
  - [1. 脉冲神经网络概述](#1-脉冲神经网络概述)
    - [1.1 什么是SNN](#11-什么是snn)
    - [1.2 脉冲编码方式](#12-脉冲编码方式)
    - [1.3 突触与可塑性](#13-突触与可塑性)
  - [2. 神经元模型](#2-神经元模型)
    - [2.1 泄露积分发放模型(LIF)](#21-泄露积分发放模型lif)
    - [2.2 Izhikevich模型](#22-izhikevich模型)
  - [3. 突触可塑性(STDP)](#3-突触可塑性stdp)
    - [3.1 STDP原理](#31-stdp原理)
  - [4. SNN模拟器实现](#4-snn模拟器实现)
    - [4.1 网络结构定义](#41-网络结构定义)
  - [5. 模式识别示例](#5-模式识别示例)
    - [5.1 简单分类器](#51-简单分类器)
    - [5.2 时间编码示例](#52-时间编码示例)
  - [6. 总结](#6-总结)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 1. 脉冲神经网络概述

### 1.1 什么是SNN

脉冲神经网络(Spiking Neural Network, SNN)是第三代神经网络，更真实地模拟生物神经系统的信息处理方式。
与第二代人工神经网络(ANN)不同，SNN使用离散的脉冲(Spike)来传递信息，而非连续值。

**SNN的核心特点：**

| 特性 | ANN | SNN |
|------|-----|-----|
| 信号类型 | 连续实值 | 离散脉冲 |
| 时间维度 | 静态或离散时间步 | 连续时间 |
| 能耗效率 | 较低 | 高(仅在有脉冲时计算) |
| 生物真实性 | 低 | 高 |
| 硬件友好性 | 需要专用加速器 | 适合神经形态芯片 |

### 1.2 脉冲编码方式

**1. 频率编码 (Rate Coding)**:

- 信息编码在脉冲发放频率中
- 发放频率：$r = n/T$，其中n是脉冲数，T是时间窗口
- 简单但信息传输效率低

**2. 时间编码 (Temporal Coding)**:

- 信息编码在脉冲的精确时间中
- 首次脉冲时间(First-Spike-Time)：$t_{first}$
- 相位编码：脉冲相对于参考信号的时间
- 信息传输效率高

**3. 群体编码 (Population Coding)**:

- 信息分布在多个神经元的脉冲模式中
- 提高编码精度和鲁棒性

### 1.3 突触与可塑性

**突触传递模型：**

$$I_{syn}(t) = w \cdot \sum_{t_i^{pre}} \alpha(t - t_i^{pre})$$

其中$\alpha(t)$是突触后电流(PSP)波形，常用指数衰减：

$$\alpha(t) = (t/\tau)e^{1-t/\tau} \cdot H(t)$$

$H(t)$是Heaviside阶跃函数，$\tau$是时间常数。

---

## 2. 神经元模型

### 2.1 泄露积分发放模型(LIF)

LIF(Leaky Integrate-and-Fire)是最常用的简化神经元模型。

**膜电位动态：**

$$\tau_m \frac{dV}{dt} = -(V - V_{rest}) + R \cdot I_{syn}(t)$$

**发放条件：**
当$V \geq V_{th}$时，发放脉冲并重置：$V \leftarrow V_{reset}$

**C语言实现：**

```c
/* snn_lif.h - LIF神经元模型头文件 */
#ifndef SNN_LIF_H
#define SNN_LIF_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* LIF神经元参数 */
typedef struct {
    double tau_m;        /* 膜时间常数 (ms) */
    double v_rest;       /* 静息电位 (mV) */
    double v_reset;      /* 重置电位 (mV) */
    double v_thresh;     /* 发放阈值 (mV) */
    double r_membrane;   /* 膜电阻 (MΩ) */
    double tau_ref;      /* 不应期 (ms) */
} LIFParams;

/* LIF神经元状态 */
typedef struct {
    double v;            /* 当前膜电位 */
    double i_syn;        /* 突触电流 */
    double last_spike_time; /* 上次发放时间 */
    bool is_refractory;  /* 是否处于不应期 */
    uint32_t spike_count; /* 脉冲计数 */
} LIFNeuron;

void lif_init_params(LIFParams *params, double tau_m, double v_rest,
                      double v_reset, double v_thresh, double r_membrane,
                      double tau_ref);
void lif_neuron_init(LIFNeuron *neuron, const LIFParams *params);
bool lif_update(LIFNeuron *neuron, const LIFParams *params,
                double i_ext, double dt, double current_time);
void lif_receive_spike(LIFNeuron *neuron, double weight, double current_time);

#endif /* SNN_LIF_H */
```

```c
/* snn_lif.c - LIF模型实现 */
#include "snn_lif.h"

void lif_init_params(LIFParams *params, double tau_m, double v_rest,
                      double v_reset, double v_thresh, double r_membrane,
                      double tau_ref)
{
    params->tau_m = tau_m;
    params->v_rest = v_rest;
    params->v_reset = v_reset;
    params->v_thresh = v_thresh;
    params->r_membrane = r_membrane;
    params->tau_ref = tau_ref;
}

void lif_neuron_init(LIFNeuron *neuron, const LIFParams *params)
{
    neuron->v = params->v_rest;
    neuron->i_syn = 0.0;
    neuron->last_spike_time = -1e10;
    neuron->is_refractory = false;
    neuron->spike_count = 0;
}

void lif_receive_spike(LIFNeuron *neuron, double weight, double current_time)
{
    (void)current_time;
    neuron->i_syn += weight;
}

bool lif_update(LIFNeuron *neuron, const LIFParams *params,
                double i_ext, double dt, double current_time)
{
    if (neuron->is_refractory) {
        if (current_time - neuron->last_spike_time >= params->tau_ref) {
            neuron->is_refractory = false;
        } else {
            return false;
        }
    }

    double i_total = neuron->i_syn + i_ext;
    double dv = (-(neuron->v - params->v_rest) +
                  params->r_membrane * i_total) / params->tau_m;
    neuron->v += dv * dt;

    neuron->i_syn *= 0.9;

    if (neuron->v >= params->v_thresh) {
        neuron->v = params->v_reset;
        neuron->last_spike_time = current_time;
        neuron->is_refractory = true;
        neuron->spike_count++;
        return true;
    }

    return false;
}
```

### 2.2 Izhikevich模型

Izhikevich模型是一个二维动力系统，能够产生多种神经发放模式。

**动态方程：**

$$dv/dt = 0.04v^2 + 5v + 140 - u + I$$
$$du/dt = a(bv - u)$$

**发放条件：**
当$v \geq 30$ mV时，发放脉冲并重置：$v \leftarrow c$, $u \leftarrow u + d$

```c
/* snn_izhikevich.h - Izhikevich模型 */
#ifndef SNN_IZHIKEVICH_H
#define SNN_IZHIKEVICH_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    double a;
    double b;
    double c;
    double d;
} IzhikevichParams;

typedef struct {
    double v;
    double u;
    uint32_t spike_count;
} IzhikevichNeuron;

extern const IzhikevichParams IZ_REGULAR_SPIKING;
extern const IzhikevichParams IZ_INTRINSICALLY_BURSTING;
extern const IzhikevichParams IZ_CHATTERING;

void izhikevich_neuron_init(IzhikevichNeuron *neuron,
                             const IzhikevichParams *params);
bool izhikevich_update(IzhikevichNeuron *neuron,
                        const IzhikevichParams *params,
                        double i_ext, double dt);
#endif
```

```c
/* snn_izhikevich.c */
#include "snn_izhikevich.h"

const IzhikevichParams IZ_REGULAR_SPIKING = {0.02, 0.2, -65.0, 8.0};
const IzhikevichParams IZ_INTRINSICALLY_BURSTING = {0.02, 0.2, -55.0, 4.0};
const IzhikevichParams IZ_CHATTERING = {0.02, 0.2, -50.0, 2.0};

void izhikevich_neuron_init(IzhikevichNeuron *neuron,
                             const IzhikevichParams *params)
{
    neuron->v = -65.0;
    neuron->u = params->b * neuron->v;
    neuron->spike_count = 0;
}

bool izhikevich_update(IzhikevichNeuron *neuron,
                        const IzhikevichParams *params,
                        double i_ext, double dt)
{
    double dv = (0.04 * neuron->v * neuron->v + 5.0 * neuron->v +
                 140.0 - neuron->u + i_ext) * dt;
    double du = (params->a * (params->b * neuron->v - neuron->u)) * dt;

    neuron->v += dv;
    neuron->u += du;

    if (neuron->v >= 30.0) {
        neuron->v = params->c;
        neuron->u += params->d;
        neuron->spike_count++;
        return true;
    }
    return false;
}
```

---

## 3. 突触可塑性(STDP)

### 3.1 STDP原理

脉冲时间依赖可塑性(Spike-Timing-Dependent Plasticity)是生物突触的学习机制。

**STDP学习规则：**

$$\Delta w = \begin{cases}
A_+ \cdot e^{-\Delta t/\tau_+} & \text{if } \Delta t > 0 \\
-A_- \cdot e^{\Delta t/\tau_-} & \text{if } \Delta t < 0
\end{cases}$$

其中$\Delta t = t_{post} - t_{pre}$。

```c
/* snn_stdp.h - STDP学习规则 */
# ifndef SNN_STDP_H
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
# define SNN_STDP_H

# include <stdint.h>
# include <stdbool.h>

/* STDP参数 */
typedef struct {
    double A_plus;       /* LTP幅度 */
    double A_minus;      /* LTD幅度 */
    double tau_plus;     /* LTP时间常数 */
    double tau_minus;    /* LTD时间常数 */
    double w_min;        /* 最小权重 */
    double w_max;        /* 最大权重 */
    double learning_rate;/* 学习率 */
} STDPParams;

/* 突触连接 */
typedef struct {
    uint32_t pre_neuron;   /* 突触前神经元索引 */
    uint32_t post_neuron;  /* 突触后神经元索引 */
    double weight;         /* 突触权重 */
    double last_pre_time;  /* 上次突触前脉冲时间 */
    double last_post_time; /* 上次突触后脉冲时间 */
} Synapse;

/* STDP迹 */
typedef struct {
    double pre_trace;   /* 突触前迹 */
    double post_trace;  /* 突触后迹 */
} STDPtrace;

void stdp_init_params(STDPParams *params);
void stdp_init_synapse(Synapse *syn, uint32_t pre, uint32_t post, double w);
void stdp_pre_spike(Synapse *syn, STDPParams *params,
                     STDPtrace *trace, double current_time);
void stdp_post_spike(Synapse *syn, STDPParams *params,
                      STDPtrace *trace, double current_time);
void stdp_update_traces(STDPtrace *trace, STDPParams *params, double dt);

# endif /* SNN_STDP_H */
```

```c
/* snn_stdp.c - STDP实现 */
# include "snn_stdp.h"
# include <math.h>

void stdp_init_params(STDPParams *params)
{
    params->A_plus = 0.1;
    params->A_minus = 0.1;
    params->tau_plus = 20.0;   /* ms */
    params->tau_minus = 20.0;  /* ms */
    params->w_min = 0.0;
    params->w_max = 1.0;
    params->learning_rate = 0.01;
}

void stdp_init_synapse(Synapse *syn, uint32_t pre, uint32_t post, double w)
{
    syn->pre_neuron = pre;
    syn->post_neuron = post;
    syn->weight = w;
    syn->last_pre_time = -1e10;
    syn->last_post_time = -1e10;
}

void stdp_pre_spike(Synapse *syn, STDPParams *params,
                     STDPtrace *trace, double current_time)
{
    syn->last_pre_time = current_time;

    /* 计算时间差 */
    double dt = current_time - syn->last_post_time;

    if (dt > 0 && dt < 5 * params->tau_minus) {
        /* LTD: 突触后先于突触前发放 */
        double dw = -params->A_minus * exp(-dt / params->tau_minus);
        dw *= trace->post_trace;
        syn->weight += params->learning_rate * dw;
    }

    /* 边界限制 */
    if (syn->weight < params->w_min) syn->weight = params->w_min;
    if (syn->weight > params->w_max) syn->weight = params->w_max;

    /* 更新突触前迹 */
    trace->pre_trace += 1.0;
}

void stdp_post_spike(Synapse *syn, STDPParams *params,
                      STDPtrace *trace, double current_time)
{
    syn->last_post_time = current_time;

    /* 计算时间差 */
    double dt = current_time - syn->last_pre_time;

    if (dt > 0 && dt < 5 * params->tau_plus) {
        /* LTP: 突触前先于突触后发放 */
        double dw = params->A_plus * exp(-dt / params->tau_plus);
        dw *= trace->pre_trace;
        syn->weight += params->learning_rate * dw;
    }

    /* 边界限制 */
    if (syn->weight < params->w_min) syn->weight = params->w_min;
    if (syn->weight > params->w_max) syn->weight = params->w_max;

    /* 更新突触后迹 */
    trace->post_trace += 1.0;
}

void stdp_update_traces(STDPtrace *trace, STDPParams *params, double dt)
{
    trace->pre_trace *= exp(-dt / params->tau_plus);
    trace->post_trace *= exp(-dt / params->tau_minus);
}
```

---

## 4. SNN模拟器实现

### 4.1 网络结构定义

```c
/* snn_network.h - SNN网络定义 */
# ifndef SNN_NETWORK_H
# define SNN_NETWORK_H

# include "snn_lif.h"
# include "snn_stdp.h"
# include <stdint.h>
# include <stdbool.h>

# define MAX_NEURONS 10000
# define MAX_SYNAPSES 100000

typedef enum {
    LAYER_INPUT,
    LAYER_HIDDEN,
    LAYER_OUTPUT
} LayerType;

/* 网络层 */
typedef struct {
    LayerType type;
    uint32_t start_idx;
    uint32_t num_neurons;
    LIFParams neuron_params;
} Layer;

/* SNN网络 */
typedef struct {
    /* 神经元 */
    LIFNeuron neurons[MAX_NEURONS];
    uint32_t num_neurons;

    /* 层 */
    Layer layers[10];
    uint32_t num_layers;

    /* 突触连接 */
    Synapse synapses[MAX_SYNAPSES];
    STDPtrace traces[MAX_SYNAPSES];
    uint32_t num_synapses;

    /* 网络状态 */
    bool spikes[MAX_NEURONS];
    double current_time;
    double dt;

    /* STDP参数 */
    STDPParams stdp_params;
    bool enable_stdp;
} SNNNetwork;

void snn_init(SNNNetwork *net, double dt);
int snn_add_layer(SNNNetwork *net, LayerType type, uint32_t num_neurons,
                   const LIFParams *params);
void snn_connect_layers(SNNNetwork *net, uint32_t pre_layer, uint32_t post_layer,
                        double connection_prob, double weight_mean, double weight_std);
void snn_update(SNNNetwork *net);
void snn_apply_input(SNNNetwork *net, uint32_t layer_idx,
                      const double *input_currents);
void snn_get_output_spikes(SNNNetwork *net, uint32_t layer_idx,
                            bool *output_spikes);
void snn_get_output_rates(SNNNetwork *net, uint32_t layer_idx,
                           double *rates, double window_ms);

# endif /* SNN_NETWORK_H */
```

```c
/* snn_network.c - SNN网络实现 */
# include "snn_network.h"
# include <stdlib.h>
# include <math.h>
# include <string.h>

void snn_init(SNNNetwork *net, double dt)
{
    memset(net, 0, sizeof(SNNNetwork));
    net->dt = dt;
    net->current_time = 0.0;
    net->enable_stdp = false;
    stdp_init_params(&net->stdp_params);
}

int snn_add_layer(SNNNetwork *net, LayerType type, uint32_t num_neurons,
                   const LIFParams *params)
{
    if (net->num_layers >= 10) return -1;
    if (net->num_neurons + num_neurons > MAX_NEURONS) return -1;

    Layer *layer = &net->layers[net->num_layers];
    layer->type = type;
    layer->start_idx = net->num_neurons;
    layer->num_neurons = num_neurons;
    layer->neuron_params = *params;

    /* 初始化神经元 */
    for (uint32_t i = 0; i < num_neurons; i++) {
        lif_neuron_init(&net->neurons[net->num_neurons + i], params);
    }

    net->num_neurons += num_neurons;
    net->num_layers++;

    return (int)(net->num_layers - 1);
}

void snn_connect_layers(SNNNetwork *net, uint32_t pre_layer, uint32_t post_layer,
                        double connection_prob, double weight_mean, double weight_std)
{
    Layer *pre = &net->layers[pre_layer];
    Layer *post = &net->layers[post_layer];

    for (uint32_t i = 0; i < pre->num_neurons; i++) {
        for (uint32_t j = 0; j < post->num_neurons; j++) {
            if ((double)rand() / RAND_MAX < connection_prob) {
                if (net->num_synapses >= MAX_SYNAPSES) return;

                /* 生成高斯分布权重 */
                double u1 = (double)rand() / RAND_MAX;
                double u2 = (double)rand() / RAND_MAX;
                double z = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
                double weight = weight_mean + weight_std * z;

                stdp_init_synapse(&net->synapses[net->num_synapses],
                                  pre->start_idx + i,
                                  post->start_idx + j,
                                  weight);
                net->num_synapses++;
            }
        }
    }
}

void snn_update(SNNNetwork *net)
{
    /* 清除脉冲记录 */
    memset(net->spikes, 0, sizeof(bool) * net->num_neurons);

    /* 1. 更新所有神经元 */
    for (uint32_t i = 0; i < net->num_neurons; i++) {
        /* 找到神经元所在层 */
        Layer *layer = NULL;
        for (uint32_t l = 0; l < net->num_layers; l++) {
            if (i >= net->layers[l].start_idx &&
                i < net->layers[l].start_idx + net->layers[l].num_neurons) {
                layer = &net->layers[l];
                break;
            }
        }

        if (layer) {
            net->spikes[i] = lif_update(&net->neurons[i], &layer->neuron_params,
                                         0.0, net->dt, net->current_time);
        }
    }

    /* 2. 传递脉冲和处理STDP */
    for (uint32_t s = 0; s < net->num_synapses; s++) {
        Synapse *syn = &net->synapses[s];
        uint32_t pre_idx = syn->pre_neuron;
        uint32_t post_idx = syn->post_neuron;

        /* 突触前脉冲传递 */
        if (net->spikes[pre_idx]) {
            lif_receive_spike(&net->neurons[post_idx], syn->weight,
                               net->current_time);

            if (net->enable_stdp) {
                stdp_pre_spike(syn, &net->stdp_params,
                               &net->traces[s], net->current_time);
            }
        }

        /* 突触后脉冲STDP */
        if (net->enable_stdp && net->spikes[post_idx]) {
            stdp_post_spike(syn, &net->stdp_params,
                            &net->traces[s], net->current_time);
        }

        /* 更新迹 */
        if (net->enable_stdp) {
            stdp_update_traces(&net->traces[s], &net->stdp_params, net->dt);
        }
    }

    net->current_time += net->dt;
}

void snn_apply_input(SNNNetwork *net, uint32_t layer_idx,
                      const double *input_currents)
{
    Layer *layer = &net->layers[layer_idx];
    for (uint32_t i = 0; i < layer->num_neurons; i++) {
        net->neurons[layer->start_idx + i].i_syn += input_currents[i];
    }
}

void snn_get_output_spikes(SNNNetwork *net, uint32_t layer_idx,
                            bool *output_spikes)
{
    Layer *layer = &net->layers[layer_idx];
    for (uint32_t i = 0; i < layer->num_neurons; i++) {
        output_spikes[i] = net->spikes[layer->start_idx + i];
    }
}

void snn_get_output_rates(SNNNetwork *net, uint32_t layer_idx,
                           double *rates, double window_ms)
{
    Layer *layer = &net->layers[layer_idx];
    double window_factor = 1000.0 / window_ms; /* 转换为Hz */

    for (uint32_t i = 0; i < layer->num_neurons; i++) {
        rates[i] = net->neurons[layer->start_idx + i].spike_count * window_factor;
    }
}
```

---

## 5. 模式识别示例

### 5.1 简单分类器

```c
/* snn_classifier.c - SNN模式识别示例 */
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include "snn_network.h"

/* 编码：将输入值转换为脉冲序列 */
void encode_rate_coding(const double *input, uint32_t num_inputs,
                         double *currents, double gain)
{
    for (uint32_t i = 0; i < num_inputs; i++) {
        /* 输入值映射到注入电流 */
        currents[i] = input[i] * gain;
    }
}

/* 解码：基于发放率解码 */
uint32_t decode_rate_winner_take_all(const double *rates, uint32_t num_outputs)
{
    uint32_t max_idx = 0;
    double max_rate = rates[0];

    for (uint32_t i = 1; i < num_outputs; i++) {
        if (rates[i] > max_rate) {
            max_rate = rates[i];
            max_idx = i;
        }
    }

    return max_idx;
}

/* 训练SNN分类器 */
void train_snn_classifier(SNNNetwork *net,
                           const double **training_data,
                           const uint32_t *labels,
                           uint32_t num_samples,
                           uint32_t num_epochs)
{
    uint32_t input_layer = 0;
    uint32_t output_layer = net->num_layers - 1;
    Layer *in_layer = &net->layers[input_layer];

    double *input_currents = calloc(in_layer->num_neurons, sizeof(double));
    bool *output_spikes = calloc(net->layers[output_layer].num_neurons, sizeof(bool));

    net->enable_stdp = true;

    printf("开始训练...\n");

    for (uint32_t epoch = 0; epoch < num_epochs; epoch++) {
        uint32_t correct = 0;

        for (uint32_t sample = 0; sample < num_samples; sample++) {
            /* 编码输入 */
            encode_rate_coding(training_data[sample], in_layer->num_neurons,
                               input_currents, 20.0);

            /* 重置网络 */
            for (uint32_t i = 0; i < net->num_neurons; i++) {
                lif_neuron_init(&net->neurons[i],
                                &net->layers[0].neuron_params);
            }
            net->current_time = 0.0;

            /* 运行仿真500ms */
            for (double t = 0; t < 500.0; t += net->dt) {
                snn_apply_input(net, input_layer, input_currents);
                snn_update(net);
            }

            /* 检查输出层对应类别的脉冲 */
            snn_get_output_spikes(net, output_layer, output_spikes);

            /* 奖励调制STDP (简化版本) */
            uint32_t predicted = 0;
            uint32_t max_spikes = 0;
            for (uint32_t i = 0; i < net->layers[output_layer].num_neurons; i++) {
                uint32_t spike_count = net->neurons[net->layers[output_layer].start_idx + i].spike_count;
                if (spike_count > max_spikes) {
                    max_spikes = spike_count;
                    predicted = i;
                }
            }

            if (predicted == labels[sample]) {
                correct++;
            }
        }

        printf("Epoch %u: 准确率 = %.2f%%\n",
               epoch + 1, (double)correct / num_samples * 100);
    }

    net->enable_stdp = false;
    free(input_currents);
    free(output_spikes);
}

/* 测试SNN分类器 */
void test_snn_classifier(SNNNetwork *net,
                          const double **test_data,
                          const uint32_t *labels,
                          uint32_t num_samples)
{
    uint32_t input_layer = 0;
    uint32_t output_layer = net->num_layers - 1;
    Layer *in_layer = &net->layers[input_layer];

    double *input_currents = calloc(in_layer->num_neurons, sizeof(double));
    double *output_rates = calloc(net->layers[output_layer].num_neurons, sizeof(double));

    uint32_t correct = 0;

    printf("\n测试结果:\n");

    for (uint32_t sample = 0; sample < num_samples; sample++) {
        /* 编码输入 */
        encode_rate_coding(test_data[sample], in_layer->num_neurons,
                           input_currents, 20.0);

        /* 重置网络 */
        for (uint32_t i = 0; i < net->num_neurons; i++) {
            lif_neuron_init(&net->neurons[i], &net->layers[0].neuron_params);
        }
        net->current_time = 0.0;

        /* 运行仿真 */
        for (double t = 0; t < 500.0; t += net->dt) {
            snn_apply_input(net, input_layer, input_currents);
            snn_update(net);
        }

        /* 解码输出 */
        snn_get_output_rates(net, output_layer, output_rates, 500.0);
        uint32_t predicted = decode_rate_winner_take_all(output_rates,
                                                          net->layers[output_layer].num_neurons);

        printf("样本 %u: 预测=%u, 真实=%u", sample, predicted, labels[sample]);
        if (predicted == labels[sample]) {
            printf(" [正确]\n");
            correct++;
        } else {
            printf(" [错误]\n");
        }
    }

    printf("\n总准确率: %.2f%% (%u/%u)\n",
           (double)correct / num_samples * 100, correct, num_samples);

    free(input_currents);
    free(output_rates);
}

/* 主函数 */
int main(int argc, char *argv[])
{
    (void)argc; (void)argv;

    printf("=== SNN脉冲神经网络分类器示例 ===\n\n");

    /* 创建网络 */
    SNNNetwork net;
    snn_init(&net, 1.0);  /* 1ms时间步长 */

    /* 定义神经元参数 */
    LIFParams lif_params;
    lif_init_params(&lif_params, 20.0, -70.0, -70.0, -55.0, 10.0, 2.0);

    /* 添加层：输入(4) - 隐藏(10) - 输出(3) */
    int input_layer = snn_add_layer(&net, LAYER_INPUT, 4, &lif_params);
    int hidden_layer = snn_add_layer(&net, LAYER_HIDDEN, 10, &lif_params);
    int output_layer = snn_add_layer(&net, LAYER_OUTPUT, 3, &lif_params);

    printf("网络结构:\n");
    printf("  输入层: 4个神经元\n");
    printf("  隐藏层: 10个神经元\n");
    printf("  输出层: 3个神经元\n\n");

    /* 连接层 */
    snn_connect_layers(&net, (uint32_t)input_layer, (uint32_t)hidden_layer,
                       1.0, 0.5, 0.1);
    snn_connect_layers(&net, (uint32_t)hidden_layer, (uint32_t)output_layer,
                       1.0, 0.5, 0.1);

    printf("突触连接: %u\n\n", net.num_synapses);

    /* 示例数据 (简化版Iris数据集) */
    const double training_data[][4] = {
        {5.1, 3.5, 1.4, 0.2},  /* 类别0 */
        {4.9, 3.0, 1.4, 0.2},
        {4.7, 3.2, 1.3, 0.2},
        {7.0, 3.2, 4.7, 1.4},  /* 类别1 */
        {6.4, 3.2, 4.5, 1.5},
        {6.9, 3.1, 4.9, 1.5},
        {6.3, 3.3, 6.0, 2.5},  /* 类别2 */
        {5.8, 2.7, 5.1, 1.9},
        {7.1, 3.0, 5.9, 2.1},
    };

    const uint32_t labels[] = {0, 0, 0, 1, 1, 1, 2, 2, 2};
    const uint32_t num_samples = 9;

    /* 训练 */
    const double *data_ptrs[9];
    for (uint32_t i = 0; i < num_samples; i++) {
        data_ptrs[i] = training_data[i];
    }

    train_snn_classifier(&net, data_ptrs, labels, num_samples, 10);

    /* 测试 */
    test_snn_classifier(&net, data_ptrs, labels, num_samples);

    return 0;
}
```

### 5.2 时间编码示例

```c
/* temporal_coding.c - 时间编码示例 */
# include <stdio.h>
# include <math.h>

/* 首次脉冲时间编码 */
void encode_time_to_first_spike(double value, double min_val, double max_val,
                                 double *spike_time, double max_time)
{
    /* 归一化到0-1 */
    double normalized = (value - min_val) / (max_val - min_val);
    /* 反转：越大越早发放 */
    *spike_time = max_time * (1.0 - normalized);
}

/* 延迟学习 (Tempotron) */
void update_delays_tempotron(double *delays, const double *spike_times,
                              uint32_t num_inputs, double target_time,
                              double learning_rate, double threshold)
{
    /* 计算膜电位峰值时间 */
    double v_max = 0.0;
    double t_max = 0.0;

    for (uint32_t i = 0; i < num_inputs; i++) {
        if (spike_times[i] > 0 && spike_times[i] < target_time) {
            double v = 0.0;
            for (uint32_t j = 0; j < num_inputs; j++) {
                /* 简化PSP计算 */
                double dt = spike_times[i] - spike_times[j] - delays[j];
                if (dt > 0) {
                    v += exp(-dt / 10.0);
                }
            }
            if (v > v_max) {
                v_max = v;
                t_max = spike_times[i];
            }
        }
    }

    /* 更新延迟 */
    if (v_max >= threshold) {
        /* 应该发放但没有 - LTD */
        for (uint32_t i = 0; i < num_inputs; i++) {
            if (spike_times[i] > 0 && spike_times[i] < t_max) {
                delays[i] -= learning_rate;
            }
        }
    } else {
        /* 不应该发放但发放了 - LTP */
        for (uint32_t i = 0; i < num_inputs; i++) {
            if (spike_times[i] > 0 && spike_times[i] < target_time) {
                delays[i] += learning_rate;
            }
        }
    }
}
```

---

## 6. 总结

本文介绍了脉冲神经网络(SNN)的C语言实现：

1. **神经元模型**：LIF和Izhikevich模型的完整实现
2. **突触可塑性**：STDP学习规则的实现
3. **网络架构**：可扩展的分层网络设计
4. **编码解码**：频率编码和时间编码方法
5. **应用示例**：基于发放率的模式分类器

SNN的特点：
- **低功耗**：仅在脉冲发放时消耗能量
- **时序处理**：天然适合处理时序数据
- **生物真实**：更接近生物神经系统
- **硬件友好**：适合神经形态芯片实现

**扩展方向：**
- 实现更复杂的神经元模型(Hodgkin-Huxley)
- 添加突触延迟和动态突触
- 实现强化学习规则(R-STDP)
- 优化大规模网络性能


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
