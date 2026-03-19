/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 584
 * Language: c
 * Block ID: ce70365d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 硬件信任根生命周期管理
 */

/* RoT生命周期状态 */
typedef enum {
    ROT_STATE_MANUFACTURING = 0,    /* 制造阶段 */
    ROT_STATE_PROVISIONING,         /* 配置阶段 */
    ROT_STATE_OPERATIONAL,          /* 运行阶段 */
    ROT_STATE_MAINTENANCE,          /* 维护阶段 */
    ROT_STATE_DECOMMISSIONING,      /* 退役阶段 */
} rot_lifecycle_state_t;

/* 生命周期事件 */
typedef enum {
    ROT_EVENT_DEVICE_MANUFACTURED = 0,
    ROT_EVENT_KEYS_PROVISIONED,
    ROT_EVENT_FIRST_BOOT,
    ROT_EVENT_SECURE_BOOT_ENABLED,
    ROT_EVENT_DEBUG_DISABLED,
    ROT_EVENT_FIRMWARE_UPDATED,
    ROT_EVENT_KEY_ROTATION,
    ROT_EVENT_DECOMMISSIONED,
} rot_lifecycle_event_t;

/* 生命周期上下文 */
typedef struct {
    rot_lifecycle_state_t state;
    uint32_t boot_count;
    time_t first_boot_time;
    time_t last_boot_time;
    uint8_t device_id[16];
    bool debug_enabled;
    bool secure_boot_enabled;
    uint32_t firmware_version;
    uint32_t security_version;
} rot_lifecycle_context_t;

static rot_lifecycle_context_t g_lifecycle;

/* 生命周期状态机 */
int rot_lifecycle_transition(rot_lifecycle_event_t event) {
    switch (g_lifecycle.state) {
        case ROT_STATE_MANUFACTURING:
            if (event == ROT_EVENT_DEVICE_MANUFACTURED) {
                g_lifecycle.state = ROT_STATE_PROVISIONING;
                INFO("Transition: MANUFACTURING -> PROVISIONING\n");
            }
            break;

        case ROT_STATE_PROVISIONING:
            if (event == ROT_EVENT_KEYS_PROVISIONED) {
                g_lifecycle.state = ROT_STATE_OPERATIONAL;
                g_lifecycle.first_boot_time = time(NULL);
                INFO("Transition: PROVISIONING -> OPERATIONAL\n");
            }
            break;

        case ROT_STATE_OPERATIONAL:
            if (event == ROT_EVENT_FIRMWARE_UPDATED) {
                g_lifecycle.state = ROT_STATE_MAINTENANCE;
                INFO("Transition: OPERATIONAL -> MAINTENANCE\n");
            }
            else if (event == ROT_EVENT_DECOMMISSIONED) {
                g_lifecycle.state = ROT_STATE_DECOMMISSIONING;
                INFO("Transition: OPERATIONAL -> DECOMMISSIONING\n");
            }
            break;

        case ROT_STATE_MAINTENANCE:
            if (event == ROT_EVENT_FIRST_BOOT) {
                g_lifecycle.state = ROT_STATE_OPERATIONAL;
                INFO("Transition: MAINTENANCE -> OPERATIONAL\n");
            }
            break;

        case ROT_STATE_DECOMMISSIONING:
            /* 最终状态 */
            secure_erase_all_data();
            break;
    }

    /* 记录状态转换 */
    log_lifecycle_event(event, g_lifecycle.state);

    return 0;
}

/* 首次启动处理 */
int rot_first_boot(void) {
    /* 1. 生成设备唯一ID */
    generate_device_id(g_lifecycle.device_id);

    /* 2. 从PUF派生设备密钥 */
    derive_device_key_from_puf();

    /* 3. 禁用调试（如果配置）*/
    if (!debug_persist_enabled()) {
        disable_debug_interfaces();
        g_lifecycle.debug_enabled = false;
    }

    /* 4. 启用安全启动 */
    enable_secure_boot();
    g_lifecycle.secure_boot_enabled = true;

    /* 5. 记录首次启动 */
    g_lifecycle.first_boot_time = time(NULL);
    g_lifecycle.boot_count = 1;

    /* 6. 标记为已配置 */
    mark_device_provisioned();

    rot_lifecycle_transition(ROT_EVENT_FIRST_BOOT);

    return 0;
}

/* 后续启动处理 */
int rot_subsequent_boot(void) {
    g_lifecycle.boot_count++;
    g_lifecycle.last_boot_time = time(NULL);

    /* 1. 验证固件完整性 */
    if (verify_firmware_integrity() != 0) {
        ERROR("Firmware integrity check failed!\n");
        return -1;
    }

    /* 2. 加载安全状态 */
    load_security_state();

    /* 3. 执行安全自检 */
    if (run_security_self_test() != 0) {
        ERROR("Security self-test failed!\n");
        return -1;
    }

    return 0;
}
