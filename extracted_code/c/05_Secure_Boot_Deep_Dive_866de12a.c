/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 1443
 * Language: c
 * Block ID: 866de12a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 运行时动态测量
 * 在系统运行期间持续测量关键组件
 */

/* 运行时测量配置 */
typedef struct {
    const char *name;
    void *start_addr;
    size_t size;
    uint32_t pcr_index;
    uint32_t measure_interval_ms;
} runtime_measure_config_t;

/* 测量任务 */
typedef struct {
    runtime_measure_config_t config;
    uint8_t expected_hash[32];
    uint8_t last_hash[32];
    timer_t timer;
} measure_task_t;

/* 关键组件测量配置 */
static runtime_measure_config_t g_measure_configs[] = {
    {
        .name = "kernel_text",
        .start_addr = (void *)0x80080000,  /* 内核代码段 */
        .size = 0x1000000,                  /* 16MB */
        .pcr_index = PCR_KERNEL_IMAGE,
        .measure_interval_ms = 60000,       /* 每分钟测量一次 */
    },
    {
        .name = "kernel_rodata",
        .start_addr = (void *)0x81080000,  /* 内核只读数据 */
        .size = 0x400000,                   /* 4MB */
        .pcr_index = PCR_CORE_DATA,
        .measure_interval_ms = 60000,
    },
    /* 更多组件... */
};

static measure_task_t g_measure_tasks[ARRAY_SIZE(g_measure_configs)];

/* 初始化运行时测量 */
void runtime_measurement_init(void) {
    for (int i = 0; i < ARRAY_SIZE(g_measure_configs); i++) {
        g_measure_tasks[i].config = g_measure_configs[i];

        /* 计算初始预期哈希 */
        SHA256(g_measure_configs[i].start_addr,
               g_measure_configs[i].size,
               g_measure_tasks[i].expected_hash);

        memcpy(g_measure_tasks[i].last_hash,
               g_measure_tasks[i].expected_hash, 32);

        /* 启动定时器 */
        timer_create(CLOCK_MONOTONIC, NULL, &g_measure_tasks[i].timer);

        struct itimerspec its = {
            .it_value.tv_sec = g_measure_configs[i].measure_interval_ms / 1000,
            .it_interval.tv_sec = g_measure_configs[i].measure_interval_ms / 1000,
        };
        timer_settime(g_measure_tasks[i].timer, 0, &its, NULL);
    }
}

/* 执行测量检查 */
void perform_measurement_check(measure_task_t *task) {
    uint8_t current_hash[32];

    /* 计算当前哈希 */
    SHA256(task->config.start_addr, task->config.size, current_hash);

    /* 与预期值比较 */
    if (memcmp(current_hash, task->expected_hash, 32) != 0) {
        /* 检测到修改！ */
        log_security_event(EVENT_CODE_MODIFIED,
                          (uint32_t)task->config.start_addr);

        /* 触发告警 */
        trigger_integrity_alert(task->config.name,
                               task->expected_hash,
                               current_hash);

        /* 可选：进入安全模式 */
        if (is_strict_mode_enabled()) {
            enter_safe_mode();
        }
    }

    /* 与上次测量比较（检测运行时修改） */
    if (memcmp(current_hash, task->last_hash, 32) != 0) {
        log_security_event(EVENT_RUNTIME_MODIFIED,
                          (uint32_t)task->config.start_addr);
    }

    memcpy(task->last_hash, current_hash, 32);
}
