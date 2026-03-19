/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\02_Hard_RealTime.md
 * Line: 185
 * Language: c
 * Block ID: 2462b506
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 时间保护 - 防止任务超时影响系统
// 符合ISO 26262时间保护要求
// ============================================================================

// 时间预算控制
typedef struct {
    uint32_t budget_us;         // 分配的时间预算
    uint32_t consumed_us;       // 已消耗时间
    uint32_t started_at_us;     // 开始时间
    bool budget_exhausted;      // 预算耗尽标志
} TimeBudget;

// 时间保护监控器
typedef struct {
    TimeBudget budgets[NUM_TASKS];
    uint32_t monitoring_period_us;
    void (*budget_violation_handler)(uint16_t task_id);
} TimeProtection;

// 硬件定时器接口 (假设1MHz计时)
#define TIMER_FREQ_HZ       1000000u
#define TIMER_TICK_US       1u

static inline uint32_t get_microseconds(void) {
    return read_hardware_timer() / (TIMER_FREQ_HZ / 1000000u);
}

// 启动任务时初始化预算
void time_budget_start(TimeProtection *tp, uint16_t task_id) {
    TimeBudget *budget = &tp->budgets[task_id];
    budget->started_at_us = get_microseconds();
    budget->consumed_us = 0;
    budget->budget_exhausted = false;
}

// 任务执行期间检查预算
bool time_budget_check(TimeProtection *tp, uint16_t task_id) {
    TimeBudget *budget = &tp->budgets[task_id];
    uint32_t now = get_microseconds();

    budget->consumed_us = now - budget->started_at_us;

    if (budget->consumed_us > budget->budget_us) {
        budget->budget_exhausted = true;
        return false;  // 预算耗尽
    }
    return true;  // 预算充足
}

// 预算违规处理
void handle_budget_violation(uint16_t task_id) {
    // 记录故障
    log_fault(FAULT_TASK_OVERRUN, task_id);

    // 根据任务重要性决定处理方式
    if (task_id <= PRIORITY_SAFETY) {
        // 关键任务超时 - 触发安全状态
        enter_safe_state();
    } else {
        // 非关键任务 - 终止并重启
        terminate_task(task_id);
    }
}

// ============================================================================
// 截止时间监控
// ============================================================================

typedef struct {
    uint32_t absolute_deadline;     // 绝对截止时间点
    uint32_t release_time;          // 释放时间
    bool deadline_monitoring;       // 是否启用监控
} DeadlineMonitor;

// 检查截止时间
bool check_deadline(const TaskControlBlock *tcb, const DeadlineMonitor *dm) {
    uint32_t now = get_microseconds() / 1000;  // 转换为ms

    if (now > dm->absolute_deadline) {
        // 截止时间错过
        ((TaskControlBlock*)tcb)->deadline_misses++;

        // 根据错过次数采取行动
        if (tcb->deadline_misses >= 3) {
            log_fault(FAULT_REPEATED_DEADLINE_MISS, tcb->task_id);
        }

        return false;
    }

    return true;
}

// ============================================================================
// 看门狗定时器管理
// ============================================================================

#define WATCHDOG_PERIOD_MS      50      // 看门狗周期
#define MAX_WDOG_MISSES         3       // 最大错过次数

typedef struct {
    uint32_t last_feed_ms;
    uint8_t miss_count;
    bool enabled;
} WatchdogManager;

void watchdog_init(WatchdogManager *wd) {
    wd->last_feed_ms = get_milliseconds();
    wd->miss_count = 0;
    wd->enabled = true;

    // 配置硬件看门狗
    configure_hardware_watchdog(WATCHDOG_PERIOD_MS);
}

void watchdog_feed(WatchdogManager *wd) {
    if (!wd->enabled) return;

    uint32_t now = get_milliseconds();
    uint32_t elapsed = now - wd->last_feed_ms;

    if (elapsed > WATCHDOG_PERIOD_MS) {
        wd->miss_count++;

        if (wd->miss_count >= MAX_WDOG_MISSES) {
            // 触发复位
            trigger_system_reset();
        }
    } else {
        wd->miss_count = 0;
    }

    wd->last_feed_ms = now;
    feed_hardware_watchdog();
}
