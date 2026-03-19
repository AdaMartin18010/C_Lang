/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\04_OBD_Diagnostics.md
 * Line: 136
 * Language: c
 * Block ID: 8f16bf14
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 常见PID定义
#define PID_ENGINE_LOAD         0x04   // 发动机负荷
#define PID_COOLANT_TEMP        0x05   // 冷却液温度
#define PID_ENGINE_RPM          0x0C   // 发动机转速
#define PID_VEHICLE_SPEED       0x0D   // 车速
#define PID_INTAKE_AIR_TEMP     0x0F   // 进气温度
#define PID_MAF_RATE            0x10   // MAF流量
#define PID_THROTTLE_POS        0x11   // 节气门位置
#define PID_O2_SENSOR_B1S1      0x14   // 氧传感器电压
#define PID_FUEL_LEVEL          0x2F   // 燃油液位
#define PID_DISTANCE_MIL        0x21   // MIL灯亮后行驶距离

// 请求格式: [服务] [PID]
// 01 0C - 请求发动机转速

// 响应格式: [服务+0x40] [PID] [数据...]
// 41 0C 1B 56 - 转速 = 0x1B56 / 4 = 1750 RPM
