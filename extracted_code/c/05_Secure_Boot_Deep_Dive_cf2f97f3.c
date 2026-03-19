/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 2874
 * Language: c
 * Block ID: cf2f97f3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * PSA Certified 安全模型实现
 * Platform Security Architecture
 */

/* PSA安全生命周期状态 */
typedef enum {
    PSA_LIFECYCLE_UNKNOWN = 0x0000,
    PSA_LIFECYCLE_ASSEMBLY_AND_TEST = 0x1000,
    PSA_LIFECYCLE_PSA_ROT_PROVISIONING = 0x2000,
    PSA_LIFECYCLE_SECURED = 0x3000,
    PSA_LIFECYCLE_NON_PSA_ROT_DEBUG = 0x4000,
    PSA_LIFECYCLE_RECOVERABLE_PSA_ROT_DEBUG = 0x5000,
    PSA_LIFECYCLE_DECOMMISSIONED = 0x6000,
} psa_lifecycle_t;

/* PSA RoT（Root of Trust） */
typedef struct {
    psa_lifecycle_t lifecycle;
    uint8_t implementation_id[32];
    uint8_t instance_id[32];
    uint8_t boot_seed[32];
    uint64_t security_lifecycle_counter;
} psa_rot_data_t;

/* PSA认证 */
typedef struct {
    uint8_t challenge[32];
    uint8_t token[256];
    uint32_t token_size;
} psa_attestation_token_t;

/* 初始化PSA RoT */
int psa_rot_init(void) {
    psa_rot_data_t rot = {0};

    /* 读取生命周期状态 */
    rot.lifecycle = get_psa_lifecycle();

    /* 读取实现ID */
    get_implementation_id(rot.implementation_id);

    /* 生成实例ID（基于设备唯一性） */
    derive_instance_id(rot.instance_id);

    /* 生成启动种子 */
    get_random(rot.boot_seed, sizeof(rot.boot_seed));

    /* 安全存储RoT数据 */
    secure_store_rot_data(&rot);

    return 0;
}

/* PSA认证令牌生成 */
int psa_generate_attestation_token(const uint8_t *challenge,
                                   size_t challenge_size,
                                   psa_attestation_token_t *token) {
    /* 1. 收集证据 */
    psa_evidence_t evidence = {0};

    /* 读取生命周期 */
    evidence.lifecycle = get_psa_lifecycle();

    /* 读取实现ID */
    get_implementation_id(evidence.implementation_id);

    /* 读取实例ID */
    derive_instance_id(evidence.instance_id);

    /* 收集启动测量 */
    for (int i = 0; i < 10; i++) {
        size_t len = 32;
        read_pcr(i, evidence.sw_components[i].measurement, &len);
    }

    /* 2. 复制挑战 */
    memcpy(token->challenge, challenge, challenge_size);

    /* 3. 签名证据 */
    sign_with_iak((uint8_t *)&evidence, sizeof(evidence),
                  token->token, &token->token_size);

    return 0;
}

/* PSA RoT API */
psa_status_t psa_initial_attest_get_token(const uint8_t *auth_challenge,
                                          size_t challenge_size,
                                          uint8_t *token_buf,
                                          size_t token_buf_size,
                                          size_t *token_size) {
    /* 验证参数 */
    if (challenge_size < 32 || challenge_size > 64) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* 生成令牌 */
    psa_attestation_token_t token;
    int ret = psa_generate_attestation_token(auth_challenge, challenge_size,
                                             &token);
    if (ret != 0) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    /* 复制到输出缓冲区 */
    if (token.token_size > token_buf_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    memcpy(token_buf, token.token, token.token_size);
    *token_size = token.token_size;

    return PSA_SUCCESS;
}
