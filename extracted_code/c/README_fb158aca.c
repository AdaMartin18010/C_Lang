/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\README.md
 * Line: 614
 * Language: c
 * Block ID: fb158aca
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file module.h
 * @brief 核心模块接口
 * @author Your Name
 * @date 2024
 * @copyright MIT License
 */

/**
 * @brief 创建模块上下文
 *
 * @param config 模块配置，不能为 NULL
 * @return 模块上下文指针，失败返回 NULL
 *
 * @note 返回的上下文必须用 module_destroy 释放
 *
 * @code
 * module_config_t cfg = {
 *     .buffer_size = 1024,
 *     .timeout_ms = 5000
 * };
 * module_ctx_t* ctx = module_create(&cfg);
 * if (!ctx) {
 *     // 错误处理
 * }
 * @endcode
 */
module_ctx_t* module_create(const module_config_t* config);

/**
 * @brief 错误码枚举
 */
typedef enum {
    MODULE_OK = 0,           /**< 成功 */
    MODULE_ERROR_PARAM = -1, /**< 无效参数 */
    MODULE_ERROR_NOMEM = -2, /**< 内存不足 */
} module_error_t;
