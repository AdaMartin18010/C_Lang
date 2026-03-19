/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\01_ABS_System.md
 * Line: 1977
 * Language: c
 * Block ID: 77044b9a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*============================================================================
 * MODULE:    ABS_FaultDiagnosis
 * FILE:      abs_fault_diag.c
 * DESCRIPTION: ABS故障诊断与处理 - 符合ISO 26262要求
 *============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*============================================================================
 * 故障类型定义
 *============================================================================*/
typedef enum {
    /* 传感器故障 */
    FAULT_SENSOR_FL_OPEN = 0x01,        /* 左前传感器开路 */
    FAULT_SENSOR_FL_SHORT,              /* 左前传感器短路 */
    FAULT_SENSOR_FR_OPEN,               /* 右前传感器开路 */
    FAULT_SENSOR_FR_SHORT,              /* 右前传感器短路 */
    FAULT_SENSOR_RL_OPEN,               /* 左后传感器开路 */
    FAULT_SENSOR_RL_SHORT,              /* 左后传感器短路 */
    FAULT_SENSOR_RR_OPEN,               /* 右后传感器开路 */
    FAULT_SENSOR_RR_SHORT,              /* 右后传感器短路 */
    FAULT_SENSOR_PLAUSIBILITY,          /* 传感器合理性故障 */

    /* 执行器故障 */
    FAULT_VALVE_INLET_FL,               /* 左前进油阀故障 */
    FAULT_VALVE_OUTLET_FL,              /* 左前出油阀故障 */
    FAULT_VALVE_INLET_FR,               /* 右前进油阀故障 */
    FAULT_VALVE_OUTLET_FR,              /* 右前出油阀故障 */
    FAULT_VALVE_INLET_RL,               /* 左后进油阀故障 */
    FAULT_VALVE_OUTLET_RL,              /* 左后出油阀故障 */
    FAULT_VALVE_INLET_RR,               /* 右后进油阀故障 */
    FAULT_VALVE_OUTLET_RR,              /* 右后出油阀故障 */
    FAULT_PUMP_MOTOR,                   /* 泵电机故障 */

    /* ECU内部故障 */
    FAULT_ECU_RAM,                      /* RAM故障 */
    FAULT_ECU_ROM,                      /* ROM/Flash故障 */
    FAULT_ECU_CLOCK,                    /* 时钟故障 */
    FAULT_ECU_ADC,                      /* ADC故障 */
    FAULT_ECU_COMMUNICATION,            /* 内部通信故障 */

    /* 电源故障 */
    FAULT_POWER_OVERVOLTAGE,            /* 过压 */
    FAULT_POWER_UNDERVOLTAGE,           /* 欠压 */

    FAULT_COUNT_MAX
} ABS_FaultCodeType;

/* 故障严重程度 */
typedef enum {
    SEVERITY_NONE = 0,
    SEVERITY_INFO,                      /* 信息 */
    SEVERITY_WARNING,                   /* 警告 */
    SEVERITY_DEGRADED,                  /* 降级 */
    SEVERITY_CRITICAL                   /* 关键 */
} ABS_FaultSeverityType;

/* 故障状态 */
typedef enum {
    FAULT_STATUS_PASSIVE = 0,           /* 未激活 */
    FAULT_STATUS_CONFIRMED,             /* 已确认 */
    FAULT_STATUS_HEALING                /* 愈合中 */
} ABS_FaultStatusType;

/* 故障记录结构 */
typedef struct {
    ABS_FaultCodeType code;             /* 故障码 */
    ABS_FaultSeverityType severity;     /* 严重程度 */
    ABS_FaultStatusType status;         /* 状态 */
    uint32_t occurrence_count;          /* 发生次数 */
    uint32_t first_time;                /* 首次发生时间 */
    uint32_t last_time;                 /* 最后发生时间 */
    uint32_t healing_time;              /* 愈合时间 */
    uint8_t environmental_data[8];      /* 环境数据 */
} ABS_FaultRecordType;

/* 故障管理器 */
#define MAX_FAULT_RECORDS               32

typedef struct {
    ABS_FaultRecordType records[MAX_FAULT_RECORDS];
    uint8_t fault_count;
    uint32_t dtc_status;                /* DTC状态位域 */
    bool system_degraded;
    bool system_disabled;
    void (*notify_callback)(ABS_FaultCodeType code);
} ABS_FaultManagerType;

/*============================================================================
 * 诊断参数
 *============================================================================*/
#define FAULT_CONFIRM_CYCLES            5       /* 故障确认周期数 */
#define FAULT_HEALING_CYCLES            40      /* 故障愈合周期数 (400ms) */
#define DTC_STORAGE_ADDR                0x00010000u  /* DTC存储地址 */

/*============================================================================
 * 静态变量
 *============================================================================*/
static ABS_FaultManagerType fault_manager;
static uint16_t fault_counters[FAULT_COUNT_MAX];
static uint16_t healing_counters[FAULT_COUNT_MAX];

/*============================================================================
 * 内部函数原型
 *============================================================================*/
static ABS_FaultRecordType* FindOrCreateFaultRecord(ABS_FaultCodeType code);
static void UpdateDTCStatus(void);
static void EnterDegradedMode(ABS_FaultCodeType code);
static void EnterLimpHomeMode(void);

/*============================================================================
 * 函数: ABS_FaultDiag_Init
 *============================================================================*/
void ABS_FaultDiag_Init(void)
{
    uint8_t i;

    memset(&fault_manager, 0, sizeof(fault_manager));

    for (i = 0u; i < FAULT_COUNT_MAX; i++) {
        fault_counters[i] = 0u;
        healing_counters[i] = 0u;
    }

    /* 从EEPROM加载历史DTC */
    ABS_FaultDiag_LoadDTCFromEEPROM();
}

/*============================================================================
 * 函数: ABS_FaultDiag_Report
 * 描述: 报告潜在故障
 *============================================================================*/
void ABS_FaultDiag_Report(ABS_FaultCodeType code, ABS_FaultSeverityType severity,
                          bool present)
{
    ABS_FaultRecordType *record;

    if (code >= FAULT_COUNT_MAX) {
        return;
    }

    if (present) {
        /* 故障存在 */
        healing_counters[code] = 0u;

        if (fault_counters[code] < FAULT_CONFIRM_CYCLES) {
            fault_counters[code]++;

            if (fault_counters[code] >= FAULT_CONFIRM_CYCLES) {
                /* 故障确认 */
                record = FindOrCreateFaultRecord(code);
                if (record != NULL) {
                    if (record->status == FAULT_STATUS_PASSIVE) {
                        record->code = code;
                        record->severity = severity;
                        record->status = FAULT_STATUS_CONFIRMED;
                        record->occurrence_count++;
                        record->first_time = GetSystemTime();

                        /* 通知上层 */
                        if (fault_manager.notify_callback != NULL) {
                            fault_manager.notify_callback(code);
                        }

                        /* 根据严重程度处理 */
                        switch (severity) {
                            case SEVERITY_WARNING:
                                /* 仅记录 */
                                break;
                            case SEVERITY_DEGRADED:
                                EnterDegradedMode(code);
                                break;
                            case SEVERITY_CRITICAL:
                                EnterLimpHomeMode();
                                break;
                            default:
                                break;
                        }
                    }
                    record->last_time = GetSystemTime();
                }
                UpdateDTCStatus();
            }
        }
    } else {
        /* 故障不存在 */
        fault_counters[code] = 0u;

        record = FindOrCreateFaultRecord(code);
        if (record != NULL && record->status == FAULT_STATUS_CONFIRMED) {
            healing_counters[code]++;

            if (healing_counters[code] >= FAULT_HEALING_CYCLES) {
                /* 故障愈合 */
                record->status = FAULT_STATUS_HEALING;
                record->healing_time = GetSystemTime();
            }
        }
    }
}

/*============================================================================
 * 函数: ABS_FaultDiag_SensorCheck
 * 描述: 传感器故障检测
 *============================================================================*/
void ABS_FaultDiag_SensorCheck(const float *wheel_speeds,
                                const uint8_t *sensor_status)
{
    uint8_t i;
    const ABS_FaultCodeType open_faults[4] = {
        FAULT_SENSOR_FL_OPEN, FAULT_SENSOR_FR_OPEN,
        FAULT_SENSOR_RL_OPEN, FAULT_SENSOR_RR_OPEN
    };
    const ABS_FaultCodeType short_faults[4] = {
        FAULT_SENSOR_FL_SHORT, FAULT_SENSOR_FR_SHORT,
        FAULT_SENSOR_RL_SHORT, FAULT_SENSOR_RR_SHORT
    };

    for (i = 0u; i < 4u; i++) {
        switch (sensor_status[i]) {
            case 1:  /* 开路 */
                ABS_FaultDiag_Report(open_faults[i], SEVERITY_DEGRADED, true);
                break;
            case 2:  /* 短路 */
                ABS_FaultDiag_Report(short_faults[i], SEVERITY_DEGRADED, true);
                break;
            default:
                ABS_FaultDiag_Report(open_faults[i], SEVERITY_DEGRADED, false);
                ABS_FaultDiag_Report(short_faults[i], SEVERITY_DEGRADED, false);
                break;
        }
    }

    /* 合理性检查 - 对角轮速差异 */
    if (ABS(wheel_speeds[0] - wheel_speeds[3]) > 50.0f ||
        ABS(wheel_speeds[1] - wheel_speeds[2]) > 50.0f) {
        ABS_FaultDiag_Report(FAULT_SENSOR_PLAUSIBILITY, SEVERITY_WARNING, true);
    } else {
        ABS_FaultDiag_Report(FAULT_SENSOR_PLAUSIBILITY, SEVERITY_WARNING, false);
    }
}

/*============================================================================
 * 函数: ABS_FaultDiag_ValveCheck
 * 描述: 阀自检 (上电时执行)
 *============================================================================*/
void ABS_FaultDiag_ValveCheck(void)
{
    /* 检测阀电阻 */
    float resistances[8];
    uint8_t i;

    ReadValveResistances(resistances);

    for (i = 0u; i < 8u; i++) {
        if (resistances[i] < 2.0f || resistances[i] > 20.0f) {
            /* 电阻异常 */
            ABS_FaultCodeType fault = FAULT_VALVE_INLET_FL + i;
            ABS_FaultDiag_Report(fault, SEVERITY_CRITICAL, true);
        }
    }

    /* 阀响应测试 */
    /* ... */
}

/*============================================================================
 * 函数: EnterDegradedMode
 *============================================================================*/
static void EnterDegradedMode(ABS_FaultCodeType code)
{
    fault_manager.system_degraded = true;

    /* 点亮ABS警告灯 */
    SetABSWarningLamp(true);

    /* 根据故障类型选择降级策略 */
    if (code >= FAULT_SENSOR_FL_OPEN && code <= FAULT_SENSOR_RR_SHORT) {
        /* 传感器故障 - 禁用该轮ABS，其他轮继续工作 */
        uint8_t wheel = (code - 1) / 2;
        DisableABSChannel(wheel);
    }
    else if (code >= FAULT_VALVE_INLET_FL && code <= FAULT_VALVE_OUTLET_RR) {
        /* 阀故障 - 禁用对应轴ABS */
        if (code <= FAULT_VALVE_OUTLET_FR) {
            DisableABSChannel(0);  /* 前轮 */
            DisableABSChannel(1);
        } else {
            DisableABSChannel(2);  /* 后轮 */
            DisableABSChannel(3);
        }
    }
}

/*============================================================================
 * 函数: EnterLimpHomeMode
 *============================================================================*/
static void EnterLimpHomeMode(void)
{
    fault_manager.system_disabled = true;

    /* 关闭所有ABS功能 */
    DisableAllABSChannels();

    /* 打开所有阀，确保常规制动通路 */
    OpenAllValves();
    StopPumpMotor();

    /* 点亮警告灯 */
    SetABSWarningLamp(true);

    /* 存储DTC到EEPROM */
    ABS_FaultDiag_StoreDTCToEEPROM();
}

/*============================================================================
 * 函数: ABS_FaultDiag_StoreDTCToEEPROM
 *============================================================================*/
void ABS_FaultDiag_StoreDTCToEEPROM(void)
{
    /* 将确认的故障码写入非易失存储 */
    uint8_t i;
    uint32_t dtc_data[8];

    memset(dtc_data, 0, sizeof(dtc_data));

    for (i = 0u; i < fault_manager.fault_count && i < 8u; i++) {
        if (fault_manager.records[i].status == FAULT_STATUS_CONFIRMED) {
            dtc_data[i] = ((uint32_t)fault_manager.records[i].code << 16) |
                          fault_manager.records[i].occurrence_count;
        }
    }

    EEPROM_Write(DTC_STORAGE_ADDR, (uint8_t*)dtc_data, sizeof(dtc_data));
}

/*============================================================================
 * 函数: ABS_FaultDiag_ClearDTC
 *============================================================================*/
void ABS_FaultDiag_ClearDTC(void)
{
    uint8_t i;

    for (i = 0u; i < MAX_FAULT_RECORDS; i++) {
        fault_manager.records[i].status = FAULT_STATUS_PASSIVE;
    }

    fault_manager.fault_count = 0u;
    fault_manager.system_degraded = false;
    fault_manager.system_disabled = false;

    /* 清除EEPROM */
    EEPROM_Erase(DTC_STORAGE_ADDR, 256u);
}
