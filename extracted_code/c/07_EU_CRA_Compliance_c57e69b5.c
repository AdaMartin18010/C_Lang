/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\07_EU_CRA_Compliance.md
 * Line: 1601
 * Language: c
 * Block ID: c57e69b5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * EU CRA分阶段实施计划
 */

typedef struct {
    const char *phase;
    const char *start_date;
    const char *end_date;
    const char *deliverables[10];
    bool (*completion_check)(void);
} implementation_phase_t;

static implementation_phase_t cra_implementation_plan[] = {
    {
        .phase = "Phase 1: 评估和准备",
        .start_date = "2025-01",
        .end_date = "2025-06",
        .deliverables = {
            "产品范围界定和分类",
            "差距分析（当前vs CRA要求）",
            "风险评估报告",
            "实施路线图",
            "预算和资源计划",
            NULL
        },
    },
    {
        .phase = "Phase 2: 设计和开发",
        .start_date = "2025-07",
        .end_date = "2026-03",
        .deliverables = {
            "安全设计文档",
            "威胁建模",
            "安全编码规范",
            "代码审查流程",
            "安全测试计划",
            NULL
        },
    },
    {
        .phase = "Phase 3: 实施",
        .start_date = "2026-04",
        .end_date = "2026-08",
        .deliverables = {
            "安全功能实现",
            "漏洞处理系统",
            "自动更新机制",
            "SBOM生成工具",
            "技术文档编写",
            NULL
        },
    },
    {
        .phase = "Phase 4: 验证和测试",
        .start_date = "2026-09",
        .end_date = "2026-11",
        .deliverables = {
            "安全测试报告",
            "渗透测试结果",
            "合规性评估报告",
            "第三方评估（如需要）",
            "CE标志准备",
            NULL
        },
    },
    {
        .phase = "Phase 5: 上市和运营",
        .start_date = "2026-12",
        .end_date = "持续",
        .deliverables = {
            "产品上市",
            "持续漏洞监控",
            "安全更新发布",
            "事件响应",
            "合规性维护",
            NULL
        },
    },
};

/* 生成甘特图数据 */
int generate_gantt_chart(const char *output_file) {
    FILE *fp = fopen(output_file, "w");
    if (!fp) return -1;

    fprintf(fp, "Phase,Start,End,Duration\n");

    for (size_t i = 0; i < sizeof(cra_implementation_plan)/sizeof(cra_implementation_plan[0]); i++) {
        implementation_phase_t *phase = &cra_implementation_plan[i];
        fprintf(fp, "%s,%s,%s,\n",
                phase->phase, phase->start_date, phase->end_date);
    }

    fclose(fp);
    return 0;
}
