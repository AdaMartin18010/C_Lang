/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\07_EU_CRA_Compliance.md
 * Line: 140
 * Language: c
 * Block ID: 4fc1a17f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * EU CRA产品分类和合规要求
 */

typedef enum {
    /* 默认类别 */
    CRA_CLASS_DEFAULT = 0,

    /* 重要产品类别 I */
    CRA_CLASS_I,            /* Class I Important Products */

    /* 重要产品类别 II */
    CRA_CLASS_II,           /* Class II Important Products */

    /* 关键产品 */
    CRA_CLASS_CRITICAL,     /* Critical Products */
} cra_product_class_t;

/* 产品分类详细信息 */
typedef struct {
    cra_product_class_t class;
    const char *name;
    const char *description;
    const char **examples;
    int num_examples;
    bool requires_third_party_assessment;
    bool requires_vulnerability_reporting;
    int vulnerability_reporting_hours;
} cra_class_info_t;

/* Class I 重要产品 */
static const char *class_i_examples[] = {
    "身份管理系统",
    "浏览器",
    "密码管理器",
    "VPN产品",
    "操作系统",
    "网络管理系统",
    "智能卡/类似设备",
    "工业自动化控制系统",
    "物联网设备（关键基础设施）",
    NULL
};

/* Class II 重要产品 */
static const char *class_ii_examples[] = {
    "微处理器/微控制器",
    "智能卡微控制器",
    "智能卡读卡器",
    "工业监控和控制系统",
    "智能电表",
    "智能卡/令牌",
    "智能电网产品",
    "智能交通系统",
    "自动驾驶系统",
    NULL
};

/* 关键产品 */
static const char *critical_examples[] = {
    "芯片/处理器（用于关键基础设施）",
    "智能卡微控制器",
    "智能卡读卡器",
    "身份管理系统",
    "信令传输系统",
    "网络管理系统",
    "DNS服务",
    "数字身份钱包",
    NULL
};

static const cra_class_info_t cra_classes[] = {
    {
        .class = CRA_CLASS_DEFAULT,
        .name = "默认类别",
        .description = "所有不属于重要或关键类别的产品",
        .requires_third_party_assessment = false,
        .requires_vulnerability_reporting = true,
        .vulnerability_reporting_hours = 0,  /* 无具体要求 */
    },
    {
        .class = CRA_CLASS_I,
        .name = "重要产品类别 I",
        .description = "提供重要安全功能的产品",
        .examples = class_i_examples,
        .requires_third_party_assessment = true,
        .requires_vulnerability_reporting = true,
        .vulnerability_reporting_hours = 24 * 90,  /* 90天内 */
    },
    {
        .class = CRA_CLASS_II,
        .name = "重要产品类别 II",
        .description = "关键基础设施和工业控制系统",
        .examples = class_ii_examples,
        .requires_third_party_assessment = true,
        .requires_vulnerability_reporting = true,
        .vulnerability_reporting_hours = 24 * 90,
    },
    {
        .class = CRA_CLASS_CRITICAL,
        .name = "关键产品",
        .description = "对国家安全至关重要的产品",
        .examples = critical_examples,
        .requires_third_party_assessment = true,
        .requires_vulnerability_reporting = true,
        .vulnerability_reporting_hours = 24 * 30,  /* 30天内 */
    },
};

/* 确定产品分类 */
cra_product_class_t determine_product_class(const char *product_type,
                                             const char *use_case) {
    /* 检查是否是关键基础设施 */
    if (strstr(use_case, "critical infrastructure") ||
        strstr(use_case, "essential service")) {
        return CRA_CLASS_CRITICAL;
    }

    /* 检查Class II */
    for (int i = 0; class_ii_examples[i] != NULL; i++) {
        if (strstr(product_type, class_ii_examples[i])) {
            return CRA_CLASS_II;
        }
    }

    /* 检查Class I */
    for (int i = 0; class_i_examples[i] != NULL; i++) {
        if (strstr(product_type, class_i_examples[i])) {
            return CRA_CLASS_I;
        }
    }

    return CRA_CLASS_DEFAULT;
}
