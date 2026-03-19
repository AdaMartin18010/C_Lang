/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\07_EU_CRA_Compliance.md
 * Line: 1706
 * Language: c
 * Block ID: ef3cafb1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * EU CRA vs CISA安全要求对比分析
 */

typedef struct {
    const char *requirement_area;
    const char *eu_cra_reference;
    const char *cisa_reference;
    int cra_level;      /* 1=必须, 2=重要, 3=建议 */
    int cisa_level;     /* 1=必须, 2=建议 */
    bool overlap;
    const char *notes;
} requirement_comparison_t;

static requirement_comparison_t cra_cisa_comparison[] = {
    {
        .requirement_area = "漏洞披露政策",
        .eu_cra_reference = "Article 14",
        .cisa_reference = "CISA BOD 20-01",
        .cra_level = 1,
        .cisa_level = 1,
        .overlap = true,
        .notes = "两者都要求公开漏洞披露渠道",
    },
    {
        .requirement_area = "安全更新",
        .eu_cra_reference = "Article 13(3)",
        .cisa_reference = "CISA BOD 19-02",
        .cra_level = 1,
        .cisa_level = 1,
        .overlap = true,
        .notes = "都要求及时修复漏洞",
    },
    {
        .requirement_area = "SBOM",
        .eu_cra_reference = "Article 13(5)",
        .cisa_reference = "CISA SBOM Guidance",
        .cra_level = 2,
        .cisa_level = 2,
        .overlap = true,
        .notes = "欧盟要求重要产品提供SBOM",
    },
    {
        .requirement_area = "漏洞报告时限",
        .eu_cra_reference = "Article 14(1): 90天",
        .cisa_reference = "无明确时限",
        .cra_level = 1,
        .cisa_level = 0,
        .overlap = false,
        .notes = "CRA对重要产品有明确的90天报告要求",
    },
    {
        .requirement_area = "安全更新支持期",
        .eu_cra_reference = "Article 13(8): 明确定义",
        .cisa_reference = "无具体要求",
        .cra_level = 1,
        .cisa_level = 0,
        .overlap = false,
        .notes = "CRA要求明确定义并公布支持期",
    },
    {
        .requirement_area = "产品安全认证",
        .eu_cra_reference = "Article 24: CE标志",
        .cisa_reference = "无认证要求",
        .cra_level = 1,
        .cisa_level = 0,
        .overlap = false,
        .notes = "CRA对重要产品要求第三方认证",
    },
    {
        .requirement_area = "供应链安全",
        .eu_cra_reference = "Article 23",
        .cisa_reference = "CISA Supply Chain Guidance",
        .cra_level = 2,
        .cisa_level = 2,
        .overlap = true,
        .notes = "两者都强调供应链安全",
    },
};

/* 生成对比报告 */
int generate_comparison_report(const char *output_file) {
    FILE *fp = fopen(output_file, "w");
    if (!fp) return -1;

    fprintf(fp, "# EU CRA vs CISA Requirements Comparison\n\n");

    fprintf(fp, "| Requirement Area | EU CRA | CISA | Overlap | Notes |\n");
    fprintf(fp, "|-----------------|--------|------|---------|-------|\n");

    for (size_t i = 0; i < sizeof(cra_cisa_comparison)/sizeof(cra_cisa_comparison[0]); i++) {
        requirement_comparison_t *comp = &cra_cisa_comparison[i];
        fprintf(fp, "| %s | %s | %s | %s | %s |\n",
                comp->requirement_area,
                comp->cra_level == 1 ? "Required" :
                comp->cra_level == 2 ? "Important" : "Recommended",
                comp->cisa_level == 1 ? "Required" :
                comp->cisa_level == 2 ? "Recommended" : "N/A",
                comp->overlap ? "Yes" : "No",
                comp->notes);
    }

    fclose(fp);
    return 0;
}
