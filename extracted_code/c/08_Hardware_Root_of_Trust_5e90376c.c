/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 1250
 * Language: c
 * Block ID: 5e90376c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 安全启动ROM设计原则
 */

/* ROM设计原则 */
typedef struct {
    const char *principle;
    const char *rationale;
    const char *implementation;
} rom_design_principle_t;

static rom_design_principle_t rom_principles[] = {
    {
        .principle = "最小化原则",
        .rationale = "代码越少，攻击面越小，验证越容易",
        .implementation = "ROM代码限制在4KB以内，只包含必要的启动功能",
    },
    {
        .principle = "不可变性",
        .rationale = "ROM代码必须是不可修改的，防止被恶意篡改",
        .implementation = "使用芯片掩膜ROM或一次性编程存储器",
    },
    {
        .principle = "自我验证",
        .rationale = "ROM应该能够验证自己的完整性（通过外部验证）",
        .implementation = "ROM哈希存储在eFuse中，允许外部验证",
    },
    {
        .principle = "故障安全",
        .rationale = "任何失败都应该导致安全状态，而非不安全状态",
        .implementation = "验证失败时停止启动或进入恢复模式",
    },
    {
        .principle = "最小依赖",
        .rationale = "ROM应该尽量减少对外部组件的依赖",
        .implementation = "内置基本的加密和验证功能，不依赖外部库",
    },
    {
        .principle = "时序安全",
        .rationale = "防止时序攻击和故障注入",
        .implementation = "使用常量时间算法，添加随机延迟",
    },
    {
        .principle = "可审计性",
        .rationale = "ROM代码应该可以被安全审计",
        .implementation = "提供ROM源码给第三方审计，发布ROM哈希",
    },
};

/* ROM安全目标 */
typedef enum {
    ROM_GOAL_VERIFY_NEXT_STAGE = 0, /* 验证下一阶段 */
    ROM_GOAL_ESTABLISH_TRUST,        /* 建立信任根 */
    ROM_GOAL_PROTECT_KEYS,           /* 保护密钥 */
    ROM_GOAL_DETECT_TAMPERING,       /* 检测篡改 */
    ROM_GOAL_SECURE_RECOVERY,        /* 安全恢复 */
} rom_security_goal_t;
