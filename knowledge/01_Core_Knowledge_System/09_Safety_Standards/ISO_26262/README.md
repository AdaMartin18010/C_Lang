# ISO 26262 - 道路车辆功能安全

> **层级定位**: 01_Core_Knowledge_System > 09_Safety_Standards > ISO_26262  
> **难度级别**: ⭐⭐⭐⭐⭐ L5  
> **前置知识**: 功能安全基础、C语言高级、嵌入式系统  
> **参考标准**: ISO 26262:2018 (第二版)

---

## 概述

ISO 26262是源自IEC 61508的国际标准，专门针对道路车辆（乘用车、卡车、巴士等）的电子电气(E/E)系统的功能安全。该标准覆盖汽车软件的全生命周期，从概念阶段到产品开发、生产、运行、服务和退役。

| 属性 | 说明 |
|:-----|:-----|
| **版本** | ISO 26262:2018（第二版，12部分）|
| **前身** | ISO 26262:2011（第一版，10部分）|
| **适用范围** | 乘用车、卡车、巴士、拖车、半挂车、摩托车 |
| **核心概念** | ASIL（汽车安全完整性等级）|
| **与IEC 61508关系** | 行业专用改编版本 |

---

## ASIL等级确定

ASIL (Automotive Safety Integrity Level) 根据风险三要素确定：

| 要素 | 描述 | 评级 |
|:-----|:-----|:-----|
| **S** (Severity) | 伤害严重程度 | S0-S3 |
| **E** (Exposure) | 暴露概率 | E0-E4 |
| **C** (Controllability) | 可控性 | C0-C3 |

### ASIL等级映射

| S | E | C | ASIL |
|:-:|:-:|:-:|:----:|
| S3 | E4 | C3 | **ASIL D** |
| S3 | E4 | C2 | ASIL C |
| S3 | E3 | C3 | ASIL C |
| S3 | E4 | C1 | ASIL B |
| S3 | E3 | C2 | ASIL B |
| S3 | E2 | C3 | ASIL B |
| S2 | E4 | C3 | ASIL B |
| ... | ... | ... | ASIL A/QM |

---

## 软件级产品开发 (Part 6)

### 软件安全要求规格 (SSR)

```c
/* 软件安全要求示例 */
/* ASIL D级别的安全监控功能 */

/* 安全要求: 转向角传感器信号必须在10ms内更新 */
#define STEERING_ANGLE_TIMEOUT_MS 10
#define STEERING_ANGLE_MAX_DEG 540  /* ±540度 */

/* 安全机制: 范围检查和时间监控 */
typedef struct {
    float angle_deg;           /* 转向角 [-540, +540] */
    uint32_t timestamp_ms;     /* 时间戳 */
    uint8_t valid;             /* 有效性标志 */
    uint32_t error_count;      /* 错误计数 */
} SteeringAngle_Signal;

/* ASIL D: 冗余校验和看门狗 */
int process_steering_angle(SteeringAngle_Signal* primary,
                          SteeringAngle_Signal* redundant)
{
    uint32_t current_time = get_system_time_ms();
    
    /* 1. 时间监控 (新鲜度检查) */
    if ((current_time - primary->timestamp_ms) > STEERING_ANGLE_TIMEOUT_MS) {
        primary->valid = 0;
        log_safety_error(ERROR_STEERING_TIMEOUT);
        return ERROR;
    }
    
    /* 2. 范围检查 */
    if (primary->angle_deg < -STEERING_ANGLE_MAX_DEG ||
        primary->angle_deg > STEERING_ANGLE_MAX_DEG) {
        primary->valid = 0;
        log_safety_error(ERROR_STEERING_RANGE);
        return ERROR;
    }
    
    /* 3. 冗余一致性检查 (ASIL D) */
    if (redundant != NULL) {
        float diff = fabsf(primary->angle_deg - redundant->angle_deg);
        if (diff > 2.0f) {  /* 允许2度偏差 */
            log_safety_error(ERROR_STEERING_MISMATCH);
            return ERROR;
        }
    }
    
    primary->valid = 1;
    return OK;
}
```

### 软件架构设计原则

```c
/* ISO 26262推荐的软件架构模式 */

/* 1. 分层架构 */
typedef enum {
    LAYER_APP,      /* 应用层 */
    LAYER_SAFETY,   /* 安全监控层 */
    LAYER_MIDDLE,   /* 中间件 */
    LAYER_DRV,      /* 驱动层 */
    LAYER_MAX
} SoftwareLayer;

/* 2. 安全监控层 (独立安全机制) */
void safety_monitor_task(void)
{
    while (1) {
        /* 监控应用层输出 */
        SafetyCheckResult result = check_application_outputs();
        
        if (result.fault_detected) {
            /* 触发安全状态 */
            enter_safe_state(result.fault_type);
        }
        
        /* ASIL D: 看门狗喂狗 */
        watchdog_refresh();
        
        sleep_ms(SAFETY_MONITOR_PERIOD_MS);
    }
}

/* 3. 多样性设计 (ASIL D) */
/* 使用不同算法实现相同功能，交叉验证 */
float calculate_torque_method1(PedalData* data);
float calculate_torque_method2(PedalData* data);

int validate_torque_calculation(PedalData* data, float* output)
{
    float torque1 = calculate_torque_method1(data);
    float torque2 = calculate_torque_method2(data);
    
    /* 交叉比较 */
    if (fabsf(torque1 - torque2) > TORQUE_TOLERANCE) {
        return ERROR_CALCULATION_MISMATCH;
    }
    
    *output = torque1;  /* 或取平均值 */
    return OK;
}
```

---

## 深入理解

### 1. 安全机制实现

```c
/* ISO 26262 Part 5 Annex D 推荐的安全机制 */

/* 1. 输入/输出比较监控 */
typedef struct {
    float commanded_value;
    float feedback_value;
    float tolerance;
    uint32_t timeout_ms;
} OutputMonitor;

int monitor_output(OutputMonitor* mon, float* actual)
{
    /* 范围检查 */
    if (*actual < mon->feedback_value - mon->tolerance ||
        *actual > mon->feedback_value + mon->tolerance) {
        return FAULT_DETECTED;
    }
    return NO_FAULT;
}

/* 2. 程序序列监控 (看门狗) */
typedef struct {
    uint32_t expected_sequence;
    uint32_t current_sequence;
    uint32_t checkpoint_mask;
} SequenceMonitor;

void sequence_checkpoint(SequenceMonitor* sm, uint32_t point_id)
{
    sm->current_sequence |= (1 << point_id);
    
    if ((sm->current_sequence & sm->checkpoint_mask) != sm->expected_sequence) {
        /* 序列错误，触发安全响应 */
        handle_sequence_error();
    }
}

/* 3. 数据多样性 (One's Complement冗余存储) */
typedef struct {
    int32_t data;
    int32_t data_inv;  /* 按位取反 */
} DiverseData;

void store_diverse(DiverseData* dd, int32_t value)
{
    dd->data = value;
    dd->data_inv = ~value;
}

int read_diverse(DiverseData* dd, int32_t* value)
{
    if (dd->data != ~(dd->data_inv)) {
        return ERROR_DATA_CORRUPTION;
    }
    *value = dd->data;
    return OK;
}

/* 4. 心跳监控 */
typedef struct {
    uint32_t last_heartbeat;
    uint32_t timeout_threshold;
    uint8_t alive;
} HeartbeatMonitor;

void check_heartbeat(HeartbeatMonitor* hb)
{
    uint32_t current = get_tick_count();
    if ((current - hb->last_heartbeat) > hb->timeout_threshold) {
        hb->alive = 0;
        handle_heartbeat_timeout();
    }
}
```

### 2. E2E (End-to-End) 保护

```c
/* AUTOSAR E2E保护 - 用于安全相关通信 */

#include "E2E_P01.h"  /* Profile 1: CRC+Counter */

typedef struct {
    uint8_t counter;      /* 序列计数器 */
    uint8_t crc;          /* CRC校验 */
    uint16_t data_id;     /* 数据ID */
} E2E_Header;

/* E2E Profile 1 保护 */
int E2E_protect(E2E_Header* header, uint8_t* data, uint16_t length)
{
    /* 1. 更新计数器 */
    header->counter++;
    
    /* 2. 计算CRC (包含Data ID) */
    header->crc = calculate_crc8(data, length, header->data_id);
    
    return E2E_OK;
}

int E2E_check(E2E_Header* header, uint8_t* data, uint16_t length)
{
    /* 1. 检查CRC */
    uint8_t expected_crc = calculate_crc8(data, length, header->data_id);
    if (header->crc != expected_crc) {
        return E2E_ERROR_CRC;
    }
    
    /* 2. 检查计数器 (检测丢失/重复/乱序) */
    static uint8_t last_counter = 0;
    int8_t delta = (int8_t)(header->counter - last_counter);
    
    if (delta == 0) {
        return E2E_ERROR_REPEATED;
    } else if (delta > 1) {
        return E2E_ERROR_LOST;  /* 丢帧 */
    } else if (delta < 0) {
        return E2E_ERROR_WRONG_SEQ;  /* 乱序 */
    }
    
    last_counter = header->counter;
    return E2E_OK;
}
```

### 3. 安全分析与FMEA

```c
/* 故障模式示例与检测 */

/* 故障模式: 内存损坏 */
/* 安全机制: 内存保护单元(MPU) + ECC */
#ifdef USE_MPU
void configure_mpu_safety_regions(void)
{
    /* 配置MPU区域 */
    MPU_Region safety_monitor_region = {
        .base_addr = (uint32_t)&safety_monitor_task,
        .size = MPU_SIZE_4KB,
        .access = MPU_ACCESS_PRIV_RW,
        .execute = MPU_XN_ENABLE  /* 不可执行数据区 */
    };
    mpu_configure_region(0, &safety_monitor_region);
}
#endif

/* 故障模式: 时序错误 */
/* 安全机制: 看门狗 +  deadline监控 */
typedef struct {
    uint32_t start_time;
    uint32_t deadline;
    uint8_t monitoring_active;
} DeadlineMonitor;

void start_deadline_monitor(DeadlineMonitor* dm, uint32_t deadline_us)
{
    dm->start_time = get_micros();
    dm->deadline = deadline_us;
    dm->monitoring_active = 1;
}

int check_deadline(DeadlineMonitor* dm)
{
    if (!dm->monitoring_active) return OK;
    
    uint32_t elapsed = get_micros() - dm->start_time;
    if (elapsed > dm->deadline) {
        dm->monitoring_active = 0;
        return ERROR_DEADLINE_MISS;
    }
    return OK;
}
```

---

## 与MISRA C的关系

| ASIL | MISRA C:2012要求 | 覆盖率要求 |
|:-----|:-----------------|:-----------|
| QM | 建议符合 | 无 |
| ASIL A | 建议符合Required规则 | 语句覆盖 |
| ASIL B | 推荐符合Required规则 | 语句+分支覆盖 |
| ASIL C | 推荐符合Required+Mandatory | 语句+分支+MC/DC |
| ASIL D | 必须符合Required+Mandatory+Advisory | 语句+分支+MC/DC |

```c
/* ASIL D编码实践 */

/* 1. 防御性编程 */
int process_vehicle_speed(uint32_t raw_sensor_value)
{
    /* NULL/范围检查 */
    if (raw_sensor_value == 0) {
        return ERROR_INVALID_INPUT;
    }
    
    /* 合理性检查 */
    uint32_t speed_kmh = raw_to_kmh(raw_sensor_value);
    if (speed_kmh > 400) {  /* 超过任何车辆的最大速度 */
        return ERROR_IMPLAUSIBLE;
    }
    
    /* 速率限制检查 */
    static uint32_t last_speed = 0;
    if (abs((int)speed_kmh - (int)last_speed) > MAX_ACCEL_RATE) {
        return ERROR_RATE_EXCEEDED;
    }
    
    last_speed = speed_kmh;
    return (int)speed_kmh;
}

/* 2. 错误处理 */
typedef enum {
    E_OK = 0,
    E_NOT_OK = 1,
    E_UNKNOWN = 2
} Std_ReturnType;

Std_ReturnType safety_critical_function(void)
{
    Std_ReturnType result = E_NOT_OK;
    
    /* 所有路径必须设置result */
    if (precondition_check()) {
        if (execute_safety_action()) {
            result = E_OK;
        } else {
            result = E_NOT_OK;
        }
    } else {
        result = E_UNKNOWN;
    }
    
    return result;
}
```

---

## 开发工具链

| 工具类型 | ASIL D要求 | 示例工具 |
|:---------|:-----------|:---------|
| 编译器 | 经过认证 | Tasking, Green Hills |
| 静态分析 | 必须符合 | Polyspace, Klocwork |
| 单元测试 | 必须符合 | VectorCAST, Cantata |
| 需求管理 | 追溯性 | DOORS, Polarion |

---

## 权威参考

- [1] ISO 26262-6:2018 - Road vehicles - Functional safety - Part 6: Product development at the software level
- [2] "Functional Safety for Road Vehicles" by Hans-Leo Ross
- [3] AUTOSAR Specification of E2E Protocol
- [4] MISRA C:2012 Guidelines for the use of the C language in critical systems

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review  
> **内容深度**: L5 (研究级)
