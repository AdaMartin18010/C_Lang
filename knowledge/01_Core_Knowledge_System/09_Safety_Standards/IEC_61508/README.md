---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
# IEC 61508 - 电气/电子/可编程电子安全相关系统的功能安全

> **层级定位**: 01_Core_Knowledge_System > 09_Safety_Standards > IEC_61508
> **难度级别**: ⭐⭐⭐⭐⭐ L5
> **前置知识**: 功能安全基础、可靠性工程、C语言
> **参考标准**: IEC 61508:2010 (第二版)

---

## 概述

IEC 61508是功能安全的通用国际标准，适用于各种行业的电气、电子和可编程电子(E/E/PE)安全相关系统。它是众多行业专用功能安全标准的基础。

| 属性 | 说明 |
|:-----|:-----|
| **版本** | IEC 61508:2010（第二版，共7部分）|
| **第一版** | IEC 61508:1998-2000 |
| **适用范围** | 通用工业、过程控制、铁路、核电、机械等 |
| **地位** | 行业专用标准的母标准 |
| **核心概念** | SIL（安全完整性等级）|

---

## 安全完整性等级 (SIL)

### 目标失效概率

| SIL | 低需求模式 (PFDavg) | 高需求/连续模式 (PFH) |
|:---:|:-------------------:|:---------------------:|
| SIL 4 | ≥10⁻⁵ to <10⁻⁴ | ≥10⁻⁹ to <10⁻⁸ /h |
| SIL 3 | ≥10⁻⁴ to <10⁻³ | ≥10⁻⁸ to <10⁻⁷ /h |
| SIL 2 | ≥10⁻³ to <10⁻² | ≥10⁻⁷ to <10⁻⁶ /h |
| SIL 1 | ≥10⁻² to <10⁻¹ | ≥10⁻⁶ to <10⁻⁵ /h |

### 典型应用领域

| SIL | 应用领域 | 系统示例 |
|:---:|:---------|:---------|
| SIL 4 | 铁路信号、核电保护 | ATP系统、反应堆保护 |
| SIL 3 | 过程工业ESD、燃烧器管理 | 紧急停车系统、BMS |
| SIL 2 | PLC安全模块、机器人 | 安全PLC、协作机器人 |
| SIL 1 | 非关键监控 | 环境监测、报警系统 |

---

## 系统安全生命周期

```
┌─────────────────────────────────────────────────────────────┐
│  1. 整体安全生命周期                                          │
│     ├── 危害分析与风险评估 (HAZOP, LOPA)                       │
│     ├── 确定SIL等级                                           │
│     └── 安全需求规格                                          │
├─────────────────────────────────────────────────────────────┤
│  2. E/E/PE系统安全生命周期                                    │
│     ├── 系统架构设计                                          │
│     ├── 硬件设计 (Part 2)                                     │
│     └── 软件设计 (Part 3)                                     │
├─────────────────────────────────────────────────────────────┤
│  3. 软件安全生命周期 (Part 3)                                 │
│     ├── 软件计划                                              │
│     ├── 软件需求                                              │
│     ├── 软件架构设计                                          │
│     ├── 软件详细设计与实现                                    │
│     ├── 软件测试                                              │
│     └── 软件验证                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 软件安全要求 (Part 3)

### 技术与措施选择

IEC 61508-3 Annex A/B/C/D 提供了按SIL分级的推荐技术：

| 技术 | SIL 1 | SIL 2 | SIL 3 | SIL 4 |
|:-----|:-----:|:-----:|:-----:|:-----:|
| 模块化 | R | HR | HR | HR |
| 设计审查 | R | HR | HR | HR |
| 静态分析 | R | R | HR | HR |
| 动态分析 | R | R | HR | HR |
| 故障插入测试 | - | R | HR | HR |
| 形式化方法 | - | R | R | HR |

*HR = Highly Recommended, R = Recommended, - = Not applicable*

### C语言安全开发规范

```c
/* IEC 61508 SIL 3/4 编码实践 */

/* 1. 强类型检查 */
typedef enum {
    SAFE_STATE_IDLE = 0,
    SAFE_STATE_ACTIVE = 1,
    SAFE_STATE_ERROR = 2
} SafeState;

/* 不使用魔术数字，使用枚举 */
SafeState get_current_state(void);

/* 2. 防御性编程 */
int process_safety_input(uint32_t raw_value, uint32_t* validated)
{
    /* 输入验证 */
    if (validated == NULL) {
        return ERROR_NULL_POINTER;
    }

    /* 范围检查 */
    if (raw_value < INPUT_MIN || raw_value > INPUT_MAX) {
        log_safety_event(EVENT_OUT_OF_RANGE, raw_value);
        return ERROR_OUT_OF_RANGE;
    }

    /* 一致性检查 */
    static uint32_t last_value = 0;
    if (abs((int)raw_value - (int)last_value) > MAX_RATE_OF_CHANGE) {
        return ERROR_RATE_EXCEEDED;
    }

    /* 合理性检查 */
    if (!check_plausibility(raw_value)) {
        return ERROR_IMPLAUSIBLE;
    }

    *validated = raw_value;
    last_value = raw_value;
    return SUCCESS;
}

/* 3. 内存保护 */
#ifdef SIL3_SIL4
#define USE_MEMORY_PROTECTION 1
#endif

#if USE_MEMORY_PROTECTION
/* 使用硬件MPU保护关键数据 */
typedef struct {
    uint32_t safety_critical_data;
    uint32_t crc;  /* 数据完整性校验 */
} ProtectedData;

uint32_t calculate_crc32(const void* data, size_t length);

int write_protected_data(ProtectedData* pd, uint32_t value)
{
    pd->safety_critical_data = value;
    pd->crc = calculate_crc32(&value, sizeof(value));
    return 0;
}

int read_protected_data(ProtectedData* pd, uint32_t* value)
{
    uint32_t expected_crc = calculate_crc32(&pd->safety_critical_data,
                                            sizeof(pd->safety_critical_data));
    if (expected_crc != pd->crc) {
        return ERROR_DATA_CORRUPTION;
    }
    *value = pd->safety_critical_data;
    return 0;
}
#endif
```

---

## 深入理解

### 1. 多样性与冗余

```c
/* 1oo2 (1 out of 2) 架构 - SIL 3常用 */
typedef struct {
    int32_t channel_a;
    int32_t channel_b;
    uint8_t valid_a;
    uint8_t valid_b;
} DualChannelInput;

int process_1oo2(DualChannelInput* input, int32_t* output)
{
    /* 偏差检查 */
    int32_t diff = abs(input->channel_a - input->channel_b);

    if (diff <= DEVIATION_THRESHOLD) {
        /* 两个通道一致，取平均值 */
        *output = (input->channel_a + input->channel_b) / 2;
        return VOTE_OK;
    }

    /* 通道不一致，选择可信通道 */
    if (input->valid_a && !input->valid_b) {
        *output = input->channel_a;
        return VOTE_DEGRADED_A;
    } else if (!input->valid_a && input->valid_b) {
        *output = input->channel_b;
        return VOTE_DEGRADED_B;
    }

    /* 两个通道都不可信 */
    return VOTE_FAILED;
}

/* 2oo3 (2 out of 3) 架构 - SIL 4常用 */
typedef struct {
    int32_t value[3];
    uint8_t valid[3];
} TripleChannelInput;

int process_2oo3(TripleChannelInput* input, int32_t* output)
{
    int matches = 0;
    int32_t majority_value = 0;

    /* 多数表决 */
    for (int i = 0; i < 3; i++) {
        if (!input->valid[i]) continue;

        for (int j = i + 1; j < 3; j++) {
            if (!input->valid[j]) continue;

            if (abs(input->value[i] - input->value[j]) <= DEVIATION_THRESHOLD) {
                majority_value = (input->value[i] + input->value[j]) / 2;
                matches++;
            }
        }
    }

    if (matches >= 1) {
        *output = majority_value;
        return VOTE_OK;
    }

    return VOTE_FAILED;
}
```

### 2. 安全监控与诊断

```c
/* 安全监控任务 */

/* 诊断覆盖要求：
 * SIL 1: 60% 单点故障诊断覆盖率
 * SIL 2: 90% 单点故障诊断覆盖率
 * SIL 3: 99% 单点故障诊断覆盖率
 * SIL 4: 99% 单点 + 高潜伏故障诊断覆盖率
 */

typedef struct {
    uint32_t diagnostic_tests_run;
    uint32_t diagnostic_failures;
    uint32_t last_execution_time;
    uint8_t system_healthy;
} DiagnosticStatus;

void safety_monitor_task(void)
{
    DiagnosticStatus status = {0};

    while (1) {
        uint32_t start_time = get_system_tick();

        /* 1. CPU自检 */
        if (!cpu_self_test()) {
            handle_cpu_fault();
        }
        status.diagnostic_tests_run++;

        /* 2. RAM测试 (March算法) */
        if (!ram_march_test()) {
            handle_memory_fault();
            status.diagnostic_failures++;
        }

        /* 3. 时钟监控 */
        if (!clock_frequency_check()) {
            handle_clock_fault();
        }

        /* 4. 通信完整性检查 */
        if (!communication_health_check()) {
            handle_comms_fault();
        }

        /* 5. 程序流监控 */
        sequence_monitor_check();

        /* 计算执行时间 */
        status.last_execution_time = get_system_tick() - start_time;

        /* 检查执行时间是否超时 */
        if (status.last_execution_time > MAX_MONITOR_CYCLE_TIME) {
            handle_timing_fault();
        }

        /* 喂看门狗 */
        watchdog_refresh();

        sleep_ms(MONITOR_CYCLE_MS);
    }
}

/* 共因故障防护 */
void diverse_monitoring(void)
{
    /* 使用不同算法的独立监控 */

    /* 监控算法1 */
    int result1 = monitor_algorithm_primary();

    /* 监控算法2 (多样性实现) */
    int result2 = monitor_algorithm_diverse();

    /* 结果比较 */
    if (result1 != result2) {
        /* 检测到共因故障或随机故障 */
        enter_safe_state();
    }
}
```

### 3. 故障注入测试

```c
/* SIL 3/4要求的故障注入测试 */

/* 故障类型枚举 */
typedef enum {
    FAULT_NONE = 0,
    FAULT_BIT_FLIP,       /* 位翻转 */
    FAULT_STUCK_AT_ZERO,  /* 固定为0 */
    FAULT_STUCK_AT_ONE,   /* 固定为1 */
    FAULT_DELAY,          /* 时延故障 */
    FAULT_OMISSION        /* 数据丢失 */
} FaultType;

/* 故障注入框架 */
#ifdef FAULT_INJECTION_ENABLED

uint32_t inject_fault(uint32_t original_data, FaultType fault, int bit_position)
{
    switch (fault) {
        case FAULT_BIT_FLIP:
            return original_data ^ (1 << bit_position);

        case FAULT_STUCK_AT_ZERO:
            return original_data & ~(1 << bit_position);

        case FAULT_STUCK_AT_ONE:
            return original_data | (1 << bit_position);

        case FAULT_DELAY:
            /* 模拟时延 */
            sleep_ms(10);
            return original_data;

        case FAULT_OMISSION:
            return 0;  /* 数据丢失 */

        default:
            return original_data;
    }
}

/* 测试用例 */
void test_fault_detection(void)
{
    SafetyFunctionInput input = { /* ... */ };

    /* 测试无故障情况 */
    TEST_ASSERT_EQUAL(SAFE, safety_function(&input, FAULT_NONE, 0));

    /* 测试位翻转故障检测 */
    for (int bit = 0; bit < 32; bit++) {
        SafetyState result = safety_function(&input, FAULT_BIT_FLIP, bit);
        /* 应该检测到故障或产生安全输出 */
        TEST_ASSERT(result == SAFE || result == FAULT_DETECTED);
    }
}

#endif /* FAULT_INJECTION_ENABLED */
```

---

## 派生标准

IEC 61508是以下行业标准的母标准：

| 标准 | 应用领域 | 与IEC 61508关系 |
|:-----|:---------|:----------------|
| **ISO 26262** | 汽车 | 行业专用改编 |
| **EN 50128** | 铁路信号 | 行业专用改编 |
| **IEC 62304** | 医疗器械软件 | 基于IEC 61508-3 |
| **IEC 61511** | 过程工业 | 行业专用改编 |
| **IEC 62061** | 机械安全 | 与ISO 13849配套 |
| **IEC 60730** | 家用电器 | 简化版要求 |

---

## 认证过程

```
IEC 61508认证步骤:

1. 概念阶段
   ├── 危害分析与风险评估
   ├── 确定安全功能
   └── 分配SIL等级

2. 系统设计与实现
   ├── 硬件设计评估 (SFF, HFT)
   ├── 软件开发 (符合SIL要求的技术)
   └── 集成与测试

3. 验证与确认
   ├── 功能安全评估 (FSA)
   ├── 故障插入测试
   └── 共因故障分析

4. 功能安全评估
   ├── 文档审查
   ├── 现场评估
   └── 颁发证书
```

---

## 权威参考

- [1] IEC 61508-1:2010 - General requirements
- [2] IEC 61508-2:2010 - Requirements for E/E/PE safety-related systems
- [3] IEC 61508-3:2010 - Software requirements
- [4] IEC 61508-4:2010 - Definitions and abbreviations
- [5] "Functional Safety" by David J. Smith & Kenneth G. L. Simpson

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
> **内容深度**: L5 (研究级)
