/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\README.md
 * Line: 1323
 * Language: c
 * Block ID: 8a1ac680
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* safety/error_handler.h */
#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <stdint.h>

typedef enum {
    SEVERITY_INFO = 0,
    SEVERITY_WARNING,
    SEVERITY_ERROR,
    SEVERITY_CRITICAL
} error_severity_t;

typedef enum {
    ERR_MODULE_ABS = 0x01,
    ERR_MODULE_ECU = 0x02,
    ERR_MODULE_CAN = 0x03,
    ERR_MODULE_MPU = 0x04,
    ERR_MODULE_WDG = 0x05
} error_module_t;

/* 错误代码编码: 模块(8) | 严重度(4) | 序号(4) */
#define MAKE_ERROR_CODE(module, severity, code) \
    (((module) << 8) | ((severity) << 4) | (code))

/* 错误记录 */
typedef struct {
    uint32_t timestamp_ms;
    uint16_t error_code;
    uint32_t parameter;
    uint32_t pc;           /* 程序计数器 */
    uint32_t lr;           /* 链接寄存器 */
} error_record_t;

/* 错误处理函数 */
void error_handler_init(void);
void error_report(error_severity_t severity,
                  error_module_t module,
                  uint8_t code,
                  uint32_t parameter);
void error_report_with_context(error_severity_t severity,
                               error_module_t module,
                               uint8_t code,
                               uint32_t parameter,
                               uint32_t pc,
                               uint32_t lr);

/* 安全状态进入 */
void enter_safe_state(void);
void system_reset(void);

/* 故障恢复 */
typedef void (*recovery_action_t)(void);
void error_register_recovery(error_module_t module, recovery_action_t action);

#endif
