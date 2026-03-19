/*
 * Auto-generated from: 04_Industrial_Scenarios\10_Deep_Sea\02_Energy_Aware_Scheduling.md
 * Line: 188
 * Language: c
 * Block ID: c2084cb8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 能量感知任务模型
// ============================================================================

// 任务类型
typedef enum {
    TASK_CRITICAL,      // 关键任务 (必须执行)
    TASK_PERIODIC,      // 周期性任务
    TASK_EVENT,         // 事件驱动
    TASK_OPTIONAL       // 可选任务 (能量充足时执行)
} TaskType;

// 任务能量信息
typedef struct {
    float min_energy_mj;        // 最小所需能量
    float max_energy_mj;        // 最大可能消耗
    float average_power_mw;     // 平均功耗
    float exec_time_s;          // 执行时间
} TaskEnergy;

// 任务控制块
typedef struct {
    uint16_t task_id;
    TaskType type;
    uint32_t period_ms;
    uint32_t deadline_ms;
    uint32_t next_release_ms;

    TaskEnergy energy;

    bool is_ready;
    bool is_running;
    uint32_t exec_count;
    uint32_t miss_count;

    void (*task_func)(void);
} EAPTask;

// 能量感知调度器
typedef struct {
    EAPTask tasks[32];
    uint8_t num_tasks;
    EnergyManager *energy_mgr;

    uint32_t current_time_ms;
    float energy_budget_mj;     // 当前周期能量预算
    float energy_consumed_mj;   // 已消耗能量

    // 策略参数
    float critical_reserve_ratio;   // 关键任务预留比例
    float safety_margin;            // 安全裕度
} EAPScheduler;

// 初始化调度器
void eap_init(EAPScheduler *sched, EnergyManager *mgr) {
    memset(sched, 0, sizeof(EAPScheduler));
    sched->energy_mgr = mgr;
    sched->critical_reserve_ratio = 0.3f;
    sched->safety_margin = 1.2f;
}

// 添加任务
int eap_add_task(EAPScheduler *sched, const EAPTask *task) {
    if (sched->num_tasks >= 32) return -1;

    sched->tasks[sched->num_tasks++] = *task;
    return 0;
}

// 计算周期能量预算
void eap_compute_budget(EAPScheduler *sched, uint32_t period_ms) {
    float period_s = period_ms / 1000.0f;

    // 预测可用能量
    float predicted_mj = energy_predict_available(sched->energy_mgr,
                                                   period_s / 3600.0f);

    // 计算关键任务需求
    float critical_demand = 0;
    for (int i = 0; i < sched->num_tasks; i++) {
        EAPTask *t = &sched->tasks[i];
        if (t->type == TASK_CRITICAL) {
            float exec_in_period = period_ms / (float)t->period_ms;
            critical_demand += t->energy.min_energy_mj * exec_in_period;
        }
    }

    // 预留关键任务能量
    float critical_reserve = critical_demand * sched->critical_reserve_ratio;

    // 可用预算
    sched->energy_budget_mj = (predicted_mj - critical_reserve) /
                               sched->safety_margin;
    if (sched->energy_budget_mj < 0) sched->energy_budget_mj = 0;

    sched->energy_consumed_mj = 0;
}

// 选择下一个任务 (EDF + 能量感知)
EAPTask* eap_select_task(EAPScheduler *sched) {
    EAPTask *selected = NULL;
    uint32_t earliest_deadline = 0xFFFFFFFF;

    for (int i = 0; i < sched->num_tasks; i++) {
        EAPTask *t = &sched->tasks[i];

        if (!t->is_ready) continue;

        // 检查能量是否足够
        float remaining_budget = sched->energy_budget_mj - sched->energy_consumed_mj;

        if (t->type == TASK_OPTIONAL &&
            t->energy.max_energy_mj > remaining_budget) {
            continue;  // 跳过可选任务
        }

        if (t->type == TASK_CRITICAL) {
            // 关键任务优先
            return t;
        }

        // EDF
        if (t->deadline_ms < earliest_deadline) {
            earliest_deadline = t->deadline_ms;
            selected = t;
        }
    }

    return selected;
}

// 调度器主循环
void eap_schedule(EAPScheduler *sched, uint32_t dt_ms) {
    sched->current_time_ms += dt_ms;

    // 1. 检查任务释放
    for (int i = 0; i < sched->num_tasks; i++) {
        EAPTask *t = &sched->tasks[i];
        if (sched->current_time_ms >= t->next_release_ms) {
            t->is_ready = true;
            t->next_release_ms += t->period_ms;
        }
    }

    // 2. 选择并执行任务
    EAPTask *task = eap_select_task(sched);
    if (task) {
        // 执行任务
        task->is_running = true;
        task->task_func();
        task->is_running = false;
        task->is_ready = false;
        task->exec_count++;

        // 更新能耗
        sched->energy_consumed_mj += task->energy.average_power_mw *
                                      task->energy.exec_time_s / 1000.0f;

        // 检查截止时间
        if (sched->current_time_ms > task->deadline_ms) {
            task->miss_count++;
        }
    }
}
