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
# DO-178C - 航空机载系统软件标准

> **层级定位**: 01_Core_Knowledge_System > 09_Safety_Standards > DO_178C
> **难度级别**: ⭐⭐⭐⭐⭐ L5
> **前置知识**: 软件工程、C语言高级、MISRA C
> **参考标准**: RTCA DO-178C (2011), DO-330, DO-331

---

## 概述

DO-178C（Software Considerations in Airborne Systems and Equipment Certification）是民用航空软件开发和认证的核心标准，由RTCA组织发布。该标准定义了航空软件开发的全生命周期要求，确保机载软件的安全性。

| 属性 | 说明 |
|:-----|:-----|
| **前身** | DO-178B (1992) |
| **当前版本** | DO-178C (2011) |
| **补充标准** | DO-330（工具鉴定）、DO-331（模型开发）、DO-332（面向对象）、DO-333（形式化方法） |
| **适用范围** | 民用航空机载软件 |
| **监管机构** | FAA (美国), EASA (欧洲), CAAC (中国) |

---

## 软件等级 (DAL - Design Assurance Level)

| 等级 | 失效条件 | 故障概率 | 示例系统 |
|:-----|:---------|:---------|:---------|
| **Level A** | 灾难性 (Catastrophic) | < 10⁻⁹ /飞行小时 | 飞行控制、引擎控制FADEC |
| **Level B** | 危险/严重 (Hazardous) | < 10⁻⁷ /飞行小时 | 自动驾驶、飞行管理FMS |
| **Level C** | 重大 (Major) | < 10⁻⁵ /飞行小时 | 通信系统、燃油管理 |
| **Level D** | 较小 (Minor) | < 10⁻³ /飞行小时 | 客舱娱乐、维护系统 |
| **Level E** | 无安全影响 (No Effect) | - | 乘客信息服务 |

---

## 软件生命周期过程

### 1. 软件计划过程

```c
/* 软件计划文档结构示例 */
typedef struct {
    char* software_standard;        /* 软件开发标准 */
    char* verification_standard;    /* 验证标准 */
    char* configuration_management; /* 配置管理计划 */
    char* quality_assurance;        /* 质量保证计划 */
    char* certification_liaison;    /* 认证联络计划 */
} DO178C_Planning_Data;
```

### 2. 软件开发过程

| 过程 | 输入 | 输出 | Level A要求 |
|:-----|:-----|:-----|:------------|
| 软件需求 | 系统需求 | 软件需求规格(SRS) | 双向追溯矩阵 |
| 软件设计 | SRS | 软件设计描述(SDD) | 高层设计+低层设计 |
| 软件编码 | SDD | 源代码 | MISRA C合规 |
| 集成 | 源代码 | 可执行目标码 | 构建记录 |

### 3. 软件验证过程

```c
/* DO-178C Level A 验证要求 */

/* 需求验证：每个需求必须有测试用例 */
#define REQUIREMENT_ID "SR-001"

/* 测试用例必须包含：
 * 1. 测试目的
 * 2. 输入条件
 * 3. 预期结果
 * 4. 通过/失败准则
 */
void test_case_SR001(void) {
    /* Test Purpose: Verify altitude calculation accuracy */
    /* Input: Pressure sensor reading = 101325 Pa */
    /* Expected: Altitude = 0 ft (sea level) */

    float pressure = 101325.0f;
    float altitude = calculate_altitude(pressure);

    /* Pass/Fail Criteria: ±1 ft accuracy */
    TEST_ASSERT_FLOAT_WITHIN(1.0f, 0.0f, altitude);
}
```

---

## 深入理解

### 1. 结构化覆盖要求

DO-178C根据软件等级要求不同程度的代码覆盖：

| 覆盖类型 | Level A | Level B | Level C | Level D |
|:---------|:-------:|:-------:|:-------:|:-------:|
| 语句覆盖 (Statement) | ✓ | ✓ | ✓ | ✓ |
| 判定覆盖 (Decision) | ✓ | ✓ | ✓ | - |
| 修正条件/判定覆盖 (MC/DC) | ✓ | - | - | - |

### 2. MC/DC覆盖详解 (Level A)

MC/DC (Modified Condition/Decision Coverage) 要求每个条件独立影响判定结果。

```c
/* MC/DC示例分析 */
/* 判定: (A && B) || C */

/* 条件真值表：
 * A  B  C  | 结果  | 独立条件
 * ---------+-------+----------
 * T  T  F  |   T   | -
 * F  T  F  |   F   | A (独立影响)
 * T  F  F  |   F   | B (独立影响)
 * F  F  T  |   T   | C (独立影响)
 * T  T  T  |   T   | -
 *
 * 最小测试集: 4个用例覆盖MC/DC
 */

/* 实际航空软件示例 */
int check_flight_envelope(float altitude, float airspeed, int flaps_position)
{
    /* 条件1: 高度在允许范围内 */
    int altitude_ok = (altitude >= MIN_ALTITUDE) && (altitude <= MAX_ALTITUDE);

    /* 条件2: 空速在允许范围内 */
    int airspeed_ok = (airspeed >= MIN_AIRSPEED) && (airspeed <= MAX_AIRSPEED);

    /* 条件3: 襟翼位置有效 */
    int flaps_ok = (flaps_position >= 0) && (flaps_position <= 40);

    /* MC/DC要求: 每个条件独立影响最终结果 */
    return altitude_ok && airspeed_ok && flaps_ok;
}

/* MC/DC测试用例设计 */
void test_mcdc_flight_envelope(void) {
    /* TC1: 正常情况 - 所有条件为真 */
    TEST_ASSERT_EQUAL(1, check_flight_envelope(10000, 250, 20));

    /* TC2: 高度低 - altitude_ok独立为假 */
    TEST_ASSERT_EQUAL(0, check_flight_envelope(-1000, 250, 20));

    /* TC3: 空速高 - airspeed_ok独立为假 */
    TEST_ASSERT_EQUAL(0, check_flight_envelope(10000, 500, 20));

    /* TC4: 襟翼无效 - flaps_ok独立为假 */
    TEST_ASSERT_EQUAL(0, check_flight_envelope(10000, 250, 50));
}
```

### 3. 数据耦合与控制耦合分析

```c
/* Level A要求分析组件间的耦合关系 */

/* 数据耦合示例: 通过参数传递数据 */
int calculate_stall_speed(int aircraft_weight, int flap_setting)
{
    /* 数据耦合: 仅通过参数接收数据 */
    float base_stall = 100.0f;  /* 基础失速速度 */
    float weight_factor = sqrtf(aircraft_weight / 10000.0f);
    float flap_reduction = flap_setting * 0.5f;

    return (int)(base_stall * weight_factor - flap_reduction);
}

/* 控制耦合示例: 避免通过参数控制流程 */
/* 不推荐的做法 */
void process_sensor_data_bad(int sensor_type, void* data)
{
    if (sensor_type == TYPE_PRESSURE) {
        process_pressure(data);
    } else if (sensor_type == TYPE_TEMPERATURE) {
        process_temperature(data);
    }
    /* 控制耦合: 参数控制程序流程 */
}

/* 推荐的做法: 分离接口 */
void process_pressure_data(PressureData* data);
void process_temperature_data(TemperatureData* data);
```

### 4. 形式化方法应用 (DO-333补充)

```c
/* 使用SPARK/Ada或C with ACSL进行形式化验证 */

/* 函数契约示例 (使用ACSL - ANSI/ISO C Specification Language) */
/*@ requires \valid(sensor_data);
    requires sensor_data->value >= SENSOR_MIN;
    requires sensor_data->value <= SENSOR_MAX;
    assigns \nothing;
    ensures \result == 0 ==> sensor_data->valid == 1;
    ensures \result != 0 ==> sensor_data->valid == 0;
*/
int validate_sensor_data(SensorData* sensor_data)
{
    /* 实现 */
}
```

---

## 与MISRA C的关系

| DO-178C等级 | MISRA C:2012要求 |
|:------------|:-----------------|
| Level A/B | 必须符合Mandatory和Required规则 |
| Level C | 推荐符合Required规则 |
| Level D | 建议遵循编码规范 |

```c
/* DO-178C Level A编码规范示例 */

/* Rule 8.13: 如果指针不用于修改对象，应声明为指向const的指针 */
int process_readonly_data(const uint8_t* data, size_t length);

/* Rule 17.7: 函数返回值必须被使用或显式转换void */
(void)memset(buffer, 0, sizeof(buffer));  /* 显式忽略返回值 */

/* Rule 15.5: 每个switch语句至少有一个default标签 */
switch (mode) {
    case MODE_NORMAL: /* ... */ break;
    case MODE_TEST:   /* ... */ break;
    default:          /* 安全处理未定义模式 */
        enter_safe_state();
        break;
}
```

---

## 工具鉴定 (DO-330)

开发工具必须经过鉴定以确保其输出可信：

| 工具分类 | 鉴定要求 | 示例 |
|:---------|:---------|:-----|
| 开发工具 (TQL-5) | 低 | 代码编辑器 |
| 验证工具 (TQL-4) | 中 | 静态分析工具 |
| 自动生成代码 (TQL-3) | 高 | SCADE, Simulink Coder |
| 验证结果判定 (TQL-1) | 最高 | 测试覆盖分析工具 |

---

## 认证过程

```
认证流程:
1. 计划评审 (PSAC - Plan for Software Aspects of Certification)
2. 开发阶段评审
   - 软件需求评审
   - 软件设计评审
   - 代码评审
3. 验证阶段评审
   - 测试程序评审
   - 测试结果评审
4. 最终评审
   - 软件完成总结 (SCM/SQA/SESC)
   - 提交认证机构
```

---

## 权威参考

- [1] RTCA DO-178C: Software Considerations in Airborne Systems and Equipment Certification
- [2] RTCA DO-330: Software Tool Qualification Considerations
- [3] FAA Order 8110.49: Software Approval Guidelines
- [4] "Developing Safety-Critical Software" by Leanna Rierson

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
> **内容深度**: L5 (研究级)
