/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\01_ABS_System.md
 * Line: 2584
 * Language: c
 * Block ID: 26d19fe1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*============================================================================
 * AUTOSAR ABS Software Component
 * File: Abs_SWC.c
 * Implements: ABS functionality as an AUTOSAR Classic Platform SWC
 *============================================================================*/

#include "Rte_Abs.h"            /* RTE生成的接口头文件 */
#include "ComStack_Types.h"

/*============================================================================
 * Runnable Entities (RE)
 *============================================================================*/

/* RE: Abs_Init
 * 周期: 上电初始化
 * 描述: ABS组件初始化 */
void RE_Abs_Init(void)
{
    /* 初始化轮速处理 */
    WSP_Init();

    /* 初始化滑移率计算 */
    SR_Init();

    /* 初始化控制算法 */
    ABS_LogicControl_Init();

    /* 初始化故障诊断 */
    ABS_FaultDiag_Init();

    /* 读取标定参数 */
    Abs_ReadCalibrationData();
}

/* RE: Abs_WheelSpeedProcess
 * 周期: 10ms
 * 描述: 轮速信号处理和参考车速估计 */
void RE_Abs_WheelSpeedProcess(void)
{
    float wheel_speeds[4];

    /* 读取RTE端口 - 从传感器接口 */
    Rte_Read_RPort_WheelSpeed_FL(&wheel_speeds[0]);
    Rte_Read_RPort_WheelSpeed_FR(&wheel_speeds[1]);
    Rte_Read_RPort_WheelSpeed_RL(&wheel_speeds[2]);
    Rte_Read_RPort_WheelSpeed_RR(&wheel_speeds[3]);

    /* 处理轮速 */
    WSP_Process_10ms();

    /* 更新滑移率计算 */
    SR_Update_10ms(wheel_speeds);

    /* 输出参考车速到RTE */
    Rte_Write_PPort_VehicleSpeed(SR_GetVehicleSpeed());
    Rte_Write_PPort_VehicleAccel(SR_GetVehicleAccel());
}

/* RE: Abs_Control
 * 周期: 10ms
 * 描述: ABS控制逻辑 */
void RE_Abs_Control(void)
{
    uint8_t i;
    float slip_ratios[4];
    float wheel_accels[4];
    ABS_ValveOutputsType valve_outputs[4];

    /* 读取输入 */
    for (i = 0u; i < 4u; i++) {
        slip_ratios[i] = SR_GetSlipRatio(i);
        wheel_accels[i] = WSP_GetAcceleration(i);
    }

    /* 执行控制 */
    for (i = 0u; i < 4u; i++) {
        ABS_LogicControl_10ms(i, slip_ratios[i], wheel_accels[i]);
        ABS_GetValveOutputs(i, &valve_outputs[i]);
    }

    /* 输出到执行器 */
    for (i = 0u; i < 4u; i++) {
        Rte_Write_PPort_ValveInlet(i, valve_outputs[i].inlet_valve);
        Rte_Write_PPort_ValveOutlet(i, valve_outputs[i].outlet_valve);
    }

    Rte_Write_PPort_PumpMotor(valve_outputs[0].pump_motor ||
                               valve_outputs[1].pump_motor ||
                               valve_outputs[2].pump_motor ||
                               valve_outputs[3].pump_motor);
}

/* RE: Abs_Diagnosis
 * 周期: 50ms
 * 描述: 故障诊断 */
void RE_Abs_Diagnosis(void)
{
    float wheel_speeds[4];
    uint8_t sensor_status[4];
    uint8_t i;

    /* 收集诊断数据 */
    for (i = 0u; i < 4u; i++) {
        wheel_speeds[i] = WSP_GetSpeed(i);
        sensor_status[i] = WSP_GetStatus(i);
    }

    /* 执行诊断 */
    ABS_FaultDiag_SensorCheck(wheel_speeds, sensor_status);

    /* 检查系统状态 */
    if (ABS_FaultDiag_IsSystemDisabled()) {
        Rte_Write_PPort_ABSWarningLamp(TRUE);
        Rte_Write_PPort_ABSStatus(ABS_STATUS_DISABLED);
    } else if (ABS_FaultDiag_IsSystemDegraded()) {
        Rte_Write_PPort_ABSWarningLamp(TRUE);
        Rte_Write_PPort_ABSStatus(ABS_STATUS_DEGRADED);
    } else if (ABS_IsAnyChannelActive()) {
        Rte_Write_PPort_ABSWarningLamp(FALSE);  /* 激活时灯灭或闪烁 */
        Rte_Write_PPort_ABSStatus(ABS_STATUS_ACTIVE);
    } else {
        Rte_Write_PPort_ABSWarningLamp(FALSE);
        Rte_Write_PPort_ABSStatus(ABS_STATUS_READY);
    }
}

/*============================================================================
 * AUTOSAR 端口接口定义示例 (ARXML生成的C代码)
 *============================================================================*/

/* Sender-Receiver Interface */
typedef struct {
    float vehicle_speed;            /* km/h */
    float vehicle_accel;            /* m/s² */
} SR_VehicleDynamicsType;

/* Client-Server Interface (UDS诊断) */
Std_ReturnType CSI_Dcm_ReadDTC(uint8_t *dtc_data, uint16_t *length)
{
    ABS_FaultRecordType *records;
    uint8_t count;
    uint8_t i;

    records = ABS_FaultDiag_GetRecords(&count);

    *length = 0;
    for (i = 0u; i < count && (*length + 4) < 256u; i++) {
        dtc_data[(*length)++] = (uint8_t)(records[i].code >> 8);
        dtc_data[(*length)++] = (uint8_t)(records[i].code);
        dtc_data[(*length)++] = (uint8_t)records[i].status;
        dtc_data[(*length)++] = records[i].occurrence_count;
    }

    return E_OK;
}

Std_ReturnType CSI_Dcm_ClearDTC(void)
{
    ABS_FaultDiag_ClearDTC();
    return E_OK;
}

/* Mode Switch Interface */
typedef enum {
    ABS_MODE_NORMAL,
    ABS_MODE_DEGRADED,
    ABS_MODE_LIMP_HOME
} ABS_ModeType;

void MSI_AbsModeSwitch(ABS_ModeType new_mode)
{
    switch (new_mode) {
        case ABS_MODE_NORMAL:
            EnableABS();
            break;
        case ABS_MODE_DEGRADED:
            EnterDegradedMode();
            break;
        case ABS_MODE_LIMP_HOME:
            EnterLimpHomeMode();
            break;
    }
}

/*============================================================================
 * 标定参数 (CAL) 定义
 *============================================================================*/

/* 标定参数RAM镜像 */
typedef struct {
    float slip_threshold_enter;
    float slip_threshold_exit;
    float decel_threshold_enter;
    float decel_threshold_exit;
    uint16_t time_hold_min;
    uint16_t time_dec_min;
    uint16_t time_inc_min;
} ABS_CalibrationType;

/* 定义在标定段 */
#define ABS_CALIB_SECTION __attribute__((section(".cal")))

ABS_CALIB_SECTION ABS_CalibrationType Abs_Calibration = {
    .slip_threshold_enter   = 0.15f,
    .slip_threshold_exit    = 0.08f,
    .decel_threshold_enter  = -8.0f,
    .decel_threshold_exit   = -5.0f,
    .time_hold_min          = 20u,
    .time_dec_min           = 20u,
    .time_inc_min           = 30u
};

void Abs_ReadCalibrationData(void)
{
    /* 从Flash读取标定数据到RAM工作区 */
    memcpy(&Abs_Params, &Abs_Calibration, sizeof(ABS_CalibrationType));
}
