/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 188
 * Language: c
 * Block ID: 2e90d996
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 标准保护 */
/* project_module_component.h */
#ifndef PROJECT_MODULE_COMPONENT_H
#define PROJECT_MODULE_COMPONENT_H

/* 内容 */

#endif /* PROJECT_MODULE_COMPONENT_H */

/* ✅ 合规 - 命名空间前缀 */
/* sensor_temperature_h */
#ifndef SENSOR_TEMPERATURE_H
#define SENSOR_TEMPERATURE_H

#include <stdint.h>

/* 类型定义 */
typedef struct {
    int16_t celsius;
    uint8_t precision;
} sensor_temp_t;

/* 函数声明 */
int sensor_temp_init(void);
int sensor_temp_read(sensor_temp_t *out);

#endif /* SENSOR_TEMPERATURE_H */

/* ✅ 合规 - 使用#pragma once（非标准但广泛支持）*/
#pragma once  /* 作为双重保护 */
#ifndef UNIQUE_GUARD
#define UNIQUE_GUARD
/* ... */
#endif
