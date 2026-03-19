/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\07_EU_CRA_Compliance.md
 * Line: 2706
 * Language: c
 * Block ID: 92c180b5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * EU CRA漏洞报告接口
 * 提供给安全研究人员和产品用户的漏洞报告渠道
 */

#include <microhttpd.h>

typedef struct {
    char reporter_email[256];
    char product_name[128];
    char product_version[32];
    char vulnerability_type[64];
    char description[4096];
    char steps_to_reproduce[2048];
    char impact[1024];
    char suggested_fix[1024];
    bool is_public;
    char pgp_key[2048];  /* 可选：用于加密回复 */
} vuln_report_t;

/* HTTP请求处理 */
int handle_vulnerability_report(void *cls, struct MHD_Connection *connection,
                                 const char *url, const char *method,
                                 const char *version, const char *upload_data,
                                 size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "POST") != 0) {
        return MHD_NO;
    }

    if (strcmp(url, "/api/v1/security/report") != 0) {
        return MHD_NO;
    }

    /* 解析JSON报告 */
    vuln_report_t report = {0};
    if (parse_vuln_report_json(upload_data, &report) != 0) {
        return send_error_response(connection, "Invalid report format");
    }

    /* 验证必要字段 */
    if (strlen(report.reporter_email) == 0 ||
        strlen(report.description) == 0) {
        return send_error_response(connection, "Missing required fields");
    }

    /* 创建内部漏洞案例 */
    vulnerability_case_t *case_ptr;
    receive_vulnerability_report(report.reporter_email,
                                  report.description,
                                  report.product_name,
                                  report.product_version);

    /* 发送确认邮件 */
    send_vuln_acknowledgment_email(&report, case_ptr->id);

    /* 返回成功响应 */
    const char *response = "{\"status\": \"received\", \"message\": "
                           "\"Thank you for your report. We will respond within 48 hours.\"}";

    struct MHD_Response *mhd_response =
        MHD_create_response_from_buffer(strlen(response), (void *)response,
                                        MHD_RESPMEM_PERSISTENT);

    MHD_add_response_header(mhd_response, "Content-Type", "application/json");

    int ret = MHD_queue_response(connection, MHD_HTTP_OK, mhd_response);
    MHD_destroy_response(mhd_response);

    return ret;
}

/* PGP加密邮件发送 */
int send_encrypted_vuln_response(const char *to_email,
                                  const char *subject,
                                  const char *body,
                                  const char *recipient_pgp_key) {
    /* 1. 加密邮件内容 */
    char encrypted_body[8192];

    if (recipient_pgp_key && strlen(recipient_pgp_key) > 0) {
        /* 使用PGP加密 */
        gpg_encrypt(body, recipient_pgp_key, encrypted_body, sizeof(encrypted_body));
    } else {
        /* 不加密（警告）*/
        strncpy(encrypted_body, body, sizeof(encrypted_body));
    }

    /* 2. 发送邮件 */
    send_email(to_email, subject, encrypted_body);

    return 0;
}

/* 漏洞报告状态查询 */
int handle_vuln_status_query(struct MHD_Connection *connection,
                              const char *vuln_id) {
    /* 验证查询权限 */
    const char *auth_token = MHD_lookup_connection_value(connection,
                                                         MHD_GET_ARGUMENT_KIND,
                                                         "token");
    if (!verify_query_token(auth_token, vuln_id)) {
        return send_error_response(connection, "Unauthorized", MHD_HTTP_UNAUTHORIZED);
    }

    /* 查询案例状态 */
    vulnerability_case_t *case_ptr = find_vulnerability_case(vuln_id);
    if (!case_ptr) {
        return send_error_response(connection, "Case not found", MHD_HTTP_NOT_FOUND);
    }

    /* 构建响应（不泄露敏感信息）*/
    char response[1024];
    snprintf(response, sizeof(response),
        "{"
        "\"id\": \"%s\","
        "\"status\": \"%s\","
        "\"severity\": \"%s\","
        "\"estimated_fix_date\": \"%s\""
        "}",
        case_ptr->id,
        vuln_status_to_string(case_ptr->status),
        severity_to_string(case_ptr->severity),
        time_to_date(case_ptr->patch_due));

    return send_json_response(connection, response);
}

/* 启动漏洞报告服务器 */
int start_vuln_report_server(uint16_t port) {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_AUTO | MHD_USE_INTERNAL_POLLING_THREAD,
                              port,
                              NULL, NULL,
                              &handle_vulnerability_report, NULL,
                              MHD_OPTION_NOTIFY_COMPLETED,
                              request_completed_callback, NULL,
                              MHD_OPTION_CONNECTION_TIMEOUT, 30,
                              MHD_OPTION_END);

    if (!daemon) {
        return -1;
    }

    printf("Vulnerability report server started on port %d\n", port);
    printf("Report URL: https://%s:%d/api/v1/security/report\n",
           server_hostname, port);

    return 0;
}
