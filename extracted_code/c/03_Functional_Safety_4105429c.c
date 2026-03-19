/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\03_Functional_Safety.md
 * Line: 742
 * Language: c
 * Block ID: 4105429c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 安全监控器使用示例
 */
void SafetyMonitor_Example(void) {
    MonitorChannelConfig_t config;
    int32_t channel_id;

    /* 初始化 */
    SafetyMonitor_Init();

    /* 配置监控通道 */
    config.channel_id = 0x01U;
    config.timeout_ms = 50U;
    config.expected_signature = 0xAABBCCDDU;
    config.is_active = true;

    channel_id = SafetyMonitor_RegisterChannel(&config);

    /* 主循环 */
    while (1) {
        /* 执行任务 */

        /* 报告活动 */
        SafetyMonitor_ReportActivity(0x01U, 0xAABBCCDDU);

        /* 检查安全状态 */
        if (SafetyMonitor_GetState() == SAFETY_STATE_SAFE_STATE) {
            /* 执行安全响应 */
            break;
        }
    }
}
