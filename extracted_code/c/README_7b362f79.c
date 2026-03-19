/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\README.md
 * Line: 303
 * Language: c
 * Block ID: 7b362f79
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* safety/watchdog.c */
#include "safety/watchdog.h"
#include "mcu/mcu_registers.h"

#define WWDG_BASE           0x40002C00
#define WWDG_CR            (*(volatile uint32_t*)(WWDG_BASE + 0x00))
#define WWDG_CFR           (*(volatile uint32_t*)(WWDG_BASE + 0x04))
#define WWDG_SR            (*(volatile uint32_t*)(WWDG_BASE + 0x08))

/* 窗口看门狗配置 */
#define WWDG_COUNTER_MAX    0x7F
#define WWDG_COUNTER_MIN    0x40
#define WWDG_WINDOW_VALUE   0x5F  /* 窗口上限 */

typedef struct {
    uint8_t expected_task_signatures[8];  /* 各任务预期签名 */
    uint8_t task_count;
    uint32_t last_feed_time;
    bool enabled;
} wwdg_manager_t;

static wwdg_manager_t g_wwdg_mgr;

void wwdg_init(void) {
    /* 启用 WWDG 时钟 */
    RCC_APB1ENR |= RCC_APB1ENR_WWDGEN;

    /* 配置预分频器和窗口值 */
    WWDG_CFR = (3 << 7) |  /* 预分频器: /8 */
               WWDG_WINDOW_VALUE;

    /* 启动看门狗 */
    WWDG_CR = WWDG_COUNTER_MAX | 0x80;  /* 使能位 */

    g_wwdg_mgr.enabled = true;
    g_wwdg_mgr.task_count = 0;
}

/* 任务注册其检查点 */
void wwdg_register_task(uint8_t task_id, uint8_t expected_signature) {
    if (task_id < 8) {
        g_wwdg_mgr.expected_task_signatures[task_id] = expected_signature;
        g_wwdg_mgr.task_count++;
    }
}

/* 任务在关键点报告状态 */
bool wwdg_report_checkpoint(uint8_t task_id, uint8_t signature) {
    if (task_id >= 8) return false;

    /* 验证任务签名 */
    if (signature != g_wwdg_mgr.expected_task_signatures[task_id]) {
        /* 签名不匹配 - 任务执行异常 */
        safety_log_error(SAFETY_ERR_TASK_SIGNATURE_MISMATCH, task_id);
        return false;
    }

    /* 标记任务已完成 */
    g_wwdg_mgr.task_checkpoints |= (1 << task_id);

    /* 检查是否所有任务都已完成 */
    if (g_wwdg_mgr.task_checkpoints == ((1 << g_wwdg_mgr.task_count) - 1)) {
        wwdg_feed();
        g_wwdg_mgr.task_checkpoints = 0;  /* 重置检查点 */
    }

    return true;
}

void wwdg_feed(void) {
    /* 必须在窗口期内喂狗 */
    uint8_t current_counter = WWDG_CR & 0x7F;

    if (current_counter > WWDG_WINDOW_VALUE) {
        /* 在窗口期外喂狗 - 过早喂狗 */
        safety_log_error(SAFETY_ERR_WWDG_EARLY_FEED, current_counter);
        return;
    }

    if (current_counter < WWDG_COUNTER_MIN) {
        /* 计数器已低于最小值 - 系统即将复位 */
        safety_log_error(SAFETY_ERR_WWDG_LATE_FEED, current_counter);
        return;
    }

    /* 重新加载计数器 */
    WWDG_CR = WWDG_COUNTER_MAX | 0x80;
    g_wwdg_mgr.last_feed_time = get_system_tick();
}
