/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\06_Embedded_System_Design.md
 * Line: 50
 * Language: c
 * Block ID: ae6d4241
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 分层架构
// ┌─────────────────┐
// │   应用层 (ASW)   │
// ├─────────────────┤
// │   运行时 (RTE)   │
// ├─────────────────┤
// │   基础软件 (BSW) │
// ├─────────────────┤
// │   微控制器驱动   │
// └─────────────────┘

// 软件组件
typedef struct {
    uint16_t rpm;
    uint16_t throttle;
    uint8_t temperature;
    bool fault;
} EngineData;

// 端口接口
typedef struct {
    EngineData *data;
    void (*update)(EngineData *out);
    void (*receive)(const EngineData *in);
} EnginePort;
