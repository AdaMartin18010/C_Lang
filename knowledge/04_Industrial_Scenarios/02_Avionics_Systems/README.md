# 航空电子系统 (Avionics Systems)

## 目录

- [航空电子系统 (Avionics Systems)](#航空电子系统-avionics-systems)
  - [目录](#目录)
  - [概述](#概述)
    - [航空电子系统架构](#航空电子系统架构)
    - [安全关键等级 (DAL)](#安全关键等级-dal)
  - [DO-178C 软件认证标准](#do-178c-软件认证标准)
    - [DO-178C 概述](#do-178c-概述)
    - [软件生命周期过程](#软件生命周期过程)
    - [目标与独立性要求](#目标与独立性要求)
    - [数据结构要求示例](#数据结构要求示例)
    - [结构化需求示例](#结构化需求示例)
  - [ARINC 653 分区操作系统](#arinc-653-分区操作系统)
    - [ARINC 653 概述](#arinc-653-概述)
    - [时间分区调度](#时间分区调度)
    - [ARINC 653 APEX API 实现示例](#arinc-653-apex-api-实现示例)
  - [时间触发架构](#时间触发架构)
    - [TTA 概述](#tta-概述)
    - [TTP/C 调度表示例](#ttpc-调度表示例)
  - [容错设计](#容错设计)
    - [容错架构](#容错架构)
    - [表决算法实现](#表决算法实现)
  - [开发实践](#开发实践)
    - [编码标准](#编码标准)
  - [案例分析](#案例分析)
    - [案例：波音787飞控系统架构](#案例波音787飞控系统架构)
  - [参考资源](#参考资源)
    - [标准文档](#标准文档)
    - [开发工具](#开发工具)

---

## 概述

航空电子系统（Avionics，Aviation Electronics）是现代飞机的核心组成部分，负责飞行控制、导航、通信、监控等关键功能。由于涉及生命安全，航空电子软件开发必须遵循严格的认证标准。

### 航空电子系统架构

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        航空电子系统总体架构                              │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                      飞行管理系统 (FMS)                          │   │
│  │              飞行计划、导航、性能优化、制导                      │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐   │
│  │   飞行控制  │  │   导航系统  │  │   通信系统  │  │   监视系统  │   │
│  │   (FCS)     │  │   (NAV)     │  │   (COM)     │  │   (SURV)    │   │
│  │ • 自动驾驶  │  │ • IRS/GPS   │  │ • VHF/HF    │  │ • 雷达      │   │
│  │ • 电传操纵  │  │ • FMS集成   │  │ • SATCOM    │  │ • TCAS      │   │
│  │ • 增稳系统  │  │ • 着陆引导  │  │ • ACARS     │  │ • GPWS      │   │
│  └─────────────┘  └─────────────┘  └─────────────┘  └─────────────┘   │
│                                                                         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐   │
│  │   显示系统  │  │   维护系统  │  │   记录系统  │  │   电气系统  │   │
│  │   (DISP)    │  │   ( Maint ) │  │   (REC)     │  │   (ELEC)    │   │
│  │ • PFD/ND    │  │ • BITE      │  │ • FDR       │  │ • 电源管理  │   │
│  │ • EICAS     │  │ • 故障诊断  │  │ • CVR       │  │ • 配电      │   │
│  └─────────────┘  └─────────────┘  └─────────────┘  └─────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                  航空电子全双工交换以太网 (AFDX)                 │   │
│  │              ARINC 664 / IEEE 802.3 确定性网络                   │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │              综合模块化航空电子 (IMA) 平台                        │   │
│  │         ARINC 653 分区操作系统 + 共享计算资源                     │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 安全关键等级 (DAL)

```
┌─────────────────────────────────────────────────────────────────┐
│              软件设计保证等级 (Design Assurance Level)            │
├─────────┬─────────────────────────┬─────────────────────────────┤
│  等级   │        失效影响          │        典型应用             │
├─────────┼─────────────────────────┼─────────────────────────────┤
│  DAL A  │  灾难性 (Catastrophic)   │  飞控系统、发动机控制        │
│         │  飞机失事，多人死亡       │  自动着陆、电传操纵          │
├─────────┼─────────────────────────┼─────────────────────────────┤
│  DAL B  │  危险 (Hazardous)        │  飞行管理、自动油门          │
│         │  安全裕度大幅降低         │  近地警告系统                │
├─────────┼─────────────────────────┼─────────────────────────────┤
│  DAL C  │  重大 (Major)            │  导航系统、通信系统          │
│         │  安全裕度显著降低         │  座舱显示、维护系统          │
├─────────┼─────────────────────────┼─────────────────────────────┤
│  DAL D  │  较小 (Minor)            │  客舱娱乐、乘客信息系统      │
│         │  安全裕度轻微降低         │  非关键显示                  │
├─────────┼─────────────────────────┼─────────────────────────────┤
│  DAL E  │  无影响 (No Effect)      │  日志记录、测试功能          │
│         │  不影响安全               │  非安全相关功能              │
└─────────┴─────────────────────────┴─────────────────────────────┘
```

---

## DO-178C 软件认证标准

### DO-178C 概述

DO-178C（Software Considerations in Airborne Systems and Equipment Certification）是民用航空软件开发和认证的国际标准，由RTCA组织制定。

```
DO-178C 核心文档结构:

DO-178C
├── 核心标准 (Software Level A-E)
├── 技术补充 (Supplements)
│   ├── DO-330: 软件工具鉴定
│   ├── DO-331: 基于模型的开发
│   ├── DO-332: 面向对象技术
│   ├── DO-333: 形式化方法
│   └── DO-178C/ED-12C: 多核处理器
└── 支持文档
    ├── DO-248C: 讨论和说明
    └── DO-278A: 地面系统软件
```

### 软件生命周期过程

```
┌─────────────────────────────────────────────────────────────────────┐
│                    DO-178C 软件生命周期                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐          │
│  │   软件计划   │───→│   软件开发   │───→│  软件验证    │          │
│  │   过程       │    │   过程       │    │  过程        │          │
│  └──────────────┘    └──────────────┘    └──────────────┘          │
│         │                   │                   │                  │
│         ↓                   ↓                   ↓                  │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐          │
│  │ • 计划制定   │    │ • 需求分析   │    │ • 评审       │          │
│  │ • 标准定义   │    │ • 设计       │    │ • 分析       │          │
│  │ • 工具鉴定   │    │ • 编码       │    │ • 测试       │          │
│  │              │    │ • 集成       │    │              │          │
│  └──────────────┘    └──────────────┘    └──────────────┘          │
│                                                                     │
│  ┌──────────────┐    ┌──────────────┐                              │
│  │ 软件配置管理 │←──→│  软件质量保证│                              │
│  │   过程       │    │   过程       │                              │
│  └──────────────┘    └──────────────┘                              │
│                                                                     │
│  ┌──────────────┐                                                  │
│  │  合格审定联络│ ← 与认证机构的接口                                │
│  └──────────────┘                                                  │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 目标与独立性要求

```
┌─────────────────────────────────────────────────────────────────────┐
│              各DAL等级的目标和独立性要求                             │
├─────────┬─────────┬──────────┬─────────────┬─────────────┬─────────┤
│   DAL   │  总目标 │ 有独立性 │  测试目标   │  验证目标   │ MC/DC   │
├─────────┼─────────┼──────────┼─────────────┼─────────────┼─────────┤
│    A    │   71    │   30     │     14      │     16      │  需要   │
│    B    │   69    │   18     │     14      │     14      │  不需要 │
│    C    │   62    │    8     │     14      │      7      │  不需要 │
│    D    │   28    │    4     │      5      │      5      │  不需要 │
│    E    │    0    │    0     │      0      │      0      │  不需要 │
└─────────┴─────────┴──────────┴─────────────┴─────────────┴─────────┘

独立性要求: 验证人员不能是开发人员
MC/DC: Modified Condition/Decision Coverage（修正条件/判定覆盖）
```

### 数据结构要求示例

```c
/**
 * @file flight_control_types.h
 * @brief 飞行控制系统 - DO-178C DAL A 级数据结构定义
 *
 * 符合 DO-178C DAL A 要求:
 * - 强类型检查
 * - 范围限制
 * - 数据监控
 */

#ifndef FLIGHT_CONTROL_TYPES_H
#define FLIGHT_CONTROL_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * 基本类型定义 - 使用固定宽度整数类型确保可移植性
 * ============================================================================ */

/**
 * @brief 飞行控制系统版本信息
 * @req DO-178C: 配置标识
 */
#define FCS_MAJOR_VERSION   1
#define FCS_MINOR_VERSION   2
#define FCS_PATCH_VERSION   3
#define FCS_VERSION_STRING  "1.2.3"

/* ============================================================================
 * 范围受限的数值类型
 * 使用结构体封装确保范围检查和类型安全
 * ============================================================================ */

/**
 * @brief 高度类型（英尺）
 * @range [-1000, 60000] 英尺
 */
typedef struct {
    int32_t value;          /**< 高度值（英尺） */
    bool valid;             /**< 数据有效性标志 */
    uint32_t timestamp_ms;  /**< 时间戳（毫秒） */
} Altitude_t;

#define ALTITUDE_MIN    (-1000)
#define ALTITUDE_MAX    60000
#define ALTITUDE_INVALID (-9999)

/**
 * @brief 空速类型（节）
 * @range [0, 500] 节
 */
typedef struct {
    uint16_t value;         /**< 空速值（节） */
    bool valid;
    uint32_t timestamp_ms;
} Airspeed_t;

#define AIRSPEED_MIN    0
#define AIRSPEED_MAX    500
#define AIRSPEED_INVALID 0xFFFF

/**
 * @brief 航向类型（度）
 * @range [0, 359.9] 度
 */
typedef struct {
    uint16_t value_tenths;  /**< 航向值（0.1度为单位） */
    bool valid;
    uint32_t timestamp_ms;
} Heading_t;

#define HEADING_MIN     0
#define HEADING_MAX     3599    /* 359.9度 */
#define HEADING_INVALID 0xFFFF

/**
 * @brief 姿态角类型（度）
 * @range [-90, 90] 度（俯仰）
 * @range [-180, 180] 度（横滚）
 */
typedef struct {
    int16_t pitch_tenths;   /**< 俯仰角（0.1度） */
    int16_t roll_tenths;    /**< 横滚角（0.1度） */
    bool valid;
    uint32_t timestamp_ms;
} Attitude_t;

#define PITCH_MIN       (-900)  /* -90度 */
#define PITCH_MAX       900     /* 90度 */
#define ROLL_MIN        (-1800) /* -180度 */
#define ROLL_MAX        1800    /* 180度 */

/* ============================================================================
 * 控制面指令类型
 * ============================================================================ */

/**
 * @brief 控制面位置指令
 * @range [-100, 100] 百分比
 */
typedef struct {
    int8_t position_percent;    /**< 位置指令（%） */
    bool engaged;               /**< 作动器使能 */
    uint8_t actuator_id;        /**< 作动器标识 */
    uint32_t command_id;        /**< 指令序列号（用于追踪） */
} ControlSurfaceCommand_t;

#define CONTROL_SURFACE_MIN     (-100)
#define CONTROL_SURFACE_MAX     100
#define CONTROL_SURFACE_NEUTRAL 0

/**
 * @brief 飞行模式枚举
 */
typedef enum {
    FLIGHT_MODE_MANUAL = 0,     /**< 手动模式 */
    FLIGHT_MODE_STABILITY,      /**< 增稳模式 */
    FLIGHT_MODE_ALT_HOLD,       /**< 高度保持 */
    FLIGHT_MODE_HDG_HOLD,       /**< 航向保持 */
    FLIGHT_MODE_VNAV,           /**< 垂直导航 */
    FLIGHT_MODE_LNAV,           /**< 水平导航 */
    FLIGHT_MODE_APPROACH,       /**< 进近模式 */
    FLIGHT_MODE_LAND,           /**< 着陆模式 */
    FLIGHT_MODE_MAX             /**< 枚举边界检查 */
} FlightMode_t;

/* ============================================================================
 * 安全监控数据结构
 * ============================================================================ */

/**
 * @brief 通道监控状态
 * DO-178C DAL A 要求多通道交叉监控
 */
typedef struct {
    uint8_t channel_id;         /**< 通道标识 */
    bool healthy;               /**< 健康状态 */
    uint32_t last_update_ms;    /**< 最后更新时间 */
    uint16_t error_count;       /**< 错误计数 */
    uint16_t disagreement_count; /**< 不一致计数 */
} ChannelStatus_t;

#define MAX_CHANNELS    3
#define CHANNEL_A       0
#define CHANNEL_B       1
#define CHANNEL_C       2

/**
 * @brief 系统健康监控
 */
typedef struct {
    ChannelStatus_t channels[MAX_CHANNELS];
    uint32_t active_channel;    /**< 当前主通道 */
    bool system_degraded;       /**< 系统降级标志 */
    bool system_failed;         /**< 系统失效标志 */
    uint32_t monitor_timestamp; /**< 监控时间戳 */
} SystemHealth_t;

/* ============================================================================
 * 数据验证函数声明
 * ============================================================================ */

/**
 * @brief 验证高度值是否在有效范围内
 * @param alt 高度值
 * @return true 如果有效
 */
static inline bool Altitude_IsValid(const Altitude_t *alt) {
    return (alt != NULL) &&
           alt->valid &&
           (alt->value >= ALTITUDE_MIN) &&
           (alt->value <= ALTITUDE_MAX);
}

/**
 * @brief 验证空速值是否在有效范围内
 */
static inline bool Airspeed_IsValid(const Airspeed_t *speed) {
    return (speed != NULL) &&
           speed->valid &&
           (speed->value <= AIRSPEED_MAX);
}

/**
 * @brief 验证航向值是否在有效范围内
 */
static inline bool Heading_IsValid(const Heading_t *hdg) {
    return (hdg != NULL) &&
           hdg->valid &&
           (hdg->value_tenths <= HEADING_MAX);
}

/**
 * @brief 验证飞行模式枚举值
 */
static inline bool FlightMode_IsValid(FlightMode_t mode) {
    return (mode >= FLIGHT_MODE_MANUAL) && (mode < FLIGHT_MODE_MAX);
}

/**
 * @brief 验证控制面指令
 */
static inline bool ControlCommand_IsValid(const ControlSurfaceCommand_t *cmd) {
    return (cmd != NULL) &&
           (cmd->position_percent >= CONTROL_SURFACE_MIN) &&
           (cmd->position_percent <= CONTROL_SURFACE_MAX);
}

/* ============================================================================
 * 数据监控宏（运行时断言）
 * ============================================================================ */

#ifdef FCS_ENABLE_MONITORING
    #define FCS_MONITOR(condition, error_code) \
        do { \
            if (!(condition)) { \
                FCS_ReportError(error_code, __FILE__, __LINE__); \
            } \
        } while(0)
#else
    #define FCS_MONITOR(condition, error_code) ((void)0)
#endif

/* 错误码定义 */
typedef enum {
    FCS_ERR_NONE = 0,
    FCS_ERR_INVALID_ALTITUDE = 0x01,
    FCS_ERR_INVALID_AIRSPEED = 0x02,
    FCS_ERR_INVALID_HEADING = 0x03,
    FCS_ERR_INVALID_ATTITUDE = 0x04,
    FCS_ERR_CHANNEL_DISAGREE = 0x10,
    FCS_ERR_SYSTEM_FAILURE = 0xFF
} FcsErrorCode_t;

#endif /* FLIGHT_CONTROL_TYPES_H */
```

### 结构化需求示例

```c
/**
 * @file fcs_requirements.h
 * @brief 飞行控制系统高层需求 (HLR)
 *
 * 符合 DO-178C 需求可追溯性要求
 * 每个需求都有唯一的标识符和验证方法
 */

#ifndef FCS_REQUIREMENTS_H
#define FCS_REQUIREMENTS_H

/* ============================================================================
 * 高层需求 HLR-FCS-001 至 HLR-FCS-050
 * ============================================================================ */

/**
 * @req HLR-FCS-001
 * @brief 俯仰控制需求
 * @description
 *   飞行控制系统应控制飞机俯仰姿态，俯仰角范围 ±25度。
 *   控制精度：±0.5度（1σ）。
 * @safety_level DAL A
 * @allocation 飞控计算机
 * @verification 测试 (HLR测试用例)
 * @trace_to SSR-FCS-001, SSR-FCS-002
 */
#define HLR_FCS_001_PITCH_CONTROL \
    "FCS shall control aircraft pitch attitude within ±25 degrees"

/**
 * @req HLR-FCS-002
 * @brief 横滚控制需求
 * @description
 *   飞行控制系统应控制飞机横滚姿态，横滚角范围 ±67度。
 *   控制精度：±1.0度（1σ）。
 * @safety_level DAL A
 * @allocation 飞控计算机
 * @verification 测试
 */
#define HLR_FCS_002_ROLL_CONTROL \
    "FCS shall control aircraft roll attitude within ±67 degrees"

/**
 * @req HLR-FCS-003
 * @brief 失速保护
 * @description
 *   当迎角超过临界值时，系统应自动推杆降低机头，
 *   防止飞机进入失速状态。
 * @safety_level DAL A
 * @allocation 飞控计算机
 * @verification 测试 + 分析
 */
#define HLR_FCS_003_STALL_PROTECTION \
    "FCS shall provide automatic stall protection when AOA exceeds threshold"

/**
 * @req HLR-FCS-004
 * @brief 过载限制
 * @description
 *   飞行控制系统应限制法向过载在 [-1.0g, +2.5g] 范围内，
 *   防止结构损坏。
 * @safety_level DAL A
 * @allocation 飞控计算机
 * @verification 测试
 */
#define HLR_FCS_004_LOAD_FACTOR_LIMIT \
    "FCS shall limit normal load factor to [-1.0g, +2.5g]"

/**
 * @req HLR-FCS-005
 * @brief 多通道冗余
 * @description
 *   飞行控制系统应采用三通道冗余架构，
 *   任一通道失效时系统继续正常工作。
 * @safety_level DAL A
 * @allocation 飞控计算机硬件
 * @verification 测试 + 分析
 */
#define HLR_FCS_005_TRIPLE_REDUNDANCY \
    "FCS shall implement triple modular redundancy"

/**
 * @req HLR-FCS-006
 * @brief 通道监控
 * @description
 *   系统应持续监控各通道输出，
 *   当检测到通道故障时自动隔离故障通道。
 * @safety_level DAL A
 * @allocation 飞控软件
 * @verification 测试
 */
#define HLR_FCS_006_CHANNEL_MONITORING \
    "FCS shall monitor channel outputs and isolate faulty channels"

/**
 * @req HLR-FCS-007
 * @brief 飞行员超控
 * @description
 *   飞行员应能通过施加足够的杆力超控自动驾驶，
 *   超控力不应超过 50磅。
 * @safety_level DAL A
 * @allocation 飞控系统
 * @verification 测试
 */
#define HLR_FCS_007_PILOT_OVERRIDE \
    "FCS shall allow pilot override with force not exceeding 50 lbf"

/* ============================================================================
 * 低层需求示例（软件实现需求）
 * ============================================================================ */

/**
 * @req LLR-FCS-001
 * @brief 俯仰控制律计算
 * @description
 *   俯仰控制律应按以下公式计算：
 *   δe = Kq * q + K_theta * (theta_cmd - theta) + ...
 *   其中 δe 为升降舵指令，单位：度
 * @parent_req HLR-FCS-001
 * @safety_level DAL A
 * @verification 评审 + 测试
 */

/**
 * @req LLR-FCS-002
 * @brief 控制指令范围检查
 * @description
 *   所有控制面输出指令应进行范围检查，
 *   确保在 [-100%, +100%] 范围内。
 *   超出范围时限制在边界值并报告错误。
 * @parent_req HLR-FCS-001
 * @safety_level DAL A
 * @verification 测试
 */

#endif /* FCS_REQUIREMENTS_H */
```

---

## ARINC 653 分区操作系统

### ARINC 653 概述

ARINC 653（Avionics Application Standard Software Interface）定义了航空电子软件的标准化接口，支持时间和空间分区，确保不同安全等级软件在同一处理器上安全共存。

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    ARINC 653 分区架构                                    │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    应用层 (APEX接口)                             │   │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐   │   │
│  │  │Process 1│ │Process 2│ │Process 3│ │Process 4│ │Process 5│   │   │
│  │  │  DAL A  │ │  DAL A  │ │  DAL B  │ │  DAL C  │ │  DAL D  │   │   │
│  │  └────┬────┘ └────┬────┘ └────┬────┘ └────┬────┘ └────┬────┘   │   │
│  │       └───────────┴───────────┴───────────┴───────────┘        │   │
│  │                              │                                  │   │
│  │                    APEX API (进程管理、通信、时间管理)          │   │
│  └──────────────────────────────┼──────────────────────────────────┘   │
│                                 │                                       │
│  ┌──────────────────────────────┼──────────────────────────────────┐   │
│  │                    核心操作系统 (Core OS)                        │   │
│  │  ┌───────────────────────────┴──────────────────────────────┐   │   │
│  │  │              分区调度器 (Partition Scheduler)             │   │   │
│  │  │                                                           │   │   │
│  │  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  │   │   │
│  │  │  │Partition1│  │Partition2│  │Partition3│  │Partition4│  │   │   │
│  │  │  │  FCS     │  │   FMS    │  │   NAV    │  │  Maint   │  │   │   │
│  │  │  │  DAL A   │  │  DAL B   │  │  DAL C   │  │  DAL D   │  │   │   │
│  │  │  │          │  │          │  │          │  │          │  │   │   │
│  │  │  │ 10ms窗口 │  │ 20ms窗口 │  │ 10ms窗口 │  │ 剩余时间 │  │   │   │
│  │  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘  │   │   │
│  │  │                                                           │   │   │
│  │  │        ←─── 主时间框架 (Major Time Frame) ───→           │   │   │
│  │  └───────────────────────────────────────────────────────────┘   │   │
│  │                                                                   │   │
│  │  ┌───────────────────────────────────────────────────────────┐   │   │
│  │  │              健康监控 (Health Monitor)                    │   │   │
│  │  │     分区错误处理、故障恢复、多级错误管理                   │   │   │
│  │  └───────────────────────────────────────────────────────────┘   │   │
│  └───────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │              硬件抽象层 (HAL) / 板级支持包 (BSP)                 │   │
│  │              CPU、内存、定时器、中断控制器                        │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 时间分区调度

```
┌─────────────────────────────────────────────────────────────────────┐
│              ARINC 653 主时间框架调度示例                            │
│                    (周期 = 50ms)                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  时间轴: 0ms     10ms    20ms    30ms    40ms    50ms    60ms       │
│          │       │       │       │       │       │       │         │
│          ↓       ↓       ↓       ↓       ↓       ↓       ↓         │
│  Partition1 ████│       │████   │       │████   │       │████     │
│  (FCS/DAL A)    │       │       │       │       │       │         │
│          0-10ms │       │30-40ms│       │       │       │         │
│                                                                     │
│  Partition2     │████████████│       │       │████████████│       │
│  (FMS/DAL B)    │            │       │       │            │       │
│                 │   10-30ms  │       │       │   50-70ms  │       │
│                                                                     │
│  Partition3     │       │       │████████│       │       │████    │
│  (NAV/DAL C)    │       │       │        │       │       │        │
│                 │       │       │ 20-30ms│40-50ms│       │        │
│                                                                     │
│  Partition4     │       │       │       │████████│       │        │
│  (Maint/DAL D)  │       │       │       │        │       │        │
│                 │       │       │       │ 40-50ms│       │        │
│                                                                     │
│  Idle           │       │       │       │   ██   │       │   ██   │
│  (空闲)         │       │       │       │   空闲  │       │   空闲  │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│  特点:                                                              │
│  • 时间确定性：每个分区在固定时间窗口执行                           │
│  • 空间隔离：分区间内存隔离，防止错误传播                           │
│  • 健康监控：分区错误不会导致整个系统崩溃                           │
│  • 优先级：高安全等级分区获得更多时间资源                           │
└─────────────────────────────────────────────────────────────────────┘
```

### ARINC 653 APEX API 实现示例

```c
/**
 * @file apex_api.c
 * @brief ARINC 653 APEX API 核心实现示例
 *
 * 展示APEX接口的核心概念，简化版实现
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================================================
 * ARINC 653 标准类型定义
 * ============================================================================ */

/* 返回码 */
typedef enum {
    NO_ERROR = 0,
    NO_ACTION = 1,
    NOT_AVAILABLE = 2,
    INVALID_PARAM = 3,
    INVALID_CONFIG = 4,
    INVALID_MODE = 5,
    TIMED_OUT = 6
} RETURN_CODE_TYPE;

/* 分区状态 */
typedef enum {
    IDLE = 0,
    COLD_START = 1,
    WARM_START = 2,
    NORMAL = 3
} OPERATING_MODE_TYPE;

/* 进程状态 */
typedef enum {
    DORMANT = 0,
    READY = 1,
    RUNNING = 2,
    WAITING = 3
    /* 其他状态... */
} PROCESS_STATE_TYPE;

/* 进程优先级 */
typedef int32_t PRIORITY_TYPE;
#define MAX_PRIORITY 31
#define MIN_PRIORITY 1
#define DEFAULT_PRIORITY 10

/* 系统时间类型（纳秒）*/
typedef uint64_t SYSTEM_TIME_TYPE;

/* 分区标识符 */
typedef uint32_t PARTITION_ID_TYPE;
#define MAX_PARTITIONS 32

/* 进程标识符 */
typedef uint32_t PROCESS_ID_TYPE;
#define MAX_PROCESSES_PER_PARTITION 128

/* 消息大小 */
typedef uint32_t MESSAGE_SIZE_TYPE;
#define MAX_MESSAGE_SIZE 8192

/* ============================================================================
 * 分区管理数据结构
 * ============================================================================ */

/**
 * @brief 分区属性
 */
typedef struct {
    PARTITION_ID_TYPE identifier;
    char name[32];
    SYSTEM_TIME_TYPE period;            /* 分区周期 */
    SYSTEM_TIME_TYPE duration;          /* 分区执行时间 */
    OPERATING_MODE_TYPE operating_mode;
    uint32_t start_condition;
} PARTITION_STATUS_TYPE;

/**
 * @brief 进程属性
 */
typedef struct {
    PROCESS_ID_TYPE id;
    char name[32];
    SYSTEM_TIME_TYPE period;
    SYSTEM_TIME_TYPE time_capacity;     /* 时间容量（WCET） */
    PRIORITY_TYPE current_priority;
    PRIORITY_TYPE base_priority;
    PROCESS_STATE_TYPE process_state;
    void (*entry_point)(void);          /* 入口函数 */
    uint32_t stack_size;
} PROCESS_ATTRIBUTE_TYPE;

typedef PROCESS_ATTRIBUTE_TYPE* PROCESS_ATTRIBUTE_TYPE_PTR;

/**
 * @brief 进程状态信息
 */
typedef struct {
    PROCESS_ATTRIBUTE_TYPE attr;
    SYSTEM_TIME_TYPE deadline_time;
    uint32_t context_switches;
} PROCESS_STATUS_TYPE;

/* ============================================================================
 * 分区控制接口
 * ============================================================================ */

static PARTITION_STATUS_TYPE g_partitions[MAX_PARTITIONS];
static uint32_t g_num_partitions = 0;

/**
 * @brief 获取当前分区状态
 * @param status 输出分区状态
 * @return RETURN_CODE_TYPE 返回码
 */
RETURN_CODE_TYPE GET_PARTITION_STATUS(PARTITION_STATUS_TYPE *status) {
    if (status == NULL) {
        return INVALID_PARAM;
    }

    /* 获取当前分区ID（假设当前为分区0） */
    PARTITION_ID_TYPE current_id = 0;

    memcpy(status, &g_partitions[current_id], sizeof(PARTITION_STATUS_TYPE));
    return NO_ERROR;
}

/**
 * @brief 设置分区操作模式
 * @param operating_mode 目标模式
 */
RETURN_CODE_TYPE SET_PARTITION_MODE(OPERATING_MODE_TYPE operating_mode) {
    if (operating_mode > NORMAL) {
        return INVALID_PARAM;
    }

    PARTITION_ID_TYPE current_id = 0;
    g_partitions[current_id].operating_mode = operating_mode;

    printf("[APEX] Partition %d mode set to %d\n", current_id, operating_mode);
    return NO_ERROR;
}

/* ============================================================================
 * 进程管理接口
 * ============================================================================ */

static PROCESS_STATUS_TYPE g_processes[MAX_PROCESSES_PER_PARTITION];
static uint32_t g_num_processes = 0;

/**
 * @brief 创建进程
 * @param name 进程名称
 * @param attributes 进程属性
 * @param process_id 输出进程ID
 */
RETURN_CODE_TYPE CREATE_PROCESS(
    const char *name,
    PROCESS_ATTRIBUTE_TYPE_PTR attributes,
    PROCESS_ID_TYPE *process_id)
{
    if (name == NULL || attributes == NULL || process_id == NULL) {
        return INVALID_PARAM;
    }

    if (g_num_processes >= MAX_PROCESSES_PER_PARTITION) {
        return INVALID_CONFIG;
    }

    /* 分配进程ID */
    *process_id = g_num_processes;

    /* 复制进程属性 */
    memcpy(&g_processes[*process_id].attr, attributes, sizeof(PROCESS_ATTRIBUTE_TYPE));
    strncpy(g_processes[*process_id].attr.name, name, 31);
    g_processes[*process_id].attr.name[31] = '\0';
    g_processes[*process_id].attr.id = *process_id;

    /* 初始化状态 */
    g_processes[*process_id].attr.process_state = DORMANT;
    g_processes[*process_id].deadline_time = 0;
    g_processes[*process_id].context_switches = 0;

    g_num_processes++;

    printf("[APEX] Process '%s' created with ID %d\n", name, *process_id);
    return NO_ERROR;
}

/**
 * @brief 设置进程优先级
 */
RETURN_CODE_TYPE SET_PRIORITY(PROCESS_ID_TYPE process_id, PRIORITY_TYPE priority) {
    if (process_id >= g_num_processes) {
        return INVALID_PARAM;
    }

    if (priority < MIN_PRIORITY || priority > MAX_PRIORITY) {
        return INVALID_PARAM;
    }

    g_processes[process_id].attr.current_priority = priority;
    return NO_ERROR;
}

/**
 * @brief 挂起当前进程至下一个周期
 */
RETURN_CODE_TYPE PERIODIC_WAIT(void) {
    /* 模拟挂起操作 */
    printf("[APEX] Process periodic wait\n");
    /* 实际实现：触发上下文切换，等待下一个周期 */
    return NO_ERROR;
}

/**
 * @brief 获取进程ID
 */
RETURN_CODE_TYPE GET_MY_ID(PROCESS_ID_TYPE *process_id) {
    if (process_id == NULL) {
        return INVALID_PARAM;
    }
    /* 假设当前进程ID为0 */
    *process_id = 0;
    return NO_ERROR;
}

/* ============================================================================
 * 时间管理接口
 * ============================================================================ */

/**
 * @brief 获取当前系统时间
 */
RETURN_CODE_TYPE GET_TIME(SYSTEM_TIME_TYPE *system_time) {
    if (system_time == NULL) {
        return INVALID_PARAM;
    }

    /* 模拟获取时间 */
    static SYSTEM_TIME_TYPE simulated_time = 0;
    simulated_time += 1000000; /* 1ms增量 */
    *system_time = simulated_time;

    return NO_ERROR;
}

/**
 * @brief 相对时间延迟
 * @param delay_time 延迟时间（纳秒）
 */
RETURN_CODE_TYPE TIMED_WAIT(SYSTEM_TIME_TYPE delay_time) {
    printf("[APEX] Timed wait for %llu ns\n", (unsigned long long)delay_time);
    /* 实际实现：设置定时器，挂起进程 */
    return NO_ERROR;
}

/* ============================================================================
 * 进程间通信 (IPC) - 采样端口示例
 * ============================================================================ */

/**
 * @brief 采样端口配置
 */
typedef struct {
    char name[32];
    MESSAGE_SIZE_TYPE max_message_size;
    SYSTEM_TIME_TYPE refresh_period;    /* 数据刷新周期 */
} SAMPLING_PORT_CONFIG_TYPE;

/**
 * @brief 采样端口状态
 */
typedef struct {
    SAMPLING_PORT_CONFIG_TYPE config;
    uint8_t buffer[MAX_MESSAGE_SIZE];
    MESSAGE_SIZE_TYPE message_size;
    SYSTEM_TIME_TYPE last_write_time;
    bool valid;
} SAMPLING_PORT_TYPE;

static SAMPLING_PORT_TYPE g_sampling_ports[64];
static uint32_t g_num_sampling_ports = 0;

/**
 * @brief 创建采样端口
 */
RETURN_CODE_TYPE CREATE_SAMPLING_PORT(
    const char *name,
    MESSAGE_SIZE_TYPE max_message_size,
    SYSTEM_TIME_TYPE refresh_period,
    uint32_t *port_id)
{
    if (g_num_sampling_ports >= 64) {
        return INVALID_CONFIG;
    }

    *port_id = g_num_sampling_ports;
    strncpy(g_sampling_ports[*port_id].config.name, name, 31);
    g_sampling_ports[*port_id].config.max_message_size = max_message_size;
    g_sampling_ports[*port_id].config.refresh_period = refresh_period;
    g_sampling_ports[*port_id].valid = false;

    g_num_sampling_ports++;

    printf("[APEX] Sampling port '%s' created with ID %d\n", name, *port_id);
    return NO_ERROR;
}

/**
 * @brief 向采样端口写入消息
 */
RETURN_CODE_TYPE WRITE_SAMPLING_MESSAGE(
    uint32_t port_id,
    const uint8_t *message,
    MESSAGE_SIZE_TYPE message_size)
{
    if (port_id >= g_num_sampling_ports) {
        return INVALID_PARAM;
    }

    if (message_size > g_sampling_ports[port_id].config.max_message_size) {
        return INVALID_CONFIG;
    }

    /* 写入消息 */
    memcpy(g_sampling_ports[port_id].buffer, message, message_size);
    g_sampling_ports[port_id].message_size = message_size;
    GET_TIME(&g_sampling_ports[port_id].last_write_time);
    g_sampling_ports[port_id].valid = true;

    return NO_ERROR;
}

/**
 * @brief 从采样端口读取消息
 */
RETURN_CODE_TYPE READ_SAMPLING_MESSAGE(
    uint32_t port_id,
    uint8_t *message,
    MESSAGE_SIZE_TYPE *message_size,
    SYSTEM_TIME_TYPE *time_stamp,
    RETURN_CODE_TYPE *return_code)
{
    if (port_id >= g_num_sampling_ports) {
        *return_code = INVALID_PARAM;
        return *return_code;
    }

    if (!g_sampling_ports[port_id].valid) {
        *return_code = NOT_AVAILABLE;
        return *return_code;
    }

    /* 复制消息 */
    memcpy(message, g_sampling_ports[port_id].buffer,
           g_sampling_ports[port_id].message_size);
    *message_size = g_sampling_ports[port_id].message_size;
    *time_stamp = g_sampling_ports[port_id].last_write_time;

    *return_code = NO_ERROR;
    return NO_ERROR;
}

/* ============================================================================
 * 使用示例
 * ============================================================================ */

void flight_control_task(void) {
    printf("[FCS Task] Running flight control algorithm\n");
    /* 飞控算法实现 */
}

void navigation_task(void) {
    printf("[NAV Task] Running navigation update\n");
    /* 导航算法实现 */
}

int main(void) {
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║        ARINC 653 APEX API 演示                          ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n\n");

    /* 初始化分区 */
    g_partitions[0].identifier = 0;
    strcpy(g_partitions[0].name, "FCS_Partition");
    g_partitions[0].period = 10000000;      /* 10ms */
    g_partitions[0].duration = 5000000;     /* 5ms */
    g_partitions[0].operating_mode = NORMAL;
    g_num_partitions = 1;

    /* 创建飞控进程 */
    PROCESS_ATTRIBUTE_TYPE fcs_attr = {
        .period = 10000000,                 /* 10ms周期 */
        .time_capacity = 5000000,           /* 5ms WCET */
        .base_priority = 20,
        .current_priority = 20,
        .entry_point = flight_control_task,
        .stack_size = 65536
    };

    PROCESS_ID_TYPE fcs_pid;
    CREATE_PROCESS("FCS_Process", &fcs_attr, &fcs_pid);

    /* 创建导航进程 */
    PROCESS_ATTRIBUTE_TYPE nav_attr = {
        .period = 20000000,                 /* 20ms周期 */
        .time_capacity = 3000000,           /* 3ms WCET */
        .base_priority = 15,
        .current_priority = 15,
        .entry_point = navigation_task,
        .stack_size = 32768
    };

    PROCESS_ID_TYPE nav_pid;
    CREATE_PROCESS("NAV_Process", &nav_attr, &nav_pid);

    /* 创建采样端口 */
    uint32_t sensor_port;
    CREATE_SAMPLING_PORT("Sensor_Data", 256, 10000000, &sensor_port);

    /* 模拟写入传感器数据 */
    uint8_t sensor_data[] = {0x01, 0x02, 0x03, 0x04};
    WRITE_SAMPLING_MESSAGE(sensor_port, sensor_data, sizeof(sensor_data));

    printf("\nARINC 653分区操作系统初始化完成\n");

    return 0;
}
```

---

## 时间触发架构

### TTA 概述

时间触发架构（Time-Triggered Architecture, TTA）是一种确定性架构，所有活动由全局时间基准触发，提供高度可预测性和容错能力。

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    时间触发架构 (TTA) 总览                              │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                     全局时间基准 (Global Time Base)              │   │
│  │                 通过时钟同步协议（如TTP/C）同步                  │   │
│  │                     精度：微秒级                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                  TDMA 通信调度 (Time Division Multiple Access)   │   │
│  │                                                                 │   │
│  │  节点1    节点2    节点3    节点4    节点1    节点2             │   │
│  │  ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐                 │   │
│  │  │███│   │   │   │   │   │   │   │   │   │   │  TDMA轮询1      │   │
│  │  └───┘   └───┘   └───┘   └───┘   └───┘   └───┘                 │   │
│  │  ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐                 │   │
│  │  │   │   │███│   │   │   │   │   │   │   │   │  TDMA轮询2      │   │
│  │  └───┘   └───┘   └───┘   └───┘   └───┘   └───┘                 │   │
│  │  ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐                 │   │
│  │  │   │   │   │   │███│   │   │   │   │   │   │  TDMA轮询3      │   │
│  │  └───┘   └───┘   └───┘   └───┘   └───┘   └───┘                 │   │
│  │                                                                 │   │
│  │  ←─────────────── TDMA轮询周期 ───────────────→                 │   │
│  │                                                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                   节点架构 (TTP/C节点)                           │   │
│  │                                                                 │   │
│  │  ┌───────────┐  ┌───────────┐  ┌───────────┐                   │   │
│  │  │  主机应用  │  │  主机应用  │  │  主机应用  │   应用层          │   │
│  │  │  (C代码)  │  │  (C代码)  │  │  (C代码)  │                   │   │
│  │  └─────┬─────┘  └─────┬─────┘  └─────┬─────┘                   │   │
│  │        │              │              │                          │   │
│  │  ┌─────▼──────────────▼──────────────▼─────┐                   │   │
│  │  │          通信网络接口 (CNI)              │   服务层          │   │
│  │  │    消息调度、时钟同步、成员管理          │                   │   │
│  │  └─────┬──────────────────────────┬────────┘                   │   │
│  │        │                          │                            │   │
│  │  ┌─────▼─────┐              ┌─────▼─────┐                     │   │
│  │  │  通信控制器│              │  通信控制器│   物理层            │   │
│  │  │  (TTP/C)  │              │  (TTP/C)  │   双通道冗余        │   │
│  │  └─────┬─────┘              └─────┬─────┘                     │   │
│  │        │                          │                            │   │
│  │  ══════╪══════════════════════════╪══════  总线A              │   │
│  │        │                          │                            │   │
│  │  ══════╪══════════════════════════╪══════  总线B (冗余)       │   │
│  │        │                          │                            │   │
│  └────────┼──────────────────────────┼────────────────────────────┘   │
│           │                          │                                 │
│           └──────────────────────────┘                                 │
│                      双冗余通信总线                                    │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### TTP/C 调度表示例

```c
/**
 * @file ttp_schedule.h
 * @brief TTP/C 时间触发协议调度表定义
 *
 * TTP/C（Time-Triggered Protocol）用于确定性的分布式实时系统
 */

#ifndef TTP_SCHEDULE_H
#define TTP_SCHEDULE_H

#include <stdint.h>

/* TTP/C 参数定义 */
#define TTP_NODE_COUNT      4       /* 网络节点数量 */
#define TTP_SLOT_COUNT      16      /* TDMA轮询中的时隙数 */
#define TTP_CYCLE_TIME_US   2500    /* TDMA轮询周期：2.5ms */
#define TTP_SLOT_TIME_US    156     /* 每个时隙：156.25us */

/**
 * @brief 消息描述符（ MEDL - Message Descriptor List）
 *
 * 定义每个时隙的传输属性
 */
typedef struct {
    uint8_t node_id;            /* 发送节点ID */
    uint16_t frame_id;          /* 帧标识 */
    uint8_t data_length;        /* 数据长度（字节） */
    uint16_t offset_us;         /* 时隙内偏移（微秒） */
    uint8_t flags;              /* 标志位 */
    #define TTP_FLAG_TX     0x01    /* 发送时隙 */
    #define TTP_FLAG_RX     0x02    /* 接收时隙 */
    #define TTP_FLAG_CR     0x04    /* 时钟校正时隙 */
    #define TTP_FLAG_CS     0x08    /* 成员服务时隙 */
} TtpSlotDescriptor;

/**
 * @brief TTP/C MEDL 调度表
 *
 * 这个调度表定义了4个节点的TDMA轮询
 * 每个节点每轮询周期获得4个时隙
 */
static const TtpSlotDescriptor g_ttp_schedule[TTP_SLOT_COUNT] = {
    /* 时隙0-3: 节点0的时隙 */
    {0, 0x100, 16, 0,   TTP_FLAG_TX | TTP_FLAG_CR},  /* 节点0发送，时钟校正 */
    {0, 0x101, 16, 0,   TTP_FLAG_TX},
    {0, 0x102, 16, 0,   TTP_FLAG_TX},
    {0, 0x103, 16, 0,   TTP_FLAG_TX | TTP_FLAG_CS},  /* 成员服务 */

    /* 时隙4-7: 节点1的时隙 */
    {1, 0x200, 16, 0,   TTP_FLAG_TX | TTP_FLAG_CR},
    {1, 0x201, 16, 0,   TTP_FLAG_TX},
    {1, 0x202, 16, 0,   TTP_FLAG_TX},
    {1, 0x203, 16, 0,   TTP_FLAG_TX},

    /* 时隙8-11: 节点2的时隙 */
    {2, 0x300, 16, 0,   TTP_FLAG_TX | TTP_FLAG_CR},
    {2, 0x301, 16, 0,   TTP_FLAG_TX},
    {2, 0x302, 16, 0,   TTP_FLAG_TX},
    {2, 0x303, 16, 0,   TTP_FLAG_TX},

    /* 时隙12-15: 节点3的时隙 */
    {3, 0x400, 16, 0,   TTP_FLAG_TX | TTP_FLAG_CR},
    {3, 0x401, 16, 0,   TTP_FLAG_TX},
    {3, 0x402, 16, 0,   TTP_FLAG_TX},
    {3, 0x403, 16, 0,   TTP_FLAG_TX},
};

/**
 * @brief TTP/C 帧格式
 */
typedef struct __attribute__((packed)) {
    uint8_t header;             /* 帧头 */
    uint16_t frame_id;          /* 帧标识 */
    uint8_t node_id;            /* 发送节点ID */
    uint32_t global_time;       /* 全局时间戳 */
    uint8_t data[16];           /* 有效载荷 */
    uint16_t crc;               /* CRC校验 */
} TtpFrame;

/**
 * @brief 时钟同步参数
 */
typedef struct {
    int32_t clock_correction;   /* 时钟校正值（微秒） */
    uint32_t local_time;        /* 本地时间 */
    uint32_t global_time;       /* 全局时间 */
    uint8_t sync_quality;       /* 同步质量 */
} TtpClockSync;

#endif /* TTP_SCHEDULE_H */
```

---

## 容错设计

### 容错架构

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    航空电子容错架构                                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                     三模冗余 (TMR) 架构                          │   │
│  │                                                                 │   │
│  │  传感器输入 ──→ ┌─────────┐                                     │   │
│  │                 │通道A (主)│ ──┐                                 │   │
│  │                 └─────────┘   │                                 │   │
│  │                       ↓       │     ┌─────────┐                 │   │
│  │  传感器输入 ──→ ┌─────────┐   └────→│  表决器 │ → 输出          │   │
│  │                 │通道B (热备)│ ─────→│(Voter)  │                 │   │
│  │                 └─────────┘   ┌────→│         │                 │   │
│  │                       ↓       │     └─────────┘                 │   │
│  │  传感器输入 ──→ ┌─────────┐   │                                 │   │
│  │                 │通道C (监控)│ ──┘                                 │   │
│  │                 └─────────┘                                     │   │
│  │                                                                 │   │
│  │  特点:                                                          │   │
│  │  • 三个独立通道同时运行                                         │   │
│  │  • 输出采用多数表决（2-out-of-3）                               │   │
│  │  • 单点故障不会导致系统失效                                     │   │
│  │  • 故障检测和隔离自动完成                                       │   │
│  │                                                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                     异构冗余设计                                 │   │
│  │                                                                 │   │
│  │  需求 ──→ ┌─────────────┐                                       │   │
│  │           │  不同团队A  │ ──┐                                   │   │
│  │           │  不同处理器 │   │                                   │   │
│  │           │  不同编译器 │   │     ┌─────────┐                   │   │
│  │           └─────────────┘   └────→│ 比较器  │ → 输出            │   │
│  │                                  │         │                   │   │
│  │           ┌─────────────┐   ┌────→│(Comparator)               │   │
│  │           │  不同团队B  │ ──┘     └─────────┘                   │   │
│  │           │  不同处理器 │                                       │   │
│  │           │  不同编译器 │                                       │   │
│  │           └─────────────┘                                       │   │
│  │                                                                 │   │
│  │  目的: 防止共模故障（设计错误、编译器错误等）                    │   │
│  │                                                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 表决算法实现

```c
/**
 * @file voter.c
 * @brief 三模冗余 (TMR) 表决器实现
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define CHANNEL_COUNT 3
#define CHANNEL_A 0
#define CHANNEL_B 1
#define CHANNEL_C 2

/**
 * @brief 通道状态
 */
typedef struct {
    bool healthy;           /* 健康状态 */
    uint32_t error_count;   /* 错误计数 */
    uint32_t last_valid_output;
} ChannelStatus;

/**
 * @brief 表决器状态
 */
typedef struct {
    ChannelStatus channels[CHANNEL_COUNT];
    uint32_t active_channels;
    uint32_t disagreement_threshold;
} VoterState;

/**
 * @brief 初始化表决器
 */
void voter_init(VoterState *voter) {
    memset(voter, 0, sizeof(VoterState));
    for (int i = 0; i < CHANNEL_COUNT; i++) {
        voter->channels[i].healthy = true;
    }
    voter->active_channels = CHANNEL_COUNT;
    voter->disagreement_threshold = 10;
}

/**
 * @brief TMR 多数表决
 *
 * @param inputs 三个通道的输入
 * @param voter 表决器状态
 * @param output 表决输出
 * @return true 如果表决成功
 */
bool voter_majority_vote(const uint32_t inputs[CHANNEL_COUNT],
                         VoterState *voter,
                         uint32_t *output)
{
    uint32_t a = inputs[CHANNEL_A];
    uint32_t b = inputs[CHANNEL_B];
    uint32_t c = inputs[CHANNEL_C];

    /* 统计各通道健康状态 */
    bool valid[CHANNEL_COUNT];
    for (int i = 0; i < CHANNEL_COUNT; i++) {
        valid[i] = voter->channels[i].healthy;
    }

    /* 多数表决逻辑 */
    if (valid[CHANNEL_A] && valid[CHANNEL_B] && valid[CHANNEL_C]) {
        /* 三个通道都健康 */
        if (a == b || a == c) {
            *output = a;
            voter->channels[CHANNEL_A].last_valid_output = a;

            if (a != b) {
                voter->channels[CHANNEL_B].error_count++;
            }
            if (a != c) {
                voter->channels[CHANNEL_C].error_count++;
            }
            return true;
        } else if (b == c) {
            *output = b;
            voter->channels[CHANNEL_A].error_count++;
            voter->channels[CHANNEL_B].last_valid_output = b;
            voter->channels[CHANNEL_C].last_valid_output = c;
            return true;
        } else {
            /* 三个通道输出都不同 - 严重故障 */
            return false;
        }
    } else if (valid[CHANNEL_A] && valid[CHANNEL_B]) {
        /* A和B健康 */
        *output = (a == b) ? a : a; /* 选择A（主通道） */
        return true;
    } else if (valid[CHANNEL_A] && valid[CHANNEL_C]) {
        /* A和C健康 */
        *output = (a == c) ? a : a;
        return true;
    } else if (valid[CHANNEL_B] && valid[CHANNEL_C]) {
        /* B和C健康 */
        *output = (b == c) ? b : b;
        return true;
    } else if (valid[CHANNEL_A]) {
        /* 只有A健康 */
        *output = a;
        return true;
    } else if (valid[CHANNEL_B]) {
        /* 只有B健康 */
        *output = b;
        return true;
    } else if (valid[CHANNEL_C]) {
        /* 只有C健康 */
        *output = c;
        return true;
    }

    /* 所有通道都故障 */
    return false;
}

/**
 * @brief 通道故障检测与隔离
 */
void voter_monitor_channels(VoterState *voter) {
    for (int i = 0; i < CHANNEL_COUNT; i++) {
        if (voter->channels[i].error_count > voter->disagreement_threshold) {
            printf("[VOTER] Channel %d isolated due to excessive errors\n", i);
            voter->channels[i].healthy = false;
            voter->active_channels--;
        }
    }

    if (voter->active_channels < 2) {
        printf("[VOTER] WARNING: System in degraded mode (%d active channels)\n",
               voter->active_channels);
    }
}

/* 演示 */
int main(void) {
    VoterState voter;
    voter_init(&voter);

    printf("=== TMR Voter Demo ===\n\n");

    /* 测试用例 */
    uint32_t test_cases[][3] = {
        {100, 100, 100},    /* 全部一致 */
        {100, 100, 101},    /* C有轻微偏差 */
        {100, 200, 100},    /* B有严重偏差 */
        {100, 200, 300},    /* 全部不一致 */
    };

    for (int i = 0; i < 4; i++) {
        uint32_t output;
        printf("Inputs: A=%d, B=%d, C=%d\n",
               test_cases[i][0], test_cases[i][1], test_cases[i][2]);

        if (voter_majority_vote(test_cases[i], &voter, &output)) {
            printf("  Output: %d\n", output);
        } else {
            printf("  VOTE FAILED!\n");
        }
        printf("  Error counts: A=%d, B=%d, C=%d\n\n",
               voter.channels[0].error_count,
               voter.channels[1].error_count,
               voter.channels[2].error_count);
    }

    return 0;
}
```

---

## 开发实践

### 编码标准

```c
/**
 * @file example_compliant_code.c
 * @brief 符合 DO-178C / MISRA C 的示例代码
 */

/* ============================================================================
 * 1. 头文件保护
 * ============================================================================
 * Req: MISRA C:2012 Rule 20.5
 */
#ifndef EXAMPLE_COMPLIANT_CODE_H
#define EXAMPLE_COMPLIANT_CODE_H

/* ============================================================================
 * 2. 使用固定宽度类型
 * ============================================================================
 * Req: MISRA C:2012 Dir 4.6
 */
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * 3. 避免使用宏定义常量，使用枚举或const
 * ============================================================================
 * Req: MISRA C:2012 Dir 4.9
 */
enum {
    MAX_SENSOR_COUNT = 8,
    MAX_ACTUATOR_COUNT = 16,
    SAMPLING_PERIOD_MS = 10
};

static const uint32_t WATCHDOG_TIMEOUT_MS = 100U;

/* ============================================================================
 * 4. 函数声明明确可访问性
 * ============================================================================
 * Req: DO-178C 结构化设计
 */

/**
 * @brief 初始化传感器模块
 * @return true 如果初始化成功
 */
extern bool Sensor_Init(void);

/**
 * @brief 读取传感器值
 * @param sensor_id 传感器ID (0..MAX_SENSOR_COUNT-1)
 * @param value 输出值指针
 * @return true 如果读取成功
 */
extern bool Sensor_Read(uint8_t sensor_id, int32_t *value);

/* ============================================================================
 * 5. 避免使用隐式类型转换
 * ============================================================================
 * Req: MISRA C:2012 Rule 10.1, 10.3, 10.4
 */
static inline uint32_t safe_add_u32(uint32_t a, uint32_t b, bool *overflow) {
    uint32_t result = a + b;
    *overflow = (result < a); /* 检查溢出 */
    return result;
}

/* ============================================================================
 * 6. 所有路径必须有明确的返回值
 * ============================================================================
 * Req: MISRA C:2012 Rule 15.5
 */
static inline int32_t clamp_int32(int32_t value, int32_t min, int32_t max) {
    int32_t result;

    if (value < min) {
        result = min;
    } else if (value > max) {
        result = max;
    } else {
        result = value;
    }

    return result;
}

/* ============================================================================
 * 7. 避免使用危险函数
 * ============================================================================
 * Req: MISRA C:2012 Rule 21.6, 21.8
 * 禁止使用: malloc/free, sprintf, strcpy, strcat 等
 */

/* 使用安全的字符串复制 */
static inline void safe_strcpy(char *dest, size_t dest_size, const char *src) {
    size_t i;
    for (i = 0U; (i < (dest_size - 1U)) && (src[i] != '\0'); i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

/* ============================================================================
 * 8. 防御性编程 - 参数检查
 * ============================================================================
 * Req: DO-178C 鲁棒性要求
 */
#define CHECK_PARAM(cond, error_code) \
    do { \
        if (!(cond)) { \
            Error_Report(error_code, __FILE__, __LINE__); \
            return false; \
        } \
    } while(0)

/* ============================================================================
 * 9. 无死代码，无不可达代码
 * ============================================================================
 * Req: MISRA C:2012 Rule 2.1, 2.2
 */

/* ============================================================================
 * 10. 限制函数复杂度（圈复杂度）
 * ============================================================================
 * Req: DO-178C DAL A: 圈复杂度 <= 10
 */

#endif /* EXAMPLE_COMPLIANT_CODE_H */
```

---

## 案例分析

### 案例：波音787飞控系统架构

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    波音787 飞行控制系统架构                              │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │              主飞行控制计算机 (PFC - Primary Flight Computer)    │   │
│  │                                                                 │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐             │   │
│  │  │   PFC-A     │  │   PFC-B     │  │   PFC-C     │             │   │
│  │  │             │  │             │  │             │             │   │
│  │  │  PowerPC    │  │  PowerPC    │  │  PowerPC    │             │   │
│  │  │  DAL A      │  │  DAL A      │  │  DAL A      │             │   │
│  │  │             │  │  (异构)     │  │             │             │   │
│  │  │ 通道1-3     │  │  通道4-6    │  │  通道7-9    │             │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘             │   │
│  │                                                                 │   │
│  │  特点: 三重冗余 + 每PFC三通道 = 共9通道表决                     │   │
│  │        B计算机使用不同处理器（防共模故障）                      │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │              作动器控制电子 (ACE - Actuator Control Electronics) │   │
│  │                                                                 │   │
│  │  ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐      │   │
│  │  │ACE-L│ │ACE-R│ │ACE-E│ │ACE-A│ │ACE-B│ │ACE-C│ │ACE-D│      │   │
│  │  │     │ │     │ │     │ │     │ │     │ │     │ │     │      │   │
│  │  │升降 │ │升降 │ │方向 │ │副翼 │ │扰流 │ │方向 │ │襟翼 │      │   │
│  │  │舵左 │ │舵右 │ │舵  │ │ 左  │ │板  │ │舵备份│ │    │      │   │
│  │  └─────┘ └─────┘ └─────┘ └─────┘ └─────┘ └─────┘ └─────┘      │   │
│  │                                                                 │   │
│  │  ACE从PFC接收指令，控制电液伺服阀(ESV)驱动液压作动器            │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                     远程电子单元 (REU)                          │   │
│  │                                                                 │   │
│  │  分布式I/O模块，连接传感器和执行器到PFC                          │   │
│  │  • 传感器信号采集（ADC）                                        │   │
│  │  • 离散量输入/输出                                              │   │
│  │  • ARINC 429/664 总线接口                                       │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                     通信架构                                     │   │
│  │                                                                 │   │
│  │  • 高速: ARINC 664 (AFDX) - 确定性以太网                        │   │
│  │  • 中速: ARINC 429 - 传统航电总线                               │   │
│  │  • 低速: CAN - 传感器/作动器通信                                │   │
│  │  • 模拟: 直接模拟信号（关键传感器）                             │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                     安全特性                                     │   │
│  │                                                                 │   │
│  │  ✓ 异构冗余：不同硬件/软件设计团队                              │   │
│  │  ✓ 99.9%可用性（10^-9 失效概率/飞行小时）                       │   │
│  │  ✓ 故障安全：失效后进入安全状态                                 │   │
│  │  ✓ 故障-工作：单点故障不丧失功能                                │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 参考资源

### 标准文档

| 标准 | 描述 | 适用范围 |
|------|------|----------|
| DO-178C | 机载软件认证标准 | 民用航空软件开发 |
| DO-254 | 机载硬件认证标准 | 航空电子设备硬件 |
| ARINC 653 | 分区操作系统接口 | 综合模块化航电 |
| ARINC 429 | 数字数据总线标准 | 航电设备通信 |
| ARINC 664 | 航空全双工交换以太网 | 现代航电网络 |
| TTP/C | 时间触发协议 | 确定性通信 |
| MIL-STD-1553 | 军用数据总线 | 军用航空电子 |

### 开发工具

- **SCADE**: 基于模型的航电软件开发
- **LDRA**: DO-178C 合规性测试工具
- **VectorCAST**: 代码覆盖率分析
- **Polyspace**: 静态代码分析

---

*最后更新: 2026年3月*
