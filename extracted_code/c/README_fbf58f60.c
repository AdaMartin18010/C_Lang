/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\README.md
 * Line: 114
 * Language: c
 * Block ID: fbf58f60
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* TPM 2.0 密钥层次结构详解 */

/* 主密钥模板 (Storage Primary Seed派生) */
typedef struct {
    TPMI_ALG_PUBLIC type;           /* TPM2_ALG_RSA / TPM2_ALG_ECC */
    TPMI_ALG_HASH nameAlg;          /* TPM2_ALG_SHA256 */
    TPMA_OBJECT objectAttributes;   /* 对象属性 */
    TPM2B_DIGEST authPolicy;        /* 策略哈希 (可选) */
    TPMU_PUBLIC_PARMS parameters;   /* 算法参数 */
    TPMU_PUBLIC_ID unique;          /* 唯一标识符 */
} TPM2B_PUBLIC;

/* 对象属性定义 */
#define TPMA_OBJECT_FIXEDTPM        (1 << 1)   /* 不能离开TPM */
#define TPMA_OBJECT_STCLEAR         (1 << 2)   /* 重启后清除 */
#define TPMA_OBJECT_FIXEDPARENT     (1 << 4)   /* 不能改变父密钥 */
#define TPMA_OBJECT_SENSITIVEDATAORIGIN (1 << 5)  /* 敏感数据源于TPM */
#define TPMA_OBJECT_USERWITHAUTH    (1 << 6)   /* 用户授权 */
#define TPMA_OBJECT_ADMINWITHPOLICY (1 << 7)   /* 管理员需策略 */
#define TPMA_OBJECT_NODA            (1 << 10)  /* 无字典攻击保护 */
#define TPMA_OBJECT_RESTRICTED      (1 << 16)  /* 受限密钥(仅签名已知结构) */
#define TPMA_OBJECT_DECRYPT         (1 << 17)  /* 可用于解密 */
#define TPMA_OBJECT_SIGN_ENCRYPT    (1 << 18)  /* 可用于签名/加密 */

/* 密钥创建示例：存储密钥 */
TPM_RC create_storage_key(TSS2_SYS_CONTEXT *sys_ctx) {
    TPM2B_SENSITIVE_CREATE in_sensitive = {
        .size = 0,
        .sensitive = {
            .userAuth = { .size = 0 },  /* 空授权 */
            .data = { .size = 0 }       /* 无外部敏感数据 */
        }
    };

    TPM2B_PUBLIC in_public = {
        .size = 0,
        .publicArea = {
            .type = TPM2_ALG_RSA,
            .nameAlg = TPM2_ALG_SHA256,
            .objectAttributes =
                TPMA_OBJECT_FIXEDTPM |
                TPMA_OBJECT_FIXEDPARENT |
                TPMA_OBJECT_SENSITIVEDATAORIGIN |
                TPMA_OBJECT_USERWITHAUTH |
                TPMA_OBJECT_RESTRICTED |
                TPMA_OBJECT_DECRYPT,    /* 存储密钥用于包装子密钥 */
            .authPolicy = { .size = 0 },
            .parameters = {
                .rsaDetail = {
                    .symmetric = {
                        .algorithm = TPM2_ALG_AES,
                        .keyBits = { .aes = 128 },
                        .mode = { .aes = TPM2_ALG_CFB }
                    },
                    .scheme = { .scheme = TPM2_ALG_NULL },
                    .keyBits = 2048,
                    .exponent = 0  /* 使用默认值 2^16 + 1 */
                }
            },
            .unique = { .rsa = { .size = 0 } }
        }
    };

    TPM2B_DATA outside_info = { .size = 0 };
    TPML_PCR_SELECTION creation_pcr = { .count = 0 };

    TPM2B_PRIVATE out_private = { .size = sizeof(TPM2B_PRIVATE) - 2 };
    TPM2B_PUBLIC out_public = { .size = sizeof(TPM2B_PUBLIC) - 2 };
    TPM2B_CREATION_DATA creation_data = { .size = 0 };
    TPM2B_DIGEST creation_hash = { .size = sizeof(TPM2B_DIGEST) - 2 };
    TPMT_TK_CREATION creation_ticket = { 0 };

    TPM_RC rc = Tss2_Sys_Create(sys_ctx,
                                TPM2_RH_OWNER,      /* 父句柄: Owner层次 */
                                &cmd_auths,         /* 授权 */
                                &in_sensitive,
                                &in_public,
                                &outside_info,
                                &creation_pcr,
                                &out_private,
                                &out_public,
                                &creation_data,
                                &creation_hash,
                                &creation_ticket,
                                NULL);

    return rc;
}

/* PCR策略实现 - 用于密封存储 */
TPM_RC create_pcr_policy(TSS2_SYS_CONTEXT *sys_ctx,
                         TPML_PCR_SELECTION *pcr_selection,
                         TPM2B_DIGEST *policy_digest) {
    TPM_RC rc;
    TPMI_SH_AUTH_SESSION policy_session;

    /* 1. 启动策略会话 */
    TPM2B_NONCE nonce_caller = { .size = 20 };
    TPM2B_NONCE nonce_tpm = { .size = 0 };
    TPMT_SYM_DEF symmetric = {
        .algorithm = TPM2_ALG_AES,
        .keyBits = { .aes = 128 },
        .mode = { .aes = TPM2_ALG_CFB }
    };

    rc = Tss2_Sys_StartAuthSession(sys_ctx,
                                   TPM2_RH_NULL,      /* tpmKey */
                                   TPM2_RH_NULL,      /* bind */
                                   &cmd_auths,
                                   &nonce_caller,
                                   NULL,              /* encryptedSalt */
                                   TPM2_SE_POLICY,    /* 策略会话 */
                                   &symmetric,
                                   TPM2_ALG_SHA256,
                                   &policy_session,
                                   &nonce_tpm,
                                   NULL);
    if (rc != TPM_RC_SUCCESS) return rc;

    /* 2. 应用PolicyPCR */
    TPM2B_DIGEST pcr_digest = { .size = 0 };
    rc = Tss2_Sys_PolicyPCR(sys_ctx, policy_session,
                            &cmd_auths, &pcr_digest, pcr_selection, NULL);
    if (rc != TPM_RC_SUCCESS) {
        Tss2_Sys_FlushContext(sys_ctx, policy_session);
        return rc;
    }

    /* 3. 获取策略摘要 */
    rc = Tss2_Sys_PolicyGetDigest(sys_ctx, policy_session,
                                  &cmd_auths, policy_digest, NULL);

    /* 4. 清理会话 */
    Tss2_Sys_FlushContext(sys_ctx, policy_session);
    return rc;
}
