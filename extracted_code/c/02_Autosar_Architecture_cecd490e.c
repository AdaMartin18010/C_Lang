/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\02_Autosar_Architecture.md
 * Line: 502
 * Language: c
 * Block ID: cecd490e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Rte_Stub.c
 * RTE存根实现，用于独立测试SWC
 */

#include "EngineTempMonitor_SWC.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* 模拟传感器数据 */
static sint16 g_sensorTemperature = 85;
static uint8  g_sensorStatus = 0;

/* 模拟输出数据 */
static boolean g_tempWarning = false;
static uint8   g_fanControl = 0;
static sint16  g_outputTemp = 0;

/* 模拟诊断事件 */
static uint8 g_diagnosticEvents[8] = {0};

/* RTE接口实现 */
Std_ReturnType Rte_Read_EngineTempSensor_Temperature(sint16* data)
{
    if (data == NULL) {
        return E_NOT_OK;
    }
    *data = g_sensorTemperature;
    return E_OK;
}

Std_ReturnType Rte_Read_EngineTempSensor_SensorStatus(uint8* data)
{
    if (data == NULL) {
        return E_NOT_OK;
    }
    *data = g_sensorStatus;
    return E_OK;
}

Std_ReturnType Rte_Write_EngineTempStatus_TempWarning(boolean data)
{
    g_tempWarning = data;
    return E_OK;
}

Std_ReturnType Rte_Write_EngineTempStatus_FanControl(uint8 data)
{
    g_fanControl = data;
    return E_OK;
}

Std_ReturnType Rte_Write_EngineTempStatus_TempValue(sint16 data)
{
    g_outputTemp = data;
    return E_OK;
}

Std_ReturnType Rte_Call_DiagnosticMonitor_SetEventStatus(uint8 eventId, uint8 status)
{
    if (eventId < 8) {
        g_diagnosticEvents[eventId] = status;
    }
    return E_OK;
}

/* 测试控制接口 */
void Rte_Stub_SetTemperature(sint16 temp)
{
    g_sensorTemperature = temp;
}

void Rte_Stub_SetSensorFault(uint8 fault)
{
    g_sensorStatus = fault;
}

sint16 Rte_Stub_GetOutputTemp(void)
{
    return g_outputTemp;
}

boolean Rte_Stub_GetWarning(void)
{
    return g_tempWarning;
}

uint8 Rte_Stub_GetFanControl(void)
{
    return g_fanControl;
}

void Rte_Stub_PrintStatus(void)
{
    const char* fanStr[] = {"OFF", "LOW", "MED", "HIGH"};
    printf("  输入温度: %d°C\n", g_sensorTemperature);
    printf("  输出温度: %d°C\n", g_outputTemp);
    printf("  警告标志: %s\n", g_tempWarning ? "ON" : "OFF");
    printf("  风扇控制: %s\n", fanStr[g_fanControl & 0x03]);
}
