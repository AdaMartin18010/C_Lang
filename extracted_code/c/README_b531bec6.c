/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\README.md
 * Line: 87
 * Language: c
 * Block ID: b531bec6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// include/myproject/module.h - 模块公共接口
#ifndef MYPROJECT_MODULE_H
#define MYPROJECT_MODULE_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 前向声明不透明结构体
typedef struct module_ctx module_ctx_t;

// 错误码定义
typedef enum {
    MODULE_OK = 0,
    MODULE_ERROR_INVALID_PARAM = -1,
    MODULE_ERROR_NO_MEMORY = -2,
    MODULE_ERROR_IO = -3,
    MODULE_ERROR_TIMEOUT = -4
} module_error_t;

// 配置选项
typedef struct {
    size_t buffer_size;
    int timeout_ms;
    bool enable_logging;
    const char* log_file;
} module_config_t;

// 生命周期管理
module_ctx_t* module_create(const module_config_t* config);
void module_destroy(module_ctx_t* ctx);

// 核心操作
module_error_t module_open(module_ctx_t* ctx, const char* path);
module_error_t module_close(module_ctx_t* ctx);
module_error_t module_process(module_ctx_t* ctx, const void* input,
                               size_t input_len, void* output,
                               size_t* output_len);

// 状态查询
const char* module_error_string(module_error_t error);
bool module_is_ready(const module_ctx_t* ctx);

// 版本信息
const char* module_version(void);

#ifdef __cplusplus
}
#endif

#endif // MYPROJECT_MODULE_H
