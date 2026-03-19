/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\README.md
 * Line: 255
 * Language: c
 * Block ID: e072b995
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 远程证明流程实现 */

typedef struct {
    TPM2B_PUBLIC ek_pub;            /* 背书密钥公钥 */
    TPM2B_PUBLIC ak_pub;            /* 证明密钥公钥 */
    TPM2B_DIGEST ak_name;           /* AK名称 */
    TPM2B_DATA credential;          /* 激活凭证 */
    TPM2B_ENCRYPTED_SECRET secret;  /* 加密密钥 */
} attestation_evidence_t;

/* 步骤1: 创建证明密钥(AK) */
TPM_RC create_attestation_key(TSS2_SYS_CONTEXT *sys_ctx,
                              TPMI_DH_OBJECT ek_handle,
                              TPMI_DH_OBJECT *ak_handle,
                              attestation_evidence_t *evidence) {
    /* AK模板: 受限签名密钥 */
    TPM2B_PUBLIC ak_template = {
        .publicArea = {
            .type = TPM2_ALG_RSA,
            .nameAlg = TPM2_ALG_SHA256,
            .objectAttributes =
                TPMA_OBJECT_FIXEDTPM |
                TPMA_OBJECT_FIXEDPARENT |
                TPMA_OBJECT_SENSITIVEDATAORIGIN |
                TPMA_OBJECT_USERWITHAUTH |
                TPMA_OBJECT_RESTRICTED |
                TPMA_OBJECT_SIGN_ENCRYPT |
                TPMA_OBJECT_NODA,      /* 无字典攻击保护 */
            .parameters = {
                .rsaDetail = {
                    .symmetric = { .algorithm = TPM2_ALG_NULL },
                    .scheme = {
                        .scheme = TPM2_ALG_RSASSA,
                        .details = { .rsassa = { .hashAlg = TPM2_ALG_SHA256 } }
                    },
                    .keyBits = 2048,
                    .exponent = 0
                }
            }
        }
    };

    TPM2B_PRIVATE ak_private = {0};
    TPM2B_PUBLIC ak_public = {0};
    TPM2B_CREATION_DATA creation_data = {0};
    TPM2B_DIGEST creation_hash = {0};
    TPMT_TK_CREATION creation_ticket = {0};

    /* 创建AK */
    TPM_RC rc = Tss2_Sys_Create(sys_ctx, ek_handle, &cmd_auths,
                                &in_sensitive, &ak_template,
                                NULL, NULL,
                                &ak_private, &ak_public,
                                &creation_data, &creation_hash,
                                &creation_ticket, NULL);
    if (rc != TPM_RC_SUCCESS) return rc;

    /* 加载AK */
    rc = Tss2_Sys_Load(sys_ctx, ek_handle, &cmd_auths,
                       &ak_private, &ak_public, ak_handle, NULL, NULL);

    evidence->ak_pub = ak_public;
    return rc;
}

/* 步骤2: 生成PCR引用(Quote) */
TPM_RC generate_pcr_quote(TSS2_SYS_CONTEXT *sys_ctx,
                          TPMI_DH_OBJECT ak_handle,
                          TPML_PCR_SELECTION *pcr_selection,
                          TPM2B_DATA *extra_data,  /* 随机nonce */
                          TPM2B_ATTEST *quoted,    /* 证明数据 */
                          TPMT_SIGNATURE *signature) {
    TPM_RC rc;
    TPMI_SH_AUTH_SESSION session = TPM2_RS_PW;

    /* 使用AK对PCR状态签名 */
    rc = Tss2_Sys_Quote(sys_ctx, ak_handle, &cmd_auths,
                        extra_data,         /* 防重放随机数 */
                        pcr_selection,
                        NULL,               /* 非对称签名方案 */
                        quoted,
                        signature,
                        NULL);
    return rc;
}

/* 证明数据结构解析 */
typedef struct {
    TPM_GENERATED magic;            /* TPM_GENERATED_VALUE (0xFF544347) */
    TPMI_ST_ATTEST type;            /* TPM_ST_ATTEST_QUOTE */
    TPM2B_NAME qualified_signer;    /* 签名者名称 */
    TPM2B_DATA extra_data;          /* 调用者提供的额外数据 */
    TPMS_CLOCK_INFO clock_info;     /* 时钟信息 */
    UINT64 firmware_version;        /* 固件版本 */
    TPMS_ATTEST quote;              /* PCR引用特定数据 */
} TPMS_ATTEST_QUOTE;

typedef struct {
    TPM2B_DIGEST pcr_digest;        /* 选定PCR的哈希 */
    TPML_PCR_SELECTION pcr_select;  /* PCR选择 */
} TPMS_QUOTE_INFO;
