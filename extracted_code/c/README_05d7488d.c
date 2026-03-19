/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\README.md
 * Line: 77
 * Language: c
 * Block ID: 05d7488d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
