/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\07_EU_CRA_Compliance.md
 * Line: 279
 * Language: c
 * Block ID: b26aa3ce
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * EU CRA关键术语定义
 */

typedef struct {
    const char *term;
    const char *definition;
    const char *article_ref;
} cra_definition_t;

static const cra_definition_t cra_definitions[] = {
    {
        .term = "Product with Digital Elements (PDE)",
        .definition = "任何软件或硬件产品及其远程数据处理解决方案，包括单独投放市场的软件",
        .article_ref = "Article 3(1)",
    },
    {
        .term = "Critical Product",
        .definition = "在附件III中列出的对国家安全或公共安全至关重要的产品",
        .article_ref = "Article 3(5)",
    },
    {
        .term = "Important Product",
        .definition = "在附件I和II中列出的提供重要安全功能的产品",
        .article_ref = "Article 3(6)",
    },
    {
        .term = "Vulnerability",
        .definition = "产品中可能被威胁利用以影响其网络安全的弱点",
        .article_ref = "Article 3(8)",
    },
    {
        .term = "Security Update",
        .definition = "旨在纠正产品中漏洞的软件更新",
        .article_ref = "Article 3(9)",
    },
    {
        .term = "Manufacturer",
        .definition = "开发产品或委托设计/制造并以自己名义销售的自然人或法人",
        .article_ref = "Article 3(12)",
    },
    {
        .term = "Support Period",
        .definition = "制造商承诺提供安全更新的时间段",
        .article_ref = "Article 13(8)",
    },
    {
        .term = "Unpatched Vulnerability",
        .definition = "在支持期结束后仍存在的、制造商不再提供补丁的漏洞",
        .article_ref = "Article 13(6)",
    },
};

/* 支持期结构 */
typedef struct {
    uint32_t years;                 /* 支持年限 */
    bool defined_at_purchase;       /* 购买时明确定义 */
    bool extendable;                /* 可延长 */
    uint32_t extension_years;       /* 可延长年限 */
    uint32_t notification_months;   /* 结束前通知月数 */
} support_period_t;

/* 默认支持期要求 */
#define CRA_DEFAULT_SUPPORT_YEARS       5
#define CRA_MIN_NOTIFICATION_MONTHS     6
