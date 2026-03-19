/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\10_MISRA_C_2023_Rules_10.md
 * Line: 675
 * Language: c
 * Block ID: 5e332e90
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* module.h */
#ifndef MODULE_H
#define MODULE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============ 类型定义 ============ */

typedef enum {
    MODULE_OK = 0,
    MODULE_ERROR_INIT = -1,
    MODULE_ERROR_IO = -2
} module_status_t;

typedef struct module_config {
    uint32_t timeout_ms;
    uint8_t retry_count;
    bool enable_logging;
} module_config_t;

/* ============ 常量 ============ */

#define MODULE_VERSION_MAJOR 1
#define MODULE_VERSION_MINOR 0
#define MODULE_DEFAULT_TIMEOUT 1000

/* ============ 外部变量声明 ============ */

extern const char *MODULE_VERSION_STRING;
extern module_config_t g_default_config;

/* ============ 函数声明 ============ */

module_status_t module_init(const module_config_t *config);
void module_deinit(void);
bool module_is_ready(void);
module_status_t module_process(const uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* MODULE_H */
