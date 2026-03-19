/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\04_MISRA_C_2023_Rules_4.md
 * Line: 341
 * Language: c
 * Block ID: 96a5cb0d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用命名空间前缀 */
/* sensor.h */
typedef enum {
    SENSOR_OK = 0,
    SENSOR_ERROR_INIT,
    SENSOR_ERROR_READ
} sensor_status_t;

/* network.h */
typedef enum {
    NET_OK = 0,
    NET_ERROR_CONNECT,
    NET_ERROR_TIMEOUT
} network_status_t;

/* ✅ 合规 - 统一类型定义 */
/* common_types.h */
#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR_GENERIC,
    STATUS_ERROR_INVALID_PARAM,
    STATUS_ERROR_TIMEOUT,
    STATUS_ERROR_NO_MEMORY
} system_status_t;

#endif
