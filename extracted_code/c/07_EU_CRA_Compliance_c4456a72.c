/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\07_EU_CRA_Compliance.md
 * Line: 552
 * Language: c
 * Block ID: c4456a72
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * EU CRA漏洞处理要求实现
 */

/* 漏洞严重级别 */
typedef enum {
    VULN_CRITICAL = 0,      /* 无需用户交互即可利用 */
    VULN_HIGH,              /* 影响机密性/完整性/可用性 */
    VULN_MEDIUM,            /* 有限影响 */
    VULN_LOW,               /* 最小影响 */
} vulnerability_severity_t;

/* 漏洞信息结构 */
typedef struct {
    char vuln_id[32];               /* CVE-ID或内部ID */
    char description[1024];
    vulnerability_severity_t severity;
    char affected_versions[256];
    char fixed_version[32];
    time_t discovery_date;
    time_t report_date;
    time_t patch_date;
    bool is_exploited;
    bool is_public;
    char mitigation[512];
} vulnerability_info_t;

/* 漏洞数据库 */
typedef struct {
    vulnerability_info_t *vulns;
    size_t count;
    size_t capacity;
} vulnerability_db_t;

/* 漏洞处理政策 */
typedef struct {
    uint32_t critical_response_hours;   /* 关键漏洞响应时间 */
    uint32_t high_response_hours;       /* 高危漏洞响应时间 */
    uint32_t medium_response_hours;     /* 中危漏洞响应时间 */
    uint32_t low_response_hours;        /* 低危漏洞响应时间 */
    bool auto_patch_critical;           /* 关键漏洞自动补丁 */
    bool require_user_consent;          /* 需要用户同意 */
} vulnerability_policy_t;

/* 默认漏洞处理政策（CRA合规） */
static const vulnerability_policy_t cra_default_policy = {
    .critical_response_hours = 24 * 90,     /* 90天内 */
    .high_response_hours = 24 * 90,
    .medium_response_hours = 24 * 180,      /* 180天内 */
    .low_response_hours = 24 * 365,         /* 1年内 */
    .auto_patch_critical = false,           /* 默认不自动（用户选择） */
    .require_user_consent = true,
};

/* 漏洞报告渠道 */
typedef struct {
    const char *channel_type;
    const char *contact;
    bool encrypted;
    uint32_t response_time_hours;
} vulnerability_channel_t;

static vulnerability_channel_t vuln_channels[] = {
    {
        .channel_type = "Email",
        .contact = "security@example.com",
        .encrypted = true,  /* 使用PGP */
        .response_time_hours = 48,
    },
    {
        .channel_type = "Web Form",
        .contact = "https://example.com/security/report",
        .encrypted = true,  /* HTTPS */
        .response_time_hours = 48,
    },
    {
        .channel_type = "Bug Bounty",
        .contact = "https://hackerone.com/example",
        .encrypted = true,
        .response_time_hours = 24,
    },
};

/* 漏洞报告接收处理 */
int receive_vulnerability_report(const char *reporter_email,
                                  const char *description,
                                  const char *affected_product,
                                  const char *affected_version) {
    /* 1. 确认收到报告 */
    send_acknowledgment(reporter_email);

    /* 2. 创建内部跟踪ID */
    char internal_id[32];
    generate_internal_id(internal_id, sizeof(internal_id));

    /* 3. 记录漏洞信息 */
    vulnerability_info_t vuln = {0};
    strncpy(vuln.vuln_id, internal_id, sizeof(vuln.vuln_id));
    strncpy(vuln.description, description, sizeof(vuln.description));
    vuln.discovery_date = time(NULL);
    strncpy(vuln.affected_versions, affected_version,
            sizeof(vuln.affected_versions));

    /* 4. 启动评估流程 */
    initiate_triage(internal_id, description, affected_product);

    /* 5. 通知安全团队 */
    notify_security_team(internal_id, description);

    /* 6. 启动计时器（CRA要求的时间限制） */
    start_response_timer(internal_id, cra_default_policy.high_response_hours);

    return 0;
}

/* 漏洞评估和分级 */
vulnerability_severity_t assess_vulnerability(const vulnerability_info_t *vuln) {
    severity_score_t score = {0};

    /* CVSS评分 */
    score.cvss = calculate_cvss(vuln->description);

    /* 利用可能性 */
    if (strstr(vuln->description, "remote") ||
        strstr(vuln->description, "unauthenticated")) {
        score.exploitability = 10;
    }

    /* 影响范围 */
    if (strstr(vuln->affected_versions, "all")) {
        score.scope = 10;
    }

    /* 综合评估 */
    double total_score = score.cvss * 0.6 +
                         score.exploitability * 0.3 +
                         score.scope * 0.1;

    if (total_score >= 9.0) return VULN_CRITICAL;
    if (total_score >= 7.0) return VULN_HIGH;
    if (total_score >= 4.0) return VULN_MEDIUM;
    return VULN_LOW;
}

/* ENISA报告 */
int report_to_enisa(const vulnerability_info_t *vuln) {
    /* 仅适用于重要和关键产品 */
    if (product_class < CRA_CLASS_I) {
        return 0;  /* 不需要报告 */
    }

    /* 构建CSIRT报告 */
    char report[4096];
    snprintf(report, sizeof(report),
        "{\n"
        "  \"reporting_entity\": \"%s\",\n"
        "  \"vulnerability_id\": \"%s\",\n"
        "  \"severity\": \"%s\",\n"
        "  \"affected_products\": [\"%s\"],\n"
        "  \"discovery_date\": \"%s\",\n"
        "  \"is_actively_exploited\": %s\n"
        "}\n",
        manufacturer_name,
        vuln->vuln_id,
        severity_to_string(vuln->severity),
        vuln->affected_versions,
        time_to_iso(vuln->discovery_date),
        vuln->is_exploited ? "true" : "false"
    );

    /* 发送到ENISA CSIRT网络 */
    return submit_csirt_report(report);
}
