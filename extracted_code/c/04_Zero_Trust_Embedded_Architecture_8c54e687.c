/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\04_Zero_Trust_Embedded_Architecture.md
 * Line: 966
 * Language: c
 * Block ID: 8c54e687
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 自动威胁响应系统
 */

typedef enum {
    RESPONSE_NONE = 0,
    RESPONSE_LOG,           // 仅记录
    RESPONSE_ALERT,         // 发送告警
    RESPONSE_ISOLATE,       // 网络隔离
    RESPONSE_QUARANTINE,    // 功能限制
    RESPONSE_SELF_DESTRUCT, // 数据销毁（极端情况）
} threat_response_t;

/* 威胁响应决策矩阵 */
struct threat_response_rule {
    threat_type_t type;
    threat_severity_t severity;
    threat_response_t response;
    uint32_t cooldown_ms;
};

struct threat_response_rule response_rules[] = {
    {THREAT_PHYSICAL_TAMPER,    SEVERITY_CRITICAL,  RESPONSE_SELF_DESTRUCT, 0},
    {THREAT_FIRMWARE_MODIFIED,  SEVERITY_CRITICAL,  RESPONSE_QUARANTINE,    0},
    {THREAT_UNAUTHORIZED_ACCESS, SEVERITY_HIGH,     RESPONSE_ISOLATE,       5000},
    {THREAT_NETWORK_ANOMALY,    SEVERITY_MEDIUM,    RESPONSE_ALERT,         60000},
    {THREAT_POLICY_VIOLATION,   SEVERITY_LOW,       RESPONSE_LOG,           300000},
};

void execute_response(threat_response_t response, const threat_event_t *event) {
    switch (response) {
        case RESPONSE_ISOLATE:
            /* 切断所有外部连接，只允许管理通道 */
            block_all_outbound();
            enable_emergency_channel();
            report_to_cloud(event);
            break;

        case RESPONSE_QUARANTINE:
            /* 限制功能模式 */
            disable_non_critical_services();
            switch_to_readonly_mode();
            notify_user("Security mode activated");
            break;

        case RESPONSE_SELF_DESTRUCT:
            /* 安全擦除敏感数据 */
            secure_erase_keys();
            secure_erase_credentials();
            factory_reset();  // 恢复出厂设置
            break;

        default:
            break;
    }
}
