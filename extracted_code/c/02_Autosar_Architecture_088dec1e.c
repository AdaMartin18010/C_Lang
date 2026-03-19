/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\02_Autosar_Architecture.md
 * Line: 608
 * Language: c
 * Block ID: 088dec1e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Autosar_SWC_Test.c
 * AUTOSAR SWC测试程序
 */

#include "EngineTempMonitor_SWC.h"
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

/* 外部存根接口 */
extern void Rte_Stub_SetTemperature(sint16 temp);
extern void Rte_Stub_SetSensorFault(uint8 fault);
extern sint16 Rte_Stub_GetOutputTemp(void);
extern boolean Rte_Stub_GetWarning(void);
extern uint8 Rte_Stub_GetFanControl(void);
extern void Rte_Stub_PrintStatus(void);

/* 测试用例 */
void test_init(void)
{
    printf("\n测试: 初始化\n");
    EngineTempMonitor_Init();
    assert(EngineTempMonitor_GetSampleCount() == 0);
    printf("  ✓ 初始化成功\n");
}

void test_normal_operation(void)
{
    printf("\n测试: 正常运行\n");

    /* 设置正常温度 */
    Rte_Stub_SetTemperature(75);
    EngineTempMonitor_Run();

    assert(Rte_Stub_GetOutputTemp() == 75);
    assert(Rte_Stub_GetFanControl() == 1);  /* FAN_LOW */
    assert(Rte_Stub_GetWarning() == false);
    printf("  ✓ 正常运行测试通过\n");
    Rte_Stub_PrintStatus();
}

void test_warning_threshold(void)
{
    printf("\n测试: 警告阈值\n");

    /* 设置警告温度 */
    Rte_Stub_SetTemperature(105);
    EngineTempMonitor_Run();

    assert(Rte_Stub_GetWarning() == true);
    assert(Rte_Stub_GetFanControl() == 3);  /* FAN_HIGH */
    printf("  ✓ 警告阈值测试通过\n");
    Rte_Stub_PrintStatus();
}

void test_sensor_fault(void)
{
    printf("\n测试: 传感器故障\n");

    /* 设置故障 */
    Rte_Stub_SetSensorFault(1);
    EngineTempMonitor_Run();

    assert(Rte_Stub_GetWarning() == true);
    assert(Rte_Stub_GetFanControl() == 3);  /* 安全模式：风扇全速 */
    printf("  ✓ 传感器故障测试通过\n");
    Rte_Stub_PrintStatus();

    /* 恢复 */
    Rte_Stub_SetSensorFault(0);
}

void test_temperature_curve(void)
{
    printf("\n测试: 温度曲线\n");

    const sint16 temps[] = {20, 50, 65, 85, 95, 110, 125};
    const char* fanNames[] = {"OFF", "LOW", "MED", "HIGH"};

    for (int i = 0; i < 7; i++) {
        Rte_Stub_SetTemperature(temps[i]);
        EngineTempMonitor_Run();

        printf("  输入: %3d°C => 风扇: %s, 警告: %s\n",
               temps[i],
               fanNames[Rte_Stub_GetFanControl()],
               Rte_Stub_GetWarning() ? "ON" : "OFF");
    }
    printf("  ✓ 温度曲线测试完成\n");
}

int main(void)
{
    printf("=== AUTOSAR SWC测试程序 ===\n");
    printf("组件: 发动机温度监控 (EngineTempMonitor)\n");

    /* 运行测试 */
    test_init();
    test_normal_operation();
    test_warning_threshold();
    test_sensor_fault();
    test_temperature_curve();

    /* 停止组件 */
    EngineTempMonitor_Stop();
    printf("\n=== 所有测试通过 ===\n");

    return 0;
}
