/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\07_EU_CRA_Compliance.md
 * Line: 730
 * Language: c
 * Block ID: 1d30e8ce
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * EU CRA安全更新要求实现
 */

/* 更新信息结构 */
typedef struct {
    char version[32];
    char release_notes[4096];
    char security_fixes[2048];
    char checksum[64];
    char signature[256];
    time_t release_date;
    time_t support_end_date;
    bool is_security_update;
    bool is_mandatory;
} update_info_t;

/* 更新政策 */
typedef struct {
    uint32_t support_years;             /* 支持年限 */
    bool automatic_updates_enabled;     /* 自动更新 */
    uint32_t update_check_interval_days; /* 检查间隔 */
    bool require_user_consent;          /* 需要用户同意 */
    bool rollback_allowed;              /* 允许回滚 */
    uint32_t rollback_window_days;      /* 回滚窗口 */
} update_policy_t;

/* 更新管理器 */
typedef struct {
    update_policy_t policy;
    char current_version[32];
    time_t install_date;
    char update_server[256];
    bool update_available;
    update_info_t pending_update;
} update_manager_t;

/* 检查更新 */
int check_for_updates(update_manager_t *mgr) {
    /* 1. 连接更新服务器 */
    https_connection_t *conn = https_connect(mgr->update_server);

    /* 2. 发送当前版本信息 */
    char request[512];
    snprintf(request, sizeof(request),
        "GET /api/updates?product=%s&version=%s&hw=%s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "User-Agent: %s/%s\r\n"
        "Connection: close\r\n\r\n",
        product_id,
        mgr->current_version,
        hardware_version,
        mgr->update_server,
        product_name,
        mgr->current_version
    );

    https_send(conn, request, strlen(request));

    /* 3. 接收响应 */
    char response[4096];
    https_receive(conn, response, sizeof(response));

    /* 4. 解析更新信息 */
    if (parse_update_response(response, &mgr->pending_update) == 0) {
        mgr->update_available = true;

        /* 5. 如果是安全更新，通知用户 */
        if (mgr->pending_update.is_security_update) {
            notify_user_security_update(&mgr->pending_update);
        }
    }

    https_close(conn);
    return 0;
}

/* 下载并安装更新 */
int download_and_install_update(update_manager_t *mgr) {
    update_info_t *update = &mgr->pending_update;

    /* 1. 检查用户同意（如果需要） */
    if (mgr->policy.require_user_consent && !user_consented()) {
        return -EUSER_DECLINED;
    }

    /* 2. 检查磁盘空间 */
    if (get_free_space() < MIN_UPDATE_SPACE) {
        return -ENOSPC;
    }

    /* 3. 下载更新 */
    char download_url[512];
    snprintf(download_url, sizeof(download_url),
        "https://%s/updates/%s/firmware.bin",
        mgr->update_server, update->version);

    uint8_t *firmware_data;
    size_t firmware_size;

    if (https_download(download_url, &firmware_data, &firmware_size) != 0) {
        return -EDOWNLOAD_FAILED;
    }

    /* 4. 验证校验和 */
    char computed_checksum[64];
    sha256_hex(firmware_data, firmware_size, computed_checksum);

    if (strcmp(computed_checksum, update->checksum) != 0) {
        free(firmware_data);
        return -ECHECKSUM_MISMATCH;
    }

    /* 5. 验证签名 */
    if (verify_firmware_signature(firmware_data, firmware_size,
                                   update->signature) != 0) {
        free(firmware_data);
        return -ESIGNATURE_INVALID;
    }

    /* 6. 备份当前固件（用于回滚） */
    backup_current_firmware();

    /* 7. 安装更新 */
    if (install_firmware(firmware_data, firmware_size) != 0) {
        /* 安装失败，回滚 */
        rollback_firmware();
        free(firmware_data);
        return -EINSTALL_FAILED;
    }

    /* 8. 清理 */
    free(firmware_data);

    /* 9. 更新版本信息 */
    strncpy(mgr->current_version, update->version,
            sizeof(mgr->current_version));
    mgr->install_date = time(NULL);
    mgr->update_available = false;

    /* 10. 记录更新日志 */
    log_update_install(update->version, update->is_security_update);

    return 0;
}

/* 支持期结束通知 */
int notify_support_period_ending(const update_manager_t *mgr) {
    time_t now = time(NULL);
    time_t support_end = mgr->install_date +
                         (mgr->policy.support_years * 365 * 24 * 3600);

    double days_remaining = difftime(support_end, now) / (24 * 3600);

    /* CRA要求：至少提前6个月通知 */
    if (days_remaining <= 180 && days_remaining > 179) {
        /* 发送通知 */
        char message[512];
        snprintf(message, sizeof(message),
            "重要通知：产品 %s 的安全支持将于 %s 结束。"
            "此后将不再提供安全更新。请联系供应商了解延长支持选项。",
            product_name,
            time_to_date(support_end));

        notify_user(message, NOTIFICATION_IMPORTANT);

        /* 也发送邮件通知 */
        send_support_ending_email(mgr, support_end);
    }

    return 0;
}
