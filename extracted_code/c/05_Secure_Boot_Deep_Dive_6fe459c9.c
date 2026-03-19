/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 2281
 * Language: c
 * Block ID: 6fe459c9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 软件信任根实现
 * 运行时可验证的代码完整性
 */

/* 运行时完整性监控 */
typedef struct {
    void *code_start;
    size_t code_size;
    uint8_t expected_hash[32];
    uint32_t check_interval_ms;
    timer_t timer;
} code_integrity_monitor_t;

/* 关键代码段列表 */
static code_integrity_monitor_t g_monitored_sections[] = {
    {
        .code_start = (void *)0x80080000,  /* 内核代码 */
        .code_size = 0x800000,             /* 8MB */
        .check_interval_ms = 5000,         /* 5秒 */
    },
    {
        .code_start = (void *)0x81000000,  /* 关键驱动 */
        .code_size = 0x100000,             /* 1MB */
        .check_interval_ms = 10000,        /* 10秒 */
    },
};

/* 运行时完整性检查 */
void runtime_integrity_check(void *arg) {
    code_integrity_monitor_t *monitor = arg;

    uint8_t current_hash[32];
    SHA256(monitor->code_start, monitor->code_size, current_hash);

    if (memcmp(current_hash, monitor->expected_hash, 32) != 0) {
        /* 完整性破坏！ */
        log_security_event(EVENT_CODE_INTEGRITY_FAIL,
                          (uint32_t)monitor->code_start);

        /* 触发响应 */
        trigger_integrity_violation_response();
    }
}

/* TCB（可信计算基）验证 */
int verify_tcb_integrity(void) {
    /* 验证EL3固件 */
    uint8_t el3_hash[32];
    SHA256((void *)EL3_BASE, EL3_SIZE, el3_hash);
    if (memcmp(el3_hash, expected_el3_hash, 32) != 0) {
        return -1;
    }

    /* 验证S-EL1固件（OP-TEE） */
    uint8_t sel1_hash[32];
    SHA256((void *)SEL1_BASE, SEL1_SIZE, sel1_hash);
    if (memcmp(sel1_hash, expected_sel1_hash, 32) != 0) {
        return -1;
    }

    return 0;
}
