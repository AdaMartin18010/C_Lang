/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\07_EU_CRA_Compliance.md
 * Line: 2518
 * Language: c
 * Block ID: 71b4b53f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * EU CRA合规自动更新机制
 */

typedef enum {
    UPDATE_IDLE = 0,
    UPDATE_CHECKING,
    UPDATE_DOWNLOADING,
    UPDATE_VERIFYING,
    UPDATE_INSTALLING,
    UPDATE_REBOOTING,
    UPDATE_COMPLETED,
    UPDATE_FAILED,
} update_state_t;

typedef struct {
    update_state_t state;
    update_policy_t policy;
    char current_version[32];
    char server_url[256];
    uint32_t check_interval_hours;
    timer_t check_timer;

    /* 更新信息 */
    update_info_t available_update;
    bool update_pending;

    /* 回调函数 */
    void (*on_update_available)(const update_info_t *);
    void (*on_download_progress)(uint32_t percent);
    void (*on_install_complete)(bool success);
} auto_updater_t;

/* 初始化自动更新器 */
int auto_updater_init(auto_updater_t *updater,
                      const update_policy_t *policy,
                      const char *server_url) {
    memset(updater, 0, sizeof(*updater));

    updater->policy = *policy;
    strncpy(updater->server_url, server_url, sizeof(updater->server_url));
    updater->check_interval_hours = policy->update_check_interval_days * 24;
    updater->state = UPDATE_IDLE;

    /* 启动定期检查定时器 */
    struct itimerspec its = {
        .it_value.tv_sec = updater->check_interval_hours * 3600,
        .it_interval.tv_sec = updater->check_interval_hours * 3600,
    };
    timer_create(CLOCK_REALTIME, NULL, &updater->check_timer);
    timer_settime(updater->check_timer, 0, &its, NULL);

    return 0;
}

/* 检查更新 */
int auto_updater_check(auto_updater_t *updater) {
    updater->state = UPDATE_CHECKING;

    /* 1. 查询更新服务器 */
    char url[512];
    snprintf(url, sizeof(url), "%s/api/updates/check", updater->server_url);

    char request[1024];
    snprintf(request, sizeof(request),
        "{\n"
        "  \"product\": \"%s\",\n"
        "  \"version\": \"%s\",\n"
        "  \"hardware\": \"%s\",\n"
        "  \"channel\": \"%s\"\n"
        "}\n",
        product_id,
        updater->current_version,
        hardware_version,
        update_channel);

    char response[4096];
    if (https_post(url, request, response, sizeof(response)) != 0) {
        updater->state = UPDATE_FAILED;
        return -1;
    }

    /* 2. 解析响应 */
    if (parse_update_info(response, &updater->available_update) == 0) {
        updater->update_pending = true;

        /* 3. 通知用户 */
        if (updater->on_update_available) {
            updater->on_update_available(&updater->available_update);
        }

        /* 4. 自动下载（如果是安全更新且允许自动）*/
        if (updater->available_update.is_security_update &&
            updater->policy.automatic_updates_enabled) {
            auto_updater_download(updater);
        }
    }

    updater->state = UPDATE_IDLE;
    return 0;
}

/* 下载更新 */
int auto_updater_download(auto_updater_t *updater) {
    updater->state = UPDATE_DOWNLOADING;

    char url[512];
    snprintf(url, sizeof(url), "%s/api/updates/download/%s",
             updater->server_url, updater->available_update.version);

    /* 分配下载缓冲区 */
    size_t max_size = 10 * 1024 * 1024;  /* 10MB max */
    uint8_t *firmware = malloc(max_size);
    if (!firmware) return -ENOMEM;

    /* 下载 */
    size_t downloaded = 0;
    https_download_with_progress(url, firmware, max_size, &downloaded,
                                 updater->on_download_progress);

    /* 保存到临时文件 */
    save_to_temp_file("/tmp/update.bin", firmware, downloaded);
    free(firmware);

    updater->state = UPDATE_VERIFYING;

    /* 验证 */
    if (verify_update_file("/tmp/update.bin", &updater->available_update) != 0) {
        updater->state = UPDATE_FAILED;
        return -EINVALID_UPDATE;
    }

    /* 安装 */
    if (updater->policy.require_user_consent) {
        /* 等待用户确认 */
        updater->state = UPDATE_IDLE;
        return 0;  /* 等待用户 */
    }

    return auto_updater_install(updater);
}

/* 安装更新 */
int auto_updater_install(auto_updater_t *updater) {
    updater->state = UPDATE_INSTALLING;

    /* 1. 备份当前固件 */
    backup_firmware();

    /* 2. 安装新固件 */
    if (install_firmware("/tmp/update.bin") != 0) {
        /* 回滚 */
        restore_firmware_backup();
        updater->state = UPDATE_FAILED;

        if (updater->on_install_complete) {
            updater->on_install_complete(false);
        }
        return -EINSTALL_FAILED;
    }

    /* 3. 更新版本号 */
    strncpy(updater->current_version, updater->available_update.version, 32);
    save_version_to_config(updater->current_version);

    /* 4. 记录更新 */
    log_update_install(&updater->available_update);

    updater->state = UPDATE_COMPLETED;
    updater->update_pending = false;

    if (updater->on_install_complete) {
        updater->on_install_complete(true);
    }

    /* 5. 重启（如果需要）*/
    if (updater->available_update.is_mandatory) {
        updater->state = UPDATE_REBOOTING;
        schedule_reboot(60);  /* 60秒后重启 */
    }

    return 0;
}
