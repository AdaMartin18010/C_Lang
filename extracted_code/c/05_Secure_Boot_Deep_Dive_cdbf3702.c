/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 1814
 * Language: c
 * Block ID: cdbf3702
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 设备唯一密钥（DUK）管理
 * 基于PUF或工厂注入的密钥
 */

/* PUF接口 */
typedef struct {
    int (*init)(void);
    int (*enroll)(void);
    int (*start)(void);
    int (*stop)(void);
    int (*get_key)(uint8_t *key, size_t len, const uint8_t *helper_data);
    int (*generate_helper_data)(uint8_t *helper_data, size_t len);
} puf_interface_t;

/* 设备唯一密钥结构 */
typedef struct {
    uint8_t device_id[16];          /* 设备唯一ID */
    uint8_t duk[32];                /* 设备唯一密钥 */
    uint8_t key_derivation_key[32]; /* 密钥派生密钥 */
    bool duk_provisioned;           /* 是否已配置 */
} device_unique_keys_t;

static device_unique_keys_t g_duk;

/* 从PUF派生设备密钥 */
int derive_device_key_from_puf(puf_interface_t *puf) {
    /* 1. 初始化PUF */
    if (puf->init() != 0) {
        return -1;
    }

    /* 2. 注册（首次启动） */
    if (!is_puf_enrolled()) {
        uint8_t helper_data[256];
        puf->enroll();
        puf->generate_helper_data(helper_data, sizeof(helper_data));

        /* 安全存储helper data */
        store_helper_data_securely(helper_data, sizeof(helper_data));
        mark_puf_enrolled();
    }

    /* 3. 启动PUF */
    puf->start();

    /* 4. 读取helper data */
    uint8_t helper_data[256];
    load_helper_data_securely(helper_data, sizeof(helper_data));

    /* 5. 重构设备密钥 */
    uint8_t puf_key[64];
    puf->get_key(puf_key, sizeof(puf_key), helper_data);

    /* 6. 派生DUK */
    HKDF_SHA256(puf_key, sizeof(puf_key),
                NULL, 0,
                (const uint8_t *)"device_unique_key", 16,
                g_duk.duk, sizeof(g_duk.duk));

    /* 7. 派生KDK */
    HKDF_SHA256(puf_key, sizeof(puf_key),
                NULL, 0,
                (const uint8_t *)"key_derivation_key", 18,
                g_duk.key_derivation_key, sizeof(g_duk.key_derivation_key));

    /* 8. 清理PUF密钥 */
    secure_memzero(puf_key, sizeof(puf_key));
    puf->stop();

    g_duk.duk_provisioned = true;

    INFO("Device unique key derived from PUF\n");
    return 0;
}

/* 从eFuse读取设备密钥 */
int load_device_key_from_efuse(void) {
    /* 检查eFuse是否已编程 */
    if (!is_efuse_programmed(EFUSE_DUK_BLOCK)) {
        ERROR("Device key not programmed in eFuse\n");
        return -1;
    }

    /* 读取DUK */
    if (efuse_read(EFUSE_DUK_BLOCK, g_duk.duk, sizeof(g_duk.duk)) != 0) {
        return -1;
    }

    /* 读取设备ID */
    efuse_read(EFUSE_DEVICE_ID_BLOCK, g_duk.device_id, sizeof(g_duk.device_id));

    /* 派生KDK */
    HKDF_SHA256(g_duk.duk, sizeof(g_duk.duk),
                g_duk.device_id, sizeof(g_duk.device_id),
                (const uint8_t *)"kdk", 3,
                g_duk.key_derivation_key, sizeof(g_duk.key_derivation_key));

    g_duk.duk_provisioned = true;

    INFO("Device unique key loaded from eFuse\n");
    return 0;
}

/* 派生应用密钥 */
int derive_application_key(const char *app_name,
                           const uint8_t *context,
                           size_t context_len,
                           uint8_t *key_out,
                           size_t key_len) {
    if (!g_duk.duk_provisioned) {
        return -1;
    }

    return HKDF_SHA256(g_duk.key_derivation_key, sizeof(g_duk.key_derivation_key),
                       context, context_len,
                       (const uint8_t *)app_name, strlen(app_name),
                       key_out, key_len);
}
