---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
﻿# ISO 26262 ASIL分解与实施指南

> ASIL (Automotive Safety Integrity Level) 分解方法和实施策略

---

## ASIL分解概述

ASIL分解是将高ASIL等级的安全要求分配给多个独立元素，每个元素承担较低ASIL等级，从而降低整体开发成本和复杂性。

### 分解原则

| 原始ASIL | 分解选项 | 独立性要求 |
|:--------:|:---------|:-----------|
| ASIL D | ASIL C(D) + ASIL A(D) | 充分独立性 |
| ASIL D | ASIL B(D) + ASIL B(D) | 充分独立性 |
| ASIL C | ASIL B(C) + ASIL A(C) | 充分独立性 |
| ASIL B | ASIL A(B) + ASIL A(B) | 充分独立性 |

**注**: ASIL X(Y)表示分解后ASIL X，原始ASIL Y

---

## 分解模式

### 模式1: 冗余架构 (ASIL D → ASIL B + ASIL B)

```c
/**
 * 示例: 制动系统冗余
 * 两个独立的ASIL B通道共同实现ASIL D功能
 */

/* 通道A - ASIL B */
typedef struct {
    int32_t pedal_position;
    uint32_t checksum;
    uint8_t status;
} BrakeChannelA;

/* 通道B - ASIL B (独立设计) */
typedef struct {
    float pedal_force;
    uint16_t crc16;
    uint8_t state;
} BrakeChannelB;

/* 比较器 - ASIL D */
typedef struct {
    BrakeChannelA ch_a;
    BrakeChannelB ch_b;
    bool valid;
} BrakeSystem;

int32_t get_brake_force(const BrakeSystem *sys)
{
    /* 独立读取两个通道 */
    int32_t force_a = calculate_force_a(&sys->ch_a);
    int32_t force_b = calculate_force_b(&sys->ch_b);

    /* 交叉比较 */
    if (abs(force_a - force_b) > THRESHOLD) {
        /* 不一致 - 进入安全状态 */
        return enter_safe_state();
    }

    /* 输出取较保守值 */
    return (force_a < force_b) ? force_a : force_b;
}
```

### 模式2: 监控架构 (ASIL D → ASIL C + ASIL A)

```c
/**
 * 示例: 带监控的计算
 * ASIL C执行主要功能，ASIL A监控其正确性
 */

/* 主功能 - ASIL C */
int32_t calculate_steering_angle(int32_t sensor_raw)
{
    /* 复杂的信号处理和滤波 */
    int32_t filtered = apply_filter(sensor_raw);
    int32_t compensated = apply_compensation(filtered);
    return apply_limits(compensated);
}

/* 监控功能 - ASIL A */
bool check_steering_plausibility(int32_t input, int32_t output)
{
    /* 简化的合理性检查 */
    int32_t expected_range = abs(input) * MAX_GAIN;
    return (abs(output) <= expected_range);
}

/* 组合调用 */
int32_t safe_steering_calculation(int32_t sensor_raw)
{
    int32_t result = calculate_steering_angle(sensor_raw);

    if (!check_steering_plausibility(sensor_raw, result)) {
        /* 监控检测到异常 */
        log_error("Steering monitoring failed");
        return USE_DEFAULT_VALUE;
    }

    return result;
}
```

### 模式3: 多样化设计 (ASIL C → ASIL B + ASIL A)

```c
/**
 * 示例: 多样化算法实现
 * 两种不同算法计算同一结果，互相验证
 */

/* 算法A - 查表法 - ASIL B */
int32_t calc_speed_lookup(uint32_t pulse_count, uint32_t time_ms)
{
    uint32_t rpm = rpm_table[pulse_count];
    return (rpm * WHEEL_CIRCUMFERENCE) / (time_ms / 1000);
}

/* 算法B - 直接计算 - ASIL A */
int32_t calc_speed_direct(uint32_t pulse_count, uint32_t time_ms)
{
    float pulses_per_sec = (float)pulse_count / (time_ms / 1000.0f);
    float rev_per_sec = pulses_per_sec / PULSES_PER_REV;
    return (int32_t)(rev_per_sec * WHEEL_CIRCUMFERENCE);
}

/* 多样化比较 */
int32_t get_vehicle_speed(uint32_t pulses, uint32_t time)
{
    int32_t speed_a = calc_speed_lookup(pulses, time);
    int32_t speed_b = calc_speed_direct(pulses, time);

    int32_t diff = abs(speed_a - speed_b);
    if (diff > SPEED_TOLERANCE) {
        /* 算法不一致 */
        return DIAGNOSTIC_MODE;
    }

    return (speed_a + speed_b) / 2;
}
```

---

## 独立性要求

### 充分独立性的要素

```c
/**
 * 确保分解元素间的充分独立性
 */

/* 1. 开发独立性 - 不同团队 */
/* 团队A开发 */
// brake_channel_a.c - ASIL B

/* 团队B开发 */
// brake_channel_b.c - ASIL B (完全不同的设计)

/* 2. 资源独立性 - 独立硬件 */
typedef struct {
    /* 通道A使用ADC0 */
    volatile uint16_t *adc_ch_a = (uint16_t *)0x40001000;
    /* 通道B使用ADC1 */
    volatile uint16_t *adc_ch_b = (uint16_t *)0x40002000;
} AdcChannels;

/* 3. 时钟独立性 */
void independent_sampling(void)
{
    /* 不同时钟源触发 */
    trigger_adc_a();  /* 主时钟 */
    delay_us(10);     /* 时间偏移 */
    trigger_adc_b();  /* 相同时钟但偏移 */
}

/* 4. 软件独立性 - 不同算法 */
int32_t filter_a(int32_t input) {
    /* FIR滤波器 */
    return fir_filter(input);
}

int32_t filter_b(int32_t input) {
    /* IIR滤波器 */
    return iir_filter(input);
}
```

---

## 安全机制实现

### 硬件安全机制

| 机制 | ASIL适用 | 实现方式 |
|:-----|:---------|:---------|
| 双核锁步 | D, C | 主核+监控核逐周期比较 |
| ECC内存 | D, C | 自动纠正单比特错误 |
| 看门狗 | 所有 | 独立时钟监控程序流 |
| 时钟监控 | D, C | PLL失效检测 |
| 温度监控 | 所有 | 过温保护 |

### 软件安全机制

```c
/**
 * 软件安全机制实现
 */

/* 1. 范围检查 */
#define CHECK_RANGE(val, min, max) \
    (((val) >= (min)) && ((val) <= (max)))

/* 2. 合理性检查 */
bool plausibility_check(int32_t current, int32_t previous, int32_t max_delta)
{
    return (abs(current - previous) <= max_delta);
}

/* 3. 多样性计算 */
int32_t diverse_calculation(int32_t input)
{
    /* 方法A */
    int32_t result_a = algorithm_a(input);
    /* 方法B */
    int32_t result_b = algorithm_b(input);

    if (result_a != result_b) {
        /* 不一致处理 */
    }
    return result_a;
}

/* 4. 控制流监控 */
static uint32_t expected_checksum = 0x12345678;

void control_flow_monitor(void)
{
    uint32_t actual = calculate_block_checksum();
    if (actual != expected_checksum) {
        /* 控制流错误 */
        enter_safe_state();
    }
}

/* 5. 数据流监控 */
typedef struct {
    int32_t data;
    uint32_t timestamp;
    uint16_t sequence;
    uint16_t crc;
} ProtectedData;

bool verify_data_integrity(const ProtectedData *pd)
{
    uint16_t calc_crc = calculate_crc(pd, sizeof(*pd) - sizeof(pd->crc));
    return (calc_crc == pd->crc);
}
```

---

## ASIL实施检查清单

### ASIL D项目检查清单

```markdown
## 系统级
□ 安全概念文档
□ 技术安全需求 (TSR)
□ 系统架构设计
□ 系统安全分析 (FTA/FMEA)
□ 软硬件接口规范

## 硬件级
□ 硬件安全需求
□ 硬件设计文档
□ 硬件安全分析
□ FMEDA分析
□ 失效率计算

## 软件级
□ 软件安全需求
□ 软件架构设计
□ 软件单元设计
□ MISRA C:2023合规
□ 100%语句覆盖
□ 100%分支覆盖
□ MC/DC覆盖 (ASIL D要求)
□ 静态分析
□ 动态分析

## 测试
□ 软件单元测试
□ 软件集成测试
□ 硬件集成测试
□ 系统测试
□ 实车测试
□ 确认评审
```

---

## 成本与效益分析

| ASIL | 开发成本 | 验证成本 | 工具要求 | 典型应用 |
|:-----|:--------:|:--------:|:---------|:---------|
| QM | 1x | 1x | 基础 | 信息娱乐 |
| ASIL A | 1.2x | 1.3x | 静态分析 | 后视镜 |
| ASIL B | 1.5x | 1.8x | 静态+单元测试 | 大灯 |
| ASIL C | 2.0x | 2.5x | 全套工具链 | 自适应巡航 |
| ASIL D | 3.0x | 4.0x | 全套+形式化 | 转向/制动 |

---

**参考**: ISO 26262-2018 Part 9: Automotive Safety Integrity Level (ASIL)-oriented and safety-oriented analyses


---

## 深入理解

### 技术原理

深入探讨相关技术原理和实现细节。

### 实践指南

- 步骤1：理解基础概念
- 步骤2：掌握核心原理
- 步骤3：应用实践

### 相关资源

- 文档链接
- 代码示例
- 参考文章

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
