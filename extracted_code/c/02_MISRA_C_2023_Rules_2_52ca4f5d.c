/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 369
 * Language: c
 * Block ID: 52ca4f5d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 只声明使用的类型 */
typedef int32_t sensor_value_t;

struct SensorData {
    sensor_value_t value;
    uint32_t timestamp;
};

void process_sensor(struct SensorData *data)
{
    /* 使用声明的类型 */
}
