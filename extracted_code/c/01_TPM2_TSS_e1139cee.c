/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\01_TPM2_TSS.md
 * Line: 516
 * Language: c
 * Block ID: e1139cee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* TPM2_Sign - 使用TPM密钥签名 */
int tpm_sign(uint32_t key_handle, const uint8_t *auth, uint16_t auth_len,
             const uint8_t *digest, uint16_t digest_len,
             uint8_t *signature, uint16_t *sig_len) {
    TPM_CMD_BUFFER cmd = {0};

    tpm_build_header(&cmd, TPM_ST_SESSIONS, 0, TPM_CC_Sign);

    /* keyHandle */
    tpm_cmd_write32(&cmd, key_handle);

    /* 授权区域... */

    /* digest */
    tpm_cmd_write16(&cmd, digest_len);
    memcpy(cmd.buffer + cmd.offset, digest, digest_len);
    cmd.offset += digest_len;

    /* inScheme - 签名方案 */
    tpm_cmd_write16(&cmd, 0x0014);  /* TPM_ALG_RSASSA */
    tpm_cmd_write16(&cmd, 0x000B);  /* SHA-256 */

    /* validation - 用于限定符 */
    tpm_cmd_write16(&cmd, 0x00FF);  /* TPM_ALG_NULL */
    tpm_cmd_write16(&cmd, 0);  /* digest empty */

    cmd.size = cmd.offset;

    return 0;
}
