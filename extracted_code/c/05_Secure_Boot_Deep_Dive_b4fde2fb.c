/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 1547
 * Language: c
 * Block ID: b4fde2fb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 远程证明实现基础
 * 向远程验证者证明平台完整性
 */

/* 引证数据结构 */
typedef struct {
    uint8_t quote[128];             /* TPM Quote */
    uint32_t quote_size;
    uint8_t pcr_values[24][32];     /* PCR 0-23 */
    uint8_t signature[256];         /* AIK签名 */
    uint8_t event_log[4096];        /* 事件日志 */
    uint32_t event_log_size;
} attestation_evidence_t;

/* 生成引证 */
int generate_attestation_quote(const uint8_t *nonce, uint32_t nonce_len,
                               attestation_evidence_t *evidence) {
    TSS2_RC rc;

    /* 1. 选择要引证的PCR */
    TPML_PCR_SELECTION pcr_selection = {
        .count = 1,
        .pcrSelections[0] = {
            .hash = TPM2_ALG_SHA256,
            .sizeofSelect = 3,
            .pcrSelect = {0xFF, 0xFF, 0xFF}  /* PCR 0-23 */
        }
    };

    /* 2. 构造引证数据 */
    TPM2B_DATA qualifying_data = {.size = nonce_len};
    memcpy(qualifying_data.buffer, nonce, nonce_len);

    /* 3. 加载AIK（证明身份密钥） */
    ESYS_TR aik_handle = load_aik_key();

    /* 4. 执行Quote命令 */
    TPM2B_ATTEST *quoted = NULL;
    TPMT_SIGNATURE *signature = NULL;

    rc = Esys_Quote(g_tpm.ctx, aik_handle,
                    ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE,
                    &qualifying_data,
                    &pcr_selection,
                    &quoted,
                    &signature);

    if (rc != TSS2_RC_SUCCESS) {
        ERROR("TPM Quote failed: 0x%x\n", rc);
        return -1;
    }

    /* 5. 复制引证数据 */
    evidence->quote_size = quoted->size;
    memcpy(evidence->quote, quoted->attestationData, quoted->size);

    /* 6. 复制签名 */
    memcpy(evidence->signature,
           signature->signature.rsapss.sig.buffer,
           signature->signature.rsapss.sig.size);

    /* 7. 读取PCR值 */
    for (int i = 0; i < 24; i++) {
        size_t len = 32;
        read_pcr(i, evidence->pcr_values[i], &len);
    }

    /* 8. 复制事件日志 */
    evidence->event_log_size = get_event_log_size();
    memcpy(evidence->event_log, get_event_log(), evidence->event_log_size);

    Esys_Free(quoted);
    Esys_Free(signature);

    return 0;
}

/* 发送证明到验证服务器 */
int send_attestation_to_verifier(const char *server_url,
                                 const attestation_evidence_t *evidence) {
    /* 构造HTTP POST请求 */
    char json_body[8192];
    snprintf(json_body, sizeof(json_body),
        "{"
        "\"quote\":\"%s\","
        "\"pcr_values\":[",
        base64_encode(evidence->quote, evidence->quote_size));

    /* 添加PCR值... */

    /* 发送HTTPS请求 */
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, server_url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

    CURLcode res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);

    return (res == CURLE_OK) ? 0 : -1;
}
