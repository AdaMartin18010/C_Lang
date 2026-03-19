/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\02_Key_Sealing.md
 * Line: 260
 * Language: c
 * Block ID: 9debab39
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 启动策略会话 */
int tpm_start_auth_session_policy(uint32_t *session_handle,
                                  TPM2B_DIGEST *nonce_tpm) {
    TPM_CMD_BUFFER cmd = {0};

    /* TPM2_StartAuthSession */
    tpm_build_header(&cmd, TPM_ST_NO_SESSIONS, 0, TPM_CC_StartAuthSession);

    /* tpmKey - 用于加密的密钥句柄 (0x40000007 = TPM_RH_NULL) */
    tpm_cmd_write32(&cmd, TPM_RH_NULL);

    /* bind - 绑定实体 (0x40000007 = TPM_RH_NULL) */
    tpm_cmd_write32(&cmd, TPM_RH_NULL);

    /* nonceCaller - 调用方nonce */
    uint8_t nonce_caller[32];
    get_random(nonce_caller, sizeof(nonce_caller));
    tpm_cmd_write16(&cmd, sizeof(nonce_caller));
    memcpy(cmd.buffer + cmd.offset, nonce_caller, sizeof(nonce_caller));
    cmd.offset += sizeof(nonce_caller);

    /* encryptedSalt - 空 */
    tpm_cmd_write16(&cmd, 0);

    /* sessionType - 0x01 = Policy Session */
    cmd.buffer[cmd.offset++] = 0x01;

    /* symmetric - 空算法 */
    tpm_cmd_write16(&cmd, 0x0010);  /* TPM_ALG_NULL */

    /* authHash - SHA-256 */
    tpm_cmd_write16(&cmd, 0x000B);

    cmd.size = cmd.offset;

    /* 发送并解析session handle和nonceTPM... */

    return 0;
}

/* 设置PCR策略 */
int tpm_policy_pcr(uint32_t session_handle,
                   const TPML_PCR_SELECTION *pcr_select,
                   const TPM2B_DIGEST *pcr_digest) {
    TPM_CMD_BUFFER cmd = {0};

    /* TPM2_PolicyPCR */
    tpm_build_header(&cmd, TPM_ST_NO_SESSIONS, 0, TPM_CC_PolicyPCR);

    /* policySession */
    tpm_cmd_write32(&cmd, session_handle);

    /* pcrDigest */
    tpm_cmd_write16(&cmd, pcr_digest->size);
    memcpy(cmd.buffer + cmd.offset, pcr_digest->buffer, pcr_digest->size);
    cmd.offset += pcr_digest->size;

    /* pcrs */
    tpm_cmd_write32(&cmd, pcr_select->count);
    for (uint32_t i = 0; i < pcr_select->count; i++) {
        TPMS_PCR_SELECTION *sel = &pcr_select->pcrSelections[i];
        tpm_cmd_write16(&cmd, sel->hash);
        cmd.buffer[cmd.offset++] = sel->sizeofSelect;
        memcpy(cmd.buffer + cmd.offset, sel->pcrSelect, sel->sizeofSelect);
        cmd.offset += sel->sizeofSelect;
    }

    cmd.size = cmd.offset;

    /* 发送命令... */

    return 0;
}

/* 设置密码策略 */
int tpm_policy_password(uint32_t session_handle) {
    TPM_CMD_BUFFER cmd = {0};

    /* TPM2_PolicyPassword */
    tpm_build_header(&cmd, TPM_ST_NO_SESSIONS, 0, TPM_CC_PolicyPassword);
    tpm_cmd_write32(&cmd, session_handle);
    cmd.size = cmd.offset;

    return tcti_transmit_receive(cmd.buffer, cmd.size, NULL, NULL);
}

/* 获取策略摘要 */
int tpm_policy_get_digest(uint32_t session_handle, TPM2B_DIGEST *digest) {
    TPM_CMD_BUFFER cmd = {0};

    /* TPM2_PolicyGetDigest */
    tpm_build_header(&cmd, TPM_ST_NO_SESSIONS, 0, TPM_CC_PolicyGetDigest);
    tpm_cmd_write32(&cmd, session_handle);
    cmd.size = cmd.offset;

    /* 发送并解析返回的policyDigest... */

    return 0;
}
