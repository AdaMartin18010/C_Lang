/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\07_EU_CRA_Compliance.md
 * Line: 2194
 * Language: c
 * Block ID: 96d445b0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 漏洞管理流程实现
 */

typedef enum {
    VULN_STATUS_RECEIVED = 0,
    VULN_STATUS_TRIAGING,
    VULN_STATUS_ASSESSED,
    VULN_STATUS_PATCHING,
    VULN_STATUS_TESTING,
    VULN_STATUS_RELEASED,
    VULN_STATUS_DISCLOSED,
    VULN_STATUS_CLOSED,
} vulnerability_status_t;

typedef struct {
    char id[32];
    char cve_id[32];
    char title[256];
    char description[2048];
    vulnerability_severity_t severity;
    vulnerability_status_t status;
    time_t received_date;
    time_t assessment_due;
    time_t patch_due;
    time_t disclosure_due;
    char assigned_to[128];
    char notes[4096];
} vulnerability_case_t;

/* 漏洞跟踪系统 */
typedef struct {
    vulnerability_case_t *cases;
    size_t count;
    size_t capacity;
    char tracking_db_path[256];
} vuln_tracking_system_t;

/* 接收漏洞报告 */
int receive_vulnerability(vuln_tracking_system_t *sys,
                          const char *reporter,
                          const char *description,
                          vulnerability_case_t **new_case) {
    /* 1. 分配新案例 */
    if (sys->count >= sys->capacity) {
        return -ENOMEM;
    }

    vulnerability_case_t *case_ptr = &sys->cases[sys->count++];
    memset(case_ptr, 0, sizeof(*case_ptr));

    /* 2. 生成ID */
    snprintf(case_ptr->id, sizeof(case_ptr->id), "VULN-%04zu", sys->count);

    /* 3. 设置初始状态 */
    case_ptr->status = VULN_STATUS_RECEIVED;
    case_ptr->received_date = time(NULL);

    /* 4. 计算截止日期（CRA要求） */
    int hours = cra_default_policy.high_response_hours;
    case_ptr->assessment_due = case_ptr->received_date + (14 * 24 * 3600);  /* 14天评估 */
    case_ptr->patch_due = case_ptr->received_date + (hours * 3600);

    /* 5. 记录描述 */
    strncpy(case_ptr->description, description, sizeof(case_ptr->description));

    /* 6. 发送确认 */
    send_vuln_acknowledgment(reporter, case_ptr->id);

    /* 7. 通知安全团队 */
    notify_security_team_new_vuln(case_ptr);

    *new_case = case_ptr;
    return 0;
}

/* 漏洞评估 */
int assess_vulnerability(vulnerability_case_t *case_ptr,
                         vulnerability_severity_t severity,
                         const char *affected_versions,
                         const char *technical_analysis) {
    case_ptr->status = VULN_STATUS_ASSESSED;
    case_ptr->severity = severity;

    /* 根据严重性调整时间表 */
    switch (severity) {
        case VULN_CRITICAL:
            case_ptr->patch_due = case_ptr->received_date + (30 * 24 * 3600);
            break;
        case VULN_HIGH:
            case_ptr->patch_due = case_ptr->received_date + (90 * 24 * 3600);
            break;
        case VULN_MEDIUM:
            case_ptr->patch_due = case_ptr->received_date + (180 * 24 * 3600);
            break;
        case VULN_LOW:
            case_ptr->patch_due = case_ptr->received_date + (365 * 24 * 3600);
            break;
    }

    /* 向ENISA报告（重要产品）*/
    if (product_class >= CRA_CLASS_I && severity <= VULN_HIGH) {
        report_to_enisa(case_ptr);
    }

    return 0;
}

/* 定期检查逾期漏洞 */
void check_overdue_vulnerabilities(vuln_tracking_system_t *sys) {
    time_t now = time(NULL);

    for (size_t i = 0; i < sys->count; i++) {
        vulnerability_case_t *case_ptr = &sys->cases[i];

        if (case_ptr->status == VULN_STATUS_CLOSED) {
            continue;
        }

        /* 检查是否逾期 */
        if (now > case_ptr->patch_due) {
            /* 发送升级通知 */
            escalate_overdue_vulnerability(case_ptr);

            /* 记录合规风险 */
            log_compliance_risk("VULNERABILITY_PATCH_OVERDUE",
                               case_ptr->id);
        }

        /* 检查是否临近截止日期 */
        else if (now > case_ptr->patch_due - (7 * 24 * 3600)) {
            /* 发送提醒 */
            send_patch_reminder(case_ptr);
        }
    }
}
