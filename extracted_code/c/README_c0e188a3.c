/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\README.md
 * Line: 38
 * Language: c
 * Block ID: c0e188a3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 辐射效应类型定义
typedef enum {
    RADIATION_TID,       // 总电离剂量 (Total Ionizing Dose)
    RADIATION_SEE,       // 单粒子效应 (Single Event Effects)
    RADIATION_DDD,       // 位移损伤剂量 (Displacement Damage Dose)
    RADIATION_PROMPT     // 瞬时剂量 (Prompt Dose)
} RadiationType;

// 单粒子效应细分
typedef enum {
    SEE_SEU,             // 单粒子翻转 (Soft Error)
    SEE_SEL,             // 单粒子闩锁 (Single Event Latchup)
    SEE_SEB,             // 单粒子烧毁 (Single Event Burnout)
    SEE_SET,             // 单粒子瞬态 (Single Event Transient)
    SEE_SEGR             // 单粒子栅穿 (Single Event Gate Rupture)
} SingleEventEffect;

// 辐射环境监测数据结构
typedef struct {
    float tid_accumulated;       // 累积 TID (krad)
    uint32_t seu_count;          // SEU 计数
    uint32_t sel_events;         // SEL 事件数
    uint64_t timestamp;          // 时间戳
    float temperature;           // 当前温度
} RadiationMonitorData;
