/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\04_MISRA_C_2023_Rules_4.md
 * Line: 87
 * Language: c
 * Block ID: ac0602d7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 标识符前63字符相同 */
int analysis_of_sensor_data_from_temperature_probe_in_celsius;
int analysis_of_sensor_data_from_temperature_probe_in_fahrenheit;
/* 前63字符相同，编译器可能视为同一标识符 */

/* ❌ 违反 - 外部标识符前31字符相同 */
void process_temperature_sensor_data(void);
void process_temperature_sensor_value(void);
/* 某些链接器只识别前31字符 */

/* ❌ 违反 - 大小写不敏感系统冲突 */
int MyVariable;
int myVariable;  /* 在不区分大小写的文件系统中冲突 */
