/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\02_Key_Sealing.md
 * Line: 558
 * Language: c
 * Block ID: 4f4e7a66
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 创建OR策略 - 满足任一条件即可解封 */
int tpm_policy_or(uint32_t session_handle,
                  const TPM2B_DIGEST *digests,  /* 多个候选策略摘要 */
                  uint8_t digest_count) {
    TPM_CMD_BUFFER cmd = {0};

    /* TPM2_PolicyOR */
    tpm_build_header(&cmd, TPM_ST_NO_SESSIONS, 0, TPM_CC_PolicyOR);
    tpm_cmd_write32(&cmd, session_handle);

    /* pHashList */
    tpm_cmd_write32(&cmd, digest_count);
    for (uint8_t i = 0; i < digest_count; i++) {
        tpm_cmd_write16(&cmd, digests[i].size);
        memcpy(cmd.buffer + cmd.offset, digests[i].buffer, digests[i].size);
        cmd.offset += digests[i].size;
    }

    cmd.size = cmd.offset;
    return tcti_transmit_receive(cmd.buffer, cmd.size, NULL, NULL);
}

/* 创建AND策略 - 必须满足所有条件 */
int tpm_policy_authorize(uint32_t session_handle,
                         const TPM2B_DIGEST *approved_policy,
                         const TPM2B_NAME *key_sign,
                         const TPM2B_NONCE *policy_ref,
                         const TPMT_SIGNATURE *signature) {
    TPM_CMD_BUFFER cmd = {0};

    /* TPM2_PolicyAuthorize */
    tpm_build_header(&cmd, TPM_ST_NO_SESSIONS, 0, TPM_CC_PolicyAuthorize);
    tpm_cmd_write32(&cmd, session_handle);

    /* approvedPolicy */
    tpm_cmd_write16(&cmd, approved_policy->size);
    memcpy(cmd.buffer + cmd.offset, approved_policy->buffer, approved_policy->size);
    cmd.offset += approved_policy->size;

    /* policyRef */
    tpm_cmd_write16(&cmd, policy_ref->size);
    memcpy(cmd.buffer + cmd.offset, policy_ref->buffer, policy_ref->size);
    cmd.offset += policy_ref->size;

    /* keySign */
    tpm_cmd_write16(&cmd, key_sign->size);
    memcpy(cmd.buffer + cmd.offset, key_sign->name, key_sign->size);
    cmd.offset += key_sign->size;

    /* checkTicket */
    /* ... 序列化签名 ... */

    cmd.size = cmd.offset;
    return tcti_transmit_receive(cmd.buffer, cmd.size, NULL, NULL);
}
