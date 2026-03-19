/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 1954
 * Language: c
 * Block ID: 2ddfeb66
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file modular_design.h
 * @brief 模块化设计示例 - 日志模块接口
 *
 * 模块化原则：
 * 1. 信息隐藏：只暴露必要接口
 * 2. 高内聚：相关功能放在一起
 * 3. 低耦合：模块间依赖最小化
 * 4. 单一职责：每个模块只做一件事
 */

#ifndef MODULAR_DESIGN_H
#define MODULAR_DESIGN_H

#include <stdarg.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========== 模块接口（头文件只暴露这些） ========== */

/**
 * @brief 日志级别
 * @note 这是模块的私有实现细节，不应直接依赖具体数值
 */
typedef enum {
    LOG_LEVEL_TRACE = 0,  /**< 最详细，仅调试 */
    LOG_LEVEL_DEBUG = 1,  /**< 调试信息 */
    LOG_LEVEL_INFO  = 2,  /**< 一般信息 */
    LOG_LEVEL_WARN  = 3,  /**< 警告 */
    LOG_LEVEL_ERROR = 4,  /**< 错误 */
    LOG_LEVEL_FATAL = 5   /**< 致命错误 */
} log_level_t;

/**
 * @brief 日志模块配置
 */
typedef struct log_config log_config_t;

/**
 * @brief 日志输出目标类型
 */
typedef enum {
    LOG_TARGET_STDOUT,    /**< 标准输出 */
    LOG_TARGET_STDERR,    /**< 标准错误 */
    LOG_TARGET_FILE,      /**< 文件 */
    LOG_TARGET_SYSLOG,    /**< 系统日志 */
    LOG_TARGET_CALLBACK   /**< 回调函数 */
} log_target_type_t;

/**
 * @brief 日志回调函数类型
 */
typedef void (*log_callback_t)(log_level_t level,
                                const char *message,
                                void *user_data);

/* ========== 模块初始化与清理 ========== */

/**
 * @brief 初始化日志模块
 * @return 0成功，非0失败
 * @note 必须在其他日志函数前调用
 */
int log_init(void);

/**
 * @brief 清理日志模块资源
 * @note 应在程序退出前调用
 */
void log_cleanup(void);

/* ========== 配置接口 ========== */

/**
 * @brief 设置全局日志级别
 * @param level 低于此级别的日志将被忽略
 */
void log_set_level(log_level_t level);

/**
 * @brief 设置日志输出目标
 * @param target 目标类型
 * @param param 目标参数（文件路径或回调函数）
 * @return 0成功
 */
int log_set_target(log_target_type_t target, const void *param);

/**
 * @brief 启用/禁用日志颜色（终端输出时）
 */
void log_set_color_enabled(bool enabled);

/* ========== 日志输出接口 ========== */

/**
 * @brief 输出格式化日志
 *
 * 使用示例：
 * @code
 * LOG_INFO("Server started on port %d", port);
 * LOG_ERROR("Failed to open file: %s (errno=%d)", path, errno);
 * @endcode
 */
void log_message(log_level_t level, const char *file, int line,
                 const char *func, const char *fmt, ...);

/* 便捷宏，自动填充位置信息 */
#define LOG_TRACE(fmt, ...) \
    log_message(LOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) \
    log_message(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  \
    log_message(LOG_LEVEL_INFO,  __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  \
    log_message(LOG_LEVEL_WARN,  __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) \
    log_message(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) \
    log_message(LOG_LEVEL_FATAL, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

/* ========== 条件日志（避免不必要的格式化开销） ========== */

#define LOG_TRACE_ENABLED() (log_is_level_enabled(LOG_LEVEL_TRACE))
#define LOG_DEBUG_ENABLED() (log_is_level_enabled(LOG_LEVEL_DEBUG))

bool log_is_level_enabled(log_level_t level);

#ifdef __cplusplus
}
#endif

#endif /* MODULAR_DESIGN_H */
