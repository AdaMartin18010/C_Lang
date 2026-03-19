/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 1659
 * Language: c
 * Block ID: 22fc9bbe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 设备密钥层次结构实现
 * 基于TCG和GlobalPlatform规范
 */

/* 密钥层次 */
typedef enum {
    KEY_HIERARCHY_PLATFORM = 0,     /* 平台层次 */
    KEY_HIERARCHY_STORAGE,          /* 存储层次 */
    KEY_HIERARCHY_ENDORSEMENT,      /* 背书层次 */
    KEY_HIERARCHY_NULL,             /* 空层次 */
} key_hierarchy_t;

/* 密钥类型 */
typedef enum {
    KEY_TYPE_EK = 0,                /* 背书密钥 */
    KEY_TYPE_SRK,                   /* 存储根密钥 */
    KEY_TYPE_AIK,                   /* 证明身份密钥 */
    KEY_TYPE_SIGNING,               /* 通用签名密钥 */
    KEY_TYPE_STORAGE,               /* 存储加密密钥 */
    KEY_TYPE_DEVICE,                /* 设备身份密钥 */
} key_type_t;

/* 密钥属性 */
typedef struct {
    key_type_t type;
    key_hierarchy_t hierarchy;
    uint32_t key_alg;               /* TPM_ALG_RSA/ECC */
    uint32_t key_size;              /* 密钥长度 */
    bool restricted;                /* 受限密钥 */
    bool fixed_tpm;                 /* 固定于TPM */
    bool fixed_parent;              /* 固定父密钥 */
    bool sensitive_data_origin;     /* 敏感数据来源 */
    bool user_with_auth;            /* 需要授权 */
    bool admin_with_policy;         /* 管理需要策略 */
} key_attributes_t;

/* 设备密钥层次结构 */
typedef struct {
    /* 平台层次 - 用于平台证明 */
    struct {
        TPM2_HANDLE ek_handle;              /* 背书密钥（只读） */
        TPM2_HANDLE platform_cert_handle;   /* 平台证书 */
    } platform;

    /* 存储层次 - 用于数据保护 */
    struct {
        TPM2_HANDLE srk_handle;             /* 存储根密钥 */
        TPM2_HANDLE device_key_handle;      /* 设备身份密钥 */
        TPM2_HANDLE sealing_key_handle;     /* 密封密钥 */
    } storage;

    /* 背书层次 - 用于隐私敏感操作 */
    struct {
        TPM2_HANDLE aik_handle;             /* 证明身份密钥 */
        TPM2_HANDLE ek_cert_handle;         /* EK证书 */
    } endorsement;
} device_key_hierarchy_t;

static device_key_hierarchy_t g_key_hierarchy;

/* 初始化密钥层次 */
int init_key_hierarchy(ESYS_CONTEXT *ctx) {
    TSS2_RC rc;

    /* 1. 创建EK（如果不存在） */
    TPM2B_PUBLIC ek_public = {0};
    rc = Esys_CreatePrimary(ctx, TPM2_RH_ENDORSEMENT,
                            ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE,
                            NULL, &ek_template, NULL, NULL, NULL,
                            &g_key_hierarchy.platform.ek_handle,
                            &ek_public, NULL, NULL, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        ERROR("Failed to create EK: 0x%x\n", rc);
        return -1;
    }
    INFO("EK created successfully\n");

    /* 2. 创建SRK */
    TPM2B_PUBLIC srk_public = {0};
    rc = Esys_CreatePrimary(ctx, TPM2_RH_OWNER,
                            ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE,
                            NULL, &srk_template, NULL, NULL, NULL,
                            &g_key_hierarchy.storage.srk_handle,
                            &srk_public, NULL, NULL, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        ERROR("Failed to create SRK: 0x%x\n", rc);
        return -1;
    }
    INFO("SRK created successfully\n");

    /* 3. 在SRK下创建设备身份密钥 */
    TPM2B_SENSITIVE_CREATE in_sensitive = {0};
    in_sensitive.sensitive.userAuth.size = 0;

    TPM2B_PUBLIC in_public = {
        .publicArea = {
            .type = TPM2_ALG_ECC,
            .nameAlg = TPM2_ALG_SHA256,
            .objectAttributes = TPMA_OBJECT_SIGN_ENCRYPT |
                               TPMA_OBJECT_FIXEDTPM |
                               TPMA_OBJECT_FIXEDPARENT |
                               TPMA_OBJECT_SENSITIVEDATAORIGIN |
                               TPMA_OBJECT_USERWITHAUTH,
            .parameters.eccDetail = {
                .symmetric.algorithm = TPM2_ALG_NULL,
                .scheme.scheme = TPM2_ALG_ECDSA,
                .scheme.details.ecdsa.hashAlg = TPM2_ALG_SHA256,
                .curveID = TPM2_ECC_NIST_P256,
                .kdf.scheme = TPM2_ALG_NULL,
            }
        }
    };

    TPM2B_DATA outside_info = {0};
    TPML_PCR_SELECTION creation_pcr = {0};
    TPM2B_PRIVATE *private = NULL;
    TPM2B_PUBLIC *public = NULL;
    TPM2B_CREATION_DATA *creation_data = NULL;
    TPM2B_DIGEST *creation_hash = NULL;
    TPMT_TK_CREATION *creation_ticket = NULL;

    rc = Esys_Create(ctx, g_key_hierarchy.storage.srk_handle,
                     ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE,
                     &in_sensitive, &in_public, &outside_info,
                     &creation_pcr,
                     &private, &public, &creation_data,
                     &creation_hash, &creation_ticket);

    if (rc != TSS2_RC_SUCCESS) {
        ERROR("Failed to create device key: 0x%x\n", rc);
        return -1;
    }

    /* 加载设备密钥 */
    rc = Esys_Load(ctx, g_key_hierarchy.storage.srk_handle,
                   ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE,
                   private, public,
                   &g_key_hierarchy.storage.device_key_handle);

    Esys_Free(private);
    Esys_Free(public);
    Esys_Free(creation_data);
    Esys_Free(creation_hash);
    Esys_Free(creation_ticket);

    INFO("Device identity key created and loaded\n");

    return 0;
}
