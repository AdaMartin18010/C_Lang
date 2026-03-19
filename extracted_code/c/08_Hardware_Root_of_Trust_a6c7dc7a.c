/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 2428
 * Language: c
 * Block ID: a6c7dc7a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 硬件信任根威胁模型
 */

typedef enum {
    THREAT_PHYSICAL = 0,        /* 物理攻击 */
    THREAT_SIDEC_CHANNEL,       /* 侧信道攻击 */
    THREAT_FAULT_INJECTION,     /* 故障注入 */
    THREAT_LOGICAL,             /* 逻辑攻击 */
    THREAT_SUPPLY_CHAIN,        /* 供应链攻击 */
} threat_category_t;

typedef struct {
    const char *threat_name;
    threat_category_t category;
    const char *description;
    const char *mitigation;
    uint32_t severity;          /* 1-10 */
} threat_entry_t;

static threat_entry_t rot_threats[] = {
    {
        .threat_name = "JTAG/Debug Interface Attack",
        .category = THREAT_PHYSICAL,
        .description = "通过调试接口提取密钥或绕过保护",
        .mitigation = "熔断调试接口、添加认证",
        .severity = 9,
    },
    {
        .threat_name = "Power Analysis Attack",
        .category = THREAT_SIDEC_CHANNEL,
        .description = "分析功耗模式提取密钥",
        .mitigation = "功耗平衡、随机延迟",
        .severity = 8,
    },
    {
        .threat_name = "EM Emanation Analysis",
        .category = THREAT_SIDEC_CHANNEL,
        .description = "分析电磁辐射泄露信息",
        .mitigation = "EM屏蔽、噪声注入",
        .severity = 7,
    },
    {
        .threat_name = "Voltage Glitching",
        .category = THREAT_FAULT_INJECTION,
        .description = "通过电压毛刺跳过安全检查",
        .mitigation = "电压监控、异常检测",
        .severity = 9,
    },
    {
        .threat_name = "Clock Glitching",
        .category = THREAT_FAULT_INJECTION,
        .description = "通过时钟毛刺造成执行错误",
        .mitigation = "时钟监控、内部振荡器",
        .severity = 8,
    },
    {
        .threat_name = "Laser Fault Injection",
        .category = THREAT_FAULT_INJECTION,
        .description = "使用激光诱导故障",
        .mitigation = "金属屏蔽、传感器网格",
        .severity = 10,
    },
    {
        .threat_name = "ROM Readout",
        .category = THREAT_PHYSICAL,
        .description = "物理提取ROM内容",
        .mitigation = "只读存储器、防探测涂层",
        .severity = 7,
    },
    {
        .threat_name = "PUF Modeling Attack",
        .category = THREAT_LOGICAL,
        .description = "通过机器学习建模PUF响应",
        .mitigation = "足够熵的PUF、定期刷新",
        .severity = 6,
    },
};

/* 风险评估 */
void assess_rot_security(void) {
    printf("Hardware RoT Security Assessment:\n\n");

    int total_risk = 0;

    for (size_t i = 0; i < sizeof(rot_threats)/sizeof(rot_threats[0]); i++) {
        threat_entry_t *threat = &rot_threats[i];

        printf("Threat: %s\n", threat->threat_name);
        printf("  Category: %s\n",
               threat->category == THREAT_PHYSICAL ? "Physical" :
               threat->category == THREAT_SIDEC_CHANNEL ? "Side-Channel" :
               threat->category == THREAT_FAULT_INJECTION ? "Fault Injection" :
               threat->category == THREAT_LOGICAL ? "Logical" : "Supply Chain");
        printf("  Severity: %d/10\n", threat->severity);
        printf("  Mitigation: %s\n", threat->mitigation);

        total_risk += threat->severity;
        printf("\n");
    }

    printf("Total Risk Score: %d/%zu\n",
           total_risk, sizeof(rot_threats)/sizeof(rot_threats[0]) * 10);
}
