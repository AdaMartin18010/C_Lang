/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\09_Side_Channel_Protection.md
 * Line: 267
 * Language: c
 * Block ID: 6e9b29ef
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 侧信道抗性评估标准
 */

typedef enum {
    RESISTANCE_NONE = 0,        /* 无防护 */
    RESISTANCE_LOW,             /* 基本防护 */
    RESISTANCE_MEDIUM,          /* 中等防护 */
    RESISTANCE_HIGH,            /* 高级防护 */
    RESISTANCE_MAXIMUM,         /* 最大防护 */
} resistance_level_t;

typedef struct {
    const char *technique;
    resistance_level_t spa_resistance;
    resistance_level_t dpa_resistance;
    resistance_level_t timing_resistance;
    resistance_level_t fault_resistance;
    uint32_t performance_overhead;
    uint32_t code_size_overhead;
    uint32_t memory_overhead;
} protection_technique_t;

static protection_technique_t protection_techniques[] = {
    {
        .technique = "常量时间实现",
        .spa_resistance = RESISTANCE_HIGH,
        .dpa_resistance = RESISTANCE_NONE,
        .timing_resistance = RESISTANCE_MAXIMUM,
        .fault_resistance = RESISTANCE_LOW,
        .performance_overhead = 10,
        .code_size_overhead = 5,
        .memory_overhead = 0,
    },
    {
        .technique = "布尔掩码",
        .spa_resistance = RESISTANCE_MEDIUM,
        .dpa_resistance = RESISTANCE_HIGH,
        .timing_resistance = RESISTANCE_LOW,
        .fault_resistance = RESISTANCE_LOW,
        .performance_overhead = 300,
        .code_size_overhead = 200,
        .memory_overhead = 100,
    },
    {
        .technique = "高阶掩码",
        .spa_resistance = RESISTANCE_HIGH,
        .dpa_resistance = RESISTANCE_MAXIMUM,
        .timing_resistance = RESISTANCE_MEDIUM,
        .fault_resistance = RESISTANCE_MEDIUM,
        .performance_overhead = 1000,
        .code_size_overhead = 500,
        .memory_overhead = 400,
    },
    {
        .technique = "随机延迟",
        .spa_resistance = RESISTANCE_LOW,
        .dpa_resistance = RESISTANCE_MEDIUM,
        .timing_resistance = RESISTANCE_HIGH,
        .fault_resistance = RESISTANCE_LOW,
        .performance_overhead = 50,
        .code_size_overhead = 10,
        .memory_overhead = 0,
    },
    {
        .technique = "双重执行+比较",
        .spa_resistance = RESISTANCE_MEDIUM,
        .dpa_resistance = RESISTANCE_LOW,
        .timing_resistance = RESISTANCE_MEDIUM,
        .fault_resistance = RESISTANCE_HIGH,
        .performance_overhead = 100,
        .code_size_overhead = 50,
        .memory_overhead = 0,
    },
};
