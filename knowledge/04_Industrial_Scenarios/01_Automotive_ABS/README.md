# 汽车 ABS 系统嵌入式开发

> **层级定位**: 04_Industrial_Scenarios > 01_Automotive_ABS
> **难度级别**: L5 专家
> **预估学习时间**: 40-60 小时

---

## 🔗 文档关联

### 前置知识

| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [MISRA C](../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md) | 安全标准 | 汽车级编码规范 |
| [嵌入式IoT](../14_Embedded_IoT/readme.md) | 硬件基础 | 嵌入式开发实践 |
| [实时系统](../../03_System_Technology_Domains/14_Concurrency_Parallelism/03_Real_Time_Systems.md) | 实时基础 | 实时调度 |
| [状态机](../../06_Thinking_Representation/09_State_Machines/readme.md) | 设计模式 | 控制逻辑实现 |

### 核心技术

| 技术 | 标准 | 应用场景 |
|:-----|:-----|:---------|
| ISO 26262 | 功能安全 | ASIL等级认证 |
| AUTOSAR | 软件架构 | ECU软件开发 |
| CAN总线 | 通信协议 | 车载网络 |
| OSEK OS | 实时系统 | 任务调度 |

### 安全关键

| 机制 | 文档 | 标准 |
|:-----|:-----|:-----|
| 内存保护 | MPU配置 | ISO 26262 |
| 看门狗 | 安全监控 | ASIL-D |
| 错误处理 | 异常框架 | 防御性编程 |

本目录涵盖汽车防抱死制动系统（ABS）的嵌入式软件开发，包括汽车电子架构、功能安全标准、实时操作系统以及符合 MISRA C 规范的嵌入式 C 语言编程。

---

## 目录

- [汽车 ABS 系统嵌入式开发](#汽车-abs-系统嵌入式开发)
  - [🔗 文档关联](#-文档关联)
    - [前置知识](#前置知识)
    - [核心技术](#核心技术)
    - [安全关键](#安全关键)
  - [目录](#目录)
  - [1. 汽车电子概述](#1-汽车电子概述)
    - [1.1 汽车电子架构 (E/E Architecture)](#11-汽车电子架构-ee-architecture)
    - [1.2 ECU (电子控制单元)](#12-ecu-电子控制单元)
    - [1.3 CAN 总线通信](#13-can-总线通信)
    - [1.4 AUTOSAR 架构](#14-autosar-架构)
  - [2. 功能安全标准 (ISO 26262)](#2-功能安全标准-iso-26262)
    - [2.1 ASIL 等级](#21-asil-等级)
    - [2.2 安全机制实现](#22-安全机制实现)
    - [2.3 看门狗与安全监控](#23-看门狗与安全监控)
  - [3. ABS 系统原理](#3-abs-系统原理)
    - [3.1 ABS 控制算法](#31-abs-控制算法)
    - [3.2 ABS 核心代码](#32-abs-核心代码)
  - [4. 实时操作系统 (OSEK/AUTOSAR OS)](#4-实时操作系统-osekautosar-os)
    - [4.1 OSEK OS 任务模型](#41-osek-os-任务模型)
    - [4.2 调度器实现](#42-调度器实现)
    - [4.3 扩展任务事件机制](#43-扩展任务事件机制)
  - [5. C 语言嵌入式编程规范 (MISRA C)](#5-c-语言嵌入式编程规范-misra-c)
    - [5.1 MISRA C:2012 关键规则](#51-misra-c2012-关键规则)
    - [5.2 防御性编程](#52-防御性编程)
  - [6. 内存保护和错误处理](#6-内存保护和错误处理)
    - [6.1 MPU (内存保护单元) 配置](#61-mpu-内存保护单元-配置)
    - [6.2 错误处理框架](#62-错误处理框架)
  - [参考资料](#参考资料)
  - [认证与合规](#认证与合规)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 1. 汽车电子概述

### 1.1 汽车电子架构 (E/E Architecture)

现代汽车电子系统采用分层分布式架构：

```
┌─────────────────────────────────────────────────────────────┐
│                      信息娱乐域 (Infotainment)               │
│         中控大屏、导航系统、音响、车联网 (V2X)               │
├─────────────────────────────────────────────────────────────┤
│                      车身电子域 (Body)                       │
│    车门控制、座椅调节、空调、灯光、雨刷、防盗系统            │
├─────────────────────────────────────────────────────────────┤
│                      底盘域 (Chassis)                        │
│    ABS、ESP、EPS、悬架控制、转向系统、制动系统               │
├─────────────────────────────────────────────────────────────┤
│                      动力域 (Powertrain)                     │
│    发动机控制 (ECM)、变速箱控制 (TCU)、混合动力              │
├─────────────────────────────────────────────────────────────┤
│                      高级辅助驾驶域 (ADAS)                   │
│    自适应巡航、车道保持、自动泊车、碰撞预警                  │
└─────────────────────────────────────────────────────────────┘
                              │
                    ┌─────────┴─────────┐
                    │    中央网关 (GW)   │
                    │  协议转换、路由    │
                    └─────────┬─────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
   ┌────┴────┐          ┌────┴────┐          ┌────┴────┐
   │  CAN 总线 │          │  CAN-FD │          │Ethernet │
   │  500Kbps │          │  2Mbps  │          │ 1Gbps   │
   └─────────┘          └─────────┘          └─────────┘
```

### 1.2 ECU (电子控制单元)

```c
/* ecu/ecu_types.h */
#ifndef ECU_TYPES_H
#define ECU_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* ECU 运行模式 */
typedef enum {
    ECU_MODE_INIT,          /* 初始化模式 */
    ECU_MODE_NORMAL,        /* 正常运行 */
    ECU_MODE_DEGRADED,      /* 降级模式 */
    ECU_MODE_DIAGNOSTIC,    /* 诊断模式 */
    ECU_MODE_SLEEP,         /* 休眠模式 */
    ECU_MODE_SHUTDOWN       /* 关机 */
} ecu_mode_t;

/* ECU 状态 */
typedef struct {
    ecu_mode_t current_mode;
    uint32_t error_flags;
    uint32_t voltage_mV;           /* 供电电压 */
    int32_t temperature_mC;        /* 芯片温度 */
    uint32_t watchdog_counter;
    uint64_t runtime_ms;           /* 累计运行时间 */
} ecu_status_t;

/* ECU 配置 */
typedef struct {
    uint16_t ecu_id;               /* ECU 标识 */
    uint8_t can_channel_count;     /* CAN 通道数 */
    uint8_t network_node_count;    /* 网络节点数 */
    uint32_t watchdog_timeout_ms;  /* 看门狗超时 */
} ecu_config_t;

/* ECU 主结构 */
typedef struct {
    ecu_config_t config;
    ecu_status_t status;
    void* can_controller;          /* CAN 控制器句柄 */
    void* diagnostic_manager;      /* 诊断管理器 */
    void* power_manager;           /* 电源管理器 */
} ecu_t;

ecu_t* ecu_init(const ecu_config_t* config);
void ecu_run(ecu_t* ecu);
void ecu_shutdown(ecu_t* ecu);

#endif
```

### 1.3 CAN 总线通信

```c
/* network/can_driver.h */
#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include <stdint.h>

/* CAN ID 定义 - 标准帧 11 位 */
#define CAN_ID_ABS_WHEEL_SPEED_FL   0x100  /* 左前轮速度 */
#define CAN_ID_ABS_WHEEL_SPEED_FR   0x101  /* 右前轮速度 */
#define CAN_ID_ABS_WHEEL_SPEED_RL   0x102  /* 左后轮速度 */
#define CAN_ID_ABS_WHEEL_SPEED_RR   0x103  /* 右后轮速度 */
#define CAN_ID_ABS_STATUS           0x200  /* ABS 状态 */
#define CAN_ID_BRAKE_PEDAL_POS      0x300  /* 制动踏板位置 */
#define CAN_ID_VEHICLE_SPEED        0x400  /* 车速 */

/* CAN 帧结构 */
typedef struct {
    uint32_t id;           /* 11 位或 29 位 ID */
    bool extended;         /* 是否为扩展帧 */
    bool rtr;              /* 远程传输请求 */
    uint8_t dlc;           /* 数据长度代码 (0-8) */
    uint8_t data[8];       /* 数据字节 */
    uint32_t timestamp_us; /* 接收时间戳 */
} can_frame_t;

/* CAN 回调函数类型 */
typedef void (*can_rx_callback_t)(const can_frame_t* frame, void* user_data);

typedef struct can_controller can_controller_t;

can_controller_t* can_init(uint8_t channel, uint32_t baudrate);
void can_register_rx_callback(can_controller_t* can,
                              uint32_t id,
                              uint32_t mask,
                              can_rx_callback_t callback,
                              void* user_data);
bool can_send(can_controller_t* can, const can_frame_t* frame, uint32_t timeout_ms);
void can_process_rx(can_controller_t* can);

/* 打包/解包辅助函数 */
static inline void pack_u16(uint8_t* data, uint16_t value) {
    data[0] = (uint8_t)(value & 0xFF);
    data[1] = (uint8_t)((value >> 8) & 0xFF);
}

static inline uint16_t unpack_u16(const uint8_t* data) {
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

#endif
```

### 1.4 AUTOSAR 架构

```
┌─────────────────────────────────────────────────────────────┐
│                     应用层 (ASW - Application Layer)         │
│                                                              │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │  SWC: ABS   │  │  SWC: Wheel │  │  SWC: Diagnostic    │  │
│  │  Controller │  │  Speed Sensor│  │  Manager            │  │
│  │             │  │             │  │                     │  │
│  │  ┌───────┐  │  │  ┌───────┐  │  │  ┌───────────────┐  │  │
│  │  │Runnable│  │  │  │Runnable│  │  │  │   Runnable    │  │  │
│  │  └───┬───┘  │  │  └───┬───┘  │  │  └───────┬───────┘  │  │
│  └────┼──────┘  │  └────┼──────┘  │  └─────────┼──────────┘  │
│       │         │       │         │            │             │
│       └─────────┴───────┴─────────┴────────────┘             │
│                        │                                     │
│                   运行时环境 (RTE)                            │
├────────────────────────┼─────────────────────────────────────┤
│                     运行时环境 (RTE)                          │
├────────────────────────┼─────────────────────────────────────┤
│                   基础软件 (BSW - Basic Software)            │
│                                                              │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │              服务层 (Services Layer)                     │ │
│  │  通信服务 (COM) | 存储服务 (NVM) | 诊断服务 (DEM/DCM)    │ │
│  └─────────────────────────────────────────────────────────┘ │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │              ECU 抽象层 (ECU Abstraction Layer)          │ │
│  │  IO 驱动 | 通信驱动 | 存储驱动 | 微控制器驱动            │ │
│  └─────────────────────────────────────────────────────────┘ │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │              微控制器驱动 (Microcontroller Driver)       │ │
│  │  MCU 驱动 | CAN 驱动 | ADC 驱动 | PWM 驱动 | GPT 驱动    │ │
│  └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. 功能安全标准 (ISO 26262)

### 2.1 ASIL 等级

```
┌────────────────────────────────────────────────────────────────┐
│                     ASIL 汽车安全完整性等级                     │
├─────────────┬──────────────────────────────────────────────────┤
│   ASIL D    │ 最高安全等级 - 转向、制动系统 (如 EPS、ABS)      │
│             │ 要求: 单点故障度量 > 99%, 潜在故障度量 > 90%     │
├─────────────┼──────────────────────────────────────────────────┤
│   ASIL C    │ 高安全等级 - 悬挂系统、大灯控制                    │
├─────────────┼──────────────────────────────────────────────────┤
│   ASIL B    │ 中等安全等级 - 后视镜除雾、座椅加热                │
├─────────────┼──────────────────────────────────────────────────┤
│   ASIL A    │ 最低安全等级 - 内饰灯、仪表板背光                  │
├─────────────┼──────────────────────────────────────────────────┤
│   QM        │ 质量管理 - 无特殊安全要求                          │
└─────────────┴──────────────────────────────────────────────────┘
```

### 2.2 安全机制实现

```c
/* safety/safety_mechanisms.h */
#ifndef SAFETY_MECHANISMS_H
#define SAFETY_MECHANISMS_H

#include <stdint.h>
#include <stdbool.h>

/* 安全监控结果 */
typedef enum {
    SAFETY_STATUS_OK,
    SAFETY_STATUS_ERROR_DETECTED,
    SAFETY_STATUS_FAULT
} safety_status_t;

/* 双核锁步 (Dual-Core Lockstep) 监控 */
typedef struct {
    uint32_t core0_result;
    uint32_t core1_result;
    uint32_t compare_count;
    uint32_t mismatch_count;
} lockstep_monitor_t;

safety_status_t lockstep_verify(lockstep_monitor_t* monitor);

/* 端到端保护 (E2E - End-to-End Protection) */
#define E2E_CRC_POLYNOMIAL 0x1021  /* CRC-16-CCITT */

typedef struct {
    uint8_t counter;      /* 序列计数器 */
    uint8_t crc;          /* CRC 校验 */
    uint16_t data_id;     /* 数据标识 */
} e2e_header_t;

uint8_t e2e_calculate_crc(const uint8_t* data, uint32_t length, uint16_t data_id);
bool e2e_verify(const uint8_t* data, uint32_t length, const e2e_header_t* header);
void e2e_protect(uint8_t* data, uint32_t length, uint16_t data_id, e2e_header_t* header);

/* 安全监控器 (Safety Monitor) */
typedef struct {
    uint32_t expected_signature;
    uint32_t actual_signature;
    uint32_t timeout_ms;
    uint32_t last_update_ms;
    bool is_alive;
} safety_monitor_t;

void safety_monitor_init(safety_monitor_t* monitor, uint32_t timeout_ms);
safety_status_t safety_monitor_check(safety_monitor_t* monitor, uint32_t current_time_ms);
void safety_monitor_kick(safety_monitor_t* monitor, uint32_t current_time_ms, uint32_t signature);

#endif
```

### 2.3 看门狗与安全监控

```c
/* safety/watchdog.c */
#include "safety/watchdog.h"
#include "mcu/mcu_registers.h"

#define WWDG_BASE           0x40002C00
#define WWDG_CR            (*(volatile uint32_t*)(WWDG_BASE + 0x00))
#define WWDG_CFR           (*(volatile uint32_t*)(WWDG_BASE + 0x04))
#define WWDG_SR            (*(volatile uint32_t*)(WWDG_BASE + 0x08))

/* 窗口看门狗配置 */
#define WWDG_COUNTER_MAX    0x7F
#define WWDG_COUNTER_MIN    0x40
#define WWDG_WINDOW_VALUE   0x5F  /* 窗口上限 */

typedef struct {
    uint8_t expected_task_signatures[8];  /* 各任务预期签名 */
    uint8_t task_count;
    uint32_t last_feed_time;
    bool enabled;
} wwdg_manager_t;

static wwdg_manager_t g_wwdg_mgr;

void wwdg_init(void) {
    /* 启用 WWDG 时钟 */
    RCC_APB1ENR |= RCC_APB1ENR_WWDGEN;

    /* 配置预分频器和窗口值 */
    WWDG_CFR = (3 << 7) |  /* 预分频器: /8 */
               WWDG_WINDOW_VALUE;

    /* 启动看门狗 */
    WWDG_CR = WWDG_COUNTER_MAX | 0x80;  /* 使能位 */

    g_wwdg_mgr.enabled = true;
    g_wwdg_mgr.task_count = 0;
}

/* 任务注册其检查点 */
void wwdg_register_task(uint8_t task_id, uint8_t expected_signature) {
    if (task_id < 8) {
        g_wwdg_mgr.expected_task_signatures[task_id] = expected_signature;
        g_wwdg_mgr.task_count++;
    }
}

/* 任务在关键点报告状态 */
bool wwdg_report_checkpoint(uint8_t task_id, uint8_t signature) {
    if (task_id >= 8) return false;

    /* 验证任务签名 */
    if (signature != g_wwdg_mgr.expected_task_signatures[task_id]) {
        /* 签名不匹配 - 任务执行异常 */
        safety_log_error(SAFETY_ERR_TASK_SIGNATURE_MISMATCH, task_id);
        return false;
    }

    /* 标记任务已完成 */
    g_wwdg_mgr.task_checkpoints |= (1 << task_id);

    /* 检查是否所有任务都已完成 */
    if (g_wwdg_mgr.task_checkpoints == ((1 << g_wwdg_mgr.task_count) - 1)) {
        wwdg_feed();
        g_wwdg_mgr.task_checkpoints = 0;  /* 重置检查点 */
    }

    return true;
}

void wwdg_feed(void) {
    /* 必须在窗口期内喂狗 */
    uint8_t current_counter = WWDG_CR & 0x7F;

    if (current_counter > WWDG_WINDOW_VALUE) {
        /* 在窗口期外喂狗 - 过早喂狗 */
        safety_log_error(SAFETY_ERR_WWDG_EARLY_FEED, current_counter);
        return;
    }

    if (current_counter < WWDG_COUNTER_MIN) {
        /* 计数器已低于最小值 - 系统即将复位 */
        safety_log_error(SAFETY_ERR_WWDG_LATE_FEED, current_counter);
        return;
    }

    /* 重新加载计数器 */
    WWDG_CR = WWDG_COUNTER_MAX | 0x80;
    g_wwdg_mgr.last_feed_time = get_system_tick();
}
```

---

## 3. ABS 系统原理

### 3.1 ABS 控制算法

```
┌─────────────────────────────────────────────────────────────────┐
│                      ABS 控制系统架构                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────────────┐ │
│  │ 轮速传感器   │───▶│ 滑移率计算  │───▶│ 控制算法 (PID/模糊) │ │
│  │ (4 个通道)  │    │             │    │                     │ │
│  └─────────────┘    └─────────────┘    └──────────┬──────────┘ │
│                                                   │            │
│  ┌─────────────┐    ┌─────────────┐              ▼            │
│  │ 液压调节器   │◀───│ 压力控制    │◀──────── 参考滑移率       │
│  │ (电磁阀)    │    │ (增压/保压/ │              │            │
│  └─────────────┘    │  减压)      │              │            │
│                     └─────────────┘    ┌─────────┴─────────┐  │
│                                        │ 路面识别          │  │
│                                        │ (高 μ / 低 μ /   │  │
│                                        │  分离路面)        │  │
│                                        └───────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

### 3.2 ABS 核心代码

```c
/* abs/abs_controller.h */
#ifndef ABS_CONTROLLER_H
#define ABS_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

/* ABS 控制状态 */
typedef enum {
    ABS_STATE_IDLE,          /* 无制动 */
    ABS_STATE_MONITORING,    /* 监控状态 */
    ABS_STATE_PRESSURE_HOLD, /* 保压 */
    ABS_STATE_PRESSURE_DEC,  /* 减压 */
    ABS_STATE_PRESSURE_INC   /* 增压 */
} abs_state_t;

/* 车轮控制通道 */
typedef struct {
    uint8_t wheel_id;

    /* 传感器输入 */
    uint32_t wheel_speed_rpm;      /* 轮速 (RPM) */
    uint32_t wheel_acceleration;    /* 轮加速度 */
    uint16_t brake_pressure_kpa;    /* 制动压力 */

    /* 控制变量 */
    abs_state_t state;
    uint32_t slip_ratio_permille;   /* 滑移率 (‰) 目标: 100-300‰ */
    int32_t velocity_reference;     /* 参考车速 */

    /* 执行器控制 */
    uint8_t inlet_valve_pwm;        /* 进油阀 PWM */
    uint8_t outlet_valve_pwm;       /* 出油阀 PWM */
    uint8_t pump_motor_pwm;         /* 泵电机 PWM */

    /* 诊断 */
    uint16_t error_flags;
    uint32_t cycle_count;
} abs_channel_t;

/* ABS 控制器主结构 */
typedef struct {
    abs_channel_t channels[4];       /* FL, FR, RL, RR */
    uint32_t vehicle_speed_kmh;      /* 估算车速 */
    uint8_t system_state;
    bool enabled;

    /* 控制参数 */
    uint32_t slip_threshold_high;    /* 高压滑移率阈值 (‰) */
    uint32_t slip_threshold_low;     /* 低压滑移率阈值 (‰) */
    uint32_t accel_threshold;        /* 减速度阈值 */
} abs_controller_t;

abs_controller_t* abs_controller_init(void);
void abs_controller_update(abs_controller_t* abs, uint32_t dt_ms);
void abs_controller_enable(abs_controller_t* abs, bool enable);

#endif
```

```c
/* abs/abs_controller.c */
#include "abs/abs_controller.h"
#include "safety/safety_mechanisms.h"
#include "platform/critical_section.h"

/* 控制参数 - ASIL D 要求硬编码安全参数 */
#define SLIP_RATIO_TARGET       150   /* 目标滑移率 15% */
#define SLIP_RATIO_THRESHOLD    200   /* 触发 ABS 阈值 20% */
#define DECELERATION_THRESHOLD  800   /* 减速度阈值 m/s² */
#define WHEEL_SPEED_MIN         50    /* 最小工作轮速 RPM */

/* 电磁阀控制时序 (ms) */
#define VALVE_CYCLE_TIME        10
#define PRESSURE_HOLD_TIME      20
#define PRESSURE_DEC_TIME       15
#define PRESSURE_INC_TIME       25

static void calculate_slip_ratio(abs_channel_t* ch, uint32_t vehicle_speed);
static void update_control_state(abs_channel_t* ch, uint32_t dt_ms);
static void execute_valve_control(abs_channel_t* ch);

void abs_controller_update(abs_controller_t* abs, uint32_t dt_ms) {
    CRITICAL_SECTION_ENTER();

    /* 步骤 1: 估算参考车速 */
    uint32_t max_wheel_speed = 0;
    for (int i = 0; i < 4; i++) {
        if (abs->channels[i].wheel_speed_rpm > max_wheel_speed) {
            max_wheel_speed = abs->channels[i].wheel_speed_rpm;
        }
    }

    /* 使用最大轮速作为参考车速估算 */
    abs->vehicle_speed_kmh = wheel_rpm_to_kmh(max_wheel_speed);

    /* 步骤 2: 处理每个车轮通道 */
    for (int i = 0; i < 4; i++) {
        abs_channel_t* ch = &abs->channels[i];

        /* 安全检查: 传感器有效性 */
        if (ch->wheel_speed_rpm == 0xFFFF) {
            ch->error_flags |= ABS_ERR_SENSOR_FAULT;
            ch->state = ABS_STATE_IDLE;
            continue;
        }

        /* 计算滑移率 */
        calculate_slip_ratio(ch, abs->vehicle_speed_kmh);

        /* 状态机更新 */
        if (abs->enabled &&
            ch->wheel_speed_rpm > WHEEL_SPEED_MIN &&
            abs->vehicle_speed_kmh > 10) {
            update_control_state(ch, dt_ms);
        } else {
            ch->state = ABS_STATE_IDLE;
        }

        /* 执行控制输出 */
        execute_valve_control(ch);

        ch->cycle_count++;
    }

    CRITICAL_SECTION_EXIT();
}

static void calculate_slip_ratio(abs_channel_t* ch, uint32_t vehicle_speed) {
    if (vehicle_speed == 0) {
        ch->slip_ratio_permille = 0;
        return;
    }

    uint32_t wheel_speed = wheel_rpm_to_kmh(ch->wheel_speed_rpm);

    /* 滑移率 λ = (V_vehicle - V_wheel) / V_vehicle * 1000‰ */
    if (wheel_speed < vehicle_speed) {
        ch->slip_ratio_permille =
            ((vehicle_speed - wheel_speed) * 1000) / vehicle_speed;
    } else {
        ch->slip_ratio_permille = 0;
    }
}

static void update_control_state(abs_channel_t* ch, uint32_t dt_ms) {
    /* ABS 状态机 */
    switch (ch->state) {
        case ABS_STATE_IDLE:
        case ABS_STATE_MONITORING:
            /* 检查是否需要介入 */
            if (ch->slip_ratio_permille > SLIP_RATIO_THRESHOLD) {
                ch->state = ABS_STATE_PRESSURE_DEC;
                ch->cycle_count = 0;
            }
            break;

        case ABS_STATE_PRESSURE_DEC:
            /* 减压直到滑移率降到目标以下 */
            if (ch->slip_ratio_permille < SLIP_RATIO_TARGET) {
                ch->state = ABS_STATE_PRESSURE_HOLD;
                ch->cycle_count = 0;
            }
            break;

        case ABS_STATE_PRESSURE_HOLD:
            /* 保压观察 */
            if (ch->cycle_count * dt_ms > PRESSURE_HOLD_TIME) {
                if (ch->slip_ratio_permille > SLIP_RATIO_THRESHOLD) {
                    ch->state = ABS_STATE_PRESSURE_DEC;
                } else if (ch->slip_ratio_permille < SLIP_RATIO_TARGET / 2) {
                    ch->state = ABS_STATE_PRESSURE_INC;
                }
                ch->cycle_count = 0;
            }
            break;

        case ABS_STATE_PRESSURE_INC:
            /* 增压 */
            if (ch->cycle_count * dt_ms > PRESSURE_INC_TIME) {
                ch->state = ABS_STATE_PRESSURE_HOLD;
                ch->cycle_count = 0;
            }
            if (ch->slip_ratio_permille > SLIP_RATIO_THRESHOLD) {
                ch->state = ABS_STATE_PRESSURE_DEC;
                ch->cycle_count = 0;
            }
            break;
    }
}

static void execute_valve_control(abs_channel_t* ch) {
    /* 电磁阀控制真值表 */
    switch (ch->state) {
        case ABS_STATE_IDLE:
        case ABS_STATE_MONITORING:
            ch->inlet_valve_pwm = 0;    /* 进油阀开 */
            ch->outlet_valve_pwm = 0;   /* 出油阀关 */
            ch->pump_motor_pwm = 0;     /* 泵关 */
            break;

        case ABS_STATE_PRESSURE_HOLD:
            ch->inlet_valve_pwm = 255;  /* 进油阀关 */
            ch->outlet_valve_pwm = 0;   /* 出油阀关 */
            ch->pump_motor_pwm = 0;
            break;

        case ABS_STATE_PRESSURE_DEC:
            ch->inlet_valve_pwm = 255;  /* 进油阀关 */
            ch->outlet_valve_pwm = 255; /* 出油阀开 */
            ch->pump_motor_pwm = 255;   /* 泵开 */
            break;

        case ABS_STATE_PRESSURE_INC:
            ch->inlet_valve_pwm = 0;    /* 进油阀开 */
            ch->outlet_valve_pwm = 0;   /* 出油阀关 */
            ch->pump_motor_pwm = 0;
            break;
    }

    /* 输出到硬件 */
    pwm_set_duty(PWM_INLET_VALVE(ch->wheel_id), ch->inlet_valve_pwm);
    pwm_set_duty(PWM_OUTLET_VALVE(ch->wheel_id), ch->outlet_valve_pwm);
    pwm_set_duty(PWM_PUMP_MOTOR(ch->wheel_id), ch->pump_motor_pwm);
}
```

---

## 4. 实时操作系统 (OSEK/AUTOSAR OS)

### 4.1 OSEK OS 任务模型

```c
/* os/osek_types.h */
#ifndef OSEK_TYPES_H
#define OSEK_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* 任务类型 */
typedef enum {
    BASIC,      /* 基础任务 - 不可等待 */
    EXTENDED    /* 扩展任务 - 可等待事件 */
} task_type_t;

/* 任务调度策略 */
typedef enum {
    FULL_PREEMPTIVE,     /* 完全抢占 */
    NON_PREEMPTIVE,      /* 非抢占 */
    MIXED_PREEMPTIVE     /* 混合 */
} schedule_policy_t;

/* 任务状态 */
typedef enum {
    RUNNING,     /* 正在执行 */
    READY,       /* 就绪等待 */
    WAITING,     /* 等待事件 (仅扩展任务) */
    SUSPENDED    /* 挂起 */
} task_state_t;

/* 优先级定义 - 数值越小优先级越高 */
#define TASK_PRIORITY_IDLE      31
#define TASK_PRIORITY_LOW       20
#define TASK_PRIORITY_NORMAL    10
#define TASK_PRIORITY_HIGH       5
#define TASK_PRIORITY_ISR        0

/* 任务控制块 */
typedef struct {
    uint8_t task_id;
    task_type_t type;
    task_state_t state;
    uint8_t priority;
    uint8_t current_priority;  /* 考虑优先级天花板 */
    void (*entry_point)(void);
    uint32_t* stack_base;
    uint32_t stack_size;
    uint32_t* stack_ptr;
    uint32_t wait_events;      /* 等待的事件掩码 */
    uint32_t current_events;   /* 当前事件状态 */
} task_control_block_t;

/* 报警类型 */
typedef struct {
    uint8_t alarm_id;
    uint32_t period;           /* 周期 (ticks) */
    uint32_t countdown;
    task_control_block_t* task;
    void (*callback)(void);
} alarm_t;

#endif
```

### 4.2 调度器实现

```c
/* os/scheduler.c */
#include "os/osek_types.h"
#include "platform/context_switch.h"

#define MAX_TASKS       16
#define MAX_ALARMS      8

static task_control_block_t* g_ready_queue[MAX_TASKS];
static uint8_t g_ready_count = 0;
static task_control_block_t* g_current_task = NULL;
static task_control_block_t g_task_table[MAX_TASKS];
static alarm_t g_alarms[MAX_ALARMS];
static volatile uint32_t g_system_tick = 0;

/* 就绪队列管理 */
static void ready_queue_insert(task_control_block_t* task) {
    /* 按优先级插入 (优先级数值小的在前) */
    uint8_t i = 0;
    while (i < g_ready_count && g_ready_queue[i]->priority <= task->priority) {
        i++;
    }

    /* 移动元素腾出位置 */
    for (uint8_t j = g_ready_count; j > i; j--) {
        g_ready_queue[j] = g_ready_queue[j - 1];
    }

    g_ready_queue[i] = task;
    g_ready_count++;
    task->state = READY;
}

static task_control_block_t* ready_queue_highest(void) {
    if (g_ready_count == 0) return NULL;
    return g_ready_queue[0];
}

static void ready_queue_remove(task_control_block_t* task) {
    for (uint8_t i = 0; i < g_ready_count; i++) {
        if (g_ready_queue[i] == task) {
            for (uint8_t j = i; j < g_ready_count - 1; j++) {
                g_ready_queue[j] = g_ready_queue[j + 1];
            }
            g_ready_count--;
            break;
        }
    }
}

/* 任务激活 */
void activate_task(uint8_t task_id) {
    CRITICAL_SECTION_ENTER();

    task_control_block_t* task = &g_task_table[task_id];

    if (task->state == SUSPENDED) {
        /* 初始化任务栈 */
        task->stack_ptr = task->stack_base + task->stack_size;
        task->current_priority = task->priority;

        ready_queue_insert(task);

        /* 抢占检查 */
        if (g_current_task == NULL ||
            task->priority < g_current_task->priority) {
            schedule();
        }
    }

    CRITICAL_SECTION_EXIT();
}

/* 任务终止 */
void terminate_task(void) {
    CRITICAL_SECTION_ENTER();

    if (g_current_task) {
        g_current_task->state = SUSPENDED;
        g_current_task = NULL;
    }

    schedule();

    CRITICAL_SECTION_EXIT();
}

/* 调度器 */
void schedule(void) {
    task_control_block_t* next = ready_queue_highest();

    if (next == NULL) {
        /* 无就绪任务 - 进入空闲 */
        return;
    }

    if (next == g_current_task) {
        return;  /* 继续执行当前任务 */
    }

    task_control_block_t* prev = g_current_task;

    if (prev && prev->state == RUNNING) {
        prev->state = READY;
        ready_queue_insert(prev);
    }

    ready_queue_remove(next);
    next->state = RUNNING;
    g_current_task = next;

    /* 上下文切换 */
    context_switch(&prev->stack_ptr, next->stack_ptr);
}

/* 系统节拍处理 (ISR) */
void sys_tick_handler(void) {
    g_system_tick++;

    /* 更新报警 */
    for (uint8_t i = 0; i < MAX_ALARMS; i++) {
        alarm_t* alarm = &g_alarms[i];
        if (alarm->period == 0) continue;

        alarm->countdown--;
        if (alarm->countdown == 0) {
            alarm->countdown = alarm->period;

            if (alarm->task) {
                activate_task(alarm->task->task_id);
            }
            if (alarm->callback) {
                alarm->callback();
            }
        }
    }

    /* 抢占调度 */
    if (g_current_task && g_ready_count > 0) {
        task_control_block_t* highest = g_ready_queue[0];
        if (highest->priority < g_current_task->priority) {
            schedule();
        }
    }
}
```

### 4.3 扩展任务事件机制

```c
/* os/events.c */
#include "os/osek_types.h"

/* 等待事件 - 仅扩展任务 */
void wait_event(uint32_t mask) {
    CRITICAL_SECTION_ENTER();

    if (g_current_task == NULL || g_current_task->type != EXTENDED) {
        CRITICAL_SECTION_EXIT();
        return;
    }

    g_current_task->wait_events = mask;

    /* 检查事件是否已满足 */
    if ((g_current_task->current_events & mask) == 0) {
        g_current_task->state = WAITING;

        task_control_block_t* waiting_task = g_current_task;
        g_current_task = NULL;

        schedule();

        /* 恢复后清除等待掩码 */
        waiting_task->wait_events = 0;
    }

    CRITICAL_SECTION_EXIT();
}

/* 设置事件 */
void set_event(uint8_t task_id, uint32_t mask) {
    CRITICAL_SECTION_ENTER();

    task_control_block_t* task = &g_task_table[task_id];

    task->current_events |= mask;

    /* 如果任务在等待这些事件，将其唤醒 */
    if (task->state == WAITING && (task->wait_events & mask)) {
        task->state = READY;
        ready_queue_insert(task);

        /* 抢占检查 */
        if (g_current_task && task->priority < g_current_task->priority) {
            schedule();
        }
    }

    CRITICAL_SECTION_EXIT();
}

/* 清除事件 */
void clear_event(uint32_t mask) {
    CRITICAL_SECTION_ENTER();

    if (g_current_task) {
        g_current_task->current_events &= ~mask;
    }

    CRITICAL_SECTION_EXIT();
}
```

---

## 5. C 语言嵌入式编程规范 (MISRA C)

### 5.1 MISRA C:2012 关键规则

```c
/* misra_examples.c - 展示 MISRA C 合规代码 */

/* ============================================================================
 * Rule 8.9: 对象定义应在块作用域内，如果仅在该块内访问
 * Rule 8.4: 外部链接对象的兼容声明应在同一文件中
 * ============================================================================ */

/* 全局变量 - 仅在本文件使用，声明为 static */
static uint32_t g_abs_cycle_counter;  /* 符合 Rule 8.8 */

/* 外部接口变量 - 在头文件中声明 */
extern volatile uint32_t g_system_tick;  /* 在头文件中声明 */

/* ============================================================================
 * Rule 17.8: 函数参数不应被修改
 * ============================================================================ */

/* 不好的做法: 修改输入参数 */
void bad_example(uint32_t speed) {
    speed = speed * 1000 / 3600;  /* 违反 Rule 17.8 */
}

/* 好的做法: 使用局部变量 */
uint32_t good_example(uint32_t speed_kmh) {
    uint32_t speed_ms;  /* 局部变量 */

    /* Rule 10.8: 复合表达式的值不应转换为不同的底层类型 */
    speed_ms = (uint32_t)((uint64_t)speed_kmh * 1000U / 3600U);

    return speed_ms;
}

/* ============================================================================
 * Rule 14.4: for 循环的循环计数器不应在循环体中修改
 * Rule 14.5: for 循环应只包含一个循环计数器
 * ============================================================================ */

/* 不好的做法 */
void bad_loop(void) {
    int i, j;
    for (i = 0, j = 10; i < 10; i++, j--) {  /* 违反 Rule 14.5 */
        if (condition) {
            i = 5;  /* 违反 Rule 14.4 */
        }
    }
}

/* 好的做法 */
void good_loop(void) {
    uint32_t i;

    for (i = 0U; i < 10U; i++) {  /* 符合规则 */
        /* 循环体不修改 i */
    }

    /* 需要额外控制时使用 while */
    i = 0U;
    while (i < 10U) {
        if (condition) {
            i = 5U;  /* 允许在 while 中修改 */
        }
        i++;
    }
}

/* ============================================================================
 * Rule 11.3: 指向 void 的指针不应转换为指向对象的指针
 * Rule 11.5: 指向 void 的指针不应转换为指向对象的指针
 * ============================================================================ */

/* 类型安全的内存访问 */
typedef union {
    uint32_t u32;
    uint16_t u16[2];
    uint8_t u8[4];
} register_access_t;

/* 不好的做法 */
void bad_pointer_cast(void* ptr) {
    uint32_t* reg = (uint32_t*)ptr;  /* 违反 Rule 11.5 */
    *reg = 0x12345678U;
}

/* 好的做法 - 使用联合体 */
void good_register_write(volatile register_access_t* reg, uint32_t value) {
    if (reg != NULL) {  /* Rule 17.2: 空指针检查 */
        reg->u32 = value;
    }
}

/* ============================================================================
 * Rule 15.5: 每个 switch 语句应有且只有一个 default 标签
 * Rule 16.1: switch 标签不应嵌套
 * Rule 16.3: 每个 switch 子句都应有无条件 break
 * ============================================================================ */

typedef enum {
    STATE_INIT,
    STATE_RUN,
    STATE_STOP,
    STATE_ERROR
} system_state_t;

void state_machine_handler(system_state_t state) {
    switch (state) {  /* Rule 15.5: 有 default */
        case STATE_INIT:
            init_system();
            break;  /* Rule 16.3: 无条件 break */

        case STATE_RUN:
            run_system();
            break;

        case STATE_STOP:
            stop_system();
            break;

        case STATE_ERROR:
            handle_error();
            break;

        default:  /* 必需 */
            /* 记录非法状态 */
            log_error(ERROR_INVALID_STATE);
            enter_safe_state();
            break;
    }
}

/* ============================================================================
 * Rule 17.7: 非 void 函数的返回值应被使用
 * Rule 17.4: 所有退出路径都应有 return 语句
 * ============================================================================ */

/* 安全关键函数 - 必须检查返回值 */
typedef enum {
    ERR_OK = 0,
    ERR_NULL_PTR,
    ERR_OUT_OF_RANGE,
    ERR_TIMEOUT
} error_code_t;

/* 不好的做法 */
void bad_function_call(void) {
    abs_controller_update(NULL, 10U);  /* 返回值被忽略 */
}

/* 好的做法 */
void good_function_call(abs_controller_t* abs) {
    error_code_t result;

    result = abs_controller_update(abs, 10U);
    if (result != ERR_OK) {
        error_handler(result);
    }
}

/* ============================================================================
 * Rule 21.3: 不应使用堆内存分配
 * Rule 21.6: 不应使用标准输入输出库
 * ============================================================================ */

/* 静态分配代替动态分配 */
#define MAX_ABS_CONTROLLERS 1U

static abs_controller_t g_abs_controllers[MAX_ABS_CONTROLLERS];
static bool g_abs_allocated[MAX_ABS_CONTROLLERS];

abs_controller_t* abs_controller_create(void) {
    uint32_t i;

    for (i = 0U; i < MAX_ABS_CONTROLLERS; i++) {
        if (!g_abs_allocated[i]) {
            g_abs_allocated[i] = true;
            /* 初始化 */
            memset(&g_abs_controllers[i], 0, sizeof(abs_controller_t));
            return &g_abs_controllers[i];
        }
    }

    return NULL;  /* 资源耗尽 */
}

/* 使用安全日志代替 printf */
void safety_log_error(error_code_t error, uint32_t param) {
    log_entry_t entry;

    entry.timestamp = get_system_tick();
    entry.error_code = error;
    entry.parameter = param;

    log_write(&entry);  /* 写入非易失性存储或 CAN */
}
```

### 5.2 防御性编程

```c
/* safety/defensive_programming.h */
#ifndef DEFENSIVE_PROGRAMMING_H
#define DEFENSIVE_PROGRAMMING_H

/* 编译时断言 */
#define STATIC_ASSERT(condition, msg) \
    typedef char static_assert_##msg[(condition) ? 1 : -1]

/* 运行时断言 - 在发布版本中不禁用 */
#ifdef NDEBUG
    #define RUNTIME_ASSERT(condition) \
        do { \
            if (!(condition)) { \
                enter_safe_state(); \
            } \
        } while (0)
#else
    #define RUNTIME_ASSERT(condition) \
        do { \
            if (!(condition)) { \
                debug_break(); \
                enter_safe_state(); \
            } \
        } while (0)
#endif

/* 范围检查宏 */
#define CHECK_RANGE(value, min, max) \
    (((value) >= (min)) && ((value) <= (max)))

#define CHECK_POINTER(ptr) ((ptr) != NULL)

/* 安全检查包装 */
#define SAFE_DEREF(ptr) \
    (CHECK_POINTER(ptr) ? (ptr) : (enter_safe_state(), (ptr)))

/* 饱和运算 */
static inline uint16_t sat_add_u16(uint16_t a, uint16_t b) {
    uint32_t result = (uint32_t)a + (uint32_t)b;
    return (result > 0xFFFFU) ? 0xFFFFU : (uint16_t)result;
}

static inline int16_t sat_add_s16(int16_t a, int16_t b) {
    int32_t result = (int32_t)a + (int32_t)b;
    if (result > INT16_MAX) return INT16_MAX;
    if (result < INT16_MIN) return INT16_MIN;
    return (int16_t)result;
}

#endif
```

---

## 6. 内存保护和错误处理

### 6.1 MPU (内存保护单元) 配置

```c
/* mpu/mpu_config.c */
#include "mpu/mpu_config.h"

/* ARM Cortex-M4 MPU 寄存器 */
#define MPU_BASE            0xE000ED90
#define MPU_TYPER          (*(volatile uint32_t*)(MPU_BASE + 0x00))
#define MPU_CTRL           (*(volatile uint32_t*)(MPU_BASE + 0x04))
#define MPU_RNR            (*(volatile uint32_t*)(MPU_BASE + 0x08))
#define MPU_RBAR           (*(volatile uint32_t*)(MPU_BASE + 0x0C))
#define MPU_RASR           (*(volatile uint32_t*)(MPU_BASE + 0x10))

/* 区域属性 */
#define MPU_ATTR_XN         (1 << 28)   /* 禁止执行 */
#define MPU_ATTR_AP_RW_RW   (3 << 24)   /* 特权读写, 非特权读写 */
#define MPU_ATTR_AP_RW_RO   (2 << 24)   /* 特权读写, 非特权只读 */
#define MPU_ATTR_AP_RW_NONE (1 << 24)   /* 特权读写, 非特权无访问 */
#define MPU_ATTR_AP_RW_RW_NP (0 << 24)  /* 特权读写 */
#define MPU_ATTR_CB_WB_WA   (3 << 16)   /* Write-Back, Write-Allocate */
#define MPU_ATTR_CB_WT      (2 << 16)   /* Write-Through */
#define MPU_ATTR_CB_WB      (1 << 16)   /* Write-Back, no Write-Allocate */
#define MPU_ATTR_CB_NON     (0 << 16)   /* Non-cacheable */
#define MPU_ATTR_S          (1 << 18)   /* 共享 */

/* MPU 区域配置表 */
typedef struct {
    uint32_t base_address;
    uint32_t size;          /* 2^n 字节, n >= 4 */
    uint32_t attributes;
    bool enable;
} mpu_region_config_t;

/* 典型嵌入式系统 MPU 配置 */
static const mpu_region_config_t g_mpu_config[] = {
    /* 区域 0: Flash - 只读执行 */
    {
        .base_address = 0x08000000,
        .size = 512 * 1024,  /* 512KB */
        .attributes = MPU_ATTR_AP_RW_RO | MPU_ATTR_CB_WT | MPU_ATTR_S,
        .enable = true
    },
    /* 区域 1: RAM - 读写, 禁止执行 */
    {
        .base_address = 0x20000000,
        .size = 128 * 1024,  /* 128KB */
        .attributes = MPU_ATTR_XN | MPU_ATTR_AP_RW_RW | MPU_ATTR_CB_WB_WA | MPU_ATTR_S,
        .enable = true
    },
    /* 区域 2: 外设 - 禁止缓存, 禁止执行 */
    {
        .base_address = 0x40000000,
        .size = 512 * 1024,  /* 512KB */
        .attributes = MPU_ATTR_XN | MPU_ATTR_AP_RW_RW_NP | MPU_ATTR_CB_NON | MPU_ATTR_S,
        .enable = true
    },
    /* 区域 3: 栈保护 - 不可访问 (作为哨兵) */
    {
        .base_address = 0x2001F000,  /* 栈底 */
        .size = 4 * 1024,  /* 4KB 保护区域 */
        .attributes = MPU_ATTR_XN | MPU_ATTR_AP_RW_NONE | MPU_ATTR_CB_NON,
        .enable = true
    }
};

void mpu_init(void) {
    /* 禁用 MPU */
    MPU_CTRL = 0;

    uint32_t region_count = sizeof(g_mpu_config) / sizeof(g_mpu_config[0]);

    for (uint32_t i = 0; i < region_count && i < 8; i++) {
        const mpu_region_config_t* cfg = &g_mpu_config[i];

        /* 选择区域 */
        MPU_RNR = i;

        /* 配置基地址 */
        uint32_t rbar = cfg->base_address | (i << 4) | (cfg->enable ? 1 : 0);
        MPU_RBAR = rbar;

        /* 计算大小编码: 2^(SIZE+1) 字节, SIZE >= 4 */
        uint32_t size_encoding = 0;
        uint32_t temp_size = cfg->size;
        while (temp_size > 1) {
            temp_size >>= 1;
            size_encoding++;
        }
        size_encoding -= 1;  /* MPU 编码: 2^(SIZE+1) */

        /* 配置属性和大小 */
        uint32_t rasr = (size_encoding << 1) | cfg->attributes | (cfg->enable ? 1 : 0);
        MPU_RASR = rasr;
    }

    /* 启用 MPU (带默认内存映射背景区域) */
    MPU_CTRL = 0x05;  /* ENABLE | HFNMIENA */

    /* 内存屏障 */
    __DSB();
    __ISB();
}

/* 栈溢出检测 */
bool mpu_check_stack_overflow(void) {
    /* 检查 MPU 故障状态 */
    uint32_t cfsr = *(volatile uint32_t*)0xE000ED28;

    if (cfsr & 0x80) {  /* MMARVALID: 内存管理故障地址有效 */
        uint32_t mmar = *(volatile uint32_t*)0xE000ED34;

        /* 检查是否访问了栈保护区 */
        if (mmar >= 0x2001F000 && mmar < 0x20020000) {
            return true;
        }
    }

    return false;
}
```

### 6.2 错误处理框架

```c
/* safety/error_handler.h */
#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <stdint.h>

typedef enum {
    SEVERITY_INFO = 0,
    SEVERITY_WARNING,
    SEVERITY_ERROR,
    SEVERITY_CRITICAL
} error_severity_t;

typedef enum {
    ERR_MODULE_ABS = 0x01,
    ERR_MODULE_ECU = 0x02,
    ERR_MODULE_CAN = 0x03,
    ERR_MODULE_MPU = 0x04,
    ERR_MODULE_WDG = 0x05
} error_module_t;

/* 错误代码编码: 模块(8) | 严重度(4) | 序号(4) */
#define MAKE_ERROR_CODE(module, severity, code) \
    (((module) << 8) | ((severity) << 4) | (code))

/* 错误记录 */
typedef struct {
    uint32_t timestamp_ms;
    uint16_t error_code;
    uint32_t parameter;
    uint32_t pc;           /* 程序计数器 */
    uint32_t lr;           /* 链接寄存器 */
} error_record_t;

/* 错误处理函数 */
void error_handler_init(void);
void error_report(error_severity_t severity,
                  error_module_t module,
                  uint8_t code,
                  uint32_t parameter);
void error_report_with_context(error_severity_t severity,
                               error_module_t module,
                               uint8_t code,
                               uint32_t parameter,
                               uint32_t pc,
                               uint32_t lr);

/* 安全状态进入 */
void enter_safe_state(void);
void system_reset(void);

/* 故障恢复 */
typedef void (*recovery_action_t)(void);
void error_register_recovery(error_module_t module, recovery_action_t action);

#endif
```

```c
/* safety/error_handler.c */
#include "safety/error_handler.h"
#include "platform/nvm.h"
#include "network/can_driver.h"

#define MAX_ERROR_RECORDS 32
#define ERROR_LOG_ADDRESS   0x080F0000  /* Flash 日志区域 */

static error_record_t g_error_log[MAX_ERROR_RECORDS];
static uint32_t g_error_count = 0;
static uint32_t g_critical_error_count = 0;
static recovery_action_t g_recovery_actions[256] = {NULL};

void error_report(error_severity_t severity,
                  error_module_t module,
                  uint8_t code,
                  uint32_t parameter) {
    uint32_t pc, lr;

    /* 获取调用上下文 */
    __asm volatile ("mov %0, pc" : "=r" (pc));
    __asm volatile ("mov %0, lr" : "=r" (lr));

    error_report_with_context(severity, module, code, parameter, pc, lr);
}

void error_report_with_context(error_severity_t severity,
                               error_module_t module,
                               uint8_t code,
                               uint32_t parameter,
                               uint32_t pc,
                               uint32_t lr) {
    error_record_t record;

    record.timestamp_ms = get_system_tick();
    record.error_code = MAKE_ERROR_CODE(module, severity, code);
    record.parameter = parameter;
    record.pc = pc;
    record.lr = lr;

    /* 记录到内存 */
    if (g_error_count < MAX_ERROR_RECORDS) {
        g_error_log[g_error_count++] = record;
    }

    /* 写入非易失性存储 */
    nvm_write(ERROR_LOG_ADDRESS + (g_error_count * sizeof(error_record_t)),
              &record, sizeof(record));

    /* 通过 CAN 发送诊断信息 */
    can_frame_t diag_frame;
    diag_frame.id = 0x7FF;  /* 诊断帧 ID */
    diag_frame.dlc = 8;
    pack_u16(&diag_frame.data[0], record.error_code);
    pack_u32(&diag_frame.data[2], record.parameter);
    pack_u16(&diag_frame.data[6], (uint16_t)(record.timestamp_ms / 1000));
    can_send(can_get_controller(0), &diag_frame, 10);

    /* 根据严重度处理 */
    switch (severity) {
        case SEVERITY_INFO:
        case SEVERITY_WARNING:
            /* 继续运行，记录日志 */
            break;

        case SEVERITY_ERROR:
            /* 尝试恢复 */
            if (g_recovery_actions[module]) {
                g_recovery_actions[module]();
            }
            break;

        case SEVERITY_CRITICAL:
            g_critical_error_count++;
            if (g_critical_error_count >= 3) {
                /* 过多严重错误，进入安全状态 */
                enter_safe_state();
            } else {
                /* 单次严重错误，尝试恢复或降级 */
                if (g_recovery_actions[module]) {
                    g_recovery_actions[module]();
                } else {
                    enter_safe_state();
                }
            }
            break;
    }
}

void enter_safe_state(void) {
    /* 禁用中断 */
    __disable_irq();

    /* 关闭 ABS 控制 */
    abs_controller_enable(NULL, false);

    /* 打开所有电磁阀 (释放压力) */
    pwm_set_duty(PWM_INLET_VALVE(0), 255);
    pwm_set_duty(PWM_INLET_VALVE(1), 255);
    pwm_set_duty(PWM_INLET_VALVE(2), 255);
    pwm_set_duty(PWM_INLET_VALVE(3), 255);

    pwm_set_duty(PWM_OUTLET_VALVE(0), 255);
    pwm_set_duty(PWM_OUTLET_VALVE(1), 255);
    pwm_set_duty(PWM_OUTLET_VALVE(2), 255);
    pwm_set_duty(PWM_OUTLET_VALVE(3), 255);

    /* 关闭泵电机 */
    pwm_set_duty(PWM_PUMP_MOTOR(0), 0);

    /* 设置故障灯 */
    gpio_set(ABS_WARNING_LED, true);

    /* 发送故障状态到 CAN */
    can_frame_t fault_frame;
    fault_frame.id = CAN_ID_ABS_STATUS;
    fault_frame.dlc = 1;
    fault_frame.data[0] = 0xFF;  /* 故障状态 */
    can_send(can_get_controller(0), &fault_frame, 10);

    /* 记录故障 */
    error_record_t fault_record;
    fault_record.timestamp_ms = get_system_tick();
    fault_record.error_code = MAKE_ERROR_CODE(ERR_MODULE_ECU, SEVERITY_CRITICAL, 0xFF);
    fault_record.parameter = g_critical_error_count;
    nvm_write(ERROR_LOG_ADDRESS, &fault_record, sizeof(fault_record));

    /* 系统复位或等待看门狗 */
    #ifdef SAFE_STATE_RESET
        system_reset();
    #else
        while (1) {
            /* 闪烁故障灯 */
            gpio_toggle(ABS_WARNING_LED);
            delay_ms(500);
        }
    #endif
}

void system_reset(void) {
    /* 触发系统复位 */
    *(volatile uint32_t*)0xE000ED0C = 0x05FA0004;  /* AIRCR: System Reset */
}
```

---

## 参考资料

1. **ISO 26262-1:2018** - Road vehicles — Functional safety
2. **MISRA C:2012 Guidelines** - Motor Industry Software Reliability Association
3. **AUTOSAR Specification** - Classic Platform OS
4. **OSEK/VDX Operating System** - Version 2.2.3
5. **ARM Cortex-M4 Technical Reference Manual**
6. **Bosch Automotive Handbook** - 10th Edition

---

## 认证与合规

- **功能安全**: ISO 26262 ASIL D 认证
- **编码规范**: MISRA C:2012 合规
- **网络安全**: ISO/SAE 21434
- **EMC**: CISPR 25, ISO 11452


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
