/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\02_Autosar_Architecture.md
 * Line: 154
 * Language: c
 * Block ID: d2b0d7bd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * AUTOSAR风格软件组件(SWC)实现示例
 * 模拟发动机温度监控系统
 * 遵循AUTOSAR Classic平台规范
 */

#ifndef __AUTOSAR_TYPES_H
#define __AUTOSAR_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* AUTOSAR标准类型定义 */
#ifndef STD_TYPES_H
#define STD_TYPES_H

typedef uint8_t  boolean;
typedef int8_t   sint8;
typedef uint8_t  uint8;
typedef int16_t  sint16;
typtypedef uint16_t uint16;
typedef int32_t  sint32;
typedef uint32_t uint32;
typedef float    float32;
typedef double   float64;

/* 标准返回类型 */
typedef uint8 Std_ReturnType;

#define E_OK        (0x00U)
#define E_NOT_OK    (0x01U)
#define E_BUSY      (0x02U)

#endif /* STD_TYPES_H */

/* RTE API函数声明 - 由RTE生成器生成 */
#ifndef RTE_EngineTempMonitor_H
#define RTE_EngineTempMonitor_H

/* Sender-Receiver接口: 温度传感器数据 */
#define RTE_E_OK            (0U)
#define RTE_E_NEVER_RECEIVED (128U)
#define RTE_E_INVALID       (1U)

/* RTE读取接口 */
extern Std_ReturnType Rte_Read_EngineTempSensor_Temperature(sint16* data);
extern Std_ReturnType Rte_Read_EngineTempSensor_SensorStatus(uint8* data);

/* RTE写入接口 */
extern Std_ReturnType Rte_Write_EngineTempStatus_TempWarning(boolean data);
extern Std_ReturnType Rte_Write_EngineTempStatus_FanControl(uint8 data);
extern Std_ReturnType Rte_Write_EngineTempStatus_TempValue(sint16 data);

/* 诊断事件设置 */
extern Std_ReturnType Rte_Call_DiagnosticMonitor_SetEventStatus(uint8 eventId, uint8 status);

#endif /* RTE_EngineTempMonitor_H */

#endif /* __AUTOSAR_TYPES_H */
