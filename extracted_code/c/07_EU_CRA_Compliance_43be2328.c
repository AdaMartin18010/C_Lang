/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\07_EU_CRA_Compliance.md
 * Line: 1857
 * Language: c
 * Block ID: 43be2328
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 双重合规策略建议
 * 同时满足EU CRA和CISA要求
 */

typedef struct {
    const char *strategy;
    const char *eu_cra_benefit;
    const char *cisa_benefit;
    int implementation_effort;  /* 1-5 */
} dual_compliance_strategy_t;

static dual_compliance_strategy_t dual_strategies[] = {
    {
        .strategy = "以EU CRA为基准，CISA为补充",
        .eu_cra_benefit = "完全合规",
        .cisa_benefit = "基本满足",
        .implementation_effort = 3,
    },
    {
        .strategy = "建立通用安全框架",
        .eu_cra_benefit = "完全合规",
        .cisa_benefit = "完全合规",
        .implementation_effort = 4,
    },
    {
        .strategy = "分地区实施",
        .eu_cra_benefit = "完全合规（仅欧盟）",
        .cisa_benefit = "完全合规（仅美国）",
        .implementation_effort = 2,
    },
};

/* 推荐的统一实施计划 */
void recommend_unified_approach(void) {
    printf("=== 推荐的双重合规策略 ===\n\n");

    printf("1. 安全设计\n");
    printf("   - 实施安全开发生命周期（满足两者）\n");
    printf("   - 遵循IEC 62443-4-2（国际认可）\n");
    printf("   - 实施威胁建模\n\n");

    printf("2. 漏洞管理\n");
    printf("   - 建立90天内响应流程（满足CRA，超出CISA）\n");
    printf("   - 公开漏洞披露渠道（满足两者）\n");
    printf("   - 向ENISA报告重要漏洞（CRA要求）\n\n");

    printf("3. 文档和SBOM\n");
    printf("   - 生成SPDX格式SBOM（两者都支持）\n");
    printf("   - 准备多语言技术文档\n");
    printf("   - 明确定义支持期（CRA要求）\n\n");

    printf("4. 认证和标志\n");
    printf("   - 获取CE标志（CRA要求）\n");
    printf("   - 考虑额外的CISA评估（可选）\n\n");
}
