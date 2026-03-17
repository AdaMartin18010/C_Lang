# 嵌入式AI的安全性与可靠性标准

> **文档状态**: 🆕 重要补充
> **创建日期**: 2026-03-18
> **适用标准**: ISO 26262, IEC 61508, ISO/IEC 24028 (AI可信度)

---

## 概述

在自动驾驶、医疗设备、工业控制等安全关键领域部署AI，必须满足严格的功能安全标准。本节介绍如何将嵌入式AI系统认证到工业安全标准。

### AI系统的特殊安全挑战

| 挑战 | 传统软件 | AI系统 | 应对方法 |
|:-----|:---------|:-------|:---------|
| **确定性** | 可预测 | 概率性 | 确定性AI、置信度阈值 |
| **可解释性** | 代码逻辑 | 黑盒模型 | XAI技术、决策日志 |
| **鲁棒性** | 输入验证 | 对抗攻击 | 对抗训练、输入滤波 |
| **数据质量** | N/A | 依赖训练数据 | 数据验证、MLOps |
| **验证方法** | 单元测试 | 统计测试 | 形式化验证、场景覆盖 |

---


---

## 📑 目录

- [嵌入式AI的安全性与可靠性标准](#嵌入式ai的安全性与可靠性标准)
  - [概述](#概述)
    - [AI系统的特殊安全挑战](#ai系统的特殊安全挑战)
  - [📑 目录](#-目录)
  - [功能安全标准映射](#功能安全标准映射)
    - [ISO 26262 (汽车功能安全)](#iso-26262-汽车功能安全)
    - [AI组件的安全生命周期](#ai组件的安全生命周期)
  - [C语言AI安全运行时](#c语言ai安全运行时)
    - [1. 安全监控框架](#1-安全监控框架)
    - [2. 实现](#2-实现)
  - [AI形式化验证](#ai形式化验证)
    - [神经网络验证框架](#神经网络验证框架)
    - [区间传播实现](#区间传播实现)
  - [对抗攻击防护](#对抗攻击防护)
    - [输入验证与滤波](#输入验证与滤波)
  - [认证检查清单](#认证检查清单)
    - [ISO 26262认证要求](#iso-26262认证要求)
  - [参考资源](#参考资源)


---

## 功能安全标准映射

### ISO 26262 (汽车功能安全)

```
┌─────────────────────────────────────────────────────────────┐
│              ISO 26262 ASIL等级与AI应用                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ASIL D (最高)  ←  自动驾驶决策、紧急制动                    │
│     │                                                       │
│  ASIL C        ←  车道保持、自适应巡航                       │
│     │                                                       │
│  ASIL B        ←  碰撞预警、盲点检测                         │
│     │                                                       │
│  ASIL A        ←  驾驶员监控、泊车辅助                       │
│     │                                                       │
│  QM            ←  信息娱乐、舒适功能                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### AI组件的安全生命周期

```c
// AI安全生命周期 (基于ISO 26262改编)
typedef enum {
    AI_PHASE_CONCEPT = 0,           // 概念阶段
    AI_PHASE_DATA_COLLECTION,       // 数据收集与验证
    AI_PHASE_MODEL_DEVELOPMENT,     // 模型开发与训练
    AI_PHASE_VERIFICATION,          // 验证与确认
    AI_PHASE_INTEGRATION,           // 系统集成
    AI_PHASE_OPERATION,             // 运行与监控
    AI_PHASE_DECOMMISSIONING        // 退役
} ai_safety_phase_t;

// 安全目标示例 (SG: Safety Goal)
typedef struct {
    uint32_t id;
    char description[256];
    uint8_t asil_level;             // ASIL A-D
    float max_false_negative_rate;  // 最大漏检率
    float max_false_positive_rate;  // 最大误检率
    uint32_t max_response_time_ms;  // 最大响应时间
} ai_safety_goal_t;

// 示例: 行人检测安全目标
ai_safety_goal_t pedestrian_detection_sg = {
    .id = 0x01,
    .description = "Avoid collision with pedestrians",
    .asil_level = ASIL_D,
    .max_false_negative_rate = 0.0001,  // 0.01%漏检率
    .max_false_positive_rate = 0.01,    // 1%误检率 (可接受)
    .max_response_time_ms = 100         // 100ms内响应
};
```

---

## C语言AI安全运行时

### 1. 安全监控框架

```c
// ai_safety_monitor.h
#ifndef AI_SAFETY_MONITOR_H
#define AI_SAFETY_MONITOR_H

#include <stdint.h>
#include <stdbool.h>

// 安全监控状态
typedef enum {
    SAFETY_STATE_NORMAL = 0,        // 正常运行
    SAFETY_STATE_DEGRADED,          // 性能降级
    SAFETY_STATE_FAIL_SAFE,         // 进入安全状态
    SAFETY_STATE_EMERGENCY_STOP     // 紧急停止
} safety_state_t;

// 监控指标
typedef struct {
    // 性能指标
    float inference_time_ms;        // 推理时间
    float confidence_score;         // 置信度
    uint32_t input_data_quality;    // 输入数据质量分数

    // 资源指标
    float cpu_utilization;          // CPU使用率
    float memory_utilization;       // 内存使用率
    float temperature;              // 温度

    // 安全指标
    uint32_t anomaly_count;         // 异常检测计数
    uint32_t timeout_count;         // 超时计数
    uint32_t checksum_failures;     // 校验失败次数
} ai_health_metrics_t;

// 安全监控器上下文
typedef struct {
    safety_state_t current_state;
    ai_health_metrics_t metrics;
    ai_health_metrics_t thresholds;

    // 安全目标
    float max_inference_time_ms;
    float min_confidence_threshold;
    float max_temperature;

    // 回调函数
    void (*on_state_change)(safety_state_t old_state, safety_state_t new_state);
    void (*on_emergency)(const char* reason);
} safety_monitor_t;

// API
int safety_monitor_init(safety_monitor_t* monitor);
safety_state_t safety_monitor_check(safety_monitor_t* monitor,
                                    const ai_health_metrics_t* current);
int safety_monitor_set_fail_safe(safety_monitor_t* monitor);

#endif
```

### 2. 实现

```c
// ai_safety_monitor.c
#include "ai_safety_monitor.h"
#include <string.h>
#include <stdio.h>

int safety_monitor_init(safety_monitor_t* monitor) {
    memset(monitor, 0, sizeof(*monitor));
    monitor->current_state = SAFETY_STATE_NORMAL;

    // 默认阈值 (ASIL D级别)
    monitor->max_inference_time_ms = 50.0f;      // 50ms
    monitor->min_confidence_threshold = 0.95f;   // 95%
    monitor->max_temperature = 85.0f;            // 85°C
    monitor->thresholds.timeout_count = 3;
    monitor->thresholds.anomaly_count = 10;

    return 0;
}

safety_state_t safety_monitor_check(safety_monitor_t* monitor,
                                    const ai_health_metrics_t* current) {
    safety_state_t new_state = monitor->current_state;

    // 检查推理时间
    if (current->inference_time_ms > monitor->max_inference_time_ms) {
        monitor->metrics.timeout_count++;
        if (monitor->metrics.timeout_count >= monitor->thresholds.timeout_count) {
            new_state = SAFETY_STATE_FAIL_SAFE;
            printf("[SAFETY] Inference timeout violation: %.1f ms\n",
                   current->inference_time_ms);
        }
    } else {
        monitor->metrics.timeout_count = 0;  // 重置
    }

    // 检查置信度
    if (current->confidence_score < monitor->min_confidence_threshold) {
        new_state = SAFETY_STATE_DEGRADED;
        printf("[SAFETY] Low confidence: %.2f\n", current->confidence_score);
    }

    // 检查温度
    if (current->temperature > monitor->max_temperature) {
        new_state = SAFETY_STATE_EMERGENCY_STOP;
        printf("[SAFETY] Over temperature: %.1f°C\n", current->temperature);
    }

    // 检查输入数据质量
    if (current->input_data_quality < 50) {  // 质量分数0-100
        new_state = SAFETY_STATE_DEGRADED;
        printf("[SAFETY] Poor input data quality: %u\n",
               current->input_data_quality);
    }

    // 状态转换
    if (new_state != monitor->current_state) {
        if (monitor->on_state_change) {
            monitor->on_state_change(monitor->current_state, new_state);
        }

        if (new_state == SAFETY_STATE_EMERGENCY_STOP && monitor->on_emergency) {
            monitor->on_emergency("Critical safety violation detected");
        }

        monitor->current_state = new_state;
    }

    // 更新指标
    monitor->metrics = *current;

    return new_state;
}

int safety_monitor_set_fail_safe(safety_monitor_t* monitor) {
    monitor->current_state = SAFETY_STATE_FAIL_SAFE;

    // 执行安全动作
    printf("[SAFETY] Entering fail-safe mode\n");

    // 示例: 切换到备份系统、降低车速等

    return 0;
}

// 使用示例
void safety_example(void) {
    safety_monitor_t monitor;
    safety_monitor_init(&monitor);

    // 设置回调
    monitor.on_state_change = [](safety_state_t old, safety_state_t new_s) {
        printf("State change: %d -> %d\n", old, new_s);
    };

    monitor.on_emergency = [](const char* reason) {
        printf("EMERGENCY: %s\n", reason);
        trigger_emergency_brake();
    };

    // 主循环
    while (1) {
        ai_health_metrics_t metrics = {0};

        // 收集指标
        metrics.inference_time_ms = measure_inference_time();
        metrics.confidence_score = get_model_confidence();
        metrics.temperature = read_temperature_sensor();
        metrics.input_data_quality = validate_input_data();

        // 安全检查
        safety_state_t state = safety_monitor_check(&monitor, &metrics);

        // 根据状态执行动作
        switch (state) {
            case SAFETY_STATE_NORMAL:
                execute_normal_control();
                break;
            case SAFETY_STATE_DEGRADED:
                execute_degraded_control();  // 保守策略
                break;
            case SAFETY_STATE_FAIL_SAFE:
                execute_fail_safe();         // 最小风险
                break;
            case SAFETY_STATE_EMERGENCY_STOP:
                emergency_stop();            // 立即停止
                return;
        }
    }
}
```

---

## AI形式化验证

### 神经网络验证框架

```c
// nn_formal_verification.h
// 神经网络的轻量级形式化验证

#ifndef NN_FORMAL_VERIFICATION_H
#define NN_FORMAL_VERIFICATION_H

#include <stdint.h>

// 区间抽象 (Interval Abstract Domain)
typedef struct {
    float lower;
    float upper;
} interval_t;

// 验证属性
typedef enum {
    PROP_LOCAL_ROBUSTNESS = 0,  // 局部鲁棒性
    PROP_OUTPUT_RANGE,          // 输出范围
    PROP_MONOTONICITY,          // 单调性
    PROP_LIPSCHITZ_BOUND        // Lipschitz约束
} verification_property_t;

// 验证结果
typedef struct {
    bool verified;
    float confidence;
    char counter_example[256];  // 反例 (如果验证失败)
} verification_result_t;

// 区间传播验证
verification_result_t verify_interval_propagation(
    const float* network_weights,
    const interval_t* input_intervals,
    int num_inputs,
    interval_t* output_intervals,
    int num_outputs
);

// 局部鲁棒性验证 (给定输入邻域内输出不变)
verification_result_t verify_local_robustness(
    const float* input,
    int input_size,
    float epsilon,              // L-infinity球半径
    int expected_class,
    float (*network_inference)(const float*, int)
);

#endif
```

### 区间传播实现

```c
// 基于抽象解释的神经网络验证

// ReLU的区间传播
interval_t relu_interval(interval_t x) {
    return (interval_t){
        .lower = (x.lower > 0) ? x.lower : 0,
        .upper = (x.upper > 0) ? x.upper : 0
    };
}

// 线性层的区间传播
void linear_interval_propagation(const interval_t* inputs,
                                int num_inputs,
                                const float* weights,
                                const float* bias,
                                int num_outputs,
                                interval_t* outputs) {
    for (int i = 0; i < num_outputs; i++) {
        float lower = bias[i];
        float upper = bias[i];

        for (int j = 0; j < num_inputs; j++) {
            float w = weights[i * num_inputs + j];

            if (w >= 0) {
                lower += w * inputs[j].lower;
                upper += w * inputs[j].upper;
            } else {
                lower += w * inputs[j].upper;  // 负权重反转边界
                upper += w * inputs[j].lower;
            }
        }

        outputs[i] = (interval_t){lower, upper};
    }
}

// 验证输出范围
bool verify_output_range(const interval_t* output_intervals,
                        int num_outputs,
                        int expected_class,
                        float margin) {
    float expected_lower = output_intervals[expected_class].lower;

    for (int i = 0; i < num_outputs; i++) {
        if (i == expected_class) continue;

        // 检查预期类的下界是否大于其他类的上界+margin
        if (expected_lower <= output_intervals[i].upper + margin) {
            return false;  // 无法保证
        }
    }
    return true;
}

// 完整验证流程
verification_result_t verify_network_safety(
    const float* input,
    int input_size,
    float epsilon,
    int expected_class,
    const neural_network_t* network) {

    // 1. 构建输入区间
    interval_t input_intervals[input_size];
    for (int i = 0; i < input_size; i++) {
        input_intervals[i] = (interval_t){
            input[i] - epsilon,
            input[i] + epsilon
        };
    }

    // 2. 逐层区间传播
    interval_t current_intervals[256];  // 假设最大层大小
    memcpy(current_intervals, input_intervals,
           input_size * sizeof(interval_t));

    int current_size = input_size;

    for (int layer = 0; layer < network->num_layers; layer++) {
        interval_t next_intervals[256];

        // 线性变换
        linear_interval_propagation(
            current_intervals, current_size,
            network->layers[layer].weights,
            network->layers[layer].bias,
            network->layers[layer].num_outputs,
            next_intervals
        );

        // 激活函数
        if (network->layers[layer].activation == ACT_RELU) {
            for (int i = 0; i < network->layers[layer].num_outputs; i++) {
                next_intervals[i] = relu_interval(next_intervals[i]);
            }
        }

        memcpy(current_intervals, next_intervals,
               network->layers[layer].num_outputs * sizeof(interval_t));
        current_size = network->layers[layer].num_outputs;
    }

    // 3. 验证输出
    verification_result_t result = {0};
    result.verified = verify_output_range(
        current_intervals, current_size, expected_class, 0.1f
    );
    result.confidence = 1.0f;  // 抽象解释给出确定保证

    if (!result.verified) {
        snprintf(result.counter_example, sizeof(result.counter_example),
                "Output intervals overlap for class %d", expected_class);
    }

    return result;
}
```

---

## 对抗攻击防护

### 输入验证与滤波

```c
// adversarial_defense.c

// 输入预处理防御
typedef struct {
    float jpeg_quality;         // JPEG压缩质量 (降低对抗噪声)
    float spatial_smooth_sigma; // 空间平滑sigma
    int bit_depth_reduction;    // 位深度降低
} input_defense_config_t;

// 空间平滑滤波 (对抗FGSM攻击)
void spatial_smoothing_filter(const uint8_t* input,
                             uint8_t* output,
                             int width, int height, int channels,
                             float sigma) {
    // 高斯平滑核
    float kernel[3][3] = {
        {0.0625, 0.125, 0.0625},
        {0.125,  0.25,  0.125},
        {0.0625, 0.125, 0.0625}
    };

    for (int c = 0; c < channels; c++) {
        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                float sum = 0;
                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        int idx = ((y + ky) * width + (x + kx)) * channels + c;
                        sum += input[idx] * kernel[ky + 1][kx + 1];
                    }
                }
                output[(y * width + x) * channels + c] = (uint8_t)sum;
            }
        }
    }
}

// 输入异常检测
bool detect_adversarial_input(const float* input,
                             int size,
                             float threshold) {
    // 检查输入统计特性
    float mean = 0, variance = 0;

    for (int i = 0; i < size; i++) {
        mean += input[i];
    }
    mean /= size;

    for (int i = 0; i < size; i++) {
        variance += (input[i] - mean) * (input[i] - mean);
    }
    variance /= size;

    // 异常高的方差可能表示对抗噪声
    if (variance > threshold) {
        return true;  // 可能是对抗样本
    }

    // 检查局部平滑性 (对抗样本通常有不自然的局部变化)
    float local_variation = 0;
    for (int i = 1; i < size; i++) {
        local_variation += fabsf(input[i] - input[i-1]);
    }

    if (local_variation / size > threshold * 2) {
        return true;
    }

    return false;
}
```

---

## 认证检查清单

### ISO 26262认证要求

```markdown
## 概念阶段
□ 危害分析和风险评估 (HARA)
□ 定义安全目标 (Safety Goals)
□ 分配到ASIL等级

## 产品开发: 系统级
□ 系统架构设计 (考虑AI组件)
□ 集成和测试策略
□ 安全机制设计 (冗余、监控)

## 产品开发: 硬件级
□ 硬件安全分析 (FMEA/FMEDA)
□ 诊断覆盖率
□ 硬件指标 (SPFM/LFM)

## 产品开发: 软件级 (AI特定)
□ 数据收集流程验证
□ 训练数据质量度量
□ 模型验证 (准确性、鲁棒性)
□ 软件单元测试 (传统+统计)
□ 软件集成测试
□ 嵌入式部署验证

## 产品开发: 集成
□ 系统集成测试
□ 整车测试
□ 实际道路测试

## 运行阶段
□ 监控和诊断
□ OTA更新安全
□ 数据记录和追溯
```

---

## 参考资源

- [ISO 26262-6:2018](https://www.iso.org/standard/68383.html) - 汽车软件功能安全
- [ISO/IEC 24028:2020](https://www.iso.org/standard/77607.html) - AI可信度概述
- [UL 4600 Standard](https://www.ul.com/research/ul-4600) - 自动驾驶产品安全
- [AI Safety Landscape](https://arxiv.org/abs/2101.00027) - AI安全研究综述

---

**上一章**: [../07_Hardware_Acceleration/01_NPU_TPU_C_Interfaces.md](../07_Hardware_Acceleration/01_NPU_TPU_C_Interfaces.md)

**最后更新**: 2026-03-18
