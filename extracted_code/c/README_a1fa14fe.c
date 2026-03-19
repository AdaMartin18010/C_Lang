/*
 * Auto-generated from: 07_Modern_Toolchain\06_Project_Templates\README.md
 * Line: 726
 * Language: c
 * Block ID: a1fa14fe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file module_name.h
 * @brief 模块功能简述
 * @author Author Name
 * @date 2024
 * @copyright MIT License
 *
 * @details
 * 详细描述模块的功能、使用方法和注意事项。
 *
 * @section example_sec 使用示例
 * @code{.c}
 *     // 初始化模块
 *     if (module_init() != 0) {
 *         // 错误处理
 *     }
 *
 *     // 使用模块功能
 *     module_process(data, len);
 *
 *     // 清理资源
 *     module_cleanup();
 * @endcode
 */

#ifndef PROJECT_MODULE_NAME_H
#define PROJECT_MODULE_NAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* 版本信息 */
#define MODULE_NAME_VERSION_MAJOR 1
#define MODULE_NAME_VERSION_MINOR 0
#define MODULE_NAME_VERSION_PATCH 0
#define MODULE_NAME_VERSION "1.0.0"

/* 可见性属性 */
#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef MODULE_NAME_BUILD_SHARED
        #define MODULE_API __declspec(dllexport)
    #elif defined(MODULE_NAME_USE_SHARED)
        #define MODULE_API __declspec(dllimport)
    #else
        #define MODULE_API
    #endif
#elif __GNUC__ >= 4
    #define MODULE_API __attribute__((visibility("default")))
#else
    #define MODULE_API
#endif

/* 错误码定义 */
typedef enum {
    MODULE_OK = 0,
    MODULE_ERROR_INVALID_PARAM = -1,
    MODULE_ERROR_MEMORY = -2,
    MODULE_ERROR_IO = -3,
    MODULE_ERROR_NOT_INITIALIZED = -4,
    MODULE_ERROR_ALREADY_INITIALIZED = -5,
    MODULE_ERROR_NOT_SUPPORTED = -6
} module_error_t;

/* 模块配置 */
typedef struct {
    uint32_t buffer_size;
    uint32_t timeout_ms;
    bool enable_feature_x;
    void (*log_callback)(int level, const char *msg);
} module_config_t;

/* 模块句柄 */
typedef struct module_handle module_handle_t;

/**
 * @brief 获取默认配置
 * @param config 配置结构体指针
 * @return MODULE_OK 成功，否则返回错误码
 */
MODULE_API module_error_t module_get_default_config(module_config_t *config);

/**
 * @brief 初始化模块
 * @param config 配置指针，NULL使用默认配置
 * @param handle 输出句柄指针
 * @return MODULE_OK 成功，否则返回错误码
 */
MODULE_API module_error_t module_init(const module_config_t *config,
                                       module_handle_t **handle);

/**
 * @brief 清理模块资源
 * @param handle 模块句柄
 */
MODULE_API void module_cleanup(module_handle_t *handle);

/**
 * @brief 处理数据
 * @param handle 模块句柄
 * @param input 输入数据
 * @param input_len 输入长度
 * @param output 输出缓冲区
 * @param output_len 输出缓冲区大小
 * @param bytes_written 实际写入字节数
 * @return MODULE_OK 成功，否则返回错误码
 */
MODULE_API module_error_t module_process(module_handle_t *handle,
                                          const uint8_t *input,
                                          size_t input_len,
                                          uint8_t *output,
                                          size_t output_len,
                                          size_t *bytes_written);

/**
 * @brief 获取错误描述
 * @param error 错误码
 * @return 错误描述字符串
 */
MODULE_API const char *module_error_string(module_error_t error);

/**
 * @brief 获取模块版本
 * @return 版本字符串
 */
MODULE_API const char *module_version(void);

#ifdef __cplusplus
}
#endif

#endif /* PROJECT_MODULE_NAME_H */
