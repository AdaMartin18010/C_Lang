/*
 * Auto-generated from: 16_Embedded_AI_Agents\09_Security_Reliability\01_AI_Safety_Standards.md
 * Line: 76
 * Language: c
 * Block ID: a3243a6e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// AI安全生命周期 (基于ISO 26262改编)
typedef enum {
    AI_PHASE_CONCEPT = 0,           // 概念阶段
    AI_PHASE_DATA_COLLECTION,       // 数据收集与验证
    AI_PHASE_MODEL_DEVELOPMENT,     // 模型开发与训练
    AI_PHASE_VERIFICATION,          // 验证与确认
    AI_PHASE_INTEGRATION,           // 系统集成
    AI_PHASE_OPERATION,             // 运行与监控
    AI_PHASE_DECOMMISSIONING        // 退役
} ai_safety_phase_t;

// 安全目标示例 (SG: Safety Goal)
typedef struct {
    uint32_t id;
    char description[256];
    uint8_t asil_level;             // ASIL A-D
    float max_false_negative_rate;  // 最大漏检率
    float max_false_positive_rate;  // 最大误检率
    uint32_t max_response_time_ms;  // 最大响应时间
} ai_safety_goal_t;

// 示例: 行人检测安全目标
ai_safety_goal_t pedestrian_detection_sg = {
    .id = 0x01,
    .description = "Avoid collision with pedestrians",
    .asil_level = ASIL_D,
    .max_false_negative_rate = 0.0001,  // 0.01%漏检率
    .max_false_positive_rate = 0.01,    // 1%误检率 (可接受)
    .max_response_time_ms = 100         // 100ms内响应
};
