/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\03_Functional_Safety.md
 * Line: 181
 * Language: c
 * Block ID: f137ede4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file safety_monitor.c
 * @brief ISO 26262 ASIL-D级别安全监控器实现
 * @version 1.0.0
 * @warning 此模块为安全关键代码，修改需经过安全评审
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* MISRA C:2012 合规类型定义 */
typedef uint32_t SafetyStatus_t;
typedef uint32_t ErrorCode_t;

/* 安全状态定义 */
#define SAFETY_STATE_INIT       (0x01U)
#define SAFETY_STATE_OPERATIONAL (0x02U)
#define SAFETY_STATE_SAFE_STATE  (0x04U)
#define SAFETY_STATE_ERROR       (0x08U)

/* 错误代码定义 */
#define ERROR_NONE              (0x00000000U)
#define ERROR_WATCHDOG_TIMEOUT  (0x00000001U)
#define ERROR_MEMORY_CORRUPTION (0x00000002U)
#define ERROR_TIMING_VIOLATION  (0x00000004U)
#define ERROR_PLAUSIBILITY_FAIL (0x00000008U)

/* 最大监控通道数 */
#define MAX_MONITOR_CHANNELS    (8U)

/* 安全检查周期(ms) */
#define SAFETY_CHECK_PERIOD_MS  (10U)

/* 最大允许连续错误次数 */
#define MAX_CONSECUTIVE_ERRORS  (3U)

/**
 * @brief 监控通道配置结构体
 */
typedef struct {
    uint32_t channel_id;
    uint32_t timeout_ms;
    uint32_t expected_signature;
    bool is_active;
} MonitorChannelConfig_t;

/**
 * @brief 监控通道运行时状态
 */
typedef struct {
    uint32_t last_timestamp;
    uint32_t consecutive_errors;
    uint32_t received_signature;
    bool is_healthy;
} MonitorChannelState_t;

/**
 * @brief 安全监控器上下文
 */
typedef struct {
    SafetyStatus_t current_state;
    ErrorCode_t active_errors;
    MonitorChannelConfig_t configs[MAX_MONITOR_CHANNELS];
    MonitorChannelState_t states[MAX_MONITOR_CHANNELS];
    uint32_t initialized_channels;
    uint32_t system_tick;
    bool safety_critical_error;
} SafetyMonitorContext_t;

/* 静态实例 - 单例模式 */
static SafetyMonitorContext_t g_safety_monitor;

/**
 * @brief 初始化安全监控器
 * @return true: 成功, false: 失败
 */
bool SafetyMonitor_Init(void) {
    (void)memset(&g_safety_monitor, 0, sizeof(g_safety_monitor));
    g_safety_monitor.current_state = SAFETY_STATE_INIT;
    g_safety_monitor.initialized_channels = 0U;
    return true;
}

/**
 * @brief 注册监控通道
 * @param config 通道配置
 * @return 通道索引或错误码
 */
int32_t SafetyMonitor_RegisterChannel(const MonitorChannelConfig_t *config) {
    int32_t channel_idx = -1;

    if (NULL == config) {
        return -1;
    }

    if (g_safety_monitor.initialized_channels >= MAX_MONITOR_CHANNELS) {
        return -2;
    }

    channel_idx = (int32_t)g_safety_monitor.initialized_channels;

    /* 复制配置 */
    g_safety_monitor.configs[channel_idx] = *config;
    g_safety_monitor.states[channel_idx].is_healthy = true;
    g_safety_monitor.states[channel_idx].consecutive_errors = 0U;
    g_safety_monitor.states[channel_idx].last_timestamp = g_safety_monitor.system_tick;

    g_safety_monitor.initialized_channels++;

    return channel_idx;
}

/**
 * @brief 通道健康检查
 * @param channel_idx 通道索引
 * @return true: 健康, false: 异常
 */
static bool check_channel_health(uint32_t channel_idx) {
    const MonitorChannelConfig_t *config;
    MonitorChannelState_t *state;
    uint32_t time_diff;

    if (channel_idx >= g_safety_monitor.initialized_channels) {
        return false;
    }

    config = &g_safety_monitor.configs[channel_idx];
    state = &g_safety_monitor.states[channel_idx];

    if (!config->is_active) {
        return true;  /* 非活动通道跳过检查 */
    }

    /* 检查超时 */
    time_diff = g_safety_monitor.system_tick - state->last_timestamp;
    if (time_diff > config->timeout_ms) {
        state->consecutive_errors++;
        if (state->consecutive_errors >= MAX_CONSECUTIVE_ERRORS) {
            state->is_healthy = false;
            g_safety_monitor.active_errors |= ERROR_WATCHDOG_TIMEOUT;
            return false;
        }
    }

    /* 检查签名 */
    if (state->received_signature != config->expected_signature) {
        state->consecutive_errors++;
        if (state->consecutive_errors >= MAX_CONSECUTIVE_ERRORS) {
            state->is_healthy = false;
            g_safety_monitor.active_errors |= ERROR_MEMORY_CORRUPTION;
            return false;
        }
    }

    /* 健康则重置错误计数 */
    state->consecutive_errors = 0U;
    return true;
}

/**
 * @brief 执行安全检查
 */
void SafetyMonitor_RunCheck(void) {
    uint32_t i;
    bool all_channels_healthy = true;

    g_safety_monitor.system_tick += SAFETY_CHECK_PERIOD_MS;

    /* 检查所有通道 */
    for (i = 0U; i < g_safety_monitor.initialized_channels; i++) {
        if (!check_channel_health(i)) {
            all_channels_healthy = false;
        }
    }

    /* 状态机处理 */
    switch (g_safety_monitor.current_state) {
        case SAFETY_STATE_INIT:
            if (all_channels_healthy) {
                g_safety_monitor.current_state = SAFETY_STATE_OPERATIONAL;
            }
            break;

        case SAFETY_STATE_OPERATIONAL:
            if (!all_channels_healthy) {
                g_safety_monitor.current_state = SAFETY_STATE_SAFE_STATE;
                g_safety_monitor.safety_critical_error = true;
            }
            break;

        case SAFETY_STATE_SAFE_STATE:
            /* 保持在安全状态直到复位 */
            break;

        case SAFETY_STATE_ERROR:
            /* 错误处理 */
            break;

        default:
            /* 非法状态 -> 安全状态 */
            g_safety_monitor.current_state = SAFETY_STATE_SAFE_STATE;
            break;
    }
}

/**
 * @brief 获取当前安全状态
 */
SafetyStatus_t SafetyMonitor_GetState(void) {
    return g_safety_monitor.current_state;
}

/**
 * @brief 报告通道活动(由被监控模块调用)
 */
void SafetyMonitor_ReportActivity(uint32_t channel_id, uint32_t signature) {
    uint32_t i;

    for (i = 0U; i < g_safety_monitor.initialized_channels; i++) {
        if (g_safety_monitor.configs[i].channel_id == channel_id) {
            g_safety_monitor.states[i].last_timestamp = g_safety_monitor.system_tick;
            g_safety_monitor.states[i].received_signature = signature;
            break;
        }
    }
}

/**
 * @brief 进入安全状态回调
 * @note 必须由应用层实现
 */
__attribute__((weak)) void SafetyMonitor_EnterSafeState(void) {
    /* 默认实现: 关闭所有输出 */
    /* 应用层应覆盖此函数实现具体的故障安全行为 */
}
