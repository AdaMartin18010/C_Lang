/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Avionics_Systems\README.md
 * Line: 182
 * Language: c
 * Block ID: 16642026
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file flight_control_types.h
 * @brief 飞行控制系统 - DO-178C DAL A 级数据结构定义
 *
 * 符合 DO-178C DAL A 要求:
 * - 强类型检查
 * - 范围限制
 * - 数据监控
 */

#ifndef FLIGHT_CONTROL_TYPES_H
#define FLIGHT_CONTROL_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * 基本类型定义 - 使用固定宽度整数类型确保可移植性
 * ============================================================================ */

/**
 * @brief 飞行控制系统版本信息
 * @req DO-178C: 配置标识
 */
#define FCS_MAJOR_VERSION   1
#define FCS_MINOR_VERSION   2
#define FCS_PATCH_VERSION   3
#define FCS_VERSION_STRING  "1.2.3"

/* ============================================================================
 * 范围受限的数值类型
 * 使用结构体封装确保范围检查和类型安全
 * ============================================================================ */

/**
 * @brief 高度类型（英尺）
 * @range [-1000, 60000] 英尺
 */
typedef struct {
    int32_t value;          /**< 高度值（英尺） */
    bool valid;             /**< 数据有效性标志 */
    uint32_t timestamp_ms;  /**< 时间戳（毫秒） */
} Altitude_t;

#define ALTITUDE_MIN    (-1000)
#define ALTITUDE_MAX    60000
#define ALTITUDE_INVALID (-9999)

/**
 * @brief 空速类型（节）
 * @range [0, 500] 节
 */
typedef struct {
    uint16_t value;         /**< 空速值（节） */
    bool valid;
    uint32_t timestamp_ms;
} Airspeed_t;

#define AIRSPEED_MIN    0
#define AIRSPEED_MAX    500
#define AIRSPEED_INVALID 0xFFFF

/**
 * @brief 航向类型（度）
 * @range [0, 359.9] 度
 */
typedef struct {
    uint16_t value_tenths;  /**< 航向值（0.1度为单位） */
    bool valid;
    uint32_t timestamp_ms;
} Heading_t;

#define HEADING_MIN     0
#define HEADING_MAX     3599    /* 359.9度 */
#define HEADING_INVALID 0xFFFF

/**
 * @brief 姿态角类型（度）
 * @range [-90, 90] 度（俯仰）
 * @range [-180, 180] 度（横滚）
 */
typedef struct {
    int16_t pitch_tenths;   /**< 俯仰角（0.1度） */
    int16_t roll_tenths;    /**< 横滚角（0.1度） */
    bool valid;
    uint32_t timestamp_ms;
} Attitude_t;

#define PITCH_MIN       (-900)  /* -90度 */
#define PITCH_MAX       900     /* 90度 */
#define ROLL_MIN        (-1800) /* -180度 */
#define ROLL_MAX        1800    /* 180度 */

/* ============================================================================
 * 控制面指令类型
 * ============================================================================ */

/**
 * @brief 控制面位置指令
 * @range [-100, 100] 百分比
 */
typedef struct {
    int8_t position_percent;    /**< 位置指令（%） */
    bool engaged;               /**< 作动器使能 */
    uint8_t actuator_id;        /**< 作动器标识 */
    uint32_t command_id;        /**< 指令序列号（用于追踪） */
} ControlSurfaceCommand_t;

#define CONTROL_SURFACE_MIN     (-100)
#define CONTROL_SURFACE_MAX     100
#define CONTROL_SURFACE_NEUTRAL 0

/**
 * @brief 飞行模式枚举
 */
typedef enum {
    FLIGHT_MODE_MANUAL = 0,     /**< 手动模式 */
    FLIGHT_MODE_STABILITY,      /**< 增稳模式 */
    FLIGHT_MODE_ALT_HOLD,       /**< 高度保持 */
    FLIGHT_MODE_HDG_HOLD,       /**< 航向保持 */
    FLIGHT_MODE_VNAV,           /**< 垂直导航 */
    FLIGHT_MODE_LNAV,           /**< 水平导航 */
    FLIGHT_MODE_APPROACH,       /**< 进近模式 */
    FLIGHT_MODE_LAND,           /**< 着陆模式 */
    FLIGHT_MODE_MAX             /**< 枚举边界检查 */
} FlightMode_t;

/* ============================================================================
 * 安全监控数据结构
 * ============================================================================ */

/**
 * @brief 通道监控状态
 * DO-178C DAL A 要求多通道交叉监控
 */
typedef struct {
    uint8_t channel_id;         /**< 通道标识 */
    bool healthy;               /**< 健康状态 */
    uint32_t last_update_ms;    /**< 最后更新时间 */
    uint16_t error_count;       /**< 错误计数 */
    uint16_t disagreement_count; /**< 不一致计数 */
} ChannelStatus_t;

#define MAX_CHANNELS    3
#define CHANNEL_A       0
#define CHANNEL_B       1
#define CHANNEL_C       2

/**
 * @brief 系统健康监控
 */
typedef struct {
    ChannelStatus_t channels[MAX_CHANNELS];
    uint32_t active_channel;    /**< 当前主通道 */
    bool system_degraded;       /**< 系统降级标志 */
    bool system_failed;         /**< 系统失效标志 */
    uint32_t monitor_timestamp; /**< 监控时间戳 */
} SystemHealth_t;

/* ============================================================================
 * 数据验证函数声明
 * ============================================================================ */

/**
 * @brief 验证高度值是否在有效范围内
 * @param alt 高度值
 * @return true 如果有效
 */
static inline bool Altitude_IsValid(const Altitude_t *alt) {
    return (alt != NULL) &&
           alt->valid &&
           (alt->value >= ALTITUDE_MIN) &&
           (alt->value <= ALTITUDE_MAX);
}

/**
 * @brief 验证空速值是否在有效范围内
 */
static inline bool Airspeed_IsValid(const Airspeed_t *speed) {
    return (speed != NULL) &&
           speed->valid &&
           (speed->value <= AIRSPEED_MAX);
}

/**
 * @brief 验证航向值是否在有效范围内
 */
static inline bool Heading_IsValid(const Heading_t *hdg) {
    return (hdg != NULL) &&
           hdg->valid &&
           (hdg->value_tenths <= HEADING_MAX);
}

/**
 * @brief 验证飞行模式枚举值
 */
static inline bool FlightMode_IsValid(FlightMode_t mode) {
    return (mode >= FLIGHT_MODE_MANUAL) && (mode < FLIGHT_MODE_MAX);
}

/**
 * @brief 验证控制面指令
 */
static inline bool ControlCommand_IsValid(const ControlSurfaceCommand_t *cmd) {
    return (cmd != NULL) &&
           (cmd->position_percent >= CONTROL_SURFACE_MIN) &&
           (cmd->position_percent <= CONTROL_SURFACE_MAX);
}

/* ============================================================================
 * 数据监控宏（运行时断言）
 * ============================================================================ */

#ifdef FCS_ENABLE_MONITORING
    #define FCS_MONITOR(condition, error_code) \
        do { \
            if (!(condition)) { \
                FCS_ReportError(error_code, __FILE__, __LINE__); \
            } \
        } while(0)
#else
    #define FCS_MONITOR(condition, error_code) ((void)0)
#endif

/* 错误码定义 */
typedef enum {
    FCS_ERR_NONE = 0,
    FCS_ERR_INVALID_ALTITUDE = 0x01,
    FCS_ERR_INVALID_AIRSPEED = 0x02,
    FCS_ERR_INVALID_HEADING = 0x03,
    FCS_ERR_INVALID_ATTITUDE = 0x04,
    FCS_ERR_CHANNEL_DISAGREE = 0x10,
    FCS_ERR_SYSTEM_FAILURE = 0xFF
} FcsErrorCode_t;

#endif /* FLIGHT_CONTROL_TYPES_H */
