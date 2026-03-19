/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\02_Autosar_Architecture.md
 * Line: 219
 * Language: c
 * Block ID: 0cc54012
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * EngineTempMonitor_SWC.c
 * 发动机温度监控软件组件
 *
 * 功能：
 * - 读取温度传感器数据
 * - 温度阈值监控
 * - 风扇控制决策
 * - 过热故障诊断
 */

#include "EngineTempMonitor_SWC.h"
#include <string.h>

/* MISRA C:2012 合规配置 */
#define TEMP_SENSOR_MIN        (-40)    /* 最低温度: -40°C */
#define TEMP_SENSOR_MAX        (150)    /* 最高温度: 150°C */
#define TEMP_WARNING_THRESHOLD (100)    /* 警告阈值: 100°C */
#define TEMP_CRITICAL_THRESHOLD (120)   /* 严重阈值: 120°C */
#define TEMP_SENSOR_INVALID    (0x8000) /* 无效值标记 */

/* 风扇控制等级 */
#define FAN_OFF                (0U)
#define FAN_LOW                (1U)
#define FAN_MEDIUM             (2U)
#define FAN_HIGH               (3U)

/* 诊断事件ID */
#define DEM_EVENT_ID_TEMP_SENSOR_FAULT  (0x01U)
#define DEM_EVENT_ID_ENGINE_OVERHEAT    (0x02U)

/* 诊断事件状态 */
#define DEM_EVENT_STATUS_PASSED         (0x00U)
#define DEM_EVENT_STATUS_FAILED         (0x01U)

/* 组件内部数据结构 */
typedef struct {
    sint16 currentTemperature;
    sint16 maxTemperature;
    uint8  fanSpeed;
    boolean tempWarning;
    boolean sensorFault;
    uint16 sampleCount;
} EngineTemp_MonitorDataType;

/* 组件私有变量 */
static EngineTemp_MonitorDataType g_monitorData;
static boolean g_initialized = false;

/* 本地函数声明 */
static uint8 CalculateFanSpeed(sint16 temperature);
static boolean ValidateTemperature(sint16 temperature);
static void UpdateDiagnosticEvents(sint16 temperature, boolean sensorFault);

/* ==================== 可运行实体实现 ==================== */

/**
 * @brief 初始化可运行实体
 * @details 由Rte_Start调用，初始化组件内部状态
 */
void EngineTempMonitor_Init(void)
{
    /* 初始化内部数据结构 */
    (void)memset(&g_monitorData, 0, sizeof(EngineTemp_MonitorDataType));

    g_monitorData.currentTemperature = 20;  /* 默认室温 */
    g_monitorData.maxTemperature = TEMP_SENSOR_MIN;
    g_monitorData.fanSpeed = FAN_OFF;
    g_monitorData.tempWarning = false;
    g_monitorData.sensorFault = false;
    g_monitorData.sampleCount = 0U;

    g_initialized = true;
}

/**
 * @brief 周期性监控可运行实体 (10ms周期)
 * @details 读取传感器、执行监控逻辑、输出控制
 */
void EngineTempMonitor_Run(void)
{
    Std_ReturnType readStatus;
    sint16 tempValue;
    uint8 sensorStatus;
    uint8 fanControl;
    boolean warning;

    /* 检查初始化状态 */
    if (!g_initialized) {
        return;
    }

    /* Step 1: 读取温度传感器数据 */
    readStatus = Rte_Read_EngineTempSensor_Temperature(&tempValue);

    if (readStatus != E_OK) {
        /* 读取失败，标记传感器故障 */
        g_monitorData.sensorFault = true;
        tempValue = (sint16)TEMP_SENSOR_INVALID;
    } else {
        g_monitorData.sensorFault = false;

        /* 验证温度值有效性 */
        if (!ValidateTemperature(tempValue)) {
            g_monitorData.sensorFault = true;
        }
    }

    /* Step 2: 读取传感器状态 */
    (void)Rte_Read_EngineTempSensor_SensorStatus(&sensorStatus);
    if (sensorStatus != 0U) {
        g_monitorData.sensorFault = true;
    }

    /* Step 3: 更新内部状态 */
    if (!g_monitorData.sensorFault) {
        g_monitorData.currentTemperature = tempValue;
        g_monitorData.sampleCount++;

        /* 更新最高温度记录 */
        if (tempValue > g_monitorData.maxTemperature) {
            g_monitorData.maxTemperature = tempValue;
        }
    }

    /* Step 4: 温度监控逻辑 */
    if (!g_monitorData.sensorFault) {
        /* 计算风扇控制等级 */
        fanControl = CalculateFanSpeed(g_monitorData.currentTemperature);
        g_monitorData.fanSpeed = fanControl;

        /* 设置警告标志 */
        warning = (g_monitorData.currentTemperature >= TEMP_WARNING_THRESHOLD);
        g_monitorData.tempWarning = warning;
    } else {
        /* 传感器故障时的安全状态 */
        fanControl = FAN_HIGH;  /* 安全模式：风扇全速 */
        warning = true;
    }

    /* Step 5: 输出结果到RTE */
    (void)Rte_Write_EngineTempStatus_FanControl(fanControl);
    (void)Rte_Write_EngineTempStatus_TempWarning(warning);

    if (!g_monitorData.sensorFault) {
        (void)Rte_Write_EngineTempStatus_TempValue(g_monitorData.currentTemperature);
    }

    /* Step 6: 更新诊断事件 */
    UpdateDiagnosticEvents(g_monitorData.currentTemperature,
                          g_monitorData.sensorFault);
}

/**
 * @brief 停止可运行实体
 * @details 由Rte_Stop调用，清理组件状态
 */
void EngineTempMonitor_Stop(void)
{
    /* 安全状态：关闭风扇，清除警告 */
    (void)Rte_Write_EngineTempStatus_FanControl(FAN_OFF);
    (void)Rte_Write_EngineTempStatus_TempWarning(false);

    g_initialized = false;
}

/* ==================== 本地函数实现 ==================== */

/**
 * @brief 计算风扇控制速度
 * @param temperature 当前温度值
 * @return 风扇控制等级 (0-3)
 */
static uint8 CalculateFanSpeed(sint16 temperature)
{
    uint8 fanSpeed;

    if (temperature < 60) {
        fanSpeed = FAN_OFF;
    } else if (temperature < 80) {
        fanSpeed = FAN_LOW;
    } else if (temperature < TEMP_WARNING_THRESHOLD) {
        fanSpeed = FAN_MEDIUM;
    } else if (temperature < TEMP_CRITICAL_THRESHOLD) {
        fanSpeed = FAN_HIGH;
    } else {
        /* 严重过热，风扇全速 */
        fanSpeed = FAN_HIGH;
    }

    return fanSpeed;
}

/**
 * @brief 验证温度值有效性
 * @param temperature 温度值
 * @return true-有效, false-无效
 */
static boolean ValidateTemperature(sint16 temperature)
{
    boolean valid = true;

    if ((temperature < TEMP_SENSOR_MIN) || (temperature > TEMP_SENSOR_MAX)) {
        valid = false;
    }

    /* 检查无效值标记 */
    if (temperature == (sint16)TEMP_SENSOR_INVALID) {
        valid = false;
    }

    return valid;
}

/**
 * @brief 更新诊断事件
 * @param temperature 当前温度
 * @param sensorFault 传感器故障标志
 */
static void UpdateDiagnosticEvents(sint16 temperature, boolean sensorFault)
{
    /* 温度传感器故障事件 */
    if (sensorFault) {
        (void)Rte_Call_DiagnosticMonitor_SetEventStatus(
            DEM_EVENT_ID_TEMP_SENSOR_FAULT,
            DEM_EVENT_STATUS_FAILED);
    } else {
        (void)Rte_Call_DiagnosticMonitor_SetEventStatus(
            DEM_EVENT_ID_TEMP_SENSOR_FAULT,
            DEM_EVENT_STATUS_PASSED);
    }

    /* 发动机过热事件 */
    if (temperature >= TEMP_CRITICAL_THRESHOLD) {
        (void)Rte_Call_DiagnosticMonitor_SetEventStatus(
            DEM_EVENT_ID_ENGINE_OVERHEAT,
            DEM_EVENT_STATUS_FAILED);
    } else if (temperature < (TEMP_CRITICAL_THRESHOLD - 5)) {
        /* 滞后：温度下降5度后才清除 */
        (void)Rte_Call_DiagnosticMonitor_SetEventStatus(
            DEM_EVENT_ID_ENGINE_OVERHEAT,
            DEM_EVENT_STATUS_PASSED);
    }
}

/* ==================== 访问器函数 ==================== */

/**
 * @brief 获取当前温度
 */
sint16 EngineTempMonitor_GetCurrentTemp(void)
{
    return g_monitorData.currentTemperature;
}

/**
 * @brief 获取最高温度记录
 */
sint16 EngineTempMonitor_GetMaxTemp(void)
{
    return g_monitorData.maxTemperature;
}

/**
 * @brief 获取采样计数
 */
uint16 EngineTempMonitor_GetSampleCount(void)
{
    return g_monitorData.sampleCount;
}

/**
 * @brief 重置最高温度记录
 */
void EngineTempMonitor_ResetMaxTemp(void)
{
    g_monitorData.maxTemperature = g_monitorData.currentTemperature;
}
