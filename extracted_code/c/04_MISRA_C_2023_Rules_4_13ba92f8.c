/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\04_MISRA_C_2023_Rules_4.md
 * Line: 105
 * Language: c
 * Block ID: 13ba92f8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 简洁明确的命名 */
int temp_celsius;
int temp_fahrenheit;

/* ✅ 合规 - 缩写使用 */
void process_temp_data(void);   /* process_temperature_data */
void process_temp_value(void);  /* process_temperature_value */

/* ✅ 合规 - 命名空间前缀 */
typedef struct {
    int sensor_id;
    float value;
} sensor_temp_t;    /* sensor_ 前缀 */

typedef struct {
    int ctrl_id;
    float setpoint;
} ctrl_param_t;     /* ctrl_ 前缀 */
