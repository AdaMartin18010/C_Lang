/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\04_Zero_Trust_Embedded_Architecture.md
 * Line: 520
 * Language: c
 * Block ID: 4eb3b042
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 运行时完整性监控
 * 检测代码/数据被篡改
 */

typedef struct {
    uint64_t text_hash;      // .text段哈希
    uint64_t rodata_hash;    // .rodata段哈希
    uint64_t timestamp;      // 测量时间戳
    uint32_t pid;            // 进程ID
} integrity_measurement_t;

/* IMA (Integrity Measurement Architecture) 扩展 */
int continuous_integrity_check(void) {
    /* 1. 定期重新测量关键代码段 */
    integrity_measurement_t current, expected;

    /* 读取TPM中存储的预期值 */
    tpm_nv_read(TPM2_NV_INTEGRITY_INDEX, &expected, sizeof(expected));

    while (1) {
        sleep(60);  // 每分钟检查一次

        /* 计算当前.text段哈希 */
        current.text_hash = hash_memory_region(
            (void*)_stext, (size_t)(_etext - _stext)
        );

        /* 与预期值比较 */
        if (current.text_hash != expected.text_hash) {
            /* 检测到篡改！ */
            trigger_security_alert(ALERT_CODE_MODIFICATION);

            /* 可选：进入安全模式或重启 */
            enter_safe_mode();
        }

        /* 上报遥测 */
        report_telemetry(&current);
    }
}
