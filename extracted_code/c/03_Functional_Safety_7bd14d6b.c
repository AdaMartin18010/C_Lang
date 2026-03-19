/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\03_Functional_Safety.md
 * Line: 422
 * Language: c
 * Block ID: 7bd14d6b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file independent_watchdog.c
 * @brief ASIL-D级别独立看门狗实现
 * @note 使用独立时钟源，主CPU故障时仍可工作
 */

#include <stdint.h>
#include <stdbool.h>

/* 看门狗寄存器定义 (示例，实际依赖硬件平台) */
#define WDOG_BASE_ADDR          (0x40003000U)
#define WDOG_CR_OFFSET          (0x00U)  /* 控制寄存器 */
#define WDOG_SR_OFFSET          (0x04U)  /* 状态寄存器 */
#define WDOG_RLR_OFFSET         (0x08U)  /* 重载寄存器 */

/* 关键字定义 */
#define WDOG_ENABLE_KEY         (0xCCCCU)
#define WDOG_REFRESH_KEY        (0xAAAAU)
#define WDOG_ACCESS_KEY         (0x5555U)

/* 默认超时时间: 100ms */
#define WDOG_DEFAULT_TIMEOUT_MS (100U)

/* 窗口看门狗阈值 */
#define WDOG_WINDOW_MIN_MS      (20U)
#define WDOG_WINDOW_MAX_MS      (80U)

/**
 * @brief 看门狗配置
 */
typedef struct {
    uint32_t timeout_ms;
    uint32_t window_min_ms;
    uint32_t window_max_ms;
    bool window_mode_enabled;
} WatchdogConfig_t;

/**
 * @brief 看门狗实例
 */
typedef struct {
    WatchdogConfig_t config;
    uint32_t refresh_count;
    uint32_t last_refresh_tick;
    bool is_initialized;
} WatchdogInstance_t;

static WatchdogInstance_t g_watchdog;

/* 硬件访问宏 */
#define WDOG_REG(offset)        (*(volatile uint32_t *)(WDOG_BASE_ADDR + (offset)))

/**
 * @brief 初始化独立看门狗
 */
bool Watchdog_Init(const WatchdogConfig_t *config) {
    if (NULL == config) {
        return false;
    }

    /* 参数范围检查 (防御性编程) */
    if ((config->timeout_ms < 10U) || (config->timeout_ms > 10000U)) {
        return false;
    }

    g_watchdog.config = *config;
    g_watchdog.refresh_count = 0U;
    g_watchdog.is_initialized = true;

    /* 配置重载值 */
    WDOG_REG(WDOG_RLR_OFFSET) = calculate_reload_value(config->timeout_ms);

    /* 使能看门狗 */
    WDOG_REG(WDOG_CR_OFFSET) = WDOG_ENABLE_KEY;

    return true;
}

/**
 * @brief 喂狗操作
 * @return true: 成功, false: 违反窗口限制
 */
bool Watchdog_Refresh(uint32_t current_tick) {
    uint32_t time_since_last;

    if (!g_watchdog.is_initialized) {
        return false;
    }

    /* 窗口模式检查 */
    if (g_watchdog.config.window_mode_enabled) {
        time_since_last = current_tick - g_watchdog.last_refresh_tick;

        if ((time_since_last < g_watchdog.config.window_min_ms) ||
            (time_since_last > g_watchdog.config.window_max_ms)) {
            /* 违反窗口限制 - 将导致复位 */
            return false;
        }
    }

    /* 执行喂狗 */
    WDOG_REG(WDOG_CR_OFFSET) = WDOG_REFRESH_KEY;

    g_watchdog.refresh_count++;
    g_watchdog.last_refresh_tick = current_tick;

    return true;
}

/**
 * @brief 计算重载值
 * @note 根据LSI时钟频率计算
 */
static uint32_t calculate_reload_value(uint32_t timeout_ms) {
    const uint32_t LSI_FREQUENCY_HZ = 32000U;  /* 典型LSI频率 */
    const uint32_t PRESCALER = 64U;

    uint32_t ticks;

    /* 防止溢出 */
    if (timeout_ms > 0xFFFFFFFFU / (LSI_FREQUENCY_HZ / 1000U)) {
        return 0x0FFFU;  /* 最大值 */
    }

    ticks = (timeout_ms * (LSI_FREQUENCY_HZ / 1000U)) / PRESCALER;

    /* 限制最大值 */
    if (ticks > 0x0FFFU) {
        ticks = 0x0FFFU;
    }

    return ticks;
}

/**
 * @brief 强制系统复位
 */
void Watchdog_ForceReset(void) {
    /* 写入非法值触发立即复位 */
    WDOG_REG(WDOG_CR_OFFSET) = 0x0000U;
}

/**
 * @brief 获取看门狗统计信息
 */
void Watchdog_GetStats(uint32_t *refresh_count) {
    if (NULL != refresh_count) {
        *refresh_count = g_watchdog.refresh_count;
    }
}
