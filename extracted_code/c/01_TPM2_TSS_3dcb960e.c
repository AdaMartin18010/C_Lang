/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\01_TPM2_TSS.md
 * Line: 476
 * Language: c
 * Block ID: 3dcb960e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* TPM2_HMAC - 使用TPM密钥计算HMAC */
int tpm_hmac(uint32_t handle, const uint8_t *auth, uint16_t auth_len,
             uint16_t hash_alg,
             const uint8_t *buffer, uint16_t buffer_len,
             uint8_t *hmac_out, uint16_t *hmac_len) {
    TPM_CMD_BUFFER cmd = {0};

    tpm_build_header(&cmd, TPM_ST_SESSIONS, 0, TPM_CC_HMAC);

    /* handle */
    tpm_cmd_write32(&cmd, handle);

    /* 授权区域 */
    tpm_cmd_write32(&cmd, 1);
    tpm_cmd_write32(&cmd, TPM_RS_PW);
    tpm_cmd_write16(&cmd, 0);
    cmd.buffer[cmd.offset++] = 0x00;
    tpm_cmd_write16(&cmd, auth_len);
    memcpy(cmd.buffer + cmd.offset, auth, auth_len);
    cmd.offset += auth_len;

    /* buffer */
    tpm_cmd_write16(&cmd, buffer_len);
    memcpy(cmd.buffer + cmd.offset, buffer, buffer_len);
    cmd.offset += buffer_len;

    /* hashAlg */
    tpm_cmd_write16(&cmd, hash_alg);

    cmd.size = cmd.offset;

    /* 发送并解析返回的HMAC值... */

    return 0;
}
